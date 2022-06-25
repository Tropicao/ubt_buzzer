#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"
#include "ubt_button.h"
#include "ubt_network.h"
#include "common.h"

#define TAG "APP"
TaskHandle_t xAppTask = NULL;

/**
 * @brief Main task in charge of managing buzzer behaviour
 * The main task listen on its task notifications with the following bits :
 * See common.h for notifications that task is listening to
 * @param pvParameters
 */
static void main_task(void *pvParameters)
{
    uint32_t u32Notification = 0;
    while (!ubt_network_ready())
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    while (true)
    {
        if (xTaskNotifyWait(0, ULONG_MAX, &u32Notification, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGD(TAG, "New notification (%d)", u32Notification);
        }
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, &xAppTask);
    ubt_network_start();
    ubt_button_start();
}
