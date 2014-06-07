#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

//comentar essa linha abaixo para nao imrpimir mensagens
#define DEBUG_MODE TRUE
#define BUFFER_SIZE 50

using namespace std;

/*
imprime as mensagens de saida se DEBUG_MODE estiver definido
a macro __func__ imprime o nome da função que chamou o log
*/

#ifdef DEBUG_MODE
#define LOG(str) do { cout << "[" << __func__ <<"]=> " << str << endl; } while(false)
#else
#define LOG(str) do { } while(false)
#endif

typedef struct Image_Matrix{
    int width;
    int height;
    int max_color;
    int *r;
    int *g;
    int *b;
}Image_Matrix;


//cria uma imagem PPM com tamanho W, altura H e maximo de cor igual a max
Image_Matrix *newPPM(int w, int h, int max){
    Image_Matrix *tmp;
    tmp = new Image_Matrix();
    tmp->width=w;
    tmp->height=h;
    tmp->max_color=max;
    tmp->r = (int*) malloc(sizeof(int)*w*h);
    tmp->g = (int*) malloc(sizeof(int)*w*h);
    tmp->b = (int*) malloc(sizeof(int)*w*h);

    LOG("Criando imagem em branco de tamanho " << w <<'x'<<h);
    return tmp;
}

Image_Matrix *smoothPPM(Image_Matrix *in){
    LOG("Aplicando filtro Smooth");
    Image_Matrix *out;
    out=newPPM(in->width, in->height, in->max_color);

    int w = out->width;
    int h = out->height;
    int pos;
    /*faz a aplicação do filtro da imagem */
    for(pos=0;pos<w*h;pos++){
        out->r[pos]=in->r[pos];
        out->g[pos]=in->g[pos];
        out->b[pos]=in->b[pos];
    }

    return out;
}

//le uma imagem PPM
Image_Matrix *readPPM(string image){
    Image_Matrix *tmp;
    tmp = new Image_Matrix();

    char buffer[BUFFER_SIZE];
    FILE *ppmfile;
    ppmfile = fopen(image.c_str(),"r");
    //le a primeira linha e descarta
    fgets(buffer,BUFFER_SIZE, ppmfile);

    /* le uma linha e verifica se não é comentario */
    /* referencia http://stackoverflow.com/questions/2693631/read-ppm-file-and-store-it-in-an-array-coded-with-c */
    int c;
    c=getc(ppmfile);
    while(c=='#'){
        while(getc(ppmfile)!= '\n');
        c = getc(ppmfile);
    }
    //volta o ultimo caracter para pegar altura e largura
    ungetc(c, ppmfile);

    /* pega altura e largura da imagem*/
    fscanf(ppmfile,"%d %d",&tmp->width, &tmp->height);

    int w = tmp->width;
    int h = tmp->height;
    /*aloca a matriz de posições*/
    tmp->r = (int*) malloc(sizeof(int)*w*h);
    tmp->g = (int*) malloc(sizeof(int)*w*h);
    tmp->b = (int*) malloc(sizeof(int)*w*h);

    /*pega o max color da imagem*/
    fscanf(ppmfile,"%d",&tmp->max_color);

    /* le as trinas rgb da imagem*/
    int pos;
    for(pos=0;pos<w*h;pos++){
        fscanf(ppmfile,"%d %d %d",&tmp->r[pos], &tmp->g[pos], &tmp->b[pos]);
    }

    LOG("Imagem '" << image << "' de tamanho " << tmp->width <<'x'<< tmp->height << " lida");

    fclose(ppmfile);
    return tmp;
}

void writePPM(string filename, Image_Matrix *img){
    FILE *ppmfile;

    ppmfile = fopen(filename.c_str(), "w");
    /* escreve o tipo de imagem */
    fprintf(ppmfile, "P3\n");

    /* escreve o nome da imagem como comentario */
    fprintf(ppmfile, "# %s\n", filename.c_str());

    /* escreve as dimensões da imagem no arquivo */
    fprintf(ppmfile, "%d %d\n", img->width, img->height);
    /* escreve o max color na imagem */
    fprintf(ppmfile, "%d\n", img->max_color);

    int w = img->width;
    int h = img->height;
    int pos;
    /* escreve as trinas rgb da imagem */
    for(pos=0;pos<w*h;pos++){
        fprintf(ppmfile,"%d %d %d",img->r[pos], img->g[pos], img->b[pos]);
        if(pos%img->width!=0)
            fprintf(ppmfile, " ");
        else
            fprintf(ppmfile, "\n");

    }
    fprintf(ppmfile,"\n");

    LOG("Imagem '" << filename << "' salva com sucesso");
}

void deletePPMmatrix(Image_Matrix *image){
    LOG("Limpando matrizes da imagem e deletando ponteiro");
    free(image->r);
    free(image->g);
    free(image->b);
    delete(image);
}

int main(int argc, char *argv[])
{
    string filename("in.ppm");
    Image_Matrix *in;
    Image_Matrix *out;

    LOG("Exibindo mensagens de saida no formato [nome_funcao]: mensagem");

    in = readPPM(filename);

    out = smoothPPM(in);

    writePPM("out.ppm",out);

    //faz um free nas matrizes das imagens e deleta o ponteiros
    deletePPMmatrix(in);
    deletePPMmatrix(out);

    return 0;
}