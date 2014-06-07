#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

//comentar essa linha abaixo para nao imrpimir mensagens
#define DEBUG_MODE TRUE
#define BUFFER_SIZE 50

using namespace std;

//imprime as mensagens de saida se DEBUG_MODE estiver definido
#ifdef DEBUG_MODE
#define DEBUG(str) do { cout << "DEBUG: " << str << endl; } while(false)
#else
#define DEBUG(str) do { } while(false)
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

    DEBUG("Criando imagem de tamanho " << w <<'x'<<h);
    return tmp;
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
    DEBUG("firstline: " << buffer);

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
    DEBUG("size: " << tmp->width << "x" << tmp->height);

    int w = tmp->width;
    int h = tmp->height;
    /*aloca a matriz de posições*/
    tmp->r = (int*) malloc(sizeof(int)*w*h);
    tmp->g = (int*) malloc(sizeof(int)*w*h);
    tmp->b = (int*) malloc(sizeof(int)*w*h);

    /*pega o max color da imagem*/
    fscanf(ppmfile,"%d",&tmp->max_color);
    DEBUG("max color: " << tmp->max_color);

    /* pega o valor max color */
    for(int lim=0,pos=0;lim<w*h;lim=lim+3,pos++){
        int r,g,b;
        fscanf(ppmfile,"%d %d %d",&tmp->r[pos], &tmp->g[pos], &tmp->b[pos]);
    }

    DEBUG("Imagem '" << image << "' de tamanho " << tmp->width <<'x'<< tmp->height << " lida");

    fclose(ppmfile);
    return tmp;
}
void deletePPM(Image_Matrix *image){
    DEBUG("Limpando matrizes da imagem e deletando ponteiro");
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

    DEBUG("Exibindo mensagens de saida.");

    in = readPPM(filename);
    out = newPPM(in->width,in->height,in->max_color);

    //faz um free nas matrizes das imagens e deleta o ponteiros
    deletePPM(in);
    deletePPM(out);

    return 0;
}