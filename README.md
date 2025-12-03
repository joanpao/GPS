# GPS
Programas para Lilygo T-Beam V1.2 (Puede adquirilo a buen precio en https://s.click.aliexpress.com/e/_c42M25rj )
Este repositorio contiene varios programas adaptados a la placa ESP32 Lilygo T-Beam v1.2
En concreto se ha provado con una placa 433 MHz con pantalla OLED y NEO-6M(GPS)
Todos los programas usan TinyGPSPlus.h que debe incluirla en la libreria de Arduino IDE
- GPS.ino es un programa sencillo para probar que la placa funciona y cuya salida de datos GPS es via el puerto serie
- GPS_Display.ino es una versión mejorada del programa anterior en la que se incluye tambien la salida a traves de la pantalla OLED, lo que permite usar la placa con su bateria. Para este programa neesita que incluya en el proyecto los programas Fixed8x16.h, Adafruit_SSD1306.h y Adafruit_SSD1306.cpp y que incluya en la biblioteca de arduino IDE SPI.h y Wire.h
Se han definido tres tipos de salida: GPS estar+tico, GPS en movimiento y reloj que comentando y descomentando puede adaptarse a las necesidades de cada uno
- GPS_Beacon.ino es una versión mejorada del anterior en la que se han comentado las salidas series y por pantalla, para incluir la transmisión del los datos GPS que deseeemos mediante LoRa codificado en Morse, para ello aprovechamos el desarrollo de morse-beacon.ino Hay que incluir en la libbreia los programas LoRa.h y LoRa.ccp. 

Conocer el funcionamiento del chip GPS incluido es la placa Lilygo T-Beam nos abre la posibilidad de nuevos desarrollos como
- Servidor NTP basado en GPS
- Transmisor WSPR (requiere denGPS para el sincronismo, se requiere licencia de radioaficionado)
- APRS
