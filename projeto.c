#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_ESTUDANTES 100
#define MAX_FICHAS 10
#define MAX_EXERCICIOS 100
#define MAX_SUBMISSOES 10000
FILE *ficheiroEstudantes;
FILE *ficheiroFichas;
FILE *ficheiroExercicios;
FILE *ficheiroSubmissoes;

//|||||||||||||||||||||||||||||||||||||||||||
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// AS FUNÇÕES NAO PODEM TER MAIS DE 25 LINHAS (exepto main)
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//|||||||||||||||||||||||||||||||||||||||||||

//Codigo de teste

/* Estudante estudante = {0, 10, "ahaha", "ahaah"};
    fwrite(&estudante, sizeof(Estudante), 1, ficheiroEstudantes);
    fflush(ficheiroEstudantes); //para garantir que guardou as cenas
    printf("%d\n%d\n%s\n%s\n", estudante.id, estudante.numero, estudante.email, estudante.nome); */

//-------------------- Estruturas de Dados --------------------//
typedef struct {
    int dia;
    int mes;
    int ano;
} Data;
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

//-------------------- Funções --------------------//
void mainFetchData();
int fetchDataEstudantesAndFichas();
int fetchDataExerciciosAndSubmissoes();
void getAndShowCountsOfFiles();

void mainGestaoEstudantes();
int menuGestaoEstudantes();
void registerEstudante();

void gestaoExercicios();
void gestaoFichas();
void gestaoSubmissoes();
void estatisticas();
void saveData();
void loadData();

int mainMenu() {
    char opcao;
    int aux = 9;

    system("cls");
    printf("\n--- Menu Principal ---\n");
    printf("1. Gestao de Estudantes\n");
    printf("2. Gestao de Exercicios\n");
    printf("3. Gestao de Fichas\n");
    printf("4. Gestao de Submissoes\n");
    printf("5. Estatisticas\n");
    printf("6. Guardar Dados\n");
    printf("7. Carregar Dados\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
    scanf(" %c", &opcao);
    if(isdigit(opcao)){
        aux = opcao - '0';
    }
    return aux;
}

void main() {
    Estudante listaEstudantes[MAX_ESTUDANTES];
    int opcao;

    mainFetchData();

    do
    {
        opcao = mainMenu();
        switch (opcao)
        {
        case 1:
            mainGestaoEstudantes();
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
        case 0:
            return ;
            break;
        default:
            printf("Opcao invalida\n");
        }

    } while (opcao != 0);
}

#pragma region Iniciar Ficheiros

void mainFetchData(){
    int aux = fetchDataEstudantesAndFichas();
    int aux2 = fetchDataExerciciosAndSubmissoes();
    if(aux != 1 || aux2 != 1){
        int *countEstudantes = 0, *countFichas = 0, *countExercicios = 0, *countSubmissoes= 0;
        getAndShowCountsOfFiles(countEstudantes, countFichas, countExercicios, countSubmissoes);
        printf("\nFicheiro aberto com sucesso!");
        printf("\n\nEstudantes carregados: %d", countEstudantes);
        printf("\nFichas carregadas: %d", countFichas);
        printf("\nExercicios carregados: %d", countExercicios);
        printf("\nSubmissoes carregadas: %d", countSubmissoes);
        printf("\n\n(Pressione ENTER para continuar) ");
        getchar();
        system("cls");
    }else{
        printf("Erro na abertura/criacao dos ficheiros");
    }
}

int fetchDataEstudantesAndFichas(){
    int aux = 0;
    ficheiroEstudantes = fopen("estudantes.dat", "r+b");
    if (ficheiroEstudantes == NULL) {
        ficheiroEstudantes = fopen("estudantes.dat", "w+b");
        if (ficheiroEstudantes == NULL) {
            printf("Não foi possivel abrir os estudantes!!!!");
            aux = 1;
        }
    }

    ficheiroFichas = fopen("fichas.dat", "r+b");
    if (ficheiroEstudantes == NULL) {
        ficheiroEstudantes = fopen("fichas.dat", "w+b");
        if (ficheiroEstudantes == NULL) {
            printf("Não foi possivel abrir as fichas!!!!");
            aux = 1;
        }
    }

    return aux;
}

int fetchDataExerciciosAndSubmissoes(){
    int aux = 0;
    ficheiroExercicios = fopen("exercicios.dat", "r+b");
    if (ficheiroEstudantes == NULL) {
        ficheiroEstudantes = fopen("exercicios.dat", "w+b");
        if (ficheiroEstudantes == NULL) {
            printf("Não foi possivel abrir os exercicios!!!!");
            aux = 1;
        }
    }

    ficheiroSubmissoes = fopen("submissoes.dat", "r+b");
    if (ficheiroEstudantes == NULL) {
        ficheiroEstudantes = fopen("submissoes.dat", "w+b");
        if (ficheiroEstudantes == NULL) {
            printf("Não foi possivel abrir as submissoes!!!!");
            aux = 1;
        }
    }

    return aux;
}

void getAndShowCountsOfFiles(int *countEstudantes, int *countFichas, int *countExercicios, int *countSubmissoes){
    Estudante estudante;
    while (fread(&estudante, sizeof(Estudante), 1, ficheiroEstudantes)) {
        countEstudantes++;
    }

    Ficha ficha;
    while (fread(&ficha, sizeof(Ficha), 1, ficheiroFichas)) {
        countFichas++;
    }

    Exercicio exercicio;
    while (fread(&exercicio, sizeof(Exercicio), 1, ficheiroExercicios)) {
        countExercicios++;
    }

    Submissao submissao;
    while (fread(&submissao, sizeof(Submissao), 1, ficheiroSubmissoes)) {
        countSubmissoes++;
    }
}

#pragma endregion

#pragma region Gestao Estudantes

void mainGestaoEstudantes() {
    int opcao;
    do{
        opcao = menuGestaoEstudantes();
        system("cls");
        switch (opcao){
            case 1:
                //LISTAR OS MANOS
                break;

            case 2:
                registerEstudante();
                break;

            case 3:
            case 4:
                printf("Opcao nao disponivel. Ainda esta em desenvolvimento");
                printf("\n(Pressione ENTER para continuar) ");
                while (getchar() != '\n'); // para funcionar o getchar (nao sei pq e preciso para ser sincero mas assim funciona e sem nao funciona)
                getchar();      
                break;
            
            case 0:
                return ;
                break;

            default:
                printf("Opcao invalida\n");
                break;
        }
    }while (opcao != 0);
}

int menuGestaoEstudantes(){
    char opcao;
    int aux = 9;

    system("cls");
    printf("\n--- Menu Gestao Estudantes ---\n");
    printf("1. Consultar Estudantes\n");
    printf("2. Inserir Estudantes\n");
    printf("3. Eliminar Estudantes\n");
    printf("4. Editar Estudantes\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
    scanf(" %c", &opcao);
    if(isdigit(opcao)){
        aux = opcao - '0';
    }
    return aux;
}

void registerEstudante(){

}

#pragma endregion



void gestaoExercicios() {
    Estudante estudante;
    rewind(ficheiroEstudantes); //para tipo voltar ao primeiro id
    fread(&estudante, sizeof(Estudante), 1, ficheiroEstudantes);
    printf("%d\n%d\n%s\n%s\n", estudante.id, estudante.numero, estudante.email, estudante.nome);
}

void gestaoFichas() {
    printf("gestaoFichas() not implemented yet.\n");
}

void gestaoSubmissoes() {
    printf("gestaoSubmissoes() not implemented yet.\n");
}

void estatisticas() {
    printf("estatisticas() not implemented yet.\n");
}

void saveData() {
    printf("saveData() not implemented yet.\n");
}

void loadData() {
    printf("loadData() not implemented yet.\n");
}
