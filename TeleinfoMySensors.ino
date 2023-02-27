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
// 2021/10/05 - FB V1.1.6 - Ajout prefixes et corrections sur type de compteur
// 2022/06/10 - FB V2.0.0 - Utilisation librairie universelle LibTeleinfo modifiée (pas d'enregistrement, pas assez de RAM sur l'ATmega328 pour avoir une solution stable en TIC full)
//                        - Compatible mode historique et mode standard; détermination automatique au démarrage
// 2022/09/25 - FB V2.0.1 - Correction sur détection TIC standard
// 2023/01/23 - FB V2.0.2 - Correction sur l'envoi des prefixes des chil_id_smax. Merci à Laurent B.
// 2023/02/02 - FB V2.0.3 - Mise à jour librairie LibTeleinfo v1.1.5 avec derniers correctifs https://github.com/hallard/LibTeleinfo/releases
// 2023/02/27 - FB V2.0.4 - Correction sur l'envoi des préfixes CHILD_ID_CCA*
//--------------------------------------------------------------------

// Enable debug prints MySensors
#define MY_DEBUG

//#define CARTE_SWITCH /* A décommenter si utilisation de PCB doté de micro-switchs */

// ----------------------------------------- OPTIONS
#ifdef CARTE_SWITCH
int8_t myNodeId;
#define MY_NODE_ID myNodeId
#else
// #define MY_NODE_ID 123
#endif

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

#define VERSION   "v2.0.4"

#define DELAY_PREFIX  50
#define DELAY_SEND    50
#define DELAY_PRESENTATION  100

#define MY_BAUD_RATE 9600    // mode standard

// Enable and select radio type attached
#define MY_RADIO_RF24

#define MY_DEFAULT_ERR_LED_PIN 3  // Error led pin
#define MY_DEFAULT_RX_LED_PIN  4  // Receive led pin, on board LED
#define MY_DEFAULT_TX_LED_PIN  4  // 

#define SWITCH_1  5
#define SWITCH_2  6
#define SWITCH_3  7
#define SWITCH_4  8

#include <MySensors.h>
#include "LibTeleinfoLite.h"

/*
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
*/

const unsigned long SEND_FREQUENCY_FULL = 180000; // 3mn, Minimum time between send (in milliseconds). 
const unsigned long SEND_FREQUENCY_TIC  =  15000; // 15s,  Minimum time between send (in milliseconds). 
unsigned long lastTime_full = 0;
unsigned long lastTime_tic = 0;
unsigned long startTime; // Date démarrage 
_Mode_e mode_tic;
boolean flag_first = true;
boolean flag_full_tic = true;
boolean flag_tic = true;

TInfo tinfo;

#define char_AMPERE "A"
#define char_VOLT "V"
#define char_WATT "W"
#define char_WATT_HEURE "Wh"
#define char_VA "VA"
#define char_KVA "kVA"
#define char_VAR_HEURE "VArh"
#define char_MIN "min"

// Mode Historique ------------
const char char_ADCO[] PROGMEM = "ADCO";
const char char_OPTARIF[] PROGMEM = "OPTARIF";
const char char_ISOUSC[] PROGMEM = "ISOUSC";
const char char_BASE[] PROGMEM = "BASE";
const char char_HCHC[] PROGMEM = "HCHC";
const char char_HCHP[] PROGMEM = "HCHP";
const char char_EJPHN[] PROGMEM = "EJPHN";
const char char_EJPHPM[] PROGMEM = "EJPHPM";
const char char_BBRHCJB[] PROGMEM = "BBRHCJB";
const char char_BBRHPJB[] PROGMEM = "BBRHPJB";
const char char_BBRHCJW[] PROGMEM = "BBRHCJW";
const char char_BBRHPJW[] PROGMEM = "BBRHPJW";
const char char_BBRHCJR[] PROGMEM = "BBRHCJR";
const char char_BBRHPJR[] PROGMEM = "BBRHPJR";
const char char_PEJP[] PROGMEM = "PEJP";
const char char_PTEC[] PROGMEM = "PTEC";
const char char_DEMAIN[] PROGMEM = "DEMAIN";
const char char_IINST[] PROGMEM = "IINST";
const char char_ADPS[] PROGMEM = "ADPS";
const char char_IMAX[] PROGMEM = "IMAX";
const char char_PAPP[] PROGMEM = "PAPP";
const char char_HHPHC[] PROGMEM = "HHPHC";
const char char_MOTDETAT[] PROGMEM = "MOTDETAT";
const char char_IINST1[] PROGMEM = "IINST1";
const char char_IINST2[] PROGMEM = "IINST2";
const char char_IINST3[] PROGMEM = "IINST3";
const char char_IMAX1[] PROGMEM = "IMAX1";
const char char_IMAX2[] PROGMEM = "IMAX2";
const char char_IMAX3[] PROGMEM = "IMAX3";
const char char_PMAX[] PROGMEM = "PMAX";
const char char_PPOT[] PROGMEM = "PPOT";
const char char_ADIR1[] PROGMEM = "ADIR1";
const char char_ADIR2[] PROGMEM = "ADIR2";
const char char_ADIR3[] PROGMEM = "ADIR3";

