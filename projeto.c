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

int nextEstudanteID;
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
int menuMainAux();

int mainFetchData();
void fetchOnSuccess();
void getCountFromFiles(int *countEstudantes, int *countFichas, int *countExercicos, int *countSubmissoes);
int fetchDataEstudantesAndFichas();
int fetchDataExerciciosAndSubmissoes();

void mainGestaoEstudantes();
int menuGestaoEstudantes();
void gestaoEstudantesListAll();
void gestaoEstudantesRegister();
void gestaoEstudantesRegisterQuestions(int passo, Estudante *estudante);
void gestaoEstudantesRegisterGuardar(Estudante estudante);

void mainGestaoExercicios();
int menuGestaoExercicios();
void gestaoExercicosListAll();
void gestaoExercicosRegister();
void gestaoExerciciosRegisterQuestions(int passo, Exercicio *exercicio);
void gestaoExerciciosRegisterQuestionIdFicha();
void gestaoExerciciosRegisterGuardar(Exercicio exercicio);

void mainGestaoFichas();
int menuGestaoFichas();
void gestaoFichasListAll();
void gestaoFichasRegister();
void gestaoFichasRegisterQuestions(int passo, Ficha *ficha);
Data gestaoFichasRegisterQuestionData();
int gestaoFichasRegisterQuestionDataAuxiliar(int passo, int mes, int ano);
int isAnoBissexto(int year);
int gestaoFichasRegisterQuestionDataAuxiliarDayTests(int aux, int mes, int ano, int input);
void gestaoFichasRegisterGuardar(Ficha ficha);

void mainGestaoSubmissoes();
int menuGestaoSubmissoes();
void gestaoSubmissoesListAll();

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
        switch (opcao){
            case 1:
                mainGestaoEstudantes();
                break;
            case 2:
                if(menuMainAux() != 0){
                    mainGestaoExercicios();
                }else{
                    printf("\nPorfavor insira uma ficha para que possa inserir exercicios.");
                    printf("\n(Pressione ENTER para continuar) ");
                    while (getchar() != '\n');
                    getchar(); 
                }
                break;
            case 3:
                mainGestaoFichas();
                break;
            case 4:
                mainGestaoSubmissoes();
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

int menuMainAux(){
    Ficha ficha;
    int aux = 0;
    rewind(ficheiroFichas);
    while (fread(&ficha, sizeof(Ficha), 1, ficheiroFichas)) {
        aux++;
        break;
    }
    return aux;
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

    nextEstudanteID = countEstudantes + 1;
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
                gestaoEstudantesListAll();
                printf("\n(Pressione ENTER para continuar) ");
                while (getchar() != '\n');
                getchar(); 
                break;

            case 2:
                gestaoEstudantesRegister();
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

void gestaoEstudantesListAll(){
    Estudante estudante; //numero, nome email

    rewind(ficheiroEstudantes); //para tipo voltar ao primeiro id

    while (fread(&estudante, sizeof(Estudante), 1, ficheiroEstudantes) == 1) { 
        printf("ID: %d\n", estudante.id);
        printf("Numero do Estudante: %d\n", estudante.numero);
        printf("Nome: %s\n", estudante.nome);
        printf("Email: %s\n", estudante.email);
        printf("--------------------\n");
    }
}

void gestaoEstudantesRegister(){
    Estudante estudante = {0, 0, "Indefenido", "Indefenido"};
    estudante.id = nextEstudanteID;

    for (int i = 0; i < 4; i++)
    {
        system("cls");
        printf("Adicionar Estudante--> \nID: %d \nNumero do Estudante: %d \nNome: %s \nEmail: %s \n\n\n", estudante.id, estudante.numero, estudante.nome, estudante.email);
        gestaoEstudantesRegisterQuestions(i, &estudante);
    }
    char output;
    do{
        printf("Deseja guardar o novo estudante? (S/n)");
        scanf(" %c", &output);
        if(output != 'S' && output != 'n' && output != 's' && output != 'N')
            printf("\nPorfavor insira apenas (S/n)\n\n");
    }while(output != 'S' && output != 'n' && output != 's' && output != 'N');
    if(output == 's' || output == 'S')
        gestaoEstudantesRegisterGuardar(estudante);
}

void gestaoEstudantesRegisterQuestions(int passo, Estudante *estudante){
    switch (passo)
        {
            case 0:
                printf("Insira o Numero do Estudante: ");
                scanf("%d", &(*estudante).numero);
                break;
            case 1:
                printf("Insira o Nome: ");
                scanf(" %30[^\n]", (*estudante).nome);
                break;
            case 2:
                printf("Insira o Email: ");
                scanf(" %10[^\n]", (*estudante).email);
                break;
        }
}

void gestaoEstudantesRegisterGuardar(Estudante estudante){
    fwrite(&estudante, sizeof(Estudante), 1, ficheiroEstudantes);
    fflush(ficheiroEstudantes);
    nextEstudanteID++;
    printf("\nEstudante guardado com Sucesso!");
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
            gestaoExerciciosRegisterQuestionIdFicha();
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

void gestaoExerciciosRegisterQuestionIdFicha(){
    int input, aux = 0;
    do
    {
        scanf("%d", &input);
        Ficha ficha;
        rewind(ficheiroFichas); //para tipo voltar ao primeiro id
        while (fread(&ficha, sizeof(Ficha), 1, ficheiroFichas) == 1) {
            if(input == ficha.id){
                aux = 1;
                return;
            }
        }
        if(aux == 0){
            printf("Nao foi possivel encontrar uma ficha com esse ID.");
            printf("\nPorfavor insira novamente um ID: ");
        }
    } while (aux != 1);
}

void gestaoExerciciosRegisterGuardar(Exercicio exercicio){
    fwrite(&exercicio, sizeof(Exercicio), 1, ficheiroExercicios);
    fflush(ficheiroExercicios);
    nextExerciciosID++;
    printf("\nExercicio guardado com Sucesso!");
}

#pragma endregion

//-------------------------------------------------------------------------------------------------------------------------------

#pragma region Fichas

void mainGestaoFichas() {
    int opcao;
    do{
        opcao = menuGestaoFichas();
        system("cls");
        switch (opcao){
            case 1:
                gestaoFichasListAll();
                printf("\n(Pressione ENTER para continuar) ");
                while (getchar() != '\n'); // para funcionar o getchar (nao sei pq e preciso para ser sincero mas assim funciona e sem nao funciona)
                getchar(); 
                break;

            case 2:
                gestaoFichasRegister();
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

int menuGestaoFichas(){
    char opcao;
    int aux = 9;

    system("cls");
    printf("\n--- Menu Gestao Fichas ---\n");
    printf("1. Consultar Fichas\n");
    printf("2. Inserir Fichas\n");
    printf("3. Eliminar Fichas\n");
    printf("4. Editar Fichas\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
    scanf(" %c", &opcao);
    if(isdigit(opcao)){
        aux = opcao - '0';
    }
    return aux;
}

void gestaoFichasListAll(){
    Ficha ficha;

    rewind(ficheiroFichas); //para tipo voltar ao primeiro id

    while (fread(&ficha, sizeof(Ficha), 1, ficheiroFichas) == 1) {
        printf("ID: %d\n", ficha.id);
        printf("Nome da Fichas: %s\n", ficha.nome);
        printf("Numero de Exercicios: %d\n", ficha.numEx);
        printf("Datade Publicacao: %02d/%02d/%d\n", ficha.dataPubl.dia, ficha.dataPubl.mes, ficha.dataPubl.ano);
        printf("--------------------\n");
    }
}

void gestaoFichasRegister(){
    Data data = {0, 0, 0};
    Ficha ficha = {0, "Indefenido", 0, data};
    ficha.id = nextFichasID;

    for (int i = 0; i < 4; i++)
    {
        system("cls");
        printf("Nova Ficha--> \nId: %d \nNome: %s \nNumero de Exercicios: %d \nData de Publicacao: %02d/%02d/%d \n\n\n", ficha.id, ficha.nome, ficha.numEx, ficha.dataPubl.dia, ficha.dataPubl.mes, ficha.dataPubl.ano);
        gestaoFichasRegisterQuestions(i, &ficha);
    }
    char output;
    do{
        printf("Deseja guardar a nova ficha? (S/n)");
        scanf(" %c", &output);
        if(output != 'S' && output != 'n' && output != 's' && output != 'N')
            printf("\nPorfavor insira apenas (S/n)\n\n");
    }while(output != 'S' && output != 'n' && output != 's' && output != 'N');
    if(output == 's' || output == 'S')
        gestaoFichasRegisterGuardar(ficha);
}

void gestaoFichasRegisterQuestions(int passo, Ficha *ficha){
    switch (passo)
        {
            case 0:
                printf("Insira o Nome (30 caracteres): ");
                scanf(" %30[^\n]", (*ficha).nome);
                while (getchar() != '\n'); // para limpar o input buffer para garantir que so entra 30 caracteres
                break;
            case 1:
                printf("Insira o Numero de Exercicios (1-99): ");
                do{
                    scanf("%d", &(*ficha).numEx);
                    if((*ficha).numEx < 1 || (*ficha).numEx > 99)
                        printf("Insira apenas entre (1-99): ");
                }while((*ficha).numEx < 1 || (*ficha).numEx > 99);
                break;
            case 2:
                printf("Insira a Data-->");
                (*ficha).dataPubl = gestaoFichasRegisterQuestionData();
                break;
        }
}

Data gestaoFichasRegisterQuestionData(){
    Data data;
    for(int i = 0; i < 3; i++){
        switch (i)
        {
            case 0:
                printf("\nAno (1900-2025): ");
                data.ano = gestaoFichasRegisterQuestionDataAuxiliar(i, data.mes, data.ano);
                break;
            case 1:
                printf("\nMes (1-12): ");
                data.mes = gestaoFichasRegisterQuestionDataAuxiliar(i, data.mes, data.ano);
                break;
            case 2:
                printf("\nDia (1-31): ");
                data.dia = gestaoFichasRegisterQuestionDataAuxiliar(i, data.mes, data.ano);
                break;
        }
    }
    return data;
}

int gestaoFichasRegisterQuestionDataAuxiliar(int passo, int mes, int ano){
    int input = 0, aux = 0;
    do{
        aux = 0;
        scanf("%d", &input);
        switch (passo)
        {
            case 0:
                if(input < 1900 || input > 2025){
                    printf("Porfavor insira um ano entre 1900 e 2025: ");
                    aux = 1;
                }
                break;
            case 1:
                if(input < 1 || input > 12){
                    printf("Porfavor insira um mes entre 1 e 12: ");
                    aux = 1;
                }
                break;
            case 2:
                aux = gestaoFichasRegisterQuestionDataAuxiliarDayTests(aux, mes, ano, input);
                break;
        }
    }while(aux != 0);
    return input;
}

int isAnoBissexto(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 1; //ano bissexto 29 dias
    }
    return 0;
}

int gestaoFichasRegisterQuestionDataAuxiliarDayTests(int aux, int mes, int ano, int input){
    if(input < 1 || input > 31){
        printf("Porfavor insira um dia entre 1 e 31: ");
        aux = 1;
    }else{
        if(mes == 2){
            if(input > 29 || input < 1){
                printf("Porfavor insira um dia entre 1 e 29 pois o mes inserido so pode ter 29 dias: ");
                aux = 1;
            }else{
                if(input > 28){
                    if(isAnoBissexto(ano) == 0){
                        printf("Porfavor insira um dia entre 1 e 28 pois o ano inserido nao e bissexto: ");
                        aux = 1;
                    }
                }
            }
        }else if(mes == 4 || mes == 6 || mes == 9 || mes == 11){
            if(input < 1 || input > 30){
                printf("Porfavor insira um dia entre 1 e 30 pois o mes inserido so tem 30 dias: ");
                aux = 1;
            }
        }
    }
    return aux;
}

void gestaoFichasRegisterGuardar(Ficha ficha){
    fwrite(&ficha, sizeof(Ficha), 1, ficheiroFichas);
    fflush(ficheiroFichas);
    nextFichasID++;
    printf("\nFicha guardada com Sucesso!");
}

#pragma endregion

//-------------------------------------------------------------------------------------------------------------------------------

#pragma region Submissoes

void mainGestaoSubmissoes() {
    int opcao;
    do{
        opcao = menuGestaoSubmissoes();
        system("cls");
        switch (opcao){
            case 1:
                gestaoSubmissoesListAll();
                printf("\n(Pressione ENTER para continuar) ");
                while (getchar() != '\n'); // para funcionar o getchar (nao sei pq e preciso para ser sincero mas assim funciona e sem nao funciona)
                getchar(); 
                break;

            case 2:
                //gestaoExercicosRegister();
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

int menuGestaoSubmissoes(){
    char opcao;
    int aux = 9;

    system("cls");
    printf("\n--- Menu Gestao Submissoes ---\n");
    printf("1. Consultar Submissoes\n");
    printf("2. Inserir Submissoes\n");
    printf("3. Eliminar Submissoes\n");
    printf("4. Editar Submissoes\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
    scanf(" %c", &opcao);
    if(isdigit(opcao)){
        aux = opcao - '0';
    }
    return aux;
}

void gestaoSubmissoesListAll(){
    Submissao submissao;

    rewind(ficheiroSubmissoes); //para tipo voltar ao primeiro id

    while (fread(&submissao, sizeof(Submissao), 1, ficheiroSubmissoes) == 1) {
        printf("ID: %d\n", submissao.id);
        printf("ID do Estudante: %d\n", submissao.estudanteID);
        printf("ID da Ficha: %d\n", submissao.fichaiD);
        printf("ID do Exercicio: %d\n", submissao.exercID);
        printf("Datade Submissao: %02d/%02d/%d\n", submissao.dataSub.dia, submissao.dataSub.mes, submissao.dataSub.ano);
        printf("Tipo: %d\n", submissao.linhasSol);
        printf("Tipo: %d\n", submissao.nota);
        printf("--------------------\n");
    }
}

#pragma endregion

//-------------------------------------------------------------------------------------------------------------------------------

void estatisticas() {
    printf("estatisticas() not implemented yet.\n");
}

void saveData() {
    printf("saveData() not implemented yet.\n");
}

void loadData() {
    printf("loadData() not implemented yet.\n");
}