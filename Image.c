#include "Image.h"
#include "utils.h"
#include <math.h>
#include <stdbool.h> 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"



/**
 * Entradas: *img puntero donde se guardara la imagen, *fname ruta donde se encuentra la imagen fisica
 * Funcionamiento: Carga una imagen fisica en memoria
 * Salidas: puntero *img donde se ubica la imagen en memoria
*/
void Image_load(Image *img, const char *fname) {
    if((img->data = stbi_load(fname, &img->width, &img->height, &img->channels, 0)) != NULL) {
        img->size = img->width * img->height * img->channels;
        img->allocation_ = STB_ALLOCATED;
    }
}

void Image_create(Image *img, int width, int height, int channels, bool zeroed) {
    size_t size = width * height * channels;
    if(zeroed) {
        img->data = calloc(size, 1);
    } else {
        img->data = malloc(size);
    }

    if(img->data != NULL) {
        img->width = width;
        img->height = height;
        img->size = size;
        img->channels = channels;
        img->allocation_ = SELF_ALLOCATED;
    }
}


/**
 * Entradas: *img puntero a una imagen guardada en memoria, *fname ruta donde se guardara la imagen
 * Funcionamiento: Crea la imagen fisica en la ruta especificada
 * Salidas: no retorna nada, en caso de error detiene el proceso
 **/
void Image_save(const Image *img, const char *fname) {
    // Check if the file name ends in one of the .jpg/.JPG/.jpeg/.JPEG or .png/.PNG
    if(str_ends_in(fname, ".jpg") || str_ends_in(fname, ".JPG") || str_ends_in(fname, ".jpeg") || str_ends_in(fname, ".JPEG")) {
        stbi_write_jpg(fname, img->width, img->height, img->channels, img->data, 100);
    } else if(str_ends_in(fname, ".png") || str_ends_in(fname, ".PNG")) {
        stbi_write_png(fname, img->width, img->height, img->channels, img->data, img->width * img->channels);
    } else {
        ON_ERROR_EXIT(false, "");
    }
}

/**
 * Entradas: *img puntero a la imagen
 * Funcionamiento: limpia la memoria
 * Salidas: no devuelve nada
**/
void Image_free(Image *img) {
    if(img->allocation_ != NO_ALLOCATION && img->data != NULL) {
        if(img->allocation_ == STB_ALLOCATED) {
            stbi_image_free(img->data);
        } else {
            free(img->data);
        }
        img->data = NULL;
        img->width = 0;
        img->height = 0;
        img->size = 0;
        img->allocation_ = NO_ALLOCATION;
    }
}

/**
 * Entradas: *orig puntero a la imagen original, *gray puntero a la ubicacion de memoria de destino
 * Funcionamiento: convierte la imagen original en una imagen en escala de grises
 * Salidas: *gray es el puntero donde se guarda la imagen en escala de grises
 * */
void Image_to_gray(const Image *orig, Image *gray) {
    ON_ERROR_EXIT(!(orig->allocation_ != NO_ALLOCATION && orig->channels >= 3), "La imagen original debe tener al menos 3 canales.");
    int channels = orig->channels == 4 ? 2 : 1;
    Image_create(gray, orig->width, orig->height, channels, false);
    ON_ERROR_EXIT(gray->data == NULL, "Error al crear la imagen");

    for(unsigned char *p = orig->data, *pg = gray->data; p != orig->data + orig->size; p += orig->channels, pg += gray->channels) {
        *pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0);
        if(orig->channels == 4) {
            *(pg + 1) = *(p + 3);
        }
    }
}

void Image_to_sepia(const Image *orig, Image *sepia) {
    ON_ERROR_EXIT(!(orig->allocation_ != NO_ALLOCATION && orig->channels >= 3), "La imagen de entrada debe tener al menos 3 canales.");
    Image_create(sepia, orig->width, orig->height, orig->channels, false);
    ON_ERROR_EXIT(sepia->data == NULL, "Error al crear imagen");

    // Sepia filter coefficients from https://stackoverflow.com/questions/1061093/how-is-a-sepia-tone-created
    for(unsigned char *p = orig->data, *pg = sepia->data; p != orig->data + orig->size; p += orig->channels, pg += sepia->channels) {
        *pg       = (uint8_t)fmin(0.393 * *p + 0.769 * *(p + 1) + 0.189 * *(p + 2), 255.0);         // red
        *(pg + 1) = (uint8_t)fmin(0.349 * *p + 0.686 * *(p + 1) + 0.168 * *(p + 2), 255.0);         // green
        *(pg + 2) = (uint8_t)fmin(0.272 * *p + 0.534 * *(p + 1) + 0.131 * *(p + 2), 255.0);         // blue        
        if(orig->channels == 4) {
            *(pg + 3) = *(p + 3);
        }
    }
}


/**
 * Entradas: *orig imagen original, *binary puntero donde se guardara la imagen convertida, umbral valor a considerar para calcular binarizacion
 * Funcionamiento: binariza una imagen en base al umbral 
 * Salidas: guarda la imagen binarizada en la *binary
 **/
void Image_to_binary(const Image *orig, Image *binary, const int umbral) {
    ON_ERROR_EXIT(!(orig->allocation_ != NO_ALLOCATION && orig->channels >= 3), "La imagen de entrada debe tener al menos 3 canales.");
    Image_create(binary, orig->width, orig->height, orig->channels, false);
    ON_ERROR_EXIT(binary->data == NULL, "Error al crear imagen");

    

    for(unsigned char *p = orig->data, *pg = binary->data; p != orig->data + orig->size; p += orig->channels, pg += binary->channels) {
        //printf("*p : %d ; *(p + 1): %d ; *(p + 2): %d \n", *p, *(p + 1), *(p + 2));
        
        *pg = (*p > umbral) ? 255 : 0;
        *(pg + 1) = (*(p + 1) > umbral) ? 255 : 0;
        *(pg + 2) = (*(p + 2) > umbral) ? 255 : 0;

        if(orig->channels == 4) {
            *(pg + 3) = (*(p + 3) > umbral) ? 255 : 0;
        }
    }
    //printf("umbral: %d \n", umbral);
}

void Image_to_binary_from_gray(const Image *orig, Image *binary, const int umbral){
    
    //ON_ERROR_EXIT(!(orig->allocation_ != NO_ALLOCATION && orig->channels >= 3), "La imagen de entrada debe tener al menos 3 canales.");
    Image_create(binary, orig->width, orig->height, orig->channels, false);
    ON_ERROR_EXIT(binary->data == NULL, "Error al crear imagen");


    for(unsigned char *p = orig->data, *pg = binary->data; p != orig->data + orig->size; p += orig->channels, pg += binary->channels) {
        //printf("*p : %d  \n", *p);

        *pg = (*p > umbral) ? 255 : 0;
        
        // *pg = (*p > umbral) ? 255 : 0;
        // *(pg + 1) = (*(p + 1) > umbral) ? 255 : 0;
        // *(pg + 2) = (*(p + 2) > umbral) ? 255 : 0;

    }
    //printf("umbral: %d \n", umbral);
}

/**
 * Entradas: *orig puntero a la imagen a categorizar, umbral es el porcentaje de categorizacion, *nombre nombre del archivo
 * Funcionamiento: Clasifica una imagen si es o no nearly black segun el umbral en %
 * Salidas: despliega una tabla en consola, columna 1 es "image", columna 2 indica si es o no "nearly black"
**/
void Image_es_casiNegra(const Image *orig, const int umbral, const char *nombre) {
    ON_ERROR_EXIT(!(orig->allocation_ != NO_ALLOCATION && orig->channels >= 3), "La imagen de entrada debe tener al menos 3 canales.");

    int pn = 0;
    int pt = 0;

    for(unsigned char *p = orig->data; p != orig->data + orig->size; p += orig->channels) {
        //printf("*p : %d ; *(p + 1): %d ; *(p + 2): %d \n", *p, *(p + 1), *(p + 2));
        bool R, G, B;

        R = (*p == 0);
        G = (*(p + 1) == 0);
        B = (*(p + 2) == 0);
        
        if (R && G && B) {
            pn++;
        }

        pt++;
    }



    int porcentaje = (pn*100)/pt;
    bool esCasiNegra;
    
    esCasiNegra = porcentaje >= umbral;

    //printf("pn: %d; pt: %d ; %d%% \n", pn, pt, porcentaje);

    printf("|  %s   |       %s      |\n", nombre, esCasiNegra ? "yes" : "no ");


    //printf("umbral casi negra: %d \n", umbral);
}


/**
 * Entradas: *orig puntero a la imagen a filtrar
 * Funcionamiento: filtra imagen con arreglo lapleciano (esta version no considera nombre obtenido del argumento)
 * Salidas: guarda imagen filtrada en *lapleciano
**/
void Image_lapleciano(const Image *orig, Image *lapleciano, const char *fname) {
    
    Image_create(lapleciano, orig->width, orig->height, orig->channels, false);
    ON_ERROR_EXIT(lapleciano->data == NULL, "Error al crear imagen");

     
    int image_array_2d[orig->height][orig->width];
    
    int i, j, m, n;

    
    int origI = 0;
    //printf("height = %d; width = %d \n", orig->height, orig->width);
    for (i = 0; i < orig->height; ++i)
        for (j = 0; j < orig->width; ++j)
        {
            //printf("i = %d; j = %d; origI = %d \n", i, j, origI);
            image_array_2d[i][j] = orig->data[origI];
            //printf("test \n");
            origI = origI + 1;
        }

/*  mostrar arreglo
    for(i = 0; i < orig->height; i++)
    {
        printf("\n");
        for(j = 0; j < orig->width; j++)
        {
            printf("%d\t", image_array_2d[i][j]);
        }
    }
*/

    // int kernel[3][3] = {
    //     {0, 1, 0},
    //     {1, -4, 1},
    //     {0, 1, 0}
    // };
    int **kernel;
    kernel = leerArchivo(fname);
    //printf("%d\n", kernel[1][1]);

    int img_array_generada[orig->height][orig->width];

    int image_h = orig->height;
    int image_w = orig->width;

    int kernel_h = 3;
    int kernel_w = 3;

    int h = 1;
    int w = 1;

    //printf("altura: %d ; anchura: %d \n", image_h, image_w);
    // proceso de convolusion entre el kernel y la imagen como arreglo 2d
    for(i = 1; i < image_h - 1; i++){
        for(j = 1; j < image_w - 1; j++){
            int sum = 0;

            for (m = 0; m < kernel_h; m++){
                for (n = 0; n < kernel_w; n++)
                {
                    //printf("image_array_2d: %d\n", image_array_2d[i - h + m][j - w + n]);
                    sum = sum + kernel[m][n] * image_array_2d[i - h + m][j - w + n];
                    //printf("kernel[%d][%d]: %d; image_array_2d[%d][%d]: %d; sum: %d\n", m, n, kernel[m][n], i-h+m, j-w+n, image_array_2d[i - h + m][j - w + n], sum);
                    
                }
            }
            //printf("sum total: %d\n", sum);
            img_array_generada[j][i] = sum;
            //printf("|  %d   |\n", image_w * i + j);
        }
    }
/* imprimir resultado de arreglo 2d 
    for(i = 0; i < image_w; i++)
    {
        printf("\n");
        for(j = 0; j < image_h; j++)
        {
            printf("%d\t", img_array_generada[i][j]);
        }
    }
*/

// transforma de arreglo 2d a arreglo 1d guardandolo en data del lapleciano
    int l = 0;
    for(i = 0;i < image_h;i++)
    {
        for(j = 0;j < image_w;j++)
        {
            lapleciano->data[l]=img_array_generada[j][i];
            l++;
        }
    }
}


/**
 * Entradas: *fname -> nombre del archivo que contiene el kernel
 * Funcionamiento: Guarda en memoria, un arreglo de int 3x3 el kernel del archivo que debe ser si o si 3x3
 * Salidas: Retorna el arreglo con los datos
**/
int **leerArchivo(const char *fname){

    //printf("leyendo array\n");

    FILE* f;
    int height, width, ii, jj;
    

    if((f = fopen(fname, "r")) == NULL)
        exit(1);

    if(fscanf(f, "%d%d", &height, &width) != 2)
        exit(1);

    //int array[height][width];
    int **array;
    array =  malloc(sizeof(int*) * 3);

    for(ii = 0; ii < 3; ii++) {
        array[ii] = malloc(sizeof(int*) * 3);
    }

    for(jj=0; jj<height; jj++)
        for(ii=0; ii<width; ii++)
            if(fscanf(f, "%d", &array[jj][ii]) != 1)
                exit(1);
    fclose(f);

    // for(jj=0; jj<height; jj++){
    //     for(ii=0; ii<width; ii++)
    //         printf ("%d", array[jj][ii]);
    //     printf("\n");
    // }

    return array;
 
    //for (--i; i >= 0; --i)
        //printf("num[%d] = %d\n", i, nums[i]);

}