#define CHILD_ID_ADCO 1
#define CHILD_ID_OPTARIF 2
#define CHILD_ID_ISOUSC 3
#define CHILD_ID_BASE 4
#define CHILD_ID_HCHC 5
#define CHILD_ID_HCHP 6
#define CHILD_ID_EJPHN 7
#define CHILD_ID_EJPHPM 8
#define CHILD_ID_BBRHCJB 9
#define CHILD_ID_BBRHPJB 10
#define CHILD_ID_BBRHCJW 11
#define CHILD_ID_BBRHPJW 12
#define CHILD_ID_BBRHCJR 13
#define CHILD_ID_BBRHPJR 14
#define CHILD_ID_PEJP 15
#define CHILD_ID_PTEC 16
#define CHILD_ID_DEMAIN 17
#define CHILD_ID_IINST 18
#define CHILD_ID_ADPS 19
#define CHILD_ID_IMAX 20
#define CHILD_ID_PAPP 21
#define CHILD_ID_HHPHC 22
#define CHILD_ID_MOTDETAT 23
#define CHILD_ID_IINST1 24
#define CHILD_ID_IINST2 25
#define CHILD_ID_IINST3 26
#define CHILD_ID_IMAX1 27
#define CHILD_ID_IMAX2 28
#define CHILD_ID_IMAX3 29
#define CHILD_ID_PMAX 30
#define CHILD_ID_PPOT 31
#define CHILD_ID_ADIR1 32
#define CHILD_ID_ADIR2 33
#define CHILD_ID_ADIR3 34


// Mode Standard --------------
const char char_ADSC[] PROGMEM = "ADSC";
const char char_VTIC[] PROGMEM = "VTIC";
const char char_DATE[] PROGMEM = "DATE";
const char char_NGTF[] PROGMEM = "NGTF";
const char char_LTARF[] PROGMEM = "LTARF";
const char char_EAST[] PROGMEM = "EAST";
const char char_EASF01[] PROGMEM = "EASF01";
const char char_EASF02[] PROGMEM = "EASF02";
const char char_EASF03[] PROGMEM = "EASF03";
const char char_EASF04[] PROGMEM = "EASF04";
const char char_EASF05[] PROGMEM = "EASF05";
const char char_EASF06[] PROGMEM = "EASF06";
const char char_EASF07[] PROGMEM = "EASF07";
const char char_EASF08[] PROGMEM = "EASF08";
const char char_EASF09[] PROGMEM = "EASF09";
const char char_EASF10[] PROGMEM = "EASF10";
const char char_EASD01[] PROGMEM = "EASD01";
const char char_EASD02[] PROGMEM = "EASD02";
const char char_EASD03[] PROGMEM = "EASD03";
const char char_EASD04[] PROGMEM = "EASD04";
const char char_EAIT[] PROGMEM = "EAIT";
const char char_ERQ1[] PROGMEM = "ERQ1";
const char char_ERQ2[] PROGMEM = "ERQ2";
const char char_ERQ3[] PROGMEM = "ERQ3";
const char char_ERQ4[] PROGMEM = "ERQ4";
const char char_IRMS1[] PROGMEM = "IRMS1";
const char char_IRMS2[] PROGMEM = "IRMS2";
const char char_IRMS3[] PROGMEM = "IRMS3";
const char char_URMS1[] PROGMEM = "URMS1";
const char char_URMS2[] PROGMEM = "URMS2";
const char char_URMS3[] PROGMEM = "URMS3";
const char char_PREF[] PROGMEM = "PREF";
const char char_PCOUP[] PROGMEM = "PCOUP";
const char char_SINSTS[] PROGMEM = "SINSTS";
const char char_SINSTS1[] PROGMEM = "SINSTS1";
const char char_SINSTS2[] PROGMEM = "SINSTS2";
const char char_SINSTS3[] PROGMEM = "SINSTS3";
const char char_SMAXSN[] PROGMEM = "SMAXSN";
const char char_SMAXSN1[] PROGMEM = "SMAXSN1";
const char char_SMAXSN2[] PROGMEM = "SMAXSN2";
const char char_SMAXSN3[] PROGMEM = "SMAXSN3";
const char char_SMAXSN_1[] PROGMEM = "SMAXSN-1";
const char char_SMAXSN1_1[] PROGMEM = "SMAXSN1-1";
const char char_SMAXSN2_1[] PROGMEM = "SMAXSN2-1";
const char char_SMAXSN3_1[] PROGMEM = "SMAXSN3-1";
const char char_SINSTI[] PROGMEM = "SINSTI";
const char char_SMAXIN[] PROGMEM = "SMAXIN";
const char char_SMAXIN_1[] PROGMEM = "SMAXIN-1";
const char char_CCASN[] PROGMEM = "CCASN";
const char char_CCASN_1[] PROGMEM = "CCASN-1";
const char char_CCAIN[] PROGMEM = "CCAIN";
const char char_CCAIN_1[] PROGMEM = "CCAIN-1";
const char char_UMOY1[] PROGMEM = "UMOY1";
const char char_UMOY2[] PROGMEM = "UMOY2";
const char char_UMOY3[] PROGMEM = "UMOY3";
const char char_STGE[] PROGMEM = "STGE";
const char char_DPM1[] PROGMEM = "DPM1";
const char char_DPM2[] PROGMEM = "DPM2";
const char char_DPM3[] PROGMEM = "DPM3";
const char char_FPM1[] PROGMEM = "FPM1";
const char char_FPM2[] PROGMEM = "FPM2";
const char char_FPM3[] PROGMEM = "FPM3";
const char char_MSG1[] PROGMEM = "MSG1";
const char char_MSG2[] PROGMEM = "MSG2";
const char char_PRM[] PROGMEM = "PRM";
const char char_RELAIS[] PROGMEM = "RELAIS";
const char char_NTARF[] PROGMEM = "NTARF";
const char char_NJOURF[] PROGMEM = "NJOURF";
const char char_NJOURF_1[] PROGMEM = "NJOURF+1";
const char char_PJOURF_1[] PROGMEM = "PJOURF+1";
const char char_PPOINTE[] PROGMEM = "PPOINTE";

