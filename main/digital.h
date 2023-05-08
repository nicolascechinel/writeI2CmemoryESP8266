
#ifndef __DIGITAL__
#define __DIGITAL__

#include "driver/gpio.h"
#include  <stdint.h>

#define LOW 0
#define HIGH 1

#define INPUT 1
#define OUTPUT 2
#define PULLUPP  4

#define PIN1 GPIO_NUM_1
#define PIN2 GPIO_NUM_2
#define PIN3 GPIO_NUM_3
#define PIN4 GPIO_NUM_4
#define PIN5 GPIO_NUM_5
#define PIN6 GPIO_NUM_6
#define PIN7 GPIO_NUM_7
#define PIN8 GPIO_NUM_8
#define PIN9 GPIO_NUM_9
#define PIN10 GPIO_NUM_10
#define PIN11 GPIO_NUM_11
#define PIN12 GPIO_NUM_12
#define PIN13 GPIO_NUM_13
#define PIN14 GPIO_NUM_14
#define PIN15 GPIO_NUM_15
#define PIN16 GPIO_NUM_16




class DIGITAL {

    public:
        void pinMode   (gpio_num_t pino, int modo);
        void digitalWrite (gpio_num_t pino, uint32_t level);
        int digitalRead (gpio_num_t pino);
};

extern DIGITAL digital;
#endif
