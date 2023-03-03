#include "mcpwm_gen.h"
#include "bt_main.h"
#include "glob_var.h"


void app_main(void)
{
    mcpwm_init();
    bt_app_main();
    while (1)
    {
        size_t item_size;
        static int16_t * item = NULL;
        item = xRingbufferReceiveUpToFromISR(ringbuf_pwm, &item_size, 40000);
        if (item != NULL){
            for(int i = 0; i<item_size/2;i++)printf("item %d = %d \n",i,item[i]);
            vRingbufferReturnItem(ringbuf_pwm, (void *)item);
        }
        
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