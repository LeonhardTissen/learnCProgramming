#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Define colors for the terminal
#define KNRM "\x1B[0m"  // Reset color
#define KGRE "\x1B[32m" // Green text
#define BWHT "\x1B[47m" // White background
#define BGRE "\x1B[42m" // Green background

bool show_solution;

enum Tile {
    Wall,
    Unpathed,
    Solution,
    Branch,
};

int current_trail = Solution;

enum Direction {
    Up,
    Right,
    Down,
    Left
};

typedef struct {
    int x;
    int y;
} Point;

Point createPoint(int x, int y) {
    Point point;
    point.x = x;
    point.y = y;
    return point;
}

Point createInvalidPoint() {
    return createPoint(-1, -1);
}

Point traverser;

typedef struct {
    int **data; // 2D matrix of integers
    int width;
    int height;
} Field;

// Function to generate a Field struct with a 2D matrix of integers
Field generateField(int rows, int cols) {
    Field field;
    field.width = cols * 2 + 1;
    field.height = rows * 2 + 1;

    // Allocate memory for array of pointers to int (rows)
    field.data = (int **)malloc(field.height * sizeof(int *));
    if (field.data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for each row (array of integers)
    for (int y = 0; y < field.height; y++) {
        field.data[y] = (int *)malloc(field.width * sizeof(int));
        if (field.data[y] == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");

            // Clean up previously allocated memory
            for (int i = 0; i < y; i++) {
                free(field.data[i]);
            }
            free(field.data);

            exit(EXIT_FAILURE);
        }

        for (int x = 0; x < field.width; x++) {
            // Initialize all tiles to be walls
            field.data[y][x] = Wall;
        }
    }

    return field;
}

// Function to change the tileId at a given position in a matrix
void changeTile(int **data, Point point, int tileId) {
    data[point.y][point.x] = tileId;
}

// Function to clean out anything in a field except for the edges
void removeNonEdges(Field *field) {
    for (int y = 0; y < field->height; y++) {
        for (int x = 0; x < field->width; x++) {
            if (y == 0 || y == field->height - 1 || x == 0 || x == field->width - 1) {
                continue; // Skip edge tiles
            }
            // Change non-edge tiles to Unpathed
            changeTile(field->data, createPoint(x, y), Unpathed);
        }
    }
}

// Cuts out the border in the top left and bottom right for the start and exit
void addStartAndExit(Field *field) {
    Point start = createPoint(1, 0);
    changeTile(field->data, start, Solution);
    Point finish = createPoint(field->width - 2, field->height - 1);
    changeTile(field->data, finish, Solution);
}

// Add a wall at every 2nd tile both horizontally and vertically
void addGridPoints(Field *field) {
    for (int y = 0; y < field->height; y++) {
        for (int x = 0; x < field->width; x++) {
            if (x % 2 == 0 && y % 2 == 0) {
                changeTile(field->data, createPoint(x, y), Wall);
            }
        }
    }
}

int getTile(Field field, Point position) {
    if (position.x < 0 ||
        position.y < 0 ||
        position.x >= field.width ||
        position.y >= field.height) {
        return -1;
    }
    return field.data[position.y][position.x];
}

// Function to return the texture representation of a tileId
const char *getTileTexture(int tileId) {
    if (tileId == Wall || tileId == Unpathed) {
        return (BWHT "  " KNRM);
    }
    if (show_solution && tileId == Solution) {
        return (BGRE "  " KNRM);
    }
    return "  ";
}

// Function to render the field by printing its tile textures
void renderField(Field field) {
    for (int y = 0; y < field.height; y++) {
        for (int x = 0; x < field.width; x++) {
            int tile = getTile(field, createPoint(x, y));
            const char *tileTexture = getTileTexture(tile);
            printf("%s", tileTexture);
        }
        printf("\n");
    }
}

// Function to free the allocated memory for the Field
void freeField(Field field) {
    for (int y = 0; y < field.height; y++) {
        free(field.data[y]);
    }
    free(field.data);
}

Point findTile(Field field, int targetTile) {
    for (int y = 0; y < field.height; y++) {
        for (int x = 0; x < field.width; x++) {
            Point tile_pos = createPoint(x, y);
            int tile = getTile(field, tile_pos);
            if (tile == targetTile) {
                return tile_pos;
            }
        }
    }
    return createInvalidPoint();
}

Point movePoint(Point point, int direction, int distance) {
    Point new_point;
    new_point.x = point.x;
    new_point.y = point.y;
    switch (direction) {
        case Up:
            new_point.y -= distance;
            break;
        case Right:
            new_point.x += distance;
            break;
        case Down:
            new_point.y += distance;
            break;
        case Left:
            new_point.x -= distance;
            break;
        default:
            return createInvalidPoint();
    }
    return new_point;
}

void moveTraverserInDirection(Field *field, int direction, int trail) {
    Point new_traverser_position = movePoint(traverser, direction, 1);
    // Update the traverser position
    traverser = new_traverser_position;

    // Change tile to trail
    changeTile(field->data, new_traverser_position, trail);
}

void moveTraverserInOtherDirection(Field *field, int direction, int trail) {
    Point new_traverser_position = movePoint(traverser, direction, 1);

    // Change tile to trail
    changeTile(field->data, traverser, trail);

    // Update the traverser position
    traverser = new_traverser_position;
}

int getTileInTraverserDirection(Field field, int direction, int distance) {
    Point check_position = movePoint(traverser, direction, distance);

    return getTile(field, check_position);
}

void printTraverserPosition() {
    printf("Traverser is at x%d, y%d\n", traverser.x, traverser.y);
}

int getRandomDirection() {
    int random = rand();
    return random % 4;
}

int getRandomRotationDirection() {
    int random = rand();
    return random % 2 == 0 ? 1 : -1;
}

bool canMoveTwice(Field *field, int direction) {
    int tile = getTileInTraverserDirection(*field, direction, 1);

    if (tile != Unpathed) return false;

    int tile_after = getTileInTraverserDirection(*field, direction, 2);

    if (tile_after != Unpathed) return false;

    return true;
}

// Returns true if successfully moved in any direction
bool traversalIteration(Field *field) {
    int random_direction = getRandomDirection();
    int rotation_direction = getRandomRotationDirection();
    for (int rotation = 0; rotation <= 3; rotation++) {
        int direction = ((random_direction + rotation * rotation_direction) + 4) % 4;

        if (!canMoveTwice(field, direction)) continue;

        moveTraverserInDirection(field, direction, current_trail);
        moveTraverserInDirection(field, direction, current_trail);
        return true;
    }
    return false;
}

void traversalUndo(Field *field) {
    for (int direction = 0; direction <= 3; direction++) {
        int tile = getTileInTraverserDirection(*field, direction, 1);

        if (tile != Solution) continue;

        moveTraverserInOtherDirection(field, direction, Branch);
        moveTraverserInOtherDirection(field, direction, Branch);
    }
}

bool isTraverserAtExit(Field field) {
    return (traverser.x == field.width - 2 &&
            traverser.y == field.height - 2);
}

void removeTraverser(Field *field) {
    if (getTile(*field, traverser) != Solution) {
        changeTile(field->data, traverser, Branch);
    }
    traverser.x = -1;
    traverser.y = -1;
}

void traverseField(Field *field) {
    while (!isTraverserAtExit(*field)) {
        bool move_successful = traversalIteration(field);

        if (!move_successful) {
            traversalUndo(field);
        }
    }
    removeTraverser(field);
}

void traverseFieldWithoutUndo(Field *field) {
    while (traversalIteration(field)) {};

    removeTraverser(field);
}

void lookForPotentialBranches(Field *field) {
    for (int y = 1; y <= field->height - 2; y += 2) {
        for (int x = 1; x <= field->width - 2; x += 2) {
            Point check_position = createPoint(x, y);

            int tile = getTile(*field, check_position);

            if (tile != Solution && tile != Branch) continue;

            traverser = check_position;

            traverseFieldWithoutUndo(field);
        }
    }
}

int main() {
    // Seed the random number generator
    srand(time(NULL));

    // Start the timer to measure the time taken
    float start_time = (float)clock() / CLOCKS_PER_SEC;


    printf(KGRE "Maze Generator!\n\n" KNRM);


    // Ask user for row and column count and whether to show the solution or not
    int rows, cols;
    printf("Enter amount of rows: ");
    scanf("%d", &rows);
    printf("Enter amount of columns: ");
    scanf("%d", &cols);
    printf("Show solution? [y/n]: ");
    char show_solution_input;
    scanf(" %c", &show_solution_input);
    show_solution = show_solution_input == 'y';

    // Generate the field
    Field field = generateField(rows, cols);

    removeNonEdges(&field);
    addGridPoints(&field);

    // Set the starting point
    changeTile(field.data, createPoint(1, 1), Solution);
    traverser = createPoint(1, 1);

    // Path out a solution
    traverseField(&field);

    // Branch out from the solution where possible
    current_trail = Branch;
    lookForPotentialBranches(&field);

    addStartAndExit(&field);

    // Calculate the time taken
    float end_time = (float)clock() / CLOCKS_PER_SEC;

    // Render the field to the terminal
    renderField(field);

    // Print the time taken
    printf("Time taken: %f seconds\n", end_time - start_time);

    // Free the allocated emory for the Field
    freeField(field);
}
