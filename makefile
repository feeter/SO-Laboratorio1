salida_out = salida


salida_headers = Image.h leerArgs.h 
salida_source  = $(salida_headers:.h=.c) main.c
salida_objects = $(salida_source:.c=.o)

CC     = gcc
CFLAGS = -Wall

depends = .depends

build : $(salida_out) 

$(salida_out) : $(salida_objects)
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(objects) :
	$(CC) $(CFLAGS) -c -o $@ $*.c

$(depends) : $(salida_source) $(salida_headers)
	@$(CC) -MM $(salida_source) > $@


all: program2

program2:
	$(CC) $(CFLAGS) -o salida2 main2.c

clean :
	$(RM) $(salida_out) $(salida_objects) $(zipfile) $(depends)

include $(depends)
