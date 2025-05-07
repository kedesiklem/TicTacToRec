# Cross Platform Makefile - Version simplifiée

# Configuration de base
SRC_DIR := src
BUILD_DIR := build
EXE := $(BUILD_DIR)/tictactorec

# Sources
IMGUI_DIR := $(SRC_DIR)/external/imgui
TTT_DIR := $(SRC_DIR)/TTT
OTHER_DIR := $(SRC_DIR)/utils

SOURCES := $(SRC_DIR)/main.cpp
SOURCES += $(wildcard $(IMGUI_DIR)/*.cpp) \
           $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
           $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp \
		   $(wildcard $(TTT_DIR)/*.cpp) \

# Génération des noms de fichiers objets
OBJS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Flags de compilation
CXX ?= g++
CXXFLAGS := -std=c++17 -I$(SRC_DIR) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(TTT_DIR) -I$(OTHER_DIR)
CXXFLAGS += -g -Wall -Wformat

# Détection de la plateforme
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
    LIBS := -lGL `pkg-config --static --libs glfw3`
    CXXFLAGS += `pkg-config --cflags glfw3`
else ifeq ($(UNAME_S), Darwin)
    LIBS := -framework OpenGL -framework Cocoa -lglfw
    CXXFLAGS += -I/usr/local/include
else ifeq ($(OS), Windows_NT)
    LIBS := -lglfw3 -lgdi32 -lopengl32 -limm32
endif

# Règle principale
all: $(EXE)
	@echo Build terminé pour $(UNAME_S)

# Lien final
$(EXE): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

# Règle générique pour la compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)  # Utilisation de dir pour créer toute l'arborescence
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean