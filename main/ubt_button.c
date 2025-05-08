#include "common.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#define TAG "BUTTON"
#define NUM(x) x
#define CONCAT(x,y) x ## y
#define GPIO_APPLY(x, y) CONCAT(x,y)
#define BUTTON_GPIO GPIO_APPLY(GPIO_NUM_, CONFIG_UBT_BUTTON_GPIO)
#define BUTTON_PIN_MASK (1 << CONFIG_UBT_BUTTON_GPIO)
#define DEBOUNCE_PERIOD_MS 10

static TaskHandle_t _task = NULL;

static void _ButtonTask(void *pvArg) {
  static uint16_t buttonDebounce = 0;
  while (true) {
    buttonDebounce =
        (buttonDebounce << 1) | gpio_get_level(BUTTON_GPIO) | 0xE000;
    if (buttonDebounce == 0xF000) {
      xTaskNotify(xAppTask, 1 << NOTIFICATION_BUTTON_EVENT, eSetBits);
    }
    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_PERIOD_MS));
  }
  vTaskDelete(NULL);
}

void ubt_button_start() {
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  ESP_LOGI(TAG, "Configuring button");
  gpio_config_t config = {.pin_bit_mask = BUTTON_PIN_MASK,
                          .mode = GPIO_MODE_INPUT,
                          .pull_up_en = GPIO_PULLUP_ENABLE};

  ESP_ERROR_CHECK(gpio_config(&config));
  xTaskCreate(&_ButtonTask, "button_task", 4096, NULL, 6, &_task);
  ESP_LOGI(TAG, "Button GPIO");
}
