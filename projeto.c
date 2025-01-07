#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

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
int isThereAtLeastOneEstudante();
int isThereAtLeastOneFicha();
int isThereAtLeastOneExercicio();
void opcaoIndisponivel();

int mainFetchData();
void fetchOnSuccess();
void getCountFromFiles(int *countEstudantes, int *countFichas, int *countExercicos, int *countSubmissoes);
int fetchDataEstudantesAndFichas();
int fetchDataExerciciosAndSubmissoes();

void mainGestaoEstudantes();
int menuGestaoEstudantes();
void gestaoEstudantesListAll();
void gestaoEstudantesRegister();
int gestaoEstudantesRegisterQuestions(int passo, Estudante *estudante);
int gestaoEstudantesRegisterQuestionNumber();
int gestaoEstudantesRegisterQuestionEmail(char *input, int maxLength);
void gestaoEstudantesRegisterGuardar(Estudante estudante);

void mainGestaoExercicios();
int menuGestaoExercicios();
void gestaoExercicosListAll();
void gestaoExercicosRegister();
int gestaoExerciciosRegisterQuestions(int passo, Exercicio *exercicio);
int gestaoExerciciosRegisterQuestionIdFicha(int *input);
int gestaoExerciciosRegisterQuestionDificulty(char *input);
int gestaoExerciciosRegisterQuestionTypeSol(char *input);
void gestaoExerciciosRegisterGuardar(Exercicio exercicio);

void mainGestaoFichas();
int menuGestaoFichas();
void gestaoFichasListAll();
void gestaoFichasRegister();
int gestaoFichasRegisterQuestions(int passo, Ficha *ficha);
int gestaoFichasRegisterQuestionNumExercicios(int *input);
int gestaoFichasRegisterQuestionDataChangeQuestion();
int gestaoFichasRegisterQuestionData(Data *data);
int gestaoFichasRegisterQuestionDataAuxiliar(int passo, int *dia, int *mes, int *ano);
int isAnoBissexto(int year);
int gestaoFichasRegisterQuestionDataAuxiliarDayTests(int aux, int mes, int ano, int input);
void gestaoFichasRegisterGuardar(Ficha ficha);

void mainGestaoSubmissoes();
int menuGestaoSubmissoes();
void gestaoSubmissoesListAll();
void gestaoSubmissoesRegister();
Data getCurrentDate();
int gestaoSubmissoesRegisterQuestions(int passo, Submissao *submissao);
int gestaoSubmissoesRegisterQuestionIdEstudante();
int gestaoSubmissoesRegisterQuestionIdExercicioExiste(int idFicha, int *exerID);
int gestaoSubmissoesRegisterQuestionIdExercicioDaFicha(int idExer, int idFicha);
int gestaoSubmissoesRegisterQuestionLinhasSolucao(int *input);
int gestaoSubmissoesRegisterQuestionNota(int *input);
int gestaoSubmissoesRegisterQuestionData(Data *data);
void gestaoSubmissoesRegisterGuardar(Submissao submissao);

void estatisticas();
void saveData();

int loadData();
void fetchOnSuccessLoadData();

int getStringInput(char *str, int maxLength);
int getNumInput(int *validInt, int maxLength);
int isInt(char *str);

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
                if(isThereAtLeastOneFicha() != 0){
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
                if(isThereAtLeastOneEstudante() != 0 && isThereAtLeastOneFicha() != 0 && isThereAtLeastOneExercicio() != 0){
                    mainGestaoSubmissoes();
                }else{
                    printf("\nPara inserir uma Submissao e necessario ter pelo menos: \n1 Estudante registado, 1 Ficha registada e 1 Exercicio registado.");
                    printf("\n(Pressione ENTER para continuar) ");
                    while (getchar() != '\n');
                    getchar(); 
                }
                break;
            case 5:
                estatisticas();
                break;
            case 6:
                saveData();
                break;
            case 7:
                opcao = loadData();
                break;
            case 0:
                return ;
                break;
            default:
                printf("Opcao invalida\n");
        }
    } while (opcao != 0);
}

int isThereAtLeastOneEstudante(){
    Estudante estudante;
    int aux = 0;
    rewind(ficheiroEstudantes);
    while (fread(&estudante, sizeof(Estudante), 1, ficheiroEstudantes)) {
        aux++;
        break;
    }
    return aux;
}

int isThereAtLeastOneFicha(){
    Ficha ficha;
    int aux = 0;
    rewind(ficheiroFichas);
    while (fread(&ficha, sizeof(Ficha), 1, ficheiroFichas)) {
        aux++;
        break;
    }
    return aux;
}

int isThereAtLeastOneExercicio(){
    Exercicio exercicio;
    int aux = 0;
    rewind(ficheiroExercicios);
    while (fread(&exercicio, sizeof(Exercicio), 1, ficheiroExercicios)) {
        aux++;
        break;
    }
    return aux;
}

void opcaoIndisponivel(){
    printf("Opcao nao disponivel.");
    printf("\n(Pressione ENTER para continuar) ");
    while (getchar() != '\n');
    getchar();   
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
                break;
            case 0:
                return ;
                break;
            case 3:
            case 4:
            default:
                opcaoIndisponivel();
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
    int returnQuestions = 1;
    for (int i = 0; i < 4; i++)
    {
        system("cls");
        printf("Adicionar Estudante--> \nID: %d \nNumero do Estudante: %d \nNome: %s \nEmail: %s \n\n(Pressione TAB e depois ENTER para voltar)\n", estudante.id, estudante.numero, estudante.nome, estudante.email);
        returnQuestions = gestaoEstudantesRegisterQuestions(i, &estudante);
        if(returnQuestions != 1)
            break;
    }
    if(returnQuestions != 0){
        char output;
        do{
            printf("Deseja guardar o novo estudante? (S/n)\n");
            scanf(" %c", &output);
            if(output != 'S' && output != 'n' && output != 's' && output != 'N')
                printf("\nPorfavor insira apenas (S/n)\n\n");
        }while(output != 'S' && output != 'n' && output != 's' && output != 'N');
        if(output == 's' || output == 'S')
            gestaoEstudantesRegisterGuardar(estudante);
    }
}

int gestaoEstudantesRegisterQuestions(int passo, Estudante *estudante){
    int aux = 1, output = 1;
    switch (passo)
    {
        case 0:
            printf("Insira o Numero do Estudante (2241000-2241999): ");
            output = gestaoEstudantesRegisterQuestionNumber(&(*estudante).numero);
            break;
        case 1:
            printf("Insira o Nome: ");
            output = getStringInput((*estudante).nome, sizeof((*estudante).nome));
            break;
        case 2:
            printf("Insira o Email (exemplo@my.ipleiria.pt): ");
            output = gestaoEstudantesRegisterQuestionEmail((*estudante).email, sizeof((*estudante).email));
            break;
    }
    if(output == 2){
        aux = 0;
    }
    return aux;
}

int gestaoEstudantesRegisterQuestionNumber(int *input){
    int aux = 1, auxOutputNumInput;
    do
    {
        auxOutputNumInput = getNumInput(&(*input), 8);
        if(auxOutputNumInput == 1){
            if((*input) < 2241000 || (*input) > 2242000){
                printf("Porfavor insira um numero entre 2241000 e 2242000.\n");
            }else{
                aux = 0;
            }
        }else{
            break;
        }
    } while (aux != 0);
    return auxOutputNumInput;
}

int gestaoEstudantesRegisterQuestionEmail(char *input, int maxLength){
    int aux = 0;
    char fimDoEmail[18] = "@my.ipleiria.pt";
    do{
        int output = getStringInput(input, maxLength);
        if(output != 0){
            aux = 2;
        }else{
            int auxForTeste = 0;
            for (int i = 0; i < strlen(input); i++){
                if(input[i] == '@'){
                    auxForTeste = i;
                    break;
                }
            }
            if(auxForTeste != 0){
                char finalDoInput[strlen(input) - auxForTeste + 1];
                strcpy(finalDoInput, input + auxForTeste);
                if(strcmp(fimDoEmail, finalDoInput) == 0){
                    aux = 1;
                }else
                    printf("O email inserido nao e valido (exemplo@my.ipleiria.pt): ");
            }
        }
    } while (aux != 1 && aux != 2);
    return aux;
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
                break;
            case 0:
                return ;
                break;
            case 3:
            case 4:
            default:
                opcaoIndisponivel();
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
    int returnQuestions = 1;
    for (int i = 0; i < 5; i++)
    {
        system("cls");
        printf("Novo Exercicio--> \nId: %d \nId da Ficha: %d \nNome: %s \nDifiuldade: %s \nTipo: %s \n\n(Pressione TAB e depois ENTER para voltar)\n", exercicio.id, exercicio.fichaID, exercicio.nome, exercicio.difuculdade, exercicio.tipoSol);
        returnQuestions = gestaoExerciciosRegisterQuestions(i, &exercicio);
        if(returnQuestions != 1)
            break;
    }
    if(returnQuestions != 0){
        char output;
        do{
            printf("Deseja guardar o novo exercicio? (S/n)\n");
            scanf(" %c", &output);
            if(output != 'S' && output != 'n' && output != 's' && output != 'N')
                printf("\nPorfavor insira apenas (S/n)\n\n");
        }while(output != 'S' && output != 'n' && output != 's' && output != 'N');
        if(output == 's' || output == 'S')
            gestaoExerciciosRegisterGuardar(exercicio);
    }
}

int gestaoExerciciosRegisterQuestions(int passo, Exercicio *exercicio){
    int aux = 1, output = 1;
    switch (passo){
        case 0:
            printf("Insira o Id da Ficha: ");
            output = gestaoExerciciosRegisterQuestionIdFicha(&(*exercicio).fichaID);
            break;
        case 1:
            printf("Insira o Nome: ");
            output = getStringInput((*exercicio).nome, sizeof((*exercicio).nome));
            break;
        case 2:
            printf("Insira a Dificuldade-->\n1- Baixo\n2- Medio\n3- Elevado\nInsira aqui: ");
            output = gestaoExerciciosRegisterQuestionDificulty((*exercicio).difuculdade);
            break;
        case 3:
            printf("Insira o Tipo-->\n1- Algoritmo\n2- Codigo\nInsira aqui: ");
            output = gestaoExerciciosRegisterQuestionTypeSol((*exercicio).tipoSol);
            break;
    }
    if(output == 2){
        aux = 0;
    }
    return aux;
}

int gestaoExerciciosRegisterQuestionIdFicha(int *input){
    int aux = 0, auxOutputNumInput;
    do
    {
        auxOutputNumInput = getNumInput(&(*input), 3);
        if(auxOutputNumInput == 1){
            Ficha ficha;
            rewind(ficheiroFichas); //para tipo voltar ao primeiro id
            while (fread(&ficha, sizeof(Ficha), 1, ficheiroFichas) == 1) {
                if((*input) == ficha.id){
                    aux = 1;
                    break;;
                }
            }
            if(aux == 0){
                printf("Nao foi possivel encontrar uma ficha com esse ID.");
                printf("\nPorfavor insira novamente um ID: ");
            }
        }else{
            break;
        }
    } while (aux != 1);
    return auxOutputNumInput;
}

int gestaoExerciciosRegisterQuestionDificulty(char *input){
    int aux = 0, auxOutputNumInput, inputInt = 0;
    do{
        auxOutputNumInput = getNumInput(&inputInt, 3);
        if(auxOutputNumInput == 1){
            switch (inputInt){
                case 1:
                    strcpy(input, "Baixo");
                    aux = 1;
                    break;
                case 2:
                    strcpy(input, "Medio");
                    aux = 1;
                    break;
                case 3:
                    strcpy(input, "Elevado");
                    aux = 1;
                    break;
                default:
                    printf("Porfavor insira apenas entre 1, 2 ou 3: ");
                    break;
            }
        }else{
            break;
        }
    } while (aux != 1);
    return auxOutputNumInput;
}

int gestaoExerciciosRegisterQuestionTypeSol(char *input){
    int aux = 0, auxOutputNumInput, inputInt = 0;
    do{
        auxOutputNumInput = getNumInput(&inputInt, 3);
        if(auxOutputNumInput == 1){
            switch (inputInt){
                case 1:
                    strcpy(input, "Algoritmo");
                    aux = 1;
                    break;
                case 2:
                    strcpy(input, "Codigo");
                    aux = 1;
                    break;
                default:
                    printf("Porfavor insira apenas entre 1 e 2: ");
                    break;
            }
        }else{
            break;
        }
    } while (aux != 1);
    return auxOutputNumInput;
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
                break;
            case 0:
                return ;
                break;
            case 3:
            case 4:
            default:
                opcaoIndisponivel();
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
    Data data = getCurrentDate();
    Ficha ficha = {0, "Indefenido", 0, data};
    ficha.id = nextFichasID;
    int returnQuestions = 1;
    for (int i = 0; i < 4; i++)
    {
        system("cls");
        printf("Nova Ficha--> \nId: %d \nNome: %s \nNumero de Exercicios: %d \nData de Publicacao: %02d/%02d/%d \n\n\n", ficha.id, ficha.nome, ficha.numEx, ficha.dataPubl.dia, ficha.dataPubl.mes, ficha.dataPubl.ano);
        returnQuestions = gestaoFichasRegisterQuestions(i, &ficha);
        if(returnQuestions != 1)
            break;
    }
    if(returnQuestions != 0){
        char output;
        do{
            printf("Deseja guardar a nova ficha? (S/n)\n");
            scanf(" %c", &output);
            if(output != 'S' && output != 'n' && output != 's' && output != 'N')
                printf("\nPorfavor insira apenas (S/n)\n\n");
        }while(output != 'S' && output != 'n' && output != 's' && output != 'N');
        if(output == 's' || output == 'S')
            gestaoFichasRegisterGuardar(ficha);
    }
}

int gestaoFichasRegisterQuestions(int passo, Ficha *ficha){
    int aux = 1, output = 1;
    switch (passo){
        case 0:
            printf("Insira o Nome (30 caracteres): ");
            while (getchar() != '\n');
            output = getStringInput((*ficha).nome, sizeof((*ficha).nome));
            break;
        case 1:
            printf("Insira o Numero de Exercicios (1-99): ");
            output = gestaoFichasRegisterQuestionNumExercicios(&(*ficha).numEx);
            break;
        case 2:
            if(gestaoFichasRegisterQuestionDataChangeQuestion() == 1){
                while (getchar() != '\n');
                output = gestaoFichasRegisterQuestionData(&(*ficha).dataPubl);
            }
            break;
    }
    if(output == 2){
        aux = 0;
    }
    return aux;
}

int gestaoFichasRegisterQuestionNumExercicios(int *input){
    int aux = 0, auxOutputNumInput, inputInt = 0;
    do{
        auxOutputNumInput = getNumInput(&(*input), 3);
        if(auxOutputNumInput == 1){
            if((*input) < 1 || (*input) > 99){
                printf("Insira apenas entre (1-99): ");
            }else{
                aux = 1;
            }
        }else{
            break;
        }
    } while (aux != 1);
    return auxOutputNumInput;
}

int gestaoFichasRegisterQuestionDataChangeQuestion(){
    char output;
    int aux = 0;
    do{
        printf("Deseja mudar a Data? (S/n)\n");
        scanf(" %c", &output);
        if(output != 'S' && output != 'n' && output != 's' && output != 'N')
            printf("\nPorfavor insira apenas (S/n)\n\n");
    }while(output != 'S' && output != 'n' && output != 's' && output != 'N');
    if(output == 's' || output == 'S'){
        aux = 1;
    }else{
        aux = 0;
    }
    return aux;
}

int gestaoFichasRegisterQuestionData(Data *data){
    int aux = 0, auxOutputNumInput, inputInt = 0;
    do{
         for(int i = 0; i < 3; i++){
            switch (i){
                case 0:
                    printf("\nAno (1900-2025): ");
                    auxOutputNumInput = gestaoFichasRegisterQuestionDataAuxiliar(i, &(*data).dia, &(*data).mes, &(*data).ano);
                    break;
                case 1:
                    printf("\nMes (1-12): ");
                    auxOutputNumInput = gestaoFichasRegisterQuestionDataAuxiliar(i, &(*data).dia, &(*data).mes, &(*data).ano);
                    break;
                case 2:
                    printf("\nDia (1-31): ");
                    auxOutputNumInput = gestaoFichasRegisterQuestionDataAuxiliar(i, &(*data).dia, &(*data).mes, &(*data).ano);
                    aux = 1;
                    break;
            }
            if(auxOutputNumInput == 2){
                aux = 1;
                break;
            }
        }
    } while (aux != 1);
    return auxOutputNumInput;
}

int gestaoFichasRegisterQuestionDataAuxiliar(int passo, int *dia, int *mes, int *ano){
    int input = 0, aux = 0, auxOutputNumInput;
    do{
        aux = 0;
        auxOutputNumInput = getNumInput(&input, 5);
        switch (passo){
            case 0:
                if(input < 1900 || input > 2025){
                    printf("Porfavor insira um ano entre 1900 e 2025: ");
                    aux = 1;}
                (*ano) = input;
                break;
            case 1:
                if(input < 1 || input > 12){
                    printf("Porfavor insira um mes entre 1 e 12: ");
                    aux = 1;}
                (*mes) = input;
                break;
            case 2:
                aux = gestaoFichasRegisterQuestionDataAuxiliarDayTests(aux, (*mes), (*ano), input);
                (*dia) = input;
                break;
        }
        if(auxOutputNumInput == 2)
            break;
    }while(aux != 0);
    return auxOutputNumInput;
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
                gestaoSubmissoesRegister();
                break;
            case 0:
                return ;
                break;
            case 3:
            case 4:
            default:
                opcaoIndisponivel();
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
        printf("Linhas de Solucao: %d\n", submissao.linhasSol);
        printf("Tipo: %d\n", submissao.nota);
        printf("--------------------\n");
    }
}

void gestaoSubmissoesRegister(){
    Data data = getCurrentDate();
    Submissao submissao = {0, 0, 0, 0, data, 0, 0};
    submissao.id = nextSubmissoesID;
    int returnQuestions = 1;
    for (int i = 0; i < 7; i++)
    {
        system("cls");
        printf("Nova Submissao--> \nId: %d \nId do Estudante: %d \nId da Ficha: %d \nId do Exercicio: %d \nData da Submissao: %02d/%02d/%d \nLinhasSol: %d \nNota: %d \n\n\n", submissao.id, submissao.estudanteID, submissao.fichaiD, submissao.exercID, submissao.dataSub.dia, submissao.dataSub.mes, submissao.dataSub.ano, submissao.linhasSol, submissao.nota);
        returnQuestions = gestaoSubmissoesRegisterQuestions(i, &submissao);
        if(returnQuestions != 1)
            break;
    }
    if(returnQuestions != 0){
        char output;
        do{
            printf("Deseja guardar a nova submissao? (S/n)\n");
            scanf(" %c", &output);
            if(output != 'S' && output != 'n' && output != 's' && output != 'N')
                printf("\nPorfavor insira apenas (S/n)\n\n");
        }while(output != 'S' && output != 'n' && output != 's' && output != 'N');
        if(output == 's' || output == 'S')
            gestaoSubmissoesRegisterGuardar(submissao);
    }
}

Data getCurrentDate(){
    Data data = {};
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);

    data.dia =  current_time->tm_mday;
    data.mes =  current_time->tm_mon + 1;
    data.ano =  current_time->tm_year + 1900;

    return data;
}

int gestaoSubmissoesRegisterQuestions(int passo, Submissao *submissao){
    int aux = 1, output = 1;
    switch (passo){
        case 0:
            printf("Insira o Id do Estudante: ");
            output = gestaoSubmissoesRegisterQuestionIdEstudante(&(*submissao).estudanteID);
            break;
        case 1:
            printf("Insira o Id da Ficha: ");
            output = gestaoExerciciosRegisterQuestionIdFicha(&(*submissao).fichaiD);
            break;
        case 2:
            printf("Insira o Id do Exercicio: ");
            output = gestaoSubmissoesRegisterQuestionIdExercicioExiste((*submissao).fichaiD, &(*submissao).exercID);
            break;
        case 3:
            printf("Insira o Linhas de Solucao (1-100): ");
            output = gestaoSubmissoesRegisterQuestionLinhasSolucao(&(*submissao).linhasSol);
            break;
        case 4:
            printf("Insira a Nota (0-100): ");
            output = gestaoSubmissoesRegisterQuestionNota(&(*submissao).nota);
            break;
        case 5:
            if(gestaoFichasRegisterQuestionDataChangeQuestion() == 1){
                while (getchar() != '\n');
                output = gestaoSubmissoesRegisterQuestionData(&(*submissao).dataSub);
            }
            break;
    }
    if(output == 2){
        aux = 0;
    }
    return aux;
}

int gestaoSubmissoesRegisterQuestionIdEstudante(int *estudanteId){
    while (getchar() != '\n');
    int aux = 0, auxOutputNumInput;
    do{
        auxOutputNumInput = getNumInput(&(*estudanteId), 3);
        Estudante estudante;
        rewind(ficheiroEstudantes); //para tipo voltar ao primeiro id
        while (fread(&estudante, sizeof(Estudante), 1, ficheiroEstudantes) == 1) {
            if((*estudanteId) == estudante.id){
                aux = 1;
                break;;
            }
        }
        if(aux == 0){
            printf("Nao foi possivel encontrar um estudante com esse ID.");
            printf("\nPorfavor insira novamente um ID: ");
        }
    } while (aux != 1);
    return auxOutputNumInput;
}

int gestaoSubmissoesRegisterQuestionIdExercicioExiste(int idFicha, int *exerID){
    int aux = 0, auxOutputNumInput;
    do{
        auxOutputNumInput = getNumInput(&(*exerID), 3);
        if(auxOutputNumInput == 1){
            Exercicio exercicio;
            rewind(ficheiroExercicios); //para tipo voltar ao primeiro id
            while (fread(&exercicio, sizeof(Exercicio), 1, ficheiroExercicios) == 1) {
                if((*exerID) == exercicio.id){
                    aux = 1;
                    break;;
                }
            }
            if(aux == 0){
                printf("Nao foi possivel encontrar um exercicio com esse ID.");
                printf("\nPorfavor insira novamente um ID: ");
            }else{
                aux = gestaoSubmissoesRegisterQuestionIdExercicioDaFicha((*exerID), idFicha);
            }
        }else{
            break;
        }
    } while (aux != 1);
    return auxOutputNumInput;
}

int gestaoSubmissoesRegisterQuestionIdExercicioDaFicha(int idExer, int idFicha){
    int input, aux = 0;
    Exercicio exercicio;
    rewind(ficheiroExercicios); //para tipo voltar ao primeiro id
    while (fread(&exercicio, sizeof(Exercicio), 1, ficheiroExercicios) == 1) {
        if(exercicio.id == idExer){
            if(exercicio.fichaID == idFicha){
                aux = 1;
                break;;
            }
            break;
        }
    }
    if(aux == 0){
        printf("A Ficha inserida anteriormente nao contem esse exercicio.");
        printf("\nPorfavor insira outro ID de exercicio: ");
    }
    return aux;
}

int gestaoSubmissoesRegisterQuestionLinhasSolucao(int *input){
    int aux = 0;
    do{
        aux = getNumInput(&(*input), 4);
        if(aux == 1){
            if((*input) < 1 || (*input) > 100)
                printf("Insira apenas entre (1-100): ");
        }else{
            break;
        }
    }while((*input) < 1 || (*input) > 100);
    return aux;
}

