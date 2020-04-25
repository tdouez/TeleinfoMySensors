# TeleinfoMySensors v1.0.6
Interface Téléinfo pour compteur Linky avec MySensors - mode historique

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

    numero d'identification du compteur, etiquette: ADCO (12 caracteres)
    option tarifaire (type d'abonnement) : OPTARIF (4 car.)
    intensite souscrite : ISOUSC ( 2 car. unite = amperes)
    index si option = base : BASE ( 9 car. unite = Wh)
    index heures creuses si option = heures creuses : HCHC ( 9 car. unite = Wh)
    index heures pleines si option = heures creuses : HCHP ( 9 car. unite = Wh)
    index heures normales si option = EJP : EJP HN ( 9 car. unite = Wh)
    index heures de pointe mobile si option = EJP : EJP HPM ( 9 car. unite = Wh)
    index heures creuses jours bleus si option = tempo : BBR HC JB ( 9 car. unite = Wh)
    index heures pleines jours bleus si option = tempo : BBR HP JB ( 9 car. unite = Wh)
    index heures creuses jours blancs si option = tempo : BBR HC JW ( 9 car. unite = Wh)
    index heures pleines jours blancs si option = tempo : BBR HP JW ( 9 car. unite = Wh)
    index heures creuses jours rouges si option = tempo : BBR HC JR ( 9 car. unite = Wh)
    index heures pleines jours rouges si option = tempo : BBR HP JR ( 9 car. unite = Wh)
    preavis EJP si option = EJP : PEJP ( 2 car.) 30mn avant periode EJP
    periode tarifaire en cours : PTEC ( 4 car.)
    couleur du lendemain si option = tempo : DEMAIN
    intensite instantanee : IINST ( 3 car. unite = amperes)
    Puissance apparente : PAPP ( 5 car. unite = Volt.amperes)
    avertissement de depassement de puissance souscrite : ADPS ( 3 car. unite = amperes) (message emis uniquement en cas de depassement effectif, dans ce cas il est immediat)
    intensite maximale : IMAX ( 3 car. unite = amperes)
    groupe horaire si option = heures creuses ou tempo : HHPHC (1 car.)
    mot d'etat (autocontrole) : MOTDETAT (6 car.)

Exemple de trames teleinfo

    ADCO 040422040644 5 (N° d'identification du compteur : ADCO (12 caractères))
    OPTARIF HC.. < (Option tarifaire (type d'abonnement) : OPTARIF (4 car.))
    ISOUSC 45 ? (Intensité souscrite : ISOUSC ( 2 car. unité = ampères))
    HCHC 077089461 0 (Index heures creuses si option = heures creuses : HCHC ( 9 car. unité = Wh))
    HCHP 096066754 > (Index heures pleines si option = heures creuses : HCHP ( 9 car. unité = Wh))
    PTEC HP.. (Période tarifaire en cours : PTEC ( 4 car.))
    IINST 002 Y (Intensité instantanée : IINST ( 3 car. unité = ampères))
    IMAX 044 G (Intensité maximale : IMAX ( 3 car. unité = ampères))
    PAPP 00460 + (Puissance apparente : PAPP ( 5 car. unité = Volt.ampères))
    HHPHC E 0 (Groupe horaire si option = heures creuses ou tempo : HHPHC (1 car.))
    MOTDETAT 000000 B (Mot d'état (autocontrôle) : MOTDETAT (6 car.))
