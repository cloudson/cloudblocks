#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.hpp"

char* Helper::str_cpy(const char* needle)
{
	int length = strlen(needle) + 1;
	char* copy = (char*)malloc(length * sizeof(char*));
	strcpy(copy, needle);
	copy[length] = '\n';
	return copy;
}

char* Helper::str_cat(const char* string1, const char* string2)
{
	int length = strlen(string1) + strlen(string2) + 2;
	char* newString = (char*)malloc(length * sizeof(char*));
	strcat(newString, string1);
	strcat(newString, string2);

	return newString;
}

bool Helper::str_eq(const char* string1, const char* string2) 
{
	return 0 == strcmp(string1, string2);

}
