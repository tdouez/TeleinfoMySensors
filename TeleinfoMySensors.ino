/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2018 Sensnology AB
 * Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
*/
//--------------------------------------------------------------------
//                 +/          
//                 `hh-        
//        ::        /mm:       
//         hy`      -mmd       
//         omo      +mmm.  -+` 
//         hmy     .dmmm`   od-
//        smmo    .hmmmy    /mh
//      `smmd`   .dmmmd.    ymm
//     `ymmd-   -dmmmm/    omms
//     ymmd.   :mmmmm/    ommd.
//    +mmd.   -mmmmm/    ymmd- 
//    hmm:   `dmmmm/    smmd-  
//    dmh    +mmmm+    :mmd-   
//    omh    hmmms     smm+    
//     sm.   dmmm.     smm`    
//      /+   ymmd      :mm     
//           -mmm       +m:    
//            +mm:       -o    
//             :dy             
//              `+:     
//--------------------------------------------------------------------
//   __|              _/           _ )  |                       
//   _| |  |   ` \    -_)   -_)    _ \  |   -_)  |  |   -_)     
//  _| \_,_| _|_|_| \___| \___|   ___/ _| \___| \_,_| \___|  
//--------------------------------------------------------------------    
// 2019/10/17 - FB V1.0.1
// 2019/12/15 - FB V1.0.2 - optimisation/simplifications et reduction conso mémoire
// 2019/12/17 - FB V1.0.3 - modif led indication envoi MySensors
// 2019/12/22 - FB V1.0.4 - remove warning message
// 2019/12/22 - FB V1.0.5 - optimisation
// 2020/04/25 - FB V1.0.6 - ADCO bug fix
// 2020/04/25 - FB V1.1.0 - Mode standard
//--------------------------------------------------------------------
// Enable debug prints
//#define MY_DEBUG


//#define MY_NODE_ID 2

#define VERSION   "v1.1.0"

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
//#define MY_RF24_PA_LEVEL RF24_PA_LOW

#define MY_BAUD_RATE 9600    // mode standard

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

#define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
#define MY_DEFAULT_RX_LED_PIN  13 // Receive led pin, on board LED
#define MY_DEFAULT_TX_LED_PIN  3  // 

#include <MySensors.h>

uint32_t SEND_FREQUENCY_CONSO =    5000; // Minimum time between send (in milliseconds). We don't want to spam the gateway.
uint32_t SEND_FREQUENCY_INFO =    60000; // Minimum time between send (in milliseconds). We don't want to spam the gateway.
uint32_t lastSend_conso = 0;
uint32_t lastSend_info = 0;

// Variables Téléinfo---------------------
struct teleinfo_s {
  char _ADSC[13]="";
  char VTIC[3]="";
  char NGTF[17]="";
  char LTARF[16]="";
  unsigned long EAST=0;
  unsigned long EAIT=0;
  unsigned int IRMS1=0;
  unsigned int IRMS2=0;
  unsigned int IRMS3=0;
  unsigned int URMS1=0;
  unsigned int URMS2=0;
  unsigned int URMS3=0;
  unsigned int PREF=0;
  unsigned int PCOUP=0;
  unsigned int SINSTS=0;
  unsigned int SINSTS1=0;
  unsigned int SINSTS2=0;
  unsigned int SINSTS3=0;
  unsigned int SINSTI=0;
  char STGE[9]="";
  char MSG1[32]="";
  char NTAF[2]="";
  char NJOURF[2]="";
  char NJOURF1[2]="";
};
teleinfo_s teleinfo;


