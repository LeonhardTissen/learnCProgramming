#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NAME_SZ 32

typedef struct {
	char firstName[MAX_NAME_SZ];
	char lastName[MAX_NAME_SZ];
	char age[MAX_NAME_SZ];
	char job[MAX_NAME_SZ];
} Person;

Person createPerson(char firstName[], char lastName[], char age[], char job[]) {
	Person person;
	strcpy(person.firstName, firstName);
	strcpy(person.lastName, lastName);
	strcpy(person.age, age);
	strcpy(person.job, job);
	return person;
}

// Print information about a person
void printAboutMe(Person person) {
	printf("My name is %s %s, I am %s years old and I work as a %s.\n", person.firstName, person.lastName, person.age, person.job);
}

// Print information about a person in the perspective of the user
void printAboutYou(Person person) {
	printf("Your name is %s %s, you are %s years old and you work as a %s.\n", person.firstName, person.lastName, person.age, person.job);
}

char* removeTrailingNewLine(char str[]) {
	int lastChar = strlen(str) - 1;

	// If the last character is a new line, replace it with a null terminator
	if (str[lastChar] == '\n') {
		str[lastChar] = '\0';
	}
	return str;
}

char* receiveInput(char nameType[]) {\
	// Allocate memory for the input
	char *input = malloc(MAX_NAME_SZ);

	// Ask for the input
	printf("Enter your %s: ", nameType);

	// Read the input
	fgets(input, MAX_NAME_SZ, stdin);

	// Remove the trailing new line
	input = removeTrailingNewLine(input);

	return input;
}

int main() {
	// Example of creating a person
	Person john = createPerson("John", "Doe", "30", "Software Developer");
	printAboutMe(john);

	Person jane = createPerson("Jane", "Foo", "25", "Designer");
	printAboutMe(jane);

	// Now it's the users turn
	printf("What about you?\n");

	char* firstName = receiveInput("first name");

	char* lastName = receiveInput("last name");

	char* age = receiveInput("age");

	char* job = receiveInput("job");

	Person you = createPerson(firstName, lastName, age, job);

	printAboutYou(you);

	return 0;
}
