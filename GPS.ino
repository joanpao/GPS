/*****************************************
* ESP32 GPS VKEL 9600 Bds
* https://github.com/LilyGO/TTGO-T-Beam/blob/master/GPS/GPS.ino 
* Modificada por EA5JTT 20250716
* Compilar para placa LilyGo T-Display
* Activar Montior Serie
* La Hora es UTC
******************************************/
/*
 * Copyright (C) 2025 Juan Pablo Sanchez EA5JTT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
// No hace falta en el proyecto pues está en la biblioteca de Arduino IDE
#include <TinyGPS++.h>                       

TinyGPSPlus gps;                            

// Da error y se comenta
//HardwareSerial Serial1(1);

// Estructura de  Serial1.begin(9600, SERIAL_8N1, 12, 15);                 
// Velocidad: 9600 baudios
// Configuración de datos: SERIAL_8N1 (8 bits, sin paridad, 1 bit de parada)
// GPIO12 como RX (recepción)
// GPIO15 como TX (transmisión)


void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 34, 12);   
  // 15-TX 12-RX  Los pins originales no proporcionan información  "No GPS data received: check wiring"
  // 17-TX 18-RX  idem
  // 12-TX 34-RX  Para T-Beam V1.2
}

void loop()
{
  Serial.print("Latitude  : ");
  Serial.println(gps.location.lat(), 5);
  Serial.print("Longitude : ");
  Serial.println(gps.location.lng(), 4);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  Serial.print("Altitude  : ");
  Serial.print(gps.altitude.feet() / 3.2808);
  Serial.println("M");
  Serial.print("Time      : ");
  Serial.print(gps.time.hour());
  Serial.print(":");
  Serial.print(gps.time.minute());
  Serial.print(":");
  Serial.println(gps.time.second());
  Serial.println("**********************");

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

