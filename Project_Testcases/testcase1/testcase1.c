//Passing a private value, a, to a public parameter!

#include<stdio.h>

int function_call()
{
	int __attribute__((sensitive)) ret,x,y;
	ret = x + y;
	return ret;
}

void myfunction()
{
	int __attribute__((sensitive)) a,b,c;
	printf("Enter value for a:\n");
	scanf("%d",&a);
	printf("Enter value for b:\n");
	scanf("%d",&b);
	c = function_call();
	c = c + a + b;	

}
