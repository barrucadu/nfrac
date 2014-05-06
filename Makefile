CC      = clang
CFLAGS  = -c -Wall -Wextra -Werror -pedantic -g -std=c99
LDFLAGS = `pkg-config --libs ncurses`
SOURCES = $(wildcard *.c)
TARGET  = nfrac

all: $(TARGET)

$(TARGET): $(SOURCES:.c=.o)
	$(CC) $(SOURCES:.c=.o) $(LDFLAGS) -o $@

clean:
	-@rm -f $(SOURCES:.c=.o) $(TARGET)
