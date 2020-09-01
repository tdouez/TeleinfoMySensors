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
// 2020/08/18 - FB V1.1.1 - Correction sur NTARF et LTARF - Merci David MARLINGE
//                        - Ajout EASF01..10, EASD01..04 et ERQ1..4
//                        - Optimisation mémoire
// 2020/08/26 - FB V1.1.2 - Ajout delais sur presentation et send, afin de soulager la gateway
//                        - Envoi données producteur et triphasé si besoin
//                        - Correction sur send EASF et EADS en WH au lieu du VA
//                        - Accents retirés sur les libellés de présentation
//--------------------------------------------------------------------
// Enable debug prints
//#define MY_DEBUG


//#define MY_NODE_ID 2

#define VERSION   "v1.1.2"

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

#define GW_DELAY	25 // ms

#include <MySensors.h>

uint32_t SEND_FREQUENCY_CONSO =    5000; // Minimum time between send (in milliseconds). We don't want to spam the gateway.
uint32_t SEND_FREQUENCY_INFO =    90000; // Minimum time between send (in milliseconds). We don't want to spam the gateway.
uint32_t lastSend_conso = 0;
uint32_t lastSend_info = 0;

unsigned long startTime; // Date démarrage 

boolean mode_producteur = false;
boolean mode_triphase = false;


// Variables Téléinfo---------------------
struct teleinfo_s {
  char _ADSC[13]="";
  char VTIC[3]="";
  char NGTF[17]="";
  char LTARF[17]="";
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
  char MSG1[33]="";
  char NTARF[3]="";
  char NJOURF[3]="";
  char NJOURF1[3]="";
  unsigned long EASF01=0;
  unsigned long EASF02=0;
  unsigned long EASF03=0;
  unsigned long EASF04=0;
  unsigned long EASF05=0;
  unsigned long EASF06=0;
  unsigned long EASF07=0;
  unsigned long EASF08=0;
  unsigned long EASF09=0;
  unsigned long EASF10=0;
  unsigned long EASD01=0;
  unsigned long EASD02=0;
  unsigned long EASD03=0;
  unsigned long EASD04=0;
  unsigned long ERQ1=0;
  unsigned long ERQ2=0;
  unsigned long ERQ3=0;
  unsigned long ERQ4=0;
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
#define CHILD_ID_NTARF    19
#define CHILD_ID_NJOURF   20
#define CHILD_ID_NJOURF1  21
#define CHILD_ID_EAIT     22
#define CHILD_ID_SINSTI   23
#define CHILD_ID_EASF01   24
#define CHILD_ID_EASF02   25
#define CHILD_ID_EASF03   26
#define CHILD_ID_EASF04   27
#define CHILD_ID_EASF05   28
#define CHILD_ID_EASF06   29
#define CHILD_ID_EASF07   30
#define CHILD_ID_EASF08   31
#define CHILD_ID_EASF09   32
#define CHILD_ID_EASF10   33
#define CHILD_ID_EASD01   34
#define CHILD_ID_EASD02   35
#define CHILD_ID_EASD03   36
#define CHILD_ID_EASD04   37
#define CHILD_ID_ERQ1     38
#define CHILD_ID_ERQ2     39
#define CHILD_ID_ERQ3     40
#define CHILD_ID_ERQ4     41
#define CHILD_ID_START    99

MyMessage msgTEXT( 0, V_TEXT);        // S_INFO
MyMessage msgCURRENT( 0, V_CURRENT ); // S_MULTIMETER
MyMessage msgVOLTAGE( 0, V_VOLTAGE ); // S_MULTIMETER
MyMessage msgWATT( 0, V_WATT ); // S_POWER
MyMessage msgKWH( 0, V_KWH );   // S_POWER
MyMessage msgVA( 0, V_VA );     // S_POWER
MyMessage msgVAR1( 0, V_VAR1 ); // Custom value


//-----------------------------------------------------------------------
// This is called when a new time value was received
void receiveTime(unsigned long controllerTime) 
{
  // incoming time 
  Serial.print(">> Time received from gw: ");
  Serial.println(controllerTime);
  startTime = controllerTime;
}

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
  
  // Récupere l'heure de la gateway
  requestTime();
  
