# compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# nome do executavel e arquivos fonte
TARGET = gerenciador
SRC = mainsimulacao.c gerenciador.c 
# regra padrao
all: $(TARGET)

# regra para criar o executavel
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# regra para limpar os arquivos objeto e o executavel
clean:
	rm -f $(TARGET) *.o