#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

typedef struct{
    char linha;
    int coluna;
} tJogada;

tJogada LeJogada(FILE *saida, char matrizJogador[10][20]);
int EhJogadaInvalida(tJogada jogada, char matrizJogador[10][20]);
void ImprimeMatrizJogador(char matrizJogador[10][20], FILE *saida);

typedef struct{
    char nome[17];
    int idade;
} tJogador;

tJogador LeNomeJogador();

typedef struct{
    char tipo[11];
    int tamanho;
    int id;
    int orientacao;
    int coluna;
    char linha;
    int marcacao;
} tNavio;

int VerificaTamanhoDoNavil(char tipo[11]);
int EhLinhaInicial(int i, char linha);
int EhColunaInicial(int j, int coluna);
tNavio LeNavio(FILE *file);
int VerificaNavioFora(tNavio navio);
int TransformaNaColunaCorresp(int coluna);
int Afundou(char matrizJogadas[10][20], char matrizTabulerio[10][20], int lMat, int cMat);
int EhCruiser(char tipo[11]);
tNavio MarcaNavio(tNavio navio, int numJogada);
int EstaMarcadoNavio(tNavio navio);
int RetornaMarcacaoNavio(tNavio navio);

typedef struct{
    char matriz[10][20];
    int qtdNavios;
    tNavio navio[51];
} tTabuleiro;

tTabuleiro MontaTabuleiro(FILE *file);
void GeraMatrizSoComAgua(char matriz[10][20]);
tTabuleiro InsereNaviosNoTabuleiro(tTabuleiro tab);
int VerificaTabuleiro(tTabuleiro tab);
int VerificaCompativeis(tTabuleiro tab1, tTabuleiro tab2);
void ImprimeTabuleiroNoArquivo(tTabuleiro tab, FILE *inicializacao);
int VerificaNavioEncostando(tNavio navio, tTabuleiro tab);
int VerificaPosicaoInicialIgual(tTabuleiro tab);
void GeraMatrizSoComPonto(char matrizJogadas[10][20]);
int MarcaJogada(tJogada jogada, char matrizJogadas[10][20], tTabuleiro tab, FILE *saida);
void ImprimeNomeNavioAfundado(tTabuleiro tab, char l, int c, FILE *saida);
void ImprimeNomeEIDNavio(tTabuleiro tab, char l, int c, FILE *resultado);
void ImprimeNaviosOrdenados(tTabuleiro tab, FILE* estatisticas);
tTabuleiro IdentificaNavioEstatisticas(int i, tTabuleiro tab, int lNum, int c);

typedef struct{
    tTabuleiro tabuleiro1;
    tTabuleiro tabuleiro2;
    tJogador jogador1;
    tJogador jogador2;
} tJogo;

void GeraTabuleiroAleatorio(char diretorio[1001]);

tJogo InicializaJogo(char * argv[]);
void RealizaJogo(tJogo jogo, char * argv[]);
void GeraEstatisticas(tJogo jogo);
int VerificaGanhou(char matrizJogadas[10][20], tTabuleiro tab);
void GeraJogadas1Resultado(FILE *resultado, FILE *saida, char *argv[], tTabuleiro tab);
void GeraJogadas2Resultado(FILE *resultado, FILE *saida, char *argv[], tTabuleiro tab);
int EhLetra(char c);
int EhNum(char c);
int EhImpar(int i);
void GeraArquivoEstatisticas(tJogo jogo, char * argv[]);
float CalculaMedia(float soma, int qtd);
float CalculaDesvioPadrao(int jogadas[200][3], float linhaMedio, float colunaMedio, int qtdJogadas);
void ListaNaviosAtingidos(int qtdJogadas, int jogadas[200][3], tTabuleiro tab, FILE *estatisticas);

int main(int argc, char * argv[]){

    if(argc <= 1){
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 0;
    }

    int rtn = 0;
    char comand[4] = "-gt", diretorio[1001];

    rtn = strcmp(argv[1], comand);
    if(!rtn){
        sprintf(diretorio, "%s", argv[2]);
        GeraTabuleiroAleatorio(diretorio);
        return 0;
    }

    tJogo jogo;
    jogo = InicializaJogo(argv);
    RealizaJogo(jogo, argv);
    GeraArquivoEstatisticas(jogo, argv);
    return 0;
}

void GeraTabuleiroAleatorio(char diretorio[1001]){

    FILE *tabuAle, *tabuAleLer;
    int orient = 0, c = 0, rtn = 0;
    char l = '\0';
    tTabuleiro tab;

    //pra mudar a seed de acordo com a hora do computador
    srand(time(NULL));

    /*funcao rand() gera numeros aleatorios com base na seed e
    com esse esquema de soma e divisao inteira eh possivel delimitar o intervalo
    em que esses numeros serao gerados*/

    do{
        tabuAle = fopen(diretorio, "w");

        orient = rand() % 1;
        c = 1 + rand() % 10;
        l = 97 + rand() % 10;
        fprintf(tabuAle, "Carrier;1;%d;%c%d\n", orient, l, c);

        orient = rand() % 2;
        c = 1 + rand() % 10;
        l = 97 + rand() % 10;
        fprintf(tabuAle, "Battleship;2;%d;%c%d\n", orient, l, c);

        orient = rand() % 1;
        c = 1 + rand() % 10;
        l = 97 + rand() % 10;
        fprintf(tabuAle, "Cruiser;3;%d;%c%d\n", orient, l, c);

        orient = rand() % 1;
        c = 1 + rand() % 10;
        l = 97 + rand() % 10;
        fprintf(tabuAle, "Submarine;4;%d;%c%d\n", orient, l, c);

        orient = rand() % 1;
        c = 1 + rand() % 10;
        l = 97 + rand() % 10;
        fprintf(tabuAle, "Destoyer;5;%d;%c%d", orient, l, c);

        fclose(tabuAle);

        tabuAleLer = fopen(diretorio, "r");
        tab = MontaTabuleiro(tabuAleLer);
        rtn = VerificaTabuleiro(tab);
        fclose(tabuAleLer);

    } while(!rtn);
}

tJogo InicializaJogo(char * argv[]){
    tJogo jogo;
    FILE *pFile1, *pFile2, *validTab, *inicializacao, *entrada, *saida;
    int i = 0, j = 0, rtnTab1 = 0, rtnTab2 = 0, EhConpativel = 0;
    char diretorio[1001];

    sprintf(diretorio, "%s/tabu_1.txt", argv[1]);//colocando o local do arquivo na variavel diretorio
    pFile1 = fopen(diretorio, "r");//abrindo o arquivo para leitura

    if(!pFile1){
        printf("Erro ao ler %s/tabu_1.txt\n", argv[1]);//Imprimindo o arquivo que tentou ler
        exit(0);
    } else{
        jogo.tabuleiro1 = MontaTabuleiro(pFile1);
        fclose(pFile1);
    }
    
    i = 0;
    j = 0;

    sprintf(diretorio, "%s/tabu_2.txt", argv[1]);
    pFile2 = fopen(diretorio, "r");

    if(!pFile2){
        printf("Erro ao ler %s/tabu_2.txt\n", argv[1]);//Imprimindo o arquivo que tentou ler
        exit(0);
    } else{
        jogo.tabuleiro2 = MontaTabuleiro(pFile2);
        fclose(pFile2);
    }

    rtnTab1 = VerificaTabuleiro(jogo.tabuleiro1);
    rtnTab2 = VerificaTabuleiro(jogo.tabuleiro2);

    //criando arquivo de validacao dos tabuleiros
    sprintf(diretorio, "%s/saida/validacao_tabuleiros.txt", argv[1]);
    validTab = fopen(diretorio, "w");

    if(!validTab){
        printf("ERRO: pasta de saida nao encontrada\n");
        exit(0);
    }

    if(rtnTab1){
        fprintf(validTab, "tabu_1.txt;valido\n");
    } else{
        fprintf(validTab, "tabu_1.txt;invalido\n");
    }
    if(rtnTab2){
        fprintf(validTab, "tabu_2.txt;valido");
    } else{
        fprintf(validTab, "tabu_2.txt;invalido");
    }
    if(rtnTab1 && rtnTab2){
        EhConpativel = VerificaCompativeis(jogo.tabuleiro1, jogo.tabuleiro2);
        if(EhConpativel){
            fprintf(validTab, "\nTabuleiros compativeis entre si");
            fclose(validTab);
        } else{
            fprintf(validTab, "\nTabuleiros incompativeis entre si");
            sprintf(diretorio, "%s/saidaLeitura.txt", argv[1]);
            saida = fopen(diretorio, "w");//cirando arquivo de saida em branco
            fclose(saida);
            fclose(validTab);
            exit(0);
        }
    } else{
        sprintf(diretorio, "%s/saidaLeitura.txt", argv[1]);
        saida = fopen(diretorio, "w");//cirando arquivo de saida em branco
        fclose(saida);
        fclose(validTab);
        exit(0);
    }
    
    //Pegando os nomes dos jogadores pela entrada padrao
    printf("Nome do Jogador 1:\n");
    jogo.jogador1 = LeNomeJogador();
    printf("Nome do Jogador 2:\n");
    jogo.jogador2 = LeNomeJogador();
    printf("\n");

    //criando arquivo de inicializacao dos tabuleiros
    sprintf(diretorio, "%s/saida/inicializacao.txt", argv[1]);
    inicializacao = fopen(diretorio, "w");
    fprintf(inicializacao, "%s\n", jogo.jogador1.nome);
    ImprimeTabuleiroNoArquivo(jogo.tabuleiro1, inicializacao);
    fprintf(inicializacao, "\n\n");
    fprintf(inicializacao, "%s\n", jogo.jogador2.nome);
    ImprimeTabuleiroNoArquivo(jogo.tabuleiro2, inicializacao);
    fclose(inicializacao);
    
    return jogo;
}

tTabuleiro MontaTabuleiro(FILE *file){

    tTabuleiro tab;
    int i = 0, j = 0;

    tab.qtdNavios = 0;

    while(!feof(file)){
        tab.navio[tab.qtdNavios] = LeNavio(file);
        tab.qtdNavios++;
        fscanf(file, "%*[^A-Z]");//jogando tudo fora ate o proximo navil
    }

    GeraMatrizSoComAgua(tab.matriz);
    tab = InsereNaviosNoTabuleiro(tab);

    return tab;
}

tNavio LeNavio(FILE *file){
    tNavio navio;
    fscanf(file, "%[^;];", navio.tipo);
    navio.tamanho = VerificaTamanhoDoNavil(navio.tipo);
    fscanf(file, "%d;", &navio.id);
    fscanf(file, "%d;", &navio.orientacao);
    fscanf(file, "%c%d", &navio.linha, &navio.coluna);
    navio.marcacao = 0;
    return navio;
}

int VerificaTamanhoDoNavil(char tipo[11]){

    int i = 0;

    if(tipo[i] == 'B'){//Battleship
        return 4;
    } else if(tipo[i] == 'S'){//Submarine
        return 3;
    } else if(tipo[i] == 'D'){//Destroyer
        return 2;
    } else if(tipo[i] == 'C'){
        i++;
        if(tipo[i] == 'a'){//Carrier
            return 5;
        } else if(tipo[i] == 'r'){//Cruiser
            return 3;
        }
    }
}

void GeraMatrizSoComAgua(char matriz[10][20]){

    int i, j;

    for(i = 0; i < 10; i++){
        for(j = 0; j < 19; j++){
            matriz[i][j] = 'o';
            j++;
            if(j != 19){
                matriz[i][j] = ' ';
            }
            
            
        }
    }
}

tTabuleiro InsereNaviosNoTabuleiro(tTabuleiro tab){

    int i = 0, j = 0, x = 0, y = 0, colunaCorresp = 0;
    //x: conta navios
    //y: conta tamanho

    while(x < tab.qtdNavios){

        //trocando pra coluna inicial correspondente da minha matriz
        colunaCorresp = TransformaNaColunaCorresp(tab.navio[x].coluna);

        for(i = 0; i < 10; i++){
            for(j = 0; j < 20; j += 2){
                if((EhLinhaInicial(i, tab.navio[x].linha)) && (EhColunaInicial(j, colunaCorresp))){
                    tab.matriz[i][j] = 'X';
                    while(tab.navio[x].tamanho - 1){
                        if(tab.navio[x].orientacao == 1 && j+2 >= 0 && j+2 <= 18){//pra impedir acesso indevido de memoria
                            tab.matriz[i][j+2] = 'X';
                            j += 2;
                        } else if(tab.navio[x].orientacao == 0 && i+1 >= 0 && i+1 <= 9){//pra impedir acesso indevido de memoria
                            tab.matriz[i+1][j] = 'X';
                            i++;
                        }
                        tab.navio[x].tamanho -= 1;
                        y++;
                    }
                    //retornando ao tamanho certo
                    tab.navio[x].tamanho = tab.navio[x].tamanho + y;
                    y = 0;
                }
            
            }

        }
        x++;
    }
    return tab;
}

int TransformaNaColunaCorresp(int coluna){
    if(coluna == 1){
        coluna = 0;
    } else{
        coluna = 2 + 2*(coluna-2);
    }
    return coluna;
}

int EhLinhaInicial(int i, char linha){
    int corresp;
    corresp = linha - 96;//transformando por ex.: 'a' em 1
    if((i+1) == corresp){
        return 1;
    } else{
        return 0;
    }
}

int EhColunaInicial(int j, int coluna){
    if(j == coluna){
        return 1;
    } else{
        return 0;
    }
}

int VerificaTabuleiro(tTabuleiro tab){

    int encosta = 0, temNavio = 0, temFora = 0, i = 0, j = 0, x = 0;//x: conta navios

    //Verificando se tem navio no tabuleiro
    for(i = 0; i < 10; i++){
        for(j = 0; j < 20; j++){
            if(tab.matriz[i][j] == 'X'){
                temNavio = 1;
            }
        }
    }

    //Verificando se um navio encosta no outro
    encosta = VerificaPosicaoInicialIgual(tab);
    while(x < tab.qtdNavios){
        if(VerificaNavioEncostando(tab.navio[x], tab)){
            encosta = 1;
        }
        x++;
    } 

    x = 0;

    //verificando se algum navio ultrapassa os limetes do tabulerio
    while(x < tab.qtdNavios){
        if(VerificaNavioFora(tab.navio[x])){
            temFora = 1;
        }
        x++;
    }
    
    if(!(temNavio)|| encosta || temFora){
        return 0;
    } else{
        return 1;
    }
}

int VerificaPosicaoInicialIgual(tTabuleiro tab){

    int ehIgual = 0, x = 1, i = 0;

    while(x < tab.qtdNavios){
        for(i = x-1; i >= 0; i--){
            if(tab.navio[x].id != tab.navio[i].id){//pra nao ser o mesmo navio
                if(tab.navio[x].linha == tab.navio[i].linha && tab.navio[x].coluna == tab.navio[i].coluna){
                    ehIgual = 1;
                }
            }
        }
        x++;
    }

    return ehIgual;
}

