#include "WiFiEsp.h"
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(10, 11);
#endif

char ssid[] = "NETGEAR14"; // your network SSID (name)
char pass[] = "ABCD1234ab"; // your network password
int status = WL_IDLE_STATUS; // the Wifi radio's status
char server[] = "54.151.87.167";

char get_request[200];
WiFiEspClient client;

long lightminimum = 1023;
long lightmaximum = 0;
long soundminimum = 1023;
long soundmaximum = 0;
long lightdiff;
long sounddiff;

#include <SparkFun_RHT03.h>

const int RHT03_DATA_PIN = 4;

char var0[] = "TEMPSS";
char var1[] = "TEMPSS";
char var2[] = "TEMPSS";
char var3[] = "LIGHTS";
char var4[] = "SOUNDS"; 

RHT03 rht;

float latestHumidity = 0;
float latestTempC = 0;
float latestTempF = 0;
int light = 0;
int sound = 0;
float calibratedLight = 0;
float calibratedSound = 0;

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  
  WiFi.init(&Serial1);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("You're connected to the network");
  printWifiStatus();

  rht.begin(RHT03_DATA_PIN);
  
  Serial.println("CALIBRATION START");
  
  while (latestHumidity == 0)
  {
    rht.update();
    latestHumidity = rht.humidity();
  }

  Serial.println("H&T CALIBRATION FINISHED");
  
  float start = millis();
  while (millis() - start < 10000)
  {
    int light = analogRead(A0);
    int sound = analogRead(A1);
    if (light < lightminimum){
      lightminimum = light;
    }
    if (light > lightmaximum) {
      lightmaximum = light;
    }
    if (sound < soundminimum){
      soundminimum = sound;
    }
    if (sound > soundmaximum) {
      soundmaximum = sound;
    }
  }
  Serial.println("CALIBRATION FINISHED");
  lightdiff = lightmaximum - lightminimum;
  sounddiff = soundmaximum - soundminimum;
}
  
void loop()
{
  if (!client.connected()){
    Serial.println("Starting connection to server...");
    client.connect(server, 5000);
  }
  
  rht.update();
  latestHumidity = rht.humidity();
  latestTempC = rht.tempC();
  latestTempF = rht.tempF();

  light = analogRead(A0);
  calibratedLight = ((light - lightminimum) * 180) / lightdiff;

  sound = analogRead(A1);
  calibratedSound = ((sound - soundminimum) * 180) / sounddiff;
  
  Serial.println("Latest Humidity: " + String(latestHumidity));
  Serial.println("Latest Temperature(C): " + String(latestTempC));
  Serial.println("Latest Temperature(F): " + String(latestTempF));
  Serial.println("Latest Light: " + String(calibratedLight));
  Serial.println("Latest Sound: " + String(calibratedSound));

  Serial.println("Connected to server");
  dtostrf(latestHumidity, 3, 2, var0);
  dtostrf(latestTempC, 3, 2, var1);
  dtostrf(latestTempF, 3, 2, var2);
  dtostrf(calibratedLight, 3, 2, var3);
  dtostrf(calibratedSound, 3, 2, var4);
  sprintf(get_request,"GET /?Humidity=%s&Temperature(C)=%s&Temperature(F)=%s&Light=%s&Sound=%s HTTP/1.1\r\nHost: 18.221.147.67\r\nConnection: close\r\n\r\n", var0, var1, var2, var3, var4);
  client.print(get_request);
  delay(500);
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  delay(20000);
}

void printWifiStatus()
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
