#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <Wire.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MLX90614.h>    // temp sensor
#include <SoftwareSerial.h>
SoftwareSerial mySerial(16, 17);
//#include <DHT.h>
//#define DHTPIN 26
//#define DHTTYPE DHT11
//DHT dht(DHTPIN, DHTTYPE);
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000

PulseOximeter pox;
int pulse;
float spo2;
uint32_t tsLastReport = 0;

#define SD_CS 5
int i = 1;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


const int ledr = 25;
const int ledg = 33;
const int butD=27;
const int butP=14;
const int butH=12;

const char* apn     = "airtelgprs.com";
const char* ssid     = "XXXXX";
const char* password = "XXXXXXXXX";
const char* host = "api.thingspeak.com";
String api_key = "XXXXXXXX";

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (mySerial.available())
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}


void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void setup_gsm() {
  Serial.println("Initializing...");
  delay(1000);
  mySerial.println("AT");  // handshake
  updateSerial();
  mySerial.println("AT+CSQ"); //Signal quality test
  updateSerial();
  mySerial.println("AT+CREG=0"); // network register
  updateSerial();
  mySerial.println("AT+CREG=1"); //network register
  updateSerial();
  mySerial.println("AT+CREG?"); //Check whether it has registered in the network
  updateSerial();
  mySerial.println("AT+CCID"); //sim id
  updateSerial();
  mySerial.println("AT+COPS?"); //available networks
  updateSerial();
  mySerial.println("AT+CGATT=1"); //gprs activation
  updateSerial();
  mySerial.println("AT+CGATT?"); //check gprs activation
  updateSerial();
  mySerial.println("AT+CLIP=1\r"); // enable caller id
  updateSerial();
}

void setup()
{
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);


  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
  setup_gsm();

  // We start by connecting to a WiFi network

  display.print("Connecting to..\n ");
  display.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
  }

  display.println("");
  display.println("WiFi connected..\n");
  display.println("IP address: ");
  display.println(WiFi.localIP());
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("TESTING TEMP SENSOR!\n");
  mlx.begin();
  //dht.begin();
  display.println("TESTING OXIMETER..\n");

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  display.println("TESTING LED..\n");

  pinMode(ledg, OUTPUT);
  pinMode(ledr, OUTPUT);
  digitalWrite(ledg, HIGH);
  digitalWrite(ledr, LOW);
  timeClient.begin();
  timeClient.setTimeOffset(19800);
  display.println("STARTING CLOCK..\n");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 0);

  // Initialize SD card
  pinMode(MOSI, INPUT_PULLUP);
  pinMode(SD_CS, OUTPUT);
  SD.begin(SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  File file = SD.open("/data.txt");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/user_2.txt", "Date, Time, Temperature, Pulse, Spo2 \r\n");
  }
  else {
    Serial.println("File already exists");
  }
  file.close();

}



void loop()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(5, 10);
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String dataString = "";
  String formattedDate = timeClient.getFormattedDate();

  int splitT = formattedDate.indexOf("T");
  String dayStamp = formattedDate.substring(0, splitT);
  display.println(dayStamp);
  display.print("\n");
  String  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
  display.setCursor(15, 40);
  display.println(timeStamp);
  display.print("\n ");
  display.display();
  dataString += String(dayStamp);
  dataString += ",";
  dataString += String(timeStamp);
  dataString += ",";
  delay(3000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  float temperature = mlx.readObjectTempF();
  display.print("Temperature: ");
  display.print(temperature);
  display.println("*F\n");
  dataString += String(temperature);
  dataString += ",";

  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    pulse=pox.getHeartRate();
    spo2=pox.getSpO2();
    display.print("Heart rate : ");
    display.print(pox.getHeartRate());
    display.print(" bpm \n\nSpO2 :");
    display.print(pox.getSpO2());
    display.println(" %");
    dataString += String(pulse);
    dataString += ",";
    dataString += String(spo2);
    dataString += " ";
    tsLastReport = millis();
  }
  display.setCursor(0, 50);
  display.setTextSize(2);
  display.print("ASHA");
  display.display();
  display.startscrollright(0x06, 0x07);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  delay(5000);
  display.stopscroll();
  String gsm_msg = dataString;
  dataString += "\r\n";

  Serial.println(dataString);
  appendFile(SD, "/user_2.txt", dataString.c_str());



  if (temperature < 96  || temperature > 99 || pulse < 60 || pulse > 90 || spo2 < 88) {
    digitalWrite(ledr, HIGH);
    digitalWrite(ledg, LOW);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 20);
    display.println("CALL TO....");
    display.setCursor(10, 35);
    display.println("+919102333381");
    display.display();
    mySerial.println("ATD+919102333381;\r\n");
    updateSerial();
    delay(60000);
    mySerial.println("ATH");
    delay(100);
    mySerial.print("AT+CMGF=1\r");
    delay(100);
    mySerial.println("AT+CMGS=\"+919102333381\"");
    delay(100);
    mySerial.println(String(formattedDate) + "\nTemp : " + String(temperature) + " *F\nPulse : " + String(pulse) + " bpm\nSpo2 : " + String(spo2) + " %");
    delay(100);
    mySerial.println((char)26);
    delay(100);
    mySerial.println();
    delay(1000);
  }
  else {
    digitalWrite(ledg, HIGH);
    digitalWrite(ledr, LOW);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connecting to ");
  display.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    display.println("connection failed");
    return;
    delay(500);
    display.clearDisplay();
    display.setCursor(0, 0);
  }

  String data_to_send = api_key;
  data_to_send += "&field1=";
  data_to_send += String(i++);
  data_to_send += "&field2=";
  data_to_send += String(temperature);
  data_to_send += "&field3=";
  data_to_send += String(pulse);
  data_to_send += "&field4=";
  data_to_send += String(spo2);
  data_to_send += "&field5=";
  data_to_send += String(formattedDate);
  data_to_send += "\r\n\r\n";

  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: " + api_key + "\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(data_to_send.length());
  client.print("\n\n");
  client.print(data_to_send);

  delay(500);
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 1000) {
      display.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    display.print(line);

  }

  display.println();
  display.println("closing connection");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setCursor(40, 28);
  display.setTextSize(2);
  display.println("ASHA");
  display.display();
  delay(900000);    //15 minutes delay

}
