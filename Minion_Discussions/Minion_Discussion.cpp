// Disscussion_Forum.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <conio.h>
#include<string.h>
#include <stdlib.h>
#define MAX_USERS 10
#define MAX_COMMENTS_PER_POST 4232
#define MAX_POSTS_PER_GROUP 16648
#define BIT_VECTOR_START_BYTE 2
#define BIT_VECTOR_END_BYTE 1048577

struct comment
{
	int likes;
	char user_name[8];
	char comment_text[116];
}post_comment;

struct post
{
	unsigned int likes;
	unsigned int number_of_comments;
	unsigned int direct_comment[8];
	unsigned int indirect_comment[4];
	unsigned int indirect_indirect_comment;
	char post_text[188];
	char user_name[8];
}group_post;

struct group
{
	unsigned int number_of_users;
	unsigned int number_of_posts;
	unsigned int direct_post[8];
	unsigned int indirect_post[4];
	unsigned int indirect_indirect_post;
	char description[58];
	char group_name[10];
} discussion_group;

void append_to_string(char *dest, char *src)
{
	int i = 0, j = 0;
	for (i = 0; *(dest + i) != '\0'; i++);
	for (j = 0; *(src + j) != '\0'; j++, i++)
		*(dest + i) = *(src + j);
	*(dest + i) = '\0';
}

void string_reverse(char *str, int j)
{
	int i = 0;
	char temp;
	while (i <= j)
	{
		temp = *(str + i);
		*(str + i) = *(str + j);
		*(str + j) = temp;
		i++;
		j--;
	}
}

void convert_int_to_str(char *str, int value)
{
	int i = 0;
	if (value == 0)
		*(str) = '0';
	else if (value > 0)
	{
		while (value != 0)
		{
			*(str + i++) = ('0' + (value % 10));
			value /= 10;
		}
		string_reverse(str, i-1);
	}
	else
	{
		*(str + i++) = '-';
		convert_int_to_str(str, (value * -1));
	}
}

int user_actions_in_group()
{
	int choice = 0;
	printf("\n1. View recent posts.\n2. Add posts.\n3. Exit from group.");
	while (true)
	{
		printf("\nEnter your choice : ");
		scanf("%d", &choice);
		if (choice > 3 && choice < 0)
			printf("\nInvalid choice.");
		else
			break;
	}
	return choice;
}

bool new_user(char *filename, char *new_user)
{
	FILE *ufile = fopen(filename, "r+");
	while (getc(ufile) != EOF)
	{
		fseek(ufile, -1, SEEK_CUR);
		char *uname = (char*)calloc(8, sizeof(char));
		if (fscanf(ufile, "%[^,],", uname) != 0)
		{
			if (strcmp(uname, new_user) == 0)
			{
				fclose(ufile);
				return false;
			}
		}
	}
	fclose(ufile);
	return true;
}

void add_new_user(char *filename, char *new_user)
{
	FILE *ufile = fopen(filename, "a");
	fseek(ufile, 0, SEEK_END);
	//	fprintf(ufile, "%s", new_user);
	fputs(new_user, ufile);
	fputc(',', ufile);
	fflush(ufile);
	fclose(ufile);
}

int user_actions_in_post(FILE *fp, group discussion_group)
{
	int choice = 0;
	printf("\n1. Like\n2. View Comments\n3. Add comment\n4. Delete post\n5. Go to previous post\n6. Go to next post\n7.Go to group home\n");
	while (true)
	{
		printf("\nEnter your choice : ");
		scanf("%d", &choice);
		if (choice > 0 && choice < 8)
			break;
		printf("\nInvalid choice.");
	}
	return choice;
}

