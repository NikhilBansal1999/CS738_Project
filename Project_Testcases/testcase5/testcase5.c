//ERROR: Parameter function_call::x marked as public, but inferred to be private!

int  __attribute__((sensitive)) ret;
int __attribute__((sensitive)) function_call(int x,int __attribute__((sensitive)) y)
{
	y = y + 2;
	x = ret ;
	return y;
}


