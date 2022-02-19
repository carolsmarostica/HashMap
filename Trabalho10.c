//Trabalho10
//Caio de Assis Ribeiro e Carolina Silva Marostica


#include<stdio.h>

int endereco(int chave, int n){
    int soma=0, j=0;
    while(j<12){ //12 caracteres fixos 
        soma= [soma + (100*chave[j]) + chave[j+1]] % 19937;
        //maximo valor da dobra das chaves minuto 26:50
        // zz | zz
        //90+90 | 90+90
        //9090 | 9090
        // 18180 -> numero primo 19937
        j=j+2; 
    }
    return soma%n;
}

int main(){
    int n=13, //numero de chaves
    op, //menu

    endereço(&chave, )

    return 0;
}
