//Clark support@matrixorbital.ca www.matrixorbital.ca/appnotes

#include <windows.h>
#include <iostream>	//For file/serial I/O

using namespace std;

void main()
{
	DCB port_attributes;		//Sturcture for serial port attributes
	DWORD bytes_written = 0;	//Counter for number of bytes written to the serial port/file
	unsigned char display_on[] = {0xFE, 0x42, 0x00};	//Command bytes to turn display on
	unsigned char clear_screen[] = {0xFE, 0x58};		//Command bytes to clear screen
	unsigned char message[] = "Hello World!";			//Message string
	
	cout<<"Creating port..."<<endl;
	HANDLE my_port = CreateFile("COM9", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//Create a file to write to and read from the serial port
	//Enable read and write functions, specify that no other functions can use this file, use no security, 
	//open only an existing port, select normal attributes, do not supply extended attributes
	if(my_port == INVALID_HANDLE_VALUE)
	{
		cout<<"Error "<<GetLastError()<<" in creating port!"<<endl;
	}

	cout<<"Setting port attributes..."<<endl;
	GetCommState(my_port, &port_attributes);	//Get and alter serial attributes for the display
	port_attributes.BaudRate = CBR_19200;
	port_attributes.ByteSize = 8;
	port_attributes.Parity = NOPARITY;
	port_attributes.StopBits = ONESTOPBIT;
	SetCommState(my_port, &port_attributes);	//Set specific display attributes

	cout<<"Writing data..."<<endl;

	if(!WriteFile(my_port, &display_on, sizeof(display_on), &bytes_written, NULL))	//Turn display on
	{
		cout<<"Error writing command to display!"<<endl;	//Check for errors in writing, call GetLastError with the int response for more info
	}

	if(!WriteFile(my_port, &clear_screen, sizeof(clear_screen), &bytes_written, NULL))	//Clear screen
	{
		cout<<"Error writing command to display!"<<endl;	//Check for errors in writing, call GetLastError with the int response for more info
	}

	if(!WriteFile(my_port, &message, sizeof(message), &bytes_written, NULL))	//Write message
	{
		cout<<"Error writing message to display!"<<endl;	//Check for errors in writing, call GetLastError with the int response for more info
	}

	CloseHandle(my_port);	//Close serial port/file

	cout<<"Write Complete!"<<endl;
	system("pause>nul");
}
