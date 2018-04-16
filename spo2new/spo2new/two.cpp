#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <stack>


void PrintInfo(char* id)
{
	const char* message = "My number is ";
	for (int i = 0; i < strlen(message); i++)
	{
		std::cout << message[i];
		Sleep(75);
	}
	for (int i = 0; i < strlen(id); i++)
	{
		std::cout << id[i] << std::endl;
		Sleep(75);
	}
	Sleep(100);
}

int main(int argc, char* argv[])
{
	char cPrintEventName[] = "hPrintEvent";
	if (argc >= 2)
	{
		HANDLE hPrintEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, cPrintEventName);
		char* id = argv[1];

		while (1)
		{
			WaitForSingleObject(hPrintEvent, INFINITE); //1
			PrintInfo(id);
			SetEvent(hPrintEvent);
		}
	}

	else
	{
		std::cout << "Choices: " << std::endl;
		std::cout << "* write '+', if you want to add new process" << std::endl;
		std::cout << "* write '-', if you want to delete last process" << std::endl;
		std::cout << "* write 'q', if you want to quit" << std::endl;

		std::stack<HANDLE> Processes;
		HANDLE hPrintEvent = CreateEvent(NULL, FALSE, TRUE, cPrintEventName);

		while (1)
		{
			WaitForSingleObject(hPrintEvent, INFINITE); //2
			rewind(stdin); 

			char choice;
			choice = getchar();
			switch (choice)
			{
			case '+':
				char idcommand[10];
				sprintf(idcommand + 1, "%d", Processes.size()+1);
				idcommand[0] = ' ';

				STARTUPINFO si;
				PROCESS_INFORMATION pi;

				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));

				if (!CreateProcess(argv[0], idcommand, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
				{
					std::cout << "CreateProcess error\n" << std::endl;
					system("pause");
					return 0;
				}
				Processes.push(pi.hProcess);
				std::cout << "\n Count of processes: " << Processes.size() << std::endl;

				break;
			case'-':
				if (!Processes.empty())
				{

					TerminateProcess(Processes.top(), 0);
					CloseHandle(Processes.top());
					Processes.pop();
					std::cout << "\n Count of processes: " << Processes.size() << std::endl;
				}
				else
				{
					std::cout << "\n Stack is empty " << std::endl;
				}
				break;
			case'q':
				while (!Processes.empty())
				{

					while (!Processes.empty())
					{
						SetEvent(hPrintEvent);
						TerminateProcess(Processes.top(), 0);
						CloseHandle(Processes.top());
						Processes.pop();
						std::cout << "\n Count of processes: " << Processes.size() << std::endl;
					}
					CloseHandle(hPrintEvent);
					return 0;
				}
				break;
			default:
				break;
			}
			SetEvent(hPrintEvent);
			Sleep(400);
		}
	}
	return 0;
}