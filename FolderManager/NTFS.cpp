#include "NTFS.h"

//---------------Đọc thông tin chi tiết của một phân vùng--------------- 
void DisplayPBSInfo(BYTE sector[512], NTFS_PBS_Struct& PBS)
{
	PBS.BytePerSec = readByteByOffset(sector, PBS.BytePerSecHex, "0b", 2);
	PBS.SecPerClus = readByteByOffset(sector, PBS.SecPerClusHex, "0d", 1);
	PBS.DiskType = readByteByOffset(sector, PBS.DiskTypeHex, "15", 1);
	PBS.SecPerTrack = readByteByOffset(sector, PBS.SecPerTrackHex, "18", 2);
	PBS.NumOfHead = readByteByOffset(sector, PBS.NumOfHeadHex, "1a", 2);
	PBS.DickSectorBegin = readByteByOffset(sector, PBS.DickSectorBeginHex, "1c", 4);
	PBS.TotalSec = readByteByOffset(sector, PBS.TotalSecHex, "28", 8);
	PBS.MFTBegin = readByteByOffset(sector, PBS.MFTBeginHex, "30", 8);
	PBS.MFTMirrBegin = readByteByOffset(sector, PBS.MFTMirrBeginHex, "38", 8);
	PBS.MFTEntrySize = readByteByOffset(sector, PBS.MFTEntrySizeHex, "40", 1);
	PBS.MFTEntrySize = (unsigned int)pow(2,abs(HexToChar(PBS.MFTEntrySizeHex)));
	PBS.ClusPerIndexBuff = readByteByOffset(sector, PBS.ClusPerIndexBuffHex, "44", 1);
	PBS.VolSerialNum = readByteByOffset(sector, PBS.VolSerialNumHex, "48", 8);
	cout << "\n========================================================================\n";
	cout << setw(52) << setfill(' ') << "NTFS PARTITION BOOT SECTOR INFO\n\n";
	cout << "Bytes Per Sector: " << PBS.BytePerSec << endl;
	cout << "Sectors Per Cluster: " << PBS.SecPerClus << endl;
	cout << "Disk Type: " << PBS.DiskType << " = " <<PBS.DiskTypeHex << endl;
	cout << "Sectors Per Track: " << PBS.SecPerTrack << endl;
	cout << "Number of Heads: " << PBS.NumOfHead << endl;
	cout << "Sector start of Dick Logic: " << PBS.DickSectorBegin << endl;
	cout << "Total Sectors: " << PBS.TotalSec << endl;
	cout << "Cluster start of MFT: " << PBS.MFTBegin << endl;
	cout << "Logical Cluster Number for the file $MFTMirr: " << PBS.MFTMirrBegin << endl;
	cout << "Size of MFT Entry: " << PBS.MFTEntrySize << endl;
	cout << "Cluster Per Index Buffer: " << PBS.ClusPerIndexBuff << endl;
	cout << "Volume Serial Number: " << PBS.VolSerialNumHex << endl;
}

//---------------Hiển thị thông tin cây thư mục của phân vùng---------------
//Kiểm tra phải NTFS không
bool isNTFS(BYTE sector[512])
{
	string type = "";
	
	for (int i = 0; i < 8; i++)
	{
		//Offset begin is 3
		BYTE b = sector[3 + i];
		type += b;
	}

	size_t found = type.find("NTFS");

	if (found != string::npos)
	{
		return true;
	}
	return false;
}

//
MFT_Entry readMFTEntry(LPCWSTR drive, NTFS_PBS_Struct pbs, unsigned int idBeginEntry, bool& flag)
{
	MFT_Entry mftEntry;
	BYTE* sector = NULL;
	unsigned int beginMFT = pbs.MFTBegin * pbs.SecPerClus * pbs.BytePerSec + idBeginEntry * pbs.MFTEntrySize;
	unsigned int mftEntrySize = pbs.MFTEntrySize;
	ReadNTFSSectorByByte(drive, beginMFT, sector, mftEntrySize);
	if (sector[0] == 0)
	{
		flag = false;
		return mftEntry;
	}
	//Header
	readMFTEntryHeader(sector, mftEntry.header);

	//Attribute-------------------
	unsigned int paddingOffset = mftEntry.header.offsetBeginAttribute;
	BYTE* sectorCurr = sector + paddingOffset;
	unsigned int remainSize = mftEntry.header.byteUsedMFTEntry - 56;
	mftEntry.numberAttribute = 0;
	while (remainSize > 8)
	{
		MFT_Entry_Attribute mftEntry1;
		mftEntry1.typeAttributeHex = 0;
		mftEntry1.informationStandard = NULL;

		readMFTEntryAttribute(sectorCurr, mftEntry1);
		mftEntry1.typeAttribute = HexToDec(mftEntry1.header.typeAttributeHex);
		BYTE* readType = sectorCurr + HexToDec(mftEntry1.header.offsetAttributeHex);
		if (mftEntry1.typeAttribute == 16)
		{
			mftEntry1.informationStandard = new Standard_Information_Attribute();
			readMTFStandardInfo(readType, *(mftEntry1.informationStandard));

		}
		else
		{
			if (mftEntry1.typeAttribute == 48)
			{
				mftEntry1.fileName = new FileName_Attribute();
				readMTFFileName(readType, *(mftEntry1.fileName));
			}
			else
			{
				if (mftEntry1.header.typeAttributeHex.find("80") != string::npos)
				{
					string l;
					//printToASCCIIandHEX(sectorCurr, 0, 32);

					unsigned int firstVCN = readByteByOffset(sectorCurr, l, "10", 8);
					unsigned int lastVCN = readByteByOffset(sectorCurr, l, "18", 8);
					unsigned int dataRunOffset = readByteByOffset(sectorCurr, l, "1a", 2);
					unsigned int dataSizeRemain = mftEntry1.header.sizeAttribute - dataRunOffset;
					mftEntry1.data = new Data_Attribute();
					if (mftEntry1.header.lengthAttribute == unsigned int(0))
					{
						mftEntry1.data->isContentInEntry = false;
						readMFTData(sectorCurr + dataRunOffset, *(mftEntry1.data), mftEntry1.header.lengthAttribute, lastVCN - firstVCN + 1, dataSizeRemain, mftEntry1.header.offsetAttributeHex);


					}
					else
					{
						mftEntry1.data->isContentInEntry = true;
						readMFTData(readType, *(mftEntry1.data), mftEntry1.header.lengthAttribute, lastVCN - firstVCN + 1, dataSizeRemain, mftEntry1.header.offsetAttributeHex);
						mftEntry1.data->sizeContent = 2;

					}

					///readMFTFileName(readType, *(mftEntry1.Filename));

				}
			}

		}

		mftEntry.attribute.push_back(mftEntry1);
		mftEntry.numberAttribute++;
		paddingOffset += mftEntry1.header.sizeAttribute;
		sectorCurr = sector + paddingOffset;
		remainSize -= mftEntry1.header.sizeAttribute;
	}

	return mftEntry;
}

void readMFTEntryHeader(BYTE*& sector, MFT_Entry_Header& pbs)
{
	pbs.signMFTEntry = readByteByOffset(sector, pbs.signMFTEntryHex, "0", 4);
	string temp = "";
	for (int i = 0; i < pbs.signMFTEntryHex.length(); i++)
	{
		temp += isascii(pbs.signMFTEntryHex[i]);
	}
	pbs.signMFTEntryHex = temp;
	pbs.updateSequenceOffset = readByteByOffset(sector, pbs.updateSequenceOffsetHex, "4", 2);
	pbs.numberFixupElement = readByteByOffset(sector, pbs.numberFixupElementHex, "6", 2);
	pbs.LSN = readByteByOffset(sector, pbs.LSNHex, "8", 8);
	pbs.sequenceNumber = readByteByOffset(sector, pbs.sequenceNumberHex, "10", 2);
	pbs.referenceCount = readByteByOffset(sector, pbs.referenceCountHex, "12", 2);
	pbs.offsetBeginAttribute = readByteByOffset(sector, pbs.offsetBeginAttributeHex, "14", 2);
	pbs.flags = readByteByOffset(sector, pbs.flagsHex, "16", 2);
	pbs.byteUsedMFTEntry = readByteByOffset(sector, pbs.byteUsedMFTEntryHex, "18", 4);
	pbs.sizeDiskMFTeEntry = readByteByOffset(sector, pbs.sizeDiskMFTeEntryHex, "1c", 4);
	pbs.extensionRecord = readByteByOffset(sector, pbs.extensionRecordHex, "20", 8);
	pbs.nextAttribute = readByteByOffset(sector, pbs.nextAttributeHex, "28", 2);
	pbs.idOfThisRecord = readByteByOffset(sector, pbs.idOfThisRecordHex, "2c", 4);
}

string attributeType(unsigned int value)
{
	unsigned short type = 0;
	string res = "";

	while (value > 0)
	{

		if (value % 2 == 1)
		{
			switch (type)
			{
			case 0: res += "Read Only "; break;
			case 1: res += "Hidden "; break;
			case 2: res += "System "; break;
			case 5: res += "Achive "; break;
			case 6: res += "Device "; break;
			case 7: res += "Normal "; break;
			case 8: res += "Temporary "; break;
			case 9: res += "Sparse "; break;
			case 10: res += "Resparese "; break;
			case 11: res += "Compressed "; break;
			case 12: res += "Offline "; break;
			case 13: res += "Not Content "; break;
			case 14: res += "Encrypted "; break;
			case 28: res += "Directory "; break;
			case 29: res += "Index View "; break;

			}
		}
		type++;
		value >>= 1;
	}

	return res;
}

void readMFTEntryAttribute(BYTE* sector, MFT_Entry_Attribute& pbs)
{
	MFT_Entry_Attribute_Header& headerAttribute = pbs.header;

	headerAttribute.typeAttribute = readByteByOffset(sector, headerAttribute.typeAttributeHex, "0", 4);
	headerAttribute.sizeAttribute = readByteByOffset(sector, headerAttribute.sizeAttributeHex, "4", 4);
	headerAttribute.nonResidentFlag = readByteByOffset(sector, headerAttribute.nonResidentFlagHex, "8", 1);
	headerAttribute.lengthName = readByteByOffset(sector, headerAttribute.lengthNameHex, "9", 1);
	headerAttribute.offsetName = readByteByOffset(sector, headerAttribute.offsetNameHex, "0a", 2);
	headerAttribute.flags = readByteByOffset(sector, headerAttribute.flagsHex, "0c", 2);
	headerAttribute.idAttribute = readByteByOffset(sector, headerAttribute.idAttributeHex, "0e", 2);
	headerAttribute.lengthAttribute = readByteByOffset(sector, headerAttribute.lengthAttributeHex, "10", 4);
	headerAttribute.offsetAttribute = readByteByOffset(sector, headerAttribute.offsetAttributeHex, "14", 2);
}

void readMTFStandardInfo(BYTE* sector, Standard_Information_Attribute& PBS)
{
	PBS.timeCreate = readByteByOffset(sector, PBS.timeCreateHex, "00", 8);
	PBS.timeLastUpdate = readByteByOffset(sector, PBS.timeLastUpdateHex, "08", 8);
	PBS.timeDiscriptionLastUpdate = readByteByOffset(sector, PBS.timeDiscriptionLastUpdateHex, "10", 8);
	PBS.timeLastSeen = readByteByOffset(sector, PBS.timeLastSeenHex, "18", 8);
	PBS.flags = readByteByOffset(sector, PBS.flagsHex, "20", 4);
	PBS.idClass = readByteByOffset(sector, PBS.idClassHex, "2c", 4);
	PBS.idOwner = readByteByOffset(sector, PBS.idOwnerHex, "30", 4);
	PBS.idSecurity = readByteByOffset(sector, PBS.idSecurityHex, "34", 2);
}

void readMTFFileName(BYTE* sector, FileName_Attribute& PBS)
{
	PBS.mftEntryFather = readByteByOffset(sector, PBS.mftEntryFatherHex, "0", 8);
	PBS.flags = readByteByOffset(sector, PBS.flagsHex, "38", 4);
	PBS.flagsHex = attributeType(PBS.flags);

	PBS.lengthName = readByteByOffset(sector, PBS.lengthNameHex, "40", 1);
	PBS.typeName = readByteByOffset(sector, PBS.typeNameHex, "41", 1);
	PBS.fileName = readByteByOffset(sector, PBS.fileNameHex, "42", PBS.lengthName*2);

	unsigned int type_name = HexToDec(PBS.typeNameHex);
	switch(type_name)
	{
	case 0:
		PBS.typeNameHex = "POSIX";
		break;
	case 1:
		PBS.typeNameHex = "Win32";
		break;
	case 2:
		PBS.typeNameHex = "DOS";
		break;
	case 3:
		PBS.typeNameHex = "Win32 & DOS";
		break;
	default:
		break;
	}

	string filename = "";
	unsigned int i = PBS.lengthName * 2 - 1;
	while (i > 0)
	{
		string s_temp = PBS.fileNameHex.substr(2 * i, 2);
		size_t found = s_temp.find("00");

		if (found == string::npos)
		{
			filename += HexToDec(s_temp);
		}
		i--;
	}

	PBS.fileNameHex = filename;
}

void readMFTData(BYTE* sector, Data_Attribute& PBS, unsigned int size, unsigned int VCN, unsigned int fullSize, string beginAttributeOffset)
{
	string dataTemp = "";
	if (PBS.isContentInEntry)
	{
		readByteByOffset(sector, dataTemp, "0", size);
		unsigned int i = dataTemp.length() - 2;
		while (i >= 0)
		{
			if (i > dataTemp.length())
				break;
			size_t found = (dataTemp.substr(i,2)).find("NTFS");
			if (found == string::npos)
			{
				string tempsub = dataTemp.substr(i, 2);
				PBS.dataHex += HexToDec(tempsub);
			}
			i-=2;
		}
	}
	else 
	{
		string data = DecToHex(VCN);
		unsigned int dataSize = data.length() % 2 == 0 ? data.length() / 2 : data.length() / 2 + 1;
		unsigned int dataOffset = readByteByOffset(sector, dataTemp, "20", 2);
		PBS.sizeContent = readByteByOffset(sector + dataOffset, PBS.sizeContentHex, "00", 1);
		PBS.countCluster = readByteByOffset(sector + dataOffset, PBS.countClusterHex, "01", dataSize);
		PBS.firstCluster = readByteByOffset(sector + dataOffset, PBS.firstClusterHex, DecToHex(1+ dataSize), fullSize - dataOffset - 1 - dataSize);
		PBS.dataHex = "";
	}
}

bool NTFSTotalResearch(LPCWSTR  drive, BYTE sector[512], NTFS_PBS_Struct& PBS, MFT& fileMFT, NTFS_Directory_File& directoryNTFS)
{

	if (isNTFS(sector) == true)
	{
		DisplayPBSInfo(sector, PBS);

		//read MFT
		unsigned int cnt = 36;
		bool flag = true;
		vector<NTFS_Directory_File>& listFile = directoryNTFS.chillFile;

		while (true)
		{
			MFT_Entry entryAdd = readMFTEntry(drive, PBS, cnt, flag);
			if (flag == false) break;
			fileMFT.arrMFTEntry.push_back(entryAdd);
			fileMFT.numberMFTEntry++;

			cnt++;
		}

		for (unsigned int k = 0; k < fileMFT.numberMFTEntry; k++)
		{
			NTFS_Directory_File file;
			file.idFile = fileMFT.arrMFTEntry[k].header.idOfThisRecord;
			file.data = NULL;
			file.allowPrintData = false;

			MFT_Entry temp = fileMFT.arrMFTEntry[k];
			for (unsigned i = 0; i < temp.numberAttribute; i++)
			{

				if (temp.attribute[i].typeAttribute == 16)
				{
				}
				if (temp.attribute[i].typeAttribute == 48)
				{
					file.idParrent = temp.attribute[i].fileName->mftEntryFather;
					file.fileName = temp.attribute[i].fileName->fileNameHex;
					file.fileType = temp.attribute[i].fileName->flagsHex;
					file.nameType = temp.attribute[i].fileName->typeNameHex;

					if (k == 0)
					{
						directoryNTFS.idFile = temp.attribute[i].fileName->mftEntryFather;
					}

					if (temp.attribute[i].fileName->fileNameHex.find("txt") != string::npos ||
						temp.attribute[i].fileName->fileNameHex.find("sql") != string::npos)
					{
						file.allowPrintData = true;
					}
				}
				if (temp.attribute[i].header.typeAttributeHex.find("80") != string::npos)
				{
					file.data = (temp.attribute[i].data);
					if (file.data->isContentInEntry)
					{
						file.data->firstCluster = PBS.MFTBegin * PBS.SecPerClus + (36 + k) * 2;
						file.data->sizeContent = 2;
					}
				}
			}
			if (!AddNTFSFileToTree(directoryNTFS, file))
			{
				directoryNTFS.chillFile.push_back(file);
			}
		}
		return true;
	}
	return false;
}
//_------
bool AddNTFSFileToTree(NTFS_Directory_File& root, NTFS_Directory_File& inp)
{
	if (root.idFile == inp.idParrent)
	{
		root.chillFile.push_back(inp);
		return true;
	}
	for (unsigned int i = 0; i < root.chillFile.size(); i++)
	{
		if (AddNTFSFileToTree(root.chillFile[i], inp))
		{
			return true;
		}

	}
	return false;

}

void DisplayNTFSFileTree(LPCWSTR  drive, NTFS_Directory_File root, NTFS_PBS_Struct origin, int level)
{
	for (unsigned int i = 0; i < root.chillFile.size(); i++)
	{
		NTFS_Directory_File& temp = root.chillFile[i];
		DisplayNTFSDirectoryFile(drive, temp, origin, level);
		DisplayNTFSFileTree(drive, root.chillFile[i], origin, level + 1);
	}
}

void DisplayNTFSDirectoryFile(LPCWSTR  drive, NTFS_Directory_File temp, NTFS_PBS_Struct origin, int level)
{
	string padding = "";
	for (int i = 0; i < level; i++)
	{
		padding += "     ";
	}
	cout << endl;
	cout << padding << "- File name: " << temp.fileName << endl;
	cout << padding << "+ File type: " << temp.fileType << endl;
	cout << padding << "+ File name type: " << temp.nameType << endl;

	if (temp.data == NULL)
	{
		return;
	}
	cout << padding << "+ File sector: ";
	if (temp.data->isContentInEntry)
	{

		cout << temp.data->firstCluster << ", ... , " << temp.data->firstCluster + temp.data->sizeContent - 1;
	}
	else
	{
		cout << temp.data->firstCluster * origin.SecPerClus << ", ... , " << temp.data->firstCluster * origin.SecPerClus + (temp.data->countCluster * origin.SecPerClus - 1);
	}
	cout << endl;
	cout << padding << "+ File data: " << endl;

	if (!temp.data->isContentInEntry)
	{
		if (temp.allowPrintData)
		{
			printFileNTFSData(drive, temp.data->countCluster, temp.data->firstCluster, origin);

		}
		else
		{
			cout << padding<< "\033[91m" << "Use another program to read this file" << endl;
		}
	}
	else
	{
		cout << padding << "\033[91m" <<"|" << setw(40) << setfill('-') <<"|" << "\033[0m" << endl;
		cout << padding << temp.data->dataHex << endl;
	}
	cout << padding << "\033[91m" << "|----------------- EOF -----------------|" << "\033[0m" << endl;
	cout << endl;
}

void printFileNTFSData(LPCWSTR  drive, unsigned int clusterSize, unsigned int clusterBegin, NTFS_PBS_Struct origin)
{
	unsigned int begin = clusterBegin * origin.SecPerClus * origin.BytePerSec;
	unsigned int totalSector = clusterSize * origin.SecPerClus;

	BYTE sector[512];
	ReadSector(drive, begin, sector);
	printFileContent(sector, 0, totalSector * origin.BytePerSec);

}

void printFileContent(BYTE sector[], unsigned int begin, unsigned int n)
{
	cout  << "------------------------" << endl;
	cout  << "     FILE CONTENT" << endl;
	cout  << "------------------------" << endl;
	unsigned int temp = begin;


	for (unsigned int i = 0; i < n; i++)
	{


		BYTE b = sector[i];

		printf("\033[95m%c", isascii(b) ? b : '.');

	}

	cout << endl << "------------ EOF ------------" << endl;
}

bool displayNTFS(LPCWSTR drive, BYTE sector[512])
{
	NTFS_PBS_Struct PBS;

	MFT mft;
	mft.numberMFTEntry = 0;

	NTFS_Directory_File ntfs;


	if (NTFSTotalResearch(drive, sector, PBS, mft, ntfs))
	{
		cout << "========================================================================\n";
		cout << setw(42) << setfill(' ') << "DIRECTORY TREE" << endl;
		DisplayNTFSFileTree(drive, ntfs, PBS, 0);

		return true;
	}
	return false;
}