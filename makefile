all:  
	g++ -g -I/usr/include/GLFW -L/usr/include/GLFW -o voxelEngine ./src/main.cc ./src/chunk.cc ./src/world.cc  ./src/skybox.cc ./src/block.cc ./src/renderer.cc ./src/BensGLUtilities.cc ./src/perlin.cc ./include/glad/glad.c -lglfw -lGL -lGLU -lX11 -lpthread -lXrandr -lXi -ldl -std=c++17
