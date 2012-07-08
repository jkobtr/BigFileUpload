#include <afx.h>											// MFC
#include "..\..\..\Common\FileSaver.h"

int main()
{
	FileSaver fs;
	fs.createFile("app1");
	fs.setFileSize(5);
	fs.setPartSize(5);
	fs.savePart("12345",5,1);
	fs.setFileCheck("test");
	fs.check();
};