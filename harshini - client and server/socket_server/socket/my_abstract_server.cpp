#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>
#define PAGETABLE _START 0
#define PAGETABLE_END 1023
#define STUDENT_RECORD_START 1024
#define BLOCK_SIZE 128

struct student
{

};


void socket_server();
void process_input(char *recvbuf, int recv_buf_cnt, int* csock);
void replyto_client(char *buf, int *csock);
DWORD WINAPI SocketHandler(void* lp);

void make_entry()


int hsock;

int verify_roll_number(unsigned int roll_number)
{
	
}

void get_new_student_fileuploads()
{
	int choice = 0;
	char ask_fileuploads[] = "\n\tHow many files do you want to upload ?  ";
	char ask_filepath[] = "\n\tEnter your filepath : ";
}

void init()
{
	unsigned int roll_number = 0, page_offset = 0, choice = 0;
	char ask_roll_number[] = "\n\tEnter your roll number :";

	//once the client is connected to server.
	//ask the roll number of the client.
	//put reply_to_client() send ask_roll_number as a replybuff to client.

	page_offset = verify_roll_number(roll_number);

}

int main()
{
	socket_server();
	init();
	return 0;
}