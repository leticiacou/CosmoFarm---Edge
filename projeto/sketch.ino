/*
 * CosmoFarm — Sistema Operacional
 * Agricultura Espacial Autônoma
 *
 * SENSORES:
 *   DHT22 (pino 2)  → temperatura e umidade
 *   LDR   (pino A0) → intensidade luminosa
 *
 * ATUADORES:
 *   Relay 1 (pino 8) → bomba de irrigação
 *   Relay 2 (pino 9) → ventilador
 *   LED vermelho (4) → grow espectro vermelho
 *   LED azul     (5) → grow espectro azul
 *   LED verde    (6) → status OK
 *   LED alerta   (7) → alerta crítico
 *   LCD 16x2 I2C (SDA=20, SCL=21)
 *
 * MODELOS:
 *   Crescimento: P(t) = P0 * e^(k * t)
 *   Consumo:     C(t) = a * t + b
 */

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <math.h>

#define PIN_DHT         2
#define PIN_LDR         A0
#define PIN_LED_RED     4
#define PIN_LED_BLUE    5
#define PIN_LED_STATUS  6
#define PIN_LED_ALERTA  7
#define PIN_RELAY_BOMBA 8
#define PIN_RELAY_VENT  9

#define TEMP_MIN    18.0
#define TEMP_MAX    26.0
#define UMID_MIN    55.0
#define UMID_MAX    80.0
#define LUZ_MIN     300
#define LUZ_MAX     800
#define CICLO_DIAS  30
#define P0          1.0
#define K_CRESC     0.115
#define CONSUMO_A   0.15
#define CONSUMO_B   0.5

DHT dht(PIN_DHT, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float temperatura, umidade;
int   luz;
bool  bombaAtiva, ventAtivo, growAtivo;
bool  alertaAtivo;
int   diaSimulado = 0;
float biomassa    = P0;
float consumoAcum = 0;
int   telaLCD     = 0;

unsigned long tLeitura=0, tDisplay=0, tDia=0, tSerial=0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_RED,     OUTPUT);
  pinMode(PIN_LED_BLUE,    OUTPUT);
  pinMode(PIN_LED_STATUS,  OUTPUT);
  pinMode(PIN_LED_ALERTA,  OUTPUT);
  pinMode(PIN_RELAY_BOMBA, OUTPUT);
  pinMode(PIN_RELAY_VENT,  OUTPUT);
  digitalWrite(PIN_RELAY_BOMBA, HIGH);
  digitalWrite(PIN_RELAY_VENT,  HIGH);
  dht.begin();
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0); lcd.print("  CosmoFarm v1  ");
  lcd.setCursor(0,1); lcd.print(" Iniciando...   ");
  delay(2000); lcd.clear();
  Serial.println(F("=== CosmoFarm Iniciado ==="));
}

