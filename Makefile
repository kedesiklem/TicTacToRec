# === Variables ===
CXX = g++
CXXFLAGS = -std=c++17 -g -fdiagnostics-color=always
INCLUDES = -I./imgui -I./TTT
LDFLAGS = -lglfw -lGL

# === Fichiers ===
SRCS = $(wildcard main.cpp) \
       $(wildcard imgui/*.cpp) \
       $(wildcard TTT/*.cpp)

BIN = tictactorec

# === Règles ===
all: $(BIN)

$(BIN): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BIN) $(INCLUDES) $(LDFLAGS)

clean:
	rm -f $(BIN)
