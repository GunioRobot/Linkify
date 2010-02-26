// pdacellid.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "pdacellid.h"








BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}



PDACELLID_API long fnGetCell()
{
HANDLE hCom;
char * xpos;
char rsltstr[5];
DWORD CellId;
int bufpos;
DCB dcb;
COMMTIMEOUTS to;
DWORD nWritten;
DWORD event;
DWORD nRead;
char outbuf[20], buf[256];


	
	
	BYTE comdevcmd[2]= {0x84, 0x00};


	hCom= CreateFile(L"COM2:",GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,0,0); 
	if (hCom==NULL || hCom==INVALID_HANDLE_VALUE)
	{
		hCom= NULL;
		return -1;
	}

	if (!GetCommState(hCom, &dcb))
	{
		return -2;
	}

	dcb.BaudRate= CBR_115200;
	dcb.ByteSize= 8;
	dcb.fParity= false;
	dcb.StopBits= ONESTOPBIT;

	if (!SetCommState(hCom, &dcb))
	{
		return -3;
	}

	if (!EscapeCommFunction(hCom, SETDTR))
	{
		return -4;
	}
	if (!EscapeCommFunction(hCom, SETRTS))
	{
		return -5;
	}

	if (!GetCommTimeouts(hCom, &to))
	{
		return -6;
	}
	to.ReadIntervalTimeout= 0;
	to.ReadTotalTimeoutConstant= 200;
	to.ReadTotalTimeoutMultiplier= 0;
	to.WriteTotalTimeoutConstant= 20000;
	to.WriteTotalTimeoutMultiplier= 0;
	if (!SetCommTimeouts(hCom, &to))
	{
		return -7;
	}

	if (!SetCommMask(hCom, EV_RXCHAR))
	{
		return -8;
	}



	if (!DeviceIoControl (hCom,0xAAAA5679L, comdevcmd, sizeof(comdevcmd),0,0,0,0))
	{
		return -9;
	}


	bufpos = 0;


	strcpy(outbuf,"AT+creg=2\r");

	if (!WriteFile(hCom, outbuf, 10, &nWritten, NULL))	
	{
		return -10;
	}
	
	if (nWritten != 10)
	{
		return -11;
	}

	if (!WaitCommEvent(hCom, &event, NULL))
	{
		return -12;
	}

	while(1)
	{
		if (!ReadFile(hCom, buf+bufpos, 256 - bufpos, &nRead, NULL))
		{
			return -13;
		}

		if (nRead == 0)
			break;


		bufpos += nRead;
		
		
		if (bufpos >= 256)
			break;


	}



	strcpy(outbuf,"AT+creg?\r");

	if (!WriteFile(hCom, outbuf, 9, &nWritten, NULL))	
	{
		return -14;
	}
	
	if (nWritten != 9)
	{
		return -15;
	}

	if (!WaitCommEvent(hCom, &event, NULL))
	{
		return -16;
	}

	while(1)
	{
		if (!ReadFile(hCom, buf+bufpos, 256 - bufpos, &nRead, NULL))
		{
			return -17;
		}

		if (nRead == 0)
			break;

		bufpos += nRead;

		if (bufpos >= 256)
			break;
	}

	

	
	if ((xpos = strstr(buf,"CREG")) == NULL)
	{
		CellId = -19;
	}
	else
	{
		memcpy(rsltstr,xpos+18,4);
		rsltstr[4] = 0;
		if (sscanf(rsltstr,"%X",&CellId) != 1)
		{
			CellId = -20;
		}
	}
			

	if (hCom!=NULL)
	{
		CloseHandle(hCom);
		hCom= NULL;
	}

	
	
	
	
	
	
	
	return CellId;

}