#define CHILD_ID_ADSC     1
#define CHILD_ID_VTIC     2
#define CHILD_ID_DATE     3
#define CHILD_ID_NGTF     4
#define CHILD_ID_LTARF     5
#define CHILD_ID_EAST     6
#define CHILD_ID_EASF01     7
#define CHILD_ID_EASF02     8
#define CHILD_ID_EASF03     9
#define CHILD_ID_EASF04     10
#define CHILD_ID_EASF05     11
#define CHILD_ID_EASF06     12
#define CHILD_ID_EASF07     13
#define CHILD_ID_EASF08     14
#define CHILD_ID_EASF09     15
#define CHILD_ID_EASF10     16
#define CHILD_ID_EASD01     17
#define CHILD_ID_EASD02     18
#define CHILD_ID_EASD03     19
#define CHILD_ID_EASD04     20
#define CHILD_ID_EAIT     21
#define CHILD_ID_ERQ1     22
#define CHILD_ID_ERQ2     23
#define CHILD_ID_ERQ3     24
#define CHILD_ID_ERQ4     25
#define CHILD_ID_IRMS1     26
#define CHILD_ID_IRMS2     27
#define CHILD_ID_IRMS3     28
#define CHILD_ID_URMS1     29
#define CHILD_ID_URMS2     30
#define CHILD_ID_URMS3     31
#define CHILD_ID_PREF     32
#define CHILD_ID_PCOUP     33
#define CHILD_ID_SINSTS     34
#define CHILD_ID_SINSTS1     35
#define CHILD_ID_SINSTS2     36
#define CHILD_ID_SINSTS3     37
#define CHILD_ID_SMAXSN     38
#define CHILD_ID_SMAXSN1     39
#define CHILD_ID_SMAXSN2     40
#define CHILD_ID_SMAXSN3     41
#define CHILD_ID_SMAXSN_1     42
#define CHILD_ID_SMAXSN1_1     43
#define CHILD_ID_SMAXSN2_1     44
#define CHILD_ID_SMAXSN3_1     45
#define CHILD_ID_SINSTI     46
#define CHILD_ID_SMAXIN     47
#define CHILD_ID_SMAXIN_1     48
#define CHILD_ID_CCASN     49
#define CHILD_ID_CCASN_1     50
#define CHILD_ID_CCAIN     51
#define CHILD_ID_CCAIN_1     52
#define CHILD_ID_UMOY1     53
#define CHILD_ID_UMOY2     54
#define CHILD_ID_UMOY3     55
#define CHILD_ID_STGE     56
#define CHILD_ID_DPM1     57
#define CHILD_ID_DPM2     58
#define CHILD_ID_DPM3     59
#define CHILD_ID_FPM1     60
#define CHILD_ID_FPM2     61
#define CHILD_ID_FPM3     62
#define CHILD_ID_MSG1     63
#define CHILD_ID_MSG2     64
#define CHILD_ID_PRM     65
#define CHILD_ID_RELAIS     66
#define CHILD_ID_NTARF     67
#define CHILD_ID_NJOURF     68
#define CHILD_ID_NJOURF_1     69
#define CHILD_ID_PJOURF_1     70
#define CHILD_ID_PPOINTE     71


const char char_START[] PROGMEM = "START";
#define CHILD_ID_START    99

MyMessage msgTEXT( 0, V_TEXT);        // S_INFO
MyMessage msgCURRENT( 0, V_CURRENT ); // S_MULTIMETER
MyMessage msgVOLTAGE( 0, V_VOLTAGE ); // S_MULTIMETER
MyMessage msgWATT( 0, V_WATT ); // S_POWER
MyMessage msgKWH( 0, V_KWH );   // S_POWER
MyMessage msgVA( 0, V_VA );     // S_POWER
MyMessage msgPrefix( 0, V_UNIT_PREFIX );


/*
// ---------------------------------------------------------------- 
// freeMemory
// ---------------------------------------------------------------- 
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

// ---------------------------------------------------------------- 
// freeMemory
// ---------------------------------------------------------------- 
void affiche_freeMemory() {
  Serial.print(F("SRAM:"));
  Serial.println(freeMemory());
}
*/


// ---------------------------------------------------------------- 
// change_etat_led_teleinfo
// ---------------------------------------------------------------- 
void clignote_led(uint8_t led, uint8_t nbr, int16_t delais)
{
int led_state;

  for (int i=0; i<nbr*2; i++) {
    led_state = !led_state;
    digitalWrite(led, led_state);
    delay(delais);
  }
  digitalWrite(led, HIGH);
}


// ---------------------------------------------------------------- 
// send_teleinfo_historique
//    Envoi trame de teleinfo historique
// ---------------------------------------------------------------- 
void send_teleinfo_historique(char *name, char *value)
{
  if (strcmp_P(name, char_ADCO) == 0) {send(msgTEXT.setSensor(CHILD_ID_ADCO).set(value));return;}
  if (strcmp_P(name, char_OPTARIF) == 0) {send(msgTEXT.setSensor(CHILD_ID_OPTARIF).set(value));return;}
  if (strcmp_P(name, char_ISOUSC) == 0) {send(msgCURRENT.setSensor(CHILD_ID_ISOUSC).set(atol(value)));return;}
  if (strcmp_P(name, char_BASE) == 0) {send(msgKWH.setSensor(CHILD_ID_BASE).set(atol(value)));return;}
  if (strcmp_P(name, char_HCHC) == 0) {send(msgKWH.setSensor(CHILD_ID_HCHC).set(atol(value)));return;}
  if (strcmp_P(name, char_HCHP) == 0) {send(msgKWH.setSensor(CHILD_ID_HCHP).set(atol(value)));return;}
  if (strcmp_P(name, char_EJPHN) == 0) {send(msgKWH.setSensor(CHILD_ID_EJPHN).set(atol(value)));return;}
  if (strcmp_P(name, char_EJPHPM) == 0) {send(msgKWH.setSensor(CHILD_ID_EJPHPM).set(atol(value)));return;}
  if (strcmp_P(name, char_BBRHCJB) == 0) {send(msgKWH.setSensor(CHILD_ID_BBRHCJB).set(atol(value)));return;}
  if (strcmp_P(name, char_BBRHPJB) == 0) {send(msgKWH.setSensor(CHILD_ID_BBRHPJB).set(atol(value)));return;}
  if (strcmp_P(name, char_BBRHCJW) == 0) {send(msgKWH.setSensor(CHILD_ID_BBRHCJW).set(atol(value)));return;}
  if (strcmp_P(name, char_BBRHPJW) == 0) {send(msgKWH.setSensor(CHILD_ID_BBRHPJW).set(atol(value)));return;}
  if (strcmp_P(name, char_BBRHCJR) == 0) {send(msgKWH.setSensor(CHILD_ID_BBRHCJR).set(atol(value)));return;}
  if (strcmp_P(name, char_BBRHPJR) == 0) {send(msgKWH.setSensor(CHILD_ID_BBRHPJR).set(atol(value)));return;}
  if (strcmp_P(name, char_PEJP) == 0) {send(msgTEXT.setSensor(CHILD_ID_PEJP).set(value));return;}
  if (strcmp_P(name, char_PTEC) == 0) {send(msgTEXT.setSensor(CHILD_ID_PTEC).set(value));return;}
  if (strcmp_P(name, char_DEMAIN) == 0) {send(msgTEXT.setSensor(CHILD_ID_DEMAIN).set(value));return;}
  if (strcmp_P(name, char_IINST) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IINST).set(atol(value)));return;}
  if (strcmp_P(name, char_ADPS) == 0) {send(msgCURRENT.setSensor(CHILD_ID_ADPS).set(atol(value)));return;}
  if (strcmp_P(name, char_IMAX) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IMAX).set(atol(value)));return;}
  if (strcmp_P(name, char_PAPP) == 0) {send(msgVA.setSensor(CHILD_ID_PAPP).set(atol(value)));return;}
  if (strcmp_P(name, char_HHPHC) == 0) {send(msgTEXT.setSensor(CHILD_ID_HHPHC).set(value));return;}
  if (strcmp_P(name, char_MOTDETAT) == 0) {send(msgTEXT.setSensor(CHILD_ID_MOTDETAT).set(value));return;}
  if (strcmp_P(name, char_IINST1) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IINST1).set(atol(value)));return;}
  if (strcmp_P(name, char_IINST2) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IINST2).set(atol(value)));return;}
  if (strcmp_P(name, char_IINST3) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IINST3).set(atol(value)));return;}
  if (strcmp_P(name, char_IMAX1) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IMAX1).set(atol(value)));return;}
  if (strcmp_P(name, char_IMAX2) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IMAX2).set(atol(value)));return;}
  if (strcmp_P(name, char_IMAX3) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IMAX3).set(atol(value)));return;}
  if (strcmp_P(name, char_PMAX) == 0) {send(msgWATT.setSensor(CHILD_ID_PMAX).set(atol(value)));return;}
  if (strcmp_P(name, char_PPOT) == 0) {send(msgTEXT.setSensor(CHILD_ID_PPOT).set(atol(value)));return;}
  if (strcmp_P(name, char_ADIR1) == 0) {send(msgCURRENT.setSensor(CHILD_ID_ADIR1).set(atol(value)));return;}
  if (strcmp_P(name, char_ADIR2) == 0) {send(msgCURRENT.setSensor(CHILD_ID_ADIR2).set(atol(value)));return;}
  if (strcmp_P(name, char_ADIR3) == 0) {send(msgCURRENT.setSensor(CHILD_ID_ADIR3).set(atol(value)));return;}
}