int gestaoSubmissoesRegisterQuestionNota(int *input){
    int aux = 0;
    do{
        aux = getNumInput(&(*input), 4);
        if(aux == 1){
            if((*input) < 0 || (*input) > 100)
                printf("Insira apenas entre (0-100): ");
        }else{
            break;
        }
    }while((*input) < 0 || (*input) > 100);
    return aux;
}

int gestaoSubmissoesRegisterQuestionData(Data *data){
    int aux = 0, auxOutputNumInput, inputInt = 0;
    do{
         for(int i = 0; i < 3; i++){
            switch (i){
                case 0:
                    printf("\nAno (1900-2025): ");
                    auxOutputNumInput = gestaoFichasRegisterQuestionDataAuxiliar(i, &(*data).dia, &(*data).mes, &(*data).ano);
                    break;
                case 1:
                    printf("\nMes (1-12): ");
                    auxOutputNumInput = gestaoFichasRegisterQuestionDataAuxiliar(i, &(*data).dia, &(*data).mes, &(*data).ano);
                    break;
                case 2:
                    printf("\nDia (1-31): ");
                    auxOutputNumInput = gestaoFichasRegisterQuestionDataAuxiliar(i, &(*data).dia, &(*data).mes, &(*data).ano);
                    aux = 1;
                    break;
            }
            if(auxOutputNumInput == 2){
                aux = 1;
                break;
            }
        }
    } while (aux != 1);
    return auxOutputNumInput;
}

void gestaoSubmissoesRegisterGuardar(Submissao submissao){
    fwrite(&submissao, sizeof(Submissao), 1, ficheiroSubmissoes);
    fflush(ficheiroSubmissoes);
    nextSubmissoesID++;
    printf("\nSubmissao guardada com Sucesso!");
}

#pragma endregion

//-------------------------------------------------------------------------------------------------------------------------------

void estatisticas() {
    printf("estatisticas() not implemented yet.\n");
}

//-------------------------------------------------------------------------------------------------------------------------------

void saveData() {
    printf("saveData() not implemented yet.\n");
}

//-------------------------------------------------------------------------------------------------------------------------------

int loadData() {
    fclose(ficheiroEstudantes);
    fclose(ficheiroExercicios);
    fclose(ficheiroFichas);
    fclose(ficheiroSubmissoes);
    int aux1 = fetchDataEstudantesAndFichas();
    if(aux1 == 0){
        aux1 = fetchDataExerciciosAndSubmissoes();
        if(aux1 == 0){
            fetchOnSuccessLoadData();
            return 1;
        }
    }else{
        printf("\nNao foi possivel abrir os ficheiros!");
        printf("\n\n(Pressione ENTER para continuar) ");
        while (getchar() != '\n');
        getchar();
        system("cls");
        return 0;
    }
}

void fetchOnSuccessLoadData(){
    system("cls");
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
    while (getchar() != '\n');
    getchar();
}

int getStringInput(char *str, int maxLength){
    int index = 0, aux = 0;
    char input;
    do{
        input = getchar();
        if (input == '\t') { // Tab pressionado
            aux = 2;
        } else if (input == '\n') { // Enter pressionado
            aux = 1;
        } else if (index < maxLength - 1) {
            str[index++] = input;
        }
    }while(aux != 1 && aux != 2);
    str[index] = '\0';
    return aux;
}

int getNumInput(int *validInt,  int maxLength){
    char str[20], input;
    int index = 0, aux = 0;
    do{
        input = getchar();
        if (input == '\t') { // Tab pressionado
            aux = 2;
        } else if (input == '\n') { // Enter pressionado
            str[index] = '\0';
            if (isInt(str)) {
                aux = 1;
                int ya = atoi(str);
                (*validInt) = ya;
            } else {
                printf("Porfavor insira um numero: ");
                index = 0; 
            }
        } else if (index < maxLength) {
            str[index++] = input;
        }
    }while(aux != 1 && aux != 2);
    return aux;
}

int isInt(char *str){
    if (*str == '\0') 
        return 0;
    if (*str == '-' || *str == '+') 
        str++;
    while (*str) {
        if (!isdigit(*str)) 
            return 0;
        str++;
    }
    return 1;
}
