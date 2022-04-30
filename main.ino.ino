#include "secret2.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <HTU21D.h>
#include <ArduinoJson.h>
#include <Adafruit_MCP9601.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
 
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
#define I2C_ADDRESS (0x67)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SDA 21  //OLED SSD1306 & INA219 pin
#define SCL 22  //OLED SSD1306 & INA219 pin
#define RelayPin 12

#define LED1_BIT 17
#define LED2_BIT 18
#define LED3_BIT 19
#define LED4_BIT 23
#define LED5_BIT 5
#define LED6_BIT 26
#define LED7_BIT 27
#define LED8_BIT 0

#define SW1_BIT 34
#define SW2_BIT 35
#define SW3_BIT 32
#define SW4_BIT 33
#define SW5_BIT 25

//Define Variables we'll be connecting to
float Setpoint, Input, Output;

int WindowSize = 5000;
unsigned long windowStartTime;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_MCP9601 mcp;
unsigned int desired_temp;
 
unsigned int drink_select = 1;
String drink_name = "none";
float t;

HTU21D sensor;
 
WiFiClientSecure wifi_client = WiFiClientSecure();
MQTTClient mqtt_client = MQTTClient(256); //256 indicates the maximum size for packets being published and received.
uint32_t t1;

/* This function will take in a pointer to char array
*  and print it to the OLED screen. We will use this
*  to provide visual feedback on currently selected
*  drink and it's target water temperature
*/
void testscrolltext(char * text) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.println(F(text));
  display.display();      // Show initial text
  
  //display.printf("Temp: %d degrees C",desired_temp);

  delay(1000);
}
 
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }


  // Configure wifi_client with the correct certificates and keys
  wifi_client.setCACert(AWS_CERT_CA);
  wifi_client.setCertificate(AWS_CERT_CRT);
  wifi_client.setPrivateKey(AWS_CERT_PRIVATE);

  //Connect to AWS IOT Broker. 8883 is the port used for MQTT
  mqtt_client.begin(AWS_IOT_ENDPOINT, 8883, wifi_client);

  //Set action to be taken on incoming messages
  mqtt_client.onMessage(incomingMessageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!mqtt_client.connected()) {
    mqttconnect();
    Serial.print(".");
    delay(100);
  }
  
  if(!mqtt_client.connected()){
    Serial.println("AWS IoT Timeout!");
    return;
  }

  //Subscribe to a topic
  mqtt_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["temperature"] = t;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  //Publish to the topic
  mqtt_client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  Serial.println("Sent a message");
}

void mqttconnect() {
  /* Loop until reconnected */
  while (!mqtt_client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "NathansESP32";
    /* connect now */
    if (mqtt_client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      mqtt_client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
    } else {
      Serial.print("failed, status code =");
      //Serial.print(mqtt_client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

/* This function will check the target water temperature
* and illuminate a number of LEDs depending on how hot it is */
void ShowLEDs(unsigned int howHot){
  switch(howHot){
    case 1:
      digitalWrite(LED8_BIT, HIGH);
      digitalWrite(LED7_BIT, LOW);
      digitalWrite(LED6_BIT, LOW);
      digitalWrite(LED5_BIT, LOW);
      digitalWrite(LED4_BIT, LOW);
      digitalWrite(LED3_BIT, LOW);
      digitalWrite(LED2_BIT, LOW);
      digitalWrite(LED1_BIT, LOW);
      drink_name = "Green Tea ";
      break;
    case 2:
      digitalWrite(LED8_BIT, HIGH);
      digitalWrite(LED7_BIT, HIGH);
      digitalWrite(LED6_BIT, LOW);
      digitalWrite(LED5_BIT, LOW);
      digitalWrite(LED4_BIT, LOW);
      digitalWrite(LED3_BIT, LOW);
      digitalWrite(LED2_BIT, LOW);
      digitalWrite(LED1_BIT, LOW);
      drink_name = "White Tea ";
      break;
    case 3:
      digitalWrite(LED8_BIT, HIGH);
      digitalWrite(LED7_BIT, HIGH);
      digitalWrite(LED6_BIT, HIGH);
      digitalWrite(LED5_BIT, LOW);
      digitalWrite(LED4_BIT, LOW);
      digitalWrite(LED3_BIT, LOW);
      digitalWrite(LED2_BIT, LOW);
      digitalWrite(LED1_BIT, LOW);
      drink_name = "Dark Roast ";
      break;
    case 4:
      digitalWrite(LED8_BIT, HIGH);
      digitalWrite(LED7_BIT, HIGH);
      digitalWrite(LED6_BIT, HIGH);
      digitalWrite(LED5_BIT, HIGH);
      digitalWrite(LED4_BIT, LOW);
      digitalWrite(LED3_BIT, LOW);
      digitalWrite(LED2_BIT, LOW);
      digitalWrite(LED1_BIT, LOW);
      drink_name = "Oolong Tea ";
      break;
    case 5:
      digitalWrite(LED8_BIT, HIGH);
      digitalWrite(LED7_BIT, HIGH);
      digitalWrite(LED6_BIT, HIGH);
      digitalWrite(LED5_BIT, HIGH);
      digitalWrite(LED4_BIT, HIGH);
      digitalWrite(LED3_BIT, LOW);
      digitalWrite(LED2_BIT, LOW);
      digitalWrite(LED1_BIT, LOW);
      drink_name = "Medium Roast ";
      break;
    case 6:
      digitalWrite(LED8_BIT, HIGH);
      digitalWrite(LED7_BIT, HIGH);
      digitalWrite(LED6_BIT, HIGH);
      digitalWrite(LED5_BIT, HIGH);
      digitalWrite(LED4_BIT, HIGH);
      digitalWrite(LED3_BIT, HIGH);
      digitalWrite(LED2_BIT, LOW);
      digitalWrite(LED1_BIT, LOW);
      drink_name = "Black Tea ";
      break;
    case 7:
      digitalWrite(LED8_BIT, HIGH);
      digitalWrite(LED7_BIT, HIGH);
      digitalWrite(LED6_BIT, HIGH);
      digitalWrite(LED5_BIT, HIGH);
      digitalWrite(LED4_BIT, HIGH);
      digitalWrite(LED3_BIT, HIGH);
      digitalWrite(LED2_BIT, HIGH);
      digitalWrite(LED1_BIT, LOW);
      drink_name = "Light Roast ";
      break;
    case 8:
      digitalWrite(LED8_BIT, HIGH);
      digitalWrite(LED7_BIT, HIGH);
      digitalWrite(LED6_BIT, HIGH);
      digitalWrite(LED5_BIT, HIGH);
      digitalWrite(LED4_BIT, HIGH);
      digitalWrite(LED3_BIT, HIGH);
      digitalWrite(LED2_BIT, HIGH);
      digitalWrite(LED1_BIT, HIGH);
      drink_name = "Herbal Tea ";
      break;
    default:
      printf("Temperature out of range\n");
      break;
  }
}

/* This function is an ISR that occurs every time a message is received over MQTT
* If the message contains ON, OFF, or a number 1-8 the ESP32 will react accordingly
Otherwise the message will be ignored */
void incomingMessageHandler(String &topic, String &payload) {
  Serial.println("Message received!");
  Serial.println("Topic: " + topic);

  Serial.println(payload);

  const char * s = payload.c_str();
  int i1;

  if(payload.indexOf("ON") > 0) {
    //Serial.println("on");
    digitalWrite(RelayPin, HIGH);
  }
  else if(payload.indexOf("OFF") > 0) {
    //Serial.println("off");
    digitalWrite(RelayPin, LOW);
  }
  else if(1 == sscanf(s,
                "%*[^0123456789]%d%",
                &i1))
  {
    printf("%d\n", i1);
    drink_select = i1;
    ShowLEDs(drink_select);          
  }
}

/* These next two ISR functions are hooked up to the pushbuttons
* I was using them for testing mostly */
void IRAM_ATTR RightISR() {
 // delay(100);
  if(drink_select<8){
    drink_select+=1;
  }
  ShowLEDs(drink_select);
}
void IRAM_ATTR LeftISR() {
  //delay(100);
  if(drink_select>1){
    drink_select-=1;
  }
  ShowLEDs(drink_select);
}


void setup() {
   Wire.begin(SDA, SCL); 
   Serial.begin(115200);

   pinMode(LED1_BIT, OUTPUT);
   pinMode(LED2_BIT, OUTPUT);
   pinMode(LED3_BIT, OUTPUT);
   pinMode(LED4_BIT, OUTPUT);
   pinMode(LED5_BIT, OUTPUT);
   pinMode(LED6_BIT, OUTPUT);
   pinMode(LED7_BIT, OUTPUT);
   pinMode(LED8_BIT, OUTPUT);
   pinMode(RelayPin, OUTPUT);

   pinMode(SW3_BIT, INPUT_PULLUP);
   attachInterrupt(SW3_BIT, RightISR, FALLING);
   pinMode(SW1_BIT, INPUT_PULLUP);
   attachInterrupt(SW1_BIT, LeftISR, FALLING);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

   display.fillScreen(BLACK);
   display.setTextColor(WHITE);
   display.setTextWrap(false);  
  
  t1 = millis();
    //Serial.begin(115200);
    while (!Serial) {
      delay(10);
    }
    Serial.println("Adafruit MCP9601 test");

    /* Initialise the driver with I2C_ADDRESS and the default I2C bus. */
    if (! mcp.begin(I2C_ADDRESS)) {
        Serial.println("Sensor not found. Check wiring!");
        while (1);
    }
  connectAWS();

  Serial.println("Found MCP9601!");

  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  Serial.print("ADC resolution set to ");
  switch (mcp.getADCresolution()) {
    case MCP9600_ADCRESOLUTION_18:   Serial.print("18"); break;
    case MCP9600_ADCRESOLUTION_16:   Serial.print("16"); break;
    case MCP9600_ADCRESOLUTION_14:   Serial.print("14"); break;
    case MCP9600_ADCRESOLUTION_12:   Serial.print("12"); break;
  }
  Serial.println(" bits");

  mcp.setThermocoupleType(MCP9600_TYPE_K);
  Serial.print("Thermocouple type set to ");
  switch (mcp.getThermocoupleType()) {
    case MCP9600_TYPE_K:  Serial.print("K"); break;
    case MCP9600_TYPE_J:  Serial.print("J"); break;
    case MCP9600_TYPE_T:  Serial.print("T"); break;
    case MCP9600_TYPE_N:  Serial.print("N"); break;
    case MCP9600_TYPE_S:  Serial.print("S"); break;
    case MCP9600_TYPE_E:  Serial.print("E"); break;
    case MCP9600_TYPE_B:  Serial.print("B"); break;
    case MCP9600_TYPE_R:  Serial.print("R"); break;
  }
  Serial.println(" type");

  mcp.setFilterCoefficient(3);
  Serial.print("Filter coefficient value set to: ");
  Serial.println(mcp.getFilterCoefficient());

  mcp.setAlertTemperature(1, 30);
  Serial.print("Alert #1 temperature set to ");
  Serial.println(mcp.getAlertTemperature(1));
  mcp.configureAlert(1, true, true);  // alert 1 enabled, rising temp

  mcp.enable(true);

  Serial.println(F("------------------------------"));

  display.ssd1306_command(SSD1306_DISPLAYOFF);
  delay(5000);
  display.ssd1306_command(SSD1306_DISPLAYON);
  display.println("Awake!");
  display.display();
  ShowLEDs(drink_select);
}

void loop() {
 //Check if temperature sensor is connected properly
  uint8_t status = mcp.getStatus();

  if (status & MCP9601_STATUS_OPENCIRCUIT) { 
    //Serial.println("Thermocouple open!"); 
    return; // don't continue, since there's no thermocouple
  }
  if (status & MCP9601_STATUS_SHORTCIRCUIT) { 
    //Serial.println("Thermocouple shorted to ground!"); 
    return; // don't continue, since the sensor is not working
  }

  Serial.println();

 //Get the actual water temp
  t = mcp.readThermocouple();
 // calculate the target temperature based on the drink selection
  desired_temp = 79+3*(drink_select-1);
  
  Serial.print("Hot Junction: "); Serial.println(mcp.readThermocouple());
  //Serial.print("Cold Junction: "); Serial.println(mcp.readAmbient());
  //Serial.print("ADC: "); Serial.print(mcp.readADC() * 2); Serial.println(" uV");

 // send the actual water temperature to AWS
  publishMessage();
  mqtt_client.loop();
  
/* Construct a message to be printed to the OLED screen
* to give visual feedback to the user about what the
* target water temperature is and which drink they picked */
  String test = drink_name + desired_temp + "C";
  char charBuf[50];
  test.toCharArray(charBuf,50);
  testscrolltext(charBuf);
 
 // Safety mechanism, just in case the laptop gets disconnected from AWS
  if (t>100){
    digitalWrite(RelayPin, LOW);
  }
}
