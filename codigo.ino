// Augusto Valerio, RM: 562185
// Vitor Rodrigues Tigre, RM: 561746
// Jonas Esteves, RM: 564143
// Mariana Silva Oliveira, RM: 564241

#include <WiFi.h>        // Biblioteca para conectar o ESP32 ao Wi-Fi
#include <ThingSpeak.h>  // Biblioteca para enviar dados ao ThingSpeak
#include <DHT.h>         // Biblioteca para ler sensor DHT22

// ------------------ CONFIGURAÇÃO DE SENSORES -------------------
#define DHTPIN 13        // Pino digital onde o DHT22 está conectado
#define DHTTYPE DHT22    // Tipo do sensor DHT
#define LDRPIN 23        // Pino analógico onde o sensor de luminosidade está conectado

DHT dht(DHTPIN, DHTTYPE); // Cria o objeto do sensor DHT

// ------------------ CONFIGURAÇÃO WIFI E THINGSPEAK ------------------
const char* ssid = "Wokwi-GUEST";           // Nome da rede Wi-Fi
const char* password = "";                  // Senha da rede Wi-Fi (vazia no Wokwi)

unsigned long channelID = 123456;          // ID do canal ThingSpeak (substituir pelo seu)
const char* writeAPIKey = "F7LC408AE0PHMN23"; // Chave de escrita do canal ThingSpeak

WiFiClient client; // Objeto cliente usado para conexão com ThingSpeak

// ------------------ SETUP ------------------
void setup() {
  Serial.begin(115200); // Inicializa comunicação serial para monitoramento
  dht.begin();          // Inicializa o sensor DHT

  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { // Espera até conectar
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado!");
  Serial.print("IP: ");  // Mostra o IP que o ESP32 recebeu
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client); // Inicializa ThingSpeak com o cliente Wi-Fi
}

// ------------------ LOOP ------------------
void loop() {
  // Leitura dos sensores
  float temperatura = dht.readTemperature(); // Temperatura em °C
  float umidade = dht.readHumidity();        // Umidade relativa em %
  int luminosidade = analogRead(LDRPIN);    // Valor do sensor de luminosidade (0-4095 no ESP32)

  // Tratamento de erro do DHT
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Falha ao ler o DHT!"); // Caso a leitura falhe
    delay(2000);
    return; // Sai do loop e tenta novamente
  }

  // Exibe valores no Serial Monitor
  Serial.print("Temperatura: "); Serial.println(temperatura);
  Serial.print("Umidade: "); Serial.println(umidade);
  Serial.print("Luminosidade: "); Serial.println(luminosidade);

  // Configura os campos do ThingSpeak
  ThingSpeak.setField(1, temperatura);   // Campo 1: Temperatura
  ThingSpeak.setField(2, umidade);       // Campo 2: Umidade
  ThingSpeak.setField(3, luminosidade);  // Campo 3: Luminosidade

  // Envia os dados para o ThingSpeak
  int response = ThingSpeak.writeFields(channelID, writeAPIKey);
  if (response == 200) {
    Serial.println("Dados enviados com sucesso!");
  } else {
    Serial.println("Falha no envio. Código: " + String(response));
  }

  Serial.println("-------------------------");
  delay(15000); // Aguarda 15 segundos entre cada envio (limite do ThingSpeak)
}
