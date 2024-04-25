EXECUTABLE := voxelEngine
CC := g++

SRCDIR := src
OBJDIR := obj
INCDIR := include
LIBDIR := lib

CXXFLAGS := -std=c++17
# CXXFLAGS += -Wall
# CXXFLAGS += -Wextra
# CXXFLAGS += -Wpedantic
CXXFLAGS += -I./$(LIBDIR)
CXXFLAGS += -I./$(INCDIR)
CXXFLAGS += $(shell pkg-config --cflags freetype2)

LDFLAGS := $(shell pkg-config --libs glfw3 freetype2)

SOURCES := $(wildcard $(SRCDIR)/*.cc)
OBJECTS := $(patsubst $(SRCDIR)/%.cc,$(OBJDIR)/%.o,$(SOURCES))
OBJECTS += $(OBJDIR)/glad.o

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Main program
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Standard object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(LIBDIR)/%.h | $(OBJDIR)
	$(CC) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/main.o: $(SRCDIR)/main.cc | $(OBJDIR)
	$(CC) $(CXXFLAGS) -c $< -o $@

# GLAD include
$(OBJDIR)/glad.o: $(INCDIR)/glad/glad.c $(INCDIR)/glad/glad.h | $(OBJDIR)
	$(CC) $(CXXFLAGS) -c $< -o $@

# Create the OBJDIR
$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: clean
# Remove object files and executable
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
