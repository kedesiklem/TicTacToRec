# TicTacToRec

**TicTacToRec** est une généralisation récursive du [Ultimate Tic-Tac-Toe](https://en.wikipedia.org/wiki/Ultimate_tic-tac-toe). Le jeu repose sur une structure imbriquée de grilles, permettant une complexité stratégique accrue.

## Fonctionnalités

- Grilles de jeu récursives (grilles dans les cases)
- Support de profondeur paramétrable
- Mécanisme de victoire hiérarchique
- Interface console simple
- IA de base (à venir)

## Prerequis

- Assurez-vous d’avoir `g++`, `GLFW`, et `OpenGL` installés
- Pensez à récupérer imgui-docking, inclus dans les sub-modules :

```bash
git submodule update --init --recursive
```

## Compilation

### Via Makefile (recommandé pour Linux/Unix)

Assurez-vous d’avoir `g++`, `GLFW`, et `OpenGL` installés, puis :

```bash
make
```

Sinon, va falloir mettre les mains dans la tambouille.

## Utilisation

```bash
./build/tictactorec
```

### Raccourci

`R` : reset de la partie

## Fichiers

- `main.cpp` : logique principale du jeu
- `scr/` code
- `src/TTT/` : backend
- `src/imgui/` : dependance de l'IHM
- `TODO.org` : tâches restantes
- `README.md` : vous êtes ici

## À faire

Voir `TODO.org`

## Licence

- Ce projet global : [CC BY-NC 4.0](LICENCE.md)
- ImGui (docking branch) : [MIT License](imgui/LICENSE.txt)
