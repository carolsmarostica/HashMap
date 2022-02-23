//Trabalho10
//Caio de Assis Ribeiro e Carolina Silva Marostica

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

FILE *principal;
FILE *indice;

typedef struct inserir{ //struct para a funcao carrega_arquivos
	char CodCli[3];
	char CodF[3];
	char NomeCli[50];
	char NomeF[50];
	char Genero[50];
}TIPO1;
typedef struct buscar{ //struct para a funcao carrega_arquivos
	char CodCli[3];
	char CodF[3];
}TIPO2;

TIPO1 add[30];
TIPO2 busca[30];
TIPO2 remocao[30];

//MENU
typedef enum m{ //organizar o menu
	inserir=1, remover, buscar, carregar, sair=0 
}MENU;

void carrega_arquivos(){
    FILE *arq;
    int i=0;
	//carrega vetor de struct add
	    arq = fopen("Arquivos//insere.bin", "r+b");//abre o arquivo insere
	    rewind(arq);//volta ao comeco do arquivo por garantia
	    //salva os registros de insercao em um vetor auxiliar add
	    while(!feof(arq)){
		    fread(&add[i], sizeof(TIPO1), 1, arq);
            i++;
	    }
	    fclose(arq);//fecha o arquivo insere
    //carrega vetor de struct busca
	    arq = fopen("Arquivos//busca.bin", "r+b");//abre o arquivo busca
	    rewind(arq);//volta ao comeco do arquivo por garantia
	    //salva os registros de busca em um vetor auxiliar busca
	    i=0;
        while(!feof(arq)){
		    fread(&busca[i], sizeof(TIPO2), 1, arq);
            i++;
        }
        fclose(arq);//fecha o arquivo insere
    //carrega vetor de struct remocao
	    arq = fopen("Arquivos//remove.bin", "r+b");//abre o arquivo busca
	    rewind(arq);//volta ao comeco do arquivo por garantia
	    //salva os registros de busca em um vetor auxiliar busca
	    i=0;
        while(!feof(arq)){
		    fread(&remocao[i], sizeof(TIPO2), 1, arq);
            i++;
        }
        fclose(arq);
	printf("----Upload realizado----\n");
	return;
}
void inicializa(){
    int i=0, aux=0;
    //INICIALIZACAO DO ARQUIVO PRINCIPAL
    if(access("principal.bin", F_OK)!=0){ //se o arquivo nao existe
        principal = fopen("principal.bin", "w+b"); //cria o arquivo dos registros
        fwrite(&i,sizeof(int),1,principal); //indice de insercoes ja realizadas
        fwrite("#",sizeof(char),1,principal);
        fwrite(&i,sizeof(int),1,principal); //indice de remocoes ja realizadas
        fwrite("#",sizeof(char),1,principal);
        fwrite(&i,sizeof(int),1,principal); //indice de buscas ja realizadas
        fwrite("#",sizeof(char),1,principal);
        i=19;
        fwrite(&i,sizeof(int),1,principal); //distancia do header
    }else{ //se o arquivo existe
        principal = fopen("principal.bin", "r+b"); //le o arquivo dos registros
    }

    //INICIALIZACAO DO ARQUIVO DE HASH
    if(access("indice.bin", F_OK)!=0){ //se o arquivo nao existe
        indice = fopen("indice.bin", "w+b"); //cria o arquivo de indice
        for(i=0;i<27;i++){
            fwrite(&aux, sizeof(int),1, indice);
        }
    }else{ //se o arquivo existe
        indice = fopen("indice.bin", "r+b"); //le o arquivo dos registros
    }

}
int endereco(int chave){
    int end=0;
    end = chave % 13;
    return end;
}
void escrever(){
    int i;
     int ind_add, distanciap_origem;

    //coleta dos indices
    fseek(principal, 0, 0);
    fread(&ind_add, sizeof(int),1, principal);
    fseek(principal, 15, 0);
    fread(&distanciap_origem, sizeof(int), 1, principal);
    //escrita no arquivo principal
	fseek(principal,0, SEEK_END);
    fwrite(&add[ind_add].CodCli, sizeof(char),3,principal);
    fwrite(&add[ind_add].CodF, sizeof(char),3,principal);
    fwrite(&add[ind_add].NomeCli, sizeof(char),50,principal);
    fwrite(&add[ind_add].NomeF, sizeof(char),50,principal);
    fwrite(&add[ind_add].Genero, sizeof(char),50,principal);

    fwrite(&distanciap_origem,sizeof(int),1,indice);
                
    //atualizacao do header do arquivo principal
    ind_add++;
    fseek(principal, 0, 0);
    fwrite(&ind_add, sizeof(int), 1, principal);
    distanciap_origem=distanciap_origem+156;
    fseek(principal,15,0);    
    fwrite(&distanciap_origem, sizeof(int), 1, principal);
    
   return;
}
void atualiza(int a){
    int ind=0;
    fseek(indice,-4, SEEK_END);
    fread(&ind, sizeof(int),1,indice);    
    if(a==1){
        ind++;
    }else if (a==2){
        ind--;
    }
    fseek(indice,-4, SEEK_END);
    fwrite(&ind, sizeof(int),1,indice);
    return;
}
int pega_chave(){
    int ind_add, chave;

    fseek(principal, 0 , 0);
    fread(&ind_add, sizeof(int), 1, principal);

    //variavel auxiliar
    char codf[3], codc[3];
    strcpy(codc,add[ind_add].CodCli);
    strcpy(codf,add[ind_add].CodF); 

    //concatena
    strcat(codc,codf);

    chave=atoi(codc);

    return chave;
}
void insere_registro(){
    int tentativa=0,
    end,
    chave,
    i, ind_add, h,e;
    
    chave=pega_chave();
    end=endereco(chave);
    for(i=0; i<13; i++){
        fseek(indice,8*i,0);//valor
        fread(&h,sizeof(int), 1, indice);//lendo chaves
        if(h==chave){
            printf("Chave %d duplicada\n", chave);
            fseek(principal, 0 , 0);
            fread(&ind_add, sizeof(int), 1, principal);
            ind_add++;
            fseek(principal, 0, 0);
            fwrite(&ind_add, sizeof(int), 1, principal);
            return;
        }
    }
    //valor|bof|valor|bof|valor|bof
    fseek(indice,end*8,0);//bof
    fread(&e,sizeof(int), 1, indice);//lendo posicao
    if(e==0){
        fseek(indice,-4,SEEK_CUR);
        fwrite(&chave,sizeof(int),1,indice);//posicao
        printf("Endereco %d\n", end);
        printf("Chave %d inserida com sucesso\n", chave);
        escrever();
        return;
    }else{
        printf("Endereco %d\n", end);
        printf("Colisao\n");
        fseek(indice,4,SEEK_CUR);
        for(i=end;i<13;i++){
            tentativa++; 
            fread(&e,sizeof(int), 1, indice);//lendo posicao
            if(e==0){
                fseek(indice,-4,SEEK_CUR);
                fwrite(&chave,sizeof(int),1,indice);//posicao
                printf("Tentativa %d\n", tentativa);
                printf("Chave %d inserida com sucesso\n", chave);
                escrever();
                return;
            }
            fseek(indice,4,SEEK_CUR);
        }
        rewind(indice);
        for(i=0; i<end;i++){
            tentativa++; 
            fread(&e,sizeof(int), 1, indice);//lendo posicao
            if(e==0){
                fseek(indice,-4,SEEK_CUR);
                fwrite(&chave,sizeof(int),1,indice);//posicao
                printf("Tentativa %d\n", tentativa);
                printf("Chave %d inserida com sucesso\n", chave);
                escrever();
                return;
            }
            fseek(indice,4,SEEK_CUR);
        }
    }
    printf("Nao foi possivel inserir\n");    
    return;
}
void remove_registro(){
    int ind_remove, chave, end, i, h;

    fseek(principal, 5 , 0);
    fread(&ind_remove, sizeof(int), 1, principal);

    //variavel auxiliar
    char codf[3], codc[3];
    strcpy(codc,remocao[ind_remove].CodCli);
    strcpy(codf,remocao[ind_remove].CodF); 

    //char em int
    int aux2=atoi(codf),
    aux1=atoi(codc);

    chave = (100*aux1) + aux2;
    end=endereco(chave);

    for(i=0; i<13; i++){
        fseek(indice,8*i,0);//valor
        fread(&h,sizeof(int), 1, indice);//lendo chaves
        if(h==chave){
            printf("Chave %d removida\n", chave);
            fseek(indice, -4 , SEEK_CUR);
            aux1=0;
            fwrite(&aux1, sizeof(int),1,indice);
            fwrite(&aux1, sizeof(int),1,indice);
            ind_remove++;
            fseek(principal, 5, 0);
            fwrite(&ind_remove, sizeof(int), 1, principal);
            return;        
        }
    }
    printf("Chave %d não encontrada para remocao\n", chave);
    ind_remove++;
    fseek(principal, 5, 0);
    fwrite(&ind_remove, sizeof(int), 1, principal);
    return; 
}
void print(int end){
    TIPO1 registro;
    fseek(principal, end, 0);
    fread(registro.CodCli, sizeof(char),3,principal);
    printf("Codigo Cliente:%s\n", registro.CodCli);
    fread(registro.CodF, sizeof(char),3,principal);
    printf("Codigo Filme:%s\n", registro.CodF);
    fread(registro.NomeCli, sizeof(char),50,principal);
    printf("Nome Cliente:%s\n", registro.NomeCli);
    fread(registro.NomeF, sizeof(char),50,principal);
    printf("Nome Filme:%s\n", registro.NomeF);
    fread(registro.Genero, sizeof(char),50,principal);
    printf("Genero:%s\n", registro.Genero);

    return;
}
void busca_registro(){
    int ind_busca, chave, end, e, i, tentativa=1, bof;
    char registro[156];
    fseek(principal, 10 , 0);
    fread(&ind_busca, sizeof(int), 1, principal);

    //variavel auxiliar
    char codf[3], codc[3];
    strcpy(codc,busca[ind_busca].CodCli);
    strcpy(codf,busca[ind_busca].CodF); 

    //char em int
    int aux2=atoi(codf),
    aux1=atoi(codc);

    chave = (100*aux1) + aux2;
    end=endereco(chave);
    fseek(indice,end*8,0);//bof
    fread(&e,sizeof(int), 1, indice);
    if(e==chave){
        printf("Chave %d encontrada, endereco %d, %d acesso\n", chave, end, tentativa);
        fread(&bof, sizeof(int),1,indice);
        print(bof);
        fseek(principal, 10 , 0);
        ind_busca++;
        fwrite(&ind_busca, sizeof(int), 1, principal);
        return;
    }else{
        fseek(indice,4,SEEK_CUR);
        for(i=end;i<13;i++){
            tentativa++; 
            fread(&e,sizeof(int), 1, indice);//lendo posicao
            if(e==chave){
                printf("Chave %d encontrada, endereco %d, %d acessos\n", chave, end, tentativa);
                fread(&bof, sizeof(int),1,indice);
                print(bof);
                fseek(principal, 10 , 0);
                ind_busca++;
                fwrite(&ind_busca, sizeof(int), 1, principal);
                return;
            }
            fseek(indice,4,SEEK_CUR);
        }
        rewind(indice);
        for(i=0; i<end;i++){
            tentativa++; 
            fread(&e,sizeof(int), 1, indice);//lendo posicao
            if(e==chave){
                printf("Chave %d encontrada, endereco %d, %d acessos\n", chave, end, tentativa);
                fread(&bof, sizeof(int),1,indice);
                print(bof);
                fseek(principal, 10 , 0);
                ind_busca++;
                fwrite(&ind_busca, sizeof(int), 1, principal);
                return;
            }
            fseek(indice,4,SEEK_CUR);
        }
    }
    printf("Chave nao encontrada\n");   
    fseek(principal, 10 , 0);
    ind_busca++;
    fwrite(&ind_busca, sizeof(int), 1, principal);
}
int main(){
    int 
    op; //menu
    inicializa();
    do{
        printf("\n1-Inserir\n2-Remover\n3-Buscar\n4-Carrega Arquivos\n0-Sair\n");
        scanf("%d", &op);
        switch(op){
        case inserir:
            insere_registro();
            atualiza(1);
            break;
        case remover:
            remove_registro();
            atualiza(2);
            break;
        case buscar:
            busca_registro();
            atualiza(0);
            break;
        case carregar:
            carrega_arquivos();
            break;
        default: //sair
            break;
        }
    }while(op!=0);

    return 0;
}