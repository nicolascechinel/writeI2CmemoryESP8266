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
#include <cstring>

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
	bool hasEnded = false; // variavel temporaria
	for(uint8_t i=0;i<tamanho;i++) // salva os bytes
	{
		if (st[i] == '\0' || st[i] == '\n') hasEnded = true; // se chegou ao fim, para
		if (hasEnded) {fazEscrita(endereco_base+i, '\0');continue;} // se chegou ao fim, preenche com espacos
		fazEscrita(endereco_base+i,st[i]); // salva o byte
	}
}

void adicionaUmRegistro(){
	// adiciona um registro
	header cabecalho; // cabecalho
	cabecalho = leCabecalho(); // le o cabecalho

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
	
    printf("\nnome:\n"); // imprime o nome
    for (int a=0;a<20;a++) // para cada byte do nome
    {
        char c = fazLeitura(4+a+cabecalho.qtdRegistros*64);
        if (c == '\0' || c == '\n') break;
        printf("%c", c); // imprime o byte
    }
    printf("\ntelefone:\n"); // imprime o telefone
    for (int a=0;a<14;a++) // para cada byte do telefone
    {
        char c = fazLeitura(24+a+cabecalho.qtdRegistros*64);
        if (c == '\0' || c == '\n') break;
        printf("%c", c); // imprime o byte
    }
    printf("\nendereco:\n"); // imprime o endereco
    for (int a=0;a<30;a++) // para cada byte do endereco
    {
        char c = fazLeitura(38+a+cabecalho.qtdRegistros*64);
        if (c == '\0' || c == '\n') break;
        printf("%c", c); // imprime o byte
    }
    printf("\n-------------------------------------------\n");
	cabecalho.qtdRegistros++; // incrementa a quantidade de registros
	// cabecalho.qtdMAX = 100; 
	salvaCabecalho(cabecalho); // salva o cabecalho

	cabecalho = leCabecalho(); // le o cabecalho
	printf("qtdRegistros = %d\n",cabecalho.qtdRegistros); // imprime a quantidade de registros
	printf("qtdMAX = %d\n",cabecalho.qtdMAX); // imprime a quantidade maxima de registros

}

void listaRegistros(){
	// lista os registros
	header cabecalho; // cabecalho
	cabecalho = leCabecalho(); // le o cabecalho

	if(cabecalho.qtdRegistros > 0) // se tem registros
	{
		for (size_t i = 0; i < cabecalho.qtdRegistros; i++) // para cada registro
		{
			printf("\nRegistro %d\n",i); // imprime o numero do registro
			printf("nome:\n"); // imprime o nome
			for (int a=0;a<20;a++) // para cada byte do nome
			{
                char c = fazLeitura(4+a+i*64);
                if (c == '\0' || c == '\n') break;
				printf("%c", c); // imprime o byte
			}
			printf("\ntelefone:\n"); // imprime o telefone
			for (int a=0;a<14;a++) // para cada byte do telefone
			{
                char c = fazLeitura(24+a+i*64);
                if (c == '\0' || c == '\n') break;
				printf("%c", c); // imprime o byte
			}
			printf("\nendereco:\n"); // imprime o endereco
			for (int a=0;a<30;a++) // para cada byte do endereco
			{
                char c = fazLeitura(38+a+i*64);
                if (c == '\0' || c == '\n') break;
				printf("%c", c); // imprime o byte
			}
            printf("\n-------------------------------------------\n");
		}
	}
	else {printf("Nao ha registros\n");} // se nao tem registros
}

void inicializaBancoDeDados(int qtdMAX){
	header cabecalho; // cabecalho
	cabecalho.qtdRegistros = 0; // quantidade de registros
	cabecalho.qtdMAX = qtdMAX; // quantidade maxima de registros
	salvaCabecalho(cabecalho); // escreve o cabecalho
	printf("Limpo\n"); // imprime limpo
}

void pesquisaContatoPorNome(){
	printf("Entre com o nome:\n"); // pede o nome
	char nome[20]; // nome
	serial.readString((uint8_t*) nome, 20); // le o nome
	printf("nome = %s\n",nome); // imprime o nome

	bool hasEnded = false; // variavel temporaria
	for(uint8_t i=0;i<20;i++) // salva os bytes
	{
		if (nome[i] == '\0' || nome[i] == '\n') hasEnded = true; // se chegou ao fim, para
		if (hasEnded) {nome[i] = '\0';continue;} // se chegou ao fim, preenche com espacos
	}

	// pesquisa o nome
	header cabecalho; // cabecalho
	cabecalho = leCabecalho(); // le o cabecalho

	if(cabecalho.qtdRegistros > 0) // se tem registros
	{
		for (size_t i = 0; i < cabecalho.qtdRegistros; i++) // para cada registro
		{
			char nomeRegistro[20]; // nome do registro
			for (int a=0;a<20;a++) // para cada byte do nome
			{
				nomeRegistro[a] = fazLeitura(4+a+i*64); // le o byte
			}

			if(strcmp(nomeRegistro,nome) == 0) // se o nome do registro e igual ao nome pesquisado
			{
				printf("\nRegistro %d\n",i); // imprime o numero do registro
                printf("nome:\n"); // imprime o nome
                for (int a=0;a<20;a++) // para cada byte do nome
                {
                    char c = fazLeitura(4+a+i*64);
                    if (c == '\0' || c == '\n') break;
                    printf("%c", c); // imprime o byte
                }
                printf("\ntelefone:\n"); // imprime o telefone
                for (int a=0;a<14;a++) // para cada byte do telefone
                {
                    char c = fazLeitura(24+a+i*64);
                    if (c == '\0' || c == '\n') break;
                    printf("%c", c); // imprime o byte
                }
                printf("\nendereco:\n"); // imprime o endereco
                for (int a=0;a<30;a++) // para cada byte do endereco
                {
                    char c = fazLeitura(38+a+i*64);
                    if (c == '\0' || c == '\n') break;
                    printf("%c", c); // imprime o byte
                }
                printf("\n-------------------------------------------\n");
			}
		}
	}
}

void pesquisaContatoPorTelefone(){
	printf("Entre com o telefone:\n"); // pede o telefone
	char telefone[14]; // telefone
	serial.readString((uint8_t*) &telefone, 14); // le o telefone
	printf("telefone = %s\n",telefone); // imprime o telefone

	bool hasEnded = false; // variavel temporaria
	for(uint8_t i=0;i<14;i++) // salva os bytes
	{
		if (telefone[i] == '\0' || telefone[i] == '\n') hasEnded = true; // se chegou ao fim, para
		if (hasEnded) {telefone[i] = '\0';continue;} // se chegou ao fim, preenche com espacos
	}

	header cabecalho; // cabecalho
	cabecalho = leCabecalho(); // le o cabecalho
	if(cabecalho.qtdRegistros > 0) // se tem registros
	{
		for (size_t i = 0; i < cabecalho.qtdRegistros; i++) // para cada registro
		{
			char telefoneRegistro[14]; // telefone do registro
			for (int a=0;a<14;a++) // para cada byte do telefone
			{
				telefoneRegistro[a] = fazLeitura(20+4+a+i*64); // le o byte
			}
			if(strcmp(telefone,telefoneRegistro) == 0) // se o telefone do registro e igual ao telefone
			{
				printf("\nRegistro %d\n",i); // imprime o numero do registro
                printf("nome:\n"); // imprime o nome
                for (int a=0;a<20;a++) // para cada byte do nome
                {
                    char c = fazLeitura(4+a+i*64);
                    if (c == '\0' || c == '\n') break;
                    printf("%c", c); // imprime o byte
                }
                printf("\ntelefone:\n"); // imprime o telefone
                for (int a=0;a<14;a++) // para cada byte do telefone
                {
                    char c = fazLeitura(24+a+i*64);
                    if (c == '\0' || c == '\n') break;
                    printf("%c", c); // imprime o byte
                }
                printf("\nendereco:\n"); // imprime o endereco
                for (int a=0;a<30;a++) // para cada byte do endereco
                {
                    char c = fazLeitura(38+a+i*64);
                    if (c == '\0' || c == '\n') break;
                    printf("%c", c); // imprime o byte
                }
                printf("\n-------------------------------------------\n");
			}
		}
	}
}

void removeContatoPorTelefone()
{
	printf("Entre com o telefone:\n"); // pede o telefone
	char telefone[14]; // telefone
	serial.readString((uint8_t*) &telefone, 14); // le o telefone
	printf("telefone = %s\n",telefone); // imprime o telefone

	bool hasEnded = false; // variavel temporaria
	for(uint8_t i=0;i<14;i++) // salva os bytes
	{
		if (telefone[i] == '\0' || telefone[i] == '\n') hasEnded = true; // se chegou ao fim, para
		if (hasEnded) {telefone[i] = '\0';continue;} // se chegou ao fim, preenche com espacos
	}

	header cabecalho; // cabecalho
	cabecalho = leCabecalho(); // le o cabecalho
	bool hasFound = false; // variavel temporaria
	for(int i=0; i<cabecalho.qtdRegistros; i++)
	{
		if(hasFound) // se achou
		{
			for(int a=0;a<64;a++) // para cada byte do registro
			{
				fazEscrita(4+a+(i-1)*64,fazLeitura(4+a+i*64)); // copia o byte
			}
			cabecalho.qtdRegistros--; // diminui a quantidade de registros
			salvaCabecalho(cabecalho); // escreve o cabecalho
		}
		else{
			char telefoneRegistro[14]; // telefone do registro
			for (int a=0;a<14;a++) // para cada byte do telefone
			{
				telefoneRegistro[a] = fazLeitura(20+4+a+i*64); // le o byte
			}
			if(strcmp(telefone,telefoneRegistro) == 0) {hasFound = true; // achou} // se o telefone do registro e igual ao telefone
		}
	}
}
}
