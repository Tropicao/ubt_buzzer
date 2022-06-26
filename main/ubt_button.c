#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "common.h"

#define TAG "BUTTON"
#define BUTTON_GPIO 14
#define BUTTON_PIN_MASK 1 << BUTTON_GPIO

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    xTaskNotifyFromISR(xAppTask, 1 << NOTIFICATION_BUTTON_EVENT, eSetBits, NULL);
}

void ubt_button_start()
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    ESP_LOGI(TAG, "Configuring button GPIO");
    gpio_config_t config = {
        .pin_bit_mask = BUTTON_PIN_MASK,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE};

    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, NULL));
    ESP_LOGI(TAG, "Button GPIO configured");
}