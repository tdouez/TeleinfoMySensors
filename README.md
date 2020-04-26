# TeleinfoMySensors v1.1.0

Interface Téléinfo pour compteur Linky avec MySensors - version compatible mode standard

Projet utilisant un Arduino Pro mini et un module NRF24L01

- Hardware disponible sur https://www.openhardware.io/view/744/MySensors-Linky
- Boîtier disponible sur https://www.thingiverse.com/thing:3985437
- Description disponible sur http://fumeebleue.fr

Liste des informations remontées via MySensors :

    CHILD_ID_ADCO 0
    CHILD_ID_OPTARIF 1
    CHILD_ID_ISOUSC 2
    CHILD_ID_BASE 3
    CHILD_ID_HCHC 4
    CHILD_ID_HCHP 5
    CHILD_ID_EJP_HN 6
    CHILD_ID_EJP_HPM 7
    CHILD_ID_BBR_HC_JB 8
    CHILD_ID_BBR_HP_JB 9
    CHILD_ID_BBR_HC_JW 10
    CHILD_ID_BBR_HP_JW 11
    CHILD_ID_BBR_HC_JR 12
    CHILD_ID_BBR_HP_JR 13
    CHILD_ID_PEJP 14
    CHILD_ID_PTEC 15
    CHILD_ID_DEMAIN 16
    CHILD_ID_IINST 17
    CHILD_ID_PAPP 18
    CHILD_ID_ADPS 19
    CHILD_ID_IMAX 20
    CHILD_ID_HHPHC 21

TELEINFORMATION

Ces differents messages donnent les indications suivantes en fonction de l'abonnement souscrit :

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

Exemple de trames teleinfo

VTIC	02	J
DATE	E200426181830		A
EAST	006231973	.
EASF02	000000000	#
EASF03	000000000	$
EASF04	000000000	%
EASF05	000000000	&
EASF07	000000000	(
EASF08	000000000	)
EASF09	000000000	*
EASD02	000000000	!
EASD03	000000000	"
EASD04	000000000	#
URMS1	235	D
PREF	09	H
PCOUP	09	"
SINSTS	01163	Q
SMAXSN	E200426132229	06100	2
SMAXSN-1	E200425151325	04700	Q
CCASN	E200426180000	02120	4
CCASN-1	E200426173000	02662	_
UMOY1	E200426181000	234	,
STGE	003A0001	:
MSG1	     PAS DE          MESSAGE    	<
PRM	01332272002530	_
RELAIS	000	B
NTARF	01	N
NJOURF	00	&
NJOURF+1	00	B