int VerificaNavioEncostando(tNavio navio, tTabuleiro tab){

    int i = 0, j = 0, x = 0, encosta = 0, y = 1;
    //x: conta navios 
    //y: conta o tamanho

    i = navio.linha-'a';//a tem que virar 0
    j = TransformaNaColunaCorresp(navio.coluna);

    if(navio.orientacao == 1){//horizontal

        while((navio.tamanho+1) - y){
            //pra verificar quando eh a primeira posicao do navio
            if(y == 1){
                if(tab.matriz[i][j-2] == 'X' && j > 0){//um atras
                    encosta = 1;
                }
                if(tab.matriz[i+1][j-2] == 'X' && j > 0 && i < 10){//diagonal de baixo
                    encosta = 1;
                }
                if(tab.matriz[i-1][j-2] == 'X' && j > 0 && i > 0){//diagonal de cima
                    encosta = 1;
                }

            //pra verificar quando eh a ultima posicao do navio
            } else if(y == navio.tamanho){
                if(tab.matriz[i][j+2] == 'X' && j <= 16){//um a frente
                    encosta = 1;
                }
                if(tab.matriz[i+1][j+2] == 'X' && j <= 16 && i < 10){//diagonal de baixo
                    encosta = 1;
                }
                if(tab.matriz[i-1][j+2] == 'X' && j <= 16 && i > 0){//diagonal de cima
                    encosta = 1;
                }
                       
            //pra verificar no meio do navio
            } else{
                if(tab.matriz[i+1][j] == 'X' && i < 10){
                    encosta = 1;
                }
                if(tab.matriz[i-1][j] == 'X' && i > 0){
                    encosta = 1;
                }
            }
            j += 2;
            y++;
        }


    } else if(navio.orientacao == 0){//vertical

        while((navio.tamanho+1) - y){
            //pra verificar quando eh a primeira posicao do navio
            if(y == 1){
                if(tab.matriz[i-1][j] == 'X' && i > 0){//um atras
                    encosta = 1;
                }
                if(tab.matriz[i-1][j+2] == 'X' && i > 0 && j <= 16){//diagonal da direira
                    encosta = 1;
                }
                if(tab.matriz[i-1][j-2] == 'X' && i > 0 && j > 0){//diagonal da esquerda
                    encosta = 1;
                }

            //pra verificar quando eh a ultima posicao do navio
            } else if(y == navio.tamanho){
                if(tab.matriz[i+1][j] == 'X' && i < 10){//um a frente
                    encosta = 1;
                }
                if(tab.matriz[i+1][j+2] == 'X' && i < 10 && j <= 16){//diagonal da direita
                    encosta = 1;
                }
                if(tab.matriz[i+1][j-2] == 'X' && i < 10 && j > 0){//diagonal da esquerda
                    encosta = 1;
                }

            //pra verificar quando ta no meio do navio
            } else{
                if(tab.matriz[i][j+2] == 'X' && j <= 16){
                    encosta = 1;
                }
                if(tab.matriz[i][j-2] == 'X' && j > 0){
                    encosta = 1;
                }
            }
            i++;
            y++;
        }
    }

    return encosta;
}

int VerificaNavioFora(tNavio navio){
    
    int temFora = 0;

    if(navio.linha > 'j' || navio.linha < 'a'){
        temFora = 1;
    } else if(navio.coluna > 10 || navio.coluna < 1){
        temFora = 1;
    }

    /*Se a posicao inicial + o tamanho total der maior que os
    limites do tabuleiro eh pq tem fora*/
    
    if(navio.orientacao == 1){//horizontal
        if(navio.coluna + (navio.tamanho-1) > 10){
            temFora = 1;
        }
    } else if(navio.orientacao == 0){//vertical
        if((navio.linha-96) + (navio.tamanho-1) > 10){
            temFora = 1;
        }
    }
    return temFora;
}

int VerificaCompativeis(tTabuleiro tab1, tTabuleiro tab2){
    //os dois tem que ter os mesmos navios

    int i = 0;
    int qtdCarri1 = 0, qtdBat1 = 0, qtdCru1 = 0, qtdSub1 = 0, qtdDes1 = 0;
    int qtdCarri2 = 0, qtdBat2 = 0, qtdCru2 = 0, qtdSub2 = 0, qtdDes2 = 0;

    if(tab1.qtdNavios != tab2.qtdNavios){
        return 0;
    }

    while(i < tab1.qtdNavios){
        if(tab1.navio[i].tamanho == 5){
            qtdCarri1++;
        } else if(tab1.navio[i].tamanho == 4){
            qtdBat1++;
        } else if(tab1.navio[i].tamanho == 2){
            qtdDes1++;
        } else if(tab1.navio[i].tamanho == 3){
            if(EhCruiser(tab1.navio[i].tipo)){
                qtdCru1++;
            } else{
                qtdSub1++;
            }
        }
        i++;     
    }

    i = 0;

    while(i < tab2.qtdNavios){
        if(tab2.navio[i].tamanho == 5){
            qtdCarri2++;
        } else if(tab2.navio[i].tamanho == 4){
            qtdBat2++;
        } else if(tab2.navio[i].tamanho == 2){
            qtdDes2++;
        } else if(tab2.navio[i].tamanho == 3){
            if(EhCruiser(tab2.navio[i].tipo)){
                qtdCru2++;
            } else{
                qtdSub2++;
            }
        }
        i++;     
    }
    
    if(qtdCarri1 == qtdCarri2 && qtdBat1 == qtdBat2 && qtdDes1 == qtdDes2 && qtdCru1 == qtdCru2 && qtdSub1 == qtdSub2){
        return 1;
    } else{
        return 0;
    }
}

int EhCruiser(char tipo[11]){
    if(tipo[0] == 'C'){//Crusier
        return 1;
    } else{
        return 0;//Submarine
    }
}

tJogador LeNomeJogador(){
    tJogador jogador;
    scanf("%s", jogador.nome);
    return jogador;
}

void ImprimeTabuleiroNoArquivo(tTabuleiro tab, FILE *inicializacao){

    int i = 0, j = 0;

    for(i = 0; i < 10; i++){
        for(j = 0; j < 19; j++){
            fprintf(inicializacao, "%c", tab.matriz[i][j]);
        }
        if(i != 9){
            fprintf(inicializacao, "\n");
        }
    }
}

