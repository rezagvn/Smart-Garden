#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "ESP8266WiFi.h"
#include "DHT.h"


// WiFi parameters
const char* ssid = "qw";
const char* password = "123456789";

// Pin
#define DHTPIN D5
#define pompa D4
byte kelembabantanah_pin = A0;

// Use DHT11 sensor
#define DHTTYPE DHT11

// Initialize DHT senso r
DHT dht(DHTPIN, DHTTYPE, 15);
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Host
const char* host = "dweet.io";
const char* thing  = "skripsieja";
void setup() {

  // Start Serial
  Serial.begin(115200);
  delay(10);

  // Init DHT
  dht.begin();
  lcd.begin();


  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
  pinMode(pompa, OUTPUT);
  Serial.println("");
  lcd.clear();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  float kt;
  int tanah;
  tanah = analogRead(kelembabantanah_pin);
  kt = ( 100 - ( (tanah / 1023.00) * 100 ) ) +4,3;
  delay(1000);

  // Reading temperature and humidity
  float h = dht.readHumidity();
  
  // Read temperature as Celsius
  float t;
  float suhu = dht.readTemperature();
  t = suhu + 2,18;

  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(t);
  lcd.print(" C");
  Serial.print(t);
  Serial.println(" *C");

  lcd.setCursor(0, 1);
  lcd.print("Kadar Air:");
  lcd.print(kt);
  lcd.print("%");
  Serial.print(kt);

  if (kt <= 50 && t <= 30) {
    digitalWrite(pompa, LOW);
  }
  else {
    digitalWrite(pompa, HIGH);
  }


  String url = "/dweet/for/";
  url += thing;
  url += "?";
  url += "Suhu=";
  url += t;
  url += "&Humidity=";
  url += kt;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 3000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println("closing connection");

  // Repeat every 10 seconds
  delay(1000);

}
