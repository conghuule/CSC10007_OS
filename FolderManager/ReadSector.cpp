#include <windows.h>
#include <stdio.h>
#include "ReadSector.h"

int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
{
    int retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        printf("Success!\n");
    }
}

int ReadNTFSSectorByByte(LPCWSTR  drive, unsigned int readPoint, BYTE*& sector, unsigned int totalByteSector)
{

    sector = new BYTE[totalByteSector];
    LARGE_INTEGER scale;
    scale.QuadPart = readPoint;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }
    SetFilePointer(device, scale.LowPart, &scale.HighPart, FILE_BEGIN);//Set a Point to Read


    //SetFilePointer(device, numberSector*sectorSize, NULL, FILE_CURRENT);//Set a Point to Read
    if (!ReadFile(device, sector, totalByteSector, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }

}

/// <summary>
/// Convert a byte to a decimal (unsigned int) value
/// </summary>
/// <param name="n">byte that needs to convert</param>
/// <returns></returns>
unsigned int ByteToDec(BYTE n)
{
    unsigned int res = unsigned int((unsigned char)(n));
    return res;

}

/// <summary>
/// Convert a decimal (unsigned int) value to Hexadecimal value
/// </summary>
/// <param name="n">decimal (unsigned int) value that needs to convert</param>
/// <returns></returns>
string DecToHex(unsigned int n)
{
    stringstream ss;
    ss << hex << n;
    string res = ss.str();
    if (res.length() != 2)
        res = "0" + res;
    return res;
}
/// <summary>
/// Convert a Hexadecimal value to Decimal (unsigned int) value
/// </summary>
/// <param name="n">hexadecimal (string) value that needs to convert</param>
/// <returns></returns>
unsigned int HexToDec(string hex)
{
    /*stringstream ss;
    ss << hex << n;
    unsigned int res;
    ss >> res;
    return res;*/
    unsigned int result = 0;
    for (int i = 0; i < hex.length(); i++) {
        if (hex[i] >= 48 && hex[i] <= 57)
        {
            result += (hex[i] - 48) * pow(16, hex.length() - i - 1);
        }
        else if (hex[i] >= 65 && hex[i] <= 70) {
            result += (hex[i] - 55) * pow(16, hex.length() - i - 1);
        }
        else if (hex[i] >= 97 && hex[i] <= 102) {
            result += (hex[i] - 87) * pow(16, hex.length() - i - 1);
        }
    }
    return result;
}

/// <summary>
/// display content of a Sector
/// </summary>
/// <param name="sector">sector array</param>
void displaySector(BYTE* sector, unsigned int n)
{
    cout << "========================================================================\n";
    cout << setw(45) << setfill(' ') << "SECTOR CONTENT\n\n";
    cout << "          ";
    for (unsigned int i = 0; i < 16; i++)
    {
        cout << DecToHex(i) << "  ";
    }

    for (unsigned int i = 0; i < n; i++)
    {
        if (i % 16 == 0)
        {
            cout << endl;
            cout << setw(8) << setfill('0') << DecToHex(i) << "  ";
        }
        BYTE x = sector[i];
        string hex = DecToHex(ByteToDec(x));
        cout << hex << "  ";
    }
}
/// <summary>
/// Read a number of BYTEs from a given offset, returns int value
/// </summary>
/// <param name="sector">The sector you need to read</param>
/// <param name="res">result in hexa</param>
/// <param name="offset">The offset you want to start reading (hexadecimal)</param>
/// <param name="n_of_bytes">number of bytes you want to read from the offset</param>
/// <returns></returns>
unsigned int readByteByOffset(BYTE* sector, string& res, string offset, unsigned int n_of_bytes)
{
    unsigned int index = HexToDec(offset);
    res = "";
    for (unsigned int i = n_of_bytes - 1; i >= 0; i--)
    {
        BYTE x = sector[index + i];
        res += DecToHex(ByteToDec(x));
        if (i == 0)
        {
            break;
        }
    }
    unsigned int ans = HexToDec(res);
    return ans;
}

/// <summary>
/// Read a number of BYTEs from a given offset, return string value 
/// </summary>
/// <param name="sector">The sector you need to read</param>
/// <param name="res">result in hexa</param>
/// <param name="offset">The offset you want to start reading (hexadecimal)</param>
/// <param name="n_of_bytes">number of bytes you want to read from the offset</param>
/// <returns></returns>
string readByteByOffset_string(BYTE sector[512], string& res, string offset, unsigned int n_of_bytes)
{
    unsigned int index = HexToDec(offset);

    for (unsigned int i = n_of_bytes - 1; i >= 0; i--)
    {
        BYTE x = sector[index + i];
        res += DecToHex(ByteToDec(x));
        if (i == 0)
        {
            break;
        }
    }
    return res;
}
//----
char HexToChar(string value)
{
    char res = 0;
    for (int i = 0; i < value.length(); i++)
    {
        res *= 16;
        switch (value[i])
        {
        case '0':res += 0; break;
        case '1':res += 1; break;
        case '2':res += 2; break;
        case '3':res += 3; break;
        case '4':res += 4; break;
        case '5':res += 5; break;
        case '6':res += 6; break;
        case '7':res += 7; break;
        case '8':res += 8; break;
        case '9':res += 9; break;
        case 'a':res += 10; break;
        case 'b':res += 11; break;
        case 'c':res += 12; break;
        case 'd':res += 13; break;
        case 'e':res += 14; break;
        case 'f':res += 15; break;
        default:
            res += 0;
        }
    }

    return res;
}

//int main(int argc, char** argv)
//{
//
//    BYTE sector[512];
//    ReadSector(L"\\\\.\\C:", 0, sector);
//    return 0;
//}