void RealizaJogo(tJogo jogo, char * argv[]){

    tJogada jogada;
    char matrizJogador1[10][20], matrizJogador2[10][20];//cheia de pontos finais
    FILE *saida, *resultado;
    int ganhou = 0, z = 0;
    char diretorio[1001];

    //abrindo o arquivo de saida
    sprintf(diretorio, "%s/saidaLeitura.txt", argv[1]);
    saida = fopen(diretorio, "w");

    fprintf(saida, "Nome do Jogador 1:\n");
    fprintf(saida, "Nome do Jogador 2:\n\n");

    GeraMatrizSoComPonto(matrizJogador1);
    GeraMatrizSoComPonto(matrizJogador2);
    //vao mudar aqui fora tambem

    while(1){//loop infinito ate alguem ganhar (break)

        printf("Jogada de %s:\n", jogo.jogador1.nome);
        fprintf(saida, "Jogada de %s:\n\n", jogo.jogador1.nome);
        jogada = LeJogada(saida, matrizJogador2);
        while(EhJogadaInvalida(jogada, matrizJogador2)){
            printf(" Jogue novamente %s:\n", jogo.jogador1.nome);
            fprintf(saida, " Jogue novamente %s:\n\n", jogo.jogador1.nome);
            jogada = LeJogada(saida, matrizJogador2);
        }

        //marcando a posicao da jogada na marizJogador
        ganhou = MarcaJogada(jogada, matrizJogador2, jogo.tabuleiro2, saida);

        //Imprimindo o que aconteceu
        printf("Tabuleiro atual de %s apos a jogada de %s\n", jogo.jogador2.nome, jogo.jogador1.nome);
        fprintf(saida, "Tabuleiro atual de %s apos a jogada de %s\n", jogo.jogador2.nome, jogo.jogador1.nome);
        ImprimeMatrizJogador(matrizJogador2, saida);

        if(ganhou){
            printf("Jogada de %s:\n", jogo.jogador2.nome);
            fprintf(saida, "Jogada de %s:\n\n", jogo.jogador2.nome);
            jogada = LeJogada(saida, matrizJogador1);
            while(EhJogadaInvalida(jogada, matrizJogador1)){
                printf(" Jogue novamente %s:\n", jogo.jogador2.nome);
                fprintf(saida, " Jogue novamente %s:\n\n", jogo.jogador2.nome);
                jogada = LeJogada(saida, matrizJogador1);
            }

            //marcando a posicao da jogada na marizJogador
            ganhou = MarcaJogada(jogada, matrizJogador1, jogo.tabuleiro1, saida);

            //Imprimindo o que aconteceu
            printf("Tabuleiro atual de %s apos a jogada de %s\n", jogo.jogador1.nome, jogo.jogador2.nome);
            fprintf(saida, "Tabuleiro atual de %s apos a jogada de %s\n", jogo.jogador1.nome, jogo.jogador2.nome);
            ImprimeMatrizJogador(matrizJogador1, saida);

            if(ganhou){
                printf("Empate\n");
                fprintf(saida, "Empate\n");
                break;
            } else{
                printf("Vencedor: %s\n", jogo.jogador1.nome);
                fprintf(saida, "Vencedor: %s\n", jogo.jogador1.nome);
                break;
            }

        } else{
            printf("Jogada de %s:\n", jogo.jogador2.nome);
            fprintf(saida, "Jogada de %s:\n\n", jogo.jogador2.nome);
            jogada = LeJogada(saida, matrizJogador1);
            while(EhJogadaInvalida(jogada, matrizJogador1)){
                printf(" Jogue novamente %s:\n", jogo.jogador2.nome);
                fprintf(saida, " Jogue novamente %s:\n\n", jogo.jogador2.nome);
                jogada = LeJogada(saida, matrizJogador1);
            }

            ganhou = MarcaJogada(jogada, matrizJogador1, jogo.tabuleiro1, saida);
            //marcando a posicao da jogada na marizJogador

            //Imprimindo o que aconteceu
            printf("Tabuleiro atual de %s apos a jogada de %s\n", jogo.jogador1.nome, jogo.jogador2.nome);
            fprintf(saida, "Tabuleiro atual de %s apos a jogada de %s\n", jogo.jogador1.nome, jogo.jogador2.nome);
            ImprimeMatrizJogador(matrizJogador1, saida);

            if(ganhou){
                printf("Vencedor: %s\n", jogo.jogador2.nome);
                fprintf(saida, "Vencedor: %s\n", jogo.jogador2.nome);
                break;
            }
        }
    }

    //fechando arquivos abertos
    fclose(saida);


    //Criando arquivo resultado.txt a partir de saidaLeitura.txt
    sprintf(diretorio, "%s/saida/resultado.txt", argv[1]);
    resultado = fopen(diretorio, "w");
    fprintf(resultado, "%s", jogo.jogador1.nome);
    GeraJogadas1Resultado(resultado, saida, argv, jogo.tabuleiro2);
    fprintf(resultado, "\n\n%s", jogo.jogador2.nome);
    GeraJogadas2Resultado(resultado, saida, argv, jogo.tabuleiro1);
    fclose(resultado);
}

void GeraMatrizSoComPonto(char matrizJogadas[10][20]){

    int i, j;

    for(i = 0; i < 10; i++){
        for(j = 0; j < 19; j++){
            matrizJogadas[i][j] = '.';
            j++;
            if(j != 19){
                matrizJogadas[i][j] = ' ';
            }
        }
    }
}

tJogada LeJogada(FILE *saida, char matrizJogador[10][20]){
    tJogada jogada;
    jogada.linha = 'k';//vai retornar esses valores caso seja invalido
    jogada.coluna = 21;
    char vet[10];
    scanf("%*c");//jogando fora o '\n'
    scanf("%s", vet);

    if(vet[1] == '1'){
        if(vet[2] == '0' && vet[3] == '\0'){
            jogada.linha = vet[0];
            jogada.coluna = 10;
            if(EhJogadaInvalida(jogada, matrizJogador)){
                printf("\n%s:Jogada invalida!", vet);
                fprintf(saida, "%s:Jogada invalida!", vet);
            }
            return jogada;
        } else if(vet[2] == '\0'){
            jogada.linha = vet[0];
            jogada.coluna = 1;
            if(EhJogadaInvalida(jogada, matrizJogador)){
                printf("\n%s:Jogada invalida!", vet);
                fprintf(saida, "%s:Jogada invalida!", vet);
            }
            return jogada;
        } else{
            printf("\n%s:Jogada invalida!", vet);
            fprintf(saida, "%s:Jogada invalida!", vet);
            return jogada;
        }

    } else if(vet[2] != '\0'){
        printf("\n%s:Jogada invalida!", vet);
        fprintf(saida, "%s:Jogada invalida!", vet);
        return jogada;

    } else{
        jogada.linha = vet[0];
        jogada.coluna = vet[1] - '0';//transformando carac em numero
        if(EhJogadaInvalida(jogada, matrizJogador)){
            printf("\n%s:Jogada invalida!", vet);
            fprintf(saida, "%s:Jogada invalida!", vet);
        }
        return jogada;
    }
}

int EhJogadaInvalida(tJogada jogada, char matrizJogador[10][20]){
    //nao precisa imprimir nada pois ja imprimiu tudo no LeJogada

    int l, c;

    c = TransformaNaColunaCorresp(jogada.coluna);
    l = jogada.linha - 'a';

    if(jogada.coluna > 10 || jogada.coluna < 1){
        return 1;
    } else if(jogada.linha < 'a' || jogada.linha > 'j'){
        return 1;
    } else if(matrizJogador[l][c] == 'X' || matrizJogador[l][c] == 'o'){//jogada repitida
        return 1;
    } else{
        return 0;
    }
}

int MarcaJogada(tJogada jogada, char matrizJogadas[10][20], tTabuleiro tab, FILE *saida){

    int linhaDaMatriz, colunaDaMatriz;

    linhaDaMatriz = jogada.linha - 'a';//o 'a' tem que virar 0 aqui
    colunaDaMatriz = TransformaNaColunaCorresp(jogada.coluna);

    if(tab.matriz[linhaDaMatriz][colunaDaMatriz] == 'X'){
        matrizJogadas[linhaDaMatriz][colunaDaMatriz] = 'X';
        if(Afundou(matrizJogadas, tab.matriz, linhaDaMatriz, colunaDaMatriz)){
            printf("\n%c%d:Afundou", jogada.linha, jogada.coluna);
            fprintf(saida, "%c%d:Afundou", jogada.linha, jogada.coluna);
            ImprimeNomeNavioAfundado(tab, jogada.linha, jogada.coluna, saida);
            if(VerificaGanhou(matrizJogadas, tab)){
                return 1;
            } else{
                return 0;
            }
        } else{
            printf("\n%c%d:Tiro!\n\n", jogada.linha, jogada.coluna);
            fprintf(saida, "%c%d:Tiro!\n\n", jogada.linha, jogada.coluna);
            return 0;
        }

    } else if(tab.matriz[linhaDaMatriz][colunaDaMatriz] == 'o'){
        printf("\n%c%d:Agua\n\n", jogada.linha, jogada.coluna);
        fprintf(saida, "%c%d:Agua\n\n", jogada.linha, jogada.coluna);
        matrizJogadas[linhaDaMatriz][colunaDaMatriz] = 'o';
        return 0;
    }
}

