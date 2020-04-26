//-----------------------------------------------------------------------------------------------------------------
// Mode Standard - avec horodatage
//-----------------------------------------------------------------------------------------------------------------
// Chaque message composant la trame comprend :
//
// un caractere LF (code ASCII = 0A hexa)
// une etiquette de 4 a  8 caracteres
// un espace SP (09 hexa)
// l'horodatage de 13 caracteres  (example : H081225223518)
// un espace SP (09 hexa)
// la donnee de 0 a  98 caracteres
// un espace SP (09 hexa)
// un caractere de controle
// un caractere CR (0D hexa)

// Ces differents messages donnent les indications suivantes en fonction de l'abonnement souscrit :
//  Adresse secondaire du compteur, etiquette: ADSC (12 caracteres) 
//  Version de la TIC: VTIC (2 carateres)
//  Date et heure courante: DATE (0 caractère) 
//  Nom du calendrier tarifaire fournisseur: NGTF (16 carateres)
//  Libellé tarif fournisseur en cours: LTARF  (16 carateres)
//  Energie active soutirée totale: EAST (9 carateres)
//  Energie active soutirée Fournisseur, index 01..10: EASF01 .. EASF10 (9 carateres)
//  Energie active soutirée Distributeur, index 01..4: EASD01 .. EASD04 (9 carateres)
//  Energie active injectée totale : EAIT (9 carateres)
//  Energie réactive Q1..Q4 totale : ERQ1 .. ERQ4 (9 carateres)
//  Courant efficace, phase 1..3 : IRMS1 .. IRMS3 (3 carateres)
//  Tension efficace, phase 1..3 : URMS1 .. URMS3 (3 carateres)
//  Puissance app. de référence : PREF (2 carateres)
//  Puissance app. de coupure : PCOUP (2 carateres)
//  Puissance app. Instantanée soutirée : SINSTS (5 carateres)
//  Puissance app. Instantanée soutirée phase 1..3 : SINSTS1 .. SINSTS3 (5 carateres)
//  Puissance app. max. soutirée n : SMAXSN (5 carateres)
//  Puissance app. max. soutirée n phase 1..3 : SMAXSN1 .. SMAXSN3 (5 carateres)
//  Puissance app. max. soutirée n-1 : SMAXSN-1 (5 carateres)
//  Puissance app. max. soutirée n-1 phase 1..3 : SMAXSN1-1 .. SMAXSN3-1 (5 carateres)
//  Puissance app. Instantanée injectée : SINSTI (5 carateres)
//  Puissance app. max. injectée n : SMAXIN (5 carateres)
//  Puissance app. max. injectée n-1 : SMAXIN-1 (5 carateres)
//  Point n de la courbe de charge active soutirée : CCASN (5 carateres)
//  Point n-1 de la courbe de charge active soutirée : CCASN-1 (5 carateres)
//  Point n de la courbe de charge active injectée : CCAIN (5 carateres)
//  Point n-1 de la courbe de charge active injectée : CCAIN-1 (5 carateres)
//  Tension moy. ph. 1..3 : UMOY1 .. UMOY3 (3 carateres)
//  Registre de Statuts : STGE  (8 carateres)
//  Début Pointe Mobile 1..3 : DPM1 .. DPM3 (2 carateres)
//  Fin Pointe Mobile 1..3 : FPM1 .. FPM3 (2 carateres)
//  Message court : MSG1 (32 carateres)
//  Message Ultra court  : MSG2 (16 carateres)
//  PRM : PMR (14 carateres)
//  Relais : RELAIS (3 carateres)
//  Numéro de l’index tarifaire en cours : NTARF (2 carateres)
//  Numéro du jour en cours calendrier fournisseur : NJOURF (2 carateres)
//  Numéro du prochain jour calendrier fournisseur : NJOURF+1 (2 carateres)
//  Profil du prochain jour calendrier fournisseu : PJOURF+1 (98 carateres)
//  Profil du prochain jour de point : PPOINTE (98 carateres)
//-----------------------------------------------------------------------------------------------------------------

#define LG_TRAME_MAX  25
#define LG_TRAME_MIN  10
#define DEB_TRAME 0x0A
#define FIN_TRAME 0x0D

char buffin[LG_TRAME_MAX];
int  index_buff = 0;

/*
void change_etat_led_teleinfo()
{
static int led_state;
  
  led_state = !led_state;
  digitalWrite(LED_TELEINFO, led_state);
  
}

void change_etat_led_send()
{
static int led_state;
  
  led_state = !led_state;
  digitalWrite(LED_SEND, led_state);
  
}
*/

// Traitement trame teleinfo ------------------------------------------
void traitement_trame(char *buff)
{

    
    if (strncmp("ADCO ", &buff[1] , 5)==0) {
      strncpy(teleinfo.ADCO, &buff[6], 12);
      teleinfo.ADCO[12] = '\0';
      return;
    }
    if (strncmp("OPTARIF ", &buff[1] , 8)==0) {
      strncpy(teleinfo.OPTARIF, &buff[9], 4);
      teleinfo.OPTARIF[4] = '\0';
      return;
    }
    if (strncmp("ISOUSC ", &buff[1] , 7)==0) {
      teleinfo.ISOUSC = atoi(&buff[8]);
      return;
    }
    if (strncmp("BASE ", &buff[1] , 5)==0) {
      teleinfo.BASE = atol(&buff[6]);
      return;
    }
    if (strncmp("HCHC ", &buff[1] , 5)==0) {
      teleinfo.HCHC = atol(&buff[6]);
      return;
    }
    if (strncmp("HCHP ", &buff[1] , 5)==0) {
      teleinfo.HCHP = atol(&buff[6]);
      return;
    }
    if (strncmp("EJP HN ", &buff[1] , 7)==0) {
      teleinfo.EJP_HN = atol(&buff[8]);
      return;
    }
    if (strncmp("EJP HPM ", &buff[1] , 8)==0) {
      teleinfo.EJP_HPM = atol(&buff[9]);
      return;
    }
    if (strncmp("BBR HC JB ", &buff[1] , 10)==0) {
      teleinfo.BBR_HC_JB = atol(&buff[11]);
      return;
    }
    if (strncmp("BBR HP JB ", &buff[1] , 10)==0) {
      teleinfo.BBR_HP_JB = atol(&buff[11]);
      return;
    }
    if (strncmp("BBR HC JW ", &buff[1] , 10)==0) {
      teleinfo.BBR_HC_JW = atol(&buff[11]);
      return;
    }
    if (strncmp("BBR HP JW ", &buff[1] , 10)==0) {
      teleinfo.BBR_HP_JW = atol(&buff[11]);
      return;
    }
    if (strncmp("BBR HC JR ", &buff[1] , 10)==0) {
      teleinfo.BBR_HC_JR = atol(&buff[11]);
      return;
    }
    if (strncmp("BBR HP JR ", &buff[1] , 10)==0) {
      teleinfo.BBR_HP_JR = atol(&buff[11]);
      return;
    }
    if (strncmp("PEJP ", &buff[1] , 5)==0) {
      strncpy(teleinfo.PEJP, &buff[6], 2);
      teleinfo.PEJP[2] = '\0';
      return;
    }
    if (strncmp("PTEC ", &buff[1] , 5)==0) {
      strncpy(teleinfo.PTEC, &buff[6], 4);
      teleinfo.PTEC[4] = '\0';
      return;
    }
    if (strncmp("DEMAIN ", &buff[1] , 5)==0) {
      strncpy(teleinfo.DEMAIN, &buff[6], 4);
      teleinfo.DEMAIN[4] = '\0';
      return;
    }
    if (strncmp("IINST ", &buff[1] , 6)==0) {
      teleinfo.IINST = atoi(&buff[7]);
      return;
    }
    if (strncmp("PAPP ", &buff[1] , 5)==0) {
      teleinfo.PAPP = atoi(&buff[6]);
      return;
    }
    if (strncmp("ADPS ", &buff[1] , 5)==0) {
      teleinfo.ADPS = atoi(&buff[6]);
      return;
    }
    if (strncmp("IMAX ", &buff[1] , 5)==0) {
      teleinfo.IMAX = atoi(&buff[6]);
      return;
    }
    if (strncmp("HHPHC ", &buff[1] , 6)==0) {
      strncpy(teleinfo.HHPHC, &buff[7], 1);
      teleinfo.HHPHC[1] = '\0';
      return;
    }
}

// Calcul checksum  ----------------------------------------------------
char ckecksum(char *buff, int len)
{
int i;
char sum = 0;

    for (i=1; i<(len-2); i++) sum = sum + buff[i];
    sum = (sum & 0x3F) + 0x20;

    return(sum);
}

// Lecture infos Teleinfo ------------------------------------------------------
void read_teleinfo()
{
char in;
  
  if (Serial.available()>0) {
    in = (char)Serial.read() & 127;  // seulement sur 7 bits

    switch (in) {
      case DEB_TRAME:
        index_buff=0;
        break;

      case FIN_TRAME:
        Serial.println(buffin);
        Serial.print("Ind=");
        Serial.println(index_buff);
        // Test validité longueur trame
        if (index_buff >= LG_TRAME_MIN-1 && index_buff <= LG_TRAME_MAX-1) {
          Serial.print("CalCS=");
          Serial.println(buffin[index_buff-1], HEX);
          if (ckecksum(buffin, index_buff-1) == buffin[index_buff-1]) { // Test du checksum
            traitement_trame(buffin);
          } 
          index_buff=0;
        }
        break;

      default:
        // stock buffer ------------------------
        if (index_buff <= LG_TRAME_MAX) {
          buffin[index_buff] = in;
          index_buff++;
        }
        else index_buff=0;
        break;
    }     
  }
}
