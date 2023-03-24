//
// file:			main_alloc_free_hook_test03.c
// path:			src/tests/main_alloc_free_hook_test03.c
// created on:		2023 Mar 17
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//


#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#define SleepmMsIntrp(_x)	SleepEx((DWORD)(_x),TRUE)
#define TestGetPid()		(int)GetCurrentProcessId()
#else
#include <unistd.h>
#define SleepmMsIntrp(_x)	usleep((useconds_t )(_x)*1000)
#define TestGetPid()		(int)getpid()
#endif



int main(void)
{
	void* pMem;

	printf("application will do malloc/free memory in ifinite loop. Pid is: %d\n", TestGetPid());

	while (1) {
		pMem = malloc(100);
		(void)pMem;
		free(pMem);
		SleepmMsIntrp(1000);
	}

	return 0;
}
