#include <BleComboKeyboard.h>
#include <BleComboMouse.h>
#include <Adafruit_MPU6050.h>
#include "AiEsp32RotaryEncoder.h"

#define FIRE_BUTTON 19
#define AIM_BUTTON 18
#define THROW_WEAPON_ROTARY_CLK 25
#define THROW_WEAPON_ROTARY_DT 26
#define THROW_WEAPON_ROTARY_SW 27
#define FORWARD_MOVE_BUTTON 15
#define BACKWARD_MOVE_BUTTON 4
#define CHANGE_WEAPON_ROTARY_CLK 12
#define CHANGE_WEAPON_ROTARY_DT 13
#define VIBRATOR_PIN 14
#define SIT_BUTTON 23
#define RELOAD_BUTTON 33
#define JUMP_BUTTON 32
#define SPEED 8
#define ROTARY_STEPS 4

Adafruit_MPU6050 mpu;
BleComboKeyboard keyboard("ShooterX");
BleComboMouse combo(&keyboard);
AiEsp32RotaryEncoder throwWeaponRotaryEncoder = AiEsp32RotaryEncoder(THROW_WEAPON_ROTARY_DT, THROW_WEAPON_ROTARY_CLK, THROW_WEAPON_ROTARY_SW, -1, ROTARY_STEPS);
AiEsp32RotaryEncoder changeWeaponRotaryEncoder = AiEsp32RotaryEncoder(CHANGE_WEAPON_ROTARY_DT, CHANGE_WEAPON_ROTARY_CLK, -1, -1, ROTARY_STEPS);

bool sleepMPU = true;
long mpuDelayMillis;

bool buttonHeldStates[7];
unsigned long debounceTime = 50;
unsigned long lastDebounceTimes[7];

void IRAM_ATTR readThrowWeaponEncoderISR() {
  throwWeaponRotaryEncoder.readEncoder_ISR();
}

void IRAM_ATTR readChangeWeaponEncoderISR() {
  changeWeaponRotaryEncoder.readEncoder_ISR();
}

void setup() {
  Serial.begin(115200);

  pinMode(FIRE_BUTTON, INPUT_PULLUP);
  pinMode(AIM_BUTTON, INPUT_PULLUP);
  pinMode(FORWARD_MOVE_BUTTON, INPUT_PULLUP);
  pinMode(BACKWARD_MOVE_BUTTON, INPUT_PULLUP);
  pinMode(VIBRATOR_PIN, OUTPUT);
  pinMode(SIT_BUTTON, INPUT_PULLUP);
  pinMode(RELOAD_BUTTON, INPUT_PULLUP);
  pinMode(JUMP_BUTTON, INPUT_PULLUP);
  digitalWrite(VIBRATOR_PIN, LOW);

  keyboard.begin();
  combo.begin();

  delay(1000);
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  throwWeaponRotaryEncoder.begin();
  throwWeaponRotaryEncoder.setup(readThrowWeaponEncoderISR);
  throwWeaponRotaryEncoder.setBoundaries(0, 1000, false);
  throwWeaponRotaryEncoder.setAcceleration(250);

  changeWeaponRotaryEncoder.begin();
  changeWeaponRotaryEncoder.setup(readChangeWeaponEncoderISR);
  changeWeaponRotaryEncoder.setBoundaries(1, 4, true); // Set boundaries to 1-4

  mpu.enableSleep(sleepMPU);

  for (int i = 0; i < 7; i++) {
    lastDebounceTimes[i] = 0;
    buttonHeldStates[i] = false;
  }
}

void loop() {
  if (keyboard.isConnected()) {
    if (sleepMPU) {
      delay(3000);
      Serial.println("MPU6050 awakened!");
      sleepMPU = false;
      mpu.enableSleep(sleepMPU);
      delay(500);
    }

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    combo.move((int)(g.gyro.z * -SPEED * 1.5), (int)(g.gyro.x * -SPEED * 1.5));

    if (!digitalRead(AIM_BUTTON)) {
      Serial.println("Right click");
      combo.click(MOUSE_RIGHT);
    }

    if (!digitalRead(FIRE_BUTTON)) {
      Serial.println("Left click and Vibrate");
      combo.click(MOUSE_LEFT);
      digitalWrite(VIBRATOR_PIN, HIGH);
      delay(200);
      digitalWrite(VIBRATOR_PIN, LOW);
    }

    handleButton(FORWARD_MOVE_BUTTON, 'w', 2);
    handleButton(BACKWARD_MOVE_BUTTON, 's', 3);
    handleButton(SIT_BUTTON, 'c', 4);
    handleButton(RELOAD_BUTTON, 'r', 5);
    handleButton(JUMP_BUTTON, ' ', 6);

    // Handle throwWeaponRotaryEncoder (mouse scroll)
    if (throwWeaponRotaryEncoder.encoderChanged()) {
      int encoderValue = throwWeaponRotaryEncoder.readEncoder();
      static int lastEncoderValue = encoderValue;

      if (encoderValue > lastEncoderValue) {
        Serial.println("Scrolling up (Throw Weapon)");
        combo.move(0, 0, 1); // Scroll up
      } else if (encoderValue < lastEncoderValue) {
        Serial.println("Scrolling down (Throw Weapon)");
        combo.move(0, 0, -1); // Scroll down
      }
      lastEncoderValue = encoderValue;
    }

    if (throwWeaponRotaryEncoder.isEncoderButtonClicked()) {
      Serial.println("Middle click");
      combo.click(MOUSE_MIDDLE);
      delay(200);
    }

    // Handle changeWeaponRotaryEncoder (press 1-4)
    if (changeWeaponRotaryEncoder.encoderChanged()) {
      int encoderValue = changeWeaponRotaryEncoder.readEncoder();
      static int lastEncoderValue = encoderValue;

      if (encoderValue != lastEncoderValue) {
        char key = '0' + encoderValue; // Map encoder value to keys '1' to '4'
        Serial.print("Key press (Change Weapon): ");
        Serial.println(key);
        keyboard.press(key);
        delay(100); // Simulate key press duration
        keyboard.release(key);
        lastEncoderValue = encoderValue;
      }
    }
  }
}

void handleButton(int buttonPin, char key, int index) {
  if (!digitalRead(buttonPin)) {
    if (!buttonHeldStates[index]) {
      Serial.print("Key pressed: ");
      Serial.println(key);
      keyboard.press(key);
      buttonHeldStates[index] = true;
    }
  } else {
    if (buttonHeldStates[index]) {
      Serial.print("Key released: ");
      Serial.println(key);
      keyboard.release(key);
      buttonHeldStates[index] = false;
    }
  }
}