void loop() {
  unsigned long agora = millis();

  if (agora - tLeitura >= 2000) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t)) temperatura = t;
    if (!isnan(h)) umidade     = h;
    luz = analogRead(PIN_LDR);
    controleAutonomo();
    tLeitura = agora;
  }

  if (agora - tDia >= 10000) {
    diaSimulado++;
    biomassa   = P0 * exp(K_CRESC * diaSimulado);
    consumoAcum += CONSUMO_A * diaSimulado + CONSUMO_B;
    Serial.print(F("Dia ")); Serial.print(diaSimulado);
    Serial.print(F(" | Biomassa: ")); Serial.print(biomassa,1); Serial.print(F("g"));
    Serial.print(F(" | Agua: ")); Serial.print(consumoAcum,1); Serial.println(F("L"));
    if (diaSimulado >= CICLO_DIAS) {
      Serial.println(F("*** COLHEITA DISPONIVEL! Reiniciando ciclo. ***"));
      diaSimulado = 0; biomassa = P0; consumoAcum = 0;
    }
    tDia = agora;
  }

  if (agora - tDisplay >= 3000) {
    atualizarLCD();
    telaLCD = (telaLCD + 1) % 3;
    tDisplay = agora;
  }

  if (agora - tSerial >= 4000) {
    Serial.print(F("T:")); Serial.print(temperatura,1);
    Serial.print(F(" H:")); Serial.print(umidade,1);
    Serial.print(F(" Luz:")); Serial.print(luz);
    Serial.print(F(" Bomba:")); Serial.print(bombaAtiva ? "ON":"OFF");
    Serial.print(F(" Vent:")); Serial.println(ventAtivo ? "ON":"OFF");
    tSerial = agora;
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c=='b') { digitalWrite(PIN_RELAY_BOMBA, LOW);  bombaAtiva=true; }
    if (c=='B') { digitalWrite(PIN_RELAY_BOMBA, HIGH); bombaAtiva=false; }
    if (c=='v') { digitalWrite(PIN_RELAY_VENT,  LOW);  ventAtivo=true; }
    if (c=='V') { digitalWrite(PIN_RELAY_VENT,  HIGH); ventAtivo=false; }
    if (c=='g') { digitalWrite(PIN_LED_RED,HIGH); digitalWrite(PIN_LED_BLUE,HIGH); growAtivo=true; }
    if (c=='G') { digitalWrite(PIN_LED_RED,LOW);  digitalWrite(PIN_LED_BLUE,LOW);  growAtivo=false; }
    if (c=='r') { diaSimulado=0; biomassa=P0; consumoAcum=0; Serial.println(F("Ciclo reiniciado")); }
  }
}

void controleAutonomo() {
  bool alerta = false;

  if (temperatura > TEMP_MAX)      { digitalWrite(PIN_RELAY_VENT,LOW);  ventAtivo=true;  alerta=true; }
  else if (temperatura < TEMP_MIN) { digitalWrite(PIN_RELAY_VENT,HIGH); ventAtivo=false; alerta=true; }
  else                              { digitalWrite(PIN_RELAY_VENT,HIGH); ventAtivo=false; }

  if (umidade < UMID_MIN)      { digitalWrite(PIN_RELAY_BOMBA,LOW);  bombaAtiva=true;  alerta=true; }
  else if (umidade > UMID_MAX) { digitalWrite(PIN_RELAY_BOMBA,HIGH); bombaAtiva=false; alerta=true; }
  else                         { digitalWrite(PIN_RELAY_BOMBA,HIGH); bombaAtiva=false; }

  if (luz < LUZ_MIN)      { digitalWrite(PIN_LED_RED,HIGH); digitalWrite(PIN_LED_BLUE,HIGH); growAtivo=true;  alerta=true; }
  else if (luz > LUZ_MAX) { digitalWrite(PIN_LED_RED,LOW);  digitalWrite(PIN_LED_BLUE,LOW);  growAtivo=false; }
  else                    { digitalWrite(PIN_LED_RED,HIGH); digitalWrite(PIN_LED_BLUE,LOW); }

  alertaAtivo = alerta;
  digitalWrite(PIN_LED_STATUS, alerta ? LOW  : HIGH);
  digitalWrite(PIN_LED_ALERTA, alerta ? HIGH : LOW);
}

void atualizarLCD() {
  lcd.clear();
  if (telaLCD == 0) {
    lcd.setCursor(0,0); lcd.print("T:"); lcd.print(temperatura,1); lcd.print((char)223); lcd.print("C");
    lcd.setCursor(9,0); lcd.print("H:"); lcd.print((int)umidade); lcd.print("%");
    lcd.setCursor(0,1); lcd.print(alertaAtivo ? "!! ALERTA !!    " : "Status: OK      ");
  } else if (telaLCD == 1) {
    lcd.setCursor(0,0); lcd.print("Dia:"); lcd.print(diaSimulado); lcd.print("/"); lcd.print(CICLO_DIAS);
    lcd.setCursor(0,1); lcd.print("Bio:"); lcd.print(biomassa,1); lcd.print("g");
  } else {
    lcd.setCursor(0,0); lcd.print("Agua:"); lcd.print(consumoAcum,1); lcd.print("L");
    lcd.setCursor(0,1); lcd.print("Luz:"); lcd.print(luz); lcd.print(luz<LUZ_MIN?" BAIXA":" OK");
  }
}