# Treasure-Hunt
## Overview
Embark on an epic adventure across treacherous islands in Pirate Treasure Hunt , a text-based treasure-hunting game. As a brave sailor or cunning pirate, you must navigate grids filled with treasures, traps, cursed items, and power-ups to claim the legendary riches of Blacktide Cove. Use your wits, manage your resources, and avoid deadly traps to become the greatest treasure hunter of all time!

## Installation
### Prerequisites
- A C compiler (e.g., gcc)
- A terminal or command-line interface
### Steps to Install
1. Clone or download the source code files:
   `git clone https://github.com/your-repo/pirate-treasure-hunt.git`
2. Navigate to the project directory:
   `cd pirate-treasure-hunt`
3. Compile the game using `gcc`:
   `gcc TreasureHunt.c -o TreasureHunt`
4. Run the game:
   `./TreasureHunt`

## How to Play
1. Start the Game : Run the compiled executable to begin your adventure.
2. Choose Your Avatar : Select from a list of unique pirate avatars, each with their own backstory.
3. Select Difficulty : Pick a difficulty level (Easy, Medium, or Hard) to suit your skill.
4. Navigate the Grid : Guess coordinates to uncover treasures, traps, and power-ups.
5. Manage Resources : Use keys to unlock treasures, collect power-ups, and avoid traps.
6. Complete Levels : Progress through three increasingly challenging levels.
7. Beat the High Score : Aim to surpass the current high score and etch your name into pirate history.

## Game Mechanics

### Objective
Find the treasure (T) on the grid while avoiding traps (X), cursed treasures (C), and managing your lives and guesses.

### Gameplay Flow
1. Each level presents a grid of increasing size (starting at 5x5).
2. You have a limited number of guesses and lives to find the treasure.
3. Hints are provided after each missed guess to guide you closer to the treasure.
4. Power-ups like jewels (J), extra guesses (G), and maps (M) can help you succeed.
   
### Scoring
1. Treasure : Earn points based on the treasure value (increases per level).
2. Jewels : Gain bonus points for finding jewels.
3. Cursed Treasures : Lose points and gain additional traps.
4. High Score : Your total score is saved if it surpasses the current high score.

## Legend

| Symbol | Meaning                  |
|--------|--------------------------|
| `.`    | Unexplored space         |
| `T`    | Treasure                 |
| `L`    | Locked treasure          |
| `K`    | Key                      |
| `C`    | Cursed treasure          |
| `!`    | Hit trap                 |
| `J`    | Jewel (power-up)         |
| `M`    | Map (reveals all traps)  |
| `G`    | Extra guess (power-up)   |
| `x`    | Already guessed (empty space) |
