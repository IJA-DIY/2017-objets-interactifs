// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------

#include <NewPing.h>

#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     14  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 20 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  pinMode(LED_BUILTIN, OUTPUT);
  

}
int intervalle = 0,pong=0,temp=0,dist_obj=20;//pong variable distance en cours temp distance avant dist_obj distance de l'objet
int limite_dist = 20; //distance max 
void loop() {
  delay(500);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  Serial.print("Ping: ");
  pong= sonar.ping_cm(); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.print(pong);
  Serial.print("cm distance =");
  Serial.println(dist_obj);

  if (( pong < dist_obj-1) and (pong> 0))// si inferieur à la distance alors mouvement devant objet => afficher nom oobjet
  {
    //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.print("objet atteint! ");
    Serial.print("inf à ");
    Serial.print(dist_obj);
  }
  if ((temp-2<=pong<=temp+2) and (pong>=dist_obj+2 or pong <= dist_obj-2))// si l'objet (inconnu) ne bouge pas (+-2) et s'il n'est pas au même endroit alors
  {
    intervalle++;
    Serial.print("++");
  }
  else intervalle=0;
  
  
  if (intervalle>8)// l'objet est change de distance car il est rester suffisament de temps à cette distance
  {
    Serial.print("ok reinit a ");
    Serial.println(pong);
    dist_obj = pong;
    intervalle = 0;
  }
  Serial.println(""); 
  
  temp=pong;
  
  
  // turn the LED off by making the voltage LOW
    //delay(100); 
  

}
