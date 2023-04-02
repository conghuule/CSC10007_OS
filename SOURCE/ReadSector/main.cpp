#include "ReadSector.h"

int main(int argc, char** argv)
{
    BYTE sector[512];
    string USB = "E";
    wstring convert(USB.begin(), USB.end());
    wstring path = L"\\\\.\\" + convert + L":"; 
    ReadSector(path.c_str(), 0, sector); //read the first sector, save info into sector array
    //cout << readByteByOffset(sector, "0b", 2); //read 2 BYTES from offset "0b"
    displaySector(sector);
    return 0;
}