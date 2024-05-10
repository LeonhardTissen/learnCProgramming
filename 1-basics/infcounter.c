#include <stdio.h>

int main() {
	int counter = 0;
	while (1) {
		if (counter % 100000000 == 0) {
			printf("%d\n", counter);
		}
		counter++;
	}
    return 0;
}
