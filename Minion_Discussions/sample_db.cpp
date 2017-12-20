#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include<conio.h>
#include <stdlib.h>
#define MAX_USERS  5
#define MAX_POSTS  5
#define MAX_COMMENTS 5
#define MAX_GROUPS 5

struct group
{
	unsigned int number_of_posts;
	unsigned int number_of_users;
};

group fb_group[MAX_GROUPS];

struct post
{
	unsigned int user_id;
	unsigned int group_id;
//	unsigned int number_of_comments;
	unsigned int number_of_likes;
	unsigned int posts[MAX_POSTS];
	unsigned int number_posts;
	char post_text[100];
};

post fb_post[MAX_POSTS];

//struct comments
//{
//	unsigned int group_id;
//	unsigned int post_id;
//	unsigned int user_id;
//	unsigned int number_of_likes;
//};

//comments fb_comments[MAX_COMMENTS];

struct user
{
	unsigned int group[MAX_GROUPS];
	unsigned int number_of_groups;
	unsigned int post[MAX_POSTS];
	unsigned int number_of_groups;
	//unsigned int comments[MAX_COMMENTS];
};

user fb_user[MAX_USERS];

void init()
{
	char * query = (char *)calloc(50, sizeof(char));
	for (int i = 0; i < MAX_USERS; i++)
	{
		printf("\nFor User %d : \n\n", i + 1);
		printf("\nEnter the size of number of groups and number of post : ")
		scanf("%d %d")
	}
	for (int i = 0; i < MAX_GROUPS; i++)
	{

	}
	for (int i = 0; i < MAX_POSTS; i++)
	{

	}
}

int main()
{
	init();
	_getch();
	return 0;
}