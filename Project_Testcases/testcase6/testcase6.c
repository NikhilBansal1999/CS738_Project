//ERROR: Parameter fetch_or_zero::ptr marked as public, but inferred to be private!

int __attribute__((sensitive)) global_var = 0;

typedef int my_int;

int __attribute__((sensitive)) fetch_or_zero(int* __attribute__((sensitive)) ptr, int __attribute__((sensitive)) a, int b)
{
	
	b = global_var;
	int* tmp_copy = ptr;
	my_int __attribute__((sensitive)) a_num;
	if(ptr == (void*)0)
	{
		return 0;
	}
	else
	{
		a_num = *tmp_copy;
		return a_num + global_var;
	}
}

int __attribute__((sensitive)) call_function(int* a, int* b, int* c)
{
	int __attribute__((sensitive)) r1 = fetch_or_zero(a, 0, 0);
	int __attribute__((sensitive)) r2 = fetch_or_zero(b, 0, 0);
	int __attribute__((sensitive)) r3 = fetch_or_zero(c, 0, 0);
	return r1+r2+r3;
}
