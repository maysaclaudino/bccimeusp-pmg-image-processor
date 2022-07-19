#include <stdio.h>
#include <stdlib.h>

/* gcc -ansi -Wall -pedantic -o ep3 ep3.c */

void print_image(int *image, int largura, int altura) {
    int i, j;

    for (i = 0; i < altura; i++) {
        for (j = 0; j < largura; j++) {
            printf("%3d ", image[i*largura + j]);
        }
        printf("\n");
    }
}

int *load_image_from_file(char *arquivo, int *largura, int *altura) {
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

void save_image_to_file(char *arquivo, int *imagem, int largura, int altura) {
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

int *add_border(int *imagem, int largura, int altura) {
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

int *remove_border(int *imagemComBorda, int larguraSemBorda, int alturaSemBorda) {
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
                    print_image(imagem, 14, 5); */
                    substituiVizinhos(imagem, i, j, qtdComponentes, largura);
            }
        }
    }
}

int *label_components(int *imagem, int largura, int altura) {
    int i, j, qtdComponentes; 
    int *imagemComBorda;
    int *imagemRotulada;

    imagemComBorda = add_border(imagem, largura, altura);
/*     print_image(imagemComBorda, largura+2, altura+2); */

    qtdComponentes = 1;
    for (i = 1; i < (altura+2); i++) {
        for (j = 1; j < (largura+2); j++) {
            if (imagemComBorda[i*(largura+2)+j] == 255) {
                substituiVizinhos(imagemComBorda, i, j, qtdComponentes, (largura+2));
                qtdComponentes++;
            }
        }
    }

    imagemRotulada = remove_border(imagemComBorda, largura, altura);

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

    elementosVizinhos = malloc(sizeof(int)*(2*valor+1)*(2*valor+1));

    k = 0;
    for (i = linha-valor; i <= linha+valor; i++) {
        for (j = coluna-valor; j <= coluna+valor; j++) {
            elementosVizinhos[k] = imagem[i*largura+j];
            k++;
        }
    }

    return elementosVizinhos;
}

void swap(int *a, int *b) {
    int t;

    t = *a;
    *a = *b;
    *b = t;
}

void quick_sort(int *vetor, int inicio, int fim) {
    int i, pivot, indicePivot, cont;

    if (inicio < fim) {
        indicePivot = inicio;
        pivot = vetor[indicePivot];

        cont = 0;
        for (i = indicePivot+1; i <= fim; i++) {
            if (vetor[i] <= pivot) {
                cont++;
                swap(&vetor[i], &vetor[indicePivot+cont]);
            }
        }
        swap(&vetor[indicePivot], &vetor[indicePivot+cont]);
        indicePivot = indicePivot+cont;

        quick_sort(vetor, inicio, indicePivot-1);
        quick_sort(vetor, indicePivot+1, fim);
    }
}

int *filter_image(int *imagem, int largura, int altura, int dimensao, int tipo) {
    int i, j, novaLargura;
    int *imagemAlargada;
    int *imagemFiltrada;
    int *vizinhos;

    novaLargura = largura + dimensao-1;

    imagemFiltrada = malloc(sizeof(int)*largura*altura);

    imagemAlargada = alargaImagem(imagem, largura, altura, dimensao/2);

    for (i = dimensao/2; i < altura+(dimensao/2); i++) {
        for (j = dimensao/2; j < largura+(dimensao/2); j++) {
            vizinhos = coletaVizinhos(imagemAlargada, novaLargura, i, j, dimensao/2);
            quick_sort(vizinhos, 0, (dimensao*dimensao)-1);
            if (tipo == 1) {
                imagemFiltrada[(i-(dimensao/2)) * largura + (j-(dimensao/2))] = vizinhos[0];
            }
            else if (tipo == 2) {
                imagemFiltrada[(i-(dimensao/2)) * largura + (j-(dimensao/2))] = vizinhos[((dimensao*dimensao)/2)];
            }
            else if (tipo == 3) {
                imagemFiltrada[(i-(dimensao/2)) * largura + (j-(dimensao/2))] = vizinhos[(dimensao*dimensao)-1];
            }
        }
    }

    free(imagemAlargada);
    imagemAlargada = NULL;

    return imagemFiltrada;
}

int *contorno(int *imagem, int largura, int altura) {
    int i, j, index;
    int *contorno;
    int *filtro;

    filtro = filter_image(imagem, largura, altura, 3, 1);

    contorno = malloc(sizeof(int)*largura*altura);

    for (i = 0; i < altura; i++) {
        for (j = 0; j < largura; j++) {
            index = i*largura+j;
            contorno[index] = imagem[index] - filtro[index];
        }
    }

    free(filtro);
    filtro = NULL;

    return contorno;
}

int main ()
{
/*     int i; */
    int *imagem;
    int *imagem2;
    int largura, altura;

    char arquivo[50];
    printf("Nome do arquivo de entrada: ");
    scanf("%s", arquivo);
    imagem = load_image_from_file(arquivo, &largura, &altura);

/*     printf("Nome do arquivo de entrada: ");
    printf("imagemTeste.txt\n\n");
    imagem = load_image_from_file("imagemTeste.txt", &largura, &altura); */

/*     imagem2 = invert_image(imagem, largura, altura); */

/*     imagem2 = binarize(imagem, largura, altura, 127); */

/*     printf("Nome do arquivo de saida INVERSÃO: ");
    scanf("%s", arquivo);
    save_image_to_file(arquivo, imagem2, largura, altura); */

/*     imagem2 = label_components(imagem2, largura, altura); */

/*     imagem2 = alargaImagem(imagem, largura, altura, 2); */

    imagem2 = contorno(imagem, largura, altura);
    printf("----------- IMAGEM FINAL -----------\n");
/*     print_image(imagem2, largura, altura); */

/*     int imagem[8] = {3, 5, 6, 4, 2, 3, 6, 1};
    quick_sort(imagem, 0, 7);
    printf("----------- IMAGEM FINAL -----------\n");

    for (i = 0; i < 8; i++) {
        printf("%d ", imagem[i]);
    }
    printf("\n"); */

/*     printf("Nome do arquivo de saida ROTULARIZAÇÃO: ");
    scanf("%s", arquivo);
    save_image_to_file(arquivo, imagem2, largura, altura); */

    printf("Nome do arquivo de saida: ");
    printf("resultado.pgm.txt\n\n");
    save_image_to_file("resultado.pgm", imagem2, largura, altura);

    return 0;
}