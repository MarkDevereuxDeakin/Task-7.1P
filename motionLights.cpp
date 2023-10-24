// C++ code
// References: https://docs.arduino.cc/built-in-examples/sensors/Ping

//			   https://www.tinkercad.com/projects/Ultrasonic-Distance-Sensor-Arduino-Tinkercad

//			   https://www.tinkercad.com/things/3kCYZfx9fXN-multiple-leds-/
//             editel?lessonid=ELB4FYAJD0K8TZV&projectid=O94BCSIL26F9EV3&collectionid=O0K87SQL1W5N4P2&tenant=circuits#/lesson-viewer

//			   https://www.tinkercad.com/things/8ZN0hPJWOfw-ultrasonic-distance-sensor/
//			   editel?lessonid=EAYGNT8JL6UYXE2&projectid=O3TAWASL26F9H1J&collectionid=O0K87SQL1W5N4P2#/lesson-viewer

//			   https://www.youtube.com/watch?v=Uv9UeYUsA8A

//			   https://github.com/Arduino-IRremote/Arduino-IRremote#examples-for-this-library

//			   https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/examples/SendAndReceive/SendAndReceive.ino#L167-L170



#include <IRremote.hpp>

const uint8_t IR_RECEIVE_PIN = 2;
//Left Sensor
const uint8_t TRIGGER_PIN_LEFT = 9;
const uint8_t ECHO_PIN_LEFT = 8;

const uint8_t LED_RIGHT = 3;
const uint8_t LED_CENTRE = 4;
const uint8_t LED_LEFT = 5;

const int pResistor = A0;

int distanceThreshold = 300;
int distance = 0;

uint8_t ledState_RIGHT = LOW;
uint8_t ledState_CENTRE = LOW;
uint8_t ledState_LEFT = LOW;
uint8_t lightStatus = 0;

volatile uint8_t systemStatus = HIGH;
int value;

void setup()
{
  Serial.begin(115200); 
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);//start the receiver
    
  pinMode(LED_RIGHT, OUTPUT);
  pinMode(LED_CENTRE, OUTPUT);
  pinMode(LED_LEFT, OUTPUT);
  pinMode(pResistor, INPUT);
  
  startTimer();
  attachInterrupt(digitalPinToInterrupt(IR_RECEIVE_PIN), systemToggle, RISING);
   
   //Enable Pin Change Interrupt on Port C   
  //PCICR  |= B00000001; //Enable PCMSK0 (Group 0: PCINT0 to PCINT7 (D8 - D13)
  //PCMSK0 |= B00001010;//D10 will trigger interrupt   
}  

void systemToggle()
{  
  if (IrReceiver.decode() && IrReceiver.decodedIRData.command == 0)
  {
    systemStatus = !systemStatus;   
    
    if (systemStatus == 0)
    {
        digitalWrite(ECHO_PIN_LEFT, LOW);
      	 
      	updateStatus(LOW, LOW, LOW);
    	Serial.println("Smart lights Off!!");
      	Serial.println("");
      	Serial.println("Motion Sensors Deactivated!!");
      	Serial.println("");
      	IrReceiver.resume();
    }
    else
    { 
      Serial.println("Smart lights On!!");
      Serial.println("");
      Serial.println("Motion Sensors Activated!!");
      Serial.println("");
      IrReceiver.resume();
    }   
   
  }
  
}

void startTimer()
{
  	//turn off interrupts while configuring registers
    noInterrupts();//cli(); does the same thing
	
    //define registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    TCCR1B |= (1 << WGM12);// CTC mode. Clears timer on compare

    //Clock Select Bit Description settings
    TCCR1B |= (1 << CS12);// CS12 and CS10 need to be ON for 1024 prescaler
    TCCR1B |= (1 << CS10);// CS12 and CS10 need to be ON for 1024 prescaler
    //TCCR1B |= B00000101; is the same as above
  
    //Output compare register (adjusted clock speed for timing)
    OCR1A = 15625/64; // (16000000/(1024))
    
    //Time Mask Register compared to Output compare register
    TIMSK1 = (1 << OCIE1A); //enable time1 compare interrupt

    interrupts();//sei(); does the same thing 
  
}

ISR(TIMER1_COMPA_vect)
{
  if(systemStatus == 1)
  { 	
    readUltrasonicDistanceLeft();
    
  }  
       
}

ISR(PCINT0_vect)
{     
  //Empty for pin change interrupt
}

void readUltrasonicDistanceLeft()
{  
    pinMode(TRIGGER_PIN_LEFT, OUTPUT); //clears the signal
   	
    digitalWrite(TRIGGER_PIN_LEFT, LOW);
  	
    delayMicroseconds(2);  
    // Sets the trigger pin to HIGH state for 10 microseconds
    digitalWrite(TRIGGER_PIN_LEFT, HIGH);
  	
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN_LEFT, LOW);
  	

    pinMode(ECHO_PIN_LEFT, INPUT);
  	
	
  if (systemStatus == 1)
   {    	
      distance = 0.01723 * pulseIn(ECHO_PIN_LEFT, HIGH);
      handleDetectionLeft(distance);
      
   }
    
}

void updateStatus(byte right, byte centre, byte left)
{
	digitalWrite(LED_RIGHT, right);
  	digitalWrite(LED_CENTRE, centre);
  	digitalWrite(LED_LEFT, left);
}

void lightsToActivate()
{
  switch(lightStatus)
  {
    case 0: 
    	Serial.print("Nothing Detected: ");
    	Serial.println("No Lights Turned On!");
    break;
    case 1:
    	Serial.print("Motion Detected: ");
    	Serial.println("First Light Turned On!");
    	Serial.println(distance);
    break;
    case 2:
    	Serial.print("Motion Detected: ");
    	Serial.println("Second Light Turned On!");
    	Serial.println(distance);
    break;
    case 3:
    	Serial.print("Motion Detected: ");
    	Serial.println("Third Light Turned On!");
    	Serial.println(distance);
    break;
    
  } 
  
}

void handleDetectionLeft(int distance)
{

  if (distance > distanceThreshold) {
    	lightStatus = 0;
    	updateStatus(LOW, LOW, LOW);
    	lightsToActivate();
  }
  if (distance <= distanceThreshold && distance > distanceThreshold - 100) {
    	lightStatus = 1;
    	updateStatus(LOW, LOW, HIGH);
    	lightsToActivate();    	
  }
  if (distance <= distanceThreshold - 100 && distance > distanceThreshold - 200) {
    	lightStatus = 2;
    	updateStatus(LOW, HIGH, HIGH);
    	lightsToActivate(); 
  }
  if (distance <= distanceThreshold - 200 && distance > distanceThreshold - 300) {
    	lightStatus = 3;
    	updateStatus(HIGH, HIGH, HIGH);
    	lightsToActivate(); 
  }
    
}



void loop()
{
  value = analogRead(pResistor);
  if(distance < distanceThreshold && value < 100)
  {
    Serial.println("Globe is Broken!! Please Replace Globe");
  }
}