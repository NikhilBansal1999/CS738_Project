/* branching on sensitive data*/

int __attribute__((sensitive)) bar (int __attribute__((sensitive)) a, int* b)
{
	if(*b == a)
	{
		return a;
	}
	else
	{
		return a = *b + 3; 
	}
}
