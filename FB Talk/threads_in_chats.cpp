
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include<conio.h>
#include<stdlib.h>

static int userid;
static char user_input_file1[15] = "userinput1.txt";
static char user_input_file2[15] = "userinput2.txt";
static char user_output_file1[16] = "useroutput1.txt";
static char user_output_file2[16] = "useroutput2.txt";
static char network_file[] = "network_datafile.bin";

struct message
{
	int sender_id;
	int receiver_id;
	char buffer[100];
};

void copy(char *dest, char *src)
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
		string_reverse(str, i - 1);
	}
	else
	{
		*(str + i++) = '-';
		convert_int_to_str(str, (value * -1));
	}
}

int get_choice()
{
	int choice = 0;
	printf("\n\t\tOPTIONS\n\t\t^^^^^^^^\n\n\t\t1. Read a message\n\t\t2. Write a message\n\t\t3. Exit");
	while (true)
	{
		printf("\nEnter a choice (1 - 3) :");
		scanf("%d", &choice);
		if (choice > 0 && choice < 4)
			return choice;
	}
}

DWORD WINAPI sender(__in LPVOID lpParameter)
{
	int prev_offset = 0;
	FILE *fp1 = fopen(user_input_file1, "r");
	FILE *fp2 = fopen(network_file, "a");
	char temp;
	while (true)
	{
		fseek(fp2, 0, SEEK_END);
		fseek(fp1, 0, SEEK_SET);
		temp = fgetc(fp1);
		if (temp != EOF)
		{
			fputc(temp, fp2);
			prev_offset = ftell(fp1);
		}
		else
			Sleep(1000);
	}
	return 0;
}

DWORD WINAPI receiver(__in LPVOID lpParameter)
{
	int prev_offset = 0;
	FILE *fp1 = fopen(user_output_file2, "a");
	FILE *fp2 = fopen(network_file, "r");
	char temp;
	Sleep(1000);
	while (true)
	{
		fseek(fp1, 0, SEEK_END);
		fseek(fp2, prev_offset, SEEK_SET);
		temp = fgetc(fp2);
		if (temp != EOF)
		{
			fputc(temp, fp1);
			prev_offset = ftell(fp2);
		}
		else
			Sleep(1000);
	}
	return 0;
}

//void generate_filename(char *in_prefix, char *out_prefix, char *ext)
//{
//	char id[2];
//	convert_int_to_str(id, userid);
//	id[1] = '\0';
//	copy(user_input_file, in_prefix);
//	copy(user_input_file, id);
//	copy(user_input_file, ext);
//	copy(user_output_file, out_prefix);
//	copy(user_output_file, id);
//	copy(user_output_file, ext);
//}

void init()
{
	DWORD receiver_threadid = 2, sender_threadid = 1;
	HANDLE receive, send;
	//char input_file_prefix[10] = "userinput", output_file_prefix[11] = "useroutput", file_ext[5] = ".txt";
	//int choice = 0, read_count = 0, write_count = 0;
	printf("\nEnter your User - ID : ");
	scanf("%d", &userid);

	//generate input and output filename.
	//generate_filename(input_file_prefix, output_file_prefix, file_ext);
	//while (choice != 3)
	//{
	//	message *m1 = (message *)calloc(1, sizeof(char));
	//	choice = get_choice();
	//	if (choice == 1)
	//	{
	//		if (write_count == 0)
	//			printf("\nNo messages to write.\n");
	//		else
	//		{
	//			FILE *fp = fopen(user_output_file, "r");
	//			fseek(fp, sizeof(message) * read_count, SEEK_SET);
	//			fread(&m1, sizeof(message), 1, fp);
	//			fseek(fp, (sizeof(message) * read_count) + 20, SEEK_SET);
	//			read_count++;
	//			for (int i = 0; i < 100; i++)
	//			{
	//				m1->buffer[i] = getc(fp);
	//				if (m1->buffer[i] == '\n')
	//				{
	//					m1->buffer[i + 1] = '\0';
	//					break;
	//				}
	//			}
	//			printf("\n%-10s : %-100s", m1->sender_id, m1->buffer);
	//		}
	//	}
	//	else if (choice == 2)
	//	{
	//		FILE *fp = fopen(user_input_file, "a");
	//		printf("\nEnter Destination userid : ");
	//		scanf("%d", m1->receiver_id);
	//		printf("\nEnter message : ");
	//		for (int i = 0; i < 100; i++)
	//		{
	//			m1->buffer[i] = getc(stdin);
	//			if (m1->buffer[i] == '\n')
	//			{
	//				m1->buffer[i + 1] = '\0';
	//				break;
	//			}
	//		}
	//		write_count++;
	//		m1->sender_id = userid;
	//		fflush(stdin);
	//		fwrite(&m1, sizeof(m1), 1, fp);
	//		fflush(fp);
	//	}
	//}
	send = CreateThread(0, 0, sender, 0, 0, &sender_threadid);
	receive = CreateThread(0, 0, receiver, 0, 0, &receiver_threadid);

	//CloseHandle(send);
	//CloseHandle(receive);
}

int main()
{
	init();
	_getch();
	return 0;
}