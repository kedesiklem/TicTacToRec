# TicTacToRec

**TicTacToRec** est une généralisation récursive du [Ultimate Tic-Tac-Toe](https://en.wikipedia.org/wiki/Ultimate_tic-tac-toe). Le jeu repose sur une structure imbriquée de grilles, permettant une complexité stratégique accrue.

## Fonctionnalités

- Grilles de jeu récursives (grilles dans les cases)
- Support de profondeur paramétrable
- Mécanisme de victoire hiérarchique
- IA de base (à venir)

## Dependance & Prérequis

Pensez à récupérer imgui-docking, inclus dans les sub-modules :

```bash
git submodule update --init --recursive
```

Assurez-vous d'avoir installé :
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

### Via Makefile

```bash
make
```

Sinon, va falloir mettre les mains dans la tambouille.

## Utilisation

```bash
./build/tictactorec
```

### Raccourci

| Touche    | Command                       |
|-          |-                              |
| `R`       | Reset de la partie            |
| `Esc`     | Quit l'application            |

## Fichiers

- `src/` : Code source.
    - `main.cpp` : Point d'entrée, logique principale.
    - `TTT/` : Backend.
    - `external/imgui/` : Dépendances de l'IHM.
- `Makefile` : Compilation
- `TODO.org` : Tâches restantes (org-mode).
- `README.md` : Documentation (Vous êtes ici).

## À faire

Voir [TODO](TODO.org)

## Licence

- [CC BY-NC 4.0](LICENCE.md)
