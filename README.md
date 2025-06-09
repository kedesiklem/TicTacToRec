# TicTacToRec

**TicTacToRec** est une généralisation récursive du [Ultimate Tic-Tac-Toe](https://en.wikipedia.org/wiki/Ultimate_tic-tac-toe). Le jeu repose sur une structure imbriquée de grilles, permettant une complexité stratégique accrue.

## Fonctionnalités

- Grilles de jeu récursives (grilles dans les cases)
- Support de profondeur paramétrable
- Mécanisme de victoire hiérarchique
- Historique des coups [Undo/Redo]
- IA de base (à venir)

## Dependance & Prérequis

Pensez à récupérer imgui-docking, inclus dans les sub-modules :

```bash
git submodule update --init --recursive
```

Assurez-vous d'avoir installé :

- **cmake**
- **g++** (compilateur C++)
- **GLFW** et **OpenGL** (bibliothèques graphiques)

### Installation rapide (Linux) :

```bash
sudo apt install g++ libglfw3 libglfw3-dev libglu1-mesa-dev  # Ubuntu/Debian
```

```bash
sudo dnf install gcc-c++ glfw glfw-devel mesa-libGLU-devel    # Fedora
```

```bash
sudo pacman -S gcc glfw-x11 glu                               # Arch
```

## Compilation

### Via CMake

```bash
cmake .
make
```

## Utilisation

```bash
./tictactorec
```

### Raccourci

| Touche                  | Command           |
| ----------------------- | ------------------|
| `Ctrl + Z`              | Undo              |
| `Ctrl + Y`              | Redo              |
| `Ctrl + S`              | Save Current Game |
| `Ctrl + O`              | Load Current Game |
| `Ctrl + X, Ctrl + X`    | Reset Game        |
| `Esc`                   | Quit              |

## Fichiers

- `src/` : Code source.
  - `main.cpp` : Point d'entrée, logique principale.
  - `Grid/` : Backend.
    - `GridLogic.hpp` : Grilles recursives (template)
    - `GridView.hpp` : Afficheurs de grilles recursives (template)
    - `TTT/` : Implementation specifique au TicTacToeRec
  - `UI/` : Divers
    - `config.h` : Parametrage ImGui
    - `emacsStyleShortCut.hpp` : Gestionnaire de racourcie fait maison
    - `functor.hpp` : Utilitaire pour `emacsStyleShortCut.hpp`
  - `external/imgui/` : Dépendances de l'IHM.
  - `external/imgui_file_explorer/` : Dépendances de l'IHM.
- `CMakelists.txt` : Compilation
- `TODO.org` : Tâches restantes (org-mode).
- `README.md` : Documentation (Vous êtes ici).

## À faire

Voir [TODO](TODO.org)

## Licence

- [CC BY-NC 4.0](LICENCE.md)
