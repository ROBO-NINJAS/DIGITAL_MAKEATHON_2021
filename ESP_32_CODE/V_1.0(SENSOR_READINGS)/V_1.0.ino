#include <Wire.h>

#include <Adafruit_MLX90614.h>          //body temprature sensor
#include <MAX30100_PulseOximeter.h>     //heart rate and o2 levels 
#define REPORTING_PERIOD_MS 1000

 
//using the body temprature sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// measuring the max30100 heart rate and oxygen level conc(SPO2 level)
PulseOximeter pox;
uint32_t tsLastReport = 0; 





void setup()

{
Serial.begin(115200);                       // baud rate of 115200 ms
mlx.begin();
Serial.print("Initializing body temp.."); 


// check for the pulse oximeter for working
if(!pox.begin()) {
  Serial.println("FAILED OXIMETER");
  for(;;);
  } 
else {
  Serial.println("Initializing pulse oximeter..");
  }
}




 
void loop()

{
pox.update()
;                               // Updating the pusle oximeter asap
Serial.print("TEMP= ");
Serial.print(mlx.readAmbientTempF());       // Checking for the temprature
Serial.print("*F \n");

if (millis() - tsLastReport > REPORTING_PERIOD_MS)  //Checking for the last reporting period
{
  Serial.print("Heart rate - ");
  Serial.print(pox.getHeartRate());         // Checking the beats per min
  Serial.print("bpm \n SpO2 level - ");
  Serial.print(pox.getSpO2());              // Checking for the SPO2 levels
  Serial.print("% \n");
 tsLastReport = millis();
}


delay(1000);

}

 

