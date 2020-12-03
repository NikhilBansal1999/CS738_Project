//ERROR: Function function_call marked public, but is returning a private value!

#include <stdio.h>
#include <string.h>
char __attribute__((sensitive)) s1[100] = "Welcome to ";
char* function_call()
{
	char s2[] = "cs738";
	int length, i;
  	length = 0;
  	while (s1[length] != '\0')
  	{
    		++length;
  	}

    	for (i = 0; s2[i] != '\0'; ++i, ++length)
    	{
    		s1[length] = s2[i];
  	}

	s1[length] = '\0';
	return s1;

}

void myfunction()
{
	char *a;
	a = function_call();
	puts(a);
	
}
