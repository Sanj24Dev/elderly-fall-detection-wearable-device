# Fall Detection and Health Monitoring System (FreeRTOS-based)

This project implements a real-time, multitasking fall detection and health monitoring system using ESP32, developed and tested on the [Wokwi simulator](https://wokwi.com/). The system collects data from rotation, temperature, distance, and acceleration sensors, processes this data in real-time using FreeRTOS, and pushes relevant readings to ThingSpeak.

## 🧠 Features

- **Fall Detection** using acceleration magnitude (MPU6050)
- **Body Monitoring** (Temperature using NTC Thermistor)
- **Distance calculation** (HCSR04 sensor)
- **OLED Display** output for temperature and fall alerts
- **Wi-Fi connectivity** to upload sensor data to [ThingSpeak](https://thingspeak.com/)
- **RTOS** based architecture with multiple concurrent tasks
- **System Monitoring** task displaying uptime and heap usage

---

## 📁 Project Structure
├── sketch.ino # Main logic with FreeRTOS tasks <br>
├── main.h # Data structures and typedefs <br>
├── conf.h # Configuration: pins, ThingSpeak keys, I2C addresses <br>
├── wifi_thingspeak.cpp # Wi-Fi setup and ThingSpeak pushing logic <br>
├── wifi_thingspeak.h <br>
├── oled.cpp # OLED wrapper class (I2C) <br>
├── oled.h <br>
├── mpu6050.cpp # MPU6050 class for reading accelerometer/gyro <br>
├── mpu6050.h <br>
├── ntc.cpp # NTC temperature sensor class <br>
├── ntc.h <br>
├── hcsr04.cpp # Distance sensor detection class <br>
├── hcsr04.h <br>
├── buzzer.cpp # Buzzer class <br>
├── buzzer.h <br>
├── wokwi-project.txt # Contains wokwi url to run the project <br>
└── README.md # This file


---

## 🧪 Simulated Hardware on Wokwi

This project is fully tested and simulated using [Wokwi](https://wokwi.com/):

- ✅ ESP32 Dev Module  
- ✅ MPU6050 (I2C)  
- ✅ NTC
- ✅ HCSR04 Sensor  
- ✅ Buzzer  
- ✅ SSD1306 OLED Display (I2C)
- ✅ Wi-Fi 

---

## 🚀 How it Works

### 1. **Task: `check_distance`**
- Reads the distance sensor state every 500ms.
- Updates the shared `distance` object.

### 2. **Task: `get_sensor_data`**
- Reads MPU6050 data every second.
- Pushes the readings to ThingSpeak.

### 3. **Task: `system_monitor`**
- Prints system uptime and free heap memory every 5 seconds.

### 4. **Task: `fall_detect`**
- Waits for sensor data from the queue.
- Calculates acceleration and rotation magnitude.
- If above threshold, detects a fall and updates status.


---

## 📡 Data Sent to ThingSpeak

The `push_data()` function sends the following fields to your ThingSpeak channel:
- Body temperature
- Accelerometer data
- Rotation data
- Distance
- Fall status

Make sure to update your `THINGSPEAK_CHANNEL_NUMBER` and `THINGSPEAK_API_KEY` in `conf.h`.

---

## 🔧 Setup Instructions

1. Clone this repo or load the project into [Wokwi](https://wokwi.com/).
2. Configure the following in `conf.h`:
   - I2C pins and addresses
   - Input/output pin for sensors using GPIO
   - ThingSpeak credentials
3. Run the simulation.
4. Monitor the serial output and ThingSpeak dashboard for data updates.

---

## 📷 Demo
Check out the [demo video](./fall%20detection%20demo.mp4) to view the features the deivce.