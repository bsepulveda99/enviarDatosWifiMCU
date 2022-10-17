#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

//red wifi
const char* ssid = "REDWIFI";
const char* password = "PASSWIFI";

//Datos del servidor.
const char* host = "ipservidor";
const int port = 80;
const int refreshclock = 1000;
unsigned long previousMillis = millis();


String line;
String var;

void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.print("Conectando ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado a la red ");
  Serial.println(ssid);
  Serial.print("IP obtenida: ");
  Serial.println(WiFi.localIP());
  delay(5000);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}


//Calcula la calidad de la señal -1 a 100
int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}


void loop() {

  //Almacena la calidad de la señal en un variable de int
  int calidad = getQuality();
  Serial.println(String(calidad));

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > refreshclock) {
    previousMillis = currentMillis;
    WiFiClient client;

    if (!client.connect(host, port)) {
      Serial.println("conexion fallida");
      return;
    }

    String url = "inacap/index.php?senal=";
    //IF de calculo de extraccion
    url += String(calidad);
 
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available()) {

      line = client.readStringUntil('\r');
    }

    Serial.println(line);
  }

  delay(5000);
}