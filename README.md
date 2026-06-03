[![Typing SVG](https://readme-typing-svg.demolab.com?font=Fira+Code&weight=600&size=45&pause=1000&color=110169&background=2BC2FF&center=true&vCenter=true&width=1100&height=100&lines=Orbital+Fishing;Boia+com+Sensores;Monitoramento+Oceanico;C%2B%2B)](https://git.io/typing-svg)

![Status](https://img.shields.io/badge/status-Finalizado-green)
![Tipo](https://img.shields.io/badge/tipo-Projeto%20Acad%C3%AAmico-blue)
![Linguagem](https://img.shields.io/badge/linguagem-Arduino%20C%2B%2B-teal)
![Simulador](https://img.shields.io/badge/simulador-Wokwi-orange)
![Global Solution](https://img.shields.io/badge/FIAP-Global%20Solution%202026-red)

<img width="861" height="594" alt="image" src="https://github.com/user-attachments/assets/977342aa-39a3-4a54-930b-3e464ba9c44e" />

---

## Sobre o Projeto

O **Orbital Fishing** representa a camada de hardware do sistema de monitoramento oceânico desenvolvido pelo grupo. A boia é equipada com sensores que medem as principais variáveis ambientais determinantes para a localização de cardumes. Os dados são exibidos em um LCD e transmitidos em formato JSON, simulando o pacote que seria enviado via satélite LEO (órbita baixa) para a plataforma cloud do Orbital Fishing, onde algoritmos de IA fazem a predição dos pontos de pesca mais favoráveis para pescadores artesanais ao redor do mundo.

---

## Descrição do Projeto

O circuito simula uma **boia sensora oceânica autossustentável**, componente central da solução Orbital Fishing. A boia coleta dados das condições da água a cada 4 segundos, exibe cada leitura no display LCD e envia um pacote JSON pelo Serial contendo todos os valores medidos. Esse pacote representa os dados que seriam transmitidos via protocolo LoRa para satélites LEO da constelação Iridium/Starlink, chegando à plataforma cloud para processamento e geração de mapas de cardumes para os pescadores.

---

## Objetivo da Solução

Democratizar o acesso a dados oceanográficos de precisão para pescadores artesanais ao redor do mundo, utilizando tecnologia de sensoriamento embarcado combinada com transmissão via satélite. O sistema reduz a desigualdade de acesso tecnológico entre a pesca artesanal e a pesca industrial, conectando o hardware da boia à plataforma Orbital Fishing que gera orientações de pesca em tempo real.

---

## Componentes Utilizados

| Componente | Quantidade | Função |
| :--- | :---: | :--- |
| Arduino UNO | 1 | Microcontrolador principal |
| DS18B20 | 1 | Sensor de temperatura da água |
| Potenciômetro | 3 | Simula sensores de pH, salinidade e clorofila-a |
| LCD I2C 16x2 | 1 | Exibe as leituras em tempo real |
| Resistor 4.7kΩ | 1 | Pull-up obrigatório do DS18B20 |

> **Por que DS18B20 e não DHT11?**  
> O sensor ensinado em aula foi o **DHT11**, porém ele foi projetado para medir temperatura e umidade do **ar** — não é adequado para uso submerso em água. O **DS18B20** é um sensor à prova d'água, especialmente desenvolvido para medir temperatura de **líquidos**, sendo o componente correto para representar uma boia oceânica. Por esse motivo, optamos pelo DS18B20 neste projeto, mesmo que ele não tenha sido abordado em aula.

---

## Código Principal

```cpp
// Projeto: Orbital Fishing ( Boia com sensores )
// Integrantes:
// Diego Candido Stoianof / RM: 570748
// Felipe Moreira Mendes  / RM: 570807
// Lucas Zezzi Custodio   / RM: 571161
// Romulo Mendes Sousa    / RM: 570620

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pinos dos sensores
const int pinoPH         = A0;
const int pinoSalinidade = A1;
const int pinoClorofila  = A2;
const int pinoDS18B20    = 2;

// Sensor de temperatura DS18B20
OneWire oneWire(pinoDS18B20);
DallasTemperature sensorTemp(&oneWire);

// Controle de telas e temporizador
int telaAtual             = 0;
unsigned long ultimaTroca = 0;
const long intervalo      = 4000;

void setup() {
  Serial.begin(9600);
  sensorTemp.begin();
  lcd.init();
  lcd.backlight();

  // Tela de boas-vindas
  lcd.setCursor(0, 0);
  lcd.print("OrbitalFishing");
  lcd.setCursor(0, 1);
  lcd.print("OceanBoia");
  delay(3000);
  lcd.clear();
}

void loop() {
  // Verifica se ja passou o intervalo para trocar de tela
  if (millis() - ultimaTroca >= intervalo) {
    ultimaTroca = millis();
    telaAtual++;
    if (telaAtual > 3) telaAtual = 0;

    // Leitura dos sensores
    sensorTemp.requestTemperatures();
    float temperatura = sensorTemp.getTempCByIndex(0);
    float ph          = map(analogRead(pinoPH),         0, 1023, 0, 140) / 10.0;
    float salinidade  = map(analogRead(pinoSalinidade), 0, 1023, 0, 400) / 10.0;
    float clorofila   = map(analogRead(pinoClorofila),  0, 1023, 0, 100) / 10.0;

    // Monta e envia JSON pelo Serial
    // Simula o pacote de dados transmitido ao satelite
    JsonDocument doc;
    doc["temperatura_C"]  = temperatura;
    doc["ph"]             = ph;
    doc["salinidade_PSU"] = salinidade;
    doc["clorofila_mgm3"] = clorofila;
    serializeJsonPretty(doc, Serial);
    Serial.println();

    // Exibe no LCD o dado da tela atual
    lcd.clear();

    if (telaAtual == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Temperatura:");
      lcd.setCursor(0, 1);
      lcd.print(temperatura, 1);
      lcd.print(" C");
    }

    if (telaAtual == 1) {
      lcd.setCursor(0, 0);
      lcd.print("pH Oceanico:");
      lcd.setCursor(0, 1);
      lcd.print(ph, 1);
      lcd.print(" pH");
    }

    if (telaAtual == 2) {
      lcd.setCursor(0, 0);
      lcd.print("Salinidade:");
      lcd.setCursor(0, 1);
      lcd.print(salinidade, 1);
      lcd.print(" PSU");
    }

    if (telaAtual == 3) {
      lcd.setCursor(0, 0);
      lcd.print("Clorofila-a:");
      lcd.setCursor(0, 1);
      lcd.print(clorofila, 1);
      lcd.print(" mg/m3");
    }
  }
}
```

---

## Explicação do Funcionamento

1. Ao iniciar, o LCD exibe a tela de boas-vindas **"ORBITAL FISHING / OceanBoia"** por 3 segundos
2. A cada **4 segundos** o sistema faz a leitura de todos os sensores simultaneamente
3. O LCD alterna entre 4 telas — **Temperatura**, **pH**, **Salinidade** e **Clorofila-a** — exibindo o valor atual de cada parâmetro
4. A cada leitura, um **pacote JSON** é montado com os 4 valores e enviado pelo Serial, simulando a transmissão ao satélite
5. O ciclo se repete continuamente, gerando dados em tempo real

**Exemplo de pacote JSON gerado:**
```json
{
  "temperatura_C": 22.0,
  "ph": 8.1,
  "salinidade_PSU": 35.4,
  "clorofila_mgm3": 1.8
}
```

---

## Estrutura do Circuito

```
DS18B20  →  Pino Digital 2  (com resistor 4.7kΩ entre VCC e pino de dados)
Potenciômetro pH         →  Pino A0
Potenciômetro Salinidade →  Pino A1
Potenciômetro Clorofila  →  Pino A2
LCD I2C  →  SDA (A4) + SCL (A5)
```

---

## Estrutura do Repositório

```
Orbital-Fishing-Arduino/
├── OrbitalFishing.ino    # Código fonte principal do projeto
└── README.md         # Documentação do projeto
```

---

## Bibliotecas Necessárias

| Biblioteca | Função |
| :--- | :--- |
| `Wire` | Comunicação I2C (nativa) |
| `LiquidCrystal I2C` | Controle do display LCD |
| `OneWire` | Protocolo de comunicação do DS18B20 |
| `DallasTemperature` | Leitura de temperatura do DS18B20 |
| `ArduinoJson` | Serialização dos dados em formato JSON |

---

## Instruções de Execução

**Simulação no Wokwi (recomendado):**

1. Acesse o projeto diretamente pelo link abaixo
2. Clique no botão **▶ Play** para iniciar a simulação
3. Gire os potenciômetros para simular variações nos sensores de pH, salinidade e clorofila
4. Acompanhe as leituras no **LCD** e os pacotes JSON no **Serial Monitor**

🔗 **Link do projeto no Wokwi:** https://wokwi.com/projects/465510012091382785

---

## Equipe do Projeto

| Nome | RM | 
| :--- | :--- |
| Diego Candido Stoianof | 570748 |
| Felipe Moreira Mendes | 570807 |
| Lucas Zezzi Custódio | 571161 |
| Romulo Mendes Sousa | 570620 |
