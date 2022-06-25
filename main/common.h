#pragma once
#include "freertos/task.h"

extern TaskHandle_t xAppTask;

/* Mapping for application notifications */
typedef enum {
    NOTIFICATION_NETWORK_UP = 0,
    NOTIFICATION_NETWORK_DOWN = 1,
    NOTIFICATION_BUTTON_EVENT = 2,
} eNotificationBit_t;