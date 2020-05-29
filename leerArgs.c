#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "leerArgs.h"



//Entradas: Recibe
//argc: que consiste en el contador de argumentos ingresados en la linea de comandos. El nombre del programa que se ejecuta se cuenta como un argumento
//argv: arreglo con las entradas ingresadas por linea de comandos
//*n y *flag: punteros a variables que se utilizaran para pasar valores por referencia, ambos son enteros
//Funcionamiento:
//Salida: No posee retorno, ya que es una función void
char * recibirArgumentos(int argc, char *argv[], int *cantImgs, char *maskLaplaciana, int *uBinarImg, int *uClasi, int *mostrarResult)
{
    int flags, opt;
    char *aux3;
    aux3 = malloc(10*sizeof(char));
    char *auxNombre = NULL;


    if(argc < 9){//si se ingresa un numero de argumentos menor a 9, se finaliza la ejecucion del programa
        //Debe ser 9, porque el nombre del programa se considera como un argumento, 
        //siendo -c y el valor que acompañe a -c hasta -n y su valor, el argumento faltante no se considera -b (que seria el decimo argumento) 
        //porque es un flag que es opcional
        printf("Se ingreso un numero incorrecto de argumentos\n");
        fprintf(stderr, "Uso correcto: %s [-c cantidad de imagenes] [-m nombre archivo de mascara laplaciana] [-u UMBRAL para binarizar la imagen] [-n UMBRAL para clasigicacion] [-b para mostrar resultados por pantalla]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    int N = -1;
    int UMBRALB = -1;
    int UMBRALC = -1;
    flags = 0;
    //Se inicia un ciclo while hasta que se verifiquen todos los argumentos ingresados como entradas, los cuales se reciben con getopt()
    //int getopt (int argc, char *const *argv, const char *options): Siempre se le debe entregar argc y argv. El tercer argumento consiste en el formato de las entradas, ya sea
    //el caracter correspondiente a la opcion/entrada que se quiere recibir y además se debe indicar si dicha entrada estará acompañada de algún valor
    //Esto se indica con ":". Por lo tanto, "c:" quiere decir que se espera recibir la opcion -c y esta si o si debe estar acompañada de un valor
    //En cambio, la opcion "-b" no necesita estar acompañada de un valor
    while((opt = getopt(argc, argv, "bc:u:n:m:")) != -1) {
        //opt recibe el argumento leido (se hace de forma secuencial) y se ingresa a un switch
        //En caso de que opt sea -1, se dejaran de buscar entradas y por lo tanto se podrá salir del while
        switch(opt) {
            case 'b'://se busca la entrada -b, en caso de ser encontrado se setea el valor flags = 1, no se considera lo que se ingrese despues del flag -m
                flags = 1;
                break;
            case 'c': //se busca la entrada -c
                N = strtol(optarg, &aux3, 10);//se parsea el argumento ingresado junto al flag -h a entero

                if(optarg!=0 && N==0){//si no se ingresa un argumento junto a -h o si no se logra parsear el argumento ingresado, se considera como invalido
                    fprintf(stderr, "Uso correcto: %s [-c numero entero]\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'u': //se busca la entrada -u
                UMBRALB = strtol(optarg, &aux3, 10);//se parsea el argumento ingresado junto al flag -h a entero

                if(optarg!=0 && UMBRALB==0){//si no se ingresa un argumento junto a -h o si no se logra parsear el argumento ingresado, se considera como invalido
                    fprintf(stderr, "Uso correcto: %s [-u numero entero]\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'n': //se busca la entrada -n
                UMBRALC = strtol(optarg, &aux3, 10);//se parsea el argumento ingresado junto al flag -h a entero

                if(optarg!=0 && UMBRALC==0){//si no se ingresa un argumento junto a -h o si no se logra parsear el argumento ingresado, se considera como invalido
                    fprintf(stderr, "Uso correcto: %s [-n numero entero]\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'm':
                //strcpy(ruta, optarg);
                auxNombre = optarg;
                //printf ("folder file: \"%s\"\n", optarg);
                break;
            default: /* '?' */
                fprintf(stderr, "Uso correcto: %s [-c numero entero] [-m nombre archivo] [-u numero entero] [-n numero entero] [-b]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if(flags==1){//si se encontro un flag -c, se setea la variable global mostrarResult = 1, respecto al scope del proceso principal
        (*mostrarResult) = 1;
    }
    (*cantImgs) = N; //se iguala la variable n a N, para poder acceder al valor en el main
    (*uBinarImg) = UMBRALB;
    (*uClasi) = UMBRALC;
    (maskLaplaciana) = auxNombre;
    if(N<=0){
        printf("El valor que acompaña a -c debe ser mayor a 0\n");
        fprintf(stderr, "Uso correcto: %s [-c nchild]\n", argv[0]); //si el valor que acompaña a -c es negativo, se retorna un error
        exit(EXIT_FAILURE);
    }

    if(UMBRALB<=0){
        printf("El valor que acompaña a -u debe ser mayor a 0\n");
        fprintf(stderr, "Uso correcto: %s [-u nchild]\n", argv[0]); //si el valor que acompaña a -u es negativo, se retorna un error
        exit(EXIT_FAILURE);
    }

    if(UMBRALC<=0){
        printf("El valor que acompaña a -n debe ser mayor a 0\n");
        fprintf(stderr, "Uso correcto: %s [-n nchild]\n", argv[0]); //si el valor que acompaña a -n es negativo, se retorna un error
        exit(EXIT_FAILURE);
    }

    return maskLaplaciana;
}