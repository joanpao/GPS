/*****************************************
* ESP32 GPS VKEL 9600 Bds
* https://github.com/LilyGO/TTGO-T-Beam/blob/master/GPS/GPS.ino 
* https://github.com/mikalhart/TinyGPSPlus
* https://arduiniana.org/libraries/tinygpsplus/
* Modificado por EA5JTT 20250716 para:
*  - Que funcione con placa LilyGo T-Display OLED GPS
*  - Mostrar la salida tambien por el  display OLED si se tiene instalado
*  - Completar las variables que proporciona el GPS de movimiento y de fecha
* ATENCION
*  - Activar Montior Serie si se quiere esa opción
*  - Puede salir por serie y/o por display segun como se quiera
*  - La Hora es UTC
******************************************/

// No hace falta en el proyecto pues está en la biblioteca de Arduino IDE
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include "Fixed8x16.h"
#include "LoRa.h"
#include <TinyGPS++.h>                       

TinyGPSPlus gps;                            

// Board pins.
// ESP32 433 LORA32 V2.1_1.6
#define LED 25
// LoRa pins for ESP32 433 GPS T-BEAM V1.2
#define LORA_SCK 5    
#define LORA_MISO 19  
#define LORA_MOSI 27  
#define LORA_SS 18    
#define LORA_RST 23  
#define LORA_DI0 26  
// OLED pins for for ESP32 433 GPS T-BEAM V1.2
#define OLED_RST NOT_A_PIN  // IO16 -- RESET
#define OLED_SDA 21         // IO21 -- SDA for TTGO Lora32 board
#define OLED_SCL 22         // IO22 -- SCL

Adafruit_SSD1306 display(OLED_RST);

// Da error y se comenta
//HardwareSerial Serial1(1);

// Estructura de  Serial1.begin(9600, SERIAL_8N1, 12, 15);                 
// Velocidad: 9600 baudios
// Configuración de datos: SERIAL_8N1 (8 bits, sin paridad, 1 bit de parada)
// GPIO12 como RX (recepción)
// GPIO15 como TX (transmisión)


void setup()
{
  // Set LED pin.
  pinMode(LED, OUTPUT);
  // salida serie
  Serial.begin(115200);
  while (!Serial)
    ;
  // Comunicacion con GPS
  Serial1.begin(9600, SERIAL_8N1, 34, 12);   
  // 15-TX 12-RX  Los pins originales no proporcionan información  "No GPS data received: check wiring"
  // 17-TX 18-RX  idem
  // 12-TX 34-RX  Para T-Beam V1.2
  // Escribe por el puerto serie a la consola
  Serial.println();
  delay(1000);
  Serial.println("GPS_Display by Juanpa Sanchez EA5JTT");
  // Initialize OLED display with the I2C address 0x3C.
  // OLED_SDA, OLED_SCL son las variables ya inicializadas con los pines usados apra OLED, el 21 y 22 en este caso
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, OLED_SDA, OLED_SCL);
  display.clearDisplay();
  // Set font.
  // Tamaño de las letras 8x16
  // color WHITE es el unico posible en un display B/N
  display.setFont(&Fixed8x16);
  display.setTextColor(WHITE);
  display.setCursor(0, Fixed8x16.yAdvance);
  // Visualize the result.
  display.display();
}

void loop()
{
  // SALIDA SERIE
  // Movimiento
  Serial.print(F("km/h="));
  Serial.println(gps.speed.kmph());
  // Serial.print(gps.speed.mps());
  Serial.print(F(" Deg="));
  Serial.println(gps.course.deg());
  // Posición
  Serial.print("Latitude  : ");
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitude : ");
  Serial.println(gps.location.lng(), 5);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Altitude  : ");
  Serial.print(gps.altitude.feet() / 3.2808);
  Serial.println("M");
  //Tiempo
  Serial.print(F("DATE: "));
  Serial.print(gps.date.year());
  Serial.print(F("/"));
  Serial.print(gps.date.month());
  Serial.print(F("/"));
  Serial.println(gps.date.day());
  Serial.print("Time (UTC): ");
  Serial.print(gps.time.hour());
  Serial.print(":");
  Serial.print(gps.time.minute());
  Serial.print(":");
  Serial.println(gps.time.second());
  Serial.println("**********************");

  // DISPLAY ESTATICO
  /* 
  display.clearDisplay();
  display.display();
  display.setCursor(0,12);          
  display.print(F("Lat: "));
  display.println(gps.location.lat(),5);
  display.setCursor(0,24); 
  display.print(F("Lon: "));
  display.println(gps.location.lng(), 5);
  display.setCursor(0,36); 
  display.print(F("Sat: "));
  display.println(gps.satellites.value());
  display.setCursor(0,48); 
  display.print("Alt: ");
  display.print(gps.altitude.feet() / 3.2808);
  display.println("m");
  display.setCursor(0,60);
  display.print("UTC: "); 
  display.print(gps.time.hour());
  display.print(":");
  display.print(gps.time.minute());
  display.print(":");
  display.println(gps.time.second());
  display.display();
 */
 // DISPLAY DINAMICO
  
  display.clearDisplay();
  display.display();
  display.setCursor(0,12);          
  display.print(F("Lat: "));
  display.println(gps.location.lat(),5);
  display.setCursor(0,24); 
  display.print(F("Lon: "));
  display.println(gps.location.lng(), 5);
  //display.setCursor(0,36); 
  //display.print(F("Sat: "));
  //display.println(gps.satellites.value());
  display.setCursor(0,36); 
  display.print("Alt: ");
  display.print(gps.altitude.feet() / 3.2808);
  display.println("m");
  display.setCursor(0,48);
  display.print(F("km/h: "));
  display.println(gps.speed.kmph());
  display.setCursor(0,60);
  display.print(F("Azm: "));
  display.println(gps.course.deg());
  

  // RELOJ
  /*
  display.clearDisplay();
  display.display();
  display.setCursor(22,25);  
  display.print(gps.date.day());
  display.print(F("-"));
  display.print(gps.date.month());
  display.print(F("-"));
  display.println(gps.date.year());
  display.setCursor(30,50);  
  display.print(gps.time.hour());
  display.print(":");
  display.print(gps.time.minute());
  display.print(":");
  display.print(gps.time.second());
  display.print(".");
  display.println(gps.time.centisecond());
  */
   display.display();
  // Aqui se modifia la velocidad de refresco en ms
  smartDelay(1000);                                      

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}
