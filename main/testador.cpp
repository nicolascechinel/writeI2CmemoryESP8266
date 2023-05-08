
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "digital.h"
#include "delay.h"
#include "i2c.h"
#include <inttypes.h> 
#include "serial.h"

#define DEVICE 0x50

I2C i2c =  I2C();
volatile uint32_t x;
struct contato
{
	char nome[20];
	char telefone[14];
	char endereco[30];
};


extern "C" void app_main() ;

uint8_t a;

void testa_se_esta_pronto(void)
{
	uint8_t a;
	while (1)
	{
		i2c.start();
		a = i2c.write(DEVICE << 1);
		if (a==0) break;
		i2c.stop();
		delay_ms(10);
	}
	i2c.stop();
}
void fazEscrita (uint16_t endereco, uint8_t valor)
{
	testa_se_esta_pronto();
	i2c.start();
	i2c.write(DEVICE << 1);	 
	i2c.write(endereco >> 8);
	i2c.write(endereco);
	i2c.write(valor);
	i2c.stop();
}


uint8_t fazLeitura  (uint16_t endereco)
{
	uint8_t a;
	testa_se_esta_pronto();
	i2c.start();
	i2c.write(DEVICE << 1); // operacao fake
	i2c.write(endereco >> 8);
	i2c.write(endereco);
	i2c.start(); // agora sim eh leitura
	i2c.write( (DEVICE << 1) | 1);
	a = i2c.read();
	i2c.stop();
	return a;
}




void salvaString(char * st, uint16_t endereco_base)
{
	uint8_t x=0;
	while (st[x] != 0)
	{
		fazEscrita(endereco_base+x,st[x]);
		x++;
	}
	for (uint8_t a=x;a<endereco_base;a++){
		fazEscrita(endereco_base+a,' ');
	}
}




void app_main()
{
    contato contato;
	printf("Entre com o nome:\n");
	serial.readString((uint8_t*) contato.nome,20);
	printf("Entre com o telefone:\n");
	serial.readString((uint8_t*) contato.telefone, 14);
	printf("Entre com o endereco:\n");
	serial.readString((uint8_t*) contato.endereco, 30);
	uint8_t lido=0;
	
	i2c.configura(PIN16,PIN5);
	salvaString(contato.nome,200);
	salvaString(contato.telefone,220);
	salvaString(contato.endereco,234);
	//salvaString((char *)"Engenharia de ComputacaoAHDHSADHASHDAHSDHASHDAHSD",200);
	for (int a=0;a<20;a++)
	{
		printf("%c",fazLeitura(200+a));
	}
	for (int a=0;a<14;a++)
	{
		printf("%c",fazLeitura(220+a));
	}
	for (int a=0;a<30;a++)
	{
		printf("%c",fazLeitura(234+a));
	}
	printf("\n");
    printf("FIM\n");
}
