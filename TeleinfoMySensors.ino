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

uint32_t SEND_FREQUENCY =    5000; // Minimum time between send (in milliseconds). We don't want to spam the gateway.
uint32_t lastSend = 0;

// Variables Téléinfo---------------------
struct teleinfo_s {
  char _ADSC[13]="";
  char VTIC[3]="";
  char NGTF[17]="";
  char LTARF[16]="";
  unsigned long EAST=0;
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
  char STGE[9]="";
  char MSG1[32]="";
  char NTAF[2]="";
  char NJOURF[2]="";
  char NJOURF1[2]="";
};
teleinfo_s teleinfo;


//#define LED_SEND         4  
//#define LED_TELEINFO     3 


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


MyMessage msgVAR1_ADSC( 0, V_TEXT);
MyMessage msgVAR2_OPTARIF( 0, V_TEXT );
MyMessage msgVAR3_PTEC( 0, V_TEXT );
MyMessage msgVAR4_HHPHC( 0, V_TEXT );
MyMessage msgVAR5_DEMAIN( 0, V_TEXT );
MyMessage msgCURRENT( 0, V_CURRENT );
MyMessage msgWATT( 0, V_WATT );
MyMessage msgKWH( 0, V_KWH );


//--------------------------------------------------------------------
void setup()
{
  
  Serial.begin(MY_BAUD_RATE);
 
  Serial.println(F("   __|              _/           _ )  |"));
  Serial.println(F("   _| |  |   ` \\    -_)   -_)    _ \\  |   -_)  |  |   -_)"));
  Serial.println(F("  _| \\_,_| _|_|_| \\___| \\___|   ___/ _| \\___| \\_,_| \\___|"));
  Serial.print(F("                                             "));
  Serial.println(VERSION);
}

//--------------------------------------------------------------------
void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Teleinfo", VERSION);

  // Register this device as power sensor
  present( CHILD_ID_ADSC, S_INFO, "Adresse Compteur");
  present( CHILD_ID_VTIC, S_INFO );
  present( CHILD_ID_NGTF, S_INFO );
  present( CHILD_ID_LTARF, S_INFO );
  present( CHILD_ID_EAST, S_POWER );
  present( CHILD_ID_IRMS1, S_POWER );
  present( CHILD_ID_IRMS2, S_POWER );
  present( CHILD_ID_IRMS3, S_POWER );
  present( CHILD_ID_URMS1, S_POWER );
  present( CHILD_ID_URMS2, S_POWER );
  present( CHILD_ID_URMS3, S_POWER );
  present( CHILD_ID_PREF, S_POWER );
  present( CHILD_ID_PCOUP, S_POWER );
  present( CHILD_ID_SINSTS, S_POWER );
  present( CHILD_ID_SINSTS1, S_POWER );
  present( CHILD_ID_SINSTS2, S_POWER );
  present( CHILD_ID_SINSTS3, S_POWER );
  present( CHILD_ID_STGE, S_INFO );
  present( CHILD_ID_MSG1, S_INFO );
  present( CHILD_ID_NTAF, S_INFO );
  present( CHILD_ID_NJOURF, S_INFO );
  present( CHILD_ID_NJOURF1, S_INFO );
 

  /*
  present( CHILD_ID_BASE, S_POWER );
  present( CHILD_ID_HCHC, S_POWER );
  present( CHILD_ID_HCHP, S_POWER );
  present( CHILD_ID_EJP_HN, S_POWER );
  present( CHILD_ID_EJP_HPM, S_POWER );
  present( CHILD_ID_BBR_HC_JB, S_POWER );
  present( CHILD_ID_BBR_HP_JB, S_POWER );
  present( CHILD_ID_BBR_HC_JW, S_POWER );
  present( CHILD_ID_BBR_HP_JW, S_POWER );
  present( CHILD_ID_BBR_HC_JR, S_POWER );
  present( CHILD_ID_BBR_HP_JR, S_POWER );
  present( CHILD_ID_PEJP, S_POWER );
  present( CHILD_ID_PTEC, S_INFO );
  present( CHILD_ID_DEMAIN, S_INFO );
  present( CHILD_ID_IINST, S_POWER );
  present( CHILD_ID_ADPS, S_POWER );
  present( CHILD_ID_IMAX, S_POWER );
  present( CHILD_ID_PAPP, S_POWER );
  present( CHILD_ID_HHPHC, S_INFO );
*/
}

