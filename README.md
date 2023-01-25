# Estación meteorológica USAL

Código para una estación meteorológica compuesta del controlador Arduino MKR1310
junto al shield Modbus RS485 y los siguientes sensores:

- Temperatura y humedad ambiente.
- Velocidad y dirección del viento.
- Radiación solar.
- Intensidad de lluvia.

Todos los sensores se comunican usando el protocolo Modbus RTU mediante una
interfaz RS485. Se conectan al shield usando una configuración maestro-esclavo.

Dependencias:

- Librería Arduino Low Power (para bajo consumo mediante "deep sleep").
- Librería Arduino Modbus (para la comunicación por el shield RS485).
  - Librería Arduino RS485 (utilizada por la librería Arduino Modbus).
- Librería MKRWAN_v2 (para la comunicación por LoRaWAN).
- Librería YACL (para la codificación del payload usando CBOR).

Renombrar fichero `secrets.def.h` a `secrets.h` e introducir los valores
apropiados de AppEUI y AppKey.
