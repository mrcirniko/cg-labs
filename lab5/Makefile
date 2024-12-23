# Компилятор и флаги
CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -std=c++11 -Iinclude -Iinclude/SFML -Iinclude/GL -Llib
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lglew32 -lopengl32 -lgdi32

# Исходные и объектные файлы
SRCDIR = src
OBJDIR = obj
BINDIR = bin
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))
EXECUTABLE = $(BINDIR)/raytracer.exe

# Правила
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
    $(CXX) $(OBJECTS) -o $@ $(CXXFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
    mkdir -p $(OBJDIR)
    $(CXX) $(CXXFLAGS) -c $< -o $@

$(BINDIR):
    mkdir -p $(BINDIR)

clean:
    rm -rf $(OBJDIR)/*.o $(EXECUTABLE)

.PHONY: all clean