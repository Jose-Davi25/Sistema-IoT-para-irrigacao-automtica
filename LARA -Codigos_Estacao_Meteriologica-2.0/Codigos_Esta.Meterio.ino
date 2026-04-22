//NTPClient
//DHT sensor library (Adafruit)
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Credenciais do Wi-Fi
char ssid[] = "ESTACIO-VISITANTES";
char pass[] = "estacio@2014";

// Pinos e configurações
#define DHTPIN 2       // Pino digital para o sensor DHT
#define DHTTYPE DHT11  // Tipo do sensor DHT
#define PINO_SENSOR_SOLO A0 // Pino analógico para o sensor de umidade do solo
#define LIMITE_UMIDADE_SOLO 30 // Limite de umidade para irrigação (30%)
//#define RELAY_PIN D1   // Pino digital para o relé

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure wifiClient;

unsigned long previousMillis = 0; 
const long intervalo = 10000; 

// Configurações NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); 

const char* serverName = "https://api.powerbi.com/beta/da49a844-e2e3-40af-86a6-c3819d704f49/datasets/a291c7eb-725f-4604-bf5e-0524c7989e3b/rows?experience=power-bi&key=XKyM14afH1b%2BpdQ1bPqQ7Ykr8b6CQpRA6F2lRcfpGQhm8LaByOb6y2N08BGuO3SNaaVwRQ6N%2Ba4hgX3DdO8akQ%3D%3D";

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  // Conectar ao Wi-Fi
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  wifiClient.setInsecure();
  timeClient.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervalo) {
    previousMillis = currentMillis;

    timeClient.update();
    String formattedTime = timeClient.getFormattedTime(); 

    // Leitura do DHT
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Falha na leitura do DHT!");
      return;
    }

    // Leitura do solo
    int valorSolo0 = analogRead(PINO_SENSOR_SOLO);
    Serial.print("Valor bruto do solo: "); // DEBUG
    Serial.println(valorSolo0); // DEBUG

    // Mapeamento CORRETO (invertido se necessário)
    int porcentagemUmidadeSolo = map(valorSolo0, 0, 1023, 100, 0); // Invertido

    Serial.print("Umidade do solo: ");
    Serial.print(porcentagemUmidadeSolo);
    Serial.println("%");

    // Controle do relé
    if (porcentagemUmidadeSolo <= LIMITE_UMIDADE_SOLO) {
      Serial.println("Irrigando a planta");
      //digitalWrite(RELAY_PIN, LOW); 
    } else {
      Serial.println("Planta irrigada");
      //digitalWrite(RELAY_PIN, HIGH); 
    }

    // Envio de dados
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      if (http.begin(wifiClient, serverName)) {
        http.addHeader("Content-Type", "application/json");
        
        String postData = "[{\"UmidadeArr\":\"" + String(h) + 
                         "\",\"Temperaturaa\":\"" + String(t) + 
                         "\",\"UmidadeSoloo\":\"" + String(porcentagemUmidadeSolo) + 
                         "\",\"Horaa\":\"" + formattedTime + "\"}]";

        int httpCode = http.POST(postData);
        http.end();
      }
    }
  }
}