void ImprimeMatrizJogador(char matrizJogador[10][20], FILE *saida){
    int i = 0, j = 0;
    for(i = 0; i < 10; i++){
        for(j = 0; j < 19; j++){
            printf("%c", matrizJogador[i][j]);
            fprintf(saida, "%c", matrizJogador[i][j]);
        }
        printf("\n");
        fprintf(saida, "\n");
    }
    printf("\n");
    fprintf(saida, "\n");
}

int Afundou(char matrizJogadas[10][20], char matrizTabulerio[10][20], int lMat, int cMat){
    //se tiver uma linha/coluna iguais, é porque afundou

    int x = 1;
    int afundou = 1;

    if(matrizTabulerio[lMat+x][cMat] == 'X' || matrizTabulerio[lMat-x][cMat] == 'X'){//do tipo vertical
        //analisando navio abaixo
        while(matrizTabulerio[lMat+x][cMat] == 'X'){
            if(matrizTabulerio[lMat+x][cMat] != matrizJogadas[lMat+x][cMat]){
                afundou = 0;
                break;
            }
            x++;
        }
        x = 1;
        //analisando navio acima
        while(matrizTabulerio[lMat-x][cMat] == 'X'){
            if(matrizTabulerio[lMat-x][cMat] != matrizJogadas[lMat-x][cMat]){
                afundou = 0;
                break;
            }
            x++;
        }

    } else{//do tipo horizontal
        x = 2;
        //analisando navio a direita
        while(matrizTabulerio[lMat][cMat+x] == 'X'){
            if(matrizTabulerio[lMat][cMat+x] != matrizJogadas[lMat][cMat+x]){
                afundou = 0;
                break;
            }
            x += 2;
        }
        x = 2;
        //analisando navio a esquerda
        while(matrizTabulerio[lMat][cMat-x] == 'X'){
            if(matrizTabulerio[lMat][cMat-x] != matrizJogadas[lMat][cMat-x]){
                afundou = 0;
                break;
            }
            x += 2;
        }
    }

    return afundou;
}

void ImprimeNomeNavioAfundado(tTabuleiro tab, char l, int c, FILE *saida){

    /*A partir da posicao da jogada responsavel por afundar, percorrer por todos os pontos do navio,
    quando um desses pontos bater com a posicao original de um navio, imprimir o nome desse*/

    int linha = 0, coluna = 0, x = 1, y = 0;//y: conta navios

    linha = l-'a';//a tem que ser 0
    coluna = TransformaNaColunaCorresp(c);

    if(tab.matriz[linha+x][coluna] == 'X' || tab.matriz[linha-x][coluna] == 'X'){//do tipo vertical

        //analisando a propria posicao
        y = 0;
        while(y < tab.qtdNavios){//passando por todos os navios
            if(l == tab.navio[y].linha && c == tab.navio[y].coluna){
                printf(" %s\n\n", tab.navio[y].tipo);
                fprintf(saida, " %s\n\n", tab.navio[y].tipo);
                break;
            }
            y++;
        }

        //analisando navio abaixo
        while(tab.matriz[linha+x][coluna] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l+x == tab.navio[y].linha && c == tab.navio[y].coluna){
                    printf(" %s\n\n", tab.navio[y].tipo);
                    fprintf(saida, " %s\n\n", tab.navio[y].tipo);
                    break;
                }
                y++;
            }
            x++;
        }

        y = 0;
        x = 1;

        //analisando navio acima
        while(tab.matriz[linha-x][coluna] == 'X'){//passando por todas as posicoes do navio
            y = 0;
           while(y < tab.qtdNavios){//passando por todos os navios
                if(l-x == tab.navio[y].linha && c == tab.navio[y].coluna){
                    printf(" %s\n\n", tab.navio[y].tipo);
                    fprintf(saida, " %s\n\n", tab.navio[y].tipo);
                    break;
                }
                y++;
            }
            x++;
        }
        y = 0;

    } else{//do tipo horizontal
        //Analisando a propria posicao
        y = 0;
        while(y < tab.qtdNavios){//passando por todos os navios
            if(l == tab.navio[y].linha && c+(x/2) == tab.navio[y].coluna){
                printf(" %s\n\n", tab.navio[y].tipo);
                fprintf(saida, " %s\n\n", tab.navio[y].tipo);
                break;
            }
            y++;
        }

        x = 2;
        //analisando navio a direita
        while(tab.matriz[linha][coluna+x] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l == tab.navio[y].linha && c+(x/2) == tab.navio[y].coluna){
                    printf(" %s\n\n", tab.navio[y].tipo);
                    fprintf(saida, " %s\n\n", tab.navio[y].tipo);
                    break;
                }
                y++;
            }
            x += 2;
        }

        y = 0;
        x = 2;

        //analisando navio a esquerda
        while(tab.matriz[linha][coluna-x] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l == tab.navio[y].linha && c-(x/2) == tab.navio[y].coluna){
                    printf(" %s\n\n", tab.navio[y].tipo);
                    fprintf(saida, " %s\n\n", tab.navio[y].tipo);
                    break;
                }
                y++;
            }
            x += 2;
        }
    }
}

int VerificaGanhou(char matrizJogadas[10][20], tTabuleiro tab){
    //Se os dois tiverem a mesma quantidade de X e pq ganhou

    int qtdX1 = 0, qtdX2 = 0, i = 0, j = 0;

    for(i = 0; i < 10; i++){
        for(j = 0; j < 19; j++){
            if(tab.matriz[i][j] == 'X'){
                qtdX1++;
            }
        }
    }

    for(i = 0; i < 10; i++){
        for(j = 0; j < 19; j++){
            if(matrizJogadas[i][j] == 'X'){
                qtdX2++;
            }
        }
    }

    if(qtdX1 == qtdX2){
        return 1;
    } else{
        return 0;
    }
}