//returns block number that is free
unsigned int find_blocks(FILE *fp, int number_of_blocks)
{
	unsigned char byte, temp1;
	fseek(fp, 1027, SEEK_SET);
	for (unsigned int i = 1027, j = 0, k = 0; i <= BIT_VECTOR_END_BYTE; i++)
	{
		byte = (unsigned char)getc(fp);
		if (byte == (unsigned char)0)
		{
			if (number_of_blocks == 1)
				byte = (unsigned char)1;
			else
				byte = (unsigned char)3;
			fseek(fp, -1, SEEK_CUR);
			fwrite(&byte, sizeof(byte), 1, fp);
			return (1049602 + (i - 1027));
		}
		if (byte != (unsigned char)255)
		{
			temp1 = byte;
			if (number_of_blocks == 1)
			{
				for (j = 0; j < 8; j++)
					if (((temp1 >> j) & 1) == 0)
						break;
				if (j != 8)
				{
					byte = (byte) | (1 << j);
					fseek(fp, -1, SEEK_CUR);
					fwrite(&byte, sizeof(byte), 1, fp);
					return 1049602 + ((((i - 1027) * 8) + j) * 128);
				}
			}
			else if (number_of_blocks == 2)
			{
				for (j = 0; j < 7; j++)
					if ((((temp1 >> j) & 1) == 0) && (((temp1 >> (j + 1)) & 1) == 0))
						break;
				if (j != 7)
				{
					byte = (byte) | (3 << j);
					fseek(fp, -1, SEEK_CUR);
					fwrite(&byte, sizeof(byte), 1, fp);
					return 1049602 + ((((i - 1027) * 8) + j) * 128);
				}
			}
		}
	}
	return -1;
}

void view_comments(FILE *fp, int i, int post_offset, char *user_name)
{
	printf("post offset - %d", post_offset);
	int choice = 0;
	post_comment = { 0, "", "" };
	fseek(fp, post_offset, SEEK_CUR);
	fread(&discussion_group, sizeof(discussion_group), 1, fp);
	if (i < 8)
	{
		fseek(fp, group_post.direct_comment[i] , SEEK_SET);
		fread(&post_comment, sizeof(post_comment), 1, fp);
		printf("\nOffset : %u", group_post.direct_comment[i]);
		printf("\n................................................................................................................................");
		printf("\n%5d.  %8s - %-188s - %d likes", i + 1, post_comment.user_name, post_comment.comment_text, group_post.likes);
		printf("\n................................................................................................................................");
		fseek(fp, -128, SEEK_CUR);
	}
	else if (i > 8 && i < 136)
	{
		int temp, index, indirect_block;
		index = (i - 8) % 32;
		temp = ((i - 8) / 32);

		fseek(fp, (group_post.indirect_comment[index]) + temp, SEEK_SET);
		fread(&indirect_block, 4, 1, fp);

		post_offset = indirect_block;

		fseek(fp, indirect_block, SEEK_SET);
		fread(&post_comment, sizeof(post_comment), 1, fp);
		//printf("\nOffset : %u", );
		printf("\n................................................................................................................................");
		printf("\n%5d.  %8s - %-188s - %d likes", i + 1, post_comment.user_name, post_comment.comment_text, group_post.likes);
		printf("\n................................................................................................................................");
		fseek(fp, -128, SEEK_CUR);
	}
	printf("\n\n1. Like Comment\n2. Previous Comment\n3. Next Comment\n4. Go to Posts.\n");
	while (choice > 4 && choice < 0)
	{
		printf("\nEnter a choice (1-4) : ");
		scanf("%d", &choice);
	}
	switch (choice)
	{
	case 1://Like post
		post_comment = { 0, "", "" };
		fread(&post_comment, 128, 1, fp);
		post_comment.likes += 1;
		//printf("\n -- group like : %d", ftell(fp));
		fseek(fp, -256, SEEK_CUR);
		fwrite(&group_post.likes, 4, 1, fp);
		fflush(fp);
		view_comments(fp, i, post_offset, user_name);
		break;
	case 2: //Previous Comment
		if (i > 0)
			view_comments(fp, i - 1, post_offset, user_name);
		else
			printf("\nNo more comments.\n");
		break;
	case 3:
		group_post = { 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, "", "" };
		fseek(fp, post_offset, SEEK_SET);
		fread(&group_post, sizeof(group_post), 1, fp);
		if (i + 1 < group_post.number_of_comments)
			view_comments(fp, i + 1, post_offset, user_name);
		else
			printf("\nNo more comments.\n");
		break;
		case 4:
			return;
	}
}

void add_comments(FILE *fp, int post_offset, char *user_name)
{
	printf("post offset - %d", post_offset);
	char *comment_text = (char *)calloc(118, sizeof(char));
	int comment_block = 0, indirect_block = 0, indirect_indirect = 0, indirect_indirect_block, temp = 0, i = 0;
	post_comment = { 0,"", "" };
	while (true)
	{
		printf("\n\nEnter your post  < 116 chars : ");
		fflush(stdin);
		gets(comment_text);
		if (strlen(comment_text) < 117)
			break;
	}
	comment_text[116] = '\0';
	comment_text[117] = '\0';
	//block to store the message.
	comment_block = find_blocks(fp, 1);

	if (comment_block != -1)
	{
		fseek(fp, post_offset, SEEK_SET);
		fread(&group_post, sizeof(group_post), 1, fp);
		//printf("\n---group : %d", ftell(fp));

		if (group_post.number_of_comments < 8)
		{
			group_post.direct_comment[group_post.number_of_comments] = comment_block;
			group_post.number_of_comments += 1;
			fseek(fp, post_offset, SEEK_SET);
			fwrite(&group_post, sizeof(group_post), 1, fp);
			fseek(fp, (comment_block), SEEK_SET);
			printf("\nBlock number : %u", comment_block);
			//printf("---post : %d", ftell(fp));
			//get post text and post username.
			fread(&post_comment, 128, 1, fp);
			strcpy(post_comment.comment_text, comment_text);
			strcpy(post_comment.user_name, user_name);
			fwrite(&post_comment, sizeof(post_comment), 1, fp);
		}
		else if (group_post.number_of_comments > 7 && group_post.number_of_comments < 136)
		{
			//indirect post
			int comment_block_index = 0, indirect_block_index = 0, value = 0;

			fseek(fp, post_offset, SEEK_SET);
			fread(&discussion_group, sizeof(discussion_group), 1, fp);
			//calculate indices.
			comment_block_index = (group_post.number_of_comments - 8) % 32;
			indirect_block_index = ((group_post.number_of_comments - 8) / 32);

			//Find msg block
			comment_block = find_blocks(fp, 1);
			//Find an indirect block of 128bytes and put the address of the msg into the first 4 bytes.
			if (comment_block_index == 0)
			{
				indirect_block = find_blocks(fp, 1);
				fseek(fp, post_offset, SEEK_SET);
				fread(&discussion_group, sizeof(discussion_group), 1, fp);
				discussion_group.indirect_post[indirect_block_index] = indirect_block;
			}
			else
				indirect_block = indirect_block_index;

			fseek(fp, post_offset, SEEK_SET);
			fread(&discussion_group, sizeof(discussion_group), 1, fp);
			group_post.number_of_comments += 1;
			fseek(fp, post_offset, SEEK_SET);
			fwrite(&discussion_group, sizeof(discussion_group), 1, fp);
			fseek(fp, indirect_block + comment_block_index, SEEK_SET);
			fwrite(&comment_block, 4, 1, fp);
			printf("\n--- Indirect- msg block: %u", comment_block);
			//write the post information like text and user name to file.
			fseek(fp, comment_block, SEEK_SET);
			fread(&post_comment, 128, 1, fp);
			strcpy(post_comment.comment_text, comment_text);
			strcpy(post_comment.user_name, user_name);
			fwrite(&post_comment, sizeof(post_comment), 1, fp);
		}
		else if (group_post.number_of_comments > 135 && group_post.number_of_comments < MAX_COMMENTS_PER_POST)
		{
			//indirect indirect post
		}
		printf("\n................................................................................................................................");
		printf("\n%-8s : %-188s - %5u Likes", post_comment.user_name, post_comment.comment_text, post_comment.likes);
		printf("\n................................................................................................................................");
	}
	else
	{
		printf("\n................................................................................................................................\n");
		printf("\nMemory Full !!\nDelete posts to add new posts.");
		printf("\n................................................................................................................................\n");
	}
}

void delete_message(FILE *fp, int post_offset)
{
	//first find the post and initialize all values in that post to 0.
	//find the parent group that contains the post and initialize it to 0.
	//find the bit location in the bit vector and free the 2 adjacent 2 bits.
}

void view_messages(FILE *fp, int i, int group_offset, char *user_name)
{
	int choice = 0, post_offset = 0;
	group_post = { 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, "", "" };
	fseek(fp, group_offset, SEEK_CUR);
	fread(&discussion_group, sizeof(discussion_group), 1, fp);
	if (i < 8)
	{
		post_offset = discussion_group.direct_post[i];
		fseek(fp, post_offset, SEEK_SET);
		fread(&group_post, sizeof(group_post), 1, fp);
		printf("\nOffset : %u", discussion_group.direct_post[i]);
		printf("\n................................................................................................................................");
		printf("\n%5d.  %8s - %-188s - %d likes - %d comments", i + 1, group_post.user_name, group_post.post_text, group_post.likes, group_post.number_of_comments);
		printf("\n................................................................................................................................");
		fseek(fp, -256, SEEK_CUR);
	}
	else if (i > 8 && i < 136)
	{
		int temp, index, indirect_block;
		index = (i - 8) % 32;
		temp = ((i - 8) / 32);

		fseek(fp, (discussion_group.indirect_post[index]) + temp, SEEK_SET);
		fread(&indirect_block, 4, 1, fp);

		post_offset = indirect_block;

		fseek(fp, indirect_block, SEEK_SET);
		fread(&group_post, sizeof(group_post), 1, fp);
		//printf("\nOffset : %u", );
		printf("\n................................................................................................................................");
		printf("\n%5d.  %-8s - %-188s - %5d likes - %5d comments", i + 1, group_post.user_name, group_post.post_text, group_post.likes, group_post.number_of_comments);
		printf("\n................................................................................................................................");
		fseek(fp, -256, SEEK_CUR);
	}
	else
	{
		int temp1, index1, indirect_block1, temp2, index2, indirect_block2;
		index1 = (i - 136) % 32;
		temp1 = ((i - 136) / 32);
		fseek(fp, (discussion_group.indirect_post[index1]) + temp1, SEEK_SET);
		fread(&indirect_block1, 4, 1, fp);
		fseek(fp, indirect_block1, SEEK_SET);
		//index2 = (i-) % 32;
		temp2 = (indirect_block1) / 32;
		//fseek(fp, )
		fread(&group_post, sizeof(group_post), 1, fp);
		printf("\n................................................................................................................................");
		printf("\n%5d.  %-8s - %-188s - %5d likes - %5d comments", i + 1, group_post.user_name, group_post.post_text, group_post.likes, group_post.number_of_comments);
		printf("\n................................................................................................................................");
	}
	choice = user_actions_in_post(fp, discussion_group);
	switch (choice)
	{
	case 1 ://Like post
		group_post = { 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, "", "" };
		fread(&group_post, 256, 1, fp);
		group_post.likes += 1;
		//printf("\n -- group like : %d", ftell(fp));
		fseek(fp, -256, SEEK_CUR);
		fwrite(&group_post.likes, 4, 1, fp);
		fflush(fp);
		view_messages(fp, i, group_offset, user_name);
		break;
	case 2://View comment
		view_comments(fp, 0, ftell(fp), user_name);
		break;
	case 3://Add comment
		add_comments(fp, ftell(fp), user_name);
		break;
	case 4://Delete post
		printf("\nOption is under version control.\n");
		break;
	case 5://Previous post
		fseek(fp, group_offset, SEEK_SET);
		fread(&discussion_group, 128, 1, fp);
		if (i == 0)
			printf("\nThis is the first post. No more previous.\n");
		else
			view_messages(fp, i - 1, group_offset, user_name);
		break;
	case 6://Next post
		if (i ==discussion_group.number_of_posts)
			printf("\nThis is the last post. No more next posts.\n");
		else
			view_messages(fp, i + 1, group_offset, user_name);
		break;
	case 7 ://Go to group home
		return;
	}	
}

void add_message(FILE *fp, int group_offset, char *user_name)
{
	char *post_text = (char *)calloc(190, sizeof(char));
	int msg_block = 0, indirect_block = 0, indirect_indirect = 0, indirect_indirect_temp = 0, i = 0;
	group_post = { 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, "", "" };
	while (true)
	{
		printf("\n\nEnter your post  < 188 chars : ");
		fflush(stdin);
		gets(post_text);
		if (strlen(post_text) < 188)
			break;
	}
	post_text[188] = '\0';
	post_text[189] = '\0';
	//block to store the message.
	msg_block = find_blocks(fp, 2);

	if (msg_block != -1)
	{
		fseek(fp, group_offset, SEEK_SET);
		fread(&discussion_group, sizeof(discussion_group), 1, fp);
		//printf("\n---group : %d", ftell(fp));

		if (discussion_group.number_of_posts < 8)
		{
			discussion_group.direct_post[discussion_group.number_of_posts] = msg_block;
			discussion_group.number_of_posts += 1;
			fseek(fp, group_offset, SEEK_SET);
			fwrite(&discussion_group, sizeof(discussion_group), 1, fp);
			fseek(fp, (msg_block), SEEK_SET);
			printf("\nBlock number : %d", msg_block);
			//printf("---post : %d", ftell(fp));
			//get post text and post username.
			fread(&group_post, 256, 1, fp);
			strcpy(group_post.post_text, post_text);
			strcpy(group_post.user_name, user_name);
			fwrite(&group_post, sizeof(group_post), 1, fp);
		}
		else if (discussion_group.number_of_posts > 7 && discussion_group.number_of_posts < 136)
		{
			//indirect post
			int msg_block_index = 0, indirect_block_index = 0, value = 0;
			
			fseek(fp, group_offset, SEEK_SET);
			fread(&discussion_group, sizeof(discussion_group), 1, fp);
			//calculate indices.
			msg_block_index = (discussion_group.number_of_posts - 8) % 32;
			indirect_block_index = ((discussion_group.number_of_posts - 8) / 32);

			//Find msg block
			msg_block = find_blocks(fp, 2);
			//Find an indirect block of 128bytes and put the address of the msg into the first 4 bytes.
			if (msg_block_index == 0)
			{
				indirect_block = find_blocks(fp, 1);
				fseek(fp, group_offset, SEEK_SET);
				fread(&discussion_group, sizeof(discussion_group), 1, fp);
				discussion_group.indirect_post[indirect_block_index] = indirect_block;
			}
			else
				indirect_block = indirect_block_index;
			
			fseek(fp, group_offset, SEEK_SET);
			fread(&discussion_group, sizeof(discussion_group), 1, fp);
			discussion_group.number_of_posts += 1;
			fseek(fp, group_offset, SEEK_SET);
			fwrite(&discussion_group, sizeof(discussion_group), 1, fp);
			fseek(fp, indirect_block + msg_block_index, SEEK_SET);
			fwrite(&msg_block, 4, 1, fp);
			printf("\n--- Indirect- msg block: %u", msg_block);
			//write the post information like text and user name to file.
			fseek(fp, msg_block, SEEK_SET);
			fread(&group_post, 256, 1, fp);
			strcpy(group_post.post_text, post_text);
			strcpy(group_post.user_name, user_name);
			fwrite(&group_post, sizeof(group_post), 1, fp);
		}
		else if (discussion_group.number_of_posts > 135 && discussion_group.number_of_posts < MAX_POSTS_PER_GROUP)
		{
			//indirect indirect post
		}
		printf("\n................................................................................................................................");
		printf("\n%-8s : %-188s - %5u Likes - %5u Comments", group_post.user_name, group_post.post_text, group_post.likes, group_post.number_of_comments);
		printf("\n................................................................................................................................");
	}
	else
	{
		printf("\n................................................................................................................................\n");
		printf("\nMemory Full !!\nDelete posts to add new posts.");
		printf("\n................................................................................................................................\n");
	}
}

void open_group(FILE *fp, int group_number, char *user_name)
{
	int grp_choice = -1, grp_action_choice = -1, group_offset = (BIT_VECTOR_END_BYTE  + 1) + ((group_number-1) * 128);
	char filename[18] = "users_group", *grp_num = (char *)calloc(2, sizeof(char)), ext[5] = ".txt";
	filename[12] = '\0';
	ext[4] = '\0';
	convert_int_to_str(grp_num, group_number);
	//Get the record of given group.
	fseek(fp, group_offset, SEEK_SET);
	fread(&discussion_group, sizeof(discussion_group), 1, fp);
	//get userfile name.
	append_to_string(filename, grp_num);
	append_to_string(filename, ext);
	//check if the current user is new user.
	if (new_user(filename, user_name))
	{
		fseek(fp, group_offset, SEEK_SET);
		fread(&discussion_group, sizeof(discussion_group), 1, fp);
		add_new_user(filename, user_name);
		printf("\nWelcome to the group %s.\n\n", user_name);
		discussion_group.number_of_users += 1;
		fseek(fp, group_offset, SEEK_SET);
		fwrite(&discussion_group, sizeof(discussion_group), 1, fp);
		fflush(fp);
	}
	//Take options for group.
	while (grp_action_choice != 3)
	{
		fflush(fp);
		grp_action_choice = user_actions_in_group();
		if (grp_action_choice == 1)
		{
			if (discussion_group.number_of_posts == 0)
				printf("\nNo posts in this group.");
			else
				view_messages(fp, 0, group_offset, user_name);
		}
		else if (grp_action_choice == 2)
		{
			if (discussion_group.number_of_posts != MAX_POSTS_PER_GROUP)
				add_message(fp, group_offset, user_name);
			else
				printf("\nThis group has reached MAXIMUM number of posts.\nTry deleting an old post inorder to add a new post.\n");
		}
		else	return;
	}
}

int user_actions_after_login(FILE *fp)
{
	int choice = 0;
	fseek(fp, BIT_VECTOR_END_BYTE + 1, SEEK_SET);
	printf("\n\n\t0 . logout ?");
	printf("\n\t\t_______GROUPS_______\n");
	for (int i = 0; i < 8; i++)
	{
		//printf("\n%u", ftell(fp));
		group_post = { 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, "", "" };
		fread(&discussion_group, sizeof(discussion_group), 1, fp);
		printf("\n\t%2d : %-10s - %-58s - %10d users - %10d posts", i + 1, discussion_group.group_name, discussion_group.description, discussion_group.number_of_users, discussion_group.number_of_posts);
	}
	while (true)
	{
		printf("\nEnter your choice : ");
		scanf("%d", &choice);
		if (choice > 8 || choice < 0)
		{
			printf("\nInvalid choice.");
		}
		else
			break;
	}
	return choice;
}

void home(char *user_name)
{
	int choice = -1;
	FILE *fp;
	fp = fopen("file_system.hex", "rb+");
	while (true)
	{
		choice = user_actions_after_login(fp);
		if (choice == 0)
			break;
		open_group(fp, choice, user_name);
		fflush(fp);
	}
	fclose(fp);
	printf("\nThank you %s, you account has been successfully logged out.\n", user_name);
}

//method to allow a user to login.
void user_login(FILE *fp, int i)
{
	int choice = 0;
	char *user_name = (char *)calloc(9, sizeof(char));
	while (true)
	{
		printf("\nEnter your username(< 8 chars)\t:\t");
		scanf("%s", user_name);
		if (strlen(user_name) > 0 &&strlen(user_name) < 9)
			break;
	}
	user_name[8] = '\0';
	printf("\n\n**************  Welcome %s.  *************\n", user_name);
	home(user_name);
}

//method to initialize a file.
void init(FILE *fp)
{
	char  login_choice = '\0';
	int i = 0;
	for (i = 0; i < MAX_USERS; i++)
	{
		printf("\n__________________________WELCOME TO MINION DISCUSSION__________________________\n");
		printf("\nDo you want to login ? (y/n)\t:\t");
		login_choice = _getche();
		if (login_choice == 'y' || login_choice == 'Y')
		{
			user_login(fp, i);
		}
		else if (login_choice == 'n' || login_choice == 'N')
			break;
		else
			printf("\nEnter a valid choice.\n");
	}
}

//main function.
int main()
{
	FILE *fp = NULL;
	init(fp);
	_getch();
	return 0;
}