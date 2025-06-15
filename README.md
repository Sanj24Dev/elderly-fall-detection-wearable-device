# Fall Detection and Health Monitoring System (FreeRTOS-based)

This project implements a real-time, multitasking fall detection and health monitoring system using ESP32, developed and tested on the [Wokwi simulator](https://wokwi.com/). The system collects data from motion, temperature, and acceleration sensors, processes this data in real-time using FreeRTOS, and pushes relevant readings to ThingSpeak.

## 🧠 Features

- **Fall Detection** using acceleration magnitude (MPU6050)
- **Body Monitoring** (Temperature & Humidity via DHT22)
- **Motion Detection** (PIR sensor)
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
├── dht22.cpp # DHT22 temperature and humidity sensor class <br>
├── dht22.h <br>
├── pir.cpp # PIR sensor state detection class <br>
├── pir.h <br>
├── wokwi-project.txt # Contains wokwi url to run the project <br>
└── README.md # This file


---

## 🧪 Simulated Hardware on Wokwi

This project is fully tested and simulated using [Wokwi](https://wokwi.com/):

- ✅ ESP32 Dev Module  
- ✅ MPU6050 (I2C)  
- ✅ DHT22  
- ✅ PIR Motion Sensor  
- ✅ SSD1306 OLED Display (I2C)
- ✅ Wi-Fi 

---

## 🚀 How it Works

### 1. **Task: `sense_motion`**
- Reads the PIR sensor state every 500ms.
- Updates the shared `motion` object.

### 2. **Task: `get_sensor_data`**
- Reads MPU6050 and DHT22 data every second.
- Pushes the readings to ThingSpeak.
- Displays temperature on OLED.

### 3. **Task: `fall_detect`**
- Waits for sensor data from the queue.
- Calculates acceleration magnitude.
- If above threshold (2.5g), detects a fall and updates status.

### 4. **Task: `system_monitor`**
- Prints system uptime and free heap memory every 5 seconds.

---

## 📡 Data Sent to ThingSpeak

The `push_data()` function sends the following fields to your ThingSpeak channel:
- Body temperature
- Humidity
- Accelerometer data (X, Y, Z)
- Motion status
- Fall status

Make sure to update your `THINGSPEAK_CHANNEL_NUMBER` and `THINGSPEAK_API_KEY` in `conf.h`.

---

## 🔧 Setup Instructions

1. Clone this repo or load the project into [Wokwi](https://wokwi.com/).
2. Configure the following in `conf.h`:
   - I2C pins and addresses
   - DHT22 pin
   - ThingSpeak credentials
3. Run the simulation.
4. Monitor the serial output and ThingSpeak dashboard for data updates.

---

## 📊 Sample Output

<pre>
System monitor:
    Uptime: 00:01:23
    Free space in heap: 20984 bytes
Sensor Data:
    MPU6050: ACC=0.01,0.98,9.80
    DHT22: T=36.50, Humidity=60.20
    PIR: Moving
[FALL DETECTED] Acceleration = 3.15g
</pre>