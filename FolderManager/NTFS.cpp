#include "NTFS.h"

void DisplayPBSInfo(BYTE sector[512])
{
	NTFS_PBS_Struct PBS;
	PBS.BytePerSec = readByteByOffset(sector, PBS.BytePerSecHex, "0b", 2);
	PBS.SecPerClus = readByteByOffset(sector, PBS.SecPerClusHex, "0d", 1);
	PBS.SecPerTrack = readByteByOffset(sector, PBS.SecPerTrackHex, "18", 2);
	PBS.NumOfHead = readByteByOffset(sector, PBS.NumOfHeadHex, "1a", 2);
	PBS.HiddenSec = readByteByOffset(sector, PBS.HiddenSecHex, "1c", 4);
	PBS.TotalSec = readByteByOffset(sector, PBS.TotalSecHex, "28", 8);
	PBS.MFTBegin = readByteByOffset(sector, PBS.MFTBeginHex, "30", 8);
	PBS.MFTMirrBegin = readByteByOffset(sector, PBS.MFTMirrBeginHex, "38", 8);
	PBS.ClusPerFileRec = readByteByOffset(sector, PBS.ClusPerFileRecHex, "40", 4);
	PBS.ClusPerIndexBuff = readByteByOffset(sector, PBS.ClusPerIndexBuffHex, "44", 1);
	PBS.VolSerialNum = readByteByOffset(sector, PBS.VolSerialNumHex, "48", 8);
	cout << "\n==============================================================\n";
	cout << "NTFS Partition Boot Sector Info:\n";
	cout << "Bytes Per Sector: " << PBS.BytePerSec << endl;
	cout << "Sectors Per Cluster: " << PBS.SecPerClus << endl;
	cout << "Sectors Per Track: " << PBS.SecPerTrack << endl;
	cout << "Number of Heads: " << PBS.NumOfHead << endl;
	cout << "Hidden Sectors: " << PBS.HiddenSec << endl;
	cout << "Total Sectors: " << PBS.TotalSec << endl;
	cout << "Logical Cluster Number for the file $MFT: " << PBS.MFTBegin << endl;
	cout << "Logical Cluster Number for the file $MFTMirr: " << PBS.MFTMirrBegin << endl;
	cout << "Cluster Per File Record Segment: " << PBS.ClusPerFileRec << endl;
	cout << "Cluster Per Index Buffer: " << PBS.ClusPerIndexBuff << endl;
	cout << "Volume Serial Number: " << PBS.VolSerialNumHex << endl;
}
