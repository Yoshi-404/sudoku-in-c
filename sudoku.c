#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define ERROR_FILE_MSG	"Nao foi possivel abrir o arquivo!\n"
#define INVALID_OPTION	"Opcao invalida! Tente novamente\n!"

enum boolean
{
    FALSO=0, VERDADEIRO=1
};

/* Prototipos */
FILE* carregar(char quadro[9][9]);
FILE *carregue_continue_jogo(char quadro[9][9], char *nome_arquivo);
void carregue_novo_jogo(char quadro[9][9], char *name_arq_txt);
int determinar_quadrante(int x, int y);
FILE* crie_arquivo_binario(char quadro[9][9]);
int e_valido(const char quadro[9][9], int x, int y, int valor);
int e_valido_na_coluna(const char quadro[9][9], int y, int valor);
int e_valido_no_quadro3x3(const char quadro[9][9], int x, int y, int valor);
int e_valido_na_linha(const char quadro[9][9], int x, int valor);
int existe_campo_vazio(const char quadro[9][9]);
void imprime(const char quadro[9][9]);
void jogar();
int resolver(FILE*, char quadro[9][9]);
int resolver_um_passo(char quadro[9][9]);
void salvar_jogada_bin(FILE* fb, char quadro[9][9]);

/* Funcoes auxiliares */
int fim_x(int quadr);
int fim_y(int quadr);
int le_opcao();
void gen_random(char *s, const int len);
int ini_x(int quadr);
int ini_y(int quadr);
void menu();
void menu_arquivo();////////////////////////////////////////////////////////////////////////

int main()
{
    jogar();
    return 0;
}

