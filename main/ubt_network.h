#pragma once

#include <stdbool.h>

#define MAX_MESSAGE_LENGTH 128

void ubt_network_start(void);
void ubt_network_sendmessage(char *pcMessage);