#include "mcpwm_gen.h"



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