
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
struct contato // 64 bytes
{
	char nome[20]; // 20 bytes
	char telefone[14]; // 14 bytes
	char endereco[30]; // 30 bytes
};

struct header  // 4 bytes
{
    uint16_t qtdRegistros; // 2 bytes
    uint16_t qtdMAX; // 2 bytes
};

extern "C" void app_main() ;

uint8_t a;

void testa_se_esta_pronto(void) // espera ate que o dispositivo esteja pronto
{
	uint8_t a;
	while (1)
	{
		i2c.start(); // tenta comecar
		a = i2c.write(DEVICE << 1); // tenta escrever
		if (a==0) break; // se conseguiu, sai do loop
		i2c.stop(); // se nao conseguiu, para e tenta de novo
		delay_ms(10); // espera 10 ms
	}
	i2c.stop(); // se conseguiu, para e sai
}
void fazEscrita (uint16_t endereco, uint8_t valor) // escreve um byte no endereco
{
	testa_se_esta_pronto(); // espera ate que o dispositivo esteja pronto
	i2c.start(); // comeca
	i2c.write(DEVICE << 1);	// operacao fake
	i2c.write(endereco >> 8); // escreve o endereco do registrador
	i2c.write(endereco); // escreve o endereco do registrador
	i2c.write(valor); // escreve o valor
	i2c.stop(); // para
}

uint8_t fazLeitura  (uint16_t endereco) // le um byte no endereco
{
	uint8_t a; // variavel temporaria
	testa_se_esta_pronto(); // espera ate que o dispositivo esteja pronto
	i2c.start(); // comeca
	i2c.write(DEVICE << 1); // operacao fake
	i2c.write(endereco >> 8); // escreve o endereco do registrador
	i2c.write(endereco); // escreve o endereco do registrador
	i2c.start(); // agora sim eh leitura
	i2c.write( (DEVICE << 1) | 1); // operacao de leitura
	a = i2c.read(); // le o valor
	i2c.stop(); // para
	return a; // retorna o valor
}

header leCabecalho () // le o cabecalho
{
	header R; // variavel temporaria
	for (int a=0;a<4;a++) // le os 4 bytes
	{
		((uint8_t *) &R)[a] = fazLeitura(a); // le o byte
	}
	return R; // retorna o cabecalho
}

void salvaCabecalho (header R) // salva o cabecalho
{
	for (int a=0;a<4;a++) // salva os 4 bytes
	{
		fazEscrita(a,((uint8_t *) &R)[a]); // salva o byte
	}
	return; // retorna
}






void salvaString(char * st, uint16_t endereco_base, uint8_t tamanho) // salva uma string
{
	uint8_t x=0; // variavel temporaria
	bool hasEnded = false; // variavel temporaria
	for(uint8_t i=0;i<tamanho;i++) // salva os bytes
	{
		if (st[i] == 0 || st[i] == '\n') hasEnded = true; // se chegou ao fim, para
		if (hasEnded) {fazEscrita(endereco_base+i,' ');continue;} // se chegou ao fim, preenche com espacos
		fazEscrita(endereco_base+i,st[i]); // salva o byte
	}
}




void app_main()
{
	i2c.configura(PIN16,PIN5); // configura o i2c
	header cabecalho; // cabecalho
	
	cabecalho = leCabecalho(); // le o cabecalho
	printf("qtdRegistros = %d\n",cabecalho.qtdRegistros); // imprime a quantidade de registros
	printf("qtdMAX = %d\n",cabecalho.qtdMAX); // imprime a quantidade maxima de registros
	
	if(cabecalho.qtdRegistros > 0) // se tem registros
	{
		for (size_t i = 0; i < cabecalho.qtdRegistros; i++) // para cada registro
		{
			printf("Registro %d\n",i); // imprime o numero do registro
			printf("nome\n"); // imprime o nome
			for (int a=0;a<20;a++) // para cada byte do nome
			{
				printf("%c",fazLeitura(4+a+i*64)); // imprime o byte
			}
			printf("telefone\n"); // imprime o telefone
			for (int a=0;a<14;a++) // para cada byte do telefone
			{
				printf("%c", fazLeitura(20+4+a+i*64)); // imprime o byte
			}
			{
				printf("%c", fazLeitura(24+a+i*64)); // imprime o byte
			}
			printf("endereco\n"); // imprime o endereco
			for (int a=0;a<30;a++) // para cada byte do endereco
			{
				printf("%c",fazLeitura(34+4+a+i*64)); // imprime o byte
			}
			{
				printf("%c",fazLeitura(38+a+i*64)); // imprime o byte
			}
		}
		
	}
	
	contato contato; // contato
	printf("Entre com o nome:\n"); // pede o nome
	serial.readString((uint8_t*) contato.nome,20); // le o nome
	printf("Entre com o telefone:\n"); // pede o telefone
	serial.readString((uint8_t*) contato.telefone, 14); // le o telefone
	printf("Entre com o endereco:\n"); // pede o endereco
	serial.readString((uint8_t*) contato.endereco, 30); // le o endereco

	salvaString(contato.nome, 4 + cabecalho.qtdRegistros*64, 20); // salva o nome
	salvaString(contato.telefone,20 + 4 + cabecalho.qtdRegistros*64, 14); // salva o telefone
	salvaString(contato.endereco,34 + 4 + cabecalho.qtdRegistros*64, 30); // salva o endereco
	
	printf("nome\n"); // imprime o nome
	for (int a=0;a<20;a++) // para cada byte do nome
	{
		printf("%c",fazLeitura(4+a+cabecalho.qtdRegistros*64)); // imprime o byte
	}
	printf("telefone\n"); // imprime o telefone
	for (int a=0;a<14;a++) // para cada byte do telefone
	{
		printf("%c",fazLeitura(20+4+a+cabecalho.qtdRegistros*64)); // imprime o byte
	}
	printf("endereco\n"); // imprime o endereco
	for (int a=0;a<30;a++) // para cada byte do endereco
	{
		printf("%c",fazLeitura(34+4+a+cabecalho.qtdRegistros*64)); // imprime o byte
	}
	cabecalho.qtdRegistros++; // incrementa a quantidade de registros
	// cabecalho.qtdMAX = 100; 
	salvaCabecalho(cabecalho); // salva o cabecalho

	cabecalho = leCabecalho(); // le o cabecalho
	printf("qtdRegistros = %d\n",cabecalho.qtdRegistros); // imprime a quantidade de registros
	printf("qtdMAX = %d\n",cabecalho.qtdMAX); // imprime a quantidade maxima de registros

	printf("\n"); 
    printf("FIM\n"); 
}
