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

/*     for (i = 0; i < (largura + 2); i++) {
        borda[i] = 0;
        borda[tamanho -1 - i] = 0;
    }

    for (i = 1; i <= altura; i++) {
        borda[i*(largura+2)] = 0;
        borda[i*(largura+2)+(largura+1)] = 0;
    } */

    for (i = 0; i < altura; i++) {
        for (j = 0; j < largura; j++) {
            borda[(i+1)*(largura+2)+(j+1)] = imagem[i*largura+j];
        }
        borda[i] = 0;
        borda[tamanho -1 - i] = 0;

        borda[(i+1)*(largura+2)] = 0;
        borda[(i+1)*(largura+2)+(largura+1)] = 0;
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

int *alargaImagem(int *imagem, int largura, int altura, int valor) {
    int i, j, tamanho, novaLargura, novaAltura;
    int *imagemAlargada;

    novaLargura = largura + 2*valor;
    novaAltura = altura + 2*valor;
    tamanho = (novaLargura)*(altura + 2*valor);
    imagemAlargada = malloc(sizeof(int)*tamanho);

    for (i = 0; i < valor; i++) {      /* copia as linhas superiores e inferiores */
        for (j = valor; j < (largura + valor); j++) {
            imagemAlargada[i*novaLargura+j] = imagem[j-valor];
            imagemAlargada[(novaAltura-1-i)*novaLargura+j] = imagem[(altura-1)*largura + (j-valor)];
        }
    }

    for (i = valor; i < (altura + valor); i++) {      /* copia as colunas a direita e a esquerda */
        for (j = 0; j < valor; j++) {
            imagemAlargada[i*novaLargura+j] = imagem[(i-valor)*largura];
            imagemAlargada[i*novaLargura+(novaLargura-1-j)] = imagem[(i-valor)*largura+(largura-1)];
        }
    }

    for (i = 0; i < valor; i++) {      /* copia os cantos */
        for (j = 0; j < valor; j++) {
            imagemAlargada[i*novaLargura+j] = imagem[0];      /* canto superior esquerdo */
            imagemAlargada[i*novaLargura+(novaLargura-1-j)] = imagem[largura-1];      /* canto superior direito */
            imagemAlargada[(novaAltura-1-i)*novaLargura+j] = imagem[(altura-1)*largura];      /* canto inferior esquerdo */
            imagemAlargada[(novaAltura-1-i)*novaLargura+(novaLargura-1-j)] = imagem[(altura-1)*largura+(largura-1)];      /* canto inferior direito */
        }
    }

    for (i = 0; i < altura; i++) {      /* copia o conteudo da matriz */
        for (j = 0; j < largura; j++) {
            imagemAlargada[(i+valor)*novaLargura + (j+valor)] = imagem[i*largura+j];
        }
    }

    return imagemAlargada;
}

int *coletaVizinhos(int *imagem, int largura, int linha, int coluna, int valor) {
    int i, j, k;
    int *elementosVizinhos;

    elementosVizinhos = malloc(sizeof(int)*(2*valor+1));

    k = 0;
    for (i = linha-valor; i <= linha+valor; i++) {
        for (j = coluna-valor; j <= coluna+valor; j++) {
            elementosVizinhos[k] = imagem[i*largura+j];
            k++;
        }
    }

    return elementosVizinhos;
}

void troca(int *a, int *b) {
    int t;

    t = *a;
    *a = *b;
    *b = t;
}

void ordenaVetor(int *vetor, int tamanho) {
    int i, pivot, indicePivot, encontrouMaior, encontrouMenor;

    indicePivot = 0;
    pivot = vetor[indicePivot];

    encontrouMaior = 1;
    while (encontrouMaior || encontrouMenor) {
        encontrouMaior = 0; encontrouMenor = 0;

        for (i = tamanho-1; (!encontrouMenor && (i > indicePivot)); i--) {
            if (vetor[i] < pivot) {
                troca(&vetor[i], &vetor[indicePivot]);
                indicePivot = i;
                encontrouMenor = 1;
            }
        }

        for (i = 0; (!encontrouMaior && (i < indicePivot)); i++) {
            if (vetor[i] > pivot) {
                troca(&vetor[i], &vetor[indicePivot]);
                indicePivot = i;
                encontrouMaior = 1;
            }
        }
    }
}

/* int *filtragrem(int *imagem, int largura, int altura, int dimensao, int tipo) {
    int i, j, novaLargura, novaAltura;
    int *imagemAlargada;
    int *vizinhos;

    novaLargura = largura + dimensao-1;
    novaAltura = altura + dimensao -1;

    imagemAlargada = alargaImagem(imagem, largura, altura, dimensao/2);

    for (i = 0; i < novaAltura; i++) {
        for (j = 0; j < novaLargura; j++) {
            vizinhos = coletaVizinhos(imagem, novaLargura, i, j, dimensao/2);

        }
    }


} */

int main ()
{
    int i;
/*     int *imagem;
    int *imagem2;
    int largura, altura; */

/*     char arquivo[50];
    printf("Nome do arquivo de entrada: ");
    scanf("%s", arquivo);
    imagem = lerImagem(arquivo, &largura, &altura); */

/*     printf("Nome do arquivo de entrada: ");
    printf("imagemTeste.txt\n\n");
    imagem = lerImagem("imagemTeste.txt", &largura, &altura); */

/*     imagem2 = invert_image(imagem, largura, altura); */

/*     imagem2 = binarize(imagem, largura, altura, 127); */

/*     printf("Nome do arquivo de saida INVERSÃO: ");
    scanf("%s", arquivo);
    salvarImagem(arquivo, imagem2, largura, altura); */

/*     imagem2 = rotulacao(imagem2, largura, altura); */

/*     imagem2 = alargaImagem(imagem, largura, altura, 2); */

    int imagem[8] = {3, 5, 6, 4, 2, 3, 6, 1};
    ordenaVetor(imagem, 8);
    printf("----------- IMAGEM FINAL -----------\n");
/*     printImage(imagem, 1, 1); */

    for (i = 0; i < 8; i++) {
        printf("%d ", imagem[i]);
    }
    printf("\n");

/*     printf("Nome do arquivo de saida ROTULARIZAÇÃO: ");
    scanf("%s", arquivo);
    salvarImagem(arquivo, imagem2, largura, altura); */

/*     printf("Nome do arquivo de saida: ");
    printf("resultado_rot.pgm.txt\n\n");
    salvarImagem("resultado_rot.pgm.txt", imagem2, largura, altura); */

    return 0;
}