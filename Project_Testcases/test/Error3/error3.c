int __attribute__((sensitive)) global;

int __attribute__((sensitive)) fun( int a, int __attribute__((sensitive)) b)
{
	b = b + 2;
	a = global;			// assigning a sensitive data to a non sensitive variable
	return b;

}

