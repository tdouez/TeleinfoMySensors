# TeleinfoMySensors v1.1.1

Interface Téléinfo pour compteur Linky avec MySensors - version compatible mode standard

Projet utilisant un Arduino Pro mini et un module NRF24L01

- Hardware disponible sur https://www.openhardware.io/view/744/MySensors-Linky
- Boîtier disponible sur https://www.thingiverse.com/thing:3985437
- Description disponible sur http://fumeebleue.fr

Liste des informations remontées via MySensors :

 CHILD_ID_ADSC     0
 CHILD_ID_VTIC     1
 CHILD_ID_NGTF     2
 CHILD_ID_LTARF    3
 CHILD_ID_EAST     4
 CHILD_ID_IRMS1    5
 CHILD_ID_IRMS2    6
 CHILD_ID_IRMS3    7
 CHILD_ID_URMS1    8
 CHILD_ID_URMS2    9
 CHILD_ID_URMS3    10
 CHILD_ID_PREF     11
 CHILD_ID_PCOUP    12
 CHILD_ID_SINSTS   13
 CHILD_ID_SINSTS1  14
 CHILD_ID_SINSTS2  15
 CHILD_ID_SINSTS3  16
 CHILD_ID_STGE     17
 CHILD_ID_MSG1     18
 CHILD_ID_NTARF     19
 CHILD_ID_NJOURF   20
 CHILD_ID_NJOURF1  21
 CHILD_ID_EAIT     22
 CHILD_ID_SINSTI   23
 CHILD_ID_EASF01   24
 CHILD_ID_EASF02   25
CHILD_ID_EASF03   26
CHILD_ID_EASF04   27
CHILD_ID_EASF05   28
CHILD_ID_EASF06   29
CHILD_ID_EASF07   30
CHILD_ID_EASF08   31
CHILD_ID_EASF09   32
CHILD_ID_EASF10   33
CHILD_ID_EASD01   34
CHILD_ID_EASD02   35
CHILD_ID_EASD03   36
CHILD_ID_EASD04   37
CHILD_ID_ERQ1     38
CHILD_ID_ERQ2     39
CHILD_ID_ERQ3     40
CHILD_ID_ERQ4     41

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
//  Profil du prochain jour calendrier fournisseur : PJOURF+1 (98 carateres)
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