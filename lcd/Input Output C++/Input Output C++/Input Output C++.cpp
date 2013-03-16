//Clark support@matrixorbital.ca www.matrixorbital.ca/appnotes

#include <windows.h>
#include <iostream>		//For file/serial I/O
#include <dos.h>		//For the sleep function

using namespace std;

DWORD bytes = 0;
HANDLE port;

void ClearScreen()	//Clear screen command
{
	unsigned char command[] = {254, 88};
	WriteFile(port, &command, sizeof(command), &bytes, NULL);
}

void AutoTxKeyPressesOff()	//Turn off auto transmitted key presses to use polling mode
{
	unsigned char command[] = {254, 79};
	WriteFile(port, &command, sizeof(command), &bytes, NULL);
}

int PollKeyPress(char key[])	//Send the poll command and read a 1 byte response, report errors
{
	unsigned char command[] = {254, 38};
	if(WriteFile(port, &command, sizeof(command), &bytes, NULL))
		return(ReadFile(port, key, sizeof(key), &bytes, NULL));
	else
		return(false);
}

void ClearKeyBuffer()	//Clear the 10 key buffer to avoid overflow between reads
{
	unsigned char command[] = {254, 69};
	WriteFile(port, &command, sizeof(command), &bytes, NULL);
}

void OutputOn(char number)	//Turn a General Purpose Output on
{
	unsigned char command[] = {254, 87, number};
	WriteFile(port, &command, sizeof(command), &bytes, NULL);
}

void OutputOff(char number)	//Turn a General Purpose Output off
{
	unsigned char command[] = {254, 86, number};
	WriteFile(port, &command, sizeof(command), &bytes, NULL);
}

void main()
{	
	char menu_choice = 'c';		//User menu choice
	char key_pressed[1] = {0};	//Byte read from key buffer
	DCB port_attributes;		//Serial port attributes
	COMMTIMEOUTS timeouts={};	//Serial timeout attributes

	cout<<"Creating port..."<<endl;
	port = CreateFile("COM9", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//Create a file to write to and read from the serial port
	//Enable read and write functions, specify that no other functions can use this file, use no security, 
	//open only an existing port, select normal attributes, do not supply extended attributes
	if(port == INVALID_HANDLE_VALUE)
		cout<<"Error "<<GetLastError()<<" in creating port!"<<endl;

	cout<<"Setting port attributes..."<<endl;
	GetCommState(port, &port_attributes);	//Get and alter serial attributes for the display
	port_attributes.BaudRate = CBR_19200;
	port_attributes.ByteSize = 8;
	port_attributes.Parity = NOPARITY;
	port_attributes.StopBits = ONESTOPBIT;
	port_attributes.fRtsControl = RTS_CONTROL_DISABLE;
	SetCommState(port, &port_attributes);	//Set specific display attributes
	
	timeouts.ReadIntervalTimeout = 500;
	SetCommTimeouts(port, &timeouts);		//Set serial timeout to 500ms to avoid hanging if a read fails

	AutoTxKeyPressesOff();	//Turn auto transmission of key presses off to allow polling
	
	do
	{
		cout<<"Choose (r)ead key, (c)lear buffer, or (q)uit:";
		cin>>menu_choice;
		menu_choice = tolower(menu_choice);
		switch(menu_choice)
		{
		case 'r':
			PollKeyPress(key_pressed);	//Pull one key press from the buffer
			if(key_pressed[0]>0)		//Check that a key was pressed
			{
				cout<<"Key read is "<<key_pressed[0]<<endl;
				OutputOff(1);	//Turn outputs on to indicate more keys are to be read
				OutputOff(3);
				OutputOff(5);
				cout<<"No keys remain in the buffer!"<<endl;
			}
			else if(key_pressed[0]<0)	//Check if the MSb is 1 (signed char), indicating more keys in the buffer
			{
				cout<<"Key read is "<<(char)(key_pressed[0]+128)<<endl;	//Remove leading 1 from the character
				OutputOn(1);	//Turn outputs on to indicate more keys are to be read
				OutputOn(3);
				OutputOn(5);
				cout<<"More keys remain in the buffer!"<<endl;
			}
			else
			{
				cout<<"No key read!"<<endl;
			}
			break;
		case 'c':
			ClearKeyBuffer();	//Clear the key buffer
			cout<<"Key buffer cleared!"<<endl;
			break;
		case 'q':
			cout<<"Exiting program!"<<endl;
			CloseHandle(port);	//Close file/port before exiting
			Sleep(1000);	//Pause to allow message to be read
			break;
		default:
			break;
		}
	}while(menu_choice != 'q');
}