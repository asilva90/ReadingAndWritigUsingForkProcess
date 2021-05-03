#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Constantes
#define TAM_STRING 50

typedef struct{
  int  cod;
  char titulo[TAM_STRING];
  char autor[TAM_STRING];
  int edicao;
  int ano;
 
}lista;

  FILE *arq, *arqAutor, *arqLivro, *arqEdicao;
  int result;

void CriaArquivo(int codigo
                ,char titulo[]
				,char autor[]
				,int edicao
				,int ano){
	//Testar os registros iguais para não inserir
  	result = fprintf(arqAutor,"Autor: %s\n", autor);  					  
    if (result == EOF)		    
	printf("Erro na Gravacao do arquivo dos autores a\n");
	
	result = fprintf(arqLivro,"Titulo: %s Ano: %d\n", titulo, ano);  					  
    if (result == EOF)		    
	printf("Erro na Gravacao do arquivo dos livros a\n");
	
	result = fprintf(arqEdicao,"Ano: %d\n", ano);  					  
    if (result == EOF)		    
	printf("Erro na Gravacao do arquivo das edições a\n");
}


main()
{
  lista mylista[8];
  int i=0;
  char autorOld;
 
  
  arq = fopen("teste.txt","r");            //Abre o arquivo existente e o lê
  arqAutor = fopen("autor.txt", "w");     //Cria um novo arquivo com os autores dos livros e a quantidade de livros publicados
  arqLivro = fopen("livro.txt", "w");    //Cria um novo arqvuivo com os livros publicados e seu ano de publicação
  arqEdicao = fopen("edicao.txt", "w"); //Cria um novo arquivo com o numero de edições publicada e o ano
 
    while(!feof(arq)){
     
      fscanf(arq,"%d;%[^;]s ",&mylista[i].cod, mylista[i].titulo);
      fscanf(arq, ";%[^;]s", mylista[i].autor);
      fscanf(arq,";%d",&mylista[i].edicao);
      fscanf(arq, ";%d", &mylista[i].ano);
      
      printf("codigo: %d titulo: %s autor: %s edicao: %d ano: %d\n",mylista[i].cod, mylista[i].titulo, mylista[i].autor, mylista[i].edicao, mylista[i].ano);
      
      if (arq == NULL) // Se nào conseguiu criar
      {
      	printf("Problemas na CRIACAO do arquivo\n");
        return 0;
  	  }
  	  
  	  //Cria os arquivos: livro.txt, autor.txt e edicao.txt
  	  CriaArquivo(mylista[i].cod
		         ,mylista[i].titulo
				 ,mylista[i].autor
				 ,mylista[i].edicao
				 ,mylista[i].ano);
  	  
  	  
  	  	
	}
			
    fclose(arqAutor);
    fclose(arqLivro);
    fclose(arq);
    
}
