// Proyecto: Arduino + ENC28J60 + 2x HC-SR04 → Modbus TCP
#include <EtherCard.h>
#include <Modbus.h>
#include <ModbusIP_ENC28J60.h>

// ----------------- Pines del sensor ultrasónico 1 -----------------
#define TRIG1 7
#define ECHO1 6

// ----------------- Pines del sensor ultrasónico 2 -----------------
#define TRIG2 5
#define ECHO2 4

// ----------------- Configuración Modbus -----------------
const int SENSOR1_IREG = 50;   // Dirección del registro para el sensor 1
const int SENSOR2_IREG = 60;   // Dirección del registro para el sensor 2
ModbusIP mb;                   // Objeto Modbus
unsigned long ts;              // Temporizador

// ----------------- Setup -----------------
void setup() {
  Serial.begin(9600);

  // --- Configuración de pines sensor 1 ---
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  // --- Configuración de pines sensor 2 ---
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // --- Configuración de red ---
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; 
  byte ip[]  = {192, 168, 0, 22};   // IP fija del Arduino con ENC28J60
  mb.config(mac, ip);

  // --- Registros Modbus donde se guardarán las distancias ---
  mb.addIreg(SENSOR1_IREG);   // Sensor 1 → HR50
  mb.addIreg(SENSOR2_IREG);   // Sensor 2 → HR60

  ts = millis();
}

// ----------------- Función para medir distancia -----------------
int medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms (~5m)
  int distancia = duration * 0.034 / 2;

  if (distancia == 0 || distancia > 400) {
    distancia = -1; // Error o fuera de rango
  }

  return distancia;
}

// ----------------- Loop principal -----------------
void loop() {
  mb.task(); // Mantiene la comunicación Modbus activa

  if (millis() > ts + 500) {   // Cada 500 ms mide ambos sensores
    ts = millis();

    // --- Medir sensor 1 ---
    int distancia1 = medirDistancia(TRIG1, ECHO1);
    mb.Ireg(SENSOR1_IREG, distancia1);

    // --- Medir sensor 2 ---
    int distancia2 = medirDistancia(TRIG2, ECHO2);
    mb.Ireg(SENSOR2_IREG, distancia2);

    // --- Debug por serial ---
    Serial.print("Sensor 1 (cm): ");
    Serial.println(distancia1);

    Serial.print("Sensor 2 (cm): ");
    Serial.println(distancia2);
  }
}
