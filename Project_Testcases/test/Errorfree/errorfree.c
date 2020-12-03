void fun(int __attribute__((sensitive)) a, int __attribute__((sensitive)) b)
{
	int __attribute__((sensitive)) *ptr;
	int i=5;
	while(i--)
	{
		b = a+2;
	}
	ptr = &b;
	a = *ptr;
		
}
