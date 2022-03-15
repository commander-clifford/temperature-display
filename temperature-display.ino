#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>

// Display
#define SCREEN_ADDRESS 0x3C // OLED I2C Address
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Temp/Humid Sensor
#define SENSOR_ADDRESS 0x38; // Sensor I2C Address
Adafruit_AHTX0 aht;
int TEMP = 0;
int HMDTY = 0;
bool calcFahrenheit = true;

// Button
// constants won't change. They're used here to set pin 
const int buttonPin = 16;    // the number of the pushbut

// Variables will change:
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void update_display() {

  display.clearDisplay(); // Clear
  display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
  display.cp437(true); // Use full 256 char 'Code Page 437' font

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temp: ");

  display.fillCircle(12, 19, 5, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK); // Draw white text (white ~= LED-ON)
  display.setCursor(10, 16);
  if (calcFahrenheit){ display.print("F"); } 
  if (!calcFahrenheit){ display.print("C"); }
  
  display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
  display.setTextSize(4);
  display.setCursor(32, 0);
  display.print(TEMP);
  display.drawCircle(82, 2, 2, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(92, 0);
  display.print("Humid:");

  display.setTextSize(2);
  display.setCursor(102, 12);
  display.print(HMDTY);

  display.display();

}

void setup(void) {

  Wire.begin();
  Serial.begin(115200);

  if (! aht.begin()) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Could not find AHT? Check wiring");
    display.display();
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  pinMode(buttonPin, INPUT);

  display.clearDisplay(); // Clear
  display.setTextColor(SSD1306_WHITE); // Draw white text (white ~= LED-ON)
  display.cp437(true); // Use full 256 char 'Code Page 437' font
  display.dim(false);

}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  if (calcFahrenheit){ TEMP = (temp.temperature * 1.8) + 32;} 
  if (!calcFahrenheit){ TEMP = temp.temperature; }
  HMDTY = humidity.relative_humidity;

  update_display();

  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
        calcFahrenheit = !calcFahrenheit;
      }
    }
  }

  lastButtonState = reading;
}
