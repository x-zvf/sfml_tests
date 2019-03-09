TARGET = sfml_test

LIBS=-lsfml-graphics -lsfml-window -lsfml-system
CXX = g++

CXXFLAGS = -g -Wall -Werror

.PHONY: default all clean
default: $(TARGET)
all: default

# Find all source files.
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard src/*.cpp))
HEADERS = $(wildcard src/incl/*.hpp)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(LIBS) -o $@

clean:
	-rm -f $(TARGET)
	-find . -type f -name '*.o' -delete
