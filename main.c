#include "leerArgs.h"
#include "Image.h"
#include "utils.h"



int main(int argc, char *argv[]) {

    /* Lectura de imagen */
    // obtener cantidad de imagenes, que puede ser obtenida por argumentos
    // iterar en la ruta especificada
    int cantImgs = 0, uBinarImg = 0, uClasi = 0,  mostrarResult = 0;
    char *maskLaplaciana = "";
    char *ruta = "images/";
    maskLaplaciana = recibirArgumentos(argc, argv, &cantImgs, maskLaplaciana, &uBinarImg, &uClasi, &mostrarResult);

    //printf("cantidad imagenes: %d ; mascara laplaciana: %s ; umbral binario: %d ; umbral clasico: %d ; mostrar resultados: %d \n", cantImgs, maskLaplaciana, uBinarImg, uClasi, mostrarResult);

    for (int i = 0; i < cantImgs; ++i) {

        char fileNameOrigen[50];
        char fileNameDest[50];

        sprintf(fileNameOrigen,"imagen_%d.jpg", i);

        char *rutaCompleta = concatenar(ruta, fileNameOrigen);

        printf("%s -> procesando\n" , rutaCompleta);


        Image img_original;

        //Image_load(&img_sky, "sky.jpg");
        Image_load(&img_original, rutaCompleta);
        ON_ERROR_EXIT(img_original.data == NULL, "Error al cargar imagen");

        printf("%s -> Imagen cargada\n" , rutaCompleta);

        // Convert the images to gray
        Image img_gris;
        Image_to_gray(&img_original, &img_gris);

        printf("%s -> Imagen convertida a gris\n" , rutaCompleta);

        // Save images
        sprintf(fileNameDest,"imagen_%d_gris.jpg", i);
        char *rutaCompletaDest = concatenar(ruta, fileNameDest);
        Image_save(&img_gris, rutaCompletaDest);

        printf("%s -> Imagen gris guardando en: %s\n" , rutaCompleta, rutaCompletaDest);

        

        // Binarizando la imagen
        Image img_binary;
        Image_to_binary(&img_original, &img_binary, uBinarImg);

        printf("%s -> Imagen binarizada\n" , rutaCompleta);

    
        sprintf(fileNameDest,"imagen_%d_binary.jpg", i);
        rutaCompletaDest = concatenar(ruta, fileNameDest);
        Image_save(&img_binary, rutaCompletaDest);

        printf("%s -> Imagen binarizada guardando en: %s\n" , rutaCompleta, rutaCompletaDest);


        // Release memory
        Image_free(&img_original);
        Image_free(&img_gris);
        Image_free(&img_binary);

    }

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
            ON_ERROR_EXIT(img_original.data == NULL, "Error al cargar imagen");

          

            Image_es_casiNegra(&img_original, uClasi, fileName);
        }
        printf("\n\n");
    }

    

}




