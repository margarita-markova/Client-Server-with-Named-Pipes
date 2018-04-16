#include <iostream>
#include <windows.h>
#include <string>
#include <math.h>
#define MAX_COUNT 1
#define BUFSIZE 3
using namespace std;


int main(int argc, char* argv[])
{
	LPCSTR lName = "semaphore";
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	if (argc >= 2)
	{
		std::cout << "THIS IS THE CLIENT PROCESS" << endl;

		HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, lName);

		while (1)
		{
			WaitForSingleObject(hSemaphore, INFINITE);
			cout << endl;
			std::cout << "Server is ready to start transer the data" << endl;

			hPipe = CreateFile(
				lpszPipename,   // pipe name 
				GENERIC_READ,  // read access 
				0,              // no sharing 
				NULL,           // default security attributes
				OPEN_EXISTING,  // opens existing pipe 
				0,              // default attributes 
				NULL);          // no template file 

			// Break if the pipe handle is valid. 
			if (hPipe == INVALID_HANDLE_VALUE)
				cout << "CreateFile error";

			// Exit if an error other than ERROR_PIPE_BUSY occurs. 
			if (GetLastError() == ERROR_PIPE_BUSY)
			{
				std::cout << "Could not open pipe. GLE= " << GetLastError() << endl;
			}

			BOOL bSuccess = FALSE;
			char cInput[BUFSIZE] = { 0 };

			cout << "Received message: " << endl;

			while (1)
			{
				do
				{
					// Read from the pipe. 
					DWORD dwRead = 0;

					bSuccess = ReadFile(
						hPipe,    // pipe handle 
						cInput,    // buffer to receive reply 
						BUFSIZE*sizeof(char),  // size of buffer 
						&dwRead,  // number of bytes read 
						NULL);    // not overlapped 

					if (!bSuccess && GetLastError() != ERROR_MORE_DATA)
					{
						cout << "Error Readfile";
						break;
					}

				} while (!bSuccess);  // repeat loop if ERROR_MORE_DATA

					int i=0;
					
						for (i = 0; cInput[i] != '\0'; i++)
						{
							cout << cInput[i];
						}

						if (i < (BUFSIZE - 1))
						{
							break;
						}


					ReleaseSemaphore(hSemaphore, 1, NULL);
			}
			//notice server for transfer finished
			ReleaseSemaphore(hSemaphore, 1, NULL);
		}
	}
	
	else
	{
		int c = 3;
		char idcommand[10];
		sprintf(idcommand + 1, "%d", c);
		idcommand[0] = ' ';

		cout << "THIS IS THE SERVER PROCESS" << endl;
		cout << "Exit: write e" << endl << endl;

		string exit = "e";

		//creating semaphores
		HANDLE hSemaphore = CreateSemaphore(
			NULL, //default security attributes
			0, //initial count (signaled state)
			MAX_COUNT,
			lName); //named semaphore 

		//create child client process
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcess(argv[0], idcommand, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		{
			cout << "Create process failed" << endl;
			system("pause");
		}

		while (1)
		{
			//input string
			string sInput;
			string empty = "";
			do{
				cout << "Input string:" << endl;
				getline(cin, sInput);
			} while (strcmp(sInput.c_str(), empty.c_str()) == 0);

			if (strcmp(sInput.c_str(), exit.c_str()) == 0)
				break;

			//create pipe
			//start to transfer data
			hPipe = CreateNamedPipe(
				lpszPipename,             // pipe name 
				PIPE_ACCESS_OUTBOUND,       // server to client access only
				PIPE_TYPE_MESSAGE |       // message type pipe 
				PIPE_READMODE_MESSAGE |   // message-read mode 
				PIPE_WAIT,                // blocking mode 
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				NULL,                  // output buffer size 
				BUFSIZE,                  // input buffer size 
				0,                        // client time-out 
				NULL);                    // default security attribute 

			if (hPipe == INVALID_HANDLE_VALUE)
			{
				cout << "CreateNamedPipe failed " << GetLastError() << endl;
			}

			ReleaseSemaphore(hSemaphore, 1, NULL); //notice to client for starting transfer

			int n = sInput.length();
			int iter = n / (BUFSIZE-1); //нужное количество повторений
			int curIter = 0; //текущее количество повторений (заходов в if (fConnected))

			while (1)
			{
				bool fConnected = ConnectNamedPipe(hPipe, NULL) ?
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

				if (curIter <= iter)
				{
					if (fConnected)
					{
						DWORD cbWritten = 0;

						char cInput[BUFSIZE] = { 0 };

							strncpy(cInput, sInput.c_str(), BUFSIZE - 1);
							sInput.erase(0, BUFSIZE - 1);
					
						bool fSuccess = WriteFile(
							hPipe,        // handle to pipe 
							cInput,     // buffer to write from 
							BUFSIZE, // number of bytes to write 
							&cbWritten,   // number of bytes written 
							NULL);        // not overlapped I/O 

						if (fSuccess)
						{
							curIter++;
							if (iter == curIter)
							{
								WaitForSingleObject(hSemaphore, INFINITE);
								cout << "Client process finished receiving data" << endl << endl;
							}
							else
							WaitForSingleObject(hSemaphore, INFINITE);
						}
					}
				}
				else
					break;
			}
		}
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hProcess);
		CloseHandle(hSemaphore);
		CloseHandle(hPipe);
		return 0;
	}
}

