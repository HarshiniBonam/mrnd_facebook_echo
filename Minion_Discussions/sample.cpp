#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
/* Our structure */
struct rec
{
	unsigned int i;
};

int main()
{
	int counter;
	FILE *ptr_myfile;
	rec my_record;

	ptr_myfile = fopen("test.bin", "rb+");
	if (!ptr_myfile)
	{
		printf("Unable to open file!");
		return 1;
	}
	my_record.i = 8388608;
	fwrite(&my_record, sizeof(rec), 1, ptr_myfile);
	fflush(ptr_myfile);
	printf("\nftell - %d", ftell(ptr_myfile));
	//fseek(ptr_myfile, sizeof(rec), SEEK_END);
	rewind(ptr_myfile);
	printf("\nftell - %d", ftell(ptr_myfile));
	fread(&my_record, sizeof(rec), 7, ptr_myfile);
	printf("\n%u\n", my_record.i);
	fclose(ptr_myfile);
	_getch();
	return 0;
}
