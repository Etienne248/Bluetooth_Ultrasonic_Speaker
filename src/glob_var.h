#ifndef GLOB_VAR_H
#define GLOB_VAR_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/ringbuf.h"

extern RingbufHandle_t ringbuf_pwm;

#endif