//--------------------------------------------------------------------
void send_teleinfo()
{
boolean flag_hhphc = false;

  // ADSC
  send(msgVAR1_ADSC.setSensor(CHILD_ID_ADSC).set(teleinfo._ADSC));
/*  
  //OPTARIF
  send(msgVAR2_OPTARIF.setSensor(CHILD_ID_OPTARIF).set(teleinfo.OPTARIF));

  // ISOUC
  send(msgCURRENT.setSensor(CHILD_ID_ISOUSC).set(teleinfo.ISOUSC));

  // BASE -------
  if (strcmp(teleinfo.OPTARIF, "BASE" ) == 0) {
    send(msgKWH.setSensor(CHILD_ID_BASE).set(teleinfo.BASE));
  }

  // HC
  if (strcmp(teleinfo.OPTARIF, "HC.." ) == 0) {
      send(msgKWH.setSensor(CHILD_ID_HCHC).set(teleinfo.HCHC));
      send(msgKWH.setSensor(CHILD_ID_HCHP).set(teleinfo.HCHP));
  }

  // EJP
  if (strcmp(teleinfo.OPTARIF, "EJP." ) == 0) {
      send(msgKWH.setSensor(CHILD_ID_EJP_HN).set(teleinfo.EJP_HN));
      send(msgKWH.setSensor(CHILD_ID_EJP_HPM).set(teleinfo.EJP_HPM));
      send(msgKWH.setSensor(CHILD_ID_PEJP).set(teleinfo.PEJP));

      flag_hhphc = true;
  }
  
  // BBR - TEMPO
  if (strstr(teleinfo.OPTARIF, "BBR") != NULL) {
      send(msgKWH.setSensor(CHILD_ID_BBR_HC_JB).set(teleinfo.BBR_HC_JB));
      send(msgKWH.setSensor(CHILD_ID_BBR_HP_JB).set(teleinfo.BBR_HP_JB));
      send(msgKWH.setSensor(CHILD_ID_BBR_HC_JW).set(teleinfo.BBR_HC_JW));
      send(msgKWH.setSensor(CHILD_ID_BBR_HP_JW).set(teleinfo.BBR_HP_JW));
      send(msgKWH.setSensor(CHILD_ID_BBR_HC_JR).set(teleinfo.BBR_HC_JR));
      send(msgKWH.setSensor(CHILD_ID_BBR_HP_JR).set(teleinfo.BBR_HP_JR));
      send(msgVAR5_DEMAIN.setSensor(CHILD_ID_DEMAIN).set(teleinfo.DEMAIN));

      flag_hhphc = true;
  }
  
  // PTEC
  send(msgVAR3_PTEC.setSensor(CHILD_ID_PTEC).set(teleinfo.PTEC));

  // IINST
  send(msgCURRENT.setSensor(CHILD_ID_IINST).set(teleinfo.IINST));

  // PAPP
  send(msgWATT.setSensor(CHILD_ID_PAPP).set(teleinfo.PAPP));

  // ADPS
  send(msgCURRENT.setSensor(CHILD_ID_ADPS).set(teleinfo.ADPS));

  // IMAX
  send(msgCURRENT.setSensor(CHILD_ID_IMAX).set(teleinfo.IMAX));

  // HHPHC
  if (flag_hhphc == true) { // cas particulier, appartient a EJP et TEMPO
      send(msgVAR4_HHPHC.setSensor(CHILD_ID_HHPHC).set(teleinfo.HHPHC));
  }
  */
}

//--------------------------------------------------------------------
void loop()
{
  uint32_t currentTime = millis();

  // lecture teleinfo -------------------------
  read_teleinfo();
 
  // Only send values at a maximum frequency 
  if (currentTime - lastSend > SEND_FREQUENCY) {
    send_teleinfo();
    //change_etat_led_send();
    lastSend = currentTime;
  } 
  

}