#define CHILD_ID_ADSC     0
#define CHILD_ID_VTIC     1
#define CHILD_ID_NGTF     2
#define CHILD_ID_LTARF    3
#define CHILD_ID_EAST     4
#define CHILD_ID_IRMS1    5
#define CHILD_ID_IRMS2    6
#define CHILD_ID_IRMS3    7
#define CHILD_ID_URMS1    8
#define CHILD_ID_URMS2    9
#define CHILD_ID_URMS3    10
#define CHILD_ID_PREF     11
#define CHILD_ID_PCOUP    12
#define CHILD_ID_SINSTS   13
#define CHILD_ID_SINSTS1  14
#define CHILD_ID_SINSTS2  15
#define CHILD_ID_SINSTS3  16
#define CHILD_ID_STGE     17
#define CHILD_ID_MSG1     18
#define CHILD_ID_NTAF     19
#define CHILD_ID_NJOURF   20
#define CHILD_ID_NJOURF1  21
#define CHILD_ID_EAIT     22
#define CHILD_ID_SINSTI   23


MyMessage msgTEXT( 0, V_TEXT);        // S_INFO
MyMessage msgCURRENT( 0, V_CURRENT ); // S_MULTIMETER
MyMessage msgVOLTAGE( 0, V_VOLTAGE ); // S_MULTIMETER
MyMessage msgWATT( 0, V_WATT ); // S_POWER
MyMessage msgKWH( 0, V_KWH );   // S_POWER
MyMessage msgVA( 0, V_VA );     // S_POWER

//--------------------------------------------------------------------
void setup()
{
  
  Serial.begin(MY_BAUD_RATE);
 
  Serial.println(F("   __|              _/           _ )  |"));
  Serial.println(F("   _| |  |   ` \\    -_)   -_)    _ \\  |   -_)  |  |   -_)"));
  Serial.println(F("  _| \\_,_| _|_|_| \\___| \\___|   ___/ _| \\___| \\_,_| \\___|"));
  Serial.print(F("                                             "));
  Serial.println(VERSION);

  memset(&teleinfo, 0, sizeof(teleinfo));
}

//--------------------------------------------------------------------
void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Teleinfo", VERSION);

  // Register this device as power sensor
  present( CHILD_ID_ADSC, S_INFO, "Adresse Compteur");
  present( CHILD_ID_VTIC, S_INFO, "Version TIC" );
  present( CHILD_ID_NGTF, S_INFO, "Nom du calendrier tarifaire" );
  present( CHILD_ID_LTARF, S_INFO, "Libellé tarif" );
  present( CHILD_ID_EAST, S_POWER, "Energie active soutirée totale" );
  present( CHILD_ID_EAIT, S_POWER, "Energie active injectée" );
  present( CHILD_ID_IRMS1, S_MULTIMETER, "Courant efficace, phase 1" );
  present( CHILD_ID_IRMS2, S_MULTIMETER, "Courant efficace, phase 2" );
  present( CHILD_ID_IRMS3, S_MULTIMETER, "Courant efficace, phase 3" );
  present( CHILD_ID_URMS1, S_MULTIMETER, "Tension efficace, phase 1" );
  present( CHILD_ID_URMS2, S_MULTIMETER, "Tension efficace, phase 2" );
  present( CHILD_ID_URMS3, S_MULTIMETER, "Tension efficace, phase 3" );
  present( CHILD_ID_PREF, S_POWER, "Puissance apparente ref" );
  present( CHILD_ID_PCOUP, S_POWER, "Puissance coupure" );
  present( CHILD_ID_SINSTS, S_POWER, "Puissance apparente" );
  present( CHILD_ID_SINSTS1, S_POWER, "Puissance apparente phase 1" );
  present( CHILD_ID_SINSTS2, S_POWER, "Puissance apparente phase 2" );
  present( CHILD_ID_SINSTS3, S_POWER, "Puissance apparente phase 3" );
  present( CHILD_ID_SINSTI, S_POWER, "Puissance apparente injectée" );
  present( CHILD_ID_STGE, S_INFO, "Registre de Statuts" );
  present( CHILD_ID_MSG1, S_INFO, "Message" );
  present( CHILD_ID_NTAF, S_INFO, "N° index tarifaire en cours" );
  present( CHILD_ID_NJOURF, S_INFO, "N° jour en cours" );
  present( CHILD_ID_NJOURF1, S_INFO, "N° prochain jour"  );
}

//--------------------------------------------------------------------
void send_teleinfo_conso()
{
boolean flag_hhphc = false;

  // EAST
  send(msgKWH.setSensor(CHILD_ID_EAST).set(teleinfo.EAST));
  // EAIT
  send(msgKWH.setSensor(CHILD_ID_EAIT).set(teleinfo.EAIT));
  // IRMS1
  send(msgCURRENT.setSensor(CHILD_ID_IRMS1).set(teleinfo.IRMS1));
  // IRMS2
  send(msgCURRENT.setSensor(CHILD_ID_IRMS2).set(teleinfo.IRMS2));
  // IRMS3
  send(msgCURRENT.setSensor(CHILD_ID_IRMS3).set(teleinfo.IRMS3));
  // URMS1
  send(msgVOLTAGE.setSensor(CHILD_ID_URMS1).set(teleinfo.URMS1));
  // URMS2
  send(msgVOLTAGE.setSensor(CHILD_ID_URMS2).set(teleinfo.URMS2));
  // URMS3
  send(msgVOLTAGE.setSensor(CHILD_ID_URMS3).set(teleinfo.URMS3));
  // PREF
  send(msgVA.setSensor(CHILD_ID_PREF).set(teleinfo.PREF));
  // SINSTS
  send(msgVA.setSensor(CHILD_ID_SINSTS).set(teleinfo.SINSTS));
  // SINSTS1
  send(msgVA.setSensor(CHILD_ID_SINSTS1).set(teleinfo.SINSTS1));
  // SINSTS2
  send(msgVA.setSensor(CHILD_ID_SINSTS2).set(teleinfo.SINSTS2));
  // SINSTS3
  send(msgVA.setSensor(CHILD_ID_SINSTS3).set(teleinfo.SINSTS3));
  // SINSTI
  send(msgVA.setSensor(CHILD_ID_SINSTI).set(teleinfo.SINSTI));
  
}

//--------------------------------------------------------------------
void send_teleinfo_info()
{
boolean flag_hhphc = false;

  // ADSC
  send(msgTEXT.setSensor(CHILD_ID_ADSC).set(teleinfo._ADSC));
  // VTIC
  send(msgTEXT.setSensor(CHILD_ID_VTIC).set(teleinfo.VTIC));
  // NGTF
  send(msgTEXT.setSensor(CHILD_ID_NGTF).set(teleinfo.NGTF));
  // LTARF
  send(msgTEXT.setSensor(CHILD_ID_LTARF).set(teleinfo.LTARF));
  // PCOUP
  send(msgVA.setSensor(CHILD_ID_PCOUP).set(teleinfo.PCOUP));
  // STGE
  send(msgTEXT.setSensor(CHILD_ID_STGE).set(teleinfo.STGE));
  // MSG1
  send(msgTEXT.setSensor(CHILD_ID_MSG1).set(teleinfo.MSG1));
  // NTAF
  send(msgTEXT.setSensor(CHILD_ID_NTAF).set(teleinfo.NTAF));
  // NJOURF
  send(msgTEXT.setSensor(CHILD_ID_NJOURF).set(teleinfo.NJOURF));
  // NJOURF1
  send(msgTEXT.setSensor(CHILD_ID_NJOURF1).set(teleinfo.NJOURF1));
}


//--------------------------------------------------------------------
void loop()
{
  uint32_t currentTime = millis();

  // lecture teleinfo -------------------------
  read_teleinfo();
 
  // Envoi les infos de consommation ---------- 
  if (currentTime - lastSend_conso > SEND_FREQUENCY_CONSO) {
    send_teleinfo_conso();
    lastSend_conso = currentTime;
  } 

   // Envoi les infos techniques ---------- 
  if (currentTime - lastSend_info > SEND_FREQUENCY_INFO) {
    send_teleinfo_info();
    lastSend_info = currentTime;
  } 

}
