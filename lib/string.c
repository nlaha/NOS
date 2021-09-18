/*
    Author: Nathan Laha
    Description: This is a string library that contains functions for
    manipulating strings.
*/

#include "string.h"

// Description: This function combines two strings together.
// Input: String1, String2
// Output: String1 + String2
char *strcat(char *restrict s1, const char *restrict s2)
{
    char *orig = s1;
    while (*s1)
    {
        s1++;
    }
    strcpy(s1, s2);
    return orig;
}

// Description: This function copies a string to another string.
// Input: String1, String2
// Output: String1 = String2
char *strcpy(char *restrict s1, const char *restrict s2)
{
    while (*s2 != '\0')
    {
        *s1++ = *s2++;
    }
    *s1 = '\0';
    return s1;
}