  // Register this device as power sensor
  present( CHILD_ID_ADSC, S_INFO, F("Adresse Compteur"));
  wait(GW_DELAY);
  present( CHILD_ID_VTIC, S_INFO, F("Version TIC"));
  wait(GW_DELAY);
  present( CHILD_ID_NGTF, S_INFO, F("Nom du calendrier tarifaire"));
  wait(GW_DELAY);
  present( CHILD_ID_LTARF, S_INFO, F("Libelle tarif"));
  wait(GW_DELAY);
  present( CHILD_ID_EAST, S_POWER, F("Energie active soutiree totale"));
  wait(GW_DELAY);
  present( CHILD_ID_EAIT, S_POWER, F("Energie active injectee"));
  wait(GW_DELAY);
  present( CHILD_ID_IRMS1, S_MULTIMETER, F("Courant efficace, phase 1"));
  wait(GW_DELAY);
  present( CHILD_ID_IRMS2, S_MULTIMETER, F("Courant efficace, phase 2"));
  wait(GW_DELAY);
  present( CHILD_ID_IRMS3, S_MULTIMETER, F("Courant efficace, phase 3"));
  wait(GW_DELAY);
  present( CHILD_ID_URMS1, S_MULTIMETER, F("Tension efficace, phase 1"));
  wait(GW_DELAY);
  present( CHILD_ID_URMS2, S_MULTIMETER, F("Tension efficace, phase 2"));
  wait(GW_DELAY);
  present( CHILD_ID_URMS3, S_MULTIMETER, F("Tension efficace, phase 3"));
  wait(GW_DELAY);
  present( CHILD_ID_PREF, S_POWER, F("Puissance apparente ref"));
  wait(GW_DELAY);
  present( CHILD_ID_PCOUP, S_POWER, F("Puissance coupure"));
  wait(GW_DELAY);
  present( CHILD_ID_SINSTS, S_POWER, F("Puissance apparente"));
  wait(GW_DELAY);
  present( CHILD_ID_SINSTS1, S_POWER, F("Puissance apparente phase 1"));
  wait(GW_DELAY);
  present( CHILD_ID_SINSTS2, S_POWER, F("Puissance apparente phase 2"));
  wait(GW_DELAY);
  present( CHILD_ID_SINSTS3, S_POWER, F("Puissance apparente phase 3"));
  wait(GW_DELAY);
  present( CHILD_ID_SINSTI, S_POWER, F("Puissance apparente injectee"));
  wait(GW_DELAY);
  present( CHILD_ID_STGE, S_INFO, F("Registre de Statuts"));
  wait(GW_DELAY);
  present( CHILD_ID_MSG1, S_INFO, F("Message"));
  wait(GW_DELAY);
  present( CHILD_ID_NTARF, S_INFO, F("Index tarifaire en cours"));
  wait(GW_DELAY);
  present( CHILD_ID_NJOURF, S_INFO, F("Jour en cours"));
  wait(GW_DELAY);
  present( CHILD_ID_NJOURF1, S_INFO, F("Prochain jour"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF01, S_POWER, F("Energie active soutiree F, index 1"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF02, S_POWER, F("Energie active soutiree F, index 2"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF03, S_POWER, F("Energie active soutiree F, index 3"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF04, S_POWER, F("Energie active soutiree F, index 4"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF05, S_POWER, F("Energie active soutiree F, index 5"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF06, S_POWER, F("Energie active soutiree F, index 6"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF07, S_POWER, F("Energie active soutiree F, index 7"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF08, S_POWER, F("Energie active soutiree F, index 8"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF09, S_POWER, F("Energie active soutiree F, index 9"));
  wait(GW_DELAY);
  present( CHILD_ID_EASF10, S_POWER, F("Energie active soutiree F, index 10"));
  wait(GW_DELAY);
  present( CHILD_ID_EASD01, S_POWER, F("Energie active soutiree D, index 1"));
  wait(GW_DELAY);
  present( CHILD_ID_EASD01, S_POWER, F("Energie active soutiree D, index 1"));
  wait(GW_DELAY);
  present( CHILD_ID_EASD02, S_POWER, F("Energie active soutiree D, index 2"));
  wait(GW_DELAY);
  present( CHILD_ID_EASD03, S_POWER, F("Energie active soutiree D, index 3"));
  wait(GW_DELAY);
  present( CHILD_ID_EASD04, S_POWER, F("Energie active soutiree D, index 4"));
  wait(GW_DELAY);
  present( CHILD_ID_ERQ1, S_POWER, F("Energie reactive Q1 totale"));
  wait(GW_DELAY);
  present( CHILD_ID_ERQ2, S_POWER, F("Energie reactive Q2 totale"));
  wait(GW_DELAY);
  present( CHILD_ID_ERQ3, S_POWER, F("Energie reactive Q3 totale"));
  wait(GW_DELAY);
  present( CHILD_ID_ERQ4, S_POWER, F("Energie reactive Q4 totale"));
  wait(GW_DELAY);
  present( CHILD_ID_START, S_POWER, F("Date demarrage module"));

}

//--------------------------------------------------------------------
void send_teleinfo_conso()
{
boolean flag_hhphc = false;

  // EAST
  send(msgKWH.setSensor(CHILD_ID_EAST).set(teleinfo.EAST));
  wait(GW_DELAY);
  // IRMS1
  send(msgCURRENT.setSensor(CHILD_ID_IRMS1).set(teleinfo.IRMS1));
  wait(GW_DELAY);
  // URMS1
  send(msgVOLTAGE.setSensor(CHILD_ID_URMS1).set(teleinfo.URMS1));
  wait(GW_DELAY);
  // PREF
  send(msgVA.setSensor(CHILD_ID_PREF).set(teleinfo.PREF));
  wait(GW_DELAY);
  // SINSTS
  send(msgVA.setSensor(CHILD_ID_SINSTS).set(teleinfo.SINSTS));
  wait(GW_DELAY);
  // SINSTI
  send(msgVA.setSensor(CHILD_ID_SINSTI).set(teleinfo.SINSTI));
  wait(GW_DELAY);
  // EASF01..10
  send(msgKWH.setSensor(CHILD_ID_EASF01).set(teleinfo.EASF01));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF02).set(teleinfo.EASF02));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF03).set(teleinfo.EASF03));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF04).set(teleinfo.EASF04));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF05).set(teleinfo.EASF05));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF06).set(teleinfo.EASF06));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF07).set(teleinfo.EASF07));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF08).set(teleinfo.EASF08));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF09).set(teleinfo.EASF09));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASF10).set(teleinfo.EASF10));
  wait(GW_DELAY);
  // EASD01..4 
  send(msgKWH.setSensor(CHILD_ID_EASD01).set(teleinfo.EASD01));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASD02).set(teleinfo.EASD02));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASD03).set(teleinfo.EASD03));
  wait(GW_DELAY);
  send(msgKWH.setSensor(CHILD_ID_EASD04).set(teleinfo.EASD04));
    
  if (mode_triphase) {
	  // IRMS2
	  send(msgCURRENT.setSensor(CHILD_ID_IRMS2).set(teleinfo.IRMS2));
	  wait(GW_DELAY);
	  // IRMS3
	  send(msgCURRENT.setSensor(CHILD_ID_IRMS3).set(teleinfo.IRMS3));
	  wait(GW_DELAY);
	  // URMS2
	  send(msgVOLTAGE.setSensor(CHILD_ID_URMS2).set(teleinfo.URMS2));
	  wait(GW_DELAY);
	  // URMS3
	  send(msgVOLTAGE.setSensor(CHILD_ID_URMS3).set(teleinfo.URMS3));
	  wait(GW_DELAY);
	  // SINSTS1
	  send(msgVA.setSensor(CHILD_ID_SINSTS1).set(teleinfo.SINSTS1));
	  wait(GW_DELAY);
	  // SINSTS2
	  send(msgVA.setSensor(CHILD_ID_SINSTS2).set(teleinfo.SINSTS2));
	  wait(GW_DELAY);
	  // SINSTS3
	  send(msgVA.setSensor(CHILD_ID_SINSTS3).set(teleinfo.SINSTS3));
	  wait(GW_DELAY);
  }
  
  if (mode_producteur) {
	  // EAIT
	  send(msgKWH.setSensor(CHILD_ID_EAIT).set(teleinfo.EAIT));
	  wait(GW_DELAY);
	  // ERQ1..4 
	  send(msgKWH.setSensor(CHILD_ID_ERQ1).set(teleinfo.ERQ1));
	  wait(GW_DELAY);
	  send(msgKWH.setSensor(CHILD_ID_ERQ2).set(teleinfo.ERQ2));
	  wait(GW_DELAY);
	  send(msgKWH.setSensor(CHILD_ID_ERQ3).set(teleinfo.ERQ3));
	  wait(GW_DELAY);
	  send(msgKWH.setSensor(CHILD_ID_ERQ4).set(teleinfo.ERQ4));
	  wait(GW_DELAY);
  }
  
}

