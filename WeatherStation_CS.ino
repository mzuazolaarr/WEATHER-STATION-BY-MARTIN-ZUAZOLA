#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

DHT dht(2, DHT11);
LiquidCrystal lcd(12, 11, 4, 5, 6, 7);

int buttonPin = 8;      // Pin del pulsador
int buttonState = HIGH; // Estado actual del pulsador
int lastButtonState = HIGH; // Estado previo del pulsador
bool displayData = false; // Estado de visualización de datos

char ssid[] = "vodafoneA4CA";     
char password[] = "AGQR5MRDF2G3NM";
char server[] = "api.thingspeak.com";  // Servidor ThingSpeak
String apiKey = "0JYAZVYUR19R0B4G";   // Clave de escritura del canal ThingSpeak

WiFiClient client;
HttpClient http(client, server);

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  dht.begin();
  lcd.begin(16, 2);
  lcd.print("Weather Station");
  pinMode(buttonPin, INPUT_PULLUP); // Configurar el pin del pulsador como entrada con resistencia pull-up
  connectWiFi();
}

void loop() {
  // Leer el estado del pulsador
  buttonState = digitalRead(buttonPin);

  // Si hay un cambio de estado del pulsador
  if (buttonState != lastButtonState) {
    // Si se ha pulsado el botón
    if (buttonState == LOW) {
      displayData = !displayData; // Cambiar el estado de visualización de datos
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

  // Actualizar el estado previo del pulsador
  lastButtonState = buttonState;

  // Mostrar datos en el LCD si está habilitada la visualización
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