#include "main.h"

#include "wifi_thingspeak.h"
#include "oled.h"
#include "mpu6050.h"
#include "dht22.h"
#include "pir.h"

MPU6050_Gyro gyro(Wire);
OLED display;
DHT22 temp;
PIR motion;

SemaphoreHandle_t data_mutex;
QueueHandle_t sensor_data_queue;
patient_status curr_stat = SAFE;


// Function used for debug
void display_sensor_data(sensor data) {
  Serial.printf("Sensor Data:\n");
  Serial.printf("\tMPU6050:  ACC=%.2f,%.2f,%.2f\n", data.accX, data.accY, data.accZ);
  Serial.printf("\tDHT22:    T=%.2f, Humidity=%.2f\n", data.temp, data.humidity);
  if (data.motion == true)
    Serial.printf("\tPIR:      Moving\n");
  else
    Serial.printf("\tPIR:      Stationary\n");
}

// Task 1
void sense_motion (void *params) {
  while (1) {
    if (xSemaphoreTake(data_mutex, portMAX_DELAY)) {
      motion.read_PIR();
      xSemaphoreGive(data_mutex);
    }
    // Print stack usage
    // UBaseType_t watermark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.printf("[Motion Task] Stack high watermark: %u words\n", watermark);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// Task 2: Gets sensor data every 10.5s
void get_sensor_data(void *params) {
  sensor local_data;

  while (1) {
    gyro.read_MPU6050(&local_data);
    temp.read_DHT22(&local_data);
    if (xSemaphoreTake(data_mutex, portMAX_DELAY)) {
      local_data.motion = motion.pirState;
      xSemaphoreGive(data_mutex);
    }
    local_data.status = curr_stat;
    
    String str = "Body temp: " + String(local_data.temp) + "C";
    display.print_message(str.c_str());
    xQueueSend(sensor_data_queue, &local_data, 0);
    push_data(local_data, THINGSPEAK_CHANNEL_NUMBER, THINGSPEAK_API_KEY); // has a 10s delay for thingspeak limit of sampling rate

    // Print stack usage
    // UBaseType_t watermark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.printf("[Sensor Task] Stack high watermark: %u words\n", watermark);
    vTaskDelay(pdMS_TO_TICKS(500));  // Sample every 1s
  }
}


// Task 3: Prints uptime + heap stats every 10s
void system_monitor (void *params) {
  while (1) {
    uint32_t uptime_sec = millis() / 1000;
    size_t freeHeap = esp_get_free_heap_size();
    Serial.printf("System monitor:\n");
    Serial.printf("\tUptime: %02d:%02d:%02d\n", uptime_sec / 3600, (uptime_sec / 60) % 60, uptime_sec % 60);
    Serial.printf("\tFree space in heap: %d bytes\n", freeHeap);
    // Print stack usage
    // UBaseType_t watermark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.printf("[System Task] Stack high watermark: %u words\n", watermark);
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void fall_detect(void *params) {
  sensor received_data;
  while (1) {
    // Wait until new data is available
    if (xQueueReceive(sensor_data_queue, &received_data, portMAX_DELAY)) {
      float acc_mag = sqrt(
                        received_data.accX * received_data.accX +
                        received_data.accY * received_data.accY +
                        received_data.accZ * received_data.accZ
                      );

      if (acc_mag > 2.5) { // Adjust based on testing
        Serial.printf("[FALL DETECTED] Acceleration = %.2fg\n", acc_mag);
        display.print_message("Fall");
        curr_stat = FALL;
      }
    }

    // Print stack usage
    // UBaseType_t watermark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.printf("[Detect Task] Stack high watermark: %u words\n", watermark);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}



void setup() {
  Serial.begin(115200);
  delay(100);

  data_mutex = xSemaphoreCreateMutex();
  if (data_mutex == NULL) {
    Serial.println("Failed to create mutex!");
    while (1); // Stop everything if mutex failed
  }

  sensor_data_queue = xQueueCreate(5, sizeof(sensor));
  if (sensor_data_queue == NULL) {
    Serial.println("Failed to create sensor data queue!");
    while (1); // Stop if queue creation fails
  }


  // I2C setup
  Wire.begin();

  delay(200);
  gyro.setup_MPU6050(I2C_MPU6050);
  delay(200);
  display.setup_OLED(I2C_OLED);
  delay(200);
  temp.setup_DHT22(DHT_PIN);
  delay(200);
  motion.setup_PIR();

  // Wi-Fi setup
  delay(200);
  setup_WiFi();

  // Create tasks
  BaseType_t result;
  result = xTaskCreate(get_sensor_data, "Sensor Data Acquisition", 4096, NULL, 2, NULL);
  if (result != pdPASS) {
    Serial.println("Failed to create Sensor data acquisition task!");
  }

  result = xTaskCreate(sense_motion, "Sense motion", 2048, NULL, 1, NULL);
  if (result != pdPASS) {
    Serial.println("Failed to create System Monitor task!");
  }

  result = xTaskCreate(system_monitor, "System Monitor", 2048, NULL, 1, NULL);
  if (result != pdPASS) {
    Serial.println("Failed to create System Monitor task!");
  }

  result = xTaskCreate(fall_detect, "Fall Detection", 2048, NULL, 3, NULL);
  if (result != pdPASS) {
    Serial.println("Failed to create Fall Detection task!");
  }


}

void loop() {
  // Idle - FreeRTOS tasks handle the logic
}
