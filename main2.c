#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 


// ejecutar: ./salida -c 2 -m test -u 120 -n 3 -b
// compiñar: make all
int main(int argc, char *argv[]) {

    
    //A null terminated array of character   
    //pointers 

    

    char *args[] = {"./salida", argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], NULL}; 
    execvp(args[0],args); 
    
    /*All statements are ignored after execvp() call as this whole  
    process(execDemo.c) is replaced by another process (EXEC.c) 
    */
    printf("Finalizando-----\n"); 
    
    return 0; 
    
   
}
