#define OUT_LEFT 3
#define OUT_RIGHT 5
#define OUT_REAR 6
#define OUT_HEAD1 9
#define OUT_HEAD2 10
#define IN_LEFT 7
#define IN_RIGHT 8
#define DEBUG LED_BUILTIN

#define LEVEL_HIGH 100
#define LEVEL_MED 40
#define LEVEL_LOW 10
#define LEVEL_OFF 0
#define ACTIVE LOW
#define INACTIVE HIGH

#define TURN_STATE_MS 600
#define TURN_FLASH_MS 50
#define TURN_ON_MS (TURN_STATE_MS-TURN_FLASH_MS)
#define TURN_CYCLE_MS (TURN_STATE_MS*2)

#define LONG_PRESS_MS 2000


unsigned long turnFlashStart = 0;
unsigned long leftPressStart = 0;
unsigned long rightPressStart = 0;

boolean leftBlink = false;
boolean rightBlink = false;
boolean changingIntensity = false;

byte intensity = LEVEL_HIGH;

void setup() {
  pinMode(OUT_LEFT, OUTPUT);
  pinMode(OUT_RIGHT, OUTPUT);
  pinMode(OUT_REAR, OUTPUT);
  pinMode(OUT_HEAD1, OUTPUT);
  pinMode(OUT_HEAD2, OUTPUT);
  pinMode(IN_LEFT, INPUT_PULLUP);
  pinMode(IN_RIGHT, INPUT_PULLUP);
  autoTest();
}

void loop() {
  int left = digitalRead(IN_LEFT);
  int right = digitalRead(IN_RIGHT);
  handleInput(left, &leftPressStart, &leftBlink, false);
  handleInput(right, &rightPressStart, &rightBlink, true);
  if (left == ACTIVE || right == ACTIVE) {
    digitalWrite(DEBUG, HIGH);
  } else {
    digitalWrite(DEBUG, LOW);
  }
  
  if (leftBlink) {
    handleBlink(OUT_LEFT);
  } else {
    analogWrite(OUT_LEFT, LEVEL_OFF);
  }

  if (rightBlink) {
    handleBlink(OUT_RIGHT);
  } else {
    analogWrite(OUT_RIGHT, LEVEL_OFF);
  }

  delay(20);
}

void handleInput(int state, unsigned long* pressStartRef, boolean* blinkRef, boolean incrInt) {
  if (state == ACTIVE && (*pressStartRef) == 0) {
    (*pressStartRef) = millis();
  } else if ((*pressStartRef) != 0) {
    if (millis() - (*pressStartRef) > LONG_PRESS_MS) {
      if (incrInt) {
        increaseIntensity();
      } else {
        decreaseIntensity();
      }
      (*pressStartRef) = millis();
      changingIntensity = true;
    } else if (state == INACTIVE) {
      if (!changingIntensity) {
        if (!(*blinkRef)) {
          (*blinkRef) = true;
          turnFlashStart = millis();
        } else {
          (*blinkRef) = false;
        }
      }
      (*pressStartRef) = 0;
      changingIntensity = false;
    }
  }
}

void handleBlink(int pin) {
  int phase = (millis() - turnFlashStart) % TURN_CYCLE_MS;

  if (phase < TURN_FLASH_MS) {
    analogWrite(pin, LEVEL_HIGH);
  } else if (phase < TURN_STATE_MS) {
    analogWrite(pin, LEVEL_MED);
  } else {
    analogWrite(pin, LEVEL_OFF);
  }
}

void decreaseIntensity() {
  if (intensity == LEVEL_HIGH) {
    intensity = LEVEL_MED;
  } else {
    intensity = LEVEL_LOW;
  }
  updateIntensity();
}

void increaseIntensity() {
  if (intensity == LEVEL_LOW) {
    intensity = LEVEL_MED;
  } else {
    intensity = LEVEL_HIGH;
  }
  updateIntensity();
}

void updateIntensity() {
  analogWrite(OUT_HEAD1, intensity);
  analogWrite(OUT_HEAD2, intensity);
  analogWrite(OUT_REAR, intensity);
}

void autoTest() {
  analogWrite(OUT_LEFT, LEVEL_LOW);
  analogWrite(OUT_RIGHT, LEVEL_LOW);
  analogWrite(OUT_REAR, LEVEL_LOW);
  analogWrite(OUT_HEAD1, LEVEL_LOW);
  analogWrite(OUT_HEAD2, LEVEL_LOW);

  delay(500);

  analogWrite(OUT_LEFT, LEVEL_MED);
  analogWrite(OUT_RIGHT, LEVEL_MED);
  analogWrite(OUT_REAR, LEVEL_MED);
  analogWrite(OUT_HEAD1, LEVEL_MED);
  analogWrite(OUT_HEAD2, LEVEL_MED);

  delay(500);

  analogWrite(OUT_LEFT, LEVEL_HIGH);
  analogWrite(OUT_RIGHT, LEVEL_HIGH);
  analogWrite(OUT_REAR, LEVEL_HIGH);
  analogWrite(OUT_HEAD1, LEVEL_HIGH);
  analogWrite(OUT_HEAD2, LEVEL_HIGH);

  delay(500);

  analogWrite(OUT_LEFT, LEVEL_OFF);
  analogWrite(OUT_RIGHT, LEVEL_OFF);
  updateIntensity();
}


