#include "memoria.cpp"

void removeContatoPorTelefone()
{
	printf("Entre com o telefone:\n"); // pede o telefone
	char telefone[14]; // telefone
	serial.readString((uint8_t*) &telefone, 14); // le o telefone
	printf("telefone = %s\n",telefone); // imprime o telefone

	bool hasEnded = false; // variavel temporaria
	for(uint8_t i=0;i<14;i++) // salva os bytes
	{
		if (telefone[i] == 0 || telefone[i] == '\n') hasEnded = true; // se chegou ao fim, para
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


void app_main()
{
	i2c.configura(PIN16,PIN5); // configura o i2c
	header cabecalho; // cabecalho
	cabecalho = leCabecalho(); // le o cabecalho
	printf("qtdRegistros = %d\n",cabecalho.qtdRegistros); // imprime a quantidade de registros
	printf("qtdMAX = %d\n",cabecalho.qtdMAX); // imprime a quantidade maxima de registrospesquisa registro por nome
	
	
	while(1){
		//menu da lista telefonica
		printf("0 - adicionar\n"); // opcao 0
		printf("1 - listar\n"); // opcao 1
		printf("2 - pesquisar por nome\n"); // opcao 2
		printf("3 - pesquisar por telefone\n"); // opcao 3
		printf("4 - remove registro pelo telefone\n"); // opcao 4
		printf("5 - mostra quantidade de registros\n"); // opcao 5
		printf("6 - inicializa o banco de dados(todas as informacoes serao perdidas)\n"); // opcao 6
		printf("7 - sair\n"); // opcao 7
		printf("Entre com a opcao:\n"); // pede a opcao
		int opcao; // opcao
		serial.readString((uint8_t*) &opcao, 1); // le a opcao
		printf("opcao = %c\n",opcao); // imprime a opcao

		
		switch (opcao){
			case '0':
				adicionaUmRegistro();
				break;

			case '1':
				listaRegistros();
				break;

			case '2':
				pesquisaContatoPorNome();
				break;
			
			case '3':
				pesquisaContatoPorTelefone();
				break;

			case '4':
				removeContatoPorTelefone();
				break;

			case '5':
				cabecalho = leCabecalho(); // le o cabecalho
				printf("qtdRegistros = %d\n",cabecalho.qtdRegistros); // imprime a quantidade de registros
				printf("qtdMAX = %d\n",cabecalho.qtdMAX); // imprime a quantidade maxima de registros
				break;

			case '6':
				inicializaBancoDeDados(200);
				break;

			case '7':
				printf("\n"); 
				printf("FIM\n");
				return;
				break;

			default:
				printf("opcao invalida %c\n", opcao);
				break;
		}
	}
	
 
}
