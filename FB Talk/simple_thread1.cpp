#include <windows.h>
#include <iostream>

DWORD WINAPI myThread(LPVOID lpParameter)
{
	unsigned int& myCounter = *((unsigned int*)lpParameter);
	while (myCounter < 0xFFFFFFFF) ++myCounter;
	return 0;
}

int main(int argc, char* argv[])
{
	unsigned int myCounter = 0;
	DWORD myThreadID;
	HANDLE myHandle = CreateThread(0, 0, myThread, &myCounter, 0, &myThreadID);
	char myChar = ' ';
	while (myChar != 'q') {
		printf("\n---- %d-----\n", myCounter);
		myChar = getchar();
	}

	CloseHandle(myHandle);
	return 0;
}