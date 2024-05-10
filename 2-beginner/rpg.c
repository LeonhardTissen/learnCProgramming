#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

// Define colors for the terminal
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

typedef struct {
	char name[32];
	int level;
	int health;
	int maxHealth;
	int attack;
	int critChance;
	int blockChance;
} Entity;

Entity createEntity(char name[], int level, int health, int attack, int critChance, int blockChance) {
	Entity entity;
	strcpy(entity.name, name);
	entity.level = level;
	entity.health = health;
	entity.maxHealth = health;
	entity.attack = attack;
	entity.critChance = critChance;
	entity.blockChance = blockChance;
	return entity;
}

char enemyAdjectives[10][32] = {
	"Evil",
	"Vicious",
	"Deadly",
	"Terrifying",
	"Sinister",
	"Monstrous",
	"Horrible",
	"Creepy",
	"Scary",
	"Dark"
};

char enemyNouns[10][32] = {
	"Orc",
	"Goblin",
	"Dragon",
	"Zombie",
	"Ghost",
	"Skeleton",
	"Vampire",
	"Werewolf",
	"Slime",
	"Spider"
};

char* getEnemyName() {
	// Pick a random name from the list of enemy names
	int totalNouns = 10;
	int totalAdjectives = 10;
	int nounIndex = rand() % totalNouns;
	int adjectiveIndex = rand() % totalAdjectives;

	// Allocate memory for the enemy name
	char *enemyName = malloc(64);

	// Combine the adjective and noun
	sprintf(enemyName, "%s %s", enemyAdjectives[adjectiveIndex], enemyNouns[nounIndex]);

	return enemyName;
}

Entity createPlayer(char name[]) {
	int level = 1;
	int health = 100;
	int attack = 20;
	int critChance = 10;
	int blockChance = 20;
	return createEntity(name, level, health, attack, critChance, blockChance);
}

Entity createEnemy(int level) {
	// Enemy has increasing stats based on the level
	int health = 50 + level * 10;
	int attack = 10 + level * 2;
	int critChance = 5 + level;
	int blockChance = 10 + level * 2;
	return createEntity(getEnemyName(), level, health, attack, critChance, blockChance);
}

char* renderEntityHealthBar(Entity entity) {
	// Calculate the health bar length
	int healthBarLength = 20;
	int totalHealthBarLength = healthBarLength + 2;
	int healthBarFill = (float)entity.health / entity.maxHealth * healthBarLength;

	// Allocate memory for the health bar
	char *healthBar = malloc(totalHealthBarLength + 1);

	// Fill the health bar
	healthBar[0] = '[';
	for (int i = 1; i <= healthBarLength; i++) {
		healthBar[i] = i <= healthBarFill ? '#' : ' '; // # for health, ' ' for empty
	}
	healthBar[totalHealthBarLength - 1] = ']';

	// Null terminate the string
	healthBar[totalHealthBarLength] = '\0';

	return healthBar;
}
void printEntityStats(Entity entity) {
	printf("- %s - (Level %d)\n", entity.name, entity.level);
	printf("Health: " KRED "%s" KNRM " %d/%d\n", renderEntityHealthBar(entity), entity.health, entity.maxHealth);
	printf("Attack: " KMAG "%d" KNRM "\n", entity.attack);
	printf("Crit Chance: " KYEL "%d%%" KNRM "\n", entity.critChance);
	printf("Block Chance: " KBLU "%d%%" KNRM "\n", entity.blockChance);
}

void attack(Entity *attacker, Entity *defender) {
    // Calculate the damage
    int damage = attacker->attack + rand() % 5;
	char* addedInfo = "";

    // Check if the attack is a critical hit
    if (rand() % 100 < attacker->critChance) {
        damage *= 2;
		addedInfo = " (Critical Hit)";
    }

    // Check if the defender blocked the attack
    if (rand() % 100 < defender->blockChance) {
        damage = 0;
		addedInfo = " (Blocked)";
    }

    // Apply the damage
    defender->health -= damage;
    printf("\n%s dealt %d damage to %s%s!\n\n", attacker->name, damage, defender->name, addedInfo);
}

void heal(Entity *healer) {
	// Calculate the amount of health to heal
	int healAmount = 20 + rand() % 10;

	char* addedInfo = "";
	if (rand() % 100 < 30) {
		healAmount *= 2;
		addedInfo = " (Critical Heal)";
	}

	// Apply the healing
	healer->health += healAmount;
	if (healer->health > healer->maxHealth) {
		healer->health = healer->maxHealth;
	}

	printf("\n%s healed for %d health%s!\n\n", healer->name, healAmount, addedInfo);
}

bool isAlive(Entity entity) {
	return entity.health > 0;
}

bool isOneDead(Entity entity1, Entity entity2) {
	// Check if either entity is dead
	return !isAlive(entity1) || !isAlive(entity2);
}

void printGameStats(Entity player, Entity enemy) {
	printf("========= Player stats: ========= \n");
	printEntityStats(player);

	printf("\n========= Enemy stats: ========== \n");
	printEntityStats(enemy);
}

void clearScreen() {
	// Clear the screen
	printf("\033[H\033[J");
}

int main() {
	// Seed the random number generator
	srand(time(NULL));

	// Ask the player what their warriors name should be
	char playerName[32];
	printf("Enter your name: ");
	fgets(playerName, 32, stdin);
	playerName[strlen(playerName) - 1] = '\0';

	// Create the player and the first enemy
	Entity player = createPlayer(playerName);

	// Start the game loop at level 1
	int level = 1;

	while (1) {
		Entity enemy = createEnemy(level);

		clearScreen();
		printf("*************** Level %d ***************\n\n", level);

		printGameStats(player, enemy);

		// The player gets the first turn in each level
		bool playerTurn = true;
		while (!isOneDead(player, enemy)) {
			msleep(2000);

			clearScreen();
			printf("*************** %s's turn ***************\n", playerTurn ? player.name : enemy.name);

			if (playerTurn) {
				// Player decides to heal if health is low
				if (player.health < player.maxHealth / 3) {
					heal(&player);
				} else {
					attack(&player, &enemy);
				}
			} else {
				// Enemy attacks the player
				attack(&enemy, &player);
			}

			// Swap turns after each move
			playerTurn = !playerTurn;

			printGameStats(player, enemy);
		}

		if (isAlive(player)) {
			printf("\n*************** %s wins! ***************\n", player.name);
		} else {
			printf("\n%s has defeated %s! R.I.P.\n", enemy.name, player.name);
			break;
		}

		msleep(2000);

		level++;
	}

	return 0;
}
