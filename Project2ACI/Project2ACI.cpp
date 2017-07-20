// Project2ACI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void getRegistryValue(HKEY hkey, CString path, CString valueName, int& err, CString& out) {
	HKEY curKey;
	WCHAR output[8096];
	DWORD size = 8096 * sizeof(WCHAR);
	if (err = RegOpenKeyEx(hkey, path, 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &curKey) != ERROR_SUCCESS) {
		RegCloseKey(curKey);
		out = "Access Denied";
		return;
	}
	if (err = RegQueryValueEx(curKey, valueName, NULL, NULL, (LPBYTE)output, &size) != ERROR_SUCCESS) {
		RegCloseKey(curKey);
		out = "Bad Value";
		return;
	}
	out = output;
	RegCloseKey(curKey);
	return;
}


int main()
{
	//Get User Name
	WCHAR username[UNLEN + 1];
	DWORD size = UNLEN + 1;
	if (GetUserName((LPWSTR)username, &size) == 0) {
		printf("Could not get user name with error code %d!\n", GetLastError());
		return -1;
	}

	//Create new output file
	std::wstring filePath(L"C:\\Users\\" + std::wstring(username) + L"\\AppData\\sys.txt");
	FILE* file;
	DeleteFile(filePath.c_str());
	file = _wfopen(filePath.c_str(), L"w");
	if (file == NULL) {
		perror("File could not be opened!\n");
		getchar();
		return -2;
	}

	//Get size of System Registry
	DWORD maxRegistry;
	DWORD curRegistry;
	if (GetSystemRegistryQuota(&maxRegistry, &curRegistry) == 0) {
		printf("Could not get system Registry Quota with error code %d!\n", GetLastError());
		getchar();
	}
	else {
		fprintf(file, "Registry Information:\n");
		fprintf(file, "-----------------------------------\n");
		fprintf(file, "Current Registry Size:  %lu\n", curRegistry);
		fprintf(file, "-----------------------------------\n");
		fprintf(file, "Max Registry Size:  \t%lu\n", maxRegistry);
		fprintf(file, "-----------------------------------\n");
	}

	//Get Product Id
	int err;
	CString value;
	getRegistryValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductId", err, value);
	if (value != "Access Denied" && value != "Bad Value") {
		fprintf(file, "Windows Product ID: \t%S\n", value);
		fprintf(file, "-----------------------------------\n");
	}
	//Get Last Registry Key Accessed
	getRegistryValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Regedit", L"LastKey", err, value);
	if (value != "Access Denied" && value != "Bad Value") {
		fprintf(file, "Last Key Accessed: \t\t%S\n", value);
		fprintf(file, "-----------------------------------\n");
	}
	
	//Hide File
	int attr = GetFileAttributes(filePath.c_str());
	if ((attr & FILE_ATTRIBUTE_HIDDEN) == 0) {
		SetFileAttributes(filePath.c_str(), attr | FILE_ATTRIBUTE_HIDDEN);
	}

	//Clean Up
	if (fclose(file) != 0) {
		printf("File could not be closed!\n");
		return -3;
	}
	return 0;
}