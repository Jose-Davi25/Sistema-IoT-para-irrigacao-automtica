//Nome da biblioteca: DHT sensor library (Adafruit)
#include <DHT.h>

// === Definições de Pinos ===
#define SENSOR_LUMIN 39    // Sensor de luminosidade (VN)
#define SENSOR_SOLO  36    // Sensor de umidade do solo (VP)
#define DHTPIN      13    // Sensor DHT11 (GPIO13)
#define DHTTYPE      DHT11

#define LED_PIN      12     // LED indicador (GPIO4)
#define LED_PIN2      14     // LED indicador (GPIO4)
#define RELAY_1      22    // Relé 1 (GPIO22)
#define RELAY_2      23    // Relé 2 (GPIO23)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);

  dht.begin();
  delay(1000);
  Serial.println("Sistema Iniciado!");
}

void loop() {
  // === Leitura da luminosidade ===
  int valorLuz = analogRead(SENSOR_LUMIN);
  int lumPorcent = map(valorLuz, 4095, 0, 0, 100); // Mais escuro = maior %

  Serial.print("Luminosidade: ");
  Serial.print(lumPorcent);
  Serial.println(" %");

  // === Leitura da umidade do solo ===
  int valorSolo = analogRead(SENSOR_SOLO);
  int soloPorcent = map(valorSolo, 4095, 0, 0, 100); // Mais úmido = maior %

  Serial.print("Umidade do Solo: ");
  Serial.print(soloPorcent);
  Serial.println(" %");

  // === Leitura do DHT11 ===
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();

  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Erro ao ler DHT11!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C\tUmidade do Ar: ");
    Serial.print(umidade);
    Serial.println(" %");
  }

  // === Controle do LED com base na umidade do solo ===
  if (soloPorcent > 60) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(RELAY_1, HIGH); // Ativa relé 1
    Serial.println("Solo úmido → LED e RELÉ 1 ACESOS");
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_1, LOW); // Desliga relé 1
    Serial.println("Solo seco → LED e RELÉ 1 APAGADOS");
  }

  // === Controle do RELÉ 2 com base na luminosidade ===
  if (soloPorcent > 16 && temperatura > 24 || soloPorcent < 30 && temperatura < 30) {
    digitalWrite(RELAY_2, HIGH); // Ativa relé 2
    digitalWrite(LED_PIN2, HIGH);
    digitalWrite(LED_PIN, LOW);
    Serial.println("RELÉ ATIVO");
  } else {
    digitalWrite(RELAY_2, LOW); // Desliga relé 2
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN, HIGH);
    Serial.println("RELÉ DESATIVADO");
  }

  Serial.println("---------------------------");
  delay(3000);
}