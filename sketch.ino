#include "main.h"
#include <stdio.h>

#include "wifi_thingspeak.h"
#include "oled.h"
#include "mpu6050.h"
#include "ntc.h"
#include "hcsr04.h"
#include "buzzer.h"


/*
  Further updates: 
    update add gps so that you dont alarm anyone else
    add OTA
    add power optimization
*/

MPU6050_Gyro gyro(Wire);
OLED display;
NTC temp;
HCSR04 distance;
BUZZER alert;

SemaphoreHandle_t data_mutex;
QueueHandle_t sensor_data_queue;
patient_status curr_stat = SAFE;

// Debug utility
void display_sensor_data(sensor data) {
  Serial.printf("Sensor Data:\n");
  Serial.printf("\tMPU6050:  ACC=%.2f,%.2f,%.2f\n", data.accX, data.accY, data.accZ);
  Serial.printf("\tNTC:      T=%.2f\n", data.temp);
}

// Task 1: Update distance sensor
void check_distance (void *params) {
  while (1) {
    if (xSemaphoreTake(data_mutex, portMAX_DELAY)) {
      distance.read_HCSR04();
      xSemaphoreGive(data_mutex);
    }
    // Print stack usage
    // UBaseType_t watermark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.printf("[Motion Task] Stack high watermark: %u words\n", watermark);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// Task 2: Sample and push sensor data
void get_sensor_data(void *params) {
  sensor local_data;

  while (1) {
    gyro.read_MPU6050(&local_data);
    temp.read_NTC(&local_data);

    if (xSemaphoreTake(data_mutex, portMAX_DELAY)) {
      local_data.distance = distance.distance;
      local_data.status = curr_stat;
      xSemaphoreGive(data_mutex);
    }

    // String str = "Body temp: " + String(local_data.temp) + " ℃\nDistance: " + String(local_data.distance) + " cm";
    // display.print_message(str.c_str());
    xQueueSend(sensor_data_queue, &local_data, 0);
    push_data(local_data, THINGSPEAK_CHANNEL_NUMBER, THINGSPEAK_API_KEY); // has a 10s delay for thingspeak limit of sampling rate

    // Print stack usage
    // UBaseType_t watermark = uxTaskGetStackHighWaterMark(NULL);
    // Serial.printf("[Sensor Task] Stack high watermark: %u words\n", watermark);
    vTaskDelay(pdMS_TO_TICKS(500));  // Sample every 0.5s
  }
}


// Task 3: Monitor system uptime and heap
void system_monitor (void *params) {
  while (1) {
    uint32_t uptime_sec = xTaskGetTickCount() / 1000;
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
  TickType_t last_fall_time = 0, fall_confirm_time = 0;
  int fall_phase = 0;

  while (1) {
    // Wait until new data is available
    if (xQueueReceive(sensor_data_queue, &received_data, portMAX_DELAY)) {
      float acc = sqrt(
                    received_data.accX * received_data.accX +
                    received_data.accY * received_data.accY +
                    received_data.accZ * received_data.accZ
                  );
      float rot = sqrt(
                    received_data.rotX * received_data.rotX +
                    received_data.rotY * received_data.rotY +
                    received_data.rotZ * received_data.rotZ
                  );

      // Phase 1: Detect high acceleration and rotation
      if (acc > 2.5 && rot > 100) { 
        last_fall_time = xTaskGetTickCount();
        fall_phase = 1;
      }

      // Phase 2: Detects if the person is still within 10s of fall = FALL SUSPECTED
      if (fall_phase == 1) {
        TickType_t now = xTaskGetTickCount();
        if ((now - last_fall_time) < pdMS_TO_TICKS(10000)) {
          if (acc < 2 && rot < 50) {
            fall_phase = 2;
            fall_confirm_time = xTaskGetTickCount();
          }
        }
        else {
          fall_phase = 0;
          curr_stat = SAFE; 
        }
      }
      // Phase 3: detects if the person is still and critically close to the ground = FALL DETECTED
      else if (fall_phase == 2) {
        if (acc < 2 && rot < 50 && received_data.distance < 10) {
          if (curr_stat!=FALL && xTaskGetTickCount() - fall_confirm_time > 10000) { // Wait 5s of continued stillness
            curr_stat = FALL;
            fall_phase = 3;
          }
        } else {
          fall_phase = 0; // Movement resumed before confirmation
        }
      }
      // Revert: if the watch is aftleast 4ft above the ground, the person has been assisted
      else if (fall_phase == 3) {
        if (received_data.distance > 120) {
          fall_phase = 0;
          curr_stat = SAFE;
        }
      }

      if (curr_stat == FALL) {
        display.print_message("FALL DETECTED");
        alert.output_buzzer();
      } else if (fall_phase == 1 || fall_phase == 2) {
        display.print_message("FALL SUSPECTED");
      } else {
        char msg[64];  // adjust size as needed
        snprintf(msg, sizeof(msg), "NORMAL\nBody temp: %.2f", received_data.temp);
        display.print_message("NORMAL");
      }
      Serial.printf("[DEBUG] Acc:%.2f Rot:%.2f Dist:%.2f phase:%d\n", acc, rot, received_data.distance, fall_phase);
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
  // delay(200);
  // temp.setup_NTC(NTC_PIN);
  delay(200);
  distance.setup_HCSR04();
  delay(200);
  alert.setup_buzzer();

  // Wi-Fi setup
  delay(200);
  setup_WiFi();

  // Create tasks
  BaseType_t result;
  result = xTaskCreate(get_sensor_data, "Sensor Data Acquisition", 4096, NULL, 2, NULL);
  if (result != pdPASS) {
    Serial.println("Failed to create Sensor data acquisition task!");
  }

  result = xTaskCreate(check_distance, "Check distance", 2048, NULL, 1, NULL);
  if (result != pdPASS) {
    Serial.println("Failed to create System Monitor task!");
  }

  result = xTaskCreate(system_monitor, "System Monitor", 2048, NULL, 1, NULL);
  if (result != pdPASS) {
    Serial.println("Failed to create System Monitor task!");
  }

  result = xTaskCreate(fall_detect, "Fall Detection", 4096, NULL, 3, NULL);
  if (result != pdPASS) {
    Serial.println("Failed to create Fall Detection task!");
  }


}

void loop() {
  // Idle - FreeRTOS tasks handle the logic
}