// ---------------------------------------------------------------- 
// send_teleinfo_standard
//    Envoi trame de teleinfo standard
// ---------------------------------------------------------------- 
void send_teleinfo_standard(char *name, char *value)
{
  if (strcmp_P(name, char_ADSC) == 0) {send(msgTEXT.setSensor(CHILD_ID_ADSC).set(value));return;}
  if (strcmp_P(name, char_VTIC) == 0) {send(msgTEXT.setSensor(CHILD_ID_VTIC).set(value));return;}
  if (strcmp_P(name, char_DATE) == 0) {send(msgTEXT.setSensor(CHILD_ID_DATE).set(value));return;}
  if (strcmp_P(name, char_NGTF) == 0) {send(msgTEXT.setSensor(CHILD_ID_NGTF).set(value));return;}
  if (strcmp_P(name, char_LTARF) == 0) {send(msgTEXT.setSensor(CHILD_ID_LTARF).set(value));return;}
  if (strcmp_P(name, char_EAST) == 0) {send(msgKWH.setSensor(CHILD_ID_EAST).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF01) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF01).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF02) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF02).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF03) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF03).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF04) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF04).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF05) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF05).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF06) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF06).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF07) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF07).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF08) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF08).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF09) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF09).set(atol(value)));return;}
  if (strcmp_P(name, char_EASF10) == 0) {send(msgKWH.setSensor(CHILD_ID_EASF10).set(atol(value)));return;}
  if (strcmp_P(name, char_EASD01) == 0) {send(msgKWH.setSensor(CHILD_ID_EASD01).set(atol(value)));return;}
  if (strcmp_P(name, char_EASD02) == 0) {send(msgKWH.setSensor(CHILD_ID_EASD02).set(atol(value)));return;}
  if (strcmp_P(name, char_EASD03) == 0) {send(msgKWH.setSensor(CHILD_ID_EASD03).set(atol(value)));return;}
  if (strcmp_P(name, char_EASD04) == 0) {send(msgKWH.setSensor(CHILD_ID_EASD04).set(atol(value)));return;}
  if (strcmp_P(name, char_EAIT) == 0) {send(msgKWH.setSensor(CHILD_ID_EAIT).set(atol(value)));return;}
  if (strcmp_P(name, char_ERQ1) == 0) {send(msgKWH.setSensor(CHILD_ID_ERQ1).set(atol(value)));return;}
  if (strcmp_P(name, char_ERQ2) == 0) {send(msgKWH.setSensor(CHILD_ID_ERQ2).set(atol(value)));return;}
  if (strcmp_P(name, char_ERQ3) == 0) {send(msgKWH.setSensor(CHILD_ID_ERQ3).set(atol(value)));return;}
  if (strcmp_P(name, char_ERQ4) == 0) {send(msgKWH.setSensor(CHILD_ID_ERQ4).set(atol(value)));return;}
  if (strcmp_P(name, char_IRMS1) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IRMS1).set(atol(value)));return;}
  if (strcmp_P(name, char_IRMS2) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IRMS2).set(atol(value)));return;}
  if (strcmp_P(name, char_IRMS3) == 0) {send(msgCURRENT.setSensor(CHILD_ID_IRMS3).set(atol(value)));return;}
  if (strcmp_P(name, char_URMS1) == 0) {send(msgVOLTAGE.setSensor(CHILD_ID_URMS1).set(atol(value)));return;}
  if (strcmp_P(name, char_URMS2) == 0) {send(msgVOLTAGE.setSensor(CHILD_ID_URMS2).set(atol(value)));return;}
  if (strcmp_P(name, char_URMS3) == 0) {send(msgVOLTAGE.setSensor(CHILD_ID_URMS3).set(atol(value)));return;}
  if (strcmp_P(name, char_PREF) == 0) {send(msgVA.setSensor(CHILD_ID_PREF).set(atol(value)));return;}
  if (strcmp_P(name, char_PCOUP) == 0) {send(msgVA.setSensor(CHILD_ID_PCOUP).set(atol(value)));return;}
  if (strcmp_P(name, char_SINSTS) == 0) {send(msgVA.setSensor(CHILD_ID_SINSTS).set(atol(value)));return;}
  if (strcmp_P(name, char_SINSTS1) == 0) {send(msgVA.setSensor(CHILD_ID_SINSTS1).set(atol(value)));return;}
  if (strcmp_P(name, char_SINSTS2) == 0) {send(msgVA.setSensor(CHILD_ID_SINSTS2).set(atol(value)));return;}
  if (strcmp_P(name, char_SINSTS3) == 0) {send(msgVA.setSensor(CHILD_ID_SINSTS3).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXSN) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXSN).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXSN1) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXSN1).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXSN2) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXSN2).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXSN3) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXSN3).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXSN_1) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXSN_1).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXSN1_1) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXSN1_1).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXSN2_1) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXSN2_1).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXSN3_1) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXSN3_1).set(atol(value)));return;}
  if (strcmp_P(name, char_SINSTI) == 0) {send(msgVA.setSensor(CHILD_ID_SINSTI).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXIN) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXIN).set(atol(value)));return;}
  if (strcmp_P(name, char_SMAXIN_1) == 0) {send(msgVA.setSensor(CHILD_ID_SMAXIN_1).set(atol(value)));return;}
  if (strcmp_P(name, char_CCASN) == 0) {send(msgWATT.setSensor(CHILD_ID_CCASN).set(atol(value)));return;}
  if (strcmp_P(name, char_CCASN_1) == 0) {send(msgWATT.setSensor(CHILD_ID_CCASN_1).set(atol(value)));return;}
  if (strcmp_P(name, char_CCAIN) == 0) {send(msgWATT.setSensor(CHILD_ID_CCAIN).set(atol(value)));return;}
  if (strcmp_P(name, char_CCAIN_1) == 0) {send(msgWATT.setSensor(CHILD_ID_CCAIN_1).set(atol(value)));return;}
  if (strcmp_P(name, char_UMOY1) == 0) {send(msgVOLTAGE.setSensor(CHILD_ID_UMOY1).set(atol(value)));return;}
  if (strcmp_P(name, char_UMOY2) == 0) {send(msgVOLTAGE.setSensor(CHILD_ID_UMOY2).set(atol(value)));return;}
  if (strcmp_P(name, char_UMOY3) == 0) {send(msgVOLTAGE.setSensor(CHILD_ID_UMOY3).set(atol(value)));return;}
  if (strcmp_P(name, char_STGE) == 0) {send(msgTEXT.setSensor(CHILD_ID_STGE).set(value));return;}
  if (strcmp_P(name, char_DPM1) == 0) {send(msgTEXT.setSensor(CHILD_ID_DPM1).set(value));return;}
  if (strcmp_P(name, char_DPM2) == 0) {send(msgTEXT.setSensor(CHILD_ID_DPM2).set(value));return;}
  if (strcmp_P(name, char_DPM3) == 0) {send(msgTEXT.setSensor(CHILD_ID_DPM3).set(value));return;}
  if (strcmp_P(name, char_FPM1) == 0) {send(msgTEXT.setSensor(CHILD_ID_FPM1).set(value));return;}
  if (strcmp_P(name, char_FPM2) == 0) {send(msgTEXT.setSensor(CHILD_ID_FPM2).set(value));return;}
  if (strcmp_P(name, char_FPM3) == 0) {send(msgTEXT.setSensor(CHILD_ID_FPM3).set(value));return;}
  if (strcmp_P(name, char_MSG1) == 0) {send(msgTEXT.setSensor(CHILD_ID_MSG1).set(value));return;}
  if (strcmp_P(name, char_MSG2) == 0) {send(msgTEXT.setSensor(CHILD_ID_MSG2).set(value));return;}
  if (strcmp_P(name, char_PRM) == 0) {send(msgTEXT.setSensor(CHILD_ID_PRM).set(value));return;}
  if (strcmp_P(name, char_RELAIS) == 0) {send(msgTEXT.setSensor(CHILD_ID_RELAIS).set(value));return;}
  if (strcmp_P(name, char_NTARF) == 0) {send(msgTEXT.setSensor(CHILD_ID_NTARF).set(value));return;}
  if (strcmp_P(name, char_NJOURF) == 0) {send(msgTEXT.setSensor(CHILD_ID_NJOURF).set(value));return;}
  if (strcmp_P(name, char_NJOURF_1) == 0) {send(msgTEXT.setSensor(CHILD_ID_NJOURF_1).set(value));return;}
  if (strcmp_P(name, char_PJOURF_1) == 0) {send(msgTEXT.setSensor(CHILD_ID_PJOURF_1).set(value));return;}
  if (strcmp_P(name, char_PPOINTE) == 0) {send(msgTEXT.setSensor(CHILD_ID_PPOINTE).set(value));return;}
}

// ---------------------------------------------------------------- 
// send_teleinfo
//    Envoi trame de teleinfo
// ---------------------------------------------------------------- 
void send_teleinfo(char *etiq, char *val)
{
  
   if (mode_tic == TINFO_MODE_HISTORIQUE) {
      send_teleinfo_historique(etiq, val);
   }
   else {
      send_teleinfo_standard(etiq, val);
   }
}

// ---------------------------------------------------------------- 
// init_speed_TIC
// ---------------------------------------------------------------- 
_Mode_e init_speed_TIC()
{
boolean flag_timeout = false;
boolean flag_found_speed = false;
uint32_t currentTime = millis();
unsigned step = 0;
_Mode_e mode;

  digitalWrite(MY_DEFAULT_ERR_LED_PIN, LOW);
  digitalWrite(MY_DEFAULT_RX_LED_PIN, LOW);
  
  // Test en mode historique
  // Recherche des éléments de début, milieu et fin de trame (0x0A, 0x20, 0x20, 0x0D)
  Serial.begin(1200); // mode historique
  while (!flag_timeout && !flag_found_speed) {
    if (Serial.available()>0) {
      char in = (char)Serial.read() & 127;  // seulement sur 7 bits
      // début trame
        if (in == 0x0A) {
        step = 1;
      }
      // premier milieu de trame
        if (step == 1 && in == 0x20) {
        step = 2;
      }
      // deuxième milieu de trame
        if (step == 2 && in == 0x20) {
        step = 3;
      }
      // fin trame
        if (step == 3 && in == 0x0D) {
        flag_found_speed = true;
        step = 0;
      }
    }
    if (currentTime + 10000 <  millis()) flag_timeout = true; // 10s de timeout
  }

  if (flag_timeout) { // trame avec vistesse histo non trouvée donc passage en mode standard
     mode = TINFO_MODE_STANDARD;
     Serial.end();
     Serial.begin(9600); // mode standard
     Serial.println(F("TIC mode standard"));
     clignote_led(MY_DEFAULT_RX_LED_PIN, 3, 500);
  }
  else {
    mode = TINFO_MODE_HISTORIQUE;
    Serial.println(F("TIC mode historique"));
    clignote_led(MY_DEFAULT_RX_LED_PIN, 5, 500);
  }
  
  digitalWrite(MY_DEFAULT_ERR_LED_PIN, HIGH);

  return mode;
}


/* ======================================================================
Function: NewFrame 
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : - 
Comments: it's called only if one data in the frame is different than
          the previous frame
====================================================================== */
/*
void UpdatedFrame(ValueList *vl_tic)
{

  //affiche_freeMemory();
  send_teleinfo(vl_tic, flag_full_tic);
    
  if (flag_full_tic) {
    send(msgTEXT.setSensor(CHILD_ID_START).set(startTime));
    flag_full_tic = false;
  }

}
*/

//-----------------------------------------------------------------------
// This is called when a new time value was received
void receiveTime(unsigned long controllerTime) 
{
  // incoming time 
  Serial.print(F(">> Time from gw: "));
  Serial.println(controllerTime);
  startTime = controllerTime;
}


//--------------------------------------------------------------------
void before()
{
  pinMode(MY_DEFAULT_ERR_LED_PIN, OUTPUT);
  pinMode(MY_DEFAULT_RX_LED_PIN, OUTPUT);

  #ifdef CARTE_SWITCH
  byte val_switch=0;
  pinMode(SWITCH_1, INPUT_PULLUP);
  pinMode(SWITCH_2, INPUT_PULLUP);
  pinMode(SWITCH_3, INPUT_PULLUP);
  pinMode(SWITCH_4, INPUT_PULLUP);

  for (int i=0; i<4; i++) {
    bitWrite(val_switch, i, !digitalRead(i+SWITCH_1));
  }
  if (val_switch >0) {
    Serial.print(F("Force NodeID="));
    Serial.println(val_switch);
    myNodeId=val_switch;
  }
  else {
    Serial.println(F("NodeID=AUTO"));
    myNodeId = AUTO;
  }
  #endif
    
  mode_tic = init_speed_TIC();
    
  Serial.println(F("   __|              _/           _ )  |"));
  Serial.println(F("   _| |  |   ` \\    -_)   -_)    _ \\  |   -_)  |  |   -_)"));
  Serial.println(F("  _| \\_,_| _|_|_| \\___| \\___|   ___/ _| \\___| \\_,_| \\___|"));
  Serial.println(VERSION);

  tinfo.init(mode_tic);
}

//--------------------------------------------------------------------
void setup()
{
  //affiche_freeMemory();
}

//--------------------------------------------------------------------
void presentation()
{

  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Teleinfo", VERSION);
  
  // Récupere l'heure de la gateway
  requestTime();

  if (mode_tic == TINFO_MODE_HISTORIQUE) {
    present (CHILD_ID_ADCO, S_INFO, F("ADCO"));
    present (CHILD_ID_OPTARIF, S_INFO, F("OPTARIF"));
    present (CHILD_ID_ISOUSC, S_MULTIMETER, F("ISOUSC"));
    present (CHILD_ID_BASE, S_POWER, F("BASE"));
    present (CHILD_ID_HCHC, S_POWER, F("HCHC"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_HCHP, S_POWER, F("HCHP"));
    present (CHILD_ID_EJPHN, S_POWER, F("EJPHN"));
    present (CHILD_ID_EJPHPM, S_POWER, F("EJPHPM"));
    present (CHILD_ID_BBRHCJB, S_POWER, F("BBRHCJB"));
    present (CHILD_ID_BBRHPJB, S_POWER, F("BBRHPJB"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_BBRHCJW, S_POWER, F("BBRHCJW"));
    present (CHILD_ID_BBRHPJW, S_POWER, F("BBRHPJW"));
    present (CHILD_ID_BBRHCJR, S_POWER, F("BBRHCJR"));
    present (CHILD_ID_BBRHPJR, S_POWER, F("BBRHPJR"));
    present (CHILD_ID_PEJP, S_INFO, F("PEJP"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_PTEC, S_INFO, F("PTEC"));
    present (CHILD_ID_DEMAIN, S_INFO, F("DEMAIN"));
    present (CHILD_ID_IINST, S_MULTIMETER, F("IINST"));
    present (CHILD_ID_ADPS, S_MULTIMETER, F("ADPS"));
    present (CHILD_ID_IMAX, S_MULTIMETER, F("IMAX"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_PAPP, S_POWER, F("PAPP"));
    present (CHILD_ID_HHPHC, S_INFO, F("HHPHC"));
    present (CHILD_ID_MOTDETAT, S_INFO, F("MOTDETAT"));
    present (CHILD_ID_IINST1, S_MULTIMETER, F("IINST1"));
    present (CHILD_ID_IINST2, S_MULTIMETER, F("IINST2"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_IINST3, S_MULTIMETER, F("IINST3"));
    present (CHILD_ID_IMAX1, S_MULTIMETER, F("IMAX1"));
    present (CHILD_ID_IMAX2, S_MULTIMETER, F("IMAX2"));
    present (CHILD_ID_IMAX3, S_MULTIMETER, F("IMAX3"));
    present (CHILD_ID_PMAX, S_POWER, F("PMAX"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_PPOT, S_INFO, F("PPOT"));
    present (CHILD_ID_ADIR1, S_MULTIMETER, F("ADIR1"));
    present (CHILD_ID_ADIR2, S_MULTIMETER, F("ADIR2"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_ADIR3, S_MULTIMETER, F("ADIR3"));
  }
  else {
    present (CHILD_ID_ADSC, S_INFO, F("ADSC"));
    present (CHILD_ID_VTIC, S_INFO, F("VTIC"));
    present (CHILD_ID_DATE, S_INFO, F("DATE"));
    present (CHILD_ID_NGTF, S_INFO, F("NGTF"));
    present (CHILD_ID_LTARF, S_INFO, F("LTARF"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_EAST, S_POWER, F("EAST"));
    present (CHILD_ID_EASF01, S_POWER, F("EASF01"));
    present (CHILD_ID_EASF02, S_POWER, F("EASF02"));
    present (CHILD_ID_EASF03, S_POWER, F("EASF03"));
    present (CHILD_ID_EASF04, S_POWER, F("EASF04"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_EASF05, S_POWER, F("EASF05"));
    present (CHILD_ID_EASF06, S_POWER, F("EASF06"));
    present (CHILD_ID_EASF07, S_POWER, F("EASF07"));
    present (CHILD_ID_EASF08, S_POWER, F("EASF08"));
    present (CHILD_ID_EASF09, S_POWER, F("EASF09"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_EASF10, S_POWER, F("EASF10"));
    present (CHILD_ID_EASD01, S_POWER, F("EASD01"));
    present (CHILD_ID_EASD02, S_POWER, F("EASD02"));
    present (CHILD_ID_EASD03, S_POWER, F("EASD03"));
    present (CHILD_ID_EASD04, S_POWER, F("EASD04"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_EAIT, S_POWER, F("EAIT"));
    present (CHILD_ID_ERQ1, S_POWER, F("ERQ1"));
    present (CHILD_ID_ERQ2, S_POWER, F("ERQ2"));
    present (CHILD_ID_ERQ3, S_POWER, F("ERQ3"));
    present (CHILD_ID_ERQ4, S_POWER, F("ERQ4"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_IRMS1, S_MULTIMETER, F("IRMS1"));
    present (CHILD_ID_IRMS2, S_MULTIMETER, F("IRMS2"));
    present (CHILD_ID_IRMS3, S_MULTIMETER, F("IRMS3"));
    present (CHILD_ID_URMS1, S_MULTIMETER, F("URMS1"));
    present (CHILD_ID_URMS2, S_MULTIMETER, F("URMS2"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_URMS3, S_MULTIMETER, F("URMS3"));
    present (CHILD_ID_PREF, S_POWER, F("PREF"));
    present (CHILD_ID_PCOUP, S_POWER, F("PCOUP"));
    present (CHILD_ID_SINSTS, S_POWER, F("SINSTS"));
    present (CHILD_ID_SINSTS1, S_POWER, F("SINSTS1"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_SINSTS2, S_POWER, F("SINSTS2"));
    present (CHILD_ID_SINSTS3, S_POWER, F("SINSTS3"));
    present (CHILD_ID_SMAXSN, S_POWER, F("SMAXSN"));
    present (CHILD_ID_SMAXSN1, S_POWER, F("SMAXSN1"));
    present (CHILD_ID_SMAXSN2, S_POWER, F("SMAXSN2"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_SMAXSN3, S_POWER, F("SMAXSN3"));
    present (CHILD_ID_SMAXSN_1, S_POWER, F("SMAXSN-1"));
    present (CHILD_ID_SMAXSN1_1, S_POWER, F("SMAXSN1-1"));
    present (CHILD_ID_SMAXSN2_1, S_POWER, F("SMAXSN2-1"));
    present (CHILD_ID_SMAXSN3_1, S_POWER, F("SMAXSN3-1"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_SINSTI, S_POWER, F("SINSTI"));
    present (CHILD_ID_SMAXIN, S_POWER, F("SMAXIN"));
    present (CHILD_ID_SMAXIN_1, S_POWER, F("SMAXIN-1"));
    present (CHILD_ID_CCASN, S_POWER, F("CCASN"));
    present (CHILD_ID_CCASN_1, S_POWER, F("CCASN-1"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_CCAIN, S_POWER, F("CCAIN"));
    present (CHILD_ID_CCAIN_1, S_POWER, F("CCAIN-1"));
    present (CHILD_ID_UMOY1, S_MULTIMETER, F("UMOY1"));
    present (CHILD_ID_UMOY2, S_MULTIMETER, F("UMOY2"));
    present (CHILD_ID_UMOY3, S_MULTIMETER, F("UMOY3"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_STGE, S_INFO, F("STGE"));
    present (CHILD_ID_DPM1, S_INFO, F("DPM1"));
    present (CHILD_ID_DPM2, S_INFO, F("DPM2"));
    present (CHILD_ID_DPM3, S_INFO, F("DPM3"));
    present (CHILD_ID_FPM1, S_INFO, F("FPM1"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_FPM2, S_INFO, F("FPM2"));
    present (CHILD_ID_FPM3, S_INFO, F("FPM3"));
    present (CHILD_ID_MSG1, S_INFO, F("MSG1"));
    present (CHILD_ID_MSG2, S_INFO, F("MSG2"));
    present (CHILD_ID_PRM, S_INFO, F("PRM"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_RELAIS, S_INFO, F("RELAIS"));
    present (CHILD_ID_NTARF, S_INFO, F("NTARF"));
    present (CHILD_ID_NJOURF, S_INFO, F("NJOURF"));
    present (CHILD_ID_NJOURF_1, S_INFO, F("NJOURF+1"));
    present (CHILD_ID_PJOURF_1, S_INFO, F("PJOURF+1"));
    wait(DELAY_PRESENTATION);
    present (CHILD_ID_PPOINTE, S_INFO, F("PPOINTE"));
  }
  present( CHILD_ID_START, S_INFO, F("START"));

  //affiche_freeMemory();

}




//--------------------------------------------------------------------
void loop()
{
  //uint32_t currentTime = millis();
  
  if (flag_first) {
	// Send prefix au démarrage
    if (mode_tic == TINFO_MODE_HISTORIQUE) {
      send(msgPrefix.setSensor(CHILD_ID_ISOUSC).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_BASE).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_HCHC).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_HCHP).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EJPHN).set(char_WATT_HEURE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_EJPHPM).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_BBRHCJB).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_BBRHPJB).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_BBRHCJW).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_BBRHPJW).set(char_WATT_HEURE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_BBRHCJR).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_BBRHPJR).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_PEJP).set(char_MIN));
      send(msgPrefix.setSensor(CHILD_ID_IINST).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_ADPS).set(char_AMPERE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_IMAX).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_PAPP).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_IINST1).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_IINST2).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_IINST3).set(char_AMPERE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_IMAX1).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_IMAX2).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_IMAX3).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_PMAX).set(char_WATT));
      send(msgPrefix.setSensor(CHILD_ID_ADIR1).set(char_AMPERE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_ADIR2).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_ADIR3).set(char_AMPERE));
    }
    else {
      send(msgPrefix.setSensor(CHILD_ID_EAST).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASF01).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASF02).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASF03).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASF04).set(char_WATT_HEURE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_EASF05).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASF06).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASF07).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASF08).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASF09).set(char_WATT_HEURE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_EASF10).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASD01).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASD02).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASD03).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_EASD04).set(char_WATT_HEURE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_EAIT).set(char_WATT_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_ERQ1).set(char_VAR_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_ERQ2).set(char_VAR_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_ERQ3).set(char_VAR_HEURE));
      send(msgPrefix.setSensor(CHILD_ID_ERQ4).set(char_VAR_HEURE));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_IRMS1).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_IRMS2).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_IRMS3).set(char_AMPERE));
      send(msgPrefix.setSensor(CHILD_ID_URMS1).set(char_VOLT));
      send(msgPrefix.setSensor(CHILD_ID_URMS2).set(char_VOLT));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_URMS3).set(char_VOLT));
      send(msgPrefix.setSensor(CHILD_ID_PREF).set(char_KVA));
      send(msgPrefix.setSensor(CHILD_ID_PCOUP).set(char_KVA));
      send(msgPrefix.setSensor(CHILD_ID_SINSTS).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SINSTS1).set(char_VA));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_SINSTS2).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SINSTS3).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXSN).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXSN1).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXSN2).set(char_VA));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_SMAXSN3).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXSN_1).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXSN1_1).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXSN2_1).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXSN3_1).set(char_VA));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_SINSTI).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXIN).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_SMAXIN_1).set(char_VA));
      send(msgPrefix.setSensor(CHILD_ID_CCASN).set(char_WATT));
      send(msgPrefix.setSensor(CHILD_ID_CCASN_1).set(char_WATT));
      wait(DELAY_PREFIX);
      send(msgPrefix.setSensor(CHILD_ID_CCAIN).set(char_WATT));
      send(msgPrefix.setSensor(CHILD_ID_CCAIN_1).set(char_WATT));
      send(msgPrefix.setSensor(CHILD_ID_UMOY1).set(char_VOLT));
      send(msgPrefix.setSensor(CHILD_ID_UMOY2).set(char_VOLT));
      send(msgPrefix.setSensor(CHILD_ID_UMOY3).set(char_VOLT));
      wait(DELAY_PREFIX);
    }

    send(msgTEXT.setSensor(CHILD_ID_START).set(startTime));
      
    flag_first= false;
  }

  if (Serial.available()) tinfo.process(Serial.read());

}
