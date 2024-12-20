#include <stdio.h>
#include <stdlib.h>

#define MAX_ESTUDANTES 100
#define MAX_FICHAS 10
#define MAX_EXERCICIOS 100
#define MAX_SUBMISSOES 10000

//|||||||||||||||||||||||||||||||||||||||||||
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// AS FUNÇÕES NAO PODEM TER MAIS DE 25 LINHAS (exepto main)
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//|||||||||||||||||||||||||||||||||||||||||||

//-------------------- Estruturas de Dados --------------------//
typedef struct {
    int id;
    int numero;
    char nome[30];
    char email[30];
} Estudante;
typedef struct {
    int id;
    char nome[30];
    int numEx;
    Data dataPubl;
} Ficha;
typedef struct {
    int id;
    int fichaID;
    char nome[30];
    char difuculdade[10]; // "baixo", "médio" ou "elevado" // MESMA MERDA QUE O DIURNO E POS LABORAL, CRIAR UMA MERDA QUE DEPOIS VAI COPIAR STRING
    char tipoSol[10]; // "algoritmo", "código" // MEMA MERDA DE CIMA (NOJENTO)
} Exercicio;
typedef struct {
    int id;
    int estudanteID;
    int fichaiD;
    int exercID;
    Data dataSub;
    int linhasSol;
    int nota; //[0,100]
} Submissao;

typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

int mainMenu() {

    char opcao;
    system("cls");
    printf("\n--- Menu Principal ---\n");
    printf("1. Gestão de Estudantes\n");
    printf("2. Gestão de Exercícios\n");
    printf("3. Gestão de Fichas\n");
    printf("4. Gestão de Submissões\n");
    printf("5. Estatísticas\n");
    printf("6. Guardar Dados\n");
    printf("7. Carregar Dados\n");
    printf("8. Sair\n");
    printf("Escolha uma opção: ");
    return scanf(" %c", &opcao);
}

void main() {

    int opcao;

    do
    {
        opcao = mainMenu();
        switch (opcao)
        {
        case 1:
            gestaoEstudantes();
            break;
        case 2:
            gestaoExercicios();
            break;
        case 3:
            gestaoFichas();
            break;
        case 4:
            gestaoSubmissoes();
            break;
        case 5:
            estatisticas();
            break;
        case 6:
            saveData();
            break;
        case 7:
            loadData();
            break;
        case 8:
            // Sair
            break;
        default:
            printf("Opção inválida\n");
        }

    } while (opcao < 1 || opcao > 8);
}