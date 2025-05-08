#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_event.h"
#include "ubt_button.h"
#include "ubt_network.h"
#include "common.h"
#include "cJSON.h"
#include "esp_mac.h"

#define TAG "APP"
TaskHandle_t xAppTask = NULL;

static void _send_button_pushed_message(void)
{
    uint8_t u8Address[6] = {0};
    char strAddress[13] = {0};
    char *pcMessage = NULL;
    esp_read_mac(u8Address, ESP_MAC_WIFI_STA);
    snprintf(strAddress, 13, "%02x%02x%02x%02x%02x%02x",
             u8Address[0], u8Address[1], u8Address[2], u8Address[3], u8Address[4], u8Address[5]);
    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "type", "buzz");
    cJSON_AddStringToObject(message, "id", strAddress);
    pcMessage = cJSON_Print(message);
    cJSON_free(message);
    ubt_network_sendmessage(pcMessage);
    free(pcMessage);
}

static void _send_identify_message(void)
{
    uint8_t u8Address[6] = {0};
    char strAddress[13] = {0};
    char *pcMessage = NULL;
    esp_read_mac(u8Address, ESP_MAC_WIFI_STA);
    snprintf(strAddress, 13, "%02x%02x%02x%02x%02x%02x",
             u8Address[0], u8Address[1], u8Address[2], u8Address[3], u8Address[4], u8Address[5]);
    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "type", "identification");
    cJSON_AddStringToObject(message, "id", strAddress);
    pcMessage = cJSON_Print(message);
    cJSON_free(message);
    ubt_network_sendmessage(pcMessage);
    free(pcMessage);
}

/**
 * @brief Main task in charge of managing buzzer behaviour
 * The main task listen on its task notifications with the following bits :
 * See common.h for notifications that task is listening to
 * @param pvParameters
 */
static void main_task(void *pvParameters)
{
    uint32_t u32Notification = 0;
    ESP_LOGI(TAG, "Buzzer ready");
    while (true)
    {
        if (xTaskNotifyWait(0, ULONG_MAX, &u32Notification, portMAX_DELAY) == pdFALSE)
        {
            continue;
        }
        ESP_LOGD(TAG, "New notification (%"PRIu32")", u32Notification);
        if (u32Notification & (1 << NOTIFICATION_BUTTON_EVENT))
        {
            ESP_LOGI(TAG, "Button pushed, send notification to server");
            _send_button_pushed_message();
        }
        else if (u32Notification & (1 << NOTIFICATION_NETWORK_UP))
        {
            ESP_LOGI(TAG, "Connected to server, send identification");
            _send_identify_message();
        }
    }
}

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    xTaskCreate(&main_task, "main_task", 4096, NULL, 5, &xAppTask);
    ubt_network_start();
    ubt_button_start();
}
