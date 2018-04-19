/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Create a WiFi access point and provide a web server on it. */
#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const int PIN_INPUT = A0;

const int PIN_BLINK = 13;    // Pin 13 is the on-board LED
const int PIN_FADE = 5;
const int THRESHOLD = 550;
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;
const char *ssid = "HeartQuake2";
const char *password = "Mdpdemerde93";
const int buzzer = D2;

int BPM = 0;
ESP8266WebServer server(80);
PulseSensorPlayground pulseSensor;


/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  //String value = String("{\"bpm\":")+BPM+String("}");
	server.send(200, "application/json", String("{\"bpm\":")+GetBPM()+String("}"));
}

void setup() {
  
	delay(1000);
	Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  Serial.println(buzzer);
  digitalWrite(buzzer, HIGH);
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
	server.begin();
	Serial.println("HTTP server started");

  pulseSensor.analogInput(PIN_INPUT);
  pulseSensor.blinkOnPulse(PIN_BLINK);
  pulseSensor.fadeOnPulse(PIN_FADE);

  pulseSensor.setSerial(Serial);
  //pulseSensor.setOutputType(OUTPUT_TYPE);
  //pulseSensor.setThreshold(THRESHOLD);

  // Skip the first SAMPLES_PER_SERIAL_SAMPLE in the loop().
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    /*
       PulseSensor initialization failed,
       likely because our Arduino platform interrupts
       aren't supported yet.

       If your Sketch hangs here, try changing USE_PS_INTERRUPT to false.
    */
    
  }
}

int GetBPM(){
  pinMode(buzzer, OUTPUT);
  tone(buzzer, HIGH);
  Serial.println(buzzer);
  int tempval = BPM;
  while(BPM == tempval){
    if (pulseSensor.sawNewSample()) {
      if (--samplesUntilReport == (byte) 0) {
        samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
        pulseSensor.outputSample();
        Serial.print("BPM: ");
        BPM = pulseSensor.getBeatsPerMinute(0);
        if(BPM<200){
          tone(buzzer, HIGH);
        }else{
          //noTone(buzzer);
        }
        Serial.print(BPM);
        
        Serial.println(" beats/minutes");
        
        if (pulseSensor.sawStartOfBeat()) {
          pulseSensor.outputBeat();
        }
      }
    }
    delay(0);
  }
  return BPM;
}

void loop() {
	
  server.handleClient();
}
