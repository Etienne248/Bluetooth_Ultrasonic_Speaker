/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"
#include "esp_pm.h"
//#include "esp_clk.h"
#include "esp_timer.h"

static const char *TAG = "example";
const int division = 100;

volatile int x = 0;
volatile int angle = 1010*division;
volatile int step = 1;

// Please consult the datasheet of your servo before changing the following parameters
#define SERVO_MIN_PULSEWIDTH_US 0    // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2000 // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE 0           // Minimum angle
#define SERVO_MAX_DEGREE 2000*division        // Maximum angle

#define SERVO_PULSE_GPIO 18                   // GPIO connects to the PWM signal line
#define SERVO_TIMEBASE_RESOLUTION_HZ 80000000 // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD 2000            // 20000 ticks, 20ms

static inline uint32_t example_angle_to_compare(int angle)
{
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

bool test_on_empty(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *comparator)
{
    x++;
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, example_angle_to_compare(angle)));
    if ((angle + step) > 2000*division || (angle + step) < 1000*division)
    {
        step *= -1;
    }
    angle += step;
    return true;
}

void mcpwm_init(){
    esp_pm_configure(ESP_PM_CPU_FREQ_MAX);

    ESP_LOGI(TAG, "Create timer and operator");
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
        .period_ticks = SERVO_TIMEBASE_PERIOD,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t operator_config = {
        .group_id = 0, // operator must be in the same group to the timer
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));

    ESP_LOGI(TAG, "Connect timer and operator");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    ESP_LOGI(TAG, "Create comparator and generator from the operator");
    mcpwm_cmpr_handle_t comparator = NULL;
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

    mcpwm_gen_handle_t generator = NULL;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = 18,
        .flags.invert_pwm = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &generator_config, &generator));

    mcpwm_gen_handle_t generatorB = NULL;
    mcpwm_generator_config_t generatorB_config = {
        .gen_gpio_num = 19,
        .flags.invert_pwm = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &generatorB_config, &generatorB));

    // set the initial compare value, so that the servo will spin to the center position
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, example_angle_to_compare(0)));

    ESP_LOGI(TAG, "Set generator action on timer and compare event");
    // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_timer_event(generator,
                                                               MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                                                               MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    // go low on compare threshold
    ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(generator,
                                                                 MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW),
                                                                 MCPWM_GEN_COMPARE_EVENT_ACTION_END()));

    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 1000,
        .negedge_delay_ticks = 1000,
        //.flags.invert_output = true,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(generator, generatorB, &dead_time_config));

    ESP_LOGI(TAG, "register event callbacks\r\n");
    mcpwm_timer_event_callbacks_t cbs = {
        .on_empty = test_on_empty,
    };
    ESP_ERROR_CHECK(mcpwm_timer_register_event_callbacks(timer, &cbs, comparator));

    ESP_LOGI(TAG, "Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
};


void app_main(void)
{
    mcpwm_init();
    while (1)
    {
        ESP_LOGI(TAG, "Angle of rotation: %d", angle);
        // Add delay, since it takes time for servo to rotate, usually 200ms/60degree rotation under 5V power supply
        // vTaskDelay(pdMS_TO_TICKS(1));
        //ESP_LOGI(TAG, "X=: %d", x);
        printf("x = %d\n",x);
        x=0;
        //printf(esp_clk_cpu_freq());
        vTaskDelay(100);
    }
}