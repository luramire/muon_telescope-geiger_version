#include <Stepper.h>

const int pulsePin = 10;
volatile uint16_t pulseCount = 0;

unsigned long lastSend = 0;
unsigned long interval = 10000;

// ----- Stepper configuration -----
const int stepsPerRevolution = 200;   // change to match your motor

// Stepper motor driver control pins

const int pinstep = 5;
const int pinenable = 6;
const int pindir = 7;

String command = "";

// delay between steps
int delayms=10;
  
// Interrupt routine
void IRAM_ATTR pulseISR()
{
  if (pulseCount < 65535)
    pulseCount++;
}

void setup(){
  Serial.begin(115200);

  pinMode(pinstep, OUTPUT);
  pinMode(pinenable, OUTPUT);
  pinMode(pindir, OUTPUT);
  digitalWrite(pinenable,0);
  pinMode(pulsePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulsePin), pulseISR, RISING);

  lastSend = millis();
}

void loop(){

  // Send count every 10 seconds
  if (millis() - lastSend >= interval){
    noInterrupts();
    uint16_t countCopy = pulseCount;
    pulseCount = 0;
    interrupts();

    Serial.print("Count: ");
    Serial.println(countCopy);

    lastSend = millis();
  }

  // Read serial commands
  if(Serial.available()>0){
    char command = Serial.read();
    if(command=='I'){
      while(Serial.available()==0);
      interval=Serial.parseInt();
      pulseCount = 0;
      Serial.print("Interval set to ");
      Serial.print(interval);
      Serial.println(" ms");
    }else if(command=='A'){
      while(Serial.available()==0);
      int steps=Serial.parseInt();
      if(steps>0){
        digitalWrite(pindir,0);
      }else{
        digitalWrite(pindir,1);
        steps=-steps;
      }
      digitalWrite(pinenable,1);
      for(int i=0; i<steps; i++){
        digitalWrite(pinstep,1);
        delay(delayms);
        digitalWrite(pinstep,0);
        delay(delayms);
      }
      digitalWrite(pinenable,0);
      Serial.println("Move complete");
    }else if(command=='D'){
      while(Serial.available()==0);
      delayms=Serial.parseInt();
      Serial.println("Motor speed updated");
    }
  }
}
