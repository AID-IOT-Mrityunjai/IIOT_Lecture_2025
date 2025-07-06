#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static const uint8_t QUEUE_LENGTH    = 5;
static const char*   messages[QUEUE_LENGTH] = {
  "Hello world",
  "Good Morning",
  "Welcome",
  "FreeRTOS",
  "Good Bye"
};

// Queue holds pointers to constant strings
static QueueHandle_t xQueue = NULL;

static void SenderTask(void* pvParameters) {
  for (uint8_t i = 0; i < QUEUE_LENGTH; ++i) {
    const char* msg = messages[i];
    if (xQueueSend(xQueue, &msg, portMAX_DELAY) == pdPASS) {
      Serial.printf("Sender: queued \"%s\"\n", msg);
    } else {
      Serial.printf("Sender: FAILED to queue \"%s\"\n", msg);
    }
  }
  Serial.println("Sender: done, deleting self");
  vTaskDelete(NULL);
}

static void ReceiverTask(void* pvParameters) {
  for (uint8_t i = 0; i < QUEUE_LENGTH; ++i) {
    const char* recvd;
    if (xQueueReceive(xQueue, &recvd, portMAX_DELAY) == pdPASS) {
      Serial.printf("Receiver: got \"%s\"\n", recvd);
    } else {
      Serial.println("Receiver: no data");
    }
  }
  Serial.println("Receiver: done, deleting self");
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { /* wait for Serial */ }

  // Create a queue that can hold 5 pointers to char
  xQueue = xQueueCreate(QUEUE_LENGTH, sizeof(const char*));
  if (xQueue == NULL) {
    Serial.println("ERROR: queue creation failed");
    while (1);  // halt
  }

  // Create tasks: sender has higher priority so it fills queue first
  xTaskCreate(SenderTask,   "Sender",   2048, NULL, 2, NULL);
  xTaskCreate(ReceiverTask, "Receiver", 2048, NULL, 1, NULL);

  // No vTaskStartScheduler() needed on Arduino + FreeRTOS—
  // the scheduler is already running.
}

void loop() {
  // Nothing here—tasks do all the work.
}
