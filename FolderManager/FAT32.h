#pragma once
#include "ReadSector.h"

struct FAT32_PBS_STRUCT
{
	unsigned int BytesPerSector = 0;
	unsigned int SectorsPerCluster = 0;
	unsigned int SectorsInPBS = 0;
	unsigned int NumOfFAT = 0;
	unsigned int NumOfHead = 0;
	unsigned int VolumeSize = 0;
	unsigned int FATSize = 0;
	unsigned int RDETClusterBegin = 0;
	unsigned int OtherInfoSector = 0;
	unsigned int PBSCopySector = 0;

	string BytesPerCluster_Hex = "";
	string SectorsPerCluster_Hex = "";
	string SectorsInPBS_Hex = "";
	string NumOfFAT_Hex = "";
	string NumOfHead_Hex = "";
	string VolumeSize_Hex = "";
	string FATSize_Hex = "";
	string RDETClusterBegin_Hex = "";
	string OtherInfoSector_Hex = "";
	string PBSCopySector_Hex = "";
	string IsFAT32_Hex = "";
};

//bool IsFAT32(BYTE sector[512]);
void DisplayFAT32PBSInfo(BYTE Sector[512]);