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

void setEntryAttr(Entry& e)
{
	unsigned int dec = ByteToDec(e.content[11]);
	switch (dec) {
	case 1: e.attribute = 0; break;
	case 2: e.attribute = 1; break;
	case 4: e.attribute = 2; break;
	case 8: e.attribute = 3; break;
	case 16: e.attribute = 4; break;
	case 32: e.attribute = 5; break;
	case 64: e.attribute = 6; break;
	case 128: e.attribute = 7; break;
	}
}

unsigned int clusterToSector(FAT32_PBS_STRUCT PBS, unsigned int cluster)
{
	return PBS.SectorsInPBS + PBS.NumOfFAT * PBS.FATSize + (cluster - 2) * PBS.SectorsPerCluster;
}

void readMainEntryInfo(Entry e, FAT32_Directory_File& dir)
{
	dir.numOfFiles = 0;
	for (int i = 0; i < 8; i++)
		dir.name += char(e.content[i]);
	for (int i = 8; i < 11; i++)
		dir.name += char(e.content[i]);
	switch (e.attribute) {
	case 0: dir.attribute = "Read Only"; break;
	case 1: dir.attribute = "Hidden"; break;
	case 2: dir.attribute = "System"; break;
	case 3: dir.attribute = "VolLabel"; break;
	case 4: dir.attribute = "Directory"; break;
	case 5: dir.attribute = "Archive"; break;
	case 6: dir.attribute = "Device (internal use only)"; break;
	case 7: dir.attribute = "Unused"; break;
	}
	string beginClusterhex = DecToHex(ByteToDec(e.content[21]));
	beginClusterhex += DecToHex(ByteToDec(e.content[20]));
	beginClusterhex += DecToHex(ByteToDec(e.content[27]));
	beginClusterhex += DecToHex(ByteToDec(e.content[26]));
	dir.beginCluster = HexToDec(beginClusterhex);

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
		cout << clusterNum;
	}
}

unsigned int RDETNumOSector(LPCWSTR  drive, FAT32_PBS_STRUCT PBS)
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

void showDirectoryFile(LPCWSTR  drive, FAT32_PBS_STRUCT PBS, FAT32_Directory_File dir)
{
	cout << "\n===================================================\n";
	cout << "FILE/FOLDER INFO\n";
	wcout << L"Name: " << dir.name << endl;
	cout << "Attribute: " << dir.attribute << endl;
	cout << "Begin Cluster: " << dir.beginCluster << endl;
	cout << "Lists of Clusters: ";
	for (int i = 0; i < dir.clusters.size(); i++)
		cout << dir.clusters[i] << " ";
	cout << "\nList of Sectors: ";
	cout << clusterToSector(PBS, dir.beginCluster) << ",..., " << clusterToSector(PBS, dir.beginCluster) + dir.clusters.size() * PBS.SectorsPerCluster << endl;
	cout << "File size: " << dir.size << endl;


}

void readDirectoryEntry(LPCWSTR  drive, BYTE* sector, unsigned int& i, FAT32_PBS_STRUCT PBS, FAT32_Directory_File& dir)
{
	dir.numOfEntries = 0;
	wstring longname = L"";
	while (true)
	{
		Entry e;
		for (int j = 0; j < 32; j++) {
			e.content[j] = sector[i + j];
		}
		setEntryAttr(e);
		setEntryType(e);
		if (DecToHex(ByteToDec(e.content[0])) != "e5" || DecToHex(ByteToDec(e.content[0])) != "00")
		{
			if (e.content[11] == 15)
			{
				dir.numOfEntries++;
				longname.insert(0, readSubEntryInfo(e));
			}
			else
			{
				dir.numOfEntries++;
				readMainEntryInfo(e, dir);
				readFATTable(drive, PBS, dir.clusters, dir.beginCluster);
				i += 32;
				break;
			}
		}
		i += 32;
	}
	dir.name = longname;

}

void readRDET(LPCWSTR  drive, FAT32_PBS_STRUCT PBS)
{
	unsigned int readPoint = (PBS.SectorsInPBS + PBS.NumOfFAT * PBS.FATSize) * PBS.BytesPerSector;
	unsigned int readSize = RDETNumOSector(drive, PBS) * PBS.BytesPerSector;
	BYTE* sector;
	ReadNTFSSectorByByte(drive, readPoint, sector, readSize);
	displaySector(sector, readSize);
	vector <FAT32_Directory_File> dirs;
	unsigned int i = 0;
	while (i < readSize)
	{
		FAT32_Directory_File dir;
		readDirectoryEntry(drive, sector, i, PBS, dir);
		showDirectoryFile(drive, PBS, dir);
		/*if (dir.attribute == "Directory")
		{
			readSDET(drive, PBS, dir);
		}
		dirs.push_back(dir);*/

	}
}

void readSDET(LPCWSTR  drive, FAT32_PBS_STRUCT PBS, FAT32_Directory_File& dir)
{
	readFATTable(drive, PBS, dir.clusters, dir.beginCluster);
	BYTE* childSector;

	FAT32_Directory_File childFile;


}