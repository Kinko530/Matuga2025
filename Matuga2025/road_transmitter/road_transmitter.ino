#include <HardwareSerial.h>
#include <ESP32Servo.h>
#define FLOW_SENSOR_PIN 13           // Pin connected to the flow sensor
#define block 15

HardwareSerial TxSerial(1);          // Initialize HardwareSerial on UART1
const int analogPin1 = 34;           // Pin connected to analog sensor 1
const int analogPin2 = 35;           // Pin connected to analog sensor 2

// Variables for sensor values
float level1;
float level2;

// Variables for flow rate calculation
volatile byte pulseCount;            // Number of pulses from the flow sensor
float flowRate;                      // Calculated flow rate in L/min
unsigned long totalMilliLitres;      // Total flow in mL
unsigned long previousMillis;        // Last time the flow rate was calculated
const float calibrationFactor = 4.5; // Calibration factor for flow sensor (adjust based on your sensor specs)
float level_1;
float level_2;
float threshold1 = 50.00;
float threshold2 = 25.00;
String str_tresh1;
String str_tresh2;
// Define output strings
String str_lev1;
String str_lev2;
String str_flowRate;
String str_out;
Servo myservo;

// Interrupt service routine for the flow sensor
void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);                       // Start Serial for debugging
  TxSerial.begin(2400, SERIAL_8N1, 16, 17);   // Start HardwareSerial on UART1
  myservo.attach(block);
  myservo.write(90);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);     // Set flow sensor pin as input
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);  // Attach interrupt to the flow sensor
  str_tresh1 = String(threshold1);
  str_tresh2 = String(threshold2);
  // Initialize flow rate variables
  pulseCount = 0;
  flowRate = 0.0;
  totalMilliLitres = 0;
  previousMillis = millis();
}

void loop() {
  // Calculate time interval for flow rate calculation
  unsigned long currentMillis = millis();                                                       
  if (currentMillis - previousMillis > 1000) {  // Update every 1 second
    // Read the analog values from the sensors
    level1 = analogRead(analogPin1);
    level_1 = map(level1, 0, 4095, 0, 100);
    level2 = analogRead(analogPin2);
    level_2 = map(level2, 0, 4095, 0, 100);

    // Calculate flow rate in L/min
    float pulseFrequency = pulseCount;  // Get pulse count for the last second
    pulseCount = 0;                     // Reset pulse count for next interval
    flowRate = (pulseFrequency / calibrationFactor);  // Flow rate in L/min

    // Calculate flow in milliliters and accumulate total
    float flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    // Update previousMillis for the next calculation interval
    
    previousMillis = currentMillis;

    // Convert sensor values and flow rate to strings
  }
    str_lev1 = String(level_1);
    str_lev2 = String(level_2);
    str_flowRate = String(flowRate);
    str_out = str_lev1 + "," + str_lev2 + "," + str_flowRate;

    // Transmit data
    const char* msg = str_out.c_str();
    Serial.println(msg);  // Print to Serial Monitor for debugging
    TxSerial.println(msg); // Send data over UART

    if (str_lev1 > str_tresh1 || str_lev2 > str_tresh1 || str_flowRate > str_tresh2){
      myservo.write(0);
    } else {
      myservo.write(90);
    }
  delay(100);  // Small delay to stabilize readings
}
