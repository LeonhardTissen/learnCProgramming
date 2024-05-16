#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Define colors for the terminal
#define KNRM  "\x1B[0m"
#define BWHT  "\x1B[47m"

enum Tile {
    Wall,
    Unpathed,
    Traverser,
    Exit,
    Trail,
    Herring,
    Unknown
};

enum Direction {
    Up,
    Right,
    Down,
    Left
};

typedef struct {
    int** data;
    int width;
    int height;
} Field;

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

// Function to generate a Field struct with a 2D matrix of integers
Field generateField(int rows, int cols) {
    Field field;
    field.width = cols * 2 + 1;
    field.height = rows * 2 + 1;

    // Allocate memory for array of pointers to int (rows)
    field.data = (int**)malloc(field.height * sizeof(int*));
    if (field.data == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for each row (array of integers)
    for (int y = 0; y < field.height; y++) {
        field.data[y] = (int*)malloc(field.width * sizeof(int));
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
            field.data[y][x] = Wall;
        }
    }

    return field;
}

// Function to change the tileId at a given position in a matrix
void changeTile(int** data, Point point, int tileId) {
    data[point.y][point.x] = tileId;
}

// Function to clean out anything in a field except for the edges
void removeNonEdges(Field *field) {
    for (int y = 0; y < field->height; y++) {
        for (int x = 0; x < field->width; x++) {
            if (y == 0 || y == field->height - 1 || x == 0 || x == field->width - 1) {
                // Keep the edges (do nothing)
            } else {
                // Change non-edge tiles to Unpathed
                changeTile(field->data, createPoint(x, y), Unpathed);
            }
        }
    }
}

void addStartAndExit(Field *field) {
    Point start = createPoint(1, 0);
    changeTile(field->data, start, Trail);
    Point finish = createPoint(field->width - 2, field->height - 1);
    changeTile(field->data, finish, Trail);
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

void addTraverser(Field *field) {
    Point start_pos = createPoint(1, 1);
    changeTile(field->data, start_pos, Traverser);
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
const char* getTileTexture(int tileId) {
    /*switch(tileId) {
        case Wall:
            return "[]";
        case Unpathed:
            return "  ";
        case Traverser:
            return "hi";
        case Trail:
            return "- ";
        case Herring:
            return "..";
        case Exit:
            return "!!";
        default:
            return "??";
    }*/
    switch (tileId) {
        case Wall:
        case Unpathed:
            return (BWHT "  ");
        default:
            return (KNRM "  ");
    }
}

// Function to render the field by printing its tile textures
void renderField(Field field) {
    for (int y = 0; y < field.height; y++) {
        for (int x = 0; x < field.width; x++) {
            int tile = getTile(field, createPoint(x, y));
            const char* tileTexture = getTileTexture(tile);
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

Point getTraverser(Field field) {
    return findTile(field, Traverser);
}

void moveTraverserInDirection(Field *field, int direction, int trail) {
    Point traverser_position = getTraverser(*field);
    Point new_traverser_position = movePoint(traverser_position, direction, 1);

    // Remove tile the traverser is on
    changeTile(field->data, traverser_position, trail);

    // Add the traverser back
    changeTile(field->data, new_traverser_position, Traverser);
}

int getTileInTraverserDirection(Field field, int direction, int distance) {
    Point traverser_position = getTraverser(field);
    Point check_position = movePoint(traverser_position, direction, distance);

    return getTile(field, check_position);
}

void printTraverserPosition(Field *field) {
    Point traverser = getTraverser(*field);

    printf("Traverser is at x%d, y%d\n", traverser.x, traverser.y);
}

int getRandomDirection() {
    int random = rand();
    return random % 4;
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
    for (int rotation = 0; rotation <= 3; rotation ++) {
        int direction = (random_direction + rotation) % 4;

        if (!canMoveTwice(field, direction)) continue;

        moveTraverserInDirection(field, direction, Trail);
        moveTraverserInDirection(field, direction, Trail);
        return true;
    }
    return false;
}

void traversalUndo(Field *field) {
    for (int direction = 0; direction <= 3; direction ++) {
        int tile = getTileInTraverserDirection(*field, direction, 1);

        if (tile != Trail) continue;

        moveTraverserInDirection(field, direction, Herring);
        moveTraverserInDirection(field, direction, Herring);
    }
}

bool isTraverserAtExit(Field field) {
    Point traverser_position = getTraverser(field);

    return (traverser_position.x == field.width - 2 &&
            traverser_position.y == field.height - 2);
}

void removeTraverser(Field *field) {
    Point traverser_position = getTraverser(*field);

    changeTile(field->data, traverser_position, Trail);
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
    while(traversalIteration(field)) {};

    removeTraverser(field);
}

void lookForPotentialBranches(Field *field) {
    for (int y = 1; y <= field->height - 2; y += 2) {
        for (int x = 1; x <= field->width - 2; x += 2) {
            Point check_position = createPoint(x, y);

            int tile = getTile(*field, check_position);

            if (tile != Trail && tile != Herring) continue;

            changeTile(field->data, check_position, Traverser);

            traverseFieldWithoutUndo(field);
        }   
    }
}

int main() {
    // Randomize seed
    srand(time(NULL));

    printf("Maze Generator!\n\n");
    int rows, cols;
    
	printf("Enter rows: ");
	scanf("%d", &rows);
    printf("Enter columns: ");
	scanf("%d", &cols);

    Field field = generateField(rows, cols);

    removeNonEdges(&field);
    addGridPoints(&field);
    addTraverser(&field);

    traverseField(&field);
    lookForPotentialBranches(&field);

    addStartAndExit(&field);

    renderField(field);
    
    // Free the allocated emory for the Field
    freeField(field);
}
