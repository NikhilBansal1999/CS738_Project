//No errors detected!
#include <stdio.h>
#include <string.h>
char __attribute__((sensitive)) s1[100] = "Welcome to ";
char* __attribute__((sensitive)) function_call()
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
	char __attribute__((sensitive)) *a;
	a = function_call();
	
	
}
