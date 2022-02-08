#include <WiFi.h>
#include <PubSubClient.h>
#include <HX711_ADC.h>

#define ssid ""
#define pass ""
byte mqtt_server[] = {...,...,..,.};

WiFiClient espClient;
PubSubClient client(espClient);

//pins:
const int HX711_dout1 = 13; //mcu > HX711 dout pin
const int HX711_sck1 = 12; //mcu > HX711 sck pin

const int HX711_dout2 = 14; //mcu > HX711 dout pin
const int HX711_sck2 = 27; //mcu > HX711 sck pin

const int HX711_dout3 = 26; //mcu > HX711 dout pin
const int HX711_sck3 = 25; //mcu > HX711 sck pin

const int HX711_dout4 = 33; //mcu > HX711 dout pin
const int HX711_sck4 = 32; //mcu > HX711 sck pin

static char outstr[12];
//HX711 constructor:
HX711_ADC LoadCell1(HX711_dout1, HX711_sck1);
HX711_ADC LoadCell2(HX711_dout2, HX711_sck2);
HX711_ADC LoadCell3(HX711_dout3, HX711_sck3);
HX711_ADC LoadCell4(HX711_dout4, HX711_sck4);

const int calVal_eepromAdress = 0;
unsigned long t = 0;
///////////////////////////////////////////////////////////
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg1[50], msg2[50], msg3[50], msg4[50];
float berat1, berat2, berat3, berat4;

void setup_wifi(){
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Message arrive [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }

  Serial.println();
}

void reconnect() {
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");
    String clientId = "espClient";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())){
      Serial.print("connected");
      client.publish("jumlahBarang1", "cek 1");
      client.publish("jumlahBarang2", "cek 2");
      client.publish("jumlahBarang3", "cek 3");
      client.publish("jumlahBarang4", "cek 4");
   
    }
    else{
      Serial.print("failed, ro-");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); delay(10);
  Serial.println();
  Serial.println("Starting...");
  setup_wifi();
  client.setServer(mqtt_server, 1885);
  client.setCallback(callback);

  LoadCell1.begin();
  LoadCell2.begin();
  LoadCell3.begin();
  LoadCell4.begin();
  
  float calibrationValue1;
  float calibrationValue2;
  float calibrationValue3;
  float calibrationValue4;
  
  // calibration value (see example file "Calibration.ino")
  calibrationValue1 = 362.12; 
  calibrationValue2 = 326.72; 
  calibrationValue3 = -122.81; 
  calibrationValue4 = 345.98; 

  
#if defined(ESP8266)|| defined(ESP32)
#endif
  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell1.start(stabilizingtime, _tare);
  if (LoadCell1.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell1.setCalFactor(calibrationValue1); // set calibration value (float)
    Serial.println("Startup 1 i's complete");
  }


 LoadCell2.start(stabilizingtime, _tare);
  if (LoadCell2.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell2.setCalFactor(calibrationValue2); // set calibration value (float)
    Serial.println("Startup 2 i's complete");
  }


 LoadCell3.start(stabilizingtime, _tare);
  if (LoadCell3.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell3.setCalFactor(calibrationValue3); // set calibration value (float)
    Serial.println("Startup 3 i's complete");
  }


 LoadCell4.start(stabilizingtime, _tare);
  if (LoadCell4.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell4.setCalFactor(calibrationValue4); // set calibration value (float)
    Serial.println("Startup 4 i's complete");
  }
}


int SH=0;

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static boolean newDataReady1 = 0;
  static boolean newDataReady2 = 0;
  static boolean newDataReady3 = 0;
  static boolean newDataReady4 = 0;
          
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell1.update()) newDataReady1 = true;
  if (LoadCell2.update()) newDataReady2 = true;
  if (LoadCell3.update()) newDataReady3 = true;
  if (LoadCell4.update()) newDataReady4 = true;


  unsigned long now = millis();
  if(now - lastMsg >500){
    float i1 = LoadCell1.getData();
    float i2 = LoadCell2.getData();
    float i3 = LoadCell3.getData();
    float i4 = LoadCell4.getData();
    
    sprintf(msg1, "aqua:%f", i1);
    sprintf(msg2, "sunlight:%f", i2);
    sprintf(msg3, "rusak", i3);
    sprintf(msg4, "belibis:%f", i4);

    
    Serial.print("Load_cell output val1: ");
    Serial.println(msg1);
    client.publish("jumlahBarang1", msg1);

    Serial.print("Load_cell output val2: ");
    Serial.println(msg2);
    client.publish("jumlahBarang2", msg2);

    Serial.print("Load_cell output val3: ");
    Serial.println(msg3);
    client.publish("jumlahBarang3", msg3);

    Serial.print("Load_cell output val4: ");
    Serial.println(msg4);
    client.publish("jumlahBarang4", msg4);

    lastMsg = now;
  }
}
