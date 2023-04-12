﻿#pragma once
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

struct Entry
{
	int attribute;
	bool subEntry = 0;
	BYTE content[32];
};

struct FAT32_Directory_File
{
	string attribute; // loại 
	wstring name = L""; // tên
	unsigned int beginCluster; // cluster bắt đầu
	unsigned int size; // kích thước
	Entry* currentEntry; // Entry hiện tại
	vector<unsigned int> clusters; //danh sách các clusters
	vector<unsigned int> sectors; //danh sách các sector
	unsigned int numOfEntries; // số lượng entry
	unsigned int numOfFiles; // số lượng file
	vector <FAT32_Directory_File> child; //các file/folder con (nếu là thư mục)
	vector <FAT32_Directory_File> father; // các thư mục cha
};

bool isFAT32(BYTE sector[512]);
void DisplayFAT32PBSInfo(BYTE Sector[512], FAT32_PBS_STRUCT& PBS);
void readMainEntryInfo(Entry e, FAT32_Directory_File& dir);
wstring readSubEntryInfo(Entry e);
unsigned int RDETNumOSector(LPCWSTR  drive, FAT32_PBS_STRUCT PBS);
void readRDET(LPCWSTR  drive, FAT32_PBS_STRUCT PBS);
void readDirectoryEntry(LPCWSTR  drive, BYTE* sector, unsigned int& i, FAT32_PBS_STRUCT PBS, FAT32_Directory_File& dir);

