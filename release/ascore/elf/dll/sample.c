#include <stdio.h>

void goodBye(void)
{
	printf("Good Bye!\n");
}

int main(int argc, char* argv[])
{
	int i;
	for(i=0; i<argc; i++)
	{
		printf("argv[%d] = %s\n", i, argv[i]);
	}
	printf("Hello World!\n");
	goodBye();
	return 0;
}
