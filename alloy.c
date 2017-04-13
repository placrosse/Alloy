/* Alloy - The BareMetal middleware */
/* Copyright 2017 Return Infinity, Inc */

/* Global includes */
#include "libBareMetal.h"

/* Global variables */
unsigned short running = 1;
unsigned char input[256];
int tokens;

/* Strings */
const char alloy_version[] = "Alloy v0.0.1\n";
unsigned char cls_string[] = "cls";
unsigned char dir_string[] = "dir";
unsigned char ver_string[] = "ver";
unsigned char exit_string[] = "exit";
unsigned char help_string[] = "help";

/* Built-in functions*/
int str_cmp(unsigned char str1[], unsigned char str2[]);
int str_parse(unsigned char str[]);
void str_chomp(unsigned char str[]);
int str_len(unsigned char str[]);

/* Program code */
int main()
{
	b_output(alloy_version);
	while (running == 1)
	{
		b_output("> ");
		b_input(input, 255);
		tokens = str_parse(input);
		if (str_cmp(input, exit_string) == 0)
			running = 0;
		else if (str_cmp(input, ver_string) == 0)
			b_output(alloy_version);
		else if (str_cmp(input, help_string) == 0)
			b_output("Available commands: ver, help, exit\n");
		else
			b_output("Unknown command\n");
	}
	b_output(quit_string);
}

/* string compare
 * returns 0 if equal, 1 if not equal
 */
int str_cmp(unsigned char str1[], unsigned char str2[])
{
	int flag = -1;
	for (int i=0; str1[i] != '\0'; i++)
	{
		if (str1[i] == str2[i])
			flag = 0;
		else
		{
			flag = 1;
			break;
		}
	}
	return flag;
}

/* string parse
 * Remove extra spaces and return number of words in string
 */
int str_parse(unsigned char str[])
{
	int i = 0;
	int c = 0;
	int x = 0;

	str_chomp(str); // remove leading and trailing spaces

	for(i=x=0; str[i]; ++i)
		if((str[i] != ' ') || (i>0 && (str[i-1] != ' ')))
			str[x++] = str[i];
	str[x] = '\0';

	x = 0;
	for (i=0; i<str_len(str); i++)
	{
		if (str[i] == ' ')
			x = 0;
		else
		{
			if (x == 0)
				c++;
			x = 1;
		}
	}

	return c;
}

/* string chomp
 * Remove leading and trailing spaces from a string
 */
void str_chomp(unsigned char str[])
{
	int i = 0, begin = 0;
	int end = str_len(str) - 1;

	while (str[begin] == ' ')
		begin++;

	while ((end >= begin) && (str[end] == ' '))
		end--;

	for (i = begin; i <= end; i++)
		str[i - begin] = str[i];

	str[i - begin] = '\0';
}

/* string length
 * returns the length of a string not including the null terminator
 */
int str_len(unsigned char str[])
{
	int count = 0;
	while (str[count] != '\0')
		count++;
	return count;
}
