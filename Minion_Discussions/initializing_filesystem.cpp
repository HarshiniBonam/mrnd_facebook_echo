//Program to initialize the filesystem.

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>

struct group
{
	unsigned int number_of_users;
	unsigned int number_of_posts;
	unsigned int direct_post[8];
	unsigned int indirect_post[4];
	unsigned int indirect_indirect_post;
	char description[58];
	char group_name[10];
};

group discussion_group[8] = 
{
	{ 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0 ,0 ,0 }, 0, "Its all about sports buzz.", "Sports" },
	{ 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 0 }, 0, "Literature in all languages.", "Litrature" },
	{ 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 0 }, 0, "Current affairs and puzzles everyday.", "IKnow!" },
	{ 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 0 }, 0, "All time favorites are here.", "MuviMuzic" },
	{ 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 0 }, 0, "Various art forms information is shared.", "Arts" },
	{ 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 0 }, 0, "Cooking isn't Rocket science.", "Food" },
	{ 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 0 }, 0, "Step out and put your feet to ground.", "Dance" },
	{ 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0}, { 0, 0, 0, 0 }, 0, "Know about environment around us.", "Nature" },
};

void put_initial(FILE *fp)
{
	fprintf(fp, "pb");
}

void fill_bit_vector(FILE *fp)
{
	char zero = (char)(0), one = char(255);
	//fill 2**13 + 8 bits with 1 since they are occupied by the bit vector and 8 groups.
	for (int i = 0; i <= 1024; i++)
	{
		fwrite(&one, sizeof(one), 1, fp);
	}
	//fill (2**23) - (2**13) bits with 0 since they are empty.
	for (unsigned int i = 0; i < 1047550; i++)
	{
		fwrite(&zero, sizeof(zero), 1, fp);
	}
	fwrite("h", 1, 1, fp);
	fflush(fp);
}

void fill_groups(FILE *fp)
{
	for (int i = 0; i < 8; i++)
		fwrite(&discussion_group[i], sizeof(discussion_group[i]), 1, fp);
	fflush(fp);
}

void initialize_filesystem()
{
	FILE *fp = fopen("file_system_copy1.hex", "wb+");
	put_initial(fp);
	fill_bit_vector(fp);
	fill_groups(fp);
	fclose(fp);
}

int main()
{
	initialize_filesystem();
	return 0;
}