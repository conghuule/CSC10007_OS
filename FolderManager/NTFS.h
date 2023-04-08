#pragma once
#include "ReadSector.h"

struct NTFS_PBS_Struct
{
	unsigned int BytePerSec;
	unsigned int DiskType;
	unsigned int SecPerClus;
	unsigned int SecPerTrack;
	unsigned int NumOfHead;
	unsigned int DickSectorBegin;
	unsigned int TotalSec;
	unsigned int MFTBegin;
	unsigned int MFTMirrBegin;
	unsigned int MFTEntrySize;
	unsigned int ClusPerIndexBuff;
	unsigned int VolSerialNum;

	string BytePerSecHex = "";
	string DiskTypeHex = "";
	string SecPerClusHex = "";
	string SecPerTrackHex = "";
	string NumOfHeadHex = "";
	string DickSectorBeginHex = "";
	string TotalSecHex = "";
	string MFTBeginHex = "";
	string MFTMirrBeginHex = "";
	string MFTEntrySizeHex="";
	string ClusPerIndexBuffHex = "";
	string VolSerialNumHex = "";
};

//Attribute $DATA
//lưu trữ tất cả loại dữ liệu
struct Data_Attribute
{
	unsigned int sizeContent;
	unsigned int countCluster;
	unsigned int firstCluster;
	unsigned int data;
	bool isContentInEntry;

	string sizeContentHex;
	string countClusterHex;
	string firstClusterHex;
	string dataHex;
};

//Cây thư mục NTFS
struct NTFS_Directory_File
{
	unsigned int idFile;//id của thư mục/tập tin
	unsigned int idParrent;//id của thư mục cha chứa nó
	string fileName;//tên thư mục
	string fileType;//loại thư mục
	string nameType;//định dạng tên thư mục
	Data_Attribute* data;//dữ liệu thư mục
	bool allowPrintData;

	vector<NTFS_Directory_File> chillFile;//danh sách thư mục con
};

//attribute $FILE_NAME
struct FileName_Attribute
{
	unsigned int mftEntryFather;//địa chỉ MFT Entry của thư mục cha chứ nó
	unsigned int flags;//cờ báo
	unsigned int lengthName;//chiều dài của tên
	unsigned int typeName;//định dạng tên tập tin
	unsigned int fileName;//tên của tập tin

	string mftEntryFatherHex;
	string flagsHex;
	string lengthNameHex;
	string typeNameHex;
	string fileNameHex;
};


//Attribute $STANDARD_INFORMATION
// nằm ngay sau header của MFT entry
struct Standard_Information_Attribute
{
	unsigned int timeCreate;//thời gian tạo tập tin
	unsigned int timeLastUpdate;//Thời gian thay đổi mới nhất tập tin
	unsigned int timeDiscriptionLastUpdate;//Thời gain thay đổi mới nhất mô tả tập tin
	unsigned int timeLastSeen;//thời gian truy cập tập tin mới nhất
	unsigned int flags;//cờ báo
	unsigned int idClass;//class id
	unsigned int idOwner;// định danh sở hữu
	unsigned int idSecurity;// định danh bảo mật

	string timeCreateHex;
	string timeLastUpdateHex;
	string timeDiscriptionLastUpdateHex;
	string timeLastSeenHex;
	string flagsHex;
	string idClassHex;
	string idOwnerHex;
	string idSecurityHex;
};


//- Header gồm 42 byte đầu tiên được sử dụng để
//chứa một số thông tin mô tả cho MFT Entry
struct MFT_Entry_Header
{
	unsigned int signMFTEntry;
	unsigned int updateSequenceOffset;
	unsigned int numberFixupElement;
	unsigned int LSN;
	unsigned int sequenceNumber;
	unsigned int referenceCount;
	unsigned int offsetBeginAttribute;
	unsigned int flags;
	unsigned int byteUsedMFTEntry;
	unsigned int sizeDiskMFTeEntry;
	unsigned int extensionRecord;
	unsigned int nextAttribute;
	unsigned int idOfThisRecord;

	string signMFTEntryHex;
	string updateSequenceOffsetHex;
	string numberFixupElementHex;
	string LSNHex;
	string sequenceNumberHex;
	string referenceCountHex;
	string offsetBeginAttributeHex;
	string flagsHex;
	string byteUsedMFTEntryHex;
	string sizeDiskMFTeEntryHex;
	string extensionRecordHex;
	string nextAttributeHex;
	string idOfThisRecordHex;
};

//header
struct MFT_Entry_Attribute_Header
{
	unsigned int typeAttribute;//mã loại attribute
	unsigned int sizeAttribute;//kích thước attribute
	unsigned int nonResidentFlag;// cờ báo non-resident
	unsigned int lengthName;//chiều dài của tên attribute
	unsigned int offsetName;//vị trí(offset) chứa tên của attribute
	unsigned int flags;//các cờ báo
	unsigned int idAttribute;//định danh của attribute
	//----
	//xác định attribute có phải kiểu resident không?
	unsigned int lengthAttribute;//kích thước nội dung attribute
	unsigned int offsetAttribute;//vị trí bắt đầu (offset) phần nội dung

	string typeAttributeHex;
	string sizeAttributeHex;
	string nonResidentFlagHex;
	string lengthNameHex;
	string offsetNameHex;
	string flagsHex;
	string idAttributeHex;
	//----
	string lengthAttributeHex;
	string offsetAttributeHex;
};

//Các Attribute
//- Là 1 cấu trúc dữ liệu
//- Chứa nội dung tập tin, chứa thông tin liên quan
//đến tập tin, thư mục,...
struct MFT_Entry_Attribute
{
	//Attribute gồm:
	//- Header
	//- Nội dung
	MFT_Entry_Attribute_Header header;//header
	unsigned int typeAttribute;
	char typeAttributeHex;
	//Nội dung:
	//- Lưu trực tiếp trong Entry (resident attribute)
	//- Lưu ở ngoài Entry (non-resident attribute)
	FileName_Attribute* fileName;
	Data_Attribute* data;
	//Attribute $STANDARD_INFORMATION
	Standard_Information_Attribute* informationStandard;
};

struct MFT_Entry
{
	//MFT Entry gồm 2 phần:
	//- Header
	//- Các attribute
	MFT_Entry_Header header;

	unsigned int numberAttribute;//Số lượng attribute
	vector<MFT_Entry_Attribute> attribute;
};


//MFT
//- Chứa thông tin tất cả các tập tin và thư mục trong
//ổ đỉa logic
struct MFT
{
	//- MFT được chia nhỏ thành các phần bằng
	//nhau gọi là MFT Entry
	unsigned int numberMFTEntry; // số lượng MFT Entry
	vector<MFT_Entry> arrMFTEntry; //Mảng các MFT Entry
};















void DisplayPBSInfo(BYTE sector[512]);
bool isNTFS(BYTE sector[512]);
MFT_Entry readMFTEntry(LPCWSTR drive, NTFS_PBS_Struct pbs, unsigned int idBeginEntry, bool& flag);
void readMFTEntryHeader(BYTE*& sector, MFT_Entry_Header& pbs);
void readMFTEntryAttribute(BYTE* sector, MFT_Entry_Attribute& pbs);
void readMTFStandardInfo(BYTE* sector, Standard_Information_Attribute& PBS);
void readMTFFileName(BYTE* sector, FileName_Attribute& PBS);
void readMFTData(BYTE* sector, Data_Attribute& PBS, unsigned int size, unsigned int VCN, unsigned int fullSize, string beginAttributeOffset);
bool NTFSTotalResearch(LPCWSTR  drive, BYTE sector[512], NTFS_PBS_Struct& PBS, MFT& fileMFT, NTFS_Directory_File& directoryNTFS);
bool AddNTFSFileToTree(NTFS_Directory_File& root, NTFS_Directory_File& inp);
void DisplayNTFSFileTree(LPCWSTR  drive, NTFS_Directory_File root, NTFS_PBS_Struct origin, int level);
void DisplayNTFSDirectoryFile(LPCWSTR  drive, NTFS_Directory_File temp, NTFS_PBS_Struct origin, int level);
void printFileNTFSData(LPCWSTR  drive, unsigned int clusterSize, unsigned int clusterBegin, NTFS_PBS_Struct origin);
void printFileContent(BYTE sector[], unsigned int begin, unsigned int n);
bool displayNTFS(LPCWSTR drive, BYTE sector[512]);