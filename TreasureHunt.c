#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h> // For using 'bool' type
#include <string.h>  // For string operations

#define MAX_LEVELS 3
#define BASE_GRID_SIZE 5
#define MAX_GUESSES 6
#define HIGH_SCORE_FILE "high_score.txt" // File to store the high score

// ANSI color codes for text formatting
#define YELLOW "\033[1;33m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define LIGHT_BLUE "\033[1;34m"
#define LIGHT_PURPLE "\033[1;35m"
#define RESET "\033[0m"

// Define the Avatar struct
typedef struct {
    char name[50];
    char description[200];
} Avatar;

// Function prototypes
void displayIntroStory();
void displayAvatars(Avatar avatars[], int numAvatars);
int selectAvatar(Avatar avatars[], int numAvatars);
void displayLegend();
void initializeGrid(char grid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS],
                    char hiddenGrid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS], int gridSize);
void placeObjects(char hiddenGrid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS], int gridSize,
                  int *treasureX, int *treasureY, int *keyX, int *keyY, int *cursedX, int *cursedY, int difficulty);
void displayGrid(char grid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS], int gridSize);
void giveDirectionalHint(int guessX, int guessY, int treasureX, int treasureY);
int processGuess(char grid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS],
                 char hiddenGrid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS], int gridSize,
                 int guessX, int guessY, int *lives, int *guessesLeft, int *score, int *keys);
int loadHighScore();
void saveHighScore(int highScore);
int chooseChest();
void displayBanner();

void displayAvatars(Avatar avatars[], int numAvatars) {
    printf("\nChoose your avatar:\n");
    for (int i = 0; i < numAvatars; i++) {
        printf("%d. %s\n", i + 1, avatars[i].name);
        printf("   %s\n", avatars[i].description);
    }
}

