
#include "digital.h"

void DIGITAL::pinMode  (gpio_num_t pino, int modo)
{
    if (modo & 1) gpio_set_direction (pino,GPIO_MODE_INPUT);
    if (modo & 2) gpio_set_direction (pino,GPIO_MODE_OUTPUT);
    if (modo & 4) gpio_set_pull_mode (pino, GPIO_PULLUP_ONLY);
}
void DIGITAL::digitalWrite (gpio_num_t pino, uint32_t level)
{
    gpio_set_level(pino, level);
}
int DIGITAL::digitalRead (gpio_num_t pino)
{
    return gpio_get_level(pino);
}

DIGITAL digital = DIGITAL();

