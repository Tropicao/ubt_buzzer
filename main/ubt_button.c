#include "common.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "sdkconfig.h"

#define TAG "BUTTON"
#define NUM(x) x
#define CONCAT(x,y) x ## y
#define GPIO_APPLY(x, y) CONCAT(x,y)
#define BUTTON_GPIO GPIO_APPLY(GPIO_NUM_, CONFIG_UBT_BUTTON_GPIO)
#define BUTTON_PIN_MASK (1 << CONFIG_UBT_BUTTON_GPIO)
#define BUTTON_DEBOUNCE_DELAY_MS 10

static TaskHandle_t _button_task = NULL;

static void gpio_isr_handler(void *arg);

static esp_err_t configure_button_isr(bool  enable)
{
	return enable ? gpio_isr_handler_add(CONFIG_UBT_BUTTON_GPIO, gpio_isr_handler,
			NULL)
		: gpio_isr_handler_remove(CONFIG_UBT_BUTTON_GPIO);
}

static void _button_task_func(void *pvArg) {
	uint32_t notification;
	int button_state;
	while(true) {
		notification = ulTaskNotifyTake(true, pdMS_TO_TICKS(1000));
		// Timeout, no event occured
		if (!notification)
			continue;

		/* An ISR has triggered: debounce it by waiting a bit, and
		 * then reading the actual value
		 */
		vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_DELAY_MS));
		button_state = gpio_get_level(CONFIG_UBT_BUTTON_GPIO);

		/* If the button is 0 after debounce delay, it is an actual
		 * push: send push notification
		 */
		if (!button_state)
			xTaskNotify(xAppTask, 1 << NOTIFICATION_BUTTON_EVENT, eSetBits);

		/* Whether it is a glitch or an actual button push,
		 * re-enable the button ISR
		 */
		configure_button_isr(true);
	}
}

static void gpio_isr_handler(void *arg)
{
	BaseType_t higher_prio_task_awoken = false;
	/* The ISR could be an actual push, or it could be a glitch
	 * following an actual button movement. While it is not a big deal
	 * if it follows a button push, it is an actual issue if the glitch
	 * follows a button release, so disable ISR to allow debouncing it
	 * (cf button task code)
	 */
	configure_button_isr(false);
	vTaskNotifyGiveFromISR(_button_task, &higher_prio_task_awoken);
}

void ubt_button_start() {
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  ESP_LOGI(TAG, "Configuring button");
  gpio_config_t config = {.pin_bit_mask = BUTTON_PIN_MASK,
                          .intr_type = GPIO_INTR_NEGEDGE,
                          .mode = GPIO_MODE_INPUT,
                          .pull_up_en = GPIO_PULLUP_ENABLE};
  xTaskCreate(&_button_task_func, "button_task", 4096, NULL, 6, &_button_task);
  ESP_ERROR_CHECK(gpio_config(&config));
  gpio_install_isr_service(0);
  configure_button_isr(true);
  ESP_LOGI(TAG, "Button ready");
}