int main() {
    srand(time(NULL)); // Seed random number generator

    bool isFirstRun = true; // Flag to track if it's the first run
    bool playAgain = true;

    while (playAgain) {
        if (isFirstRun) {
            // Display the intro story at the start of the game
            displayIntroStory();
            isFirstRun = false; // Set flag to false after first run
        }

        // Define avatars
        Avatar avatars[] = {
            {"Captain Blackbeard", "A ruthless pirate known for his cunning strategies."},
            {"Lady Storm", "A fearless sailor with unmatched navigation skills."},
            {"Kraken Keeper", "A mysterious figure rumored to control the seas."}
        };
        int numAvatars = sizeof(avatars) / sizeof(avatars[0]);

        // Let the player choose an avatar
        int selectedAvatarIndex = selectAvatar(avatars, numAvatars);
        Avatar playerAvatar = avatars[selectedAvatarIndex];
        printf("\nWelcome aboard, %s!\n", playerAvatar.name);
        printf("%s\n", playerAvatar.description);

        // Difficulty selection
        int maxGuesses, treasureValueMultiplier;
        printf("\nChoose your difficulty:\n");
        printf("1. Easy (More guesses, fewer traps, higher treasure value)\n");
        printf("2. Medium (Moderate guesses, traps, and treasure value)\n");
        printf("3. Hard (Fewer guesses, more traps, lower treasure value)\n");
        int difficultyChoice;
        do {
            printf("Enter your choice (1-3): ");
            scanf("%d", &difficultyChoice);
        } while (difficultyChoice < 1 || difficultyChoice > 3);

        switch (difficultyChoice) {
            case 1:
                maxGuesses = 8;
                treasureValueMultiplier = 15;
                printf("Easy mode selected.\n");
                break;
            case 2:
                maxGuesses = 6;
                treasureValueMultiplier = 10;
                printf("Medium mode selected.\n");
                break;
            case 3:
                maxGuesses = 4;
                treasureValueMultiplier = 5;
                printf("Hard mode selected.\n");
                break;
        }

        int highScore = loadHighScore(); // Load the high score from the file

        int level = 1;
        int lives = 3;
        int totalScore = 0;
        int keys = 0; // Inventory: Number of keys

        while (level <= MAX_LEVELS) {
            int gridSize = BASE_GRID_SIZE + level - 1; // Grid size increases by one each level
            int treasureValue = 50 + (level - 1) * treasureValueMultiplier; // Treasure value depends on difficulty
            printf("\n=== Level %d ===\n", level);
            printf("Grid Size: %dx%d | Treasure Value: %d\n", gridSize, gridSize, treasureValue);

            char grid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS];       // Visible grid
            char hiddenGrid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS]; // Hidden grid
            int guessesLeft = maxGuesses;

            // Initialize grids and place objects
            initializeGrid(grid, hiddenGrid, gridSize);
            int treasureX, treasureY, keyX, keyY, cursedX, cursedY;
            placeObjects(hiddenGrid, gridSize, &treasureX, &treasureY, &keyX, &keyY, &cursedX, &cursedY, difficultyChoice);

            bool treasureFound = false; // Track if the treasure was found in this level

            while (guessesLeft > 0 && lives > 0) {
                printf("\nGUESSES: %d | LIVES: %d | SCORE: %d\n", guessesLeft, lives, totalScore);
                printf("Inventory:\n");
                printf("Gold: %d | Gems: 0 | Maps: 0 | Keys: %d\n", totalScore, keys);
                displayGrid(grid, gridSize); // Display visible grid

                // Get user input for coordinates
                int guessX, guessY;
                printf("Enter your guess (row and column, separated by space): ");
                scanf("%d %d", &guessX, &guessY);

                // Validate input
                if (guessX < 0 || guessX >= gridSize || guessY < 0 || guessY >= gridSize) {
                    printf(RED "Invalid coordinates! Please try again.\n" RESET);
                    continue;
                }

                // Check if the coordinate was already guessed
                if (grid[guessX][guessY] != '.') {
                    printf(RED "You've already guessed this coordinate. Try again.\n" RESET);
                    continue;
                }

                // Process the guess
                int result = processGuess(grid, hiddenGrid, gridSize, guessX, guessY, &lives, &guessesLeft,
                                          &totalScore, &keys);
                if (result == 1) {
                    // Found the treasure
                    totalScore += treasureValue;
                    treasureFound = true; // Mark treasure as found
                    printf(YELLOW "Shimmerin' doubloons! The crew'll be singin' shanties for weeks!\n" RESET);
                    break;
                } else if (result == -1) {
                    // Hit a trap or cursed treasure
                    printf(RED "Boom! Trap hit!\n" RESET);
                } else if (result == -2) {
                    // Hit cursed treasure
                    printf(RED "Cursed Treasure! You lose 5 points and gain a trap!\n" RESET);
                } else if (result == 2) {
                    // Found the key
                    keys++;
                    printf(LIGHT_PURPLE "You found a key! It might unlock a treasure...\n" RESET);
                } else if (result == 3) {
                    // Hit locked treasure
                    printf(LIGHT_PURPLE "Locked treasure! Ye need a key to break it open!\n" RESET);
                } else {
                    // Missed the treasure
                    giveDirectionalHint(guessX, guessY, treasureX, treasureY);
                }
            }

            if (lives <= 0) {
                printf(RED "\nGame Over! You ran out of lives.\n" RESET);
                break;
            }

            if (!treasureFound && guessesLeft <= 0) {
                printf(RED "\nNot a single treasure found... the crew be questionin' yer compass skills!\n" RESET);
            }

            // Chest selection after completing the level
            int chestResult = chooseChest();
            if (chestResult == 1) {
                // Bonus item chest
                printf(YELLOW "You found a bonus item! Your score increases by 20 points.\n" RESET);
                totalScore += 20;
            } else if (chestResult == -1) {
                // Trap chest
                lives--;
                printf(RED "You lost a life due to the trap! Lives remaining: %d\n" RESET, lives);
            }

            // Ask the user if they want to proceed to the next level after every level
            if (level < MAX_LEVELS) {
                displayBanner();
                char choice;
                printf("Do you want to proceed to the next level? (y/n): ");
                scanf(" %c", &choice);
                if (choice != 'y' && choice != 'Y') {
                    printf(GREEN "\nAbandonin' ship, are ye? The treasure'll still be waitin'... if the sea don't claim it first!\n" RESET);
                    return 0;
                }
            }

            level++;
        }

        // Display final results
        printf("\n=== Final Results ===\n");
        printf("Inventory:\n");
        printf("Gold: %d | Gems: 0 | Maps: 0 | Keys: %d\n", totalScore, keys);
        printf("Your Final Score: %d\n", totalScore);
        printf("Current High Score: %d\n", highScore);

        if (totalScore > highScore) {
            highScore = totalScore;
            saveHighScore(highScore); // Save the new high score to the file
            printf(YELLOW "Shiver me timbers! That's the richest bounty we've ever seen!\n" RESET);
        } else {
            printf(RED "Ye fought like a true pirate... but the treasure still belongs to another.\n" RESET);
        }

        // Ask the user if they want to play again after completing all levels
        char choice;
        printf("Do you want to play again? (y/n): ");
        scanf(" %c", &choice);
        if (choice != 'y' && choice != 'Y') {
            playAgain = false; // Exit the outer loop
        } else {
            printf("\nRestarting the game...\n");
        }
    }

    return 0;
}

// Function to display the intro story
void displayIntroStory() {
    printf(LIGHT_BLUE "~\n");
    printf("   THE LEGEND OF THE LOST TREASURE OF BLACKTIDE COVE  \n");
    printf("~\n");
    printf("Long ago, the infamous pirate Captain Blacktide ruled the seas.\n");
    printf("His ship, The Crimson Kraken, was said to carry riches from\n");
    printf("a hundred plundered ports. But one stormy night, he vanished —\n");
    printf("leaving behind nothing but rumors... and a treasure map.\n");
    printf("\n");
    printf("That map has found its way into your hands, brave sailor.\n");
    printf("You now stand at the edge of the unknown, ready to seek\n");
    printf("the cursed gold, glimmerin' gems, and ancient secrets buried\n");
    printf("deep within these treacherous islands.\n");
    printf("\n");
    printf("But beware! Traps guard the loot, and cursed treasures may\n");
    printf("cost ye dearly. Only the wisest pirates with keen eyes\n");
    printf("and steady hearts can claim the riches and live to tell the tale.\n");
    printf("\n");
    printf("Now hoist the sails, sharpen yer blade, and let the hunt begin!\n");
    printf("~\n" RESET);
}


// Function to let the player select an avatar
int selectAvatar(Avatar avatars[], int numAvatars) {
    int choice;
    do {
        displayAvatars(avatars, numAvatars);
        printf("Enter the number of your chosen avatar: ");
        scanf("%d", &choice);
        if (choice < 1 || choice > numAvatars) {
            printf(RED "Invalid choice! Please try again.\n" RESET);
        }
    } while (choice < 1 || choice > numAvatars);
    return choice - 1; // Convert to zero-based index
}

// Function to display the legend
void displayLegend() {
    printf("Legend:\n");
    printf(". - Empty space\n");
    printf("T - Treasure\n");
    printf("L - Locked Treasure\n");
    printf("K - Key\n");
    printf("C - Cursed Treasure\n");
    printf("X - Trap\n");
    printf("! - Hit trap\n");
    printf("J - Jewel (Power-up)\n");
    printf("M - Map (Reveals all traps)\n");
    printf("G - Extra Guess (Power-up)\n");
    printf("x - Already guessed\n");
    printf("\n");
}

// Function to initialize the grid
void initializeGrid(char grid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS],
                    char hiddenGrid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS], int gridSize) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            grid[i][j] = '.';         // Visible grid starts as empty
            hiddenGrid[i][j] = '.';   // Hidden grid starts as empty
        }
    }
}

// Function to place objects on the grid
void placeObjects(char hiddenGrid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS], int gridSize,
                  int *treasureX, int *treasureY, int *keyX, int *keyY, int *cursedX, int *cursedY, int difficulty) {
    // Place treasure
    *treasureX = rand() % gridSize;
    *treasureY = rand() % gridSize;
    hiddenGrid[*treasureX][*treasureY] = 'T';

    // Place locked treasure
    int lockedX, lockedY;
    do {
        lockedX = rand() % gridSize;
        lockedY = rand() % gridSize;
    } while (hiddenGrid[lockedX][lockedY] != '.');
    hiddenGrid[lockedX][lockedY] = 'L';

    // Place key
    do {
        *keyX = rand() % gridSize;
        *keyY = rand() % gridSize;
    } while (hiddenGrid[*keyX][*keyY] != '.');
    hiddenGrid[*keyX][*keyY] = 'K';

    // Place cursed treasure
    do {
        *cursedX = rand() % gridSize;
        *cursedY = rand() % gridSize;
    } while (hiddenGrid[*cursedX][*cursedY] != '.');
    hiddenGrid[*cursedX][*cursedY] = 'C';

    // Place traps (number depends on difficulty)
    int numTraps = (difficulty == 1) ? 2 : (difficulty == 2) ? 3 : 4;
    for (int i = 0; i < numTraps; i++) {
        int x = rand() % gridSize;
        int y = rand() % gridSize;
        if (hiddenGrid[x][y] == '.') {
            hiddenGrid[x][y] = 'X';
        }
    }

    // Place power-ups
    for (int i = 0; i < 2; i++) {
        int x = rand() % gridSize;
        int y = rand() % gridSize;
        if (hiddenGrid[x][y] == '.') {
            hiddenGrid[x][y] = (rand() % 2 == 0) ? 'J' : 'G'; // Randomly place jewel or extra guess
        }
    }

    // Place map
    int x = rand() % gridSize;
    int y = rand() % gridSize;
    if (hiddenGrid[x][y] == '.') {
        hiddenGrid[x][y] = 'M';
    }
}

// Function to display the grid
void displayGrid(char grid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS], int gridSize) {
    printf("\nGrid:\n");
    printf("   "); // Column headers
    for (int j = 0; j < gridSize; j++) {
        printf("%d ", j);
    }
    printf("\n");
    for (int i = 0; i < gridSize; i++) {
        printf("%d  ", i); // Row header
        for (int j = 0; j < gridSize; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

// Function to give directional hints
void giveDirectionalHint(int guessX, int guessY, int treasureX, int treasureY) {
    printf("Hint: The treasure is ");
    if (guessX < treasureX) {
        printf("south");
    } else if (guessX > treasureX) {
        printf("north");
    }
    if (guessY < treasureY) {
        printf("east");
    } else if (guessY > treasureY) {
        printf("west");
    }
    printf(".\n");
}

int processGuess(char grid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS],
    char hiddenGrid[BASE_GRID_SIZE + MAX_LEVELS][BASE_GRID_SIZE + MAX_LEVELS], int gridSize,
    int guessX, int guessY, int *lives, int *guessesLeft, int *score, int *keys) {
(*guessesLeft)--; // Decrement guesses left
char cell = hiddenGrid[guessX][guessY]; // Get the object at the guessed coordinate

if (cell == 'T') {
grid[guessX][guessY] = 'T'; // Reveal the treasure
return 1; // Found treasure
} else if (cell == 'L') {
if (*keys > 0) {
printf(YELLOW "You unlocked the treasure! Claim yer gold!\n" RESET);
grid[guessX][guessY] = 'T';
(*keys)--; // Use one key
return 1; // Unlocked treasure
} else {
printf(RED "Locked treasure! Ye need a key to break it open!\n" RESET);
grid[guessX][guessY] = 'L';
return 3; // Locked treasure
}
} else if (cell == 'K') {
grid[guessX][guessY] = 'K'; // Reveal the key
return 2; // Found key
} else if (cell == 'C') {
grid[guessX][guessY] = 'C'; // Mark cursed treasure as 'C'
(*lives)--; // Lose a life
*score -= 5; // Deduct 5 points

// Add an extra trap ('!') at a random location
int trapX, trapY;
do {
trapX = rand() % gridSize;
trapY = rand() % gridSize;
} while (grid[trapX][trapY] != '.'); // Ensure the new trap is placed on an empty spot
grid[trapX][trapY] = '!'; // Place the extra trap

return -2; // Cursed treasure
} else if (cell == 'X') {
grid[guessX][guessY] = '!'; // Mark the trap with '!'
(*lives)--; // Hit a trap
return -1; // Trap hit
} else if (cell == 'J') {
printf(YELLOW "You found a jewel! It's worth bonus points.\n" RESET);
*score += 20; // Add bonus points for jewels
grid[guessX][guessY] = 'J'; // Display 'J' for jewels
hiddenGrid[guessX][guessY] = '.'; // Remove the jewel from the hidden grid
} else if (cell == 'G') {
printf(YELLOW "You found an extra guess!\n" RESET);
(*guessesLeft)++;
hiddenGrid[guessX][guessY] = '.'; // Remove the extra guess from the hidden grid
} else if (cell == 'M') {
printf(YELLOW "You found a map! Revealing all traps...\n" RESET);
grid[guessX][guessY] = 'M'; // Mark the map as 'M'
for (int i = 0; i < gridSize; i++) {
for (int j = 0; j < gridSize; j++) {
   if (hiddenGrid[i][j] == 'X') {
       grid[i][j] = '!'; // Reveal traps as '!'
   }
}
}
hiddenGrid[guessX][guessY] = '.'; // Remove the map from the hidden grid
}

// Mark the guessed coordinate with 'x' (if it's not a trap or cursed treasure)
if (cell != 'X' && cell != 'C') {
grid[guessX][guessY] = 'x';
}

return 0; // Missed the treasure
}

// Function to load the high score
int loadHighScore() {
    FILE *file = fopen(HIGH_SCORE_FILE, "r");
    if (file == NULL) {
        return 0; // Default high score if file doesn't exist
    }
    int highScore;
    fscanf(file, "%d", &highScore);
    fclose(file);
    return highScore;
}

// Function to save the high score
void saveHighScore(int highScore) {
    FILE *file = fopen(HIGH_SCORE_FILE, "w");
    if (file == NULL) {
        printf("Error saving high score.\n");
        return;
    }
    fprintf(file, "%d", highScore);
    fclose(file);
}

// Function to handle chest selection
int chooseChest() {
    printf("\nYou have completed the level! Choose one of three chests:\n");
    printf("1. Chest A\n");
    printf("2. Chest B\n");
    printf("3. Chest C\n");
    int choice;
    do {
        printf("Enter your choice (1-3): ");
        scanf("%d", &choice);
        if (choice < 1 || choice > 3) {
            printf(RED "Invalid choice! Please try again.\n" RESET);
        }
    } while (choice < 1 || choice > 3);

    switch (choice) {
        case 1:
            printf(YELLOW "You opened Chest A and found a bonus item!\n" RESET);
            return 1; // Bonus item
        case 2:
            printf("You opened Chest B... but it's empty!\n");
            return 0; // Nothing
        case 3:
            printf(RED "You opened Chest C and triggered a trap! Lose 1 life.\n" RESET);
            return -1; // Trap
    }
    return 0;
}

// Function to display the banner after finding a bonus item
void displayBanner() {
    printf("\n**\n");
    printf("*  What would you like to do? *\n");
    printf("\n");
    printf("1. Proceed to the next level\n");
    printf("2. Exit the game\n");
    printf("\n");
}