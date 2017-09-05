CC = g++
BLAH = $(shell uname)
MACOS_INCLUDES = -I/opt/local/include
MACOS_LIBS = -L/opt/local/lib -framework OpenGL -framework GLUT -lglew -lIL
LINUX_INCLUDES = 
LINUX_LIBS = -lGL -lGLU -lglut -lGLEW -lIL -lILU
OBJECTS = SimpleEngine.o EngineMath.o GLObjViewer/model_obj.o ObjectNode.o

ifeq ($(BLAH), Darwin)
  INCLUDES = $(MACOS_INCLUDES)
  LIBS = $(MACOS_LIBS)
else
  INCLUDES = $(LINUX_INCLUDES)
  LIBS = $(LINUX_LIBS)
endif

SimpleEngine: $(OBJECTS)
	$(CC) -g -std=c++11 -o SimpleEngine $(OBJECTS) $(LIBS)

%.o : %.cpp %.h
	$(CC) -g -std=c++11 -c $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJECTS)
