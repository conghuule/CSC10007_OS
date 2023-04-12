#include "FAT32.h"

bool isFAT32(BYTE sector[512])
{
	string type = "";

	for (int i = 0; i < 8; i++)
	{
		BYTE b = sector[HexToDec("52") + i];
		type += b;
	}

	size_t found = type.find("FAT32");

	if (found != string::npos)
	{
		return true;
	}
	return false;
}

void DisplayFAT32PBSInfo(BYTE sector[512], FAT32_PBS_STRUCT& PBS)
{
	// Read
	PBS.BytesPerSector = readByteByOffset(sector, PBS.BytesPerCluster_Hex, "0b", 2);
	PBS.SectorsPerCluster = readByteByOffset(sector, PBS.SectorsPerCluster_Hex, "0d", 1);
	PBS.SectorsInPBS = readByteByOffset(sector, PBS.SectorsInPBS_Hex, "0e", 2);
	PBS.NumOfFAT = readByteByOffset(sector, PBS.NumOfFAT_Hex, "10", 1);
	PBS.NumOfHead = readByteByOffset(sector, PBS.NumOfHead_Hex, "1a", 2);
	PBS.VolumeSize = readByteByOffset(sector, PBS.VolumeSize_Hex, "20", 4);
	PBS.FATSize = readByteByOffset(sector, PBS.FATSize_Hex, "24", 4);
	PBS.RDETClusterBegin = readByteByOffset(sector, PBS.RDETClusterBegin_Hex, "2c", 4);
	PBS.OtherInfoSector = readByteByOffset(sector, PBS.OtherInfoSector_Hex, "30", 2);
	PBS.PBSCopySector = readByteByOffset(sector, PBS.PBSCopySector_Hex, "32", 2);

	cout << "\n==============================================================\n";
	cout << "FAT32 Partition Boot Sector Info:\n";
	cout << "Bytes Per Sector (bytes): " << PBS.BytesPerSector << endl;
	cout << "Sectors Per Cluster (sectors): " << PBS.SectorsPerCluster << endl;
	cout << "Number of Sectors in Partition Boot Sector (sectors): " << PBS.SectorsInPBS << endl;
	cout << "Number of FATs: " << PBS.NumOfFAT << endl;
	cout << "Number of Heads: " << PBS.NumOfHead << endl;
	cout << "Volume size (sectors): " << PBS.VolumeSize << endl;
	cout << "Size of each FAT (sectors): " << PBS.FATSize << endl;
	cout << "RDET beginning cluster: " << PBS.RDETClusterBegin << endl;
	cout << "Sector containing other info: " << PBS.OtherInfoSector << endl;
	cout << "Sector containing a copy of PBS: " << PBS.PBSCopySector << endl;
}

void setEntryType(Entry& e)
{
	if (ByteToDec(e.content[11]) == 15)
	{
		e.subEntry == 1;
	}
}

bool checkEmptyEntry(Entry e)
{
	bool check = 1;
	for (int i = 0; i < 32; i++)
	{
		if (ByteToDec(e.content[i]) != 0)
			check = 0;
	}
	return check;
}

void setAttribute(Entry e, FAT32_Directory_File& dir)
{
	string res = "";
	unsigned short type = 0;
	while (e.attribute > 0)
	{

		if (e.attribute % 2 == 1)
		{
			switch (type)
			{
			case 0: res += "Read Only "; break;
			case 1: res += "Hidden "; break;
			case 2: res += "System "; break;
			case 3: res += "Vollable "; break;
			case 4: res += "Directory "; break;
			case 5: res += "Archive "; break;

			}
		}
		type++;
		e.attribute >>= 1;
	}
	dir.attribute = res;
}

unsigned int clusterToSector(FAT32_PBS_STRUCT PBS, unsigned int cluster)
{
	return PBS.SectorsInPBS + PBS.NumOfFAT * PBS.FATSize + (cluster - 2) * PBS.SectorsPerCluster;
}

