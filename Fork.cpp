#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h> 

#define KEY 123  
#define TAMANHO 10
#define INDICE 5000
//********************************************************************************************************************
   pid_t pid;
   clock_t t;
   char linhaAtual[95], maiorAno[6];
   char *token;
   const char s[2] = ";";
   int shmidAutores, shmid,c, k=0, j=0,contadorLivrosTotal=0, contadorAnosTotal=0, maiorNumeroEdicoes=0, novoItem;
   key_t key = KEY;
   FILE *pFile, *pWrite;  
  
   //Estrutura dos dados do arquivo original
   struct livro{
   	char codigo [8];
   	char titulo [47];
   	char autor[32];
   	char edicao[3];
   	char ano[6];
   };
	//Estrutura dos dados dos autores
   struct autores{
   	char nome[34];
   	int quantidade;
   };
	
	//Estrutura dos anos de publicacao
   struct anos{
   	char year [6];
   };
	
	//Estrutura dos livros publicados
   struct publicados{
		char tituloPublicado [47];
      struct anos vetorAnos[100]; 
      int contadorAnos;
   };
	
	//Estrutura dos anos e numero de edicao dos livros
   struct edicao{
   	char ano[6];
   	int numeroEdicoes;
   };
  

    //***************************************************************************SEPARACAO DOS DADOS***************************************************************//
    struct livro* separaLivros(){
		 //struct livro vetorLivros;
		 struct livro* vetorLivros;
		 vetorLivros = malloc (TAMANHO * sizeof (struct livro)); //alocar memoria para um vetor de livros
		 //vetorLivros = malloc (sizeof (struct livro)); 
		
		//Le o arquivo original
		 FILE *pFile;
		 pFile = fopen ("livros2.txt", "r" );

		while (k< TAMANHO){
			fgets(linhaAtual,95, pFile);   	
			strcpy(linhaAtual,strtok(linhaAtual, "\n"));
			token = strtok(linhaAtual, s); 
		
			//colocar o c�digo do livro no vetor	
			strcpy(vetorLivros[k].codigo, token );
			token = strtok (NULL, s);
			
			//T	itulo
			strcpy(vetorLivros[k].titulo, token );
			token = strtok(NULL, s);
		
			//colocar autor
			strcpy(vetorLivros[k].autor, token );
			token = strtok (NULL, s);

		
			//Edicao
			strcpy(vetorLivros[k].edicao, token );
			token = strtok (NULL, s);
		
			//Ano
			strcpy(vetorLivros[k].ano, token );
			token = strtok (NULL, s);		 
		k=k+1;
		j++;	
		}
   
   return vetorLivros;
   }
  //*************************************************************************FIM SEPARACAO**********************************************************************//

  
  //******************************************************************SEPARACAO DOS AUTORES********************************************************************//
	struct autores* separaAutores(struct livro *vetorLivrosRecebido, struct autores *vetorAutoresSHM, int indiceEntrada, int indiceSaida){
		struct livro *vetorLivros = vetorLivrosRecebido;
		struct autores *vetorAutores = vetorAutoresSHM;
			
		for (k=indiceEntrada; k<indiceSaida; k++){
			vetorAutores[k].quantidade = 0;
		}
			 
		novoItem = indiceEntrada;
		for (k=indiceEntrada; k<indiceSaida;k++){
			
			for (j = indiceEntrada; j<k; j++){
			
				if ( strcmp(vetorAutores[j].nome, vetorLivros[k].autor) == 0 ) {
					vetorAutores[j].quantidade= vetorAutores[j].quantidade +1;
					j = k+1;
					}    
				}
				
			   if (k==j){
				strcpy(vetorAutores[novoItem].nome, vetorLivros[k].autor);
				vetorAutores[novoItem].quantidade = vetorAutores[novoItem].quantidade+1 ;
				novoItem++;
			  }
			}
			return vetorAutores;
   }
   //***************************************************************FIM SEPARACAO AUTORES*************************************************************************//
         
         
   //**************************************************************SEPARACAO POR NOME E ANO**********************************************************************//
   void filtraLivros(struct livro *vetorLivrosRecebido){
		struct livro *vetorLivros = vetorLivrosRecebido;
      struct  publicados *livrosPublicados;
      shmid = shmget(key, TAMANHO * sizeof(struct livro), IPC_CREAT | 0600 );
      livrosPublicados = shmat(shmid, 0, 0); 
      pWrite = fopen ("livrosPublicados.txt", "w"); 
      int flag = 0;
      
		for (k=0; k<TAMANHO; k++){
         livrosPublicados[k].contadorAnos = 0;
      }
       
      novoItem = 0;
	   
		for (k=0; k<TAMANHO;k++){
	   
	   	for (j = 0; j<k; j++){
	   	
	     		if ( strcmp(livrosPublicados[j].tituloPublicado, vetorLivros[k].titulo) == 0 ) {
	     		break;
	     		}    
	   	}
	   	
		     if (k==j){ 
		     	strcpy(livrosPublicados[novoItem].tituloPublicado, vetorLivros[k].titulo);
		     	strcpy(livrosPublicados[novoItem].vetorAnos[0].year, vetorLivros[k].ano); 
		     	
				 livrosPublicados[novoItem].contadorAnos++;
				 contadorLivrosTotal++;
		     	 novoItem++;
		     }
		     else{ 
		    	 for (c=0; c < livrosPublicados[j].contadorAnos; c++) {
		    	 flag = 0;
		    	 	if (strcmp (vetorLivros[k].ano, livrosPublicados[j].vetorAnos[c].year) == 0) {
		    	 	flag++;
		    	 	}
		    	 	
		    	 }
		    		if (flag == 0){ 
						strcpy(livrosPublicados[j].vetorAnos[ livrosPublicados[j].contadorAnos ].year, vetorLivros[k].ano); //concatena strings 
		    		    livrosPublicados[j].contadorAnos++; 
					}
		     }
	   }
	         
		for (k = 0; k < TAMANHO; k++){
			fprintf (pWrite, "Titulo: %s\n", livrosPublicados[k].tituloPublicado);
		   fprintf (pWrite, "Publicaoes por ano:\n");
			
			for(c=0; c < livrosPublicados[k].contadorAnos ; c++){
			  fprintf (pWrite, "%s, ", livrosPublicados[k].vetorAnos[c].year );
			  }
		   fprintf (pWrite, "\n---------------------------------------------------------------------------\n\n");
		}
		shmdt(livrosPublicados); //libera mem�ria compartilhada
		shmctl(shmid, IPC_RMID, NULL);

   }
   //*******************************************************FIM SEPARACAO POR NOME E ANO***********************************************************************//


   //*******************************************************SEPARACAO DAS EDICOES POR ANO*********************************************************************//
	void separaEdicoes(struct livro *vetorLivrosRecebido){
		struct livro *vetorLivros = vetorLivrosRecebido;
		 
		pWrite = fopen ("edicoes.txt", "w"); 
		struct  edicao *vetorEdicoes;
		shmid = shmget(key, TAMANHO * sizeof(struct livro), IPC_CREAT | 0700 );
		vetorEdicoes = shmat(shmid, 0, 0); 
			 
		for (k=0; k<TAMANHO; k++){
			vetorEdicoes[k].numeroEdicoes = 0;
		}
		
		novoItem = 0;
		for (k=0; k<TAMANHO;k++){
			
			for (j = 0; j<k; j++){
				
				if ( strcmp(vetorEdicoes[j].ano, vetorLivros[k].ano) == 0 ) {
					vetorEdicoes[j].numeroEdicoes= vetorEdicoes[j].numeroEdicoes + 1;
					j = k+1;
				}    
			}
				
		   if (k==j){
				strcpy(vetorEdicoes[novoItem].ano, vetorLivros[k].ano);
				vetorEdicoes[novoItem].numeroEdicoes = vetorEdicoes[novoItem].numeroEdicoes + 1 ;
				contadorAnosTotal++;
				novoItem++;
		   }
		}
					
		for (k = 0; k < TAMANHO; k++){
			fprintf (pWrite, "Ano: %s        Edicao: %d\n", vetorEdicoes[k].ano, vetorEdicoes[k].numeroEdicoes );
		}

		printf ("Total de Livros: %d\n", contadorLivrosTotal); 

		for (k=0; k < TAMANHO; k++){
			if ( vetorEdicoes[k].numeroEdicoes > maiorNumeroEdicoes){
			strcpy (maiorAno, vetorEdicoes[k].ano);
			maiorNumeroEdicoes = vetorEdicoes[k].numeroEdicoes;
			}
		}
		printf ("Total de Anos Publicados: %d\n Maior Ano: %s\n", contadorAnosTotal, maiorAno); 
		shmdt(vetorEdicoes);
		shmctl(shmid, IPC_RMID, NULL);   
	}
	//**************************************************FIM DA SEPARACAO DAS EDICOES POR ANO***********************************************************************//

	void main () {
		struct livro *passarVetorLivros = separaLivros();
		  
		struct autores *vetorAutoresSHM;
		shmidAutores = shmget(key, TAMANHO * sizeof(struct livro), IPC_CREAT | 0700 );
		vetorAutoresSHM = shmat(shmidAutores, 0, 0); 
		
		if ( ( shmid = shmget (key, TAMANHO * sizeof(struct livro), IPC_CREAT | 0700)) < 0 )
			printf("Falha na Memoria Compartilhada\n");

		 
		   separaAutores(passarVetorLivros, vetorAutoresSHM, 0, 1000);
		   pid = fork();
		   if (pid > 0){
				pid = fork();
			
				if (pid > 0){
					pid = fork();
					if (pid >0){
						separaAutores(passarVetorLivros,vetorAutoresSHM, 1001, 2000);   
						wait(NULL);
						wait(NULL);
						wait(NULL);
						filtraLivros (passarVetorLivros);
						separaEdicoes(passarVetorLivros);
								
					}
					else separaAutores(passarVetorLivros,vetorAutoresSHM, 2001, 3000);			     		
				}
				else separaAutores(passarVetorLivros,vetorAutoresSHM, 3001, 5000);
			}
			else {
				pWrite = fopen ("autoresFork.txt", "w");
			
				for (k = 0; k < TAMANHO; k++){
					 fprintf (pWrite, "Autor: %s           Quantidade: %d\n", vetorAutoresSHM[k].nome, vetorAutoresSHM[k].quantidade );
				}
		   }
		  shmctl(shmidAutores, IPC_RMID, NULL);   

	}
