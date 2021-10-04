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
//--------------------------------------------------------------------
// Enable debug prints
//#define MY_DEBUG

#define MY_NODE_ID 3
#define MY_REPEATER_FEATURE

#define VERSION   "v1.0.7"

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
//#define MY_RF24_PA_LEVEL RF24_PA_LOW

#define MY_BAUD_RATE 1200

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
  char ADCO[13]="";
  char OPTARIF[5]="";
  unsigned int ISOUSC=0; 
  unsigned long BASE=0;
  unsigned long HCHC=0;
  unsigned long HCHP=0;
  unsigned long EJP_HN=0;
  unsigned long EJP_HPM=0;
  unsigned long BBR_HC_JB=0;
  unsigned long BBR_HP_JB=0;
  unsigned long BBR_HC_JW=0;
  unsigned long BBR_HP_JW=0;
  unsigned long BBR_HC_JR=0;
  unsigned long BBR_HP_JR=0;
  char PEJP[3]="";
  char PTEC[5]="";
  char DEMAIN[5]="";
  unsigned int IINST=0;
  unsigned int PAPP=0;
  unsigned int ADPS=0;
  unsigned int IMAX=0;
  char HHPHC[2]="";
};
teleinfo_s teleinfo;


//#define LED_SEND         4  
//#define LED_TELEINFO     3 


#define CHILD_ID_ADCO     0
#define CHILD_ID_OPTARIF  1
#define CHILD_ID_ISOUSC   2
#define CHILD_ID_BASE     3
#define CHILD_ID_HCHC     4
#define CHILD_ID_HCHP     5
#define CHILD_ID_EJP_HN   6
#define CHILD_ID_EJP_HPM  7
#define CHILD_ID_BBR_HC_JB 8
#define CHILD_ID_BBR_HP_JB 9
#define CHILD_ID_BBR_HC_JW 10
#define CHILD_ID_BBR_HP_JW 11
#define CHILD_ID_BBR_HC_JR 12
#define CHILD_ID_BBR_HP_JR 13
#define CHILD_ID_PEJP      14
#define CHILD_ID_PTEC      15
#define CHILD_ID_DEMAIN    16
#define CHILD_ID_IINST     17
#define CHILD_ID_PAPP      18
#define CHILD_ID_ADPS      19
#define CHILD_ID_IMAX      20
#define CHILD_ID_HHPHC     21
// #define CHILD_ID_DEBUG     22



MyMessage msgVAR1_ADCO( 0, V_VAR1 );
MyMessage msgVAR2_OPTARIF( 0, V_VAR2 );
MyMessage msgVAR3_PTEC( 0, V_VAR3 );
MyMessage msgVAR4_HHPHC( 0, V_VAR4 );
MyMessage msgVAR5_DEMAIN( 0, V_VAR5 );
MyMessage msgCURRENT( 0, V_CURRENT );
MyMessage msgVA( 0, V_VA );
MyMessage msgKWH( 0, V_KWH );
MyMessage msgPrefix( 0, V_UNIT_PREFIX );



//--------------------------------------------------------------------
void setup()
{
  
  /*
  pinMode(LED_SEND, OUTPUT);
  pinMode(LED_TELEINFO, OUTPUT);

  digitalWrite(LED_SEND , HIGH);
  digitalWrite(LED_TELEINFO , HIGH);
  */
  
  Serial.begin(1200);
 
  Serial.println(F("   __|              _/           _ )  |"));
  Serial.println(F("   _| |  |   ` \\    -_)   -_)    _ \\  |   -_)  |  |   -_)"));
  Serial.println(F("  _| \\_,_| _|_|_| \\___| \\___|   ___/ _| \\___| \\_,_| \\___|"));
  Serial.print(F("                                             "));
  Serial.println(VERSION);

  send(msgPrefix.setSensor(CHILD_ID_BASE).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_EJP_HN).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_EJP_HPM).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_PEJP).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_HCHC).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_HCHP).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_BBR_HC_JB).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_BBR_HP_JB).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_BBR_HC_JW).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_BBR_HP_JW).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_BBR_HC_JR).set("Wh"));
  send(msgPrefix.setSensor(CHILD_ID_BBR_HP_JR).set("Wh"));
/*
  digitalWrite(LED_SEND, LOW);
  delay(200);
  digitalWrite(LED_SEND , HIGH);
  delay(200);
  digitalWrite(LED_TELEINFO, LOW);
  delay(200);
  digitalWrite(LED_TELEINFO , HIGH);
  */
}

//--------------------------------------------------------------------
void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Teleinfo", VERSION);

  // Register this device as power sensor
  present( CHILD_ID_ADCO, S_CUSTOM, "ADCO" );
  present( CHILD_ID_OPTARIF, S_CUSTOM, "OPTARIF" );
  present( CHILD_ID_ISOUSC, S_MULTIMETER, "ISOUSC" );
  present( CHILD_ID_BASE, S_POWER, "BASE" );
  present( CHILD_ID_HCHC, S_POWER, "HCHC" );
  present( CHILD_ID_HCHP, S_POWER, "HCHP" );
  present( CHILD_ID_EJP_HN, S_POWER, "EJP_HN" );
  present( CHILD_ID_EJP_HPM, S_POWER, "EJP_HPM" );
  present( CHILD_ID_BBR_HC_JB, S_POWER, "BBR_HC_JB" );
  present( CHILD_ID_BBR_HP_JB, S_POWER, "BBR_HP_JB" );
  present( CHILD_ID_BBR_HC_JW, S_POWER, "BBR_HC_JW" );
  present( CHILD_ID_BBR_HP_JW, S_POWER, "BBR_HP_JW" );
  present( CHILD_ID_BBR_HC_JR, S_POWER, "BBR_HC_JR" );
  present( CHILD_ID_BBR_HP_JR, S_POWER, "BBR_HP_JR" );
  present( CHILD_ID_PEJP, S_CUSTOM, "PEJP" );
  present( CHILD_ID_PTEC, S_POWER, "PTEC" );
  present( CHILD_ID_DEMAIN, S_CUSTOM, "DEMAIN" );
  present( CHILD_ID_IINST, S_MULTIMETER, "IINST" );
  present( CHILD_ID_ADPS, S_MULTIMETER, "ADPS" );
  present( CHILD_ID_IMAX, S_MULTIMETER, "IMAX" );
  present( CHILD_ID_PAPP, S_POWER, "PAPP" );
  present( CHILD_ID_HHPHC, S_CUSTOM, "HHPHC" );
}

//--------------------------------------------------------------------
void send_teleinfo()
{
boolean flag_hhphc = false;

  // ADCO
  send(msgVAR1_ADCO.setSensor(CHILD_ID_ADCO).set(teleinfo.ADCO));
  
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
  send(msgVA.setSensor(CHILD_ID_PAPP).set(teleinfo.PAPP));

  // ADPS
  send(msgCURRENT.setSensor(CHILD_ID_ADPS).set(teleinfo.ADPS));

  // IMAX
  send(msgCURRENT.setSensor(CHILD_ID_IMAX).set(teleinfo.IMAX));

  // HHPHC
  if (flag_hhphc == true) { // cas particulier, appartient a EJP et TEMPO
      send(msgVAR4_HHPHC.setSensor(CHILD_ID_HHPHC).set(teleinfo.HHPHC));
  }
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
