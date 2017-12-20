#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>

//Student structure of 128bytes.
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

//Function to post the new user details.
void post_new_user_details()
{
	char buffer[20];
	int buffer_len = 20;
	//Take user name and copy it student_name stucture.
	do
	{
		printf("\n\tEnter your name (max 13 chars) : ");
		scanf("%s", buffer);
	} while (strlen(buffer) > 13);
	buffer[13] = '\0';
	strcpy(temp_student.name, buffer);
	memset(buffer, '\0', buffer_len);
	//Take user name and copy it student_father_name stucture.
	do
	{
		printf("\n\tEnter your father's name (max 10 chars) : ");
		scanf("%s", buffer);
	} while (strlen(buffer) > 10);
	buffer[10] = '\0';
	strcpy(temp_student.fname, buffer);
	memset(buffer, '\0', buffer_len);
	//Take user name and copy it student_mothers_name stucture.
	do
	{
		printf("\n\tEnter your mother's name (max 10 chars) : ");
		scanf("%s", buffer);
	} while (strlen(buffer) > 10);
	buffer[10] = '\0';
	strcpy(temp_student.mname, buffer);

	//Take user name and copy it student_address stucture.
	do
	{
		printf("\n\tEnter your residential address (max 15 chars) : ");
		scanf("%s", buffer);
	} while (strlen(buffer) > 15);
	buffer[15] = '\0';
	strcpy(temp_student.address, buffer);
}

//Function to get the personal details of the user.
void get_personal_details()
{
	printf("\n\tPersonal details : \n\tName : %-13s\n\tFather's Name : %-10s", temp_student.name, temp_student.fname);
	printf("\n\tMother's Name : %-10s\n\tResidential Address : %-15s\n", temp_student.mname, temp_student.address);
}

//Function to get marks of the user.
void get_marks()
{
	int year = 0, marks_offset = 0;
	printf("\n\tEnter year of examination (1 or 2) : ");
	scanf("%d", &year);
	marks_offset = (year - 1) * 6;
	if (temp_student.marks[marks_offset] != '\0')
	{
		printf("\n\tMarks of year - %d - ", year);
		for (int subject = 0; subject < 6; subject++)
			printf("\n\t\tSubject - %d  : %2d", (int)subject + 1, temp_student.marks[marks_offset + subject]);
	}
	else
		printf("\n Marks for year - %d are not yet updated.\n", year);
}

//Function to upload the user files to server.
void get_fileuploads()
{
	char filename[30] = { '\0' };
	char choice = '\0';
	printf("\n\tEnter the name of the file : ");
	scanf("%s", filename);

	//search the student structure if file exists.
	for (int i = 0; i < temp_student.file_count; i++)
	{
		if (strcmp(temp_student.files[i], filename) == 0)
		{
			printf("\n\tFile Found.");
			do
			{
				printf("\n\tDo you want to open the file now ? (y/n) : ");
				choice = getche();
			} while (choice != 'y' && choice != 'n' && choice != 'Y' && choice != 'N');
			//receive the file from server to the local directory.
			if (choice == 'y' || choice == 'Y')
			{

			}
			break;
		}
	}
}

//Function to post marks to the server.
void post_marks()
{
	int year = 0, marks_offset = 0, temp_marks = 0;
	printf("\n\tEnter year of examination (1 or 2) : ");
	scanf("%d", &year);
	marks_offset = (year - 1) * 6;
	
	printf("\n\tEnter marks of year - %d (max marks 100) - ", year);
	for (int subject = 0; subject < 6; subject++)
	{
		printf("\n\t\tSubject - %d  : ", subject + 1);
		scanf("%d", &temp_marks);
		temp_student.marks[marks_offset + subject] = (char)temp_marks;
	}
}

//Function to post files to the server.
void post_fileuploads(int hsock)
{
	if (temp_student.file_count < 8)
	{
		char filename[8] = { '\0' };
		char filepath[50] = { '\0' }, ack[20];
		char choice = '\0';
		int bytecount;
		printf("\n\tEnter the name of the file (max 8 chars): ");
		scanf("%s", filename);
		printf("\n\tEnter the file path : ");
		scanf("%s", filepath);
		//Send the filename to server
		if ((bytecount = send(hsock, filepath, strlen(filepath), 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError sending data %d\n", WSAGetLastError());
			return;
		}
		if ((bytecount = recv(hsock, ack, sizeof(student), 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError receiving data %d\n", WSAGetLastError());
			return;
		}
		printf("\n%s", ack);
		//Send the filepath to the server.
		if ((bytecount = send(hsock, filepath, strlen(filepath), 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError sending data %d\n", WSAGetLastError());
			return;
		}
		if ((bytecount = recv(hsock, ack, sizeof(student), 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError receiving data %d\n", WSAGetLastError());
			return;
		}
		printf("\n%s", ack);
	}
	else
		printf("\n\tYour space for files is exhausted, delete a previous file to add new file.");
}

//Function to choose a option for a client.
int get_choice()
{
	int choice = 0;
	printf("\n\n\tOPTIONS : \n\t1. View your personal details.\n\t2. View your marks.\n\t3. Post your marks\n\t4. View your file uploads\n\t5. Add new files\n\t6.EXIT");
	do
	{
		printf("\n\tEnter your choice (1-6) : ");
		scanf("%d", &choice);

	} while (choice < 1 || choice > 7);
	return choice;
}

//Function to get a socket connection from the client.
int getsocket()
{
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1)
	{
		printf("\n\tError initializing socket %d\n",WSAGetLastError());
		return -1;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) )
	{
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		return -1;
	}
	free(p_int);

	return hsock;
}

//Function to set the entire strucure to null.
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

//Function to perform main functions of the client.
void socket_client()
{

	//The port and address you want to connect to
	int host_port= 1101;
	char* host_name="127.0.0.1";

	//Initialize socket support WINDOWS ONLY!
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );
 	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 ||HIBYTE( wsaData.wVersion ) != 2 )) 
	{
	    fprintf(stderr, "\n\tCould not find sock dll %d\n",WSAGetLastError());
		goto FINISH;
	}

	//Initialize sockets and set any options
	//Connect to the server
	struct sockaddr_in my_addr;

	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = inet_addr(host_name);

	//if( connect( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR ){
	//	fprintf(stderr, "Error connecting socket %d\n", WSAGetLastError());
	//	goto FINISH;
	//}

	//create a local directory to current client.
	_mkdir("localDirectory");

	//Now lets do the client related stuff
	char buffer[128];
	int buffer_len = 128, bytecount, c, roll_number = 0, choice = 0;

	while (true)
	{
		int hsock = getsocket();
		//add error checking on hsock...
		if (connect(hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError connecting socket %d\n", WSAGetLastError());
			goto FINISH;
		}

		//set the global stucrture to null.
		set_strct_to_null();

		//Get roll number from the client.
		printf("\n-------------------------------------------------------------------------------\n");
		printf("\n\tEnter your roll number : ");
		scanf("%u", &roll_number);

		//Send the roll number of the client as a typecasted char *.
		if ((bytecount = send(hsock, (char *)&roll_number, sizeof(roll_number), 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError sending data %d\n", WSAGetLastError());
			goto FINISH;
		}
		//Recieve the server response after verifying the student roll number existence in the DB.
		if ((bytecount = recv(hsock, (char*)&temp_student, buffer_len, 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError receiving data %d\n", WSAGetLastError());
			goto FINISH;
		}
		//if the user is new, take details and post it to server.
		if (temp_student.name[0] == '\0')
		{
			post_new_user_details();
			printf("\n------------------------------------------------------------------------------\n");
			printf("\n\t\tWelcome New user, %s.\n\tYour details have been successfully saved.\n", temp_student.name);
		}
		
		//Give options to the user and take his choice.
		while (choice != 6)
		{
			printf("\n------------------------------------------------------------------------------\n");
			choice = get_choice();
			switch (choice)
			{
			case 1:
				get_personal_details();
				break;
			case 2:
				get_marks();
				break;
			case 3:
				post_marks();
				break;
			case 4:
				get_fileuploads();
				break;
			case 5:
				post_fileuploads(hsock);
				break;
			case 6:
				break;
			}
		}
		//Inform the server that the client will not upload files anymore.
		if ((bytecount = send(hsock, "0", 1, 0)) == SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError sending data %d\n", WSAGetLastError());
			goto FINISH;
		}

		if( (bytecount=send(hsock, (char *)&temp_student, sizeof(student),0))==SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError sending data %d\n", WSAGetLastError());
			goto FINISH;
		}
		printf("\n\tSent bytes : %d\n\n\tSent Message : %s\n", bytecount, buffer);
		printf("\n------------------------------------------------------------------------------\n");
		if((bytecount = recv(hsock, (char *)&temp_student, sizeof(student), 0))==SOCKET_ERROR)
		{
			fprintf(stderr, "\n\tError receiving data %d\n", WSAGetLastError());
			goto FINISH;
		}
		printf("\n\tRecieved bytes %d\nReceived string \"%s\"\n", bytecount, buffer);
		printf("\n-----------------------------------------------------------------------------\n");

		closesocket(hsock);
	}

	//closesocket(hsock);
FINISH:
;
}