#include <Wire.h>

#include <Adafruit_MLX90614.h>          //body temprature sensor

#include <MAX30100_PulseOximeter.h>     //heart rate and o2 levels 
#define REPORTING_PERIOD_MS 1000


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif18pt7b.h>       // setting the fonts
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


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



#define TINY_GSM_MODEM_SIM800 //definition of the modem used (SIM800L)
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
  // we wait until there is a response from the SIM800L
  while (!SerialGSM.available());
  response = SerialGSM.readString();
  return response;
}


// initializes GSM
void setupGSM()
{
  display.println("Setup GSM...");
  // initializing serial SIM800L
  SerialGSM.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN, false);
  delay(3000);
  // displays modem info on the serial monitor
  Serial.println(modemGSM.getModemInfo());
  // inicia o modem
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
  if (sendAT("AT+CMGF=1").indexOf("OK") < 0) //Sets SMS mode to text (0 = PDU mode, 1 = Text mode)
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

  // switching on the leds
  pinMode(led_green, green_state);
  pinMode(led_red, red_state);


  //check for the display working
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
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
  delay(200);


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


  // assigns the current time before entering the loop to the timekeeping variables
  millisRefCon = millisUserResp = millis();


  // inicia e configura o SIM800L
  setupGSM();
  display.println("GPRS: Connected");

  display.display();
  delay(200);

}






void loop()

{
  pox.update();                               // Updating the pusle oximeter asap
  display.clearDisplay();



  // checking for normal/ high/ low body temprature
  if (mlx.readAmbientTempF()>0)
  {
    if (mlx.readAmbientTempF() > 100)
    {
      display.print("HIGH_TEMP..!! = ");
      digitalWrite(green_state, LOW);
      digitalWrite(red_state, HIGH);
      modemGSM.sendSMS(numbers[0],"HIGH TEMP : " + String(mlx.readAmbientTempF()) + " *F");
    }
    else if (mlx.readAmbientTempF() < 97)
    {
      display.print("LOW_TEMP..!! = ");
      digitalWrite(green_state, LOW);
      digitalWrite(red_state, HIGH);
      modemGSM.sendSMS(numbers[0],"HIGH TEMP : " + String(mlx.readAmbientTempF()) + " *F");
    }
    else
    {
      display.print("TEMP = ");
      digitalWrite(green_state, HIGH);
      digitalWrite(red_state, LOW);
    }
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
        modemGSM.sendSMS(numbers[0],"HIGH PULSE : " + String(pox.getHeartRate()) + " bpm");
      }
      else if (pox.getHeartRate() < 60)
      {
        display.println("LOW_PULSE..!! = ");
        digitalWrite(green_state, LOW);
        digitalWrite(red_state, HIGH);
        modemGSM.sendSMS(numbers[0],"LOW PULSE : " + String(pox.getHeartRate()) + " bpm");
      }
      else
      {
        display.println("Heart rate = ");
        digitalWrite(green_state, HIGH);
        digitalWrite(red_state, LOW);
      }
    }
    display.print(pox.getHeartRate());          // Checking the beats per min
    display.print(" bpm");


    // Checking for the correct SpO2 levels
    if (pox.getSpO2()>0)
    {
      if (pox.getSpO2() < 88)
      {
        display.println("LOW_SpO2..!! = ");
        digitalWrite(green_state, LOW);
        digitalWrite(red_state, HIGH);
        modemGSM.sendSMS(numbers[0],"LOW O2 LEVEL : " + String(pox.getSpO2()) + " %");
      }
      else
      {
        display.println("SpO2 level =  ");
        digitalWrite(green_state, HIGH);
        digitalWrite(red_state, LOW);
      }
    }
    display.print(pox.getSpO2());              // Checking for the SPO2 levels
    display.print(" %");

    tsLastReport = millis();
  }
  display.display();
  delay(600000);

}
