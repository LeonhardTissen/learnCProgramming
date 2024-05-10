#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	srand(time(NULL));
	int randomNumber = rand();

	printf("Random number: %d\n", randomNumber);
    return 0;
}
