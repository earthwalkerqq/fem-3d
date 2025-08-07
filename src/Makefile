CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99
INCLUDES = -I../dependencies/glew/2.2.01/include \
           -I../dependencies/glfw/3.4/include \
           -I../dependencies/glut/include
LIBS = -L../dependencies/glew/2.2.01/lib \
       -L../dependencies/glfw/3.4/lib \
       -framework OpenGL \
       -framework GLUT \
       -framework Cocoa \
       -lGLEW -lglfw -lm

SOURCES = main.c fem3d.c formation_mtrx3d.c draw.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = fem3d

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -DDEBUG
debug: $(TARGET) 