#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "BUTTON"
#define BUTTON_GPIO GPIO_NUM_14
#define BUTTON_PIN_MASK (1 << BUTTON_GPIO)
#define DEBOUNCE_DELAY_MS (100 / portTICK_PERIOD_MS)

static TaskHandle_t _task = NULL;
static bool _bButtonPushed = false;

static void IRAM_ATTR gpio_isr_handler(void *arg) {
  gpio_intr_disable(BUTTON_GPIO);
  xTaskNotifyGive(_task);
}

static void _ButtonTask(void *pvArg) {
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(!_bButtonPushed)
        {
            xTaskNotify(xAppTask, 1 << NOTIFICATION_BUTTON_EVENT, eSetBits);
        }
        // Debounce
        vTaskDelay(DEBOUNCE_DELAY_MS);
        _bButtonPushed = gpio_get_level(BUTTON_GPIO) == 0 ? true : false;
        gpio_intr_enable(BUTTON_GPIO);
    }
  }

void ubt_button_start() {
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  ESP_LOGI(TAG, "Configuring button");
  xTaskCreate(&_ButtonTask, "button_task", 4096, NULL, 6, &_task);
  gpio_config_t config = {.pin_bit_mask = BUTTON_PIN_MASK,
                          .mode = GPIO_MODE_INPUT,
                          .pull_up_en = GPIO_PULLUP_ENABLE,
                          .intr_type = GPIO_INTR_NEGEDGE};

  ESP_ERROR_CHECK(gpio_config(&config));
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, NULL));
  ESP_LOGI(TAG, "Button GPIO");
}