# TicTacToRec

**TicTacToRec** est une généralisation récursive du [Ultimate Tic-Tac-Toe](https://en.wikipedia.org/wiki/Ultimate_tic-tac-toe). Le jeu repose sur une structure imbriquée de grilles, permettant une complexité stratégique accrue.

## Fonctionnalités

- Grilles de jeu récursives (grilles dans les cases)
- Support de profondeur paramétrable
- Mécanisme de victoire hiérarchique
- Interface console simple
- IA de base (à venir)

## Compilation

### 🔧 Via Makefile (recommandé pour Linux/Unix)

Assurez-vous d’avoir `g++`, `GLFW`, et `OpenGL` installés, puis :

```bash
make
```
Sinon, va falloir mettre les mains dans la tambouille.

## Utilisation

```bash
./tictactorec
```


### Raccourci
`R` : reset de la partie

## Fichiers

- `main.cpp` : logique principale du jeu
- `imgui/` : dependance de l'IHM
- `TTT/` : backend
- `TODO.org` : tâches restantes
- `README.md` : vous êtes ici

## À faire

Voir `TODO.org`

## Licence

Ce projet est sous **double licence** 

- **Creative Commons BY-NC 4.0** pour les usages **non commerciaux** :  
  Vous pouvez copier, modifier, et partager le projet librement tant que ce n’est pas pour un usage commercial et que vous créditez l’auteur.  
  → https://creativecommons.org/licenses/by-nc/4.0/deed.fr

- **Licence commerciale sur demande** :  
  Pour tout usage commercial (vente, intégration dans un produit payant, publicité, etc.), merci de me contacter pour obtenir une autorisation ou une licence adaptée.