void GeraJogadas1Resultado(FILE *resultado, FILE *saida, char *argv[], tTabuleiro tab){

    char vet[200];//vetor que vai ser as jogadas
    vet[2] = 'Z';//pra saber se foi lido
    int i = 1, c = 0, l = 0;
    char acao[8];//Tiro, Agua ou Afundou
    char diretorio[1001], matrizFake1[10][20], matrizFake2[10][20], confere;
    tJogada jogada;

    GeraMatrizSoComPonto(matrizFake1);
    GeraMatrizSoComPonto(matrizFake2);

    //abrindo saidaLeitura.txt para leitura agora
    sprintf(diretorio, "%s/saidaLeitura.txt", argv[1]);
    saida = fopen(diretorio, "r");

    while(!feof(saida)){

        fscanf(saida, "%3s", vet);   

        if(EhLetra(vet[0]) && EhNum(vet[1])){

            if(vet[2] == ':'){
                jogada.coluna = vet[1]-'0';//pra passar como argumento da funcao
                jogada.linha = vet[0];

                if(!(EhJogadaInvalida(jogada, matrizFake1)) && (EhImpar(i))){//primeiro jogador

                    fscanf(saida, "%s", acao);//o que aconteceu na jogada

                    if(acao[0] == 'T' || acao[1] == 'f'){//Tiro/Afundou
                        fprintf(resultado, "\n%.2s - Tiro", vet);//imprimindo a jogada e o que aconteceu
                        ImprimeNomeEIDNavio(tab, jogada.linha, jogada.coluna, resultado);
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake1[l][c] = 'X';
                    
                    } else{//Agua
                        fprintf(resultado, "\n%.2s - %s", vet, acao);//imprimindo a jogada e o que aconteceu
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake1[l][c] = 'o';
                    }
                    i++;

                } else if(!(EhImpar(i))){//segundo jogador
                    if(!(EhJogadaInvalida(jogada, matrizFake2))){
                        i++;
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake2[l][c] = 'X';//aqui nao importa se eh X ou o
                    } else{
                        fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                        fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                        continue;
                    }

                }else {//Jogada invalida, volta pro inicio
                    fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                    fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                    continue;
                }
                    
            } else if(vet[1] == '1' && vet[2] == '0'){//i10
                jogada.coluna = 10;
                jogada.linha = vet[0];

                fscanf(saida, "%c", &confere);
                if(confere != ':'){//jogada invalida
                    fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                    fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                    continue;
                }


                if(!(EhJogadaInvalida(jogada, matrizFake1)) && (EhImpar(i))){//primeiro jogador
                    
                    fscanf(saida, "%s", acao);

                    if(acao[0] == 'T' || acao[1] == 'f'){//Tiro!/Afundou
                        fprintf(resultado, "\n%s - Tiro", vet);//imprimindo a jogada e o que aconteceu
                        ImprimeNomeEIDNavio(tab, jogada.linha, jogada.coluna, resultado);
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake1[l][c] = 'X';
                
                    } else{//Agua
                        fprintf(resultado, "\n%s - %s", vet, acao);//imprimindo a jogada e o que aconteceu
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake1[l][c] = 'o';
                    }
                    i++;

                } else if(!(EhImpar(i))){//segundo jogador

                    if(!(EhJogadaInvalida(jogada, matrizFake2))){
                        i++;
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake2[l][c] = 'X';//aqui nao importa se eh X ou o
                    } else{
                        fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                        fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                        continue;
                    }

                }else{//jogada invalida, voltar pro incio
                    fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                    fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                    continue;
                }

            } else{//jogada invalida, voltar pro incio
                fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                continue;
            }
                      
        } else{//jogada invalida
            fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
            fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
        }
    }

    fclose(saida);
}

void GeraJogadas2Resultado(FILE *resultado, FILE *saida, char *argv[], tTabuleiro tab){
    //mesma coisa que o 1 so que com i par (segundo jogador)

    char vet[200];//vetor que vai ser as jogadas
    vet[2] = 'Z';//pra saber se foi lido
    int i = 1, c = 0, l = 0;
    char acao[8];//Tiro, Agua ou Afundou
    char nome[17], diretorio[1001], matrizFake1[10][20], matrizFake2[10][20], confere;
    tJogada jogada;

    GeraMatrizSoComPonto(matrizFake1);
    GeraMatrizSoComPonto(matrizFake2);

    //abrindo saidaLeitura.txt para leitura agora
    sprintf(diretorio, "%s/saidaLeitura.txt", argv[1]);
    saida = fopen(diretorio, "r");

    while(!feof(saida)){

        fscanf(saida, "%3s", vet);

        if(EhLetra(vet[0]) && EhNum(vet[1])){

            if(vet[2] == ':'){//jogada simples
                jogada.coluna = vet[1]-'0';
                jogada.linha = vet[0];

                if(!(EhJogadaInvalida(jogada, matrizFake2)) && !(EhImpar(i))){//segundo jogador
                    
                    fscanf(saida, "%s", acao);//o que aconteceu na jogada

                    if(acao[0] == 'T' || acao[1] == 'f'){//Tiro!/Afundou
                        fprintf(resultado, "\n%.2s - Tiro", vet);//imprimindo a jogada e o que aconteceu
                        ImprimeNomeEIDNavio(tab, jogada.linha, jogada.coluna, resultado);
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake2[l][c] = 'X';

                    } else{//Agua
                        fprintf(resultado, "\n%.2s - %s", vet, acao);//imprimindo a jogada e o que aconteceu
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake2[l][c] = 'o';
                    }
                    i++;

                } else if((EhImpar(i))){//primeiro jogador
                    if(!(EhJogadaInvalida(jogada, matrizFake1))){
                        i++;
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake1[l][c] = 'X';//aqui nao importa se eh X ou o
                    } else{
                        fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                        fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                        continue;
                    }

                }else{//Jogada invalida, volta pro inicio
                    fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                    fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                    continue;
                }

            } else if(vet[1] == '1' && vet[2] == '0'){//i10
                jogada.coluna = 10;
                jogada.linha = vet[0];

                fscanf(saida, "%c", &confere);
                if(confere != ':'){//jogada invalida
                    fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                    fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                    continue;
                }

                if(!(EhJogadaInvalida(jogada, matrizFake2)) && !(EhImpar(i))){//segundo jogador
                    
                    fscanf(saida, "%s", acao);//o que aconteceu na jogada

                    if(acao[0] == 'T' || acao[1] == 'f'){//Tiro!(imprimir sem exclamacao)
                        fprintf(resultado, "\n%s - Tiro", vet);//imprimindo a jogada e o que aconteceu
                        ImprimeNomeEIDNavio(tab, jogada.linha, jogada.coluna, resultado);
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake2[l][c] = 'X';
                        
                    } else{
                        fprintf(resultado, "\n%s - %s", vet, acao);//imprimindo a jogada e o que aconteceu
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake2[l][c] = 'o';
                    }
                    i++;

                } else if((EhImpar(i))){//primeiro jogador
                    if(!(EhJogadaInvalida(jogada, matrizFake1))){
                        i++;
                        c = TransformaNaColunaCorresp(jogada.coluna);
                        l = jogada.linha - 'a';
                        matrizFake1[l][c] = 'X';//aqui nao importa se eh X ou o
                    } else{
                        fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                        fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                        continue;
                    }

                }else{//Jogada invalida, volta pro inicio
                    fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                    fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                    continue;
                }

            } else{//jogada invalida, voltar pro incio
                fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
                fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
                continue;
            }


        } else if(vet[0] == 'V' && vet[1] == 'e' && vet[2] == 'n'){//Vencedor
            fscanf(saida, "%*[^:]");//jogando fora tudo da linha
            fscanf(saida, ": %s", nome);/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
            fprintf(resultado, "\n\nVencedor: %s", nome);//imprimindo o nome do vencedor
            break;//depois que eu coloque \n no vencedor/empate do saida, comecou a imprimir duas vezes aqui
            
        } else if(vet[0] == 'E' && vet[1] == 'm' && vet[2] == 'p'){//Empate
            fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
            fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
            fprintf(resultado, "\n\nEmpate");
            break;//depois que eu coloque \n no vencedor/empate do saida, comecou a imprimir duas vezes aqui
            
        } else{//jogada invalida
            fscanf(saida, "%*[^\n]");//jogando fora tudo da linha
            fscanf(saida, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
        }
    }

    fclose(saida);
}

int EhLetra(char c){
    if(c >= 'a' && c <= 'j'){
        return 1;
    } else{
        return 0;
    }
}

int EhNum(char c){
    if(c >= '1' && c <= '9'){
        return 1;
    } else{
        return 0;
    }
}

int EhImpar(int i){
    if(i%2 == 0){
        return 0;
    } else{
        return 1;
    }
}

void ImprimeNomeEIDNavio(tTabuleiro tab, char l, int c, FILE *resultado){

    /*Mesma coisa que o ImprimeNomeNavioAfundado() porem imprimindo outras coisas e em outro arquivo*/

    /*A partir da posicao da jogada responsavel por afundar, percorrer por todos os pontos do navio,
    quando um desses pontos bater com a posicao original de um navio, imprimir o nome desse*/
    int linha = 0, coluna = 0, x = 1, y = 0;//y: conta navios

    linha = l-'a';//a tem que ser 0
    coluna = TransformaNaColunaCorresp(c);

    if(tab.matriz[linha+x][coluna] == 'X' || tab.matriz[linha-x][coluna] == 'X'){//do tipo vertical

        //analisando a propria posicao
        y = 0;
        while(y < tab.qtdNavios){//passando por todos os navios
            if(l == tab.navio[y].linha && c == tab.navio[y].coluna){
                fprintf(resultado, " - %s - ID %.2d", tab.navio[y].tipo, tab.navio[y].id);
                break;
            }
            y++;
        }

        //analisando navio abaixo
        while(tab.matriz[linha+x][coluna] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l+x == tab.navio[y].linha && c == tab.navio[y].coluna){
                    fprintf(resultado, " - %s - ID %.2d", tab.navio[y].tipo, tab.navio[y].id);
                    break;
                }
                y++;
            }
            x++;
        }

        y = 0;
        x = 1;

        //analisando navio acima
        while(tab.matriz[linha-x][coluna] == 'X'){//passando por todas as posicoes do navio
            y = 0;
           while(y < tab.qtdNavios){//passando por todos os navios
                if(l-x == tab.navio[y].linha && c == tab.navio[y].coluna){
                    fprintf(resultado, " - %s - ID %.2d", tab.navio[y].tipo, tab.navio[y].id);
                    break;
                }
                y++;
            }
            x++;
        }
        y = 0;

    } else{//do tipo horizontal
        //analisando a propria posicao
        y = 0;
        while(y < tab.qtdNavios){//passando por todos os navios
            if(l == tab.navio[y].linha && c == tab.navio[y].coluna){
                fprintf(resultado, " - %s - ID %.2d", tab.navio[y].tipo, tab.navio[y].id);
                break;
            }
            y++;
        }

        x = 2;
        //analisando navio a direita
        while(tab.matriz[linha][coluna+x] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l == tab.navio[y].linha && c+(x/2) == tab.navio[y].coluna){
                    fprintf(resultado, " - %s - ID %.2d", tab.navio[y].tipo, tab.navio[y].id);
                    break;
                }
                y++;
            }
            x += 2;
        }

        y = 0;
        x = 2;

        //analisando navio a esquerda
        while(tab.matriz[linha][coluna-x] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l == tab.navio[y].linha && c-(x/2) == tab.navio[y].coluna){
                    fprintf(resultado, " - %s - ID %.2d", tab.navio[y].tipo, tab.navio[y].id);
                    break;
                }
                y++;
            }
            x += 2;
        }
    }
}

void GeraArquivoEstatisticas(tJogo jogo, char * argv[]){

    /*Como so se deve analisar jogadas validas e nao repetidas eu posso 
    olhar no resultado.txt mesmo*/

    char diretorio[1001], vet[200];
    FILE *estatisticas, *resultado;
    int qtdTirosAcertados = 0, qtdTirosErrados = 0, qtdJogadas1 = 0, qtdJogadas2 = 0, i = 0, j = 0, x = 0, l = 0, c = 0;
    int jogadas1[200][3], jogadas2[200][3];//armazenar as jogadas
    float linhaMedio = 0, colunaMedio = 0, desvioPadrao = 0;

    //abrindo estatisticas.txt para escrita
    sprintf(diretorio, "%s/saida/estatisticas.txt", argv[1]);
    estatisticas = fopen(diretorio, "w");

    //abrindo resultado.txt para leitura
    sprintf(diretorio, "%s/saida/resultado.txt", argv[1]);
    resultado = fopen(diretorio, "r");


    //lendo e imprimindo o nome do jogador 1
    fscanf(resultado, "%s", jogo.jogador1.nome);
    fscanf(resultado, "%*c");//jogando fora o \n
    fprintf(estatisticas, "%s\n", jogo.jogador1.nome);

    while(1){//loop pra analisar o primeiro jogador

        if(x){//entrar a partir da segunda passada
            fscanf(resultado, "%*[^\n]");//jogando fora tudo da linha
            fscanf(resultado, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/
        }

        fscanf(resultado, "%[^\n]", vet);//AGuA, TIrO

        if(vet[0] >= 'A' && vet[0] <= 'Z'){//proximo jogador
            i++;
            break;

        } else if(vet[3] == ' '){//Ex.: i10 - 
            if(vet[6] == 'A'){//Agua
                qtdTirosErrados++;
            } else if(vet[6] == 'T'){//Tiro
                qtdTirosAcertados++;
            }

        } else{//Ex.: b1 -
            if(vet[5] == 'A'){//Agua
                qtdTirosErrados++;
            } else if(vet[5] == 'T'){//Tiro
                qtdTirosAcertados++;
            }

        }

        //Pro calculo da localizacao media
        l = vet[0] - 96;//a tem que virar 1
        if(vet[2] == '0'){
            c = 10;
        } else{
            c = vet[1] - '0';//1 tem que virar 1 mesmo
        }

        //armazenando na matriz das jogadas
        jogadas1[i][0] = l;
        jogadas1[i][1] = c;

        //somas
        linhaMedio = linhaMedio + l;
        colunaMedio = colunaMedio + c;

        i++;
        x++;
    }

    fprintf(estatisticas, "Tiros Errados: %d\n", qtdTirosErrados);
    fprintf(estatisticas, "Tiros Acertados: %d\n", qtdTirosAcertados);

    //calculando localizacao media
    qtdJogadas1 = qtdTirosAcertados + qtdTirosErrados;
    linhaMedio = CalculaMedia(linhaMedio, qtdJogadas1);
    colunaMedio = CalculaMedia(colunaMedio, qtdJogadas1);
    fprintf(estatisticas, "Localizacao Media: (%.2f,%.2f)\n", linhaMedio, colunaMedio);

    //calculando o desvio padrao
    desvioPadrao = CalculaDesvioPadrao(jogadas1, linhaMedio, colunaMedio, qtdJogadas1);
    fprintf(estatisticas, "Desvio Padrao da Localizacao: %.2f\n", desvioPadrao);


    i = 0;
    x = 0;
    qtdTirosAcertados = 0;
    qtdTirosErrados = 0;
    linhaMedio = 0;
    colunaMedio = 0;


    while(1){//loop pra analisar o segundo jogador

        fscanf(resultado, "%*[^\n]");//jogando fora tudo da linha
        fscanf(resultado, "%*[^a-zA-Z]");/*jogando fora tudo ate encontrar outra letra
                                            (proxima linha)*/

        fscanf(resultado, "%[^\n]", vet);//AGuA, TIrO

        if(vet[0] >= 'A' && vet[0] <= 'Z'){//proximo jogador
            i++;
            break;

        } else if(vet[3] == ' '){//Ex.: i10 - 
            if(vet[6] == 'A'){//Agua
                qtdTirosErrados++;
            } else if(vet[6] == 'T'){//Tiro
                qtdTirosAcertados++;
            }

        } else{//Ex.: b1 -
            if(vet[5] == 'A'){//Agua
                qtdTirosErrados++;
            } else if(vet[5] == 'T'){//Tiro
                qtdTirosAcertados++;
            }

        }

        //Pro calculo da localizacao media
        l = vet[0] - 96;//a tem que virar 1
        if(vet[2] == '0'){
            c = 10;
        } else{
            c = vet[1] - '0';//1 tem que virar 1 mesmo
        }

        //armazenando na matriz das jogadas
        jogadas2[i][0] = l;
        jogadas2[i][1] = c;

        //somas
        linhaMedio = linhaMedio + l;
        colunaMedio = colunaMedio + c;

        i++;
        x++;
    }

    qtdJogadas2 = qtdTirosAcertados + qtdTirosErrados;

    //listando os navios atingidos do jogador 1
    fprintf(estatisticas, "Navios de %s:\n", jogo.jogador1.nome);
    ListaNaviosAtingidos(qtdJogadas2, jogadas2, jogo.tabuleiro1, estatisticas);

    fprintf(estatisticas, "\n%s\n", jogo.jogador2.nome);
    fprintf(estatisticas, "Tiros Errados: %d\n", qtdTirosErrados);
    fprintf(estatisticas, "Tiros Acertados: %d\n", qtdTirosAcertados);

    //calculando localizacao media
    linhaMedio = CalculaMedia(linhaMedio, qtdJogadas2);
    colunaMedio = CalculaMedia(colunaMedio, qtdJogadas2);
    fprintf(estatisticas, "Localizacao Media: (%.2f,%.2f)\n", linhaMedio, colunaMedio);

    //calculando o desvio padrao
    desvioPadrao = CalculaDesvioPadrao(jogadas2, linhaMedio, colunaMedio, qtdJogadas2);
    fprintf(estatisticas, "Desvio Padrao da Localizacao: %.2f\n", desvioPadrao);

    //listando os navios atingidos do jogador 2
    fprintf(estatisticas, "Navios de %s:\n", jogo.jogador2.nome);
    ListaNaviosAtingidos(qtdJogadas1, jogadas1, jogo.tabuleiro2, estatisticas);
    
    fclose(estatisticas);
    fclose(resultado);
}

float CalculaMedia(float soma, int qtd){
    float media = 0.00;
    media = soma/qtd;
    return media;
}

float CalculaDesvioPadrao(int jogadas[200][3], float linhaMedio, float colunaMedio, int qtdJogadas){

    int i = 0;
    float dp = 0;

    for(i = 0; i < qtdJogadas; i++){
        dp += pow((jogadas[i][0] - linhaMedio), 2);
        dp += pow((jogadas[i][1] - colunaMedio), 2);
    }
    dp = dp/qtdJogadas;
    dp = sqrt(dp);
    return dp;
}

void ListaNaviosAtingidos(int qtdJogadas, int jogadas[200][3], tTabuleiro tab, FILE *estatisticas){

    int i = 0, l = 0, c = 0;

    while(i < qtdJogadas){

        //trandformando pra correspondente na matriz
        l = jogadas[i][0]-1;//1 tem que ser 0
        c = TransformaNaColunaCorresp(jogadas[i][1]);

        if(tab.matriz[l][c] == 'X'){
            tab = IdentificaNavioEstatisticas(i, tab, jogadas[i][0], jogadas[i][1]);
        }
        i++;
    }

    ImprimeNaviosOrdenados(tab, estatisticas);
}

tTabuleiro IdentificaNavioEstatisticas(int i, tTabuleiro tab, int lNum, int c){

    //mesma ideia do ImprimeNomeNavioAfundado

    /*A partir da posicao da jogada responsavel por afundar, percorrer por todos os pontos do navio,
    quando um desses pontos bater com a posicao original de um navio, imprimir o nome desse*/

    int linha = 0, coluna = 0, x = 1, y = 0;//y: conta navios
    char l;

    linha = lNum - 1;
    l = linha + 'a';
    coluna = TransformaNaColunaCorresp(c);

    if(tab.matriz[linha+x][coluna] == 'X' || tab.matriz[linha-x][coluna] == 'X'){//do tipo vertical

        //analisando a propria posicao
        y = 0;
        while(y < tab.qtdNavios){//passando por todos os navios
            if(l == tab.navio[y].linha && c == tab.navio[y].coluna){
                if(!(EstaMarcadoNavio(tab.navio[y]))){
                    tab.navio[y] = MarcaNavio(tab.navio[y], i+1);
                }
                break;
            }
            y++;
        }

        //analisando navio abaixo
        while(tab.matriz[linha+x][coluna] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l+x == tab.navio[y].linha && c == tab.navio[y].coluna){
                    if(!(EstaMarcadoNavio(tab.navio[y]))){
                        tab.navio[y] = MarcaNavio(tab.navio[y], i+1);
                    }
                    break;
                }
                y++;
            }
            x++;
        }

        y = 0;
        x = 1;

        //analisando navio acima
        while(tab.matriz[linha-x][coluna] == 'X'){//passando por todas as posicoes do navio
            y = 0;
           while(y < tab.qtdNavios){//passando por todos os navios
                if(l-x == tab.navio[y].linha && c == tab.navio[y].coluna){
                    if(!(EstaMarcadoNavio(tab.navio[y]))){
                        tab.navio[y] = MarcaNavio(tab.navio[y], i+1);
                    }
                    break;
                }
                y++;
            }
            x++;
        }
        y = 0;

    } else{//do tipo horizontal
        //Analisando a propria posicao
        y = 0;
        while(y < tab.qtdNavios){//passando por todos os navios
            if(l == tab.navio[y].linha && c+(x/2) == tab.navio[y].coluna){
                if(!(EstaMarcadoNavio(tab.navio[y]))){
                    tab.navio[y] = MarcaNavio(tab.navio[y], i+1);
                }
                break;
            }
            y++;
        }

        x = 2;
        //analisando navio a direita
        while(tab.matriz[linha][coluna+x] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l == tab.navio[y].linha && c+(x/2) == tab.navio[y].coluna){
                   if(!(EstaMarcadoNavio(tab.navio[y]))){
                        tab.navio[y] = MarcaNavio(tab.navio[y], i+1);
                    }
                    break;
                }
                y++;
            }
            x += 2;
        }

        y = 0;
        x = 2;

        //analisando navio a esquerda
        while(tab.matriz[linha][coluna-x] == 'X'){//passando por todas as posicoes do navio
            y = 0;
            while(y < tab.qtdNavios){//passando por todos os navios
                if(l == tab.navio[y].linha && c-(x/2) == tab.navio[y].coluna){
                   if(!(EstaMarcadoNavio(tab.navio[y]))){
                        tab.navio[y] = MarcaNavio(tab.navio[y], i+1);
                    }
                    break;
                }
                y++;
            }
            x += 2;
        }
    }

    return tab;
}

