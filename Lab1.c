// definicija pinova
#define LED_INT0 10   
#define LED_INT1 9  
#define LED_INT2 8
#define LED_Timer 13
#define LED_Sensor 12 

// definiranje gumba
#define button_INT0 2
#define button_INT1 3
#define button_INT2 21

#define TIMER1_PRESCALER 1024
#define TIMER1_COMPARE 15624
const int TIMER_DELAY = 1000;
volatile unsigned long lastTimerTime = 0;
volatile bool timerFlag = false;

#define DEBOUNCE_DELAY 50

volatile unsigned long lastInterruptTime_INT0 = 0;
volatile unsigned long lastInterruptTime_INT1 = 0;
volatile unsigned long lastInterruptTime_INT2 = 0;

// varijable za praćenje trenutnog stanja
volatile bool INT0_pressed = false;
volatile bool INT1_pressed = false;
volatile bool INT2_pressed = false;

volatile bool LED_INT0_active = false; // praćenje stanja LED_INT0
volatile bool LED_INT1_active = false; // praćenje stanja LED_INT1
volatile bool LED_INT2_active = false; // praćenje stanja LED_INT2

// dodavanje varijabli za HC-SR04 senzor
#define TRIG_PIN 6
#define ECHO_PIN 7
#define MAX_DISTANCE 400  // maksimalna udaljenost koju može mjeriti HC-SR04 (cm)

unsigned long duration;
unsigned long lastLEDSensorBlinkTime = 0;
int distance;

unsigned long lastDistancePrintTime = 0;  // varijabla za praćenje vremena ispisivanja udaljenosti

// funkcija za titranje INT0, INT1 i INT2 LED svijetla
void blinkLED(int ledPin) {
  unsigned long startMillis = millis();
  unsigned long currentMillis;  

  while (millis() - startMillis < 1000) {  // LED titranje koje traje 1s
    currentMillis = millis();  

    // LED titra svakih 100 ms unutar 1 sekunde (ON/OFF 100 ms)
    if ((currentMillis - startMillis) % 200 < 100) {
      digitalWrite(ledPin, HIGH);  
    } else {
      digitalWrite(ledPin, LOW);   
    }
  }

  digitalWrite(ledPin, LOW); 
}

// funkcija za titranje LED svijetla udaljenosti
void blinkLED_Distance(int ledPin, unsigned long *lastBlinkTime) {
  unsigned long currentMillis = millis();

  // provjera je li prošlo više od 200 ms od zadnjeg titranja
  if (currentMillis - *lastBlinkTime >= 200) {
    digitalWrite(ledPin, !digitalRead(ledPin));  // promijeni stanje LED-a
    *lastBlinkTime = currentMillis;  // ažuriraj vrijeme zadnjeg titranja
  }
}


void INT0Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime_INT0 > DEBOUNCE_DELAY && !LED_INT0_active) {
    lastInterruptTime_INT0 = interruptTime;
    INT0_pressed = true;  
    LED_INT0_active = true; 
  }
}

void INT1Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime_INT1 > DEBOUNCE_DELAY && !LED_INT1_active) {
    lastInterruptTime_INT1 = interruptTime;
    INT1_pressed = true;  
    LED_INT1_active = true;
  }
}

void INT2Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime_INT2 > DEBOUNCE_DELAY && !LED_INT2_active) {
    lastInterruptTime_INT2 = interruptTime;
    INT2_pressed = true;  
    LED_INT2_active = true;
  }
}

void setup() {
  pinMode(button_INT0, INPUT_PULLUP);  
  pinMode(button_INT1, INPUT_PULLUP);  
  pinMode(button_INT2, INPUT_PULLUP);  
  pinMode(LED_INT0, OUTPUT);  
  pinMode(LED_INT1, OUTPUT);  
  pinMode(LED_INT2, OUTPUT); 
  pinMode(LED_Timer, OUTPUT);
  pinMode(LED_Sensor, OUTPUT); 

  pinMode(TRIG_PIN, OUTPUT);  
  pinMode(ECHO_PIN, INPUT);   

  attachInterrupt(digitalPinToInterrupt(button_INT0), INT0Interrupt, FALLING);  // INT0 
  attachInterrupt(digitalPinToInterrupt(button_INT1), INT1Interrupt, FALLING);  // INT1 
  attachInterrupt(digitalPinToInterrupt(button_INT2), INT2Interrupt, FALLING);  // INT2

  // postavljanje timera
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
  OCR1A = TIMER1_COMPARE;
  TIMSK1 = (1 << OCIE1A);

  Serial.begin(9600);
  Serial.println("Arduino je pokrenut.");
}

void loop() {

  if (timerFlag) {
    digitalWrite(LED_Timer, HIGH);
    Serial.println("Timer LED upaljen!");
    delay(100);
    digitalWrite(LED_Timer, LOW);
    timerFlag = false;    
  }

  if (INT0_pressed) {
    INT0_pressed = false;  
    blinkLED(LED_INT0); 
    Serial.println("Gumb INT0 pritisnut!"); 
    LED_INT0_active = false; 
  }
  else if (INT1_pressed) {
    INT1_pressed = false;  
    blinkLED(LED_INT1);  
    Serial.println("Gumb INT1 pritisnut!");
    LED_INT1_active = false; 
  }
  else if (INT2_pressed) {
    INT2_pressed = false;  
    blinkLED(LED_INT2);  
    Serial.println("Gumb INT2 pritisnut!");
    LED_INT2_active = false; 
  }

  distance = getDistance();

  // provjera je li prošla 1s od zadnjeg ispisivanja udaljenosti
  unsigned long currentMillis = millis();
  if (currentMillis - lastDistancePrintTime >= 1000) {
    Serial.print("Udaljenost: ");
    Serial.print(distance);
    Serial.println(" cm");
    lastDistancePrintTime = currentMillis; 
  }

  if (distance < 100) {
    blinkLED_Distance(LED_Sensor, &lastLEDSensorBlinkTime); 
  }
}

// funkcija za mjerenje udaljenosti s HC-SR04
int getDistance() {
  // pokreni TRIG signal
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // mjeri vrijeme trajanja ECHO signala
  duration = pulseIn(ECHO_PIN, HIGH, 50000); 

  if (duration == 0) {
    return MAX_DISTANCE;  // timeout
  }

  // računanje udaljenosti u centimetrima
  int distance = duration * 0.034 / 2;  
  return distance;
}

// timer interrupt
ISR(TIMER1_COMPA_vect) {
  timerFlag = true;
  lastTimerTime = millis();
}
