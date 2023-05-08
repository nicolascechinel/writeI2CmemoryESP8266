
#ifndef __DELAY__
#define __DELAY__

#include "esp_system.h"

#define delay_us(A) ets_delay_us(2*A)

#define delay_ms(A) vTaskDelay(A/portTICK_RATE_MS)
    

#endif