tNavio MarcaNavio(tNavio navio, int numJogada){
    navio.marcacao = numJogada;
    return navio;
}

int EstaMarcadoNavio(tNavio navio){
    return navio.marcacao;
}

void ImprimeNaviosOrdenados(tTabuleiro tab, FILE* estatisticas){

    int i = 0, j = 0, rtn = 0, marcaI = 0, marcaJ = 0;
    tNavio aux;


    for(i = 0; i < tab.qtdNavios; i++){
        if(EstaMarcadoNavio(tab.navio[i])){
            for(j = i+1; j < tab.qtdNavios; j++){
                if(EstaMarcadoNavio(tab.navio[j])){
                    marcaI = RetornaMarcacaoNavio(tab.navio[i]);//pra saber qual foi atingido primeiro
                    marcaJ = RetornaMarcacaoNavio(tab.navio[j]);
                    rtn = strcmp(tab.navio[i].tipo, tab.navio[j].tipo);
                    if(rtn > 0){
                        aux = tab.navio[i];
                        tab.navio[i] = tab.navio[j];
                        tab.navio[j] = aux;
                        
                    } else if(rtn == 0){//Mesmo tipo
                        if(marcaJ < marcaI){
                            aux = tab.navio[i];
                            tab.navio[i] = tab.navio[j];
                            tab.navio[j] = aux;
                        }
                    }
                }
            }
        }
    }

    for(i = 0; i < tab.qtdNavios; i++){
        if(EstaMarcadoNavio(tab.navio[i])){
            fprintf(estatisticas, "%.2d - %s - ID %.2d\n", tab.navio[i].marcacao, tab.navio[i].tipo, tab.navio[i].id);
        }
    }
}

int RetornaMarcacaoNavio(tNavio navio){
    return navio.marcacao;
}