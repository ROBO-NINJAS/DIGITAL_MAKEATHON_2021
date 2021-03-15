#include <Wire.h>
#include<SoftwareSerial.h>

#include <Adafruit_MLX90614.h>          //body temprature sensor

#include <MAX30100_PulseOximeter.h>     //heart rate and o2 levels 
#define REPORTING_PERIOD_MS 1000


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif18pt7b.h>       // setting the fonts
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#include <SD.h>     //library for sd card
#include <SPI.h>


#define CSpin 5
File sensorData;  //file to which the data is going to be written


char incoming_char = 0;
int buttonStateP = 0;
int buttonStateD = 0;
int buttonStateH = 0;
const int buttonP = 34;
const int buttonD = 35;
const int buttonH = 32;
const int led_red = 26;  // 26 corresponds to GPIO26
const int led_green = 27; // 27 corresponds to GPIO27
const int green_state = HIGH;
const int red_state = LOW;


//using the body temprature sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// measuring the max30100 heart rate and oxygen level conc(SPO2 level)
PulseOximeter pox;
uint32_t tsLastReport = 0;


// declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


#define TINY_GSM_MODEM_SIM800 // definition of the modem used (SIM800L)
#include <TinyGsmClient.h>// library with GSM commands
#include <Fonts/FreeSans9pt7b.h> //font used on the display

// SIM800L serial communication object
HardwareSerial SerialGSM(1);

// library object with GSM functions
TinyGsm modemGSM(SerialGSM);

long int millisRefCon, millisUserResp;



// pins where the relays will be connected and RX / TX where the SIM800L will be connected
const int  RX_PIN = 16, TX_PIN = 17;

//Vivo access point name
const char *APN = "zap.vivo.com.br";
//User, if it does not exist, leave it empty
const char *USER = "";
//Password, if it does not exist, leave it empty
const char *PASSWORD = "";



// the variables below used by the loop function
// flag that indicates if, after the connection made by SIM800L, a user answered with an SMS within 1min
bool userResponseSMS = false;

// index of cell numbers vector, used to scroll through the vector
int i = 0;


// number of cell phones that will receive messages and calls and will be able to send SMS commands
const int numbersTL = 2;
// number of cell phones, the call order by the program is from left to right
const String numbers[numbersTL] = {"+91XXXXXXXXX", "+91XXXXXXXXX"};

void resetDisplay()
{
  display.clearDisplay();
  display.setCursor(0, 1);
}


//Sends AT command and waits for a response to be obtained
String sendAT(String command)
{
  String response = "";
  SerialGSM.println(command);
  //we wait until there is a response from the SIM800L
  while (!SerialGSM.available());
  response = SerialGSM.readString();
  return response;
}


// initializes GSM
void setupGSM()
{
  display.println("Setup GSM...");
  // starts SIM800L serial
  SerialGSM.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN, false);
  delay(3000);
  // displays modem info on the serial monitor
  Serial.println(modemGSM.getModemInfo());
  // start the modem
  if (!modemGSM.restart())
  {
    display.println("Restarting GSM\nModem failed");
    delay(10000);
    ESP.restart();
    return;
  }
  display.println("Modem restart OK");
  display.println("Modem network OK");
  if (!modemGSM.gprsConnect(APN, USER, PASSWORD)) // connect to the network (GPRS technology)
  {
    display.println("GPRS Connection\nFailed");
    delay(10000);
    ESP.restart();
    return;
  }
  display.println("GPRS Connect OK");
  if (sendAT("AT+CMGF=1").indexOf("OK") < 0) //Defines SMS mode for text (0 = PDU mode, 1 = Text mode)
  {
    display.println("SMS Txt mode Error");
    delay(10000);
    ESP.restart();
    return;
  }
  display.println("SMS Txt mode OK");
  //Deletes all stored SMS
  sendAT("AT + CMGD=1,4");
  resetDisplay();
}



void setup()

{
  Serial.begin(115200);                 // baud rate of 115200 ms

  pinMode(buttonP, INPUT);
  pinMode(buttonD, INPUT);
  pinMode(buttonH, INPUT);
  pinMode(led_green, green_state);        // switching on the leds
  pinMode(led_red, red_state);


  //check for the display working
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))     // Address 0x3D for 128x64
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(1000);
  display.clearDisplay();

  display.setFont(&FreeSerif18pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println("Initialized display..");     //checked for display
  display.display();
  delay(2000);


  // check for the infrared temprature sensor
  mlx.begin();
  display.println("Initializing body temp..");   // checked for temp sen
  display.display();
  delay(200);


  // check for the pulse oximeter for working
  if (!pox.begin())
  {
    display.println("FAILED OXIMETER");
    for (;;);
  }
  else
  {
    display.println("Initializing pulse oximeter..");
  }
  display.display();



  // check for the working of SD card
  display.print("Initializing SD card...");
  pinMode(CSpin, OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin(CSpin)) {
    display.println("Card failed, or not present");
    return;         // don't do anything more:
  }
  display.println("card initialized.");
  display.display();



  // assigns the current time before entering the loop to the timekeeping variables
  millisRefCon = millisUserResp = millis();


  // starts and configures the SIM800L
  setupGSM();
  display.println("GPRS: Connected");
  sendAT("AT+CLIP=1\r");
  display.print("Caller ID Active");

  display.display();
  delay(200);

}




