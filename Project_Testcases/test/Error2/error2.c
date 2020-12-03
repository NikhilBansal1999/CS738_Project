__attribute__((sensitive)) int global_var ; // global variable as sensitive

int fun(){
	__attribute__((sensitive)) int a ; // sensitive local variable
	a += global_var;				   
	return a;							// returning a sensitive data but return type is not sensitive
}

