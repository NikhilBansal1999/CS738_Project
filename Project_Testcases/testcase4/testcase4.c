//Passing a private value, tmp3, to a public parameter!

int function_call(int x, int y)
{
	int ret;
	ret = x + y;
	return ret;
}

void myfunction()
{
	int  x,y,z;
	int __attribute__((sensitive)) result;
	result = x + y + z;
	z = function_call(result,x);	
	

}
