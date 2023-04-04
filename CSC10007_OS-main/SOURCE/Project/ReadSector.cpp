#include "ReadSector.h"
/// <summary>
/// Read a sector with a size of 512 bytes in a drive from a given read point 
/// </summary>
/// <param name="drive">Name of the drive</param>
/// <param name="readPoint">the starting point to read</param>
/// <param name="sector">a BYTE array to save the sector</param>
/// <returns></returns>
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
unsigned int HexToDec(string n)
{
    stringstream ss;
    ss << hex << n;
    unsigned int res;
    ss >> res;
    return res;
}

/// <summary>
/// Convert a Hexadecimal value to Text (in ASCII encoding)
/// </summary>
/// <param name="n">hexadecimal (string) value that needs to convert</param>
/// <returns></returns>
string HexToAscii(string n)
{
    string ascii = "";
    for (size_t i = n.length() - 2; i > 0; i -= 2)
    {
        string part = n.substr(i, 2);
        char ch = stoul(part, nullptr, 16);
        ascii += ch;
    }
    return ascii;
}

/// <summary>
/// display content of a Sector
/// </summary>
/// <param name="sector">sector array</param>
void displaySector(BYTE sector[512])
{
    cout << "==============================================================\n";
    cout << "Sector's content:\n";
    for (unsigned int i = 0; i < 512; i++)
    {
        if (i % 16 == 0  && i > 0)
        {
            cout << endl;
        }
        BYTE x = sector[i];
        string hex = DecToHex(ByteToDec(x));
        cout << hex << "  ";
    }
}
/// <summary>
/// Read a number of BYTEs from a given offset 
/// </summary>
/// <param name="sector">The sector you need to read</param>
/// <param name="res">result in hexa</param>
/// <param name="offset">The offset you want to start reading (hexadecimal)</param>
/// <param name="n_of_bytes">number of bytes you want to read from the offset</param>
/// <returns></returns>
int readByteByOffset(BYTE sector[512], string& res, string offset, unsigned int n_of_bytes)
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
    return HexToDec(res);
}