void readMainEntryInfo(Entry e, FAT32_Directory_File& dir)
{
	for (int i = 0; i < 8; i++)
		dir.name += char(e.content[i]); // đọc 8 ký tự đầu của tên
	for (int i = 8; i < 11; i++)
		dir.extension += char(e.content[i]); // đọc 3 ký tự phần mở rộng
	
	string beginClusterhex = DecToHex(ByteToDec(e.content[21]));
	beginClusterhex += DecToHex(ByteToDec(e.content[20]));
	beginClusterhex += DecToHex(ByteToDec(e.content[27]));
	beginClusterhex += DecToHex(ByteToDec(e.content[26]));
	dir.beginCluster = HexToDec(beginClusterhex); //đọc cluster bắt đầu
	//đọc kích thước (kích thước = 0 nếu là tập tin)
	if (dir.attribute == "Directory")
	{
		dir.size = 0;
	}
	else {
		string sizeHex;
		for (int i = 32; i--; i > 27)
		{
			BYTE x = e.content[i];
			sizeHex += DecToHex(ByteToDec(x));
			if (i == 0)
			{
				break;
			}
		}
		dir.size = HexToDec(sizeHex); 
	}

}

wstring readSubEntryInfo(Entry e)
{
	wstring res = L"";
	//đọc 5 ký tự đầu
	for (int i = 0; i < 5; i++)
	{
		unsigned short dec = 0 + (unsigned short)e.content[1 + 2 * i + 1];
		dec <<= 8;
		dec = 0 + (unsigned short)e.content[1 + 2 * i];
		if (dec == 0 || dec == HexToDec("ff"))
		{
			break;
		}
		wchar_t ch = wchar_t(dec);
		res += ch;
	}
	//đọc 6 ký tự tiếp
	for (int i = 0; i < 6; i++)
	{
		unsigned short dec = 0 + (unsigned short)e.content[14 + 2 * i + 1];
		dec <<= 8;
		dec = 0 + (unsigned short)e.content[14 + 2 * i];
		if (dec == 0 || dec == HexToDec("ff"))
		{
			break;
		}
		wchar_t ch = wchar_t(dec);
		res += ch;
	}
	//đọc 2 ký tự tiếp
	for (int i = 0; i < 2; i++)
	{
		unsigned short dec = 0 + (unsigned short)e.content[28 + 2 * i + 1];
		dec <<= 8;
		dec = 0 + (unsigned short)e.content[28 + 2 * i];
		if (dec == 0 || dec == HexToDec("ff"))
		{
			break;
		}
		wchar_t ch = wchar_t(dec);
		res += ch;
	}
	return res;
}

void readFATTable(LPCWSTR  drive, FAT32_PBS_STRUCT PBS, vector<unsigned int>& clusters, unsigned int beginCluster)
{
	unsigned int readPoint = PBS.SectorsInPBS * PBS.BytesPerSector;
	unsigned int readSize = (beginCluster / (PBS.BytesPerSector / 4) + 1) * PBS.BytesPerSector;
	BYTE* sector;
	ReadNTFSSectorByByte(drive, readPoint, sector, readSize);
	if (beginCluster == 0 || beginCluster == 1)
	{
		clusters.push_back(beginCluster);
		return;
	}
	unsigned int offset = beginCluster * 4;
	string cluster_string = "";
	clusters.push_back(beginCluster);
	while (cluster_string != "0fffffff")
	{
		unsigned int clusterNum = readByteByOffset(sector, cluster_string, DecToHex(offset), 4);
		offset = clusterNum * 4;
		if (cluster_string != "0fffffff")
			clusters.push_back(clusterNum);
	}
}

unsigned int RDETNumOfSector(LPCWSTR  drive, FAT32_PBS_STRUCT PBS)
{
	unsigned int readPoint = PBS.SectorsInPBS * PBS.BytesPerSector;
	string cluster_string = "";
	BYTE sector[512];
	ReadSector(drive, readPoint, sector);
	unsigned int offset = 8;
	unsigned int count = 0;
	while (cluster_string != "0fffffff")
	{
		cluster_string = "";
		unsigned int clusterNum = readByteByOffset(sector, cluster_string, DecToHex(offset), 4);
		count++;
		offset += 4;
	}
	return count;
}

void readDirectoryEntry(LPCWSTR  drive, BYTE* sector, unsigned int& i, FAT32_PBS_STRUCT PBS, FAT32_Directory_File& dir)
{
	wstring longname = L"";
	int count = 0;
	while (true)
	{
		Entry e;
		for (int j = 0; j < 32; j++) {
			e.content[j] = sector[i + j];
		}
		e.attribute = e.content[11];
		setEntryType(e);
		setAttribute(e, dir);
		string status = DecToHex(ByteToDec(e.content[0]));
		if (status.find("e5") == string::npos && status.find("00") == string::npos)
		{
			if (ByteToDec(e.content[11]) == 15)
			{
				dir.LFN_flag = 1;
				longname.insert(0, readSubEntryInfo(e));
			}
			else
			{
				readMainEntryInfo(e, dir);
				readFATTable(drive, PBS, dir.clusters, dir.beginCluster);
				i += 32;
				break;
			}
		}
		i += 32;
	}
	if (dir.LFN_flag)
	{
		dir.name = longname;
	}
}

void readRDET(LPCWSTR  drive, FAT32_PBS_STRUCT PBS)
{
	unsigned int readPoint = (PBS.SectorsInPBS + PBS.NumOfFAT * PBS.FATSize) * PBS.BytesPerSector;
	unsigned int readSize = RDETNumOfSector(drive, PBS) * PBS.BytesPerSector;
	BYTE* sector;
	ReadNTFSSectorByByte(drive, readPoint, sector, readSize);
	unsigned int i = 0;
	while (i < readSize)
	{
		FAT32_Directory_File dir;
		readDirectoryEntry(drive, sector, i, PBS, dir);
		DisplayFAT32DirectoryFile(drive, dir, PBS, dir.level);
		if (dir.attribute.find("Directory") != string::npos)
		{
			readSDET(drive, PBS, dir);
		}		
	}
}

void readSDET(LPCWSTR  drive, FAT32_PBS_STRUCT PBS, FAT32_Directory_File& dir)
{
	unsigned int readPoint = clusterToSector(PBS, dir.beginCluster) * PBS.BytesPerSector;
	bool flag = 1;
	unsigned int i = 32 * 2;
	while (flag)
	{
		BYTE* childSector;
		ReadNTFSSectorByByte(drive, readPoint, childSector, 512);
		FAT32_Directory_File childFile;
		wstring longname = L"";
		while (true)
		{
			Entry e;
			for (int j = 0; j < 32; j++) {
				e.content[j] = childSector[i + j];
			}
			e.attribute = e.content[11];
			if (checkEmptyEntry(e))
			{
				return;
			}
			setAttribute(e, childFile);
			setEntryType(e);
			string status = DecToHex(ByteToDec(e.content[0]));
			if (status.find("e5") == string::npos && status.find("00") == string::npos)
			{
				if (ByteToDec(e.content[11] == 15))
				{
					childFile.LFN_flag = 1;
					longname.insert(0, readSubEntryInfo(e));
				}
				else
				{
					readMainEntryInfo(e, childFile);
					readFATTable(drive, PBS, childFile.clusters, childFile.beginCluster);
					i += 32;
					break;
				}
			}
			i += 32;
		}
		if (childFile.LFN_flag)
		{
			childFile.name = longname;
		}
		childFile.level++;
		dir.child.push_back(childFile);
		DisplayFAT32DirectoryFile(drive, childFile, PBS, childFile.level);
		if (childFile.attribute.find("Directory") != string::npos)
			readSDET(drive, PBS, childFile);
		readPoint += 512;
		if (!flag)
		{
			break;
		}
	}
}

void DisplayFAT32DirectoryFile(LPCWSTR  drive, FAT32_Directory_File temp, FAT32_PBS_STRUCT PBS, int level)
{
	string padding = "";
	wstring wpadding = L"";
	for (int i = 0; i < level; i++)
	{
		padding += "     ";
		wpadding += L"     ";
	}
	cout << endl;
	if (temp.LFN_flag)
		wcout << wpadding << L"- File name: " << temp.name << endl;
	else
		wcout << wpadding << L"- File name: " << temp.name + temp.extension << endl;
	cout << padding << "+ File attribute: " << temp.attribute << endl;
	cout << padding << "+ File cluster: "; 
	for (int i = 0; i < temp.clusters.size();i++) 
		cout << temp.clusters[i] << " ";
	cout << endl;
	cout << padding << "+ File sector: ";
	cout << clusterToSector(PBS, temp.beginCluster) << "..." << clusterToSector(PBS, temp.beginCluster) + temp.clusters.size() * PBS.SectorsPerCluster << endl;
	cout << padding << "+ File data: " << endl;
	if (temp.extension.find(L"txt") != string::npos || temp.extension.find(L"TXT") != string::npos)
	{
		unsigned int dataSector = clusterToSector(PBS, temp.beginCluster) * PBS.BytesPerSector;
		BYTE content[512];
		ReadSector(drive, dataSector, content);
		printFileContent(content, 0, 512);
	}
	else
	{
		cout << padding << "\033[91m" << "Use another program to read this file" << endl;
	}
}
