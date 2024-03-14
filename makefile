CFLAGS=
CXXCFLGS=$(CFLAGS)
LIB=-L/usr/include/GLFW -lglfw -lGL -lGLU -lX11 -lpthread -lXrandr -lXi -ldl
INC=-I/usr/include/GLFW -I./include/
OBJ=obj/main.o obj/world.o obj/skybox.o obj/block.o obj/BensGLUtilities.o obj/perlin.o obj/renderer.o obj/input.o obj/glad.o
all: voxelEngine

obj/%.o: include/glad/%.c
	$(CC) $(INC) -c -o $@ $< $(CFLAGS)

obj/%.o: src/%.cc
	$(CXX) $(INC) -c -o $@ $< $(CXXFLAGS) -std=c++17

voxelEngine: $(OBJ)
	$(CXX) -o voxelEngine $^ $(LIB)

clean:
	rm voxelEngine $(OBJ)
