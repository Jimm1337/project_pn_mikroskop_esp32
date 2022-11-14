#include <cstdio>
#include <concepts>

#include "freertos/freeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

extern "C" void app_main() {
    char *taskName = pcTaskGetName(nullptr);
    ESP_LOGI(taskName, "The task name is: %s", taskName);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

