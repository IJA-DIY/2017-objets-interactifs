#include <NewPing.h>

/*
 * 
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * 
 * Typical pin layout used:
 * --------------------------------------
 *             MFRC522      Teensy 3.6     
 *             Reader/PCD   
 * Signal      Pin          Pin          
 * ---------------------------------------
 * RST/Reset   RST          31           
 * SPI SS      SDA(SS)      32           
 * SPI MOSI    MOSI         11 
 * SPI MISO    MISO         12 
 * SPI SCK     SCK          13 
 */
/*
 *Bienvenue dans le mode 1 du projet objets connectés: 
 *Conseil:
 * il faut jouer sur la maquette
 * les objets  sont dans le champ de vision des capteurs
 * appuyer sur le bouton en face de l'objet pour le désigner en tant qu'objectif
 * vous pouvez régler les distances des capteurs plus bas ainsi que les pins
 *
*/
// Constante NFC
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 32
#define RST_PIN 31
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 
// Init array that will store new NUID 
byte nuidPICC[4];
byte tableau_ordre[3][4]; //tableau contenant 3 tags nfc de taille 4
char* tableau_ordre_objet[3]; //tableau contenant les objets associés des 3 tags
int indice = 0;
char *test = "bananne";
// tag= DA 45 B1 AB carte
//tag= 70 74 89 15 cle
// 04 C5 7E DA 68 3B 80

//mettre tableau avec nom objet ainsi que tag en base 10 dy type: 
// byte  objet_nom = { code nfc base 10};
byte objet_cle[4] = {112,116,137,21};
byte objet_carte[4] = {218,69,177,171}; 
byte objet_jabra[7] = {04,197,126,218,104,59,128};

//Constante capteur/bouton
//Capteur 1
#define TRIG1  24 //12
#define ECHO1  25 //11
#define MAX_DIST1 20 //5
//Capteur 2
#define TRIG2  26 //10
#define ECHO2  27 //9
#define MAX_DIST2 20 
//Capteur 3
#define TRIG3  28 //7
#define ECHO3  29 //8
#define MAX_DIST3 20 
// autre constante condition capteur



NewPing sonar1(TRIG1, ECHO1, MAX_DIST1); // Capteur 1
NewPing sonar2(TRIG2, ECHO2, MAX_DIST2);  // Capteur 2
NewPing sonar3(TRIG3, ECHO3, MAX_DIST3);    // Capteur 3

const int BP1 = 33;  //bouton1 branché en Pin 2  //33 //2
const int BP2 = 34;  //bouton2 branché en Pin 3  //34 //3
const int BP3 = 35;  //bouton3 branché en Pin 4  //35 //4
const int L1 = 37;  //Led1 branché en Pin 5      //37 //5
const int L2 = 38;  //Led2 branché en Pin 6     //38  //6
const int L3 = 39; //39 //13
int test1,test2,test3;
int nombre1,nombre2,nombre3,bouton_courant;
//Fin constante 
int temps_capt1 = 0, temps_capt2 = 0, temps_capt3 = 0;// variable qui stock le temps pour changer la disctance de l'objet
int dist_obj1 = MAX_DIST1, dist_obj2 = MAX_DIST2, dist_obj3 = MAX_DIST3;
int temp_nombre1 = 0, temp_nombre2 = 0, temp_nombre3 = 0;

void setup() { 
  Serial.begin(9600);
  //Initialisation bouton/capteur
  pinMode(BP1, INPUT);  //intialisation bouton1
  pinMode(BP2, INPUT);  //intialisation bouton2
  pinMode(BP3, INPUT);  //intialisation bouton3
  pinMode(L1, OUTPUT);  //intialisation Led1
  pinMode(L2, OUTPUT);  //intialisation Led2
  pinMode(L3,OUTPUT);

  //Initialisation NFC
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 7; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  while(indice<3){
    //Serial.print("debut test");
      indice = initialisation(indice);
      if (indice == 3){    // si 3 tag sont lu donc on arrête
        Serial.println("ordre = ");// on affiche l'ordre
        for (int i=0; i<3;i++){
          Serial.print("carte sur le capteur ");
          Serial.print(i+1);
          Serial.print(" : ");
          printDec(tableau_ordre[i],4);    //on affiche le tag de l'objet sur le i+1 eme capteur (1,2,3)
          tableau_ordre_objet[i] = objet_capteur(tableau_ordre[i]); //on met dans le tableau le nom de l'objet associé au tag s'il existe.
          Serial.println(tableau_ordre_objet[i]); // on affiche le nom de l'objet 
        } 
      }
   
  }
  

Serial.println("fin setup");
}
 

void loop() {
 delay(500);
 test1 = digitalRead(BP1); //lit l'état du bouton1
 test2 = digitalRead(BP2); //lit l'état du bouton2
 test3 = digitalRead(BP3); //lit l'état du bouton3
 nombre1 = sonar1.ping_cm(); //mesure distance capteur 1
 nombre2 = sonar2.ping_cm(); //mesure distance capteur 2
 nombre3 = sonar3.ping_cm(); //mesure distance capteur 3
 
 if(test1==0) // Si test est à l'état bas
 {
  Serial.print("changement d'objectif sur ");
  Serial.println(tableau_ordre_objet[0]);
  bouton_courant = 1;
 }
 if(test2==0) // Si test bouton 2 est à l'etat bas
 {
 Serial.print("changement d'objectif sur ");
 Serial.println(tableau_ordre_objet[1]);
  bouton_courant = 2;
 }
 if(test3==0) // si test bouton 3 est à l'ett bas
 {
  Serial.print("changement d'objectif sur ");
  Serial.println(tableau_ordre_objet[2]);
  bouton_courant = 3; 
 }
 
// Conditions sur le capteur 1

 // Condition pour savoir si l'objet change de distance (incrémentation)
 if ((temp_nombre1-2<=nombre1<=temp_nombre1+2) and (nombre1>=dist_obj1+2 or nombre1 <= dist_obj1-2))// si l'objet ne bouge pas (+-2) et s'il n'est pas au même endroit alors
 {
   temps_capt1++;  //on incremeent l'intervalle
   Serial.print("++");
   Serial.print(temps_capt1);
 }
 else temps_capt1=0;
 
 temp_nombre1 = nombre1;
 // Condition de finalité pour changer la distance de l'objet (changement)
 if (temps_capt1>8)// l'objet change de distance car il est rester suffisament de temps à cette distance
 {
   Serial.print("ok reinit a ");
   Serial.println(nombre1);
   dist_obj1 = nombre1;
   temps_capt1 = 0;
 }
 // condition pour voir si il  y a un mouvement entre l'objet et le capteur
 if (nombre1 > 0 && nombre1 <= dist_obj1 - 2)
 { 
   if(bouton_courant == 1 )
  
     {
      Serial.print("bien joué! ");  
      digitalWrite(L1, HIGH);
      delay(1000);
      digitalWrite(L1, LOW);
     }
    Serial.print("objet1 "); //correspond au premier objet
    Serial.println(tableau_ordre_objet[0]);

 }


  
 // Conditions sur le capteur 2
 
 // Condition pour savoir si l'objet change de distance (incrémentation)
 if ((temp_nombre2-2<=nombre2<=temp_nombre2+2) and (nombre2>=dist_obj2+2 or nombre2 <= dist_obj2-2))// si l'objet ne bouge pas 
 { // (+-2) et s'il n'est pas au même endroit alors
   temps_capt2++;  //on incremeent l'intervalle
   Serial.print("++");
   Serial.print(temps_capt2);
 }
 else temps_capt2=0;
 
 temp_nombre2 = nombre2;
 // Condition de finalité pour changer la distance de l'objet (changement)
 if (temps_capt2>8)// l'objet change de distance car il est rester suffisament de temps à cette distance
 {
   Serial.print("ok reinit a ");
   Serial.println(nombre2);
   dist_obj2 = nombre2;
   temps_capt2 = 0;
 }

 if (nombre2 > 0 && nombre2 <= dist_obj2 -2 )
 { 
   if(bouton_courant == 2 )
  
     {
      Serial.print("bien joué! ");
      digitalWrite(L2, HIGH);
      delay(1000);
      digitalWrite(L2, LOW);
     }
    Serial.print("objet2 "); //correspond au deuxieme objet
    Serial.println(tableau_ordre_objet[1]);
 }
 // Conditions sur le capteur 3

 // Condition pour savoir si l'objet change de distance (incrémentation)
 if ((temp_nombre3-2<=nombre3<=temp_nombre3+2) and (nombre3>=dist_obj3+2 or nombre3 <= dist_obj3-2))// si l'objet ne bouge pas (+-2) et s'il n'est pas au même endroit alors
 {
   temps_capt3++;  //on incremeent l'intervalle
   Serial.print("++");
   Serial.print(temps_capt3);
 }
 else temps_capt3=0;
 
 temp_nombre3 = nombre3;
 // Condition de finalité pour changer la distance de l'objet (changement)
 if (temps_capt3>8)// l'objet change de distance car il est rester suffisament de temps à cette distance
 {
   Serial.print("ok reinit a ");
   Serial.println(nombre3);
   dist_obj3 = nombre3;
   temps_capt3 = 0;
 }
 if (nombre3 > 0 && nombre3 <= dist_obj3 - 2)
 {
   if(bouton_courant == 3)
      {
       Serial.print("bien joué! ");
       digitalWrite(L3, HIGH);
       delay(1000);
       digitalWrite(L3, LOW);
      }
    Serial.print("objet3 "); //corespond au troisieme objet
    Serial.println(tableau_ordre_objet[2]);
    //Serial.println(nombre1);
  }
  //Serial.println("suite");
      
}

int initialisation(int ind){
  //Serial.print("debut init");
  // PARTIE NFC NE PAS MODIFIER
    // Look for new cards
    if ( ! rfid.PICC_IsNewCardPresent())
      return ind;
  
    // Verify if the NUID has been readed
    if ( ! rfid.PICC_ReadCardSerial())
      return ind;
  
    if (rfid.uid.uidByte[0] != nuidPICC[0] || // verifie si la nouvelle et comme la derniere
      rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || 
      rfid.uid.uidByte[3] != nuidPICC[3] ) {
      Serial.println(F("msg=A new card has been detected."));
      
  
      // Store NUID into nuidPICC array
      for (byte i = 0; i < 4; i++) {
        nuidPICC[i] = rfid.uid.uidByte[i];
      }
      
      ordre(tableau_ordre[ind], rfid.uid.uidByte, rfid.uid.size, ind); // ajoute dans le tableau ordre un tag nfc
      ind++;
    } else {
        Serial.println(F("msg=Card read previously."));
    }
  
    // Halt PICC
    rfid.PICC_HaltA();
  
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
  //FIN NFC
  
 Serial.print("fin init");
 return ind;
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
char* objet_capteur(byte *tag){ //assigne un tag lu à un objet connu
  // if (diff(tag lu par le lecteur, tag d'un objet) == 1 (renvoie 1 si c'est pareil)) renvoie "Nom_objet" stocké dans un tableau après 
  if (diff(tag,objet_carte)==1) return "carte";
  if (diff(tag,objet_cle)==1) return "cle";
  if (diff(tag,objet_jabra)==1) return "jabra";
  return "non connu";
  }
// tag= DA 45 B1 AB carte
//tag= 70 74 89 15 cle
// 04 C5 7E DA 68 3B 80
void ordre(byte * tableau, byte *tag, byte bufferSize, int indice){ //Code qui definit l'ordre
  
  for (byte i = 0; i< bufferSize; i++){
    tableau[i] = tag[i];
  }
  Serial.print("carte ranger = ");
  Serial.print(indice);
  Serial.print(" : ");
  printHex(tableau, bufferSize);
  printDec(tableau, bufferSize);

  
}
int diff( byte *tag, byte *tagcomp){
  int pareil = 1;
  for (byte i = 0; i< 4; i++){
    if (tag[i] != tagcomp[i]) pareil = 0;
    
  }
  return pareil;
}
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
