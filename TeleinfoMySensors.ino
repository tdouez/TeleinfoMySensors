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
// 2020/10/20 - FB V1.1.3 - Ajout options reglages NRF24L01 (puissance, débit, canal)
//                        - Optimisation émission. n'est envoyé que les données de conso changeantes
// 2020/10/26 - FB V1.1.4 - Optimisation bug fix
// 2020/11/12 - FB V1.1.5 - Ajout valeurs min, max sur INSTS..4
//                        - Suppression données à partir de EAS..4, profondeur inutile
//--------------------------------------------------------------------
// Enable debug prints
#define MY_DEBUG

// ----------------------------------------- OPTIONS
//#define MY_NODE_ID 2

/*
 @def MY_RF24_PA_LEVEL
 * @brief Default RF24 PA level. Override in sketch if needed.
 *
 * RF24_PA_MIN = -18dBm
 * RF24_PA_LOW = -12dBm
 * RF24_PA_HIGH = -6dBm    -> par defaut
 * RF24_PA_MAX = 0dBm
 */
#define MY_RF24_PA_LEVEL     RF24_PA_MAX

/*
 * @def MY_RF24_CHANNEL
 * @brief RF channel for the sensor net, 0-125.
 * Frequence: 2400 Mhz - 2525 Mhz Channels: 126
 * http://www.mysensors.org/radio/nRF24L01Plus.pdf
 * 0 => 2400 Mhz (RF24 channel 1)
 * 1 => 2401 Mhz (RF24 channel 2)
 * 76 => 2476 Mhz (RF24 channel 77)   -> par defaut
 * 83 => 2483 Mhz (RF24 channel 84)
 * 124 => 2524 Mhz (RF24 channel 125)
 * 125 => 2525 Mhz (RF24 channel 126)
 * In some countries there might be limitations, in Germany for example only the range 2400,0 - 2483,5 Mhz is allowed
 * http://www.bundesnetzagentur.de/SharedDocs/Downloads/DE/Sachgebiete/Telekommunikation/Unternehmen_Institutionen/Frequenzen/Allgemeinzuteilungen/2013_10_WLAN_2,4GHz_pdf.pdf
 */
//#define MY_RF24_CHANNEL   96       // !! doit être identique sur tous les éléments Mysensors incluant la Gateway

/**
 * @def MY_RF24_DATARATE
 * @brief RF24 datarate (RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps or RF24_2MBPS for 2Mbps).
 */
//#define MY_RF24_DATARATE RF24_250KBPS  // Valeur par défaut conservée. !! doit être identique sur tous les éléments Mysensors incluant la Gateway

// ----------------------------------------- FIN OPTIONS

#define VERSION   "v1.1.5"

#define MY_BAUD_RATE 9600    // mode standard

// Enable and select radio type attached
#define MY_RADIO_RF24
 
#define MY_DEFAULT_ERR_LED_PIN 3  // Error led pin
#define MY_DEFAULT_RX_LED_PIN  4  // Receive led pin, on board LED
#define MY_DEFAULT_TX_LED_PIN  4  // 

#define GW_DELAY	1000 // ms

#include <MySensors.h>

uint32_t SEND_FREQUENCY_CONSO =   30000; // Minimum time between send (in milliseconds). We don't want to spam the gateway.
uint32_t SEND_FREQUENCY_INFO =   120000; // Minimum time between send (in milliseconds). We don't want to spam the gateway.
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
  unsigned long EASD01=0;
  unsigned long EASD02=0;
  unsigned long EASD03=0;
  unsigned long ERQ1=0;
  unsigned long ERQ2=0;
  unsigned long ERQ3=0;
  unsigned long ERQ4=0;
} teleinfo_s;

struct teleinfo_s teleinfo;
struct teleinfo_s teleinfo_memo;

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
#define CHILD_ID_EASD01   34
#define CHILD_ID_EASD02   35
#define CHILD_ID_EASD03   36
#define CHILD_ID_ERQ1     38
#define CHILD_ID_ERQ2     39
#define CHILD_ID_ERQ3     40
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
void before()
{


  Serial.begin(MY_BAUD_RATE);
  
  Serial.println(F("   __|              _/           _ )  |"));
  Serial.println(F("   _| |  |   ` \\    -_)   -_)    _ \\  |   -_)  |  |   -_)"));
  Serial.println(F("  _| \\_,_| _|_|_| \\___| \\___|   ___/ _| \\___| \\_,_| \\___|"));
  Serial.print(F("                                             "));
  Serial.println(VERSION);

  memset(&teleinfo, 0, sizeof(teleinfo));
  memset(&teleinfo_memo, 0, sizeof(teleinfo_memo));
  //delay(2000);
  Serial.println("--> FIN BEFORE");
}

