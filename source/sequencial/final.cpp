#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

//comentar essa linha abaixo para nao imrpimir mensagens
#define DEBUG_MODE
#define BUFFER_SIZE 50
#define MASK_SIZE 3

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
    int **r;
    int **g;
    int **b;
}Image_Matrix;


//cria uma imagem PPM com tamanho W, altura H e maximo de cor igual a max
Image_Matrix *newPPM(int w, int h, int max){
    Image_Matrix *tmp;
    tmp = new Image_Matrix();
    tmp->width=w;
    tmp->height=h;
    tmp->max_color=max;
    tmp->r = (int**) malloc(sizeof(int*)*h);
    tmp->g = (int**) malloc(sizeof(int*)*h);
    tmp->b = (int**) malloc(sizeof(int*)*h);
    for(int i=0;i<w;i++){
        tmp->r[i] = (int*) malloc(sizeof(int)*w);
        tmp->g[i] = (int*) malloc(sizeof(int)*w);
        tmp->b[i] = (int*) malloc(sizeof(int)*w);
    }

    LOG("Criando imagem em branco de tamanho " << w <<'x'<<h);
    return tmp;
}

Image_Matrix *smoothPPM(Image_Matrix *in){
    LOG("Aplicando filtro Smooth");
    Image_Matrix *out;
    out=newPPM(in->width, in->height, in->max_color);

    /*int w = out->width;
    int h = out->height;*/

    int meio=(MASK_SIZE-1)/2;
    /*faz a aplicação do filtro da imagem */
    for(int y=0;y<in->height;y++){
        for(int x=0;x<in->width;x++){

            int sumr=0;
            int sumg=0;
            int sumb=0;
            for(int i=-meio;i<meio+1;i++)
                for(int j=-meio;j<meio+1;j++){
                    if(x+j>=0 && x+j<in->width && y+i>=0 && y+i<in->height){
                        sumr+=in->r[x+j][y+i];
                        sumg+=in->g[x+j][y+i];
                        sumb+=in->b[x+j][y+i];
                    }
                }
            int d = MASK_SIZE*MASK_SIZE;
            out->r[x][y]=sumr/d;
            out->g[x][y]=sumg/d;
            out->b[x][y]=sumb/d;
        }
    }
    return out;
}

//le uma imagem PPM
Image_Matrix *readPPM(string image){
    Image_Matrix *tmp;
    int w,h;
    int max;

    char buffer[BUFFER_SIZE];
    FILE *ppmfile;
    ppmfile = fopen(image.c_str(),"r");
    if(ppmfile==NULL){
        LOG("Falha ao abrir o arquivo '" << image << "'. Saindo...");
        exit(1);
    }

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
    fscanf(ppmfile,"%d %d",&w, &h);

    /*pega o max color da imagem*/
    fscanf(ppmfile,"%d",&max);

    /*cria a imagem com os vetores RGB alocados */
    tmp = newPPM(w,h,max);

    /* le as trinas rgb da imagem*/
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            fscanf(ppmfile,"%d %d %d",&tmp->r[x][y], &tmp->g[x][y], &tmp->b[x][y]);
        }
    }

    LOG("Imagem '" << image << "' de tamanho " << tmp->width <<'x'<< tmp->height << " lida");

    fclose(ppmfile);
    return tmp;
}

void writePPM(string filename, Image_Matrix *img){
    FILE *ppmfile;

    /* abre o arquivo e verifica se abriu corretamente */
    ppmfile = fopen(filename.c_str(), "w");
    if(ppmfile==NULL){
        LOG("Falha ao abrir o arquivo '" << filename << "'. Saindo...");
        exit(1);
    }

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

    /* escreve as trinas rgb da imagem */
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            fprintf(ppmfile,"%d %d %d ",img->r[x][y], img->g[x][y], img->b[x][y]);
        }
        fprintf(ppmfile, "\n");
    }
    fclose(ppmfile);
    LOG("Imagem '" << filename << "' salva com sucesso");
}

void deletePPMmatrix(Image_Matrix *image){
    LOG("Limpando matrizes da imagem e deletando ponteiro");
    for(int i=0;i<image->height;i++){
        free(image->r[i]);
        free(image->g[i]);
        free(image->b[i]);
    }

    free((image->r));
    free((image->g));
    free((image->b));
    delete(image);
}

int main(int argc, char *argv[])
{
    string filename("in.ppm");
    Image_Matrix *in;
    Image_Matrix *out;

    LOG("Exibindo mensagens de saida no formato [nome_funcao]: mensagem");

    /* pega uma imagem caso seja passada como argumento, caso contrario continua com a in.ppm definida anteriormente */
    if(argc>1)
        filename.assign(argv[1]);


    in = readPPM(filename);

    out = smoothPPM(in);

    writePPM("out.ppm",out);

    //faz um free nas matrizes das imagens e deleta o ponteiros
    deletePPMmatrix(in);
    deletePPMmatrix(out);

    return 0;
}