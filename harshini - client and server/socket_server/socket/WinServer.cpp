#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>

#define PAGETABLE_START_OFFSET 0
#define PAGETABLE_END_OFFSET 65472
#define STUDENT_RECORD_START_OFFSET 65536
#define BLOCK_SIZE 128
#define MAX_RECORD 8192

char directory_path[] = "D:/StudentDirectories/";

struct student
{
	unsigned int file_count;
	char name[13];
	char fname[10];
	char mname[10];
	char address[15];
	char marks[12];
	char files[8][8];
}temp_student;

//File information table to check the student by his roll number as index.
struct file_information_table
{
	unsigned int roll_number;
	unsigned int page_offset;
}ftable_rec;

unsigned int roll_number = 0;

DWORD WINAPI SocketHandler(void*);
void replyto_client(bool isrollnum, int *csock);

void set_strct_to_null()
{
	temp_student.file_count = 0;
	memset(temp_student.name, '\0', 13);
	memset(temp_student.fname, '\0', 10);
	memset(temp_student.mname, '\0', 10);
	memset(temp_student.address, '\0', 15);
	memset(temp_student.marks, '\0', 12);
	memset(temp_student.files, '\0', 64);
}

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
		string_reverse(str, i - 1);
	}
	else
	{
		*(str + i++) = '-';
		convert_int_to_str(str, (value * -1));
	}
}

void verify_roll_number()
{
	//Open the file system and seek to the corresponding filesystem page offset for the current record. 
	//file_information_table *ftable_rec = (file_information_table *)calloc(1, sizeof(file_information_table));
	int bytecount = 0;
	FILE *fp = fopen("filesystem.bin", "rb+");
	fseek(fp, 0, SEEK_SET);
	for (int i = 0; i < MAX_RECORD; i++)
	{
		fread(&ftable_rec, sizeof(file_information_table), 1, fp);
		//For new user.
		if (ftable_rec.roll_number == 0 && ftable_rec.page_offset == 0)
		{
			ftable_rec.roll_number = roll_number;
			ftable_rec.page_offset = (i * BLOCK_SIZE) + STUDENT_RECORD_START_OFFSET;
			fseek(fp, -sizeof(file_information_table), SEEK_CUR);
			fwrite(&ftable_rec, sizeof(file_information_table), 1, fp);
			fflush(fp);
			fclose(fp);

			//Create a directory with rollnumber as his directory name.
			char roll_num[10] = { '\0' }, temp_path[100] = { '\0' };
			convert_int_to_str(roll_num, roll_number);
			append_to_string(temp_path, directory_path);
			append_to_string(temp_path, roll_num);
			_mkdir(temp_path);
			break;
		}
		else if (roll_number == ftable_rec.roll_number)
		{
			fseek(fp, ftable_rec.page_offset, SEEK_SET);
			fread(&temp_student, sizeof(student), 1, fp);
			fflush(fp);
			fclose(fp);
			break;
		}
	}
}

void socket_server()
{
	//The port you want the server to listen on
	int host_port= 1101;
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
 	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )) 
	{
	    fprintf(stderr, "\n\tNo sock dll %d\n",WSAGetLastError());
		goto FINISH;
	}

	//Initialize sockets and set options
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1)
	{
		printf("\n\tError initializing socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) )
	{
		printf("\n\tError setting options %d\n", WSAGetLastError());
		free(p_int);
		goto FINISH;
	}
	free(p_int);

	//Bind and listen
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;
	
	/* if you get error in bind 	make sure nothing else is listening on that port */
	if( bind( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
	{
		fprintf(stderr,"\n\tError binding to socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	if(listen( hsock, 10) == -1 )
	{
		fprintf(stderr, "\n\tError listening %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	//Now lets do the actual server stuff.
	int* csock;
	sockaddr_in sadr;
	int addr_size = sizeof(SOCKADDR);
	
	while(true)
	{
		printf("\n\tWaiting for a connection.\n");
		csock = (int*)malloc(sizeof(int));
		
		if((*csock = accept( hsock, (SOCKADDR*)&sadr, &addr_size)) != INVALID_SOCKET )
		{
			printf("\n\tReceived connection from %s\n",inet_ntoa(sadr.sin_addr));
			CreateThread(0,0,&SocketHandler, (void*)csock , 0,0);
		}
		else
		{
			fprintf(stderr, "Error accepting %d\n",WSAGetLastError());
		}
	}

FINISH:
;
}

void process_input(bool isrollnum, int* csock) 
{
	if (isrollnum == true)
	{
		verify_roll_number();
		replyto_client(true, csock);
	}
	else
	{
		FILE *fp = fopen("filesystem.bin", "a");
		fseek(fp, 0, SEEK_END);
		fwrite(&temp_student, sizeof(student), 1, fp);
		fflush(fp);
		fclose(fp);
		replyto_client(false, csock);
	}

}

void replyto_client(bool isrollnum, int *csock)
{
	int bytecount;
	if (isrollnum == true)
	{
		if ((bytecount = send(*csock, (char*)&temp_student, sizeof(student), 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError sending data %d\n", WSAGetLastError());
			free(csock);
		}
	}
	else
	{
		char ack[] = "\n\tYour details are stored into the Database successfully.\n\t\t\t\t:)\n ";
		if ((bytecount = send(*csock, ack, strlen(ack), 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
			free(csock);
		}
	}
	fflush(stdout);
}

DWORD WINAPI SocketHandler(void* lp)
{
    int *csock = (int*)lp;
	int recv_byte_count;
	char filename[50] = { '\0' }, filepath[50] = { '\0' };
	
	//Set the roll number to Zero, before taking the input.
	roll_number = 0;
	set_strct_to_null();
	if((recv_byte_count = recv(*csock, (char *)&roll_number, sizeof(int), 0))==SOCKET_ERROR)
	{
		fprintf(stderr, "\n\tError receiving data %d\n", WSAGetLastError());
		free (csock);
		return 0;
	}
	fflush(stdin);
	printf("\n\tReceived bytes %d\n\t Received roll number : \"%d\" \n", recv_byte_count, roll_number);
	process_input(true, csock);
	
	//Wait if there are any file transfers.
	if ((recv_byte_count = recv(*csock, filename, 50, 0)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
		free(csock);
		return 0;
	}
	if (filename[0] != '\0')
	{
		if ((recv_byte_count = recv(*csock, filepath, 50, 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
			free(csock);
			return 0;
		}
		if (filepath[0] != '\0')
		{
			strcat(filepath, filename);
			FILE *srcfp = fopen(filepath, "r");
		}
	}

	//Receive the structure containing the student record and write it back to database.
	if ((recv_byte_count = recv(*csock, (char *)&temp_student, sizeof(student), 0)) == SOCKET_ERROR)
	{
		fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
		free(csock);
		return 0;
	}
	fflush(stdin);
	printf("\n\tReceived bytes %d \n\tReceived student record {%13s, %10s, %10s, %15, %12s, %8s, %8s,... } \n", recv_byte_count, 
		temp_student.name, temp_student.fname, temp_student.mname, temp_student.address, temp_student.marks, temp_student.files[0], temp_student.files[1]);
	process_input(false, csock);
    return 0;
}