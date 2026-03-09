#include <Stepper.h>

const int pulsePin = 2;
volatile uint16_t pulseCount = 0;

unsigned long lastSend = 0;
unsigned long interval = 10000;

// ----- Stepper configuration -----
const int stepsPerRevolution = 200;   // change to match your motor

// H-bridge control pins
const int IN1 = 3;
const int IN2 = 4;
const int IN3 = 5;
const int IN4 = 6;

// create stepper object
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

String command = "";

// Interrupt routine
void IRAM_ATTR pulseISR()
{
  if (pulseCount < 65535)
    pulseCount++;
}

void setup(){
  Serial.begin(115200);

  pinMode(pulsePin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulsePin), pulseISR, RISING);

  // stepper speed (RPM)
  myStepper.setSpeed(60);

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
      myStepper.step(steps);
      Serial.println("Move complete");
    }
  }
}
