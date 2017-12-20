#include "stdafx.h"
#include <stdio.h>

struct filetable
{
	unsigned int roll_number;
	unsigned int offset;
} ft;

int main()
{
	FILE *fp = fopen("filesystem.bin", "rb+");
	ft.offset = 0;
	ft.roll_number = 0;
	for (int i = 0; i < 8192; i++)
	{
		fwrite(&ft, sizeof(filetable), 1, fp);
	}
}