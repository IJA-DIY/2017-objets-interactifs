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
 *Bienvenue dans le mode 2 du projet objets connectés: 
 *Conseil:
 * il faut jouer en-dehors de la maquette
 * les objets ne sont pas dans le champ de vision des capteurs
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

//mettre tableau avec nom objet ainsi que tag en base 10 dy type: 
// byte  objet_nom = { code nfc base 10};
     // /!\ lecture des 4 premiers nombre du tag!!!!! faire attention au début des tags (possible de changer)
    // mais ce n'est pas nécessaire car normalement les changement se font sur les 4 premiers nombres (comme les autocollants fournis)

byte objet_cle[4] = {112,116,137,21};
byte objet_carte[4] = {218,69,177,171}; 
byte objet_jabra[7] = {04,197,126,218,104,59,128};

//Constante capteur/bouton
//Capteur 1
#define TRIG1  24 //12
#define ECHO1  25 //11
#define MAX_DIST1 40 //5
//Capteur 2
#define TRIG2  26 //10
#define ECHO2  27 //9
#define MAX_DIST2 40
//Capteur 3
#define TRIG3  28 //7
#define ECHO3  29 //8
#define MAX_DIST3 40 //peut-être régler


NewPing sonar1(TRIG1, ECHO1, MAX_DIST1); // Capteur 1
NewPing sonar2(TRIG2, ECHO2, MAX_DIST2);  // Capteur 2
NewPing sonar3(TRIG3, ECHO3, MAX_DIST3);    // Capteur 3

const int BP1 = 33;  //bouton1 branché en Pin 2  //33 //2
const int BP2 = 34;  //bouton2 branché en Pin 3  //34 //3
const int BP3 = 36;  //bouton3 branché en Pin 4  //35 //4
const int L1 = 37;  //Led1 branché en Pin 5      //37 //5
const int L2 = 38;  //Led2 branché en Pin 6     //38  //6
const int L3 = 39; //39 //13
int test1,test2,test3;
int nombre1,nombre2,nombre3,bouton_courant;
//Fin constante 


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
  //Partie de base ne pas toucher
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 7; i++) {
    key.keyByte[i] = 0xFF;
  }
  // // // // 
  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);// Affichage du tag vide

  // Système qui attend la lecteure de 3 tag NFC
  while(indice<3){
    //Serial.print("debut test");
      indice = initialisation(indice); // lit les tags
      if (indice == 3){    // si 3 tag sont lu on arrête
        Serial.println("ordre = ");// ensuite on affiche l'ordre
        for (int i=0; i<3;i++){
          Serial.print("objet sur le capteur ");
          Serial.print(i+1);
          Serial.print(" : ");
          printDec(tableau_ordre[i],4);    //on affiche le tag de l'objet sur le i+1 eme capteur (1,2,3)
          tableau_ordre_objet[i] = objet_capteur(tableau_ordre[i]); //on met dans le tableau le nom de l'objet 
                                                                    //associé au tag s'il existe.
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
 if (nombre1 > 0 && nombre1 <= MAX_DIST1) // s'il y a un mouvement devant le capteur à moinsde MAX_DIST cm
 { 
 if(bouton_courant == 1 ) // si c'est l'objectif on fait clignoter la led

   {
    Serial.print("bien joué! ");  
    digitalWrite(L1, HIGH);
    delay(1000);
    digitalWrite(L1, LOW);
   }
  //Serial.print("objet1 "); //correspond au premier objet
  Serial.println(tableau_ordre_objet[0]);

 }
 if (nombre2 > 0 && nombre2 <= MAX_DIST2)
 { 
 if(bouton_courant == 2 )

   {
    Serial.print("bien joué! ");
    digitalWrite(L2, HIGH);
    delay(1000);
    digitalWrite(L2, LOW);
   }
  //Serial.print("objet2 "); //correspond au deuxieme objet
  Serial.println(tableau_ordre_objet[1]);


 }
 if (nombre3 > 0 && nombre3 <= MAX_DIST3)
 {
  if(bouton_courant == 3)
    {
     Serial.print("bien joué! ");
     digitalWrite(L3, HIGH);
     delay(1000);
     digitalWrite(L3, LOW);
    }
  //Serial.print("objet3"); //corespond au troisieme objet
  Serial.println(tableau_ordre_objet[2]);


  
  }
  
  //Serial.println("suite");
      
}

int initialisation(int ind){ // appelle le lecteur NFC et regarde s'il y a une nouveau tag présent
  //Serial.print("debut init");
  // PARTIE NFC NE PAS MODIFIER // Code de la librairie RFID
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
    // FIN code librairie RFID
    // //
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
  // if (diff(tag lu par le lecteur, tag d'un objet) == 1 (renvoie 1 si c'est pareil))
  // return "Nom_objet" // renvoie le nom de l'objet et le stock dans un tableau 
  if (diff(tag,objet_carte)==1) return "carte";
  if (diff(tag,objet_cle)==1) return "cle";
  if (diff(tag,objet_jabra)==1) return "jabra";
  return "non connu";
  }

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
