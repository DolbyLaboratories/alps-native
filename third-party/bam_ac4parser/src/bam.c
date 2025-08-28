#include <bam.h>
#include <stdio.h>
#define MAX_ENTRIES 100000

char buffer[MAX_ENTRIES];
char string_return[MAX_ENTRIES];

/* converts int to char* */
char*
BAM_to_string(int value)
{
	sprintf(buffer, "%d", value);

	return buffer;
}

/* concatenation of two strings */
const char*
BAM_string_concat(const char* string_1
	, char* string_2)
{
	strcpy(string_return, string_1);
	strcat(string_return, string_2);

	return string_return;
}

