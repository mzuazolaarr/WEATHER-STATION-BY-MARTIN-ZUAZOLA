#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

DHT dht(2, DHT11);
LiquidCrystal lcd(12, 11, 4, 5, 6, 7);

int buttonPin = 8;      // Buttom pin
int buttonState = HIGH; // Current status of the button
int lastButtonState = HIGH; // Previous button status
bool displayData = false; // Data display status

char ssid[] = "vodafoneA4CA";     
char password[] = "AGQR5MRDF2G3NM";
char server[] = "api.thingspeak.com";  // ThingSpeak Server
String apiKey = "0JYAZVYUR19R0B4G";   // ThingSpeak Channel Write Key

WiFiClient client;
HttpClient http(client, server);

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  dht.begin();
  lcd.begin(16, 2);
  lcd.print("Weather Station");
  pinMode(buttonPin, INPUT_PULLUP); // Configure push-button pin as input with pull-up resistor
  connectWiFi();
}

void loop() {
  // Read pushbutton status
  buttonState = digitalRead(buttonPin);

  // If there is a change of status of the push button
  if (buttonState != lastButtonState) {
    // If the button has been pressed
    if (buttonState == LOW) {
      displayData = !displayData; // Change the data display status
      if (displayData) {
        Serial.println("ON");
      } else {
        Serial.println("OFF");
        lcd.clear();
        digitalWrite(13, LOW);
        lcd.print("Weather Station");
      }
    }
  }

  // Update the previous status of the pushbutton
  lastButtonState = buttonState;

  // Display data on the LCD if display is enabled
  if (displayData) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    //Read MQ2
    int gasvalue = analogRead(A4);

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print("C  ");

    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");

    if (temperature < 30.00) {
      digitalWrite(13, HIGH);
      } else {
      digitalWrite(13, LOW);
      }
  
  sendDataToThingSpeak(temperature, humidity, gasvalue);
  }
  delay(2000);
}


void connectWiFi() {
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conexión WiFi exitosa");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}
void sendDataToThingSpeak(float temperature, float humidity, int gasvalue) {
  String url = "/update?api_key=" + apiKey + "&field1=" + String(temperature) + "&field2=" + String(humidity) + "&field3=" + String(gasvalue);
  
  Serial.print("Enviando datos a ThingSpeak: ");
  Serial.println(url);

  http.beginRequest();
  http.get(url);
  http.endRequest();
}