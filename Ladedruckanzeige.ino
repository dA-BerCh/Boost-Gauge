//Ladedruckanzeige 

//Hardware: Arduino Nano
          //128*32 I2C Display
          //Luftdruck Schleswig-Holstein
          //Werte in BAR


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define sensorPin A0

int OLED_RESET = 4;
Adafruit_SSD1306 display(OLED_RESET); //driver for the screen

// bar graph

float rawval = 0; // Setup raw sensor value
float barboost = 0; // Setup value for boost bar

// peak

int boostPeakReset = 4000; // time in milis to reset peak value
float boostPeak = 0.00;
float boostMax = 0.00;
unsigned long boostPeakTimer = 0;



// log

byte count;
byte sensorArray[128];
byte drawHeight;
boolean filled = 0; //decide either filled, or dot-display. 0==dot display.


void setup()
{
  Serial.begin(9600); // start monitoring raw voltage for calibration
  display.begin(SSD1306_SWITCHCAPVCC); // 3.3V power supply
  display.clearDisplay(); // Clear the display and ram
  display.display();
  delay(2000); // display Adafruit logo for 2 seconds
  for (count = 0; count <= 128; count++) //zero all elements
  {
    sensorArray[count] = 0;
  }
}


void loop() // Start loop
{


  
  int boostmbar = map(analogRead(sensorPin), 21, 961, 100, 2600);
  rawval = analogRead(0); // Read MAP sensor raw value on analog port 0
  barboost = (((rawval * 0.19) - 86.81) + 10); // Calculate boost value for the graph



  if (boostPeak < boostmbar && boostmbar > 0.50) {
    boostPeak = boostmbar;
    boostPeakTimer = millis();
    if (boostMax < boostPeak) {
      boostMax = boostPeak;
    }
  }
  else if (boostPeak > boostmbar && (millis() - boostPeakTimer) > boostPeakReset) {
    boostPeak = 0.00;
  }


  // log 

  drawHeight = map(analogRead(A0), 0, 1023, 0, 25 );  

  sensorArray[0] = drawHeight;

  for (count = 55; count <= 128; count++ )
  {
    if (filled == false)
    {
      display.drawPixel(count, 71 - sensorArray[count - 55], WHITE);
    }
    else
      display.drawLine(count, 1, count, 71 - sensorArray[count - 55], WHITE); 
  }

  for (count = 80; count >= 2; count--) // count down from 160 to 2
  {
    sensorArray[count - 1] = sensorArray[count - 2];
  }

  //display.drawLine(55, 43, 55, 64, WHITE);
  //display.setTextColor(WHITE);
  //display.setTextSize(3);
 // display.setCursor(0, 10);
 // display.println((boostmbar * 0.001) - 0.865);    // calibrated for a 2.5 bar sensor in Denver (+/- 1psi)


  display.fillRect(0, 0, barboost, 4, WHITE); // Draws the bar depending on the sensor value

  //display.setTextColor(WHITE);
 // display.setTextSize(1);
  //display.setCursor(97, 15);
  //display.println("BOOST");


  display.setTextSize(1); //Anzeige für Spitzendruck
  display.setCursor(97, 15);
  display.println("MAX");
  
  display.setTextSize(1); //Anzeige für Spitzendruck
  display.setCursor(97, 25);
  display.println((boostPeak * 0.001) - 1.012); // 0.97 = 970mbar atmospheric pressure correction

  

  if (((boostmbar  * 0.001) - 1.012) < 0) {
    
    
    
    display.setCursor(0, 10);
    display.setTextSize(3);
    display.println(((boostmbar * 0.001) - 1.012)*63.2);

  }
  else if (((boostmbar * 0.001) - 1.012) > 0) {
    
    
    
    display.setTextColor(WHITE);
    display.setTextSize(3);
    display.setCursor(0, 10);
    display.println((boostmbar * 0.001) - 1.012);    //Kalibriert für einen 2,5 BAR Sensor in Schleswig-Holstein (Wert 2 für Druckanpassung)


}

  delay(1);
  display.display();
  display.clearDisplay();

  Serial.print("Input from A0 is ");
  Serial.println(rawval);    // Schreibt den Ausgangswert des Sensors (0-100%)

  delay(10); // Halbe Sekunde Delay zwischen den Zahlen
}
