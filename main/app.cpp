//TRABALHO DE SISTEMAS DIGITAIS EMbarcados
//Alunos: Nicolas Cechinel Rosa e Augusto Scarduelli Prudencio 





#include "memoria.cpp"


void app_main()
{
	i2c.configura(PIN16,PIN5); // configura o i2c
	header cabecalho; // cabecalho
	cabecalho = leCabecalho(); // le o cabecalho
	printf("qtdRegistros = %d\n",cabecalho.qtdRegistros); // imprime a quantidade de registros
	printf("qtdMAX = %d\n",cabecalho.qtdMAX); // imprime a quantidade maxima de registrospesquisa registro por nome
	
	
	while(1){
		//menu da lista telefonica
		printf("\n0 - adicionar\n"); // opcao 0
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
