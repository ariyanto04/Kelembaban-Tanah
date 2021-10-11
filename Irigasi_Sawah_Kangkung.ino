#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

const char *ssid = "*******"; //ganti nama wifi
const char *pass = "*******";//ganti password

const char *host = "api.thingspeak.com";
String apiKey = "XXXXXXXXXXXXXXXX"; //apiKey pada thinkspeak

Servo servo;

int Lampu = 2;
int sudut = 0;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(Lampu,OUTPUT);

  servo.attach(0);
  servo.write(sudut);

  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  WiFi.begin(ssid, pass);     //Connect to your WiFi router
  Serial.println("");
  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

// the loop routine runs over and over again forever:
void loop() {
  WiFiClient client;
  const int httpPort = 80; //Port 80 is commonly used for www
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection Failed");
    delay(300);
    return; //Keep retrying until we get connected
  }

  String lembab;
  int sensorValue = analogRead(A0); //Read Analog value of LDR
  lembab = String(sensorValue);   //String to interger conversion
  Serial.println(sensorValue);
  delay(3000);
  if(sensorValue>522){
    digitalWrite(Lampu,HIGH);
    servo.write(180);
  }
  else if(sensorValue<328){
    digitalWrite(Lampu,LOW);
    servo.write(0);
  }
  
  String Link = "GET /update?api_key=" + apiKey + "&field1="; //Requeste webpage
  Link = Link + lembab;
  Link = Link + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
  client.print(Link);
  delay(100);

  int timeout = 0;
  while ((!client.available()) && (timeout < 1000)) {
    delay(10);  //Use this with time out
    timeout++;
  }
  if (timeout < 500) {
    while (client.available()) {
      Serial.println(client.readString()); //Response from ThingSpeak
    }
  }
  else {
    Serial.println("Request timeout..");
  }
  delay(1000);

}
