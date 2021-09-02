##################################################
# nastaveni
CFLAGS_COMMON=-pipe -Wall -ggdb
CC=gcc
CXX=g++

##################################################
# nastaveni knihoven s OpenGL
ifeq ($(OS), Windows_NT)
	CFLAGS=$(CFLAGS_COMMON) -Wl,--enable-auto-import
	LIBS=-lopengl32 -lglew32 -lSDL -lSDL_image -lassimp
	RM=del
else
	USE_GLEE=1
	CFLAGS=$(CFLAGS_COMMON) `sdl-config --cflags` -I. -IGL -DUSE_GLEE
	LIBS=-L/usr/lib `sdl-config --libs` -lGL -lSDL_image -lassimp
	RM=rm -f
endif

CXXFLAGS=$(CFLAGS)

##################################################
# obecne cile

PROJEKT=main



ifdef USE_GLEE
	DEPS=pgr.o mesh.o model.o water.o bubble.o model_shader.o bounding_box.o game_models.o drag_and_drop.o aquarium.o collision.o skybox.o GLee.o
else
	DEPS=pgr.o mesh.o model.o water.o bubble.o model_shader.o bounding_box.o game_models.o drag_and_drop.o aquarium.o collision.o skybox.o
endif

.PHONY: all projekt clean

all: projekt

run: projekt run-main

projekt: $(PROJEKT)

clean:
	$(RM) $(PROJEKT) $(DEPS) *.o

##################################################
# pravidla

%.exe: %.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.exe: %.cpp
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)


##################################################
# zavislosti

pgr.o: pgr.cpp pgr.h
mesh.o: mesh.cpp mesh.h
model.o: model.cpp model.h
water.o: water.cpp water.h
bubble.o: bubble.cpp bubble.h
model_shader.o: model_shader.cpp model_shader.h
bounding_box.o: bounding_box.cpp bounding_box.h
game_models.o: game_models.cpp game_models.h
drag_and_drop.o: drag_and_drop.cpp drag_and_drop.h
aquarium.o: aquarium.cpp aquarium.h
collision.o: collision.cpp collision.h
skybox.o: skybox.cpp skybox.h


# PROJEKT

main: main.cpp aquarium_data.h sphere.h $(DEPS)
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

run-main:
	./main



