#include <stdio.h>

void printSum(int a, int b) {
	int sum = a + b;
	// %d is used to print an integer
	printf("%d+%d=%d\n", a, b, sum);
}

void printDifference(int a, int b) {
	int difference = a - b;
	printf("%d-%d=%d\n", a, b, difference);
}

void printProduct(int a, int b) {
	int product = a * b;
	printf("%d*%d=%d\n", a, b, product);
}

void printQuotient(int a, int b) {
	// We need to cast a to float to get a float result
	float quotient = (float)a / b;
	// %f is used to print a float
	// %.2f is used to print a float with 2 decimal places
	printf("%d/%d=%.2f\n", a, b, quotient);
}

int main() {
	int a, b;
    
	printf("Enter two numbers: ");
	scanf("%d %d", &a, &b);

	printSum(a, b);
	printDifference(a, b);
	printProduct(a, b);
	printQuotient(a, b);

    return 0;
}
