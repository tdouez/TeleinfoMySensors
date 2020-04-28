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


#define LG_TRAME_MAX  125
#define LG_TRAME_MIN  5
#define DEB_TRAME 0x0A
#define FIN_TRAME 0x0D

char buffin[LG_TRAME_MAX];
int  index_buff = 0;

// Traitement trame teleinfo ------------------------------------------
void traitement_trame(char *buff)
{

    Serial.println(buff);
    
    if (strncmp("ADSC", &buff[0] , 4)==0) {
      strncpy(teleinfo._ADSC, &buff[5], 12);
      teleinfo._ADSC[12] = '\0';
      return;
    }
    if (strncmp("VTIC", &buff[0] , 4)==0) {
      strncpy(teleinfo.VTIC, &buff[5], 2);
      teleinfo.VTIC[2] = '\0';
      return;
    }
    if (strncmp("NGTF", &buff[0] , 4)==0) {
      strncpy(teleinfo.NGTF, &buff[5], 16);
      teleinfo.NGTF[16] = '\0';
      return;
    }
    if (strncmp("LTARF", &buff[0] , 4)==0) {
      strncpy(teleinfo.LTARF, &buff[5], 16);
      teleinfo.LTARF[16] = '\0';
      return;
    }
    if (strncmp("EAST", &buff[0] , 4)==0) {
      teleinfo.EAST = atol(&buff[5]);
      return;
    }
    if (strncmp("EAIT", &buff[0] , 4)==0) {
      teleinfo.EAIT = atol(&buff[5]);
      return;
    }
    if (strncmp("IRMS1", &buff[0] , 5)==0) {
      teleinfo.IRMS1 = atoi(&buff[6]);
      return;
    }
    if (strncmp("IRMS2", &buff[0] , 5)==0) {
      teleinfo.IRMS2 = atoi(&buff[6]);
      return;
    }
    if (strncmp("IRMS3", &buff[0] , 5)==0) {
      teleinfo.IRMS3 = atoi(&buff[6]);
      return;
    }
    if (strncmp("URMS1", &buff[0] , 5)==0) {
      teleinfo.URMS1 = atoi(&buff[6]);
      return;
    }
    if (strncmp("URMS2", &buff[0] , 5)==0) {
      teleinfo.URMS2 = atoi(&buff[6]);
      return;
    }
    if (strncmp("URMS3", &buff[0] , 5)==0) {
      teleinfo.URMS3 = atoi(&buff[6]);
      return;
    }
    if (strncmp("PREF", &buff[0] , 4)==0) {
      teleinfo.PREF = atoi(&buff[5]);
      return;
    }
    if (strncmp("PCOUP", &buff[0] , 5)==0) {
      teleinfo.PCOUP = atoi(&buff[6]);
      return;
    }
    if (strncmp("SINSTS", &buff[0] , 6)==0) {
      teleinfo.SINSTS = atoi(&buff[7]);
      return;
    }
    if (strncmp("SINSTS1", &buff[0] , 7)==0) {
      teleinfo.SINSTS1 = atoi(&buff[8]);
      return;
    }
    if (strncmp("SINSTS2", &buff[0] , 7)==0) {
      teleinfo.SINSTS2 = atoi(&buff[8]);
      return;
    }
    if (strncmp("SINSTS3", &buff[0] , 7)==0) {
      teleinfo.SINSTS3 = atoi(&buff[8]);
      return;
    }
    if (strncmp("SINSTI", &buff[0] , 6)==0) {
      teleinfo.SINSTI = atoi(&buff[7]);
      return;
    }
    if (strncmp("STGE", &buff[0] , 4)==0) {
      strncpy(teleinfo.STGE, &buff[5], 8);
      teleinfo.STGE[8] = '\0';
      return;
    }
    if (strncmp("MSG1", &buff[0] , 4)==0) {
      strncpy(teleinfo.MSG1, &buff[5], 32);
      teleinfo.MSG1[32] = '\0';
      return;
    }
    if (strncmp("NTAF", &buff[0] , 4)==0) {
      strncpy(teleinfo.NTAF, &buff[5], 2);
      teleinfo.NTAF[2] = '\0';
      return;
    }
    if (strncmp("NJOURF", &buff[0] , 6)==0) {
      strncpy(teleinfo.NJOURF, &buff[7], 2);
      teleinfo.NJOURF[2] = '\0';
      return;
    }
    if (strncmp("NJOURF1", &buff[0] , 7)==0) {
      strncpy(teleinfo.NJOURF1, &buff[8], 2);
      teleinfo.NJOURF1[2] = '\0';
      return;
    }
}

// Calcul checksum  ----------------------------------------------------
char ckecksum(char *buff, int len)
{
int i;
char sum = 0;

    for (i=0; i<len; i++) sum = sum + buff[i];
    sum = (sum & 0x3F) + 0x20;

    //Serial.print("CalCS=");
    //Serial.println(sum, HEX);
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
        buffin[index_buff]=0;
        
        // Test validité longueur trame
        if (index_buff >= LG_TRAME_MIN-1 && index_buff <= LG_TRAME_MAX-1) {
          //Serial.println(buffin);
          //for (int i=0; i<index_buff; i++) {
          //  Serial.print(".");
          //  Serial.print(buffin[i], HEX);
          //}
          //Serial.println("");
          //Serial.print("CS=");
          //Serial.println(buffin[index_buff-1], HEX);
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