//--------------------------------------------------------------------
void send_teleinfo_info()
{
boolean flag_hhphc = false;

  // ADSC
  send(msgTEXT.setSensor(CHILD_ID_ADSC).set(teleinfo._ADSC));
  wait(GW_DELAY);
  // VTIC
  send(msgTEXT.setSensor(CHILD_ID_VTIC).set(teleinfo.VTIC));
  wait(GW_DELAY);
  // NGTF
  send(msgTEXT.setSensor(CHILD_ID_NGTF).set(teleinfo.NGTF));
  wait(GW_DELAY);
  // LTARF
  send(msgTEXT.setSensor(CHILD_ID_LTARF).set(teleinfo.LTARF));
  wait(GW_DELAY);
  // PCOUP
  send(msgVA.setSensor(CHILD_ID_PCOUP).set(teleinfo.PCOUP));
  wait(GW_DELAY);
  // STGE
  send(msgTEXT.setSensor(CHILD_ID_STGE).set(teleinfo.STGE));
  wait(GW_DELAY);
  // MSG1
  send(msgTEXT.setSensor(CHILD_ID_MSG1).set(teleinfo.MSG1));
  wait(GW_DELAY);
  // NTARF
  send(msgTEXT.setSensor(CHILD_ID_NTARF).set(teleinfo.NTARF));
  wait(GW_DELAY);
  // NJOURF
  send(msgTEXT.setSensor(CHILD_ID_NJOURF).set(teleinfo.NJOURF));
  wait(GW_DELAY);
  // NJOURF1
  send(msgTEXT.setSensor(CHILD_ID_NJOURF1).set(teleinfo.NJOURF1));
  wait(GW_DELAY);
  // START
  send(msgVAR1.setSensor(CHILD_ID_START).set(startTime));
  wait(GW_DELAY);  
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
