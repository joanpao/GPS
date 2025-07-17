/*****************************************
* ESP32 GPS VKEL 9600 Bds
* https://github.com/LilyGO/TTGO-T-Beam/blob/master/GPS/GPS.ino 
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

// LoRa frecuency
//
// EU433 (433.05 - 434.79 MHz) (Free transmision)
// ETSI has defined 433 to 434 MHz frequency band for LoRa application.
// It uses 433.175 MHz, 433.375 MHz and 433.575 MHz frequency channels.
//
// EU863 (863 - 870 MHz) (Free transmision)
//
// HAM band (only with license)
// 2m 144 - 146 MHz Beacon in 144.400 - 144.491
// 70 cm 430 - 440 MHz BEacon in 432.400 - 432.490
//
// PMR band (Free transmision)
// PMR 446 432.400 - 432.490
//
#define FREQ_HZ     144400000   // 144.400 MHz
//#define FREQ_HZ 432400000   // 432.400 MHz
//#define FREQ_HZ 866500000  // 866.500 MHz
//
// Max TX Power in Lyligo ESP32 LoRa is 20 dBm = 100 mW
// Tested:  11dBm 12,5 mW, 14 25 mW, 17dBm 50 mW, 20 dBm 100 mW
#define TX_DBM 20
//
// Morse speed, words per minute.
//
#define WPM 12

// OLED
Adafruit_SSD1306 display(OLED_RST);

//// long time definition: dot, dash ans pause 

int dot_msec = 1200 / WPM;
int dash_msec = 3600 / WPM;
int pause_msec = 3600 / WPM;
int space_seen;

//GPS
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
  // Escribe por el puerto serie a la consola
  Serial.println();
  delay(1000);
  Serial.println("GPS_Beacon by Juanpa Sanchez EA5JTT");

  // Comunicacion serie con GPS
  Serial1.begin(9600, SERIAL_8N1, 34, 12);   

  // Initialize LoRa chip.
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  if (!LoRa.begin(FREQ_HZ)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  LoRa.setSignalBandwidth(41700);
  LoRa.setCodingRate4(5);
  LoRa.setSpreadingFactor(6);
  LoRa.disableCrc();
  LoRa.setTxPower(TX_DBM);

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

// Rutinas LORA-MORSE
void beep(int msec) {
  // Compute preamble length of required duration.
  // To measure the timing, use plen = msec.
  int plen = (1000L * msec - 19089) / 1536;

  digitalWrite(LED, HIGH);  // LED on

  LoRa.setPreambleLength(plen);  // preamble length
  LoRa.beginPacket(true);        // no header
  LoRa.write(0);                 // data 1 byte
  LoRa.endPacket();

  digitalWrite(LED, LOW);  // LED off
}

void send_morse(char *sym, char *str) {
  display.print(sym);
  display.display();
  Serial.print(sym);

  if (*str++ == '.')
    beep(dot_msec);
  else
    beep(dash_msec);

  while (*str) {
    delay(dot_msec);

    if (*str++ == '.')
      beep(dot_msec);
    else
      beep(dash_msec);
  }
}

void send_char(int c)
{
    if (isspace(c) || ! isprint(c))
        c = ' ';
    else if (isalpha(c) && islower(c))
        c = toupper(c);

    switch (c) {
    default:
        return;
    case ' ' :
        if (! space_seen) {
            display.write(' ');
            display.display();
            Serial.write(c);
            delay(dot_msec + pause_msec);
        }
        space_seen = 1;
        return;
// MORSE CODE
// https://www.itu.int/dms_pubrec/itu-r/rec/m/r-rec-m.1677-1-200910-i!!pdf-e.pdf
    case 'A': send_morse("A", ".-"); break;
    case 'B': send_morse("B", "-..."); break;
    case 'C': send_morse("C", "-.-."); break;
    case 'Ç': send_morse("Ç", "-.-.."); break;
    case 'D': send_morse("D", "-.."); break;
    case 'E': send_morse("E", "."); break;
    case 'F': send_morse("F", "..-."); break;
    case 'G': send_morse("G", "--."); break;
    case 'H': send_morse("H", "...."); break;
    case 'I': send_morse("I", ".."); break;
    case 'J': send_morse("J", ".---"); break;
    case 'K': send_morse("K", "-.-"); break;
    case 'L': send_morse("L", ".-.."); break;
    case 'M': send_morse("M", "--"); break;
    case 'N': send_morse("N", "-."); break;
    case 'Ñ': send_morse("Ñ", "--.--"); break;
    case 'O': send_morse("O", "---"); break;
    case 'P': send_morse("P", ".--."); break;
    case 'Q': send_morse("Q", "--.-"); break;
    case 'R': send_morse("R", ".-."); break;
    case 'S': send_morse("S", "..."); break;
    case 'T': send_morse("T", "-"); break;
    case 'U': send_morse("U", "..-"); break;
    case 'V': send_morse("V", "...-"); break;
    case 'W': send_morse("W", ".--"); break;
    case 'X': send_morse("X", "-..-"); break;
    case 'Y': send_morse("Y", "-.--"); break;
    case 'Z': send_morse("Z", "--.."); break;
    case '0': send_morse("0", "-----"); break;
    case '1': send_morse("1", ".----"); break;
    case '2': send_morse("2", "..---"); break;
    case '3': send_morse("3", "...--"); break;
    case '4': send_morse("4", "....-"); break;
    case '5': send_morse("5", "....."); break;
    case '6': send_morse("6", "-...."); break;
    case '7': send_morse("7", "--..."); break;
    case '8': send_morse("8", "---.."); break;
    case '9': send_morse("9", "----."); break;
    case '.': send_morse(".", ".-.-.-"); break;
    case ',': send_morse(",", "--..--"); break;
    case ';': send_morse(";", "-.-.-."); break;
    case '?': send_morse("?", "..--.."); break;
    case '/': send_morse("/", "-..-."); break;
    case '=': send_morse("=", "-...-"); break;
    case ':': send_morse(":", "---..."); break;
    case '@': send_morse("@", ".--.-."); break;
    case '-': send_morse("-", "-....-"); break;
    case '(': send_morse("(", "-.--."); break;
    case ')': send_morse(")", "-.--.-"); break;
    case '+': send_morse("+", ".-.-."); break;
    case 'x': send_morse("x", "-..-"); break;
  }
  space_seen = 0;
}

void send_message(const char *str) {
  while (*str) {
    send_char(*str++);
    delay(pause_msec);
  }
  delay(dot_msec + pause_msec);
  display.clearDisplay();
  display.setCursor(0, Fixed8x16.yAdvance);
}
// No limit, tested 5 characters x 58 grups = 348 characters


void loop()
{
  // SALIDA SERIE
  // Movimiento
  // Serial.print(F("km/h="));
  // Serial.println(gps.speed.kmph());
  // Serial.print(gps.speed.mps());
  // Serial.print(F(" Deg="));
  // Serial.println(gps.course.deg());
  // Posición
  // Serial.print("Latitude  : ");
  // Serial.println(gps.location.lat(), 5);
  // Serial.print("Longitude : ");
  // Serial.println(gps.location.lng(), 5);
  // Serial.print("Satellites: ");
  // Serial.println(gps.satellites.value());
  // Serial.print("Altitude  : ");
  // Serial.print(gps.altitude.feet() / 3.2808);
  // Serial.println("M");
  //Tiempo
  // Serial.print(F("DATE: "));
  // Serial.print(gps.date.year());
  // Serial.print(F("/"));
  // Serial.print(gps.date.month());
  // Serial.print(F("/"));
  // Serial.println(gps.date.day());
  // Serial.print("Time (UTC): ");
  // Serial.print(gps.time.hour());
  // Serial.print(":");
  // Serial.print(gps.time.minute());
  // Serial.print(":");
  // Serial.println(gps.time.second());
  // Serial.println("**********************");

  // DISPLAY
  // display.clearDisplay();
  // display.display();
  // display.setCursor(0,12);          
  // display.print(F("Lat: "));
  // display.println(gps.location.lat(),5);
  // display.setCursor(0,24); 
  // display.print(F("Lon: "));
  // display.println(gps.location.lng(), 5);
  // display.setCursor(0,36); 
  // display.print(F("Sat: "));
  // display.println(gps.satellites.value());
  // display.setCursor(0,48); 
  // display.print("Alt: ");
  // display.print(gps.altitude.feet() / 3.2808);
  // display.println("m");
  // display.setCursor(0,60);
  // display.print("UTC: "); 
  // display.print(gps.time.hour());
  // display.print(":");
  // display.print(gps.time.minute());
  // display.print(":");
  // display.println(gps.time.second());
  // display.display();
  display.clearDisplay();
  display.setCursor(0, Fixed8x16.yAdvance);
  // Puede incluirse cualquier valor obtenido del GPS por eso se han comentado las lineas de las salidas serie y display
  String mensaje = 
  String("EA5JTT ") +
  gps.time.hour() + " : " +
  gps.time.minute() + " : " +
  gps.time.second();

  send_message(mensaje.c_str());
 
  // Aqui se modifia la velocidad de refresco en ms
  smartDelay(5000);                                      

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

