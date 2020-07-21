#include "leerArgs.h"
#include "Image.h"
#include "utils.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

Image LeerImagenByName(char *ruta, char *nombre);
Image LeerImagen(int num, char *ruta);
Image ConvertirAGris(Image *orig, int i, char *ruta);
void FiltroLapleciano(const Image *orig, int i, char *ruta, char *fnameLaplaciana);
void Binarizacion(Image *orig, int i, char *ruta, int umbral);


// ejecutar: ./salida -c 2 -m test -u 120 -n 3 -b
// compilar: make all
int main(int argc, char *argv[]) {
    printf("Iniciando main\n"); 
    /* Lectura de imagen */
    // obtener cantidad de imagenes, que puede ser obtenida por argumentos
    // iterar en la ruta especificada
    int cantImgs = 0, uBinarImg = 0, uClasi = 0,  mostrarResult = 0;
    char *maskLaplaciana = "";
    char *ruta = "images/";
    //char *input_str = "";
    
    maskLaplaciana = recibirArgumentos(argc, argv, &cantImgs, maskLaplaciana, &uBinarImg, &uClasi, &mostrarResult);

    //printf("cantidad imagenes: %d ; mascara laplaciana: %s ; umbral binario: %d ; umbral clasico: %d ; mostrar resultados: %d \n", cantImgs, maskLaplaciana, uBinarImg, uClasi, mostrarResult);

    // int **kernel;
    // kernel = leerArchivo(maskLaplaciana);
    // printf("%d\n", kernel[1][1]);

    pid_t pidNearlyBlack;

    if ((pidNearlyBlack = fork()) == 0) {
        //printf("Soy el hijo y voy a procesar las imagenes. Proceso Id de mi hijo es:%d \n", pidNearlyBlack);

        for (int i = 0; i < cantImgs; i++)
        {
            int miPipeFd[2];
            pid_t pidProcessImg;
            int ret;
            ret = pipe(miPipeFd);
            char buffer[512] = "";
            Image img_original;

            if (ret == -1){
                perror("pipe");
                exit(1);
            }

            pidProcessImg = fork();

            if (pidProcessImg == 0){

                // soy el hijo

                img_original = LeerImagen(i, ruta);

                char input_str[50];
                sprintf(input_str,"imagen_%d.jpg", i);

                Image gris;
                gris = ConvertirAGris(&img_original, i, ruta);

                //printf("nombre: %s; len: %i\n", input_str, strlen(input_str));

                close(miPipeFd[0]);
                //write(miPipeFd[1], maskLaplaciana, strlen(maskLaplaciana));
                write(miPipeFd[1], input_str, strlen(input_str));
                //write(miPipeFd[1], &gris, sizeof(gris));
                close(miPipeFd[1]); 
                
                //printf("\nfin proceso imagen conversion a gris\n");
                Image_free(&img_original);
                Image_free(&gris);
                //exit(0);
                
            } else {
                // soy el padre

                close(miPipeFd[1]); 
                read(miPipeFd[0], buffer, 512); 
                //read(miPipeFd[0], &img_gris, sizeof(img_gris));
                close(miPipeFd[0]); 
                
                printf("Desde pipe: %s\n", buffer);

                img_original = LeerImagenByName(ruta, buffer);
                img_original = ConvertirAGris(&img_original, i, ruta);

                // printf("img largo: %d ; i: %d ; ruta: %s ; mask: %s  \n", img_convertida.height, i, ruta, maskLaplaciana);

                //printf("imagen: %d\n", img_gris.channels);

                FiltroLapleciano(&img_original, i, ruta, maskLaplaciana);

                Binarizacion(&img_original, i, ruta, uBinarImg);


                // Image img_original;
                // img_original = LeerImagen(i, ruta);

                // img_original = ConvertirAGris(&img_original, i, ruta);

                // FiltroLapleciano(&img_original, i, ruta, maskLaplaciana);

                // Binarizacion(&img_original, i, ruta, uBinarImg);

                
               
            }

            
        }
        
       
        exit(0); // hijo de pidNearlyBlack

    } 
        

    //exit(0);

    // muestra tabla de nearly black
    if (mostrarResult == 1)
    {
        printf("\n\n");
        printf("|    image    |  nearly black  |\n");
        printf(" ------------------------------\n");
        for (int i = 0; i < cantImgs; ++i) {

            char fileName[50];

            sprintf(fileName,"imagen_%d", i);

            char *fullName = concatenar(fileName, ".jpg");
            

            char *rutaCompleta = concatenar(ruta, fullName);
            Image img_original;

            Image_load(&img_original, rutaCompleta);
            //ON_ERROR_EXIT(img_original.data == NULL, "Error al cargar imagen");

          

            Image_es_casiNegra(&img_original, uClasi, fileName);
        }
        printf("\n\n");
    }

    return 0;
   
}

Image LeerImagenByName(char *ruta, char *nombre) {
    
    char *rutaCompleta = concatenar(ruta, nombre);

    //printf("%s -> procesando\n" , rutaCompleta);

    Image tmp;

    Image_load(&tmp, rutaCompleta);

    return tmp;
}

Image LeerImagen(int num, char *ruta){
    char fileNameOrigen[50];

    sprintf(fileNameOrigen,"imagen_%d.jpg", num);

    char *rutaCompleta = concatenar(ruta, fileNameOrigen);

    //printf("%s -> procesando\n" , rutaCompleta);

    Image tmp;

    Image_load(&tmp, rutaCompleta);

    return tmp;
}

Image ConvertirAGris(Image *orig, int i, char *ruta){
    char fileNameDest[50];
    Image img_gris;
    Image_to_gray(orig, &img_gris);

    //printf("%s -> Imagen convertida a gris\n" , rutaCompleta);

    // Guardar imagenes
    sprintf(fileNameDest,"imagen_%d_gris.jpg", i);
    
    

    char *rutaCompletaDest = concatenar(ruta, fileNameDest);
    Image_save(&img_gris, rutaCompletaDest);


    //printf("%s -> Imagen convertida a gris\n" , fileNameDest);
    return img_gris;
}

void FiltroLapleciano(const Image *orig, int i, char *ruta, char *fnameLaplaciana){
    //printf("img largo: %d ; i: %d ; ruta: %s ; mask: %s \n", orig->height, i, ruta, fnameLaplaciana);

    char fileNameDest[50];
    
    // Filtro lapleciano
    Image img_lapleciano;
    Image_lapleciano(orig, &img_lapleciano, fnameLaplaciana);

    sprintf(fileNameDest,"imagen_%d_lapleciano.jpg", i);

    

    char *rutaCompletaDest = concatenar(ruta, fileNameDest);
    Image_save(&img_lapleciano, rutaCompletaDest);
}

void Binarizacion(Image *orig, int i, char *ruta, int umbral){
    char fileNameDest[50];
    // Binarizando la imagen
    Image img_binary;
    //Image_to_binary(&img_original, &img_binary, uBinarImg);
    Image_to_binary_from_gray(orig, &img_binary, umbral); // binarizado por la escala de grises


    //printf("%s -> Imagen binarizada\n" , rutaCompleta);


    sprintf(fileNameDest,"imagen_%d_binary.jpg", i);
    char *rutaCompletaDest = concatenar(ruta, fileNameDest);
    Image_save(&img_binary, rutaCompletaDest);

    //printf("%s -> Imagen binarizada guardando en: %s\n" , rutaCompleta, rutaCompletaDest);

}