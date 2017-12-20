// Disscussion_Forum.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>


void user_login(FILE *fp)
{
	char *user_name = (char *)calloc(10, sizeof(char));
	char abc[10];
	printf("\nEnter your username\t:\t");
	scanf("%10[^\n]\n", user_name);
	fp = fopen("file_system.bin", "wb+");
	fscanf(fp, "%s", abc);
	printf("%s", abc);
}

void init(FILE *fp)
{
	char  login_choice = '\0';
	while (true)
	{
		printf("\n__________________________WELCOME TO MINION DISCUSSION__________________________\n");
		printf("\nDo you want to login ? (y/n)\t:\t");
		scanf("%c", login_choice);
		if (login_choice == 'y' || login_choice == 'Y')
			user_login(fp);
		else if (login_choice == 'n' || login_choice == 'N')
			break;
		else
			printf("\nEnter a valid choice.\n");
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	FILE *fp;
	init(fp);
	_getch();
	return 0;
}