void saveData(String dataString) {
  if (SD.exists("data.csv")) { // check the card is still there
    // now append new data file
    sensorData = SD.open("data.csv", FILE_WRITE);
    if (sensorData) {
      sensorData.println(dataString);
      sensorData.close(); // close the file
    }
  }
  else {
    display.println("Error writing to file !");
  }
}




void Location_link()
{ String Longitude,responce,Latitude,Link="https://www.google.com/maps/place/";
  
  responce=sendAT("AT+CIPGSMLOC=1,1");
  //Sample Output for AT+CIPGSMLOC=1,1   ==> +CIPGSMLOC: 0,75.802460,26.848892,2019/04/23,08:32:35 //where 26.8488832 is Lattitude and 75.802460 is longitute
  int first_comma = responce.indexOf(','); //Find the position of 1st comma
  int second_comma = responce.indexOf(',', first_comma+1); //Find the position of 2nd comma
  int third_comma = responce.indexOf(',', second_comma+1); //Find the position of 3rd comma

  for(int i=first_comma+1; i<second_comma; i++) //Values form 1st comma to 2nd comma is Longitude
    Longitude = Longitude + responce.charAt(i);

  for(int i=second_comma+1; i<third_comma; i++) //Values form 2nd comma to 3rd comma is Latitude
    Latitude = Latitude + responce.charAt(i);

  Link = Link + Latitude + "," + Longitude; //Update the Link with latitude and Logitude values 

  modemGSM.sendSMS(numbers[0],Link);
} 




void loop()

{
  pox.update();                         // Updating the pusle oximeter asap

  display.clearDisplay();
  String dataString = "";           // holds the data to be written to the SD card
  



  // checking for normal/ high/ low body temprature
  if (mlx.readAmbientTempF()>0)
  {
    if (mlx.readAmbientTempF() > 100)
    {
      display.print("HIGH_TEMP..!! = ");
      digitalWrite(green_state, LOW);
      digitalWrite(red_state, HIGH);
      Location_link();
      modemGSM.sendSMS(numbers[0], "HIGH TEMP : " + String(mlx.readAmbientTempF()) + " *F");
    }
    else if (mlx.readAmbientTempF() < 97)
    {
      display.print("LOW_TEMP..!! = ");
      digitalWrite(green_state, LOW);
      digitalWrite(red_state, HIGH);
      Location_link();
      modemGSM.sendSMS(numbers[0], "HIGH TEMP : " + String(mlx.readAmbientTempF()) + " *F");
    }
    else
    {
      display.print("TEMP = ");
      digitalWrite(green_state, HIGH);
      digitalWrite(red_state, LOW);
    }
    dataString.concat(String(mlx.readAmbientTempF()));
    dataString.concat(",");
    display.print(mlx.readAmbientTempF());      // Checking for the temprature
    display.print("*F \n");
  }


  //Checking for the last reporting period
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    // Checking for the correct heart rate
    if (pox.getHeartRate()>0)
    {
      if (pox.getHeartRate() > 90)
      {
        display.println("HIGH_PULSE..!! = ");
        digitalWrite(green_state, LOW);
        digitalWrite(red_state, HIGH);
        Location_link();
        modemGSM.sendSMS(numbers[0], "HIGH PULSE : " + String(pox.getHeartRate()) + " bpm");
      }
      else if (pox.getHeartRate() < 60)
      {
        display.println("LOW_PULSE..!! = ");
        digitalWrite(green_state, LOW);
        digitalWrite(red_state, HIGH);
        Location_link();
        modemGSM.sendSMS(numbers[0], "LOW PULSE : " + String(pox.getHeartRate()) + " bpm");
      }
      else
      {
        display.println("Heart rate = ");
        digitalWrite(green_state, HIGH);
        digitalWrite(red_state, LOW);
      }
      dataString.concat(String(pox.getHeartRate()));
      dataString.concat(",");
      display.print(pox.getHeartRate());          // Checking the beats per min
      display.print(" bpm");
    }


    // Checking for the correct SpO2 levels
    if (pox.getSpO2()>0)
    {
      if (pox.getSpO2() < 88)
      {
        display.println("LOW_SpO2..!! = ");
        digitalWrite(green_state, LOW);
        digitalWrite(red_state, HIGH);
        sendAT("AT+CIPGSMLOC=1,1");       // sends the latitude as well as the date and time
        Location_link();
        modemGSM.sendSMS(numbers[0], "LOW O2 LEVEL : " + String(pox.getSpO2()) + " %");
      }
      else
      {
        display.println("SpO2 level =  ");
        digitalWrite(green_state, HIGH);
        digitalWrite(red_state, LOW);
      }
      dataString.concat(String(pox.getSpO2()));
      dataString.concat(",");
      display.print(pox.getSpO2());              // Checking for the SPO2 levels
      display.print(" %");
    }
    tsLastReport = millis();
  }

  display.display();
  saveData(dataString);
  delay(300000);
  buttonStateD = digitalRead(buttonD);
  // if the dial button is high dial the guardian
  if (buttonStateD == HIGH) {
    sendAT("ATD + numbers[0];");
    buttonStateH = digitalRead(buttonH);
    if (buttonStateH == HIGH) {
      sendAT("ATH");
    }
  }
  // here it checks whether the user wants to pick up the call or not
  buttonStateP = digitalRead(buttonP);
  if (buttonStateP == HIGH) {
    sendAT("ATA\r");
    delay(30000);               // user can have a 30 sec call
    if (buttonStateH == HIGH) {
      sendAT("ATH");
    }
  }

  // checks if he wants to delete the call or not
  if (buttonStateH == HIGH) {
    sendAT("ATH");
  }


}
