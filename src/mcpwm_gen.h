 // This header file was generated on
// z5214348.web.cse.unsw.edu.au/header_generator/

// header guard: https://en.wikipedia.org/wiki/Include_guard
// This avoids errors if this file is included multiple times
// in a complex source file setup

#ifndef MCPWM_GEN_H
#define MCPWM_GEN_H

// #includes

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"
#include "esp_pm.h"
//#include "esp_clk.h"
#include "esp_timer.h"

// #defines
#define DIVISION 100

// Please consult the datasheet of your servo before changing the following parameters
#define SERVO_MIN_PULSEWIDTH_US 0    // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2000 // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE 0           // Minimum angle
#define SERVO_MAX_DEGREE 2000*DIVISION        // Maximum angle

#define SERVO_PULSE_GPIO 18                   // GPIO connects to the PWM signal line
#define SERVO_TIMEBASE_RESOLUTION_HZ 80000000 // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD 2000            // 20000 ticks, 20ms


// Global variable 

static const char *TAG = "example";

volatile int x = 0;
volatile int angle = 1010*DIVISION;
volatile int step = 1;


// Functions


/**
 * @brief 
 * @param angle 
 * @return 
 */
static inline uint32_t example_angle_to_compare(int angle);

/**
 * @brief callback function that is execute on timer empty 
 * @param timer 
 * @param edata 
 * @param comparator 
 * @return 
 * @note the folowing code attach the callback:
 * @note \code{.c}
 *   mcpwm_timer_event_callbacks_t cbs = {
 *       .on_empty = change_duty_on_empty,
 *   };
 *   ESP_ERROR_CHECK(mcpwm_timer_register_event_callbacks(timer, &cbs, comparator));
 * \endcode
 */
bool change_duty_on_empty(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *comparator);

/** 
 * @brief start the pwm
 * @param[in] dsdfs sfdfds
 */
void mcpwm_init();

// End of header file
#endif
