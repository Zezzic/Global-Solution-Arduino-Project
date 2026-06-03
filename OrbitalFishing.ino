// Projeto: Orbital Fishing - Boia com sensores
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
  lcd.print("  ORBITAL FISHING    ");
  lcd.setCursor(0, 1);
  lcd.print("Boia de monit...");
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
