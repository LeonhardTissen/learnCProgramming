#include <stdio.h>

int main() {
	printf("Enter your name: ");
	char name[32];
	// %s is used to read a string
	scanf("%s", &name);

	printf("Hello, %s!\n", name);
    return 0;
}
