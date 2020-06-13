#include "leerArgs.h"
#include "Image.h"
#include "utils.h"


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

    for (int i = 0; i < cantImgs; ++i) {

        char fileNameOrigen[50];
        char fileNameDest[50];

        sprintf(fileNameOrigen,"imagen_%d.jpg", i);

        char *rutaCompleta = concatenar(ruta, fileNameOrigen);

        printf("%s -> procesando\n" , rutaCompleta);


        Image img_original;


        Image_load(&img_original, rutaCompleta);
        ON_ERROR_EXIT(img_original.data == NULL, "Error al cargar imagen");

        printf("%s -> Imagen cargada\n" , rutaCompleta);

        // Convertir imagen a escala de grises
        Image img_gris;
        Image_to_gray(&img_original, &img_gris);

        printf("%s -> Imagen convertida a gris\n" , rutaCompleta);

        // Guardar imagenes
        sprintf(fileNameDest,"imagen_%d_gris.jpg", i);
        char *rutaCompletaDest = concatenar(ruta, fileNameDest);
        Image_save(&img_gris, rutaCompletaDest);

        printf("%s -> Imagen gris guardando en: %s\n" , rutaCompleta, rutaCompletaDest);

        // Filtro lapleciano
        Image img_lapleciano;
        Image_lapleciano(&img_gris, &img_lapleciano);
        printf("%s -> Imagen convertida a lapleciano\n" , rutaCompleta);

        sprintf(fileNameDest,"imagen_%d_lapleciano.jpg", i);
        rutaCompletaDest = concatenar(ruta, fileNameDest);
        Image_save(&img_lapleciano, rutaCompletaDest);

        printf("%s -> Imagen gris guardando en: %s\n" , rutaCompleta, rutaCompletaDest);

        // Fin Filtro Lapleciano
        

        // Binarizando la imagen
        Image img_binary;
        //Image_to_binary(&img_original, &img_binary, uBinarImg);
        Image_to_binary_from_gray(&img_gris, &img_binary, uBinarImg); // binarizado por la escala de grises


        printf("%s -> Imagen binarizada\n" , rutaCompleta);

    
        sprintf(fileNameDest,"imagen_%d_binary.jpg", i);
        rutaCompletaDest = concatenar(ruta, fileNameDest);
        Image_save(&img_binary, rutaCompletaDest);

        printf("%s -> Imagen binarizada guardando en: %s\n" , rutaCompleta, rutaCompletaDest);


        // Liberar memoria
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




