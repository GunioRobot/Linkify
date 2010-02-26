#include "stdafx.h"
#include "pdacellid.h"
#include "ril.h"








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



PDACELLID_API long fnGetCell(LPTSTR outData)
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

	buf[bufpos] = 0;

	mbstowcs(outData,buf,bufpos+1);
	

	
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

///////////////////////////////////////////////////////////////////


void CALLBACK NotifyCallback(DWORD dwCode, const void *lpData,
										   DWORD cbData, DWORD dwParam)
{


	switch (dwCode & RIL_NCLASS_ALL) {
	case RIL_NCLASS_MESSAGE:
		{

			switch (dwCode & 0xff) {
			case RIL_NOTIFY_MESSAGE:
				{
					RILMESSAGE *prmMsg = (RILMESSAGE *)lpData;

//					DisplayMessage(prm);
					switch(prmMsg->dwType) {
						case RIL_MSG_IN_DELIVER:
							{

								break;
							}
						default:
								break;
					}

					break;
				}
			case RIL_NOTIFY_STATUSMESSAGE:
				{
					RILMESSAGE *prm = (RILMESSAGE *)lpData;

					break;
				}
			}
			break;
		}
	}

}

void CALLBACK ResultCallback(DWORD dwCode, HRESULT hrCmdID,		const void *lpData,
										   DWORD cbData, DWORD dwParam)
{
}






PDACELLID_API long fnGetCell2(LPTSTR outData)
{
HANDLE hCom;
HRIL hRil;
char * xpos;
char rsltstr[5];
DWORD CellId;
DWORD erc, erc2;
int bufpos;
DCB dcb;
COMMTIMEOUTS to;
DWORD nWritten;
DWORD nRead;
char outbuf[20], buf[256];


	
	
//	BYTE comdevcmd[2]= {0x84, 0x00};

	DWORD dwNotificationClasses = 0xFF0000;


	DWORD g_dwParam = 0x55AA55AA;


	erc = RIL_Initialize(1, ResultCallback, NotifyCallback,
					   dwNotificationClasses, g_dwParam, &hRil);

	if (erc != 0)
		return erc;


	erc = RIL_GetSerialPortHandle(hRil,&hCom);


	if (erc != 0)
	{
		erc2 = RIL_Deinitialize(hRil);

		return erc;
	}


	if (hCom==NULL || hCom==INVALID_HANDLE_VALUE)
	{
		hCom= NULL;
		erc = RIL_Deinitialize(hRil);
		return -1;
	}


	if (!GetCommState(hCom, &dcb))
	{
		erc = RIL_Deinitialize(hRil);
		return -2;
	}

	dcb.BaudRate= CBR_115200;
	dcb.ByteSize= 8;
	dcb.fParity= false;
	dcb.StopBits= ONESTOPBIT;

	if (!SetCommState(hCom, &dcb))
	{
		erc = RIL_Deinitialize(hRil);
		return -3;
	}

	if (!EscapeCommFunction(hCom, SETDTR))
	{
		erc = RIL_Deinitialize(hRil);
		return -4;
	}
//	if (!EscapeCommFunction(hCom, SETRTS))
//	{
//		erc = RIL_Deinitialize(hRil);
//		return -5;
//	}

	if (!GetCommTimeouts(hCom, &to))
	{
		erc = RIL_Deinitialize(hRil);
		return -6;
	}
	to.ReadIntervalTimeout= MAXDWORD;
	to.ReadTotalTimeoutConstant= 1000;
	to.ReadTotalTimeoutMultiplier= MAXDWORD;
	to.WriteTotalTimeoutConstant= 2000;
	to.WriteTotalTimeoutMultiplier= 0;
	if (!SetCommTimeouts(hCom, &to))
	{
		erc = RIL_Deinitialize(hRil);
		return -7;
	}

	if (!SetCommMask(hCom, EV_RXCHAR+EV_ERR+EV_BREAK))
	{
		erc = RIL_Deinitialize(hRil);
		return -8;
	}



	bufpos = 0;


	strcpy(outbuf,"AT+creg=2\r");

	if (!WriteFile(hCom, outbuf, 10, &nWritten, NULL))	
	{
		erc = RIL_Deinitialize(hRil);
		return -10;
	}
	
	if (nWritten != 10)
	{
		erc = RIL_Deinitialize(hRil);
		return -11;
	}


	while(1)
	{
		if (!ReadFile(hCom, buf+bufpos, 256 - bufpos, &nRead, NULL))
		{
			erc = RIL_Deinitialize(hRil);
			return -13;
		}

		if (nRead == 0)
			break;


		bufpos += nRead;
		
		
		if (bufpos >= 256)
			break;


	}

	if (bufpos == 0)
	{
			erc = RIL_Deinitialize(hRil);
			return -130;
	}


	strcpy(outbuf,"AT+creg?\r");

	if (!WriteFile(hCom, outbuf, 9, &nWritten, NULL))	
	{
		erc = RIL_Deinitialize(hRil);
		return -14;
	}
	
	if (nWritten != 9)
	{
		erc = RIL_Deinitialize(hRil);
		return -15;
	}



	bufpos = 0;

	while(1)
	{
		if (!ReadFile(hCom, buf+bufpos, 256 - bufpos, &nRead, NULL))
		{
			erc = RIL_Deinitialize(hRil);
			return -17;
		}

		if (nRead == 0)
			break;

		bufpos += nRead;

		if (bufpos >= 256)
			break;
	}

	if (bufpos == 0)
	{
			erc = RIL_Deinitialize(hRil);
			return -131;
	}
	

	buf[bufpos] = 0;

	mbstowcs(outData,buf,bufpos+1);
	
	
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

	
	
	
	
	
	
	erc = RIL_Deinitialize(hRil);
	
	return CellId;

}
