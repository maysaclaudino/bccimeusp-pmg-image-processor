#include <stdio.h>
#include <stdlib.h>

/* gcc -ansi -Wall -pedantic -o ep3 ep3.c */

/* OK inversão
   OK binarização
      contorno
      filtragem
   OK rotulação
   OK leitura
   OK salvar

   !!! DAR FREE !!! */

void printImage(int *image, int largura, int altura) {
    int i, j;

    for (i = 0; i < altura; i++) {
        for (j = 0; j < largura; j++) {
            printf("%3d ", image[i*largura + j]);
        }
        printf("\n");
    }
}

int *lerImagem(char *arquivo, int *largura, int *altura) {
    FILE *entrada;
    int i;
    int *imagem;
    char line[10];

    entrada = fopen(arquivo, "r");
    fscanf(entrada, "%[^\n]\n", line);

    fscanf(entrada, "%d %d\n", largura, altura);

    fscanf(entrada,"%[^\n]\n", line);

    imagem = malloc(sizeof(int)*(*largura)*(*altura));

    for (i = 0; i < (*largura)*(*altura); i++) {
        fscanf(entrada, "%d", &imagem[i]);
    }
    fclose(entrada);
    return imagem;
}

int *invert_image(int *image, int width, int height) {
    int i, j;

    int *invertedImage;
    invertedImage = malloc(sizeof(int)*width*height);

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            invertedImage[i*width + j] = 255 - image[i*width + j];
        }
    }
    return invertedImage;
}

int *binarize(int *image, int width, int height, int limiar) {
    int i;
    int *imagemLimiarizada;

    imagemLimiarizada = malloc(sizeof(int)*width*height);
    for (i = 0; i < width*height; i++) {
        if (image[i] >= limiar) {
            imagemLimiarizada[i] = 255;
        }
        else {
            imagemLimiarizada[i] = 0;
        }
    }

    return imagemLimiarizada;
}

void salvarImagem(char *arquivo, int *imagem, int largura, int altura) {
    FILE *saida;
    int i, j;

    saida = fopen(arquivo, "w");

    fprintf(saida, "P2\n");
    fprintf(saida, "%d %d\n", largura, altura);
    fprintf(saida, "255\n");

    for (i = 0; i < altura; i++) {
        for (j = 0; j < largura; j++) {
            fprintf(saida, "%d ", imagem[i*largura + j]);
        }
        fprintf(saida, "\n");
    }
}

int *adicionaBorda(int *imagem, int largura, int altura) {
    int i, j, tamanho;
    int *borda;
    tamanho = (largura + 2)*(altura+2);
    borda = malloc(sizeof(int)*tamanho);

    for (i = 0; i < (largura + 2); i++) {
        borda[i] = 0;
        borda[tamanho -1 - i] = 0;
    }

    for (i = 1; i <= altura; i++) {
        borda[i*(largura+2)] = 0;
        borda[i*(largura+2)+(largura+1)] = 0;
    }

    for (i = 0; i < altura; i++) {
        for (j = 0; j< largura; j++) {
            borda[(i+1)*(largura+2)+(j+1)] = imagem[i*largura+j];
        }
    }

    return borda;
}

int *removeBorda(int *imagemComBorda, int larguraSemBorda, int alturaSemBorda) {
    int i, j;
    int *semBorda;
    semBorda = malloc(sizeof(int)*larguraSemBorda*alturaSemBorda);

    for (i = 1; i <= alturaSemBorda; i++) {
        for (j = 1; j <=larguraSemBorda; j++) {
            semBorda[(i-1)*larguraSemBorda+(j-1)] = imagemComBorda[i*(larguraSemBorda+2)+j];
        }
    }

    return semBorda;
}

void substituiVizinhos(int *imagem, int linha, int coluna, int qtdComponentes, int largura) {
    int i, j, indiceVizinho;

    for (i = linha-1; i <= (linha+1); i++) {
        for (j = coluna-1; j <= (coluna+1); j++) {
            indiceVizinho = i*largura+j;
            
            if (imagem[indiceVizinho] == 255) {
                    imagem[indiceVizinho] = qtdComponentes;
/*                     printf("\n\nLinha: %d   Coluna: %d\n", linha, coluna);
                    printImage(imagem, 14, 5); */
                    substituiVizinhos(imagem, i, j, qtdComponentes, largura);
            }
        }
    }
}

int *rotulacao(int *imagem, int largura, int altura) {
    int i, j, qtdComponentes; 
    int *imagemComBorda;
    int *imagemRotulada;

    imagemComBorda = adicionaBorda(imagem, largura, altura);
/*     printImage(imagemComBorda, largura+2, altura+2); */

    qtdComponentes = 1;
    for (i = 1; i < (altura+2); i++) {
        for (j = 1; j < (largura+2); j++) {
            if (imagemComBorda[i*(largura+2)+j] == 255) {
                substituiVizinhos(imagemComBorda, i, j, qtdComponentes, (largura+2));
                qtdComponentes++;
            }
        }
    }

    imagemRotulada = removeBorda(imagemComBorda, largura, altura);

    free(imagemComBorda);
    imagemComBorda = NULL;

    for (i = 0; i < altura; i++) {
        for (j = 0; j < largura; j++) {
            if (imagemRotulada[i*largura+j] != 0) {
                imagemRotulada[i*largura+j] = 255 / imagemRotulada[i*largura+j];
            }
        }
    }

    return imagemRotulada;
}

int main ()
{
    char arquivo[50];
    int *imagem;
    int *imagem2;
    int largura, altura;

    printf("Nome do arquivo de entrada: ");
    scanf("%s", arquivo);

    imagem = lerImagem(arquivo, &largura, &altura);

/*     printf("Nome do arquivo de entrada: ");
    printf("imagemTeste.txt\n\n");
    imagem = lerImagem("imagemTeste.txt", &largura, &altura); */

    imagem2 = invert_image(imagem, largura, altura);

/*     imagem2 = binarize(imagem, largura, altura, 127); */

/*     printf("Nome do arquivo de saida INVERSÃO: ");
    scanf("%s", arquivo);
    salvarImagem(arquivo, imagem2, largura, altura); */

    imagem2 = rotulacao(imagem2, largura, altura);

    printf("----------- IMAGEM FINAL -----------\n");
    printImage(imagem2, largura, altura);

    printf("Nome do arquivo de saida ROTULARIZAÇÃO: ");
    scanf("%s", arquivo);
    salvarImagem(arquivo, imagem2, largura, altura);

/*     printf("Nome do arquivo de saida: ");
    printf("resultado_rot.pgm.txt\n\n");
    salvarImagem("resultado_rot.pgm.txt", imagem2, largura, altura); */

    return 0;
}