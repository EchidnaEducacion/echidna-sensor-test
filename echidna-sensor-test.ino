/*
 * Test de Sensores - Placa Robótica Educativa
 * Prueba secuencial de todos los sensores en Modo Normal y Modo MkMk
 */

#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Definición de pines - Modo Normal
#define PIN_SR 2        // Pulsador derecho
#define PIN_SL 3        // Pulsador izquierdo
#define PIN_JOYX A0     // Joystick X
#define PIN_JOYY A1     // Joystick Y
#define PIN_LDR A3      // Sensor de luz
#define PIN_TEMP A6     // Sensor de temperatura
#define PIN_MIC A7      // Micrófono

// Objeto para el acelerómetro LIS3DH
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Variables de control
enum Estado {
  INICIO_MODO_NORMAL,
  TEST_MODO_NORMAL,
  INICIO_MODO_MKMK,
  TEST_MODO_MKMK,
  FIN_TEST
};

Estado estadoActual = INICIO_MODO_NORMAL;
int sensorActual = 0;
bool esperandoPulsacion = true;
bool mensajeMostrado = false;
unsigned long ultimoRefresh = 0;
const unsigned long INTERVALO_REFRESH = 500; // 0.5 segundos

// Nombres de sensores - Modo Normal
const char* sensoresNormal[] = {
  "SR (Pulsador Derecho - D2)",
  "SL (Pulsador Izquierdo - D3)",
  "Joystick X (A0)",
  "Joystick Y (A1)",
  "Acelerómetro LIS3DH (I2C A4/A5)",
  "LDR - Sensor de Luz (A3)",
  "Temperatura (A6)",
  "Micrófono (A7)"
};
const int NUM_SENSORES_NORMAL = 8;

// Nombres de sensores - Modo MkMk
const char* sensoresMkMk[] = {
  "A0",
  "A1",
  "A2",
  "A3",
  "A6",
  "A7",
  "D2",
  "D3"
};
const int NUM_SENSORES_MKMK = 8;

// Variables para debounce
unsigned long ultimaPulsacion = 0;
const unsigned long DEBOUNCE_DELAY = 250;
bool estadoAnteriorSR = HIGH;
bool pulsacionDetectada = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10); // Esperar a que se abra el puerto serial

  // Configurar pines digitales
  pinMode(PIN_SR, INPUT_PULLUP);
  pinMode(PIN_SL, INPUT_PULLUP);

  // Inicializar acelerómetro LIS3DH
  if (!lis.begin(0x18)) {  // Dirección I2C por defecto 0x18 (probar 0x19 si no funciona)
    Serial.println("Error: No se pudo inicializar el LIS3DH");
    Serial.println("Revise las conexiones I2C (A4/A5)");
  } else {
    lis.setRange(LIS3DH_RANGE_4_G);  // Rango de ±4g
  }

  Serial.println("========================================");
  Serial.println("  TEST DE SENSORES - PLACA ROBOTICA");
  Serial.println("========================================");
  Serial.println();
  delay(1000);
}

void loop() {
  switch (estadoActual) {
    case INICIO_MODO_NORMAL:
      iniciarModoNormal();
      break;

    case TEST_MODO_NORMAL:
      ejecutarTestModoNormal();
      break;

    case INICIO_MODO_MKMK:
      iniciarModoMkMk();
      break;

    case TEST_MODO_MKMK:
      ejecutarTestModoMkMk();
      break;

    case FIN_TEST:
      finalizarTest();
      break;
  }
}

void iniciarModoNormal() {
  if (!mensajeMostrado) {
    Serial.println("========================================");
    Serial.println("       TEST MODO NORMAL");
    Serial.println("========================================");
    Serial.println("Pulse SR (pulsador derecho) para comenzar...");
    Serial.println();
    mensajeMostrado = true;
  }

  if (esperarPulsacionSR()) {
    sensorActual = 0;
    esperandoPulsacion = true;
    mensajeMostrado = false;
    estadoActual = TEST_MODO_NORMAL;
  }
}

void ejecutarTestModoNormal() {
  if (sensorActual >= NUM_SENSORES_NORMAL) {
    // Terminado el modo normal
    estadoActual = INICIO_MODO_MKMK;
    sensorActual = 0;
    esperandoPulsacion = true;
    mensajeMostrado = false;
    return;
  }

  if (esperandoPulsacion) {
    if (!mensajeMostrado) {
      Serial.println("----------------------------------------");
      Serial.print("Sensor ");
      Serial.print(sensorActual + 1);
      Serial.print(" de ");
      Serial.print(NUM_SENSORES_NORMAL);
      Serial.print(": ");
      Serial.println(sensoresNormal[sensorActual]);
      Serial.println("Pulse SR para comenzar el test...");
      Serial.println();
      mensajeMostrado = true;
    }

    if (esperarPulsacionSR()) {
      esperandoPulsacion = false;
      mensajeMostrado = false;
      ultimoRefresh = 0;
      Serial.println("Iniciando lectura (pulse SR para siguiente sensor)...");
    }
  } else {
    // Mostrar valores del sensor
    if (millis() - ultimoRefresh >= INTERVALO_REFRESH) {
      leerSensorNormal(sensorActual);
      ultimoRefresh = millis();
    }

    // Comprobar si se pulsa SR para pasar al siguiente
    if (leerPulsadorSR()) {
      sensorActual++;
      esperandoPulsacion = true;
      mensajeMostrado = false;
      Serial.println();
    }
  }
}

void iniciarModoMkMk() {
  if (!mensajeMostrado) {
    Serial.println();
    Serial.println("========================================");
    Serial.println("       TEST MODO MKMK");
    Serial.println("========================================");
    Serial.println("ATENCION: En modo normal pulsa SR para comenzar.");
    Serial.println("Después cambie a modo MkMk y pruebe el sensor.");
    Serial.println("Cuando quiera terminar cambie a modo normal.");
    Serial.println("Pulse SR para comenzar el test y cambie a modo MkMk para probar el siguiente sensor.");
    Serial.println("Y así hasta que se terminen todos los test.");
    Serial.println();
    mensajeMostrado = true;
  }

  if (esperarPulsacionSR()) {
    sensorActual = 0;
    esperandoPulsacion = true;
    mensajeMostrado = false;
    estadoActual = TEST_MODO_MKMK;
  }
}

void ejecutarTestModoMkMk() {
  if (sensorActual >= NUM_SENSORES_MKMK) {
    // Terminado el modo MkMk
    estadoActual = FIN_TEST;
    mensajeMostrado = false;
    return;
  }

  if (esperandoPulsacion) {
    if (!mensajeMostrado) {
      Serial.println("----------------------------------------");
      Serial.print("Sensor ");
      Serial.print(sensorActual + 1);
      Serial.print(" de ");
      Serial.print(NUM_SENSORES_MKMK);
      Serial.print(": ");
      Serial.println(sensoresMkMk[sensorActual]);
      Serial.println("Pulse SR para comenzar el test...");
      Serial.println();
      mensajeMostrado = true;
    }

    if (esperarPulsacionSR()) {
      esperandoPulsacion = false;
      mensajeMostrado = false;
      ultimoRefresh = 0;
      Serial.println("Iniciando lectura (pulse SR para siguiente sensor)...");
    }
  } else {
    // Mostrar valores del sensor
    if (millis() - ultimoRefresh >= INTERVALO_REFRESH) {
      leerSensorMkMk(sensorActual);
      ultimoRefresh = millis();
    }

    // Comprobar si se pulsa SR para pasar al siguiente
    if (leerPulsadorSR()) {
      sensorActual++;
      esperandoPulsacion = true;
      mensajeMostrado = false;
      Serial.println();
    }
  }
}

void finalizarTest() {
  Serial.println();
  Serial.println("========================================");
  Serial.println("    TEST COMPLETADO EXITOSAMENTE");
  Serial.println("========================================");
  Serial.println("Todos los sensores han sido probados.");
  Serial.println("Para reiniciar, presione el boton RESET.");
  Serial.println();

  while (true) {
    delay(1000);
  }
}

bool esperarPulsacionSR() {
  return leerPulsadorSR();
}

bool leerPulsadorSR() {
  bool estadoActualSR = digitalRead(PIN_SR);

  // Detectar flanco de bajada (HIGH -> LOW) = pulsación
  if (estadoAnteriorSR == HIGH && estadoActualSR == LOW) {
    if (millis() - ultimaPulsacion > DEBOUNCE_DELAY) {
      ultimaPulsacion = millis();
      estadoAnteriorSR = estadoActualSR;
      return true;
    }
  }

  estadoAnteriorSR = estadoActualSR;
  return false;
}

void leerSensorNormal(int sensor) {
  switch (sensor) {
    case 0: // SR - Pulsador derecho
      Serial.print("SR: ");
      Serial.println(digitalRead(PIN_SR) == LOW ? "NO PULSADO" : "PULSADO");
      break;

    case 1: // SL - Pulsador izquierdo
      Serial.print("SL: ");
      Serial.println(digitalRead(PIN_SL) == LOW ? "NO PULSADO" : "PULSADO");
      break;

    case 2: // Joystick X
      Serial.print("Joystick X: ");
      Serial.println(analogRead(PIN_JOYX));
      break;

    case 3: // Joystick Y
      Serial.print("Joystick Y: ");
      Serial.println(analogRead(PIN_JOYY));
      break;

    case 4: // Acelerómetro LIS3DH
      {
        sensors_event_t event;
        lis.getEvent(&event);
        Serial.print("Acelerometro - X: ");
        Serial.print(event.acceleration.x, 2);
        Serial.print(" m/s^2, Y: ");
        Serial.print(event.acceleration.y, 2);
        Serial.print(" m/s^2, Z: ");
        Serial.print(event.acceleration.z, 2);
        Serial.println(" m/s^2");
      }
      break;

    case 5: // LDR
      Serial.print("LDR (Luz): ");
      Serial.println(analogRead(PIN_LDR));
      break;

    case 6: // Temperatura
      Serial.print("Temperatura: ");
      Serial.println(0.4658 * analogRead(PIN_TEMP) - 50.00);
      break;

    case 7: // Micrófono
      Serial.print("Microfono: ");
      Serial.println(analogRead(PIN_MIC));
      break;
  }
}

void leerSensorMkMk(int sensor) {
  switch (sensor) {
    case 0: // A0
      Serial.print("A0: ");
      Serial.println(analogRead(A0));
      break;

    case 1: // A1
      Serial.print("A1: ");
      Serial.println(analogRead(A1));
      break;

    case 2: // A2
      Serial.print("A2: ");
      Serial.println(analogRead(A2));
      break;

    case 3: // A3
      Serial.print("A3: ");
      Serial.println(analogRead(A3));
      break;

    case 4: // A6
      Serial.print("A6: ");
      Serial.println(analogRead(A6));
      break;

    case 5: // A7
      Serial.print("A7: ");
      Serial.println(analogRead(A7));
      break;

    case 6: // D2
      Serial.print("D2: ");
      Serial.println(digitalRead(2) == LOW ? "LOW (0)" : "HIGH (1)");
      break;

    case 7: // D3
      Serial.print("D3: ");
      Serial.println(digitalRead(3) == LOW ? "LOW (0)" : "HIGH (1)");
      break;
  }
}
