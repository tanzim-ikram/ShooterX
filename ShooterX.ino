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

// Track last button states
bool fireHeld = false;
bool aimHeld = false;
bool forwardHeld = false;
bool backwardHeld = false;
bool sitHeld = false;
bool reloadHeld = false;
bool jumpHeld = false;
bool middleMouseHeld = false;

int lastWeaponValue = 1;  // Track last encoder value for weapon switching

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
  changeWeaponRotaryEncoder.setBoundaries(1, 4, true);

  mpu.enableSleep(sleepMPU);
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

    // Handle Mouse Buttons as "Hold and Release"
    handleMouseButton(AIM_BUTTON, &aimHeld, MOUSE_RIGHT);
    handleMouseButton(FIRE_BUTTON, &fireHeld, MOUSE_LEFT);
    handleMouseButton(THROW_WEAPON_ROTARY_SW, &middleMouseHeld, MOUSE_MIDDLE);

    // Handle Keyboard Buttons
    handleKeyboardButton(FORWARD_MOVE_BUTTON, &forwardHeld, 'w');
    handleKeyboardButton(BACKWARD_MOVE_BUTTON, &backwardHeld, 's');
    handleKeyboardButton(SIT_BUTTON, &sitHeld, 'c');
    handleKeyboardButton(RELOAD_BUTTON, &reloadHeld, 'r');
    handleKeyboardButton(JUMP_BUTTON, &jumpHeld, ' ');

    // Handle throwWeaponRotaryEncoder (mouse scroll)
    if (throwWeaponRotaryEncoder.encoderChanged()) {
      int encoderValue = throwWeaponRotaryEncoder.readEncoder();
      static int lastEncoderValue = encoderValue;

      if (encoderValue > lastEncoderValue) {
        Serial.println("Scrolling up (Throw Weapon)");
        combo.move(0, 0, 1);
      } else if (encoderValue < lastEncoderValue) {
        Serial.println("Scrolling down (Throw Weapon)");
        combo.move(0, 0, -1);
      }
      lastEncoderValue = encoderValue;
    }

    // Handle changeWeaponRotaryEncoder (press top row 1, 2, 3, 4 keys)
    if (changeWeaponRotaryEncoder.encoderChanged()) {
      int encoderValue = changeWeaponRotaryEncoder.readEncoder();

      if (encoderValue != lastWeaponValue) {
        if (encoderValue >= 1 && encoderValue <= 4) {
          char key = '0' + encoderValue; // Converts 1->'1', 2->'2', etc.
          Serial.print("Key press (Change Weapon): ");
          Serial.println(key);
          keyboard.press(key);
          delay(100);
          keyboard.release(key);
          lastWeaponValue = encoderValue;
        }
      }
    }
  }
}

// Function to Handle Keyboard Key Press and Release
void handleKeyboardButton(int buttonPin, bool *buttonState, char key) {
  bool pressed = !digitalRead(buttonPin);

  if (pressed && !(*buttonState)) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    keyboard.press(key);
    *buttonState = true;
  } else if (!pressed && *buttonState) {
    Serial.print("Key released: ");
    Serial.println(key);
    keyboard.release(key);
    *buttonState = false;
  }
}

// Function to Handle Mouse Button Hold and Release
void handleMouseButton(int buttonPin, bool *buttonState, uint8_t mouseButton) {
  bool pressed = !digitalRead(buttonPin);

  if (pressed && !(*buttonState)) {
    Serial.print("Mouse button pressed: ");
    Serial.println(mouseButton == MOUSE_LEFT ? "Left Click" : 
                   mouseButton == MOUSE_RIGHT ? "Right Click" : "Middle Click");
    combo.press(mouseButton);  // Hold the button down
    *buttonState = true;
  } else if (!pressed && *buttonState) {
    Serial.print("Mouse button released: ");
    Serial.println(mouseButton == MOUSE_LEFT ? "Left Click" : 
                   mouseButton == MOUSE_RIGHT ? "Right Click" : "Middle Click");
    combo.release(mouseButton); // Release the button
    *buttonState = false;
  }
}