//--------------------------------------------------------------------
void setup()
{
  Serial.println("SETUP");
  memset(&teleinfo, 0, sizeof(teleinfo));
  memset(&teleinfo_memo, 0, sizeof(teleinfo_memo));
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
  present( CHILD_ID_VTIC, S_INFO, F("Version TIC"));
  present( CHILD_ID_NGTF, S_INFO, F("Nom du calendrier tarifaire"));
  present( CHILD_ID_LTARF, S_INFO, F("Libelle tarif"));
  present( CHILD_ID_EAST, S_POWER, F("Energie active soutiree totale"));
  present( CHILD_ID_EAIT, S_POWER, F("Energie active injectee"));
  present( CHILD_ID_IRMS1, S_MULTIMETER, F("Courant efficace, phase 1"));
  present( CHILD_ID_IRMS2, S_MULTIMETER, F("Courant efficace, phase 2"));
  present( CHILD_ID_IRMS3, S_MULTIMETER, F("Courant efficace, phase 3"));
  present( CHILD_ID_URMS1, S_MULTIMETER, F("Tension efficace, phase 1"));
  present( CHILD_ID_URMS2, S_MULTIMETER, F("Tension efficace, phase 2"));
  present( CHILD_ID_URMS3, S_MULTIMETER, F("Tension efficace, phase 3"));
  present( CHILD_ID_PREF, S_POWER, F("Puissance apparente ref"));
  present( CHILD_ID_PCOUP, S_POWER, F("Puissance coupure"));
  present( CHILD_ID_SINSTS, S_POWER, F("Puissance apparente"));
  present( CHILD_ID_SINSTS1, S_POWER, F("Puissance apparente phase 1"));
  present( CHILD_ID_SINSTS2, S_POWER, F("Puissance apparente phase 2"));
  present( CHILD_ID_SINSTS3, S_POWER, F("Puissance apparente phase 3"));
  present( CHILD_ID_SINSTI, S_POWER, F("Puissance apparente injectee"));
  present( CHILD_ID_STGE, S_INFO, F("Registre de Statuts"));
  present( CHILD_ID_MSG1, S_INFO, F("Message"));
  present( CHILD_ID_NTARF, S_INFO, F("Index tarifaire en cours"));
  present( CHILD_ID_NJOURF, S_INFO, F("Jour en cours"));
  present( CHILD_ID_NJOURF1, S_INFO, F("Prochain jour"));
  present( CHILD_ID_EASF01, S_POWER, F("Energie active soutiree F, index 1"));
  present( CHILD_ID_EASF02, S_POWER, F("Energie active soutiree F, index 2"));
  present( CHILD_ID_EASF03, S_POWER, F("Energie active soutiree F, index 3"));
  present( CHILD_ID_EASD01, S_POWER, F("Energie active soutiree D, index 1"));
  present( CHILD_ID_EASD02, S_POWER, F("Energie active soutiree D, index 2"));
  present( CHILD_ID_EASD03, S_POWER, F("Energie active soutiree D, index 3"));
  present( CHILD_ID_ERQ1, S_POWER, F("Energie reactive Q1 totale"));
  present( CHILD_ID_ERQ2, S_POWER, F("Energie reactive Q2 totale"));
  present( CHILD_ID_ERQ3, S_POWER, F("Energie reactive Q3 totale"));
  present( CHILD_ID_START, S_POWER, F("Date demarrage module"));

}

