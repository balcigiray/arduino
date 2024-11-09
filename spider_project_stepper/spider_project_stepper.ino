#define IDLE 0
#define RUN 1
#define CALIB 2

int STATE = 0;

const int motor_pin_1 = 8;
const int motor_pin_2 = 9;
const int motor_pin_3 = 10;
const int motor_pin_4 = 11;

const int motorPhA_en = 5;
const int motorPhB_en = 6;

const int BTN_PIN = A3;
const int POT_PIN = A0;
const int BTN_DEMO = A4;

const int LED_PIN = 13;


const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

float adcRead = 0;
bool btnState = true;

// initialize the stepper library on pins 8 through 11:
int dt = 100;

// TIMING VARIABLES
unsigned long timeNow = 0;
unsigned long timePrev = 0;
unsigned long timeSeperation = 2000;  //in ms

unsigned long timeSerialPrev = 0;
const unsigned long timeSerialPeriod = 1000;  //in ms

int positionInSteps = 0;
int delayBtwSteps = 5;
int steps = 50;
int direction = 0;

void setup() {
  // initialize the serial port:
  pinMode(motorPhA_en, OUTPUT);
  pinMode(motorPhB_en, OUTPUT);
  pinMode(motor_pin_1, OUTPUT);
  pinMode(motor_pin_2, OUTPUT);
  pinMode(motor_pin_3, OUTPUT);
  pinMode(motor_pin_4, OUTPUT);

  digitalWrite(motorPhA_en, HIGH);
  digitalWrite(motorPhB_en, HIGH);

  pinMode(POT_PIN, INPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_DEMO, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);

  // set random seed
  randomSeed(analogRead(A1));

  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  // step one revolution in one direction:
  timeNow = millis();

  adcRead = analogRead(POT_PIN);
  btnState = digitalRead(BTN_PIN);

  //defaul state is IDLE
  STATE = IDLE;

  // if button is pressed, set state CALIB
  if (!btnState) {
    STATE = CALIB;
  }

  //enoug time passed, set state RUN
  if ((timeNow - timePrev) > timeSeperation) {
    timePrev = timeNow;
    // generate next random time seperation
    if (digitalRead(BTN_DEMO)) {
      timeSeperation = random(10000, 100000);
    } else {
      //if read is 0, enter demo mode 
      timeSeperation = 6000;
    }

    if (STATE != CALIB) {
      STATE = RUN;
    }
  }

  switch (STATE) {
    case IDLE:  //0
      digitalWrite(motorPhA_en, LOW);
      digitalWrite(motorPhB_en, LOW);
      delay(1);
      break;

    case RUN:  //1
      Serial.println("in run");
      digitalWrite(motorPhA_en, HIGH);
      digitalWrite(motorPhB_en, HIGH);
      delay(1);

      dt = map(adcRead, 0, 1024, 2, 200);
      delayBtwSteps = random(1150, 1400);
      Serial.print("delay in steps: ");
      Serial.println(delayBtwSteps);

      //30-80
      steps = random(60, 200);  //actual steps is 4 times
      direction = random(0, 1001);
      //delayBtwSteps = 5;
      //steps = 50;
      Serial.print("steps: ");
      Serial.println(steps);


      if (direction > 500) {
        if (positionInSteps + (4 * steps) < 1000) {
          RunStepperForward(delayBtwSteps, steps);
        } else {
          RunStepperBackward(delayBtwSteps, steps);
        }
      } else {
        if (positionInSteps - (4 * steps) > -1000) {
          RunStepperBackward(delayBtwSteps, steps);
        } else {
          RunStepperForward(delayBtwSteps, steps);
        }
      }
      //additional time offset: 4 per function per steps per speed + 2 ms flywheel
      timeSeperation = timeSeperation + 4 * steps * delayBtwSteps / 1000 + 2;
      break;

    case CALIB:  //2
      //Serial.println("in calib");
      digitalWrite(motorPhA_en, HIGH);
      digitalWrite(motorPhB_en, HIGH);
      delay(1);

      positionInSteps = 0;

      if (adcRead > 512) {
        dt = map(adcRead, 512, 1024, 10000, 1100);
        //Serial.println("forward calib");
        RunStepperForward(dt, 1);

      } else {
        dt = map(adcRead, 0, 511, 1100, 10000);
        RunStepperBackward(dt, 1);
        //Serial.println("backward calib");
      }

      break;

    default:
      //Serial.println("nothing happeed");
      break;
  }


  //enoug time passed, print serial
  if ((timeNow - timeSerialPrev) > timeSerialPeriod) {
    timeSerialPrev = timeNow;
    // generate next random time seperation
    Serial.print("Position: ");
    Serial.println(positionInSteps);

    Serial.print("State: ");
    Serial.println(STATE);

    Serial.print("ADC: ");
    Serial.println(adcRead);
  }


  delay(10);
  // end of loop()
}



void RunStepperForward(int _stepDelay, int _steps) {
  // freewheeling delay for phase current decay
  int dfly = 300;
  //_stepDelay = 800;

  for (int i = 0; i < _steps; i++) {
    digitalWrite(motor_pin_1, HIGH);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, HIGH);
    digitalWrite(motor_pin_4, LOW);
    digitalWrite(LED_PIN, HIGH);
    //delay(_stepDelay);
    delayMicroseconds(_stepDelay);
    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);

    delayMicroseconds(dfly);

    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, HIGH);
    digitalWrite(motor_pin_3, HIGH);
    digitalWrite(motor_pin_4, LOW);
    digitalWrite(LED_PIN, LOW);
    //delay(_stepDelay);
    delayMicroseconds(_stepDelay);

    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);
    delayMicroseconds(dfly);

    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, HIGH);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, HIGH);
    digitalWrite(LED_PIN, HIGH);
    //delay(_stepDelay);
    delayMicroseconds(_stepDelay);

    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);
    delayMicroseconds(dfly);

    digitalWrite(motor_pin_1, HIGH);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, HIGH);
    digitalWrite(LED_PIN, LOW);
    //delay(_stepDelay);
    delayMicroseconds(_stepDelay);
    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);
    delayMicroseconds(dfly);

    positionInSteps += 4;
  }
}

void RunStepperBackward(int _stepDelay, int _steps) {
  // freewheeling delay for phase current decay
  int dfly = 300;
  //_stepDelay = 800;

  for (int i = 0; i < _steps; i++) {
    digitalWrite(motor_pin_1, HIGH);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, HIGH);
    digitalWrite(motor_pin_4, LOW);
    digitalWrite(LED_PIN, HIGH);
    //delay(_stepDelay);
    delayMicroseconds(_stepDelay);
    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);
    delayMicroseconds(dfly);

    digitalWrite(motor_pin_1, HIGH);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, HIGH);
    digitalWrite(LED_PIN, LOW);
    //delay(_stepDelay);
    delayMicroseconds(_stepDelay);
    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);
    delayMicroseconds(dfly);

    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, HIGH);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, HIGH);
    digitalWrite(LED_PIN, HIGH);
    //delay(_stepDelay);
    delayMicroseconds(_stepDelay);

    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);
    delayMicroseconds(dfly);

    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, HIGH);
    digitalWrite(motor_pin_3, HIGH);
    digitalWrite(motor_pin_4, LOW);
    digitalWrite(LED_PIN, LOW);
    //delay(_stepDelay);
    delayMicroseconds(_stepDelay);
    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);
    delayMicroseconds(dfly);
    positionInSteps -= 4;
  }
}
