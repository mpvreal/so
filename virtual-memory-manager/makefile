#	Projeto
PROJ = simulador

#	.c
SOURCES = $(wildcard src/*.c)

#	.h
HEADERS = $(wildcard src/*.h)

#	.o
OBJECTS = $(patsubst src/%.c, src/%.o, $(SOURCES))

#	Compilador
CC = gcc

#	Flags
CC_FLAGS = 	-fstack-protector-all	\
		   				-fstack-check			\
		   				-std=c99 \
						-g

#	Linkagem dos arquivos .o
$(PROJ): $(OBJECTS) $(HEADERS)
	$(CC) $^ -o $@

# Compilação de cada arquivo .c em .o
%.o: %.cpp %.hpp
	$(CC) -c $< -o $@ $(CC_FLAGS)

clean:
	rm $(OBJECTS) $(PROJ)