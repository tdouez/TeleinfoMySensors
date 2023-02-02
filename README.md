# TeleinfoMySensors v2.0.3

Interface Téléinfo pour compteur Linky avec MySensors - version compatible mode standard et historique

Projet utilisant un Arduino Pro mini et un module NRF24L01

- Hardware disponible sur https://www.openhardware.io/view/744/MySensors-Linky
- Boîtier disponible sur https://www.thingiverse.com/thing:3985437
- Description disponible sur http://fumeebleue.fr

Liste des informations remontées via MySensors historique :

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

Liste des informations remontées via MySensors standard :

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
