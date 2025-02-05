#include <LiquidCrystal_I2C.h>
#include <HardwareSerial.h>
#define relay 15
HardwareSerial RxSerial(1); // Initialize HardwareSerial on UART1

// Define output variables to store parsed values
String str_lev1;
String str_lev2;
String str_flowRate;
String str_out;
float threshold1 = 50;
float threshold2 = 25;
String str_tresh1;
String str_tresh2;
int lev1;
int lev2;
int floRate;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(115200); // Start Serial for debugging
  lcd.init();
  lcd.backlight();
  pinMode(relay, OUTPUT);
  str_tresh1 = String(threshold1);
  str_tresh2 = String(threshold2);
  RxSerial.begin(2400, SERIAL_8N1, 16, 17);   // Start HardwareSerial on UART1
  Serial.println("READY TO RECEIVE");         // Print a message indicating the receiver is ready
  digitalWrite(relay, HIGH);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("BRIDGE VEHICLE");
  lcd.setCursor(1,1);
  lcd.print("IMMOBILIZATION");
}

void loop() {
  // Check if data is available to read
  while (RxSerial.available()) {
    char c = RxSerial.read();  // Read a single character
    str_out += c;              // Accumulate characters into str_out
    
    // Check if end of message (newline character) is reached
    
    if (c == '\n') {
      int firstComma = str_out.indexOf(',');
      int secondComma = str_out.indexOf(',', firstComma + 1);

      if (firstComma != -1 && secondComma != -1) {
        // Extract each value based on the comma positions
        str_lev1 = str_out.substring(0, firstComma);
        str_lev2 = str_out.substring(firstComma + 1, secondComma);
        str_flowRate = str_out.substring(secondComma + 1, str_out.length() - 1); // Exclude newline character
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("LV1:");
        lcd.print(int(str_lev1.toFloat()));
        lcd.print("%");
        lcd.print(" LV2:");
        lcd.print(int(str_lev2.toFloat()));
        lcd.print("%");
        lcd.setCursor(0,1);
        lcd.print("FLOW RATE:");
        lcd.print(str_flowRate.toFloat());
      }
    if (str_lev1 > str_tresh1 || str_lev2 > str_tresh1 || str_flowRate > str_tresh2){
      digitalWrite(relay, LOW);
    } else {
      digitalWrite(relay, HIGH);
    }
      // Print the results
      Serial.print("Water Level 1: ");
      Serial.print(str_lev1);
      Serial.print(" || Water Level 2: ");
      Serial.print(str_lev2);
      Serial.print(" || Flow Rate (L/min): ");
      Serial.println(str_flowRate);

      // Clear str_out for the next message
      str_out = "";
    }
  }
}
