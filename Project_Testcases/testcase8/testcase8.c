//ERROR: Branching on private value cmp
int myfunction()
{
	int __attribute__((sensitive)) x=1;	
	int num=10;
	while (x<10)
	{
		    x++;
	}
	return num;
}
