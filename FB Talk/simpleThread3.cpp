#include <Windows.h>
#include <stdio.h>

DWORD WINAPI mythread(__in LPVOID lpParameter)
{
	int num = *((int *)lpParameter);
	printf("\n\nThread number %d\n", num);
	printf("\n\nThread inside %d \n", GetCurrentThreadId());
	
	return 0;
}

int main(int argc, char* argv[])
{
	int num = 1;
	HANDLE myhandle1, myhandle2;
	DWORD mythreadid1, mythreadid2, time = 1000;
	myhandle1 = CreateThread(0, 0, mythread, &num, 0, &mythreadid1);
	printf("\nThread after 1 %d \n", mythreadid1);
	num = 2;
	Sleep(time);
	myhandle2 = CreateThread(0, 0, mythread, &num, 0, &mythreadid2);
	printf("\nThread after 2 %d \n", mythreadid2);
	getchar();
	return 0;
}