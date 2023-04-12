#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
using namespace std;

int ReadSector(LPCWSTR, int, BYTE sector[512]);
int ReadNTFSSectorByByte(LPCWSTR  drive, unsigned int readPoint, BYTE*& sector, unsigned int totalByteSector);
unsigned int ByteToDec(BYTE);
string DecToHex(unsigned int);
unsigned int HexToDec(string);
string HexToAscii(string);
<<<<<<< Updated upstream
void displaySector(BYTE sector[512]);
int readByteByOffset(BYTE sector[512], string&, string, unsigned int);
=======
void displaySector(BYTE* sector, unsigned int n);
unsigned int readByteByOffset(BYTE sector[512], string&, string, unsigned int);
>>>>>>> Stashed changes
char HexToChar(string value);