#include "FAT32.h"

void DisplayFAT32PBSInfo(BYTE sector[512])
{
	FAT32_PBS_STRUCT PBS;
	// PBS.BytePerSec = readByteByOffset(sector, PBS.BytePerSecHex, "0b", 2);
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
	cout << "Bytes Per Sector: " << PBS.BytesPerSector << endl;
	cout << "Sectors Per Cluster: " << PBS.SectorsPerCluster << endl;
	cout << "Number of Sectors in Partition Boot Sector: " << PBS.SectorsInPBS << endl;
	cout << "Number of FATs: " << PBS.NumOfFAT << endl;
	cout << "Number of Heads: " << PBS.NumOfHead << endl;
	cout << "Volume size (bytes): " << PBS.VolumeSize << endl;
	cout << "Size of each FAT (bytes): " << PBS.FATSize << endl;
	cout << "RDET beginning cluster: " << PBS.RDETClusterBegin << endl;
	cout << "Sector containing other info: " << PBS.OtherInfoSector << endl;
	cout << "Sector containing a copy of PBS: " << PBS.PBSCopySector << endl;
}