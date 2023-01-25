#include <ArduinoLowPower.h>
#include <ArduinoModbus.h>
#include <MKRWAN_v2.h>
#include <YACL.h>

// Descomentar para activar mensajes a la consola y usar retrasos normales
// en vez de sueño profundo.
//#define DEBUG

#define TEMHUM_ADDR 3 /* Dirección esclavo modbus sensor de temperatura y humedad ambiente */
#define VEL_ADDR 5    /* Dirección esclavo modbus sensor de velocidad del viento */
#define DIR_ADDR 4    /* Dirección esclavo modbus sensor de dirección del viento */
#define RAD_ADDR 1    /* Dirección esclavo modbus sensor de radiación solar */
#define PLU_ADDR 2    /* Dirección esclavo modbus pluviómetro */

#define SENSORS_BAUDRATE 9600
#define MODBUS_TIMEOUT 3000

// Esperar 2 minutos antes de reintentar conectarse a la red.
#define RETRY_SLEEP 2 * 60 * 1000

// Sueño profundo de 10 minutos entre iteraciones (después de enviar un paquete LoRa).
#define ITER_SLEEP 10 * 60 * 1000

#include "secrets.h"
#define APP_EUI SECRET_APP_EUI
#define APP_KEY SECRET_APP_KEY

LoRaModem modem;

void setup() {
#if defined(DEBUG)
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Comenzando puesta en marcha...");
#endif

  // Conectar el cliente Modbus RTU.
  while (1) {
    if (!ModbusRTUClient.begin(SENSORS_BAUDRATE)) {
#if defined(DEBUG)
      Serial.println("Fallo al iniciar el cliente Modbus RTU!");
      Serial.printf("Esperando %f segundos antes de reintentar...\n", RETRY_SLEEP / 1000);
      delay(RETRY_SLEEP);
#else
      LowPower.sleep(RETRY_SLEEP);
#endif
    } else {
#if defined(DEBUG)
      Serial.println("Cliente Modbus RTU conectado.");
      Serial.println("Conectando modem LoRa...");
#endif
      break;
    }
  }

  /* Modificar el timeout del cliente para ser más permisivo con respuestas lentas */
  ModbusRTUClient.setTimeout(MODBUS_TIMEOUT);

  // Arrancar modem LoRa.
  while (1) {
    if (!modem.begin(EU868)) {
#if defined(DEBUG)
      Serial.println("Fallo al iniciar el modem LoRa!");
      Serial.printf("Esperando %f segundos antes de reintentar...\n", RETRY_SLEEP / 1000);
      delay(RETRY_SLEEP);
#else
      LowPower.sleep(RETRY_SLEEP);
#endif
    } else {
#if defined(DEBUG)
      Serial.println("Modem LoRa conectado.");
      Serial.println("Uniéndose a la red LoRaWAN...");
#endif
      break;
    }
  }

  // Puertos válidos del 1 al 223.
  modem.setPort(1);

  // Unirse a la red LoRaWAN.
  while (1) {
    if (!modem.joinOTAA(APP_EUI, APP_KEY)) {
#if defined(DEBUG)
      Serial.println("No se pudo conectar, asegúrate de tener buena cobertura.");
      Serial.printf("Esperando %f segundos antes de reintentar...\n", RETRY_SLEEP / 1000);
      delay(RETRY_SLEEP);
#else
      LowPower.sleep(RETRY_SLEEP);
#endif
    } else {
#if defined(DEBUG)
      Serial.println("Se ha unido correctamente a la red LoRaWAN.");
      Serial.println("Puesta en marcha terminada.");
#endif
      break;
    }
  }
}

void loop() {
  float temperatura,
    humedad,
    velocidad,
    direccion,
    radiacion,
    lluvia;

  // Petición de lectura sensor temperatura y humedad (esclavo 3)
  while (1) {
    if (!ModbusRTUClient.requestFrom(TEMHUM_ADDR, HOLDING_REGISTERS, 0x00, 2)) {
#if defined(DEBUG)
      Serial.printf("Fallo al leer los registros: %s", ModbusRTUClient.lastError());
      Serial.printf("Esperando %f segundos antes de reintentar...\n", RETRY_SLEEP / 1000);
      delay(RETRY_SLEEP);
#else
      LowPower.sleep(RETRY_SLEEP);
#endif
    } else {
      temperatura = ModbusRTUClient.read();
      humedad = ModbusRTUClient.read();

#if defined(DEBUG)
      Serial.printf("temperatura: %f\nhumedad: %f\n", temperatura, humedad);
#endif
      break;
    }
  }

  // Petición de lectura sensor de velocidad de viento (esclavo 5)
  while (1) {
    if (!ModbusRTUClient.requestFrom(VEL_ADDR, HOLDING_REGISTERS, 0x00, 1)) {
#if defined(DEBUG)
      Serial.printf("Fallo al leer los registros: %s", ModbusRTUClient.lastError());
      Serial.printf("Esperando %f segundos antes de reintentar...\n", RETRY_SLEEP / 1000);
      delay(RETRY_SLEEP);
#else
      LowPower.sleep(RETRY_SLEEP);
#endif
    } else {
      velocidad = ModbusRTUClient.read();

#if defined(DEBUG)
      Serial.printf("velocidad: %f\n", velocidad);
#endif
      break;
    }
  }

  // Petición de lectura sensor de dirección de viento (esclavo 4)
  while (1) {
    if (!ModbusRTUClient.requestFrom(DIR_ADDR, HOLDING_REGISTERS, 0x00, 1)) {
#if defined(DEBUG)
      Serial.printf("Fallo al leer los registros: %s", ModbusRTUClient.lastError());
      Serial.printf("Esperando %f segundos antes de reintentar...\n", RETRY_SLEEP / 1000);
      delay(RETRY_SLEEP);
#else
      LowPower.sleep(RETRY_SLEEP);
#endif
    } else {
      direccion = ModbusRTUClient.read();

#if defined(DEBUG)
      Serial.printf("direccion: %f\n", direccion);
#endif
      break;
    }
  }

  // Petición de lectura sensor de radiación solar (esclavo 1)
  while (1) {
    if (!ModbusRTUClient.requestFrom(RAD_ADDR, HOLDING_REGISTERS, 0x00, 1)) {
#if defined(DEBUG)
      Serial.printf("Fallo al leer los registros: %s", ModbusRTUClient.lastError());
      Serial.printf("Esperando %f segundos antes de reintentar...\n", RETRY_SLEEP / 1000);
      delay(RETRY_SLEEP);
#else
      LowPower.sleep(RETRY_SLEEP);
#endif
    } else {
      radiacion = ModbusRTUClient.read();

#if defined(DEBUG)
      Serial.printf("radiacion: %f\n", radiacion);
#endif
      break;
    }
  }

  // Petición de lectura pluviómetro (esclavo 2)
  while (1) {
    if (!ModbusRTUClient.requestFrom(PLU_ADDR, HOLDING_REGISTERS, 0x00, 1)) {
#if defined(DEBUG)
      Serial.printf("Fallo al leer los registros: %s", ModbusRTUClient.lastError());
      Serial.printf("Esperando %f segundos antes de reintentar...\n", RETRY_SLEEP / 1000);
      delay(RETRY_SLEEP);
#else
      LowPower.sleep(RETRY_SLEEP);
#endif
    } else {
      lluvia = ModbusRTUClient.read();

#if defined(DEBUG)
      Serial.printf("lluvia: %f\n", lluvia);
#endif
      break;
    }
  }

#if defined(DEBUG)
  Serial.println("Final del periodo de lectura");
  Serial.println("Procesando datos recogidos...");
#endif

  // Construir un objeto CBOR con las lecturas.
  CBORArray cbor_obj = CBORArray();
  cbor_obj.append((unsigned short)round(temperatura * 100));
  cbor_obj.append((unsigned short)round(humedad * 100));
  cbor_obj.append((unsigned short)round(velocidad * 100));
  cbor_obj.append((unsigned short)round(direccion * 100));
  cbor_obj.append((unsigned short)round(radiacion * 100));
  cbor_obj.append((unsigned short)round(lluvia * 100));

  // Codificar datos en CBOR.
  const uint8_t *buffer = cbor_obj.to_CBOR();
  size_t buffer_len = cbor_obj.length();

  // Enviar datos codificadoes en un paquete LoRa.
  modem.beginPacket();
  modem.write(buffer, buffer_len);
#if defined(DEBUG)
  if (modem.endPacket(false) > 0) {
    Serial.println("Paquete LoRa enviado correctamente.");
  } else {
    Serial.println("Error enviando el paquete LoRa.");
  }
#else
  modem.endPacket(false);
#endif

#if defined(DEBUG)
  Serial.printf("Iteración terminada. Durmiendo durante %f segundos...\n", ITER_SLEEP / 1000);
  delay(ITER_SLEEP);
#else
  LowPower.deepSleep(ITER_SLEEP);
#endif
}