//--------------------------------------------------------------------
void send_teleinfo_conso()
{

  // EAST
  if (teleinfo.EAST != teleinfo_memo.EAST) {
    teleinfo_memo.EAST = teleinfo.EAST;
    send(msgKWH.setSensor(CHILD_ID_EAST).set(teleinfo.EAST));
  }
  // IRMS1
  if (teleinfo.IRMS1 != teleinfo_memo.IRMS1) {
    teleinfo_memo.IRMS1 = teleinfo.IRMS1;
    send(msgCURRENT.setSensor(CHILD_ID_IRMS1).set(teleinfo.IRMS1));
  }
  // URMS1
  if (teleinfo.URMS1 != teleinfo_memo.URMS1) {
    teleinfo_memo.URMS1 = teleinfo.URMS1;
    send(msgVOLTAGE.setSensor(CHILD_ID_URMS1).set(teleinfo.URMS1));
  }
  // PREF
  if (teleinfo.PREF != teleinfo_memo.PREF) {
    teleinfo_memo.PREF = teleinfo.PREF;
    send(msgVA.setSensor(CHILD_ID_PREF).set(teleinfo.PREF));
  }
  // SINSTS
  if (teleinfo.SINSTS != teleinfo_memo.SINSTS) {
    teleinfo_memo.SINSTS = teleinfo.SINSTS;
    send(msgVA.setSensor(CHILD_ID_SINSTS).set(teleinfo.SINSTS));
  }
  // SINSTI
  if (teleinfo.SINSTI != teleinfo_memo.SINSTI) {
    teleinfo_memo.SINSTI = teleinfo.SINSTI;
    send(msgVA.setSensor(CHILD_ID_SINSTI).set(teleinfo.SINSTI));
  }
  // EASF01..3
  if (teleinfo.EASF01 != teleinfo_memo.EASF01) {
    teleinfo_memo.EASF01 = teleinfo.EASF01;
    send(msgKWH.setSensor(CHILD_ID_EASF01).set(teleinfo.EASF01));
  }
  if (teleinfo.EASF02 != teleinfo_memo.EASF02) {
    teleinfo_memo.EASF02 = teleinfo.EASF02;
    send(msgKWH.setSensor(CHILD_ID_EASF02).set(teleinfo.EASF02));
  }
  if (teleinfo.EASF03 != teleinfo_memo.EASF03) {
    teleinfo_memo.EASF03 = teleinfo.EASF03;
    send(msgKWH.setSensor(CHILD_ID_EASF03).set(teleinfo.EASF03));
  }
  // EASD01..3 
  if (teleinfo.EASD01 != teleinfo_memo.EASD01) {
    teleinfo_memo.EASD01 = teleinfo.EASD01;
    send(msgKWH.setSensor(CHILD_ID_EASD01).set(teleinfo.EASD01));
  }
  if (teleinfo.EASD02 != teleinfo_memo.EASD02) {
    teleinfo_memo.EASD02 = teleinfo.EASD02;
    send(msgKWH.setSensor(CHILD_ID_EASD02).set(teleinfo.EASD02));
  }
  if (teleinfo.EASD03 != teleinfo_memo.EASD03) {
    teleinfo_memo.EASD03 = teleinfo.EASD03;
    send(msgKWH.setSensor(CHILD_ID_EASD03).set(teleinfo.EASD03));
  }
    
  if (mode_triphase) {
	  // IRMS2
    if (teleinfo.IRMS2 != teleinfo_memo.IRMS2) {
      teleinfo_memo.IRMS2 = teleinfo.IRMS2;
	    send(msgCURRENT.setSensor(CHILD_ID_IRMS2).set(teleinfo.IRMS2));
    }
	  // IRMS3
    if (teleinfo.IRMS3 != teleinfo_memo.IRMS3) {
      teleinfo_memo.IRMS3 = teleinfo.IRMS3;
	    send(msgCURRENT.setSensor(CHILD_ID_IRMS3).set(teleinfo.IRMS3));
    }
	  // URMS2
    if (teleinfo.URMS2 != teleinfo_memo.URMS2) {
      teleinfo_memo.URMS2 = teleinfo.URMS2;
	    send(msgVOLTAGE.setSensor(CHILD_ID_URMS2).set(teleinfo.URMS2));
    }
	  // URMS3
    if (teleinfo.URMS3 != teleinfo_memo.URMS3) {
      teleinfo_memo.URMS3 = teleinfo.URMS3;
	    send(msgVOLTAGE.setSensor(CHILD_ID_URMS3).set(teleinfo.URMS3));
    }
	  // SINSTS1
    if (teleinfo.SINSTS1 != teleinfo_memo.SINSTS1) {
      teleinfo_memo.SINSTS1 = teleinfo.SINSTS1;
	    send(msgVA.setSensor(CHILD_ID_SINSTS1).set(teleinfo.SINSTS1));
    }
	  // SINSTS2
    if (teleinfo.SINSTS2 != teleinfo_memo.SINSTS2) {
      teleinfo_memo.SINSTS2 = teleinfo.SINSTS2;
	    send(msgVA.setSensor(CHILD_ID_SINSTS2).set(teleinfo.SINSTS2));
    }
	  // SINSTS3
    if (teleinfo.SINSTS3 != teleinfo_memo.SINSTS3) {
      teleinfo_memo.SINSTS3 = teleinfo.SINSTS3;
	    send(msgVA.setSensor(CHILD_ID_SINSTS3).set(teleinfo.SINSTS3));
    }
  }
  
  if (mode_producteur) {
	  // EAIT
    if (teleinfo.EAIT != teleinfo_memo.EAIT) {
      teleinfo_memo.EAIT = teleinfo.EAIT;
	    send(msgKWH.setSensor(CHILD_ID_EAIT).set(teleinfo.EAIT));
    }
	  // ERQ1..3 
    if (teleinfo.ERQ1 != teleinfo_memo.ERQ1) {
      teleinfo_memo.ERQ1 = teleinfo.ERQ1;
	    send(msgKWH.setSensor(CHILD_ID_ERQ1).set(teleinfo.ERQ1));
    }
    if (teleinfo.ERQ2 != teleinfo_memo.ERQ2) {
      teleinfo_memo.ERQ2 = teleinfo.ERQ2;
	    send(msgKWH.setSensor(CHILD_ID_ERQ2).set(teleinfo.ERQ2));
    }
    if (teleinfo.ERQ3 != teleinfo_memo.ERQ3) {
      teleinfo_memo.ERQ3 = teleinfo.ERQ3;
	    send(msgKWH.setSensor(CHILD_ID_ERQ3).set(teleinfo.ERQ3));
    }
  }
  
}

//--------------------------------------------------------------------
void send_teleinfo_info()
{
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
  // NTARF
  send(msgTEXT.setSensor(CHILD_ID_NTARF).set(teleinfo.NTARF));
  // NJOURF
  send(msgTEXT.setSensor(CHILD_ID_NJOURF).set(teleinfo.NJOURF));
  // NJOURF1
  send(msgTEXT.setSensor(CHILD_ID_NJOURF1).set(teleinfo.NJOURF1));
  // START
  send(msgVAR1.setSensor(CHILD_ID_START).set(startTime)); 
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