/* -----------------------------------------------------------------------------
 * CARREGAR
 * Inicializa o SUDOKU a partir de um novo jogo ou estado de jogo anterior
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

FILE *carregar(char quadro[9][9])
{
    int opcao;
    char name_aqr_txt[254],name_arq_bin[254];
    menu_arquivo();
    opcao = le_opcao();

    switch (opcao)
    {
    case 1:
        printf("Informe o nome do seu arquivo com o .txt\n");
        scanf("%s", name_aqr_txt);
        carregue_novo_jogo(quadro, name_aqr_txt);
        break;
    case 2:
        printf("Informe o nome de um arquivo binario existente com o .bin:\n");
        scanf("%s", name_arq_bin);
        return carregue_continue_jogo(quadro, name_arq_bin);
        break;
    case 9:
        break;
    default:
        break;
    }
}

void carregue_novo_jogo(char quadro[9][9], char *name_arq_txt)
{
    int i, j;
    FILE *novo;
    novo = fopen(name_arq_txt, "r");
    if (novo == NULL)
    {
        printf(ERROR_FILE_MSG);
        return;
    }
    else
    {
        for (i = 0; i < 9; i++)
            for (j = 0; j < 9; j++)
                fscanf(novo, "%d",&quadro[i][j]);
    }
    fclose(novo);
}

FILE *carregue_continue_jogo(char quadro[9][9], char *nome_arquivo)
{

    FILE *arq_bin;
    arq_bin = fopen(nome_arquivo,"r+b");
    if (arq_bin == NULL)
    {
        puts(ERROR_FILE_MSG);
        jogar();
    }
    fseek(arq_bin, sizeof(int) * 3,SEEK_SET);
    fread(quadro, sizeof(char), 9 * 9,arq_bin);
    return arq_bin;
}
/* -----------------------------------------------------------------------------
 * CRIAR ARQUIVO BINARIO
 * Criar arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
FILE *crie_arquivo_binario(char quadro[9][9])
{
    char arq[255];
    int t = 0;
    int x,y;
    FILE *arq_bin;
    printf("De um nome para seu arquivo binario\n");
    scanf(" %s", arq);
    arq_bin = fopen(arq, "w+b");
    if (arq_bin == NULL)
    {
        printf(ERROR_FILE_MSG);
        return 0;
    }
    fseek(arq_bin, 0, SEEK_SET);
    fwrite(&t, sizeof(int), 1,arq_bin);//inicializando jogadas com 0
    fseek(arq_bin, sizeof(int) * 3, SEEK_SET);
    for (x = 0; x < 9; x++)
        for (y = 0; y < 9; y++)
            fwrite(&quadro[x][y], sizeof(char), 1, arq_bin);
    return arq_bin;
}

/* -----------------------------------------------------------------------------
 * DETERMINAR QUADRANTE
 * Dado as posicoes x e y, determina o quadrante do quadro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int determinar_quadrante(int x, int y)
{
    if (x < 3 && y < 3)
        return 1;
    else if (x < 3 && y < 6)
        return 2;
    else if (x < 3 && y <= 8)
        return 3;
    else if (x < 6 && y < 3)
        return 4;
    else if (x < 6 && y < 6)
        return 5;
    else if (x < 6 && y <= 8)
        return 6;
    else if (x <= 8 && y < 3)
        return 7;
    else if (x <= 8 && y < 6)
        return 8;
    else
        return 9;
}

/* -----------------------------------------------------------------------------
 * E VALIDO
 * Determina se um valor na posicao x e y e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
/* -----------------------------------------------------------------------------
 * E VALIDO
 * Determina se um valor na posicao x e y e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int e_valido(const char quadro[9][9], int x, int y, int valor)
{

    if (!e_valido_na_coluna(quadro, y, valor))
        return FALSO;
    if (!e_valido_na_linha(quadro, x, valor))
        return FALSO;
    if (!e_valido_no_quadro3x3(quadro, x, y, valor))
        return FALSO;

    return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NA COLUNA
 * Verifica se um valor na coluna y e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int e_valido_na_coluna(const char quadro[9][9], int coluna, int valor)
{
    int i;
    for (i = 0; i <= 8; i++)
        if (quadro[i][coluna] == valor)
            return FALSO;
    return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NA LINHA
 * Verifica se um valor na linha x e valido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int e_valido_na_linha(const char quadro[9][9], int linha, int valor)
{
    int i;
    for (i = 0; i <= 8; i++)
        if (quadro[linha][i] == valor)
            return FALSO;
    return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * E VALIDO NO QUADRO 3X3
 * Verifica se um valor e valido no quadrante da posicao x, y
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int e_valido_no_quadro3x3(const char quadro[9][9], int x, int y, int valor)
{
    int quadrante = determinar_quadrante(x, y);
    int i, j;
    for (i = ini_x(quadrante); i <= fim_x(quadrante); i++)
        for (j = ini_y(quadrante); j <= fim_y(quadrante); j++)
            if (quadro[i][j] == valor)
                return FALSO;
    return VERDADEIRO;
}

/* -----------------------------------------------------------------------------
 * EXISTE CAMPO VAZIO
 * Verifica se existe um campo n�o preenchido
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int existe_campo_vazio(const char quadro[9][9])
{
    int i, j;
    for (i = 0; i <= 8; i++)
        for (j = 0; j <= 8; j++)
            if (quadro[i][j] == 0)
                return VERDADEIRO;
    return FALSO;
}

/* -----------------------------------------------------------------------------
 * IMPRIMIR
 * Imprime o quadro inteiro do sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void imprime(const char quadro[9][9])
{
    int i, j;
    puts("~~~~~~~~ SUDOKU ~~~~~~~~");
    puts("    1 2 3   4 5 6   7 8 9");
    for (i = 0; i <= 8; i++)
    {
        if (i % 3 == 0)
            puts("  -------------------------");
        for (j = 0; j <= 8; j++)
        {

            if (j == 0)
                printf("%d | ", i + 1);
            else if (j % 3 == 0)
                printf("| ");

            if (quadro[i][j] == 0)
                printf("- ");
            else
                printf("%d ", quadro[i][j]);
        }
        puts("|");
    }
    puts("  -------------------------");
}

/* -----------------------------------------------------------------------------
 * JOGAR
 * Realiza toda a logica do jogo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void jogar()
{
    int opcao,linha, coluna, valor,num_jogadas;
    char quadro[9][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0}, {
            0, 0, 0, 0, 0, 0, 0,
            0, 0
        }, {0, 0, 0, 0, 0, 0, 0, 0, 0}, {
            0, 0, 0, 0, 0, 0, 0,
            0, 0
        }, {0, 0, 0, 0, 0, 0, 0, 0, 0}, {
            0, 0, 0, 0, 0, 0, 0,
            0, 0
        }, {0, 0, 0, 0, 0, 0, 0, 0, 0}, {
            0, 0, 0, 0, 0, 0, 0,
            0, 0
        }, {0, 0, 0, 0, 0, 0, 0, 0, 0}
    }
    ;
    char quadro_aux[9][9];

    FILE *fb = NULL;

    opcao = 0;

    while (opcao != 9)
    {
        imprime(quadro);
        menu();
        opcao = le_opcao();

        switch (opcao)
        {

        // carregar sudoku
        case 1:
            fb = carregar(quadro);

            if (fb == NULL)
            {
                fb = crie_arquivo_binario(quadro);
            }
            break;

        // preencher quadro com um valor
        case 2:
            printf("Entre com a posicao e o valor (linha, coluna, valor):\n ");
            fflush(stdin);
            scanf(" %d %d %d", &linha, &coluna, &valor);
            if (e_valido(quadro, linha - 1, coluna - 1, valor))
            {
                quadro[linha - 1][coluna - 1] = valor;
                salvar_jogada_bin(fb, quadro);
                rewind(fb);
                fread(&num_jogadas, sizeof(int), 1, fb);
                printf("QUANTIDADE DE JOGADAS: %d\n",num_jogadas);
                break;
            }
            else
            {
                printf(INVALID_OPTION);
            }
            break;

        // resolver 1 passo
        case 3:
            resolver_um_passo(quadro);
            salvar_jogada_bin(fb, quadro);
            printf("Passo relvido\n");
            rewind(fb);
            fread(&num_jogadas, sizeof(int), 1, fb);
            printf("QUANTIDADE DE JOGADAS: %d\n",num_jogadas);
            break;

        // resolver o sudoku
        case 4:
            resolver(fb, quadro);
            salvar_jogada_bin(fb, quadro);
            fclose(fb);
            break;
        //finaliza
        case 9:
            printf("Programa finalizado ..");
            salvar_jogada_bin(fb, quadro);
            fclose(fb);
            break;

        default:
            printf("Opcao invalida\n");
            break;
        }
    }
}
/* -----------------------------------------------------------------------------
 * RESOLVER
 * Resolve o sudoku
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int resolver(FILE *fb, char quadro[9][9])
{
    int num_jogadas;
        while (existe_campo_vazio(quadro))
            {
                resolver_um_passo(quadro);
                salvar_jogada_bin(fb, quadro);
            }
        rewind(fb);
        fread(&num_jogadas, sizeof(int), 1, fb);
        printf("QUANTIDADE DE JOGADAS: %d\n",num_jogadas);
}

/* -----------------------------------------------------------------------------
 * RESOLVER UM PASSO
 * Preenche apenas um campo vazio
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

int resolver_um_passo(char quadro[9][9]) {
    if(existe_campo_vazio(quadro))
    {
        int fez_progresso,linha,coluna,candidatos,num;
        for (linha = 0; linha < 9; linha++) {
            for (coluna = 0; coluna < 9; coluna++) {
                if (quadro[linha][coluna] == 0) {
                    candidatos = 0;
                    for (num = 1; num <= 9; num++) {
                        if (e_valido(quadro, linha, coluna, num)) {
                            candidatos++;
                            quadro[linha][coluna] = num;
                            fez_progresso = VERDADEIRO;
                        }
                    }
                    if (candidatos == 1) {
                        return VERDADEIRO;  // Um candidato valido encontrado, sair do passo
                    } else {
                        quadro[linha][coluna] = 0;  // Nao ha um unico candidato valido
                    }
                }
            }
        }
        return VERDADEIRO;  // Retorna se fez algum progresso
    }
    else
    {
        printf("nao existe mais passoas a serem resolvidos\n");
    }
}



/* -----------------------------------------------------------------------------
 * SALVAR JOGADA BINARIO
 * Salva o estado atual do quadro no arquivo binario
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

void salvar_jogada_bin(FILE *fb, char quadro[9][9])
{
    int num_jogadas,i,j;
    if (fb == NULL)
    {
        printf("Erro!!\n");
        return;

    }
    else
    {
        fseek(fb, sizeof(int) * 3, SEEK_SET);
        for (i = 0; i < 9; i++)
        {
            for (j = 0; j < 9; j++)
            {
                fwrite(&quadro[i][j], sizeof(char), 1, fb);
            }
        }
        rewind(fb);
        fread(&num_jogadas, sizeof(int), 1, fb);
        num_jogadas++;
        rewind(fb);
        fwrite(&num_jogadas, sizeof(int), 1, fb);
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 							FUNCOES AUXILIARES
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/* -----------------------------------------------------------------------------
 * FIM X
 * Indice final da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int fim_x(int quadr)
{
    switch (quadr)
    {
    case 1:
    case 2:
    case 3:
        return 2;

    case 4:
    case 5:
    case 6:
        return 5;

    default:
        return 8;
    }
}

/* -----------------------------------------------------------------------------
 * FIM Y
 * Indice final da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int fim_y(int quadr)
{
    switch (quadr)
    {
    case 1:
    case 4:
    case 7:
        return 2;

    case 2:
    case 5:
    case 8:
        return 5;

    default:
        return 8;
    }
}

/* -----------------------------------------------------------------------------
 * GEN_RANDOM
 * Gera uma cadeira de caracteres randomica de tamanho len
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void gen_random(char *s, const int len)
{
    srand(time(NULL));
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
    int i;

    for (i = 0; i < len; ++i)
    {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

/* -----------------------------------------------------------------------------
 * INI X
 * Indice inicial da linha para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_x(int quadr)
{
    switch (quadr)
    {
    case 1:
    case 2:
    case 3:
        return 0;

    case 4:
    case 5:
    case 6:
        return 3;

    default:
        return 6;
    }
}

/* -----------------------------------------------------------------------------
 * INI Y
 * Indice inicial da coluna para o quadrante recebido como parametro
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int ini_y(int quadr)
{
    switch (quadr)
    {
    case 1:
    case 4:
    case 7:
        return 0;

    case 2:
    case 5:
    case 8:
        return 3;

    default:
        return 6;
    }
}

/*Abre o aquivo p/ leitura  */

/* -----------------------------------------------------------------------------
 * LE OPCAO
 * Imprime a mensagem a faz a leitura da opcao
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int le_opcao()
{
    int opcao;

    printf("Opcao: ");
    scanf("%d", &opcao);

    return opcao;
}

/* -----------------------------------------------------------------------------
 * MENU
 * Imprime o menu de opcoes
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void menu()
{
    puts("\n~~~~~~~~ SUDOKU ~~~~~~~~");
    puts("[1] - Carregar jogo");
    puts("[2] - Jogar");
    puts("[3] - Resolver um passo");
    puts("[4] - Resolver");
    puts("[9] - Finalizar");
    puts("--------");
}

/* -----------------------------------------------------------------------------
 * MENU ARQUIVO
 * Imprime o menu de opcoes do arquivo
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void menu_arquivo()
{
    puts("\n~~~~~ MENU ARQUIVO ~~~~~");
    puts("[1] - Novo jogo");
    puts("[2] - Continuar jogo");
    puts("[9] - Retornar ao menu anterior");
    puts("--------");
}
