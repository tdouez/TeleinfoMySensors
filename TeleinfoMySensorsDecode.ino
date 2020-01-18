//-----------------------------------------------------------------------------------------------------------------
// Chaque message composant la trame comprend :
// un caractere LF (code ASCII = 0A hexa)
// une etiquette de 4 a  8 caracteres
// un espace SP (20 hexa)
// des donnees de 1 a  12 caracteres
// un espace SP (20 hexa)
// un caractere de controle
// un caractere CR (0D hexa)

// Ces differents messages donnent les indications suivantes en fonction de l'abonnement souscrit :
//  numero d'identification du compteur, etiquette: ADCO (12 caracteres) 
//  option tarifaire (type d'abonnement) : OPTARIF (4 car.) 
//  intensite souscrite : ISOUSC ( 2 car. unite = amperes) 
//  index si option = base : BASE ( 9 car. unite = Wh) 
//  index heures creuses si option = heures creuses : HCHC ( 9 car. unite = Wh) 
//  index heures pleines si option = heures creuses : HCHP ( 9 car. unite = Wh) 
//  index heures normales si option = EJP : EJP HN ( 9 car. unite = Wh) 
//  index heures de pointe mobile si option = EJP : EJP HPM ( 9 car. unite = Wh) 
//  index heures creuses jours bleus si option = tempo : BBR HC JB ( 9 car. unite = Wh) 
//  index heures pleines jours bleus si option = tempo : BBR HP JB ( 9 car. unite = Wh) 
//  index heures creuses jours blancs si option = tempo : BBR HC JW ( 9 car. unite = Wh) 
//  index heures pleines jours blancs si option = tempo : BBR HP JW ( 9 car. unite = Wh) 
//  index heures creuses jours rouges si option = tempo : BBR HC JR ( 9 car. unite = Wh) 
//  index heures pleines jours rouges si option = tempo : BBR HP JR ( 9 car. unite = Wh) 
//  preavis EJP si option = EJP : PEJP ( 2 car.) 30mn avant periode EJP 
//  periode tarifaire en cours : PTEC ( 4 car.) 
//  couleur du lendemain si option = tempo : DEMAIN 
//  intensite instantanee : IINST ( 3 car. unite = amperes) 
//  Puissance apparente : PAPP ( 5 car. unite = Volt.amperes) 
//  avertissement de depassement de puissance souscrite : ADPS ( 3 car. unite = amperes) (message emis uniquement en cas de depassement effectif, dans ce cas il est immediat) 
//  intensite maximale : IMAX ( 3 car. unite = amperes) 
//  groupe horaire si option = heures creuses ou tempo : HHPHC (1 car.) 
//  mot d'etat (autocontrole) : MOTDETAT (6 car.) 
//-----------------------------------------------------------------------------------------------------------------
//  Information en provenance de la telinfo  
//    ADCO 040422040644 5      (N° d'identification du compteur : ADCO (12 caractères))
//    OPTARIF HC.. <          (Option tarifaire (type d'abonnement) : OPTARIF (4 car.))
//    ISOUSC 45 ?             (Intensité souscrite : ISOUSC ( 2 car. unité = ampères))
//    HCHC 077089461 0          (Index heures creuses si option = heures creuses : HCHC ( 9 car. unité = Wh))
//    HCHP 096066754 >          (Index heures pleines si option = heures creuses : HCHP ( 9 car. unité = Wh))
//    PTEC HP..               (Période tarifaire en cours : PTEC ( 4 car.))
//    IINST 002 Y             (Intensité instantanée : IINST ( 3 car. unité = ampères))
//    IMAX 044 G              (Intensité maximale : IMAX ( 3 car. unité = ampères))
//    PAPP 00460 +            (Puissance apparente : PAPP ( 5 car. unité = Volt.ampères))
//    HHPHC E 0                 (Groupe horaire si option = heures creuses ou tempo : HHPHC (1 car.))
//    MOTDETAT 000000 B         (Mot d'état (autocontrôle) : MOTDETAT (6 car.))
//-----------------------------------------------------------------------------------------------------------------

char buffin[32];
int  bufflen = 0;


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
    in = (char)Serial.read() & 127;
        
    if (in == 0x0A) bufflen=0;
  
    buffin[bufflen] = in;
    bufflen++;
    
    if (bufflen > 20) bufflen=0;

    if (in == 0x0D && bufflen > 5)   { // fin trame ------
      //Serial.println(buffin);
      if (ckecksum(buffin,bufflen-1) == buffin[bufflen-2]) { // Test du checksum
        traitement_trame(buffin);
      } 
//      else {
//        digitalWrite(LED_SEND, LOW);
//        delay(100);
//        digitalWrite(LED_SEND, HIGH);
//      //  Serial.print("CS KO! ");
//      //  Serial.println(buffin);
//      }
    }
  }
}
