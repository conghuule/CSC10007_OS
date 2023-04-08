#include "FAT32.h"
#include "NTFS.h"
#include "ReadSector.h"

int main(int argc, char** argv)
{
    BYTE sector[512];

    string USB = "";
    cout << "Label Name of USB: ";
    cin >> USB;

    wstring convert(USB.begin(), USB.end());
    wstring path = L"\\\\.\\" + convert + L":";
    ReadSector(path.c_str(), 0, sector); //read the first sector, save info into sector array
    //string res = "";
    //cout << readByteByOffset(sector, res, "0b", 2); //read 2 BYTES from offset "0b"
    displaySector(sector);

    //---------------Đọc thông tin chi tiết của một phân vùng--------------- 
   /* DisplayFAT32PBSInfo(sector);
    DisplayPBSInfo(sector);*/
    //---------------------------
    displayNTFS(path.c_str(), sector);


    return 0;
}