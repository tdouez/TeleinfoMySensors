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
// 2019:12/15 - FB V1.0.2 - optimisation/simplifications et reduction conso mémoire
//--------------------------------------------------------------------
// Enable debug prints
//#define MY_DEBUG

#define VERSION   "v1.0.2"


#define MY_BAUD_RATE 1200

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

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


#define LED_ERROR		     5  // Error led pin
#define LED_TELEINFO     4 


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


MyMessage msgVAR1_ADCO( 0, V_VAR1 );
MyMessage msgVAR2_OPTARIF( 0, V_VAR2 );
MyMessage msgVAR3_PTEC( 0, V_VAR3 );
MyMessage msgVAR4_HHPHC( 0, V_VAR4 );
MyMessage msgVAR5_DEMAIN( 0, V_VAR5 );
MyMessage msgCURRENT( 0, V_CURRENT );
MyMessage msgWATT( 0, V_WATT );
MyMessage msgKWH( 0, V_KWH );


//--------------------------------------------------------------------
void setup()
{
	
  pinMode(LED_ERROR, OUTPUT);
  pinMode(LED_TELEINFO, OUTPUT);

  digitalWrite(LED_ERROR , HIGH);
  digitalWrite(LED_TELEINFO , HIGH);
  
  Serial.begin(1200);
 
  Serial.println(F("   __|              _/           _ )  |"));
  Serial.println(F("   _| |  |   ` \\    -_)   -_)    _ \\  |   -_)  |  |   -_)"));
  Serial.println(F("  _| \\_,_| _|_|_| \\___| \\___|   ___/ _| \\___| \\_,_| \\___|"));
  Serial.print(F("                                             "));
  Serial.println(VERSION);

  digitalWrite(LED_ERROR, LOW);
  delay(200);
  digitalWrite(LED_ERROR , HIGH);
  delay(200);
  digitalWrite(LED_TELEINFO, LOW);
  delay(200);
  digitalWrite(LED_TELEINFO , HIGH);
}

//--------------------------------------------------------------------
void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("Teleinfo", VERSION);

	// Register this device as power sensor
	present( CHILD_ID_ADCO, S_POWER );
  present( CHILD_ID_OPTARIF, S_POWER );
  present( CHILD_ID_ISOUSC, S_POWER );
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
  present( CHILD_ID_PTEC, S_POWER );
  present( CHILD_ID_DEMAIN, S_POWER );
  present( CHILD_ID_IINST, S_POWER );
  present( CHILD_ID_ADPS, S_POWER );
  present( CHILD_ID_IMAX, S_POWER );
  present( CHILD_ID_PAPP, S_POWER );
  present( CHILD_ID_HHPHC, S_POWER );

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
  send(msgWATT.setSensor(CHILD_ID_PAPP).set(teleinfo.PAPP));

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
		lastSend = currentTime;
	} 
  

}
