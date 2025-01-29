# ShooterX - Advanced Gyro & Button-Based Game Controller 🎮  

**ShooterX** is a highly customizable **ESP32-based game controller** designed for first-person shooter (FPS) games. It combines **motion control (MPU6050 gyro)**, **rotary encoders**, **push buttons**, and **touch sensors** to enhance aiming, movement, and weapon handling with **Bluetooth HID support**.

## 🚀 Key Features  
- **🎯 Gyroscopic Aim Control** - Move the mouse cursor using motion-based controls.  
- **🔘 Button-Based Actions** - Customizable key bindings for movement, shooting, and actions.  
- **🔄 Rotary Encoder Weapon Selection** - Smooth weapon switching using a rotary encoder.  
- **💥 Vibration Feedback** - Haptic feedback when firing for a more immersive experience.  
- **📡 Bluetooth HID Support** - Works wirelessly with PCs and mobile devices.  

**ShooterX** provides an intuitive **gyro-based mouse movement**, allowing for smooth aim tracking while incorporating **button and rotary encoder-based inputs** for seamless gaming control. Perfect for **DIY gaming peripherals** and **custom controller enthusiasts**!  

## 📜 Getting Started  

### 1️⃣ Hardware Requirements  
- **ESP32 Board** (with Bluetooth support)  
- **MPU6050 Gyroscope**  
- **Rotary Encoders** (for weapon and throwable selection)  
- **Push Buttons / Touch Sensors**  
- **Vibration Motor (Optional)**  

### 2️⃣ Software Setup  
1. Install **Arduino IDE** and ESP32 board support.  
2. Install required libraries:  
   - `BleComboKeyboard`  
   - `BleComboMouse`  
   - `Adafruit MPU6050`  
   - `AiEsp32RotaryEncoder`  
3. Flash the firmware to the ESP32.  

### 3️⃣ Usage  
- Pair the device with your PC or mobile via **Bluetooth**.  
- Customize button mappings in the code if needed.  
- Enjoy precise motion-controlled aiming and responsive actions in FPS games!  

## 🛠️ Customization  
Modify `ShooterX.ino` to adjust:  
- **Button mappings** for different actions.  
- **Gyro sensitivity** for cursor movement.  
- **Rotary encoder behavior** for switching weapons.  

## 🎮 Supported Games  
ShooterX can be used with **any game that supports keyboard & mouse input**, such as:  
- **Counter-Strike: Global Offensive (CS:GO)**  
- **Call of Duty**  
- **Valorant**  
- **Apex Legends**  
- **Fortnite**  
- And more...  

## 🔥 Contributing  
Contributions are welcome! Feel free to fork the repo and submit pull requests for improvements, bug fixes, or new features.  

## 📜 License  
This project is **open-source** under the **MIT License**.  

---
🚀 **ShooterX - Elevate Your Game with Motion Precision!** 🎮
