#include <Wire.h>

#include <Adafruit_MLX90614.h>          //body temprature sensor

#include <MAX30100_PulseOximeter.h>     //heart rate and o2 levels 
#define REPORTING_PERIOD_MS 1000


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif18pt7b.h>       // setting the fonts
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


 
//using the body temprature sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// measuring the max30100 heart rate and oxygen level conc(SPO2 level)
PulseOximeter pox;
uint32_t tsLastReport = 0; 


// declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);





void setup()

{
Serial.begin(115200);                 // baud rate of 115200 ms



//check for the display working
if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
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
if(!pox.begin()) 
  {
  display.println("FAILED OXIMETER");
  for(;;);
  } 
else 
  {
  display.println("Initializing pulse oximeter..");
  }
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
    if (mlx.readAmbientTempF()>99)
        {
          display.print("HIGH_TEMP..!! = ");
        }
    else if (mlx.readAmbientTempF()<97)
        {
          display.print("LOW_TEMP..!! = ");
        }
    else
        {
          display.print("TEMP = ");
        }        
    display.print(mlx.readAmbientTempF());      // Checking for the temprature
    display.print("*F \n");
    }


//Checking for the last reporting period
if (millis() - tsLastReport > REPORTING_PERIOD_MS)  
{
  // Checking for the correct heart rate
  if(pox.getHeartRate()>0)
    {
      if (pox.getHeartRate()>100)
        {
          display.println("HIGH_PULSE..!! = ");
        }
      else if (pox.getHeartRate()<60)
        {
          display.println("LOW_PULSE..!! = ");
        }
      else
        {
          display.println("Heart rate = ");
        }
    }
  display.print(pox.getHeartRate());          // Checking the beats per min
  display.print(" bpm");
  
  
  // Checking for the correct SpO2 levels
  if(pox.getSpO2()>0)
    {
      if (pox.getSpO2()>100)
        {
          display.println("HIGH_SpO2..!! = ");
        }
      else if (pox.getSpO2()<88)
        {
          display.println("LOW_SpO2..!! = ");
        }
      else
        {
          display.println("SpO2 level =  ");
        }
    }
  display.print(pox.getSpO2());              // Checking for the SPO2 levels
  display.print(" %");
 

 tsLastReport = millis();
}
display.display();        
delay(1000);

}
