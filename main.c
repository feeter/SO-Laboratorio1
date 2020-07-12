#include "leerArgs.h"
#include "Image.h"
#include "utils.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

Image LeerImagen(int num, char *ruta);
Image ConvertirAGris(Image *orig, int i, char *ruta);
void FiltroLapleciano(const Image *orig, int i, char *ruta, char *fnameLaplaciana);
void Binarizacion(Image *orig, int i, char *ruta, int umbral);


// ejecutar: ./salida -c 2 -m test -u 120 -n 3 -b
// compiñar: make
int main(int argc, char *argv[]) {

    /* Lectura de imagen */
    // obtener cantidad de imagenes, que puede ser obtenida por argumentos
    // iterar en la ruta especificada
    int cantImgs = 0, uBinarImg = 0, uClasi = 0,  mostrarResult = 0;
    char *maskLaplaciana = "";
    char *ruta = "images/";
    maskLaplaciana = recibirArgumentos(argc, argv, &cantImgs, maskLaplaciana, &uBinarImg, &uClasi, &mostrarResult);

    //printf("cantidad imagenes: %d ; mascara laplaciana: %s ; umbral binario: %d ; umbral clasico: %d ; mostrar resultados: %d \n", cantImgs, maskLaplaciana, uBinarImg, uClasi, mostrarResult);

    // int **kernel;
    // kernel = leerArchivo(maskLaplaciana);
    // printf("%d\n", kernel[1][1]);

    pid_t nuevoProceso;

    if ((nuevoProceso = fork()) == 0){
        printf("Soy el hijo y voy a procesar las imagenes. Proceso Id de mi hijo es:%d \n", nuevoProceso);

        for (int i = 0; i < cantImgs; i++)
        {
            Image img_original;
            img_original = LeerImagen(i, ruta);

            img_original = ConvertirAGris(&img_original, i, ruta);

            FiltroLapleciano(&img_original, i, ruta, maskLaplaciana);

            Binarizacion(&img_original, i, ruta, uBinarImg);
        }
        
        exit(0);

    } else {
        printf("Soy el padre y ya lei los parametros. El proceso Hijo Id es:%d \n", nuevoProceso);
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

    return img_gris;
}

void FiltroLapleciano(const Image *orig, int i, char *ruta, char *fnameLaplaciana){
    char fileNameDest[50];
    
    // Filtro lapleciano
    Image img_lapleciano;
    Image_lapleciano(orig, &img_lapleciano, fnameLaplaciana);
    //printf("%s -> Imagen convertida a lapleciano\n" , rutaCompleta);

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