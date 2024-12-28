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

int nextEstundateID;
int nextFichasID;
int nextExerciciosID;
int nextSubmissoesID;

//|||||||||||||||||||||||||||||||||||||||||||
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// AS FUNÇÕES NAO PODEM TER MAIS DE 25 LINHAS (exepto main)
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//|||||||||||||||||||||||||||||||||||||||||||

//Codigo de teste

/* Estudante estudante = {0, 10, "ahaha", "ahaah"};
    fwrite(&estudante, sizeof(Estudante), 1, ficheiroEstudantes);
    fflush(ficheiroEstudantes); //para garantir que guardou as cenas
    printf("%d\n%d\n%s\n%s\n", estudante.id, estudante.numero, estudante.email, estudante.nome); 
    
    Estudante estudante;
    rewind(ficheiroEstudantes); //para tipo voltar ao primeiro id
    fread(&estudante, sizeof(Estudante), 1, ficheiroEstudantes);
    printf("%d\n%d\n%s\n%s\n", estudante.id, estudante.numero, estudante.email, estudante.nome);*/

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
int mainFetchData();
void fetchOnSuccess();
void getCountFromFiles(int *countEstudantes, int *countFichas, int *countExercicos, int *countSubmissoes);
int fetchDataEstudantesAndFichas();
int fetchDataExerciciosAndSubmissoes();

void mainGestaoEstudantes();
int menuGestaoEstudantes();

void mainGestaoExercicios();
int menuGestaoExercicios();
void gestaoExercicosListAll();
void gestaoExercicosRegister();
void gestaoExerciciosRegisterQuestions(int passo, Exercicio *exercicio);
void gestaoExerciciosRegisterGuardar(Exercicio exercicio);

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

    int opcao;

    opcao = mainFetchData();

    do
    {
        opcao = mainMenu();
        switch (opcao)
        {
        case 1:
            mainGestaoEstudantes();
            break;
        case 2:
            mainGestaoExercicios();
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

//-------------------------------------------------------------------------------------------------------------------------------

#pragma region fetchData

int mainFetchData(){
    int aux1 = fetchDataEstudantesAndFichas();

    if(aux1 == 0){
        aux1 = fetchDataExerciciosAndSubmissoes();
        if(aux1 == 0){
            fetchOnSuccess();
            return 1;
        }
    }else{
        printf("\nNao foi possivel abrir os ficheiros!");
        printf("\n\n(Pressione ENTER para continuar) ");
        getchar();
        system("cls");
        return 0;
    }
}

void fetchOnSuccess(){
    int countEstudantes = 0, countFichas = 0, countExercicos = 0, countSubmissoes = 0;
    
    getCountFromFiles(&countEstudantes, &countFichas, &countExercicos, &countSubmissoes);

    nextEstundateID = countEstudantes + 1;
    nextFichasID = countFichas + 1;
    nextExerciciosID = countExercicos + 1;
    nextSubmissoesID = countSubmissoes + 1;

    printf("\nFicheiros abertos com sucesso!");
    printf("\n\nEstudantes carregados: %d", countEstudantes);
    printf("\nFichas carregadas: %d", countFichas);
    printf("\nExercicos carregados: %d", countExercicos);
    printf("\nSubmissoes carregadas: %d", countSubmissoes);
    printf("\n\n(Pressione ENTER para continuar) ");
    getchar();
    system("cls");
}

void getCountFromFiles(int *countEstudantes, int *countFichas, int *countExercicios, int *countSubmissoes) {
    rewind(ficheiroEstudantes);
    rewind(ficheiroFichas);
    rewind(ficheiroExercicios);
    rewind(ficheiroSubmissoes);

    Estudante estudante;
    while (fread(&estudante, sizeof(Estudante), 1, ficheiroEstudantes)) {
        (*countEstudantes)++;
    }

    Ficha ficha;
    while (fread(&ficha, sizeof(Ficha), 1, ficheiroFichas)) {
        (*countFichas)++;
    }

    Exercicio exercicio;
    while (fread(&exercicio, sizeof(Exercicio), 1, ficheiroExercicios)) {
        (*countExercicios)++;
    }

    Submissao submissao;
    while (fread(&submissao, sizeof(Submissao), 1, ficheiroSubmissoes)) {
        (*countSubmissoes)++;
    }
}

int fetchDataEstudantesAndFichas(){
    int aux = 0;
    ficheiroEstudantes = fopen("estudantes.dat", "r+b");
    if (ficheiroEstudantes == NULL) {
        ficheiroEstudantes = fopen("estudantes.dat", "w+b");
        if (ficheiroEstudantes == NULL) {
            printf("Não foi possivel abrir o ficheiro dos Estudantes!!!!");
            aux = 1;
        }
    }

    ficheiroFichas = fopen("fichas.dat", "r+b");
    if (ficheiroFichas == NULL) {
        ficheiroFichas = fopen("fichas.dat", "w+b");
        if (ficheiroFichas == NULL) {
            printf("Não foi possivel abrir o ficheiro das Fichas!!!!");
            aux = 1;
        }
    }

    return aux;
}

int fetchDataExerciciosAndSubmissoes(){
    int aux = 0;
    ficheiroExercicios = fopen("exercicios.dat", "r+b");
    if (ficheiroExercicios == NULL) {
        ficheiroExercicios = fopen("exercicios.dat", "w+b");
        if (ficheiroExercicios == NULL) {
            printf("Não foi possivel abrir o ficheiro dos Exercicios!!!!");
            aux = 1;
        }
    }

    ficheiroSubmissoes = fopen("submissoes.dat", "r+b");
    if (ficheiroSubmissoes == NULL) {
        ficheiroSubmissoes = fopen("submissoes.dat", "w+b");
        if (ficheiroSubmissoes == NULL) {
            printf("Não foi possivel abrir o ficheiro das Submissoes!!!!");
            aux = 1;
        }
    }

    return aux;
}

#pragma endregion

//-------------------------------------------------------------------------------------------------------------------------------

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
                //registar OS MANOS
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

#pragma endregion

//-------------------------------------------------------------------------------------------------------------------------------

#pragma region Exercicos

void mainGestaoExercicios() {
    int opcao;
    do{
        opcao = menuGestaoExercicios();
        system("cls");
        switch (opcao){
            case 1:
                gestaoExercicosListAll();
                printf("\n(Pressione ENTER para continuar) ");
                while (getchar() != '\n'); // para funcionar o getchar (nao sei pq e preciso para ser sincero mas assim funciona e sem nao funciona)
                getchar(); 
                break;

            case 2:
                gestaoExercicosRegister();
                printf("\n(Pressione ENTER para continuar) ");
                while (getchar() != '\n'); // para funcionar o getchar (nao sei pq e preciso para ser sincero mas assim funciona e sem nao funciona)
                getchar(); 
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

int menuGestaoExercicios(){
    char opcao;
    int aux = 9;

    system("cls");
    printf("\n--- Menu Gestao Exercicios ---\n");
    printf("1. Consultar Exercicios\n");
    printf("2. Inserir Exercicios\n");
    printf("3. Eliminar Exercicios\n");
    printf("4. Editar Exercicios\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
    scanf(" %c", &opcao);
    if(isdigit(opcao)){
        aux = opcao - '0';
    }
    return aux;
}

void gestaoExercicosListAll(){
    Exercicio exercicio;

    rewind(ficheiroExercicios); //para tipo voltar ao primeiro id

    while (fread(&exercicio, sizeof(Exercicio), 1, ficheiroExercicios) == 1) {
        printf("ID: %d\n", exercicio.id);
        printf("ID da Ficha: %d\n", exercicio.fichaID);
        printf("Nome: %s\n", exercicio.nome);
        printf("Dificuldade: %s\n", exercicio.difuculdade);
        printf("Tipo: %s\n", exercicio.tipoSol);
        printf("--------------------\n");
    }
}

void gestaoExercicosRegister(){
    Exercicio exercicio = {0, 0, "Indefenido", "Indef", "Indef"};
    exercicio.id = nextExerciciosID;

    for (int i = 0; i < 5; i++)
    {
        system("cls");
        printf("Novo Exercicio--> \nId: %d \nId da Ficha: %d \nNome: %s \nDifiuldade: %s \nTipo: %s \n\n\n", exercicio.id, exercicio.fichaID, exercicio.nome, exercicio.difuculdade, exercicio.tipoSol);
        gestaoExerciciosRegisterQuestions(i, &exercicio);
    }
    char output;
    do{
        printf("Deseja guardar o novo exercicio? (S/n)");
        scanf(" %c", &output);
        if(output != 'S' && output != 'n' && output != 's' && output != 'N')
            printf("\nPorfavor insira apenas (S/n)\n\n");
    }while(output != 'S' && output != 'n' && output != 's' && output != 'N');
    if(output == 's' || output == 'S')
        gestaoExerciciosRegisterGuardar(exercicio);
}

void gestaoExerciciosRegisterQuestions(int passo, Exercicio *exercicio){
    switch (passo)
        {
            case 0:
                printf("Insira o Id da Ficha: ");
                scanf("%d", &(*exercicio).fichaID);
                break;
            case 1:
                printf("Insira o Nome: ");
                scanf(" %30[^\n]", (*exercicio).nome);
                break;
            case 2:
                printf("Insira a Dificuldade: ");
                scanf(" %10[^\n]", (*exercicio).difuculdade);
                break;
            case 3:
                printf("Insira o Tipo: ");
                scanf(" %10[^\n]", (*exercicio).tipoSol);
                break;
        }
}

void gestaoExerciciosRegisterGuardar(Exercicio exercicio){
    fwrite(&exercicio, sizeof(Exercicio), 1, ficheiroExercicios);
    fflush(ficheiroExercicios);
    printf("\nExercicio guardado com Sucesso!");
}

#pragma endregion

//-------------------------------------------------------------------------------------------------------------------------------

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
