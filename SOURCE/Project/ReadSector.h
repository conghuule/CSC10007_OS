#pragma once
#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

int ReadSector(LPCWSTR, int, BYTE sector[512]);
unsigned int ByteToDec(BYTE);
string DecToHex(unsigned int);
unsigned int HexToDec(string);
void displaySector(BYTE sector[512]);
int readByteByOffset(BYTE sector[512], string, unsigned int);
