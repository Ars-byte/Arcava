CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra
LDLIBS ?= -lm

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)

Arcava: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDLIBS)

src/%.o: src/%.cpp $(wildcard src/*.hpp)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) Arcava

.PHONY: clean
