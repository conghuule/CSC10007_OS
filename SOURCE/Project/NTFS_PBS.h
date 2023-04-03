#pragma once
#include "ReadSector.h"
struct NTFS_PBS_Struct {
	unsigned int BytePerSec;
	unsigned int SecPerClus;
	unsigned int SecPerTrack;
	unsigned int NumOfHead;
	unsigned int HiddenSec;
	unsigned int TotalSec;
	unsigned int MFTBegin;
	unsigned int MFTMirrBegin;
	unsigned int ClusPerFileRec;
	unsigned int ClusPerIndexBuff;
	unsigned int VolSerialNum;

	string BytePerSecHex = "";
	string SecPerClusHex = "";
	string SecPerTrackHex = "";
	string NumOfHeadHex = "";
	string HiddenSecHex = "";
	string TotalSecHex = "";
	string MFTBeginHex = "";
	string MFTMirrBeginHex = "";
	string ClusPerFileRecHex = "";
	string ClusPerIndexBuffHex = "";
	string VolSerialNumHex = "";
};

void DisplayPBSInfo(BYTE sector[512]);
