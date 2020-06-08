#ifndef FILE_FUNCTIONS_H
#define FILE_FUNCTIONS_H

#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#error NOT_WINDOWS
#endif


#define MAXFILENAMELENGTH 10000

struct DirectoryData
{
	char name[MAXFILENAMELENGTH-2];
};

struct FileObject
{
	std::string name;
	bool directory;
	bool drive;
};



void FILESYSTEM_goBack(DirectoryData *data);
bool FILESYSTEM_advance(DirectoryData *directory, FileObject *object);
bool FILESYSTEM_getCurrentDirectory(DirectoryData *data);
bool FILESYSTEM_getFileObjects(DirectoryData *directory, std::vector <FileObject> *objects);
void FILESYSTEM_copyDirectoryData(DirectoryData *source, DirectoryData *destination);
std::string FILESYSTEM_getFileNameString(DirectoryData *fileName);


#endif