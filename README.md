## Participantes
- [Augusto Valerio](https://github.com/Augusto-Valerio).
- [Jonas Esteves](https://github.com/Jonas-Franca).
- [Mariana Silva](https://github.com/Marirsil)
- [Vitor Tigre](https://github.com/VitorTigre)

# **Projeto de Monitoramento Ambiental para Vinícola com ESP32 e ThingSpeak**

Este projeto visa monitorar as condições ambientais de uma vinícola utilizando um ESP32. Através de sensores como o DHT22 (para temperatura e umidade) e um sensor LDR (para luminosidade), o sistema coleta e envia dados de maneira contínua para a plataforma ThingSpeak. Os dados são cruciais para garantir as melhores condições para o cultivo das uvas e o armazenamento adequado do vinho.

## **Componentes Necessários**

- **ESP32**
- **Sensor DHT22** (para medir temperatura e umidade, importantes para o controle climático dentro da vinícola)
- **Sensor LDR** (para medir a intensidade da luz, que pode afetar a maturação das uvas)
- **Jumpers e Protoboard**
- **Fonte de alimentação para o ESP32**

## **Diagrama de Conexões**

- **DHT22**:
  - Pino VCC → 3.3V do ESP32
  - Pino GND → GND do ESP32
  - Pino de dados → GPIO 13 do ESP32 (DHTPIN)
  
- **LDR**:
  - Um terminal no pino GND
  - O outro terminal no pino analógico GPIO 23 do ESP32 (LDRPIN)

## **Bibliotecas Utilizadas**

- **WiFi.h**: Biblioteca para conectar o ESP32 à rede Wi-Fi.
- **ThingSpeak.h**: Biblioteca para enviar dados para a plataforma ThingSpeak.
- **DHT.h**: Biblioteca para ler dados do sensor DHT22.

## **Configuração do ThingSpeak**

1. **Criar Conta no ThingSpeak**:
   - Acesse [ThingSpeak](https://thingspeak.com/) e crie uma conta (se ainda não tiver).
   
2. **Criar um Canal**:
   - No ThingSpeak, crie um novo canal.
   - Defina os campos como **Temperatura**, **Umidade** e **Luminosidade** para monitorar as condições ambientais da vinícola.

3. **Obtenha a Chave de API de Escrita**:
   - Após criar o canal, você obterá uma chave de API de escrita que será utilizada no código para enviar os dados.

4. **Substitua o `channelID` e `writeAPIKey` no Código**:
   - Substitua `channelID` e `writeAPIKey` no código pelo ID do canal e a chave de API de escrita obtidos no seu canal do ThingSpeak.

## **Como Funciona o Código**

1. **Configuração do Wi-Fi**:
   - O ESP32 se conecta a uma rede Wi-Fi para enviar dados periodicamente para a plataforma ThingSpeak.

2. **Leitura dos Sensores**:
   - O sensor DHT22 lê a temperatura e umidade, parâmetros essenciais para controlar as condições de cultivo das uvas, que são sensíveis tanto à temperatura quanto à umidade.
   - O sensor LDR mede a luminosidade, ajudando a monitorar a quantidade de luz solar, fator importante para o processo de fotossíntese das uvas.

3. **Envio de Dados ao ThingSpeak**:
   - Após ler os dados dos sensores, as informações de temperatura, umidade e luminosidade são enviadas para o ThingSpeak, onde podem ser analisadas e monitoradas remotamente.

4. **Tratamento de Erros**:
   - Se o sensor DHT22 falhar na leitura, o sistema tentará realizar a leitura novamente após alguns segundos.

## **Exemplo de Código**

```cpp
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
