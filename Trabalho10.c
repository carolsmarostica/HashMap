//Trabalho10
//Caio de Assis Ribeiro e Carolina Silva Marostica

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

FILE *principal;
FILE *indice;

typedef struct tipo1{ //struct para a funcao carrega_arquivos
	char CodCli[3];
	char CodF[3];
	char NomeCli[50];
	char NomeF[50];
	char Genero[50];
}TIPO1;
typedef struct tipo2{ //struct para a funcao carrega_arquivos
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

typedef enum b{ //booleano
	true=1, false=0 
}BOOL;

void carrega_arquivos(){//upload dos dados que devem ser inseridos, buscados e removidos
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
        fclose(arq);//fecha o arquivo busca
    //carrega vetor de struct remocao
	    arq = fopen("Arquivos//remove.bin", "r+b");//abre o arquivo remocao
	    rewind(arq);//volta ao comeco do arquivo por garantia
	    //salva os registros de busca em um vetor auxiliar remocao
	    i=0;
        while(!feof(arq)){
		    fread(&remocao[i], sizeof(TIPO2), 1, arq);
            i++;
        }
        fclose(arq);//fecha o arquivo remocao
	printf("----Upload realizado----\n");
	return;
}
void inicializa(){//abre ou cria os arquivos (principal e indice)
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
            fwrite(&aux, sizeof(int),1, indice);//inicializa todas as posicoes valendo 0
        }
    }else{ //se o arquivo existe
        indice = fopen("indice.bin", "r+b"); //le o arquivo dos registros
    }

}
int endereco(int chave){//pega o endereco em que a chave deve ser inserida
    int end=0;
    end = chave % 13;//metodo da divisao inteira
    return end;
}
void escrever(){//escreve no arquivo principal
    int i, ind_add, distanciap_origem;

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
    //escreve no arquivo de indice a distancia da chave no arquivo principal
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
void atualiza(int a){//funcao que atualiza o numero de registros no arquivo
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
int pega_chave(){//pega a chave que deve ser inserida e concatena
    int ind_add, chave;

    fseek(principal, 0 , 0);
    fread(&ind_add, sizeof(int), 1, principal);
    //variavel auxiliar
    char codf[3], codc[3];
    strcpy(codc,add[ind_add].CodCli);
    strcpy(codf,add[ind_add].CodF); 
    //concatena
    strcat(codc,codf);
    //trabsforma em inteiro
    chave=atoi(codc);

    return chave;
}
void insere_registro(){//funcao para inserir registros no hash e no arquivo principal
    int tentativa=1,end,chave,i, ind_add, h,e, aux=-1, cont=1;
    BOOL achou=false;
    
    //recupera a chave e o endereco original em que ela deve ser escrita
    chave=pega_chave();
    end=endereco(chave);
    
    for(i=end;i<13;i++){
        fseek(indice,8*i,0);
        fread(&h,sizeof(int), 1, indice);//le a chave
        if(h!=0){//pode ser -1 ou outra chave
            cont++;
            if(h==-1 && aux==-1){//pode inserir onde e -1 //aux=-1 quer dizer que ainda nao achou um lugar para a nova chave
                tentativa=cont;//numero de tentativas/colisoes
                aux=i;//atualiza aux com a posicao que a chave deve ser inserida
            }
            if(chave==h){//ja existe a chave
                printf("Chave %d duplicada\n", chave);
                //atualiza o indice de insercoes no header
                fseek(principal, 0 , 0);
                fread(&ind_add, sizeof(int), 1, principal);
                ind_add++;
                fseek(principal, 0, 0);
                fwrite(&ind_add, sizeof(int), 1, principal);
                return;
            }
        }else{
            if(aux==-1){//aux=-1 quer dizer que a chave ainda nao tinha um lugar
                aux=i;
                tentativa=cont;
            }
            achou=true;
            break;
        }
    }
    if(achou!=true){
        for(i=0;i<end;i++){
            fseek(indice,8*i,0);//valor
            fread(&h,sizeof(int), 1, indice);//lendo chaves
            if(h!=0){//pode ser -1 ou outra chave
                cont++;
                if(h==-1 && aux==-1){//pode inserir onde e -1 //aux=-1 quer dizer que ainda nao achou um lugar para a nova chave
                    tentativa=cont;//numero de tentativas/colisoes
                    aux=i;//atualiza aux com a posicao que a chave deve ser inserida
                }
                if(chave==h){//ja existe a chave
                    printf("Chave %d duplicada\n", chave);
                    //atualiza o indice de insercoes no header
                    fseek(principal, 0 , 0);
                    fread(&ind_add, sizeof(int), 1, principal);
                    ind_add++;
                    fseek(principal, 0, 0);
                    fwrite(&ind_add, sizeof(int), 1, principal);
                    return;
                }
            }else{
                if(aux==-1){//aux=-1 quer dizer que a chave ainda nao tinha um lugar
                    aux=i;
                    tentativa=cont;
                }
                break;
            }
        }
    }

    fseek(indice, aux*8, 0);//posiciona onde a chave deve ser inserida no hash
    fwrite(&chave,sizeof(int),1,indice);//escreve a chave
    printf("Endereco %d\n", end);
    if(tentativa!=1){
        printf("Colisao\n");
        printf("Tentativa %d\n", tentativa);
    }
    printf("Chave %d inserida com sucesso\n", chave);
    escrever();//escreve a distancia do header no arquivo principal + o arquivo principal

    return;
}
void remove_registro(){//funcao para remover registros no hash
    int ind_remove, chave, end, i, h, aux=-1;
    BOOL achou=false;

    //recupera a chave que deve ser removida
    fseek(principal, 5 , 0);
    fread(&ind_remove, sizeof(int), 1, principal);
    //variavel auxiliar
    char codf[3], codc[3];
    strcpy(codc,remocao[ind_remove].CodCli);
    strcpy(codf,remocao[ind_remove].CodF); 
    //concatena
    strcat(codc,codf);
    //transforma em inteiro e recupera o endereco que era para ela estar
    chave=atoi(codc);
    end=endereco(chave);

    for(i=end; i<13; i++){
        fseek(indice,8*i,0);//posiciona no endereco
        fread(&h,sizeof(int), 1, indice);//le a chave
        if(h!=0){//confere se ha um limpo verdadeiro    
            if(h==chave){//confere se existe a chave
                printf("Chave %d removida\n", chave);
                fseek(indice, -4 , SEEK_CUR);
                //transforma o endereco em um limpo falso
                aux=-1;
                fwrite(&aux, sizeof(int),1,indice);
                fwrite(&aux, sizeof(int),1,indice);
                //atualiza o indice de removidos no header do arquivo principal
                ind_remove++;
                fseek(principal, 5, 0);
                fwrite(&ind_remove, sizeof(int), 1, principal);
                return;        
            }
        }else{
            achou=true;
            break;
        }
    }
    if(achou!=true){
        rewind(indice);
        for(i=0; i<end; i++){
            fseek(indice,8*i,0);//posiciona no endereco
            fread(&h,sizeof(int), 1, indice);//le a chave
            if(h!=0){//confere se ha um limpo verdadeiro 
                if(h==chave){
                    printf("Chave %d removida\n", chave);
                    fseek(indice, -4 , SEEK_CUR);
                    aux=-1;
                    fwrite(&aux, sizeof(int),1,indice);
                    fwrite(&aux, sizeof(int),1,indice);
                    ind_remove++;
                    fseek(principal, 5, 0);
                    fwrite(&ind_remove, sizeof(int), 1, principal);
                    return;        
                }
            }else{
                break;
            }
        }
    }
    printf("Chave %d não encontrada para remocao\n", chave);
    //atualiza o indice de removidos
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
            if(e!=0){
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
            }else{
                break;
            }
        }
        rewind(indice);
        for(i=0; i<end;i++){
            tentativa++; 
            fread(&e,sizeof(int), 1, indice);//lendo posicao
            if(e!=0){
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
            }else{
                break;
            }
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
        printf("\n1-Inserir\n2-Remover\n3-Buscar\n4-Carrega Arquivos\n0-Sair\nOpcao:");
        scanf("%d", &op);
        switch(op){
        case inserir:
            printf("------------------------\n");
            insere_registro();
            atualiza(1);
            printf("------------------------\n");
            break;
        case remover:
            printf("------------------------\n");
            remove_registro();
            atualiza(2);
            printf("------------------------\n");
            break;
        case buscar:
            printf("------------------------\n");
            busca_registro();
            atualiza(0);
            printf("------------------------\n");
            break;
        case carregar:
            printf("------------------------\n");
            carrega_arquivos();
            printf("------------------------\n");
            break;
        default: //sair
            break;
        }
    }while(op!=0);

    return 0;
}