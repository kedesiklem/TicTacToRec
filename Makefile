# Cross Platform Makefile

# --------------------------
# Configuration
# --------------------------

# Répertoires
SRC_DIR := src
BUILD_DIR := build
IMGUI_DIR := $(SRC_DIR)/external/imgui
IMGUI_FILE_EXPLORER_DIR := $(SRC_DIR)/external/imgui_file_explorer
TTT_DIR := $(SRC_DIR)/TTT
OTHER_DIR := $(SRC_DIR)/utils

# Liste des répertoires à inclure
INCLUDE_DIRS := \
    $(SRC_DIR) \
    $(IMGUI_DIR) \
	$(IMGUI_FILE_EXPLORER_DIR) \
    $(IMGUI_DIR)/backends \
    $(TTT_DIR) \
    $(OTHER_DIR)

# Exécutable final
EXE := $(BUILD_DIR)/tictactorec

# --------------------------
# Fichiers sources
# --------------------------

# Sources principales
SOURCES := $(SRC_DIR)/main.cpp

# Sources ImGui
SOURCES += $(wildcard $(IMGUI_DIR)/*.cpp) \
           $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp \
           $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# Sources du jeu
SOURCES += $(wildcard $(TTT_DIR)/*.cpp)

# Génération des fichiers objets
OBJS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP_FILES := $(OBJS:.o=.d)

# --------------------------
# Compilation Flags
# --------------------------

# Flags communs
CXX ?= g++
CXXFLAGS := -std=c++17 -g -Wall -Wformat -MMD -MP

# Génération des flags d'inclusion
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))
# --------------------------
# Configuration plateforme
# --------------------------

UNAME_S := $(shell uname -s)

# Linux
ifeq ($(UNAME_S), Linux)
    LIBS := -lGL `pkg-config --static --libs glfw3`
    CXXFLAGS += `pkg-config --cflags glfw3`

# MacOS
else ifeq ($(UNAME_S), Darwin)
    LIBS := -framework OpenGL -framework Cocoa -lglfw
    CXXFLAGS += -I/usr/local/include

# Windows
else ifeq ($(OS), Windows_NT)
    LIBS := -lglfw3 -lgdi32 -lopengl32 -limm32
endif

# --------------------------
# Règles de construction
# --------------------------

.PHONY: all clean

# Cible principale
all: $(EXE)
	@echo Build terminé pour $(UNAME_S)

# Lien final
$(EXE): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCLUDE_FLAGS) $(LIBS)

# Règle générique pour la compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

# Inclure les dépendances
-include $(DEP_FILES)

# Nettoyage
clean:
	rm -rf $(BUILD_DIR)