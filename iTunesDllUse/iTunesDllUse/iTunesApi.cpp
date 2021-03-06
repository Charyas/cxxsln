#include "stdafx.h"
#include <stdlib.h> 
#include <stdio.h> 
#include <tchar.h>
#include "iTunesApi.h"


HMODULE iTunesApi::iTunesMobileDeviceDllHandle = NULL;
HMODULE iTunesApi::CoreFoundationDllHandle = NULL;
std::vector<IPHONEINFO> iTunesApi::iphoneInfoVec(NULL);
iTunesApi::iTunesApi()
{

}

iTunesApi::~iTunesApi()
{
	ReleaseApi();
}


void iTunesApi::transfer_callback(void* dict, int arg) {
	int percent;
	void* last_path;
	void* status = CFDictionaryGetValue(dict, __CFStringMakeConstantString("Status"));
	CFNumberGetValue(CFDictionaryGetValue(dict, __CFStringMakeConstantString("PercentComplete")), kCFNumberSInt32Type, &percent);

	if (CFEqual(status, __CFStringMakeConstantString("CopyingFile"))) {
		void* path = CFDictionaryGetValue(dict, __CFStringMakeConstantString("Path"));
		last_path = CFStringCreateCopy(NULL, CFStringCreateWithCString(NULL, "/", kCFStringEncodingASCII));
	}


}

int  iTunesApi::getFileAttr(void* afcConn, char* filename)
{
	void*	data = NULL;
	int ret = AFCFileInfoOpen(afcConn, filename, &data);
	if (0 == ret && NULL != data)
	{
		ret = IS_UNK;

		void* name; void* val;
		while (0 == AFCKeyValueRead(data, &name, &val) && NULL != name && NULL != val)
		{
			if (0 == strcmp((char*)name, "st_ifmt"))
			{
				if (0 == strcmp((char*)val, "S_IFDIR")) {
					ret = IS_DIR;
				} else {
					ret = IS_FIL;
				}

				break;
			}
		}

		AFCKeyValueClose(data);
	}

	return ret;
}

char g_dir_buf[4096] = {0};

int  iTunesApi::readAllFile(void* afcConn, char* fileName)
{

	int ret;
	char* buf = NULL;
	void* dir;
	ret = AFCDirectoryOpen(afcConn, fileName, &dir);
	if (0 == ret && NULL != dir)
	{
		do
		{
			char filePath[256] = { 0 };
			ret = AFCDirectoryRead(afcConn, dir, &buf);

			if (NULL == buf) {
				strcat_s(g_dir_buf, 4096, fileName);
				strcat_s(g_dir_buf, 4096, ";");
				break;
			}

			if (0 == strcmp(buf, ".") || 0 == strcmp(buf, "..")) {
				continue;
			}

			if (0 != ret) {
				strcat_s(g_dir_buf, 4096, "null;");
				break;
			}
			if ('/' == fileName[strlen(fileName) - 1]) {
				sprintf_s(filePath, 256, "%s%s", fileName, buf);
			} else {
				sprintf_s(filePath, 256, "%s/%s", fileName, buf);
			}

			ret = getFileAttr(afcConn, filePath);
			if (IS_DIR == ret) {
				//strcat_s(g_dir_buf, 4096, filePath);
				strcat_s(filePath, 256, "/");
				readAllFile(afcConn, filePath);
			} else {
				strcat_s(g_dir_buf, 4096, filePath);
				strcat_s(g_dir_buf, 4096, ";");
			}

		} while (NULL != buf);
	}

	AFCDirectoryClose(afcConn, dir);

	return 0;
}

long iTunesApi::iphoneGetFileSize(void* device, char *path)
{
	void*	data = NULL;
	long	retVal = 0;
	int ret = AFCFileInfoOpen(device, path, &data);
	if (0 == ret && NULL != data)
	{
		void* name; void* val;
		while (0 == AFCKeyValueRead(data, &name, &val) && NULL != name && NULL != val)
		{
			if (0 == strcmp((char*)name, "st_size"))
			{
				retVal = atol((char*)val);
				break;
			}
		}
		AFCKeyValueClose(data);
	}

	return retVal;
}

int iTunesApi::readDir(void* afcConn)
{
	int ret;
	char* buf = NULL;
	void* dir;
	ret = AFCDirectoryOpen(afcConn, "/", &dir);
	if (0 == ret && NULL != dir)
	{
		do 
		{
			ret = AFCDirectoryRead(afcConn, dir, &buf);
			if (0 != ret) break;

			iphoneGetFileSize(afcConn, buf);


		} while (NULL != buf);
	}

	AFCDirectoryClose(afcConn, dir);


	return 0;
}

int iTunesApi::copyFileFromDevice(void* afcConn, char *path, char *localPath)
{
	int ret;
	FILE* outfile;
	fopen_s(&outfile, localPath, "wb");
	if (NULL == outfile) return E_NOEXIST;

	iPhoneFile *iphonefile = (iPhoneFile *)malloc(sizeof(iPhoneFile));
	memset(iphonefile, 0, sizeof(iPhoneFile));

	iphonefile->mode = 1;
	ret = AFCFileRefOpen(afcConn, path, 1, &iphonefile->handle);

	long nCount = 2048;

	char buf[2048] = { 0 };

	//nCount = iphoneGetFileSize(afcConn, path);
	while (! AFCFileRefRead(afcConn, iphonefile->handle, buf, &nCount) && nCount)
	{
		//ret = AFCFileRefRead(afcConn, iphonefile->handle, buf, &nCount);
		//if (ret < 0 || nCount == 0)
		//	break;
		fwrite(buf, 1, nCount, outfile);
	}
	//;
	ret = AFCFileRefClose(afcConn, iphonefile->handle);

	

	fclose(outfile);
	return 0;
}

bool iTunesApi::isJarlBreaking(struct am_device * device)
{
	int ret;
	bool bSucceed = false;
	void* afc_refVal;
	void*  afc_conn;
	afc_refVal = CFStringCreateWithCString(NULL, "com.apple.mobilebackup", kCFStringEncodingUTF8);
	ret = AMDeviceStartService(device, afc_refVal, &afc_conn, NULL);
	if (ret != 0)
		goto end;


	void* keys = CFStringCreateWithCString(NULL, "PackageType", kCFStringEncodingUTF8);
	void* values = CFStringCreateWithCString(NULL, "Developer", kCFStringEncodingUTF8);
	void* options = CFDictionaryCreate(NULL, &keys, &values, 1, NULL, NULL);
	void* dict = NULL;

	ret = AMDeviceLookupApplications(device, options, &dict);
	if (ret != 0)
		goto end;

	long nCount = CFDictionaryGetCount(dict);
	void **val = (void **)malloc(sizeof(void *)* nCount);
	CFDictionaryGetKeysAndValues(dict, NULL, val);
	for (int n = 0; n < nCount; n++)
	{
		void* valRef = CFDictionaryGetValue(val[n], CFStringCreateWithCString(NULL, "CFBundleIdentifier", kCFStringEncodingUTF8));
		char valTmp[0x21] = { 0 };
		CFStringGetCString(valRef, valTmp, 0x20, kCFStringEncodingASCII);
		if (strstr(valTmp, ".Cydia"))
		{
			bSucceed = true;
			break;
		}
	}

	free(val);

end:

	return bSucceed;
}

int iTunesApi::connect_device(struct am_device * device)
{
	int ret;
	ret = AMDeviceConnect(device);
	if (ret < 0) return E_AMDCON;
	ret = AMDeviceIsPaired(device);
	if (ret < 0) return E_ISPAIR;
	ret = AMDeviceValidatePairing(device);
	ret = AMDeviceStartSession(device);
	//if (ret < 0) return E_VALIDATEPAIRING;

	IPHONEINFO  iphoneInfoStru;

	iphoneInfoStru.dev = device;
	iphoneInfoStru.strDeviceName		= get_device_key_value(device, "DeviceName");
	iphoneInfoStru.strUniqueDeviceID	= get_device_key_value(device, "UniqueDeviceID");
	iphoneInfoStru.strProductType		= get_device_key_value(device, "ProductType");
	iphoneInfoStru.strSerialNumber		= get_device_key_value(device, "SerialNumber");
	iphoneInfoStru.strDeviceColor		= get_device_key_value(device, "DeviceColor");
	iphoneInfoStru.strProductVersion	= get_device_key_value(device, "ProductVersion");
	iphoneInfoStru.isJarlBreaking		= isJarlBreaking(device);

	for (std::vector<IPHONEINFO>::iterator it = iphoneInfoVec.begin();
		it != iphoneInfoVec.end(); it++)
	{
		if (0 == strcmp((*it).strUniqueDeviceID, iphoneInfoStru.strUniqueDeviceID))
		{
			return	E_EXIST;
		}
	}
	iphoneInfoVec.push_back(iphoneInfoStru);

	//void* iphoneAFC;
	//void* iphoneConn = NULL;
	//void* afc_refVal;
	//afc_refVal = CFStringCreateWithCString(NULL, "com.apple.afc", kCFStringEncodingUTF8);
	//ret = AMDeviceStartService(device, afc_refVal, &iphoneAFC, NULL);

	//ret = AFCConnectionOpen(iphoneAFC, 0, &iphoneConn);
	//readAllFile(iphoneConn, "/DCIM");
	//readDir(iphoneConn);
	//copyFileFromDevice(iphoneConn, "/iTunes_Control/iTunes/i4tool2.acc", "D:\\TEST_PRO\\i4tool2.acc");
	//iphoneGetFileSize(iphoneConn, "/iTunes_Control/iTunes/iTunesCDB");

	//struct afc_connection *socket = NULL;
	//ret = AMDeviceStartService(device, __CFStringMakeConstantString("com.apple.afc"), &socket, NULL);
	//void* path = CFStringCreateWithCString(NULL, "/", kCFStringEncodingASCII);
	//ret = AMDeviceTransferApplication(socket, path, NULL, transfer_callback, NULL);

	AMDeviceStopSession(device);
	AMDeviceDisconnect(device);

	return SUCC;
}

void iTunesApi::device_callback(struct am_device_notification_callback_info *info, void* args)
{
	switch (info->msg) {
	case ADNCI_MSG_CONNECTED:
		connect_device(info->dev);
		break;
	case ADNCI_MSG_DISCONNECTED:
		AMDeviceDisconnect(info->dev);
		break;
	default:
		break;
	}
}

char* iTunesApi::get_device_key_value(struct am_device * device, char* key)
{
	void*  strVul;
	strVul = AMDeviceCopyValue(device, NULL, __CFStringMakeConstantString(key));
	if (NULL == strVul) return NULL;

	UINT retID = CFGetTypeID(strVul);
	UINT retStringID = CFStringGetTypeID();
	if (retID != retStringID) return NULL;

	UINT len = CFStringGetLength(strVul);
	char* buf = (char*)malloc(len + 1);
	memset(buf, 0, len + 1);
	CFStringGetCString(strVul, buf, MAX_PATH, kCFStringEncodingUTF8);

	return buf;
}

int iTunesApi::InitApi()
{
	HKEY regKey1 = NULL, regKey2 = NULL;
	long ret;
	DWORD dwType = REG_NONE;
	WCHAR value1[MAX_PATH] = { 0 }, value2[MAX_PATH] = {0};
	unsigned long vsize = MAX_PATH;

	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Apple Inc.\\Apple Mobile Device Support\\Shared"), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &regKey1);
	ret = RegQueryValueEx(regKey1, _T("iTunesMobileDeviceDLL"), 0, &dwType, (LPBYTE)value1, &vsize);
	RegCloseKey(regKey1);
	

	if (-1 == GetFileAttributes(value1))
	{
		return E_REGKEY;
	}

	ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)L"SOFTWARE\\Apple Inc.\\Apple Application Support", 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &regKey2);
	ret = ::RegQueryValueEx(regKey2, (LPCWSTR)L"InstallDir", 0, &dwType, (LPBYTE)value2, &vsize);
	RegCloseKey(regKey2);

	char dllpath_iTunesMobileDevice[MAX_PATH];
	char dllpath_CoreFoundation[MAX_PATH];
	size_t retsize;
	size_t len1 = wcslen(value1) - strlen("\\iTunesMobileDevice.dll");
	size_t len2 = wcslen(value2) - 1;
	wcstombs_s(&retsize, dllpath_iTunesMobileDevice, value1, wcslen(value1) - strlen("\\iTunesMobileDevice.dll"));
	wcstombs_s(&retsize, dllpath_CoreFoundation, value2, wcslen(value2) - 1);
	dllpath_iTunesMobileDevice[len1] = '\0';
	dllpath_CoreFoundation[len2] = '\0';

	WCHAR __iTunesMobileDevice_path[MAX_PATH];
	WCHAR __CoreFoundation_path[MAX_PATH];

	char libvar[BUF_LEN] = { 0 };
	char newlibvar[BUF_LEN] = { 0 };

	getenv_s(&retsize, libvar, BUF_LEN, "path");
	sprintf_s(newlibvar, BUF_LEN, "path=%s;", libvar);
	strcat_s(newlibvar, BUF_LEN, dllpath_iTunesMobileDevice);
	strcat_s(newlibvar, BUF_LEN, ";");
	strcat_s(newlibvar, BUF_LEN, dllpath_CoreFoundation);
	_putenv(newlibvar);

	getenv_s(&retsize, libvar, BUF_LEN, "path");

	wcscpy_s(__iTunesMobileDevice_path, MAX_PATH, value1);
	wcscpy_s(__CoreFoundation_path, MAX_PATH, value2);
	wcscat_s(__CoreFoundation_path, MAX_PATH, L"CoreFoundation.dll");

	iTunesMobileDeviceDllHandle = LoadLibrary(__iTunesMobileDevice_path);
	CoreFoundationDllHandle = LoadLibrary(__CoreFoundation_path);

	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceNotificationSubscribe, func_AMDeviceNotificationSubscribe, "AMDeviceNotificationSubscribe");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceConnect,				  func_AMDeviceConnect,				  "AMDeviceConnect");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceDisconnect,			  func_AMDeviceDisconnect,			  "AMDeviceDisconnect");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceIsPaired,			  func_AMDeviceIsPaired,			  "AMDeviceIsPaired");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceValidatePairing,		  func_AMDeviceValidatePairing,		  "AMDeviceValidatePairing");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceCopyDeviceIdentifier,  func_AMDeviceCopyDeviceIdentifier,  "AMDeviceCopyDeviceIdentifier");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceCopyValue,			  func_AMDeviceCopyValue,			  "AMDeviceCopyValue");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceStartSession,		  func_AMDeviceStartSession,		  "AMDeviceStartSession");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceStopSession,			  func_AMDeviceStopSession,			  "AMDeviceStopSession");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceStartService,		  func_AMDeviceStartService,		  "AMDeviceStartService");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceTransferApplication,   func_AMDeviceTransferApplication,   "AMDeviceTransferApplication");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AMDeviceLookupApplications,    func_AMDeviceLookupApplications,    "AMDeviceLookupApplications");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCFileInfoOpen,				  func_AFCFileInfoOpen,		"AFCFileInfoOpen");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCKeyValueRead,				  func_AFCKeyValueRead,		"AFCKeyValueRead");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCKeyValueClose,			  func_AFCKeyValueClose,	"AFCKeyValueClose");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCConnectionOpen,			  func_AFCConnectionOpen,	"AFCConnectionOpen");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCFileRefOpen,				  func_AFCFileRefOpen,		"AFCFileRefOpen");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCFileRefRead,				  func_AFCFileRefRead,		"AFCFileRefRead");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCFileRefWrite,				  func_AFCFileRefWrite,		"AFCFileRefWrite");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCFileRefClose,				  func_AFCFileRefClose,		"AFCFileRefClose");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCDirectoryOpen,			  func_AFCDirectoryOpen,	"AFCDirectoryOpen");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCDirectoryClose,			  func_AFCDirectoryClose,	"AFCDirectoryClose");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCDirectoryRead,			  func_AFCDirectoryRead,	"AFCDirectoryRead");
	LoadLibraryModule(iTunesMobileDeviceDllHandle, AFCFileRefTell,				  func_AFCFileRefTell,		"AFCFileRefTell");


	LoadLibraryModule(CoreFoundationDllHandle, CFGetTypeID,						func_CFGetTypeID,					"CFGetTypeID");
	LoadLibraryModule(CoreFoundationDllHandle, CFStringGetTypeID,				func_CFStringGetTypeID,				"CFStringGetTypeID");
	LoadLibraryModule(CoreFoundationDllHandle, CFStringGetLength,				func_CFStringGetLength,				"CFStringGetLength");
	LoadLibraryModule(CoreFoundationDllHandle, __CFStringMakeConstantString,	func___CFStringMakeConstantString,	"__CFStringMakeConstantString");
	LoadLibraryModule(CoreFoundationDllHandle, CFStringGetCString,				func_CFStringGetCString,			"CFStringGetCString");
	LoadLibraryModule(CoreFoundationDllHandle, CFRunLoopRun,					func_CFRunLoopRun,					"CFRunLoopRun");
	LoadLibraryModule(CoreFoundationDllHandle, CFStringCreateWithCString,		func_CFStringCreateWithCString,		"CFStringCreateWithCString");
	LoadLibraryModule(CoreFoundationDllHandle, CFURLCreateWithFileSystemPath,	func_CFURLCreateWithFileSystemPath,	"CFURLCreateWithFileSystemPath");
	LoadLibraryModule(CoreFoundationDllHandle, CFURLCopyAbsoluteURL,			func_CFURLCopyAbsoluteURL,			"CFURLCopyAbsoluteURL");
	LoadLibraryModule(CoreFoundationDllHandle, CFDictionaryGetValue,			func_CFDictionaryGetValue,			"CFDictionaryGetValue");
	LoadLibraryModule(CoreFoundationDllHandle, CFNumberGetValue,				func_CFNumberGetValue,				"CFNumberGetValue");
	LoadLibraryModule(CoreFoundationDllHandle, CFEqual,							func_CFEqual,						"CFEqual");
	LoadLibraryModule(CoreFoundationDllHandle, CFStringCreateCopy,				func_CFStringCreateCopy,			"CFStringCreateCopy");
	LoadLibraryModule(CoreFoundationDllHandle, CFDictionaryCreate,				func_CFDictionaryCreate,			"CFDictionaryCreate");
	LoadLibraryModule(CoreFoundationDllHandle, CFDictionaryGetCount,			func_CFDictionaryGetCount,			"CFDictionaryGetCount");
	LoadLibraryModule(CoreFoundationDllHandle, CFDictionaryGetKeysAndValues,	func_CFDictionaryGetKeysAndValues,	"CFDictionaryGetKeysAndValues");


	return SUCC;
}

int iTunesApi::ReleaseApi()
{
	FreeLibrary(iTunesMobileDeviceDllHandle);
	FreeLibrary(CoreFoundationDllHandle);

	return SUCC;
}

func_AMDeviceNotificationSubscribe	iTunesApi::AMDeviceNotificationSubscribe;
func_AMDeviceConnect				iTunesApi::AMDeviceConnect;
func_AMDeviceDisconnect				iTunesApi::AMDeviceDisconnect;
func_AMDeviceIsPaired				iTunesApi::AMDeviceIsPaired;
func_AMDeviceValidatePairing		iTunesApi::AMDeviceValidatePairing;
func_AMDeviceCopyDeviceIdentifier	iTunesApi::AMDeviceCopyDeviceIdentifier;
func_AMDeviceCopyValue				iTunesApi::AMDeviceCopyValue;
func_AMDeviceStartSession			iTunesApi::AMDeviceStartSession;
func_AMDeviceStopSession			iTunesApi::AMDeviceStopSession;
func_AMDeviceStartService			iTunesApi::AMDeviceStartService;
func_AMDeviceTransferApplication	iTunesApi::AMDeviceTransferApplication;
func_AMDeviceLookupApplications		iTunesApi::AMDeviceLookupApplications;

func_CFGetTypeID					iTunesApi::CFGetTypeID;
func_CFStringGetTypeID				iTunesApi::CFStringGetTypeID;
func_CFStringGetLength				iTunesApi::CFStringGetLength;
func___CFStringMakeConstantString   iTunesApi::__CFStringMakeConstantString;
func_CFStringGetCString				iTunesApi::CFStringGetCString;
func_CFRunLoopRun					iTunesApi::CFRunLoopRun;
func_CFStringCreateWithCString		iTunesApi::CFStringCreateWithCString;
func_CFURLCreateWithFileSystemPath  iTunesApi::CFURLCreateWithFileSystemPath;
func_CFURLCopyAbsoluteURL			iTunesApi::CFURLCopyAbsoluteURL;
func_CFDictionaryGetValue			iTunesApi::CFDictionaryGetValue;
func_CFNumberGetValue				iTunesApi::CFNumberGetValue;
func_CFEqual						iTunesApi::CFEqual;
func_CFStringCreateCopy				iTunesApi::CFStringCreateCopy;
func_CFDictionaryCreate				iTunesApi::CFDictionaryCreate;
func_CFDictionaryGetCount			iTunesApi::CFDictionaryGetCount;
func_CFDictionaryGetKeysAndValues	iTunesApi::CFDictionaryGetKeysAndValues;

func_AFCFileInfoOpen				iTunesApi::AFCFileInfoOpen;

func_AFCKeyValueRead				iTunesApi::AFCKeyValueRead;
func_AFCKeyValueClose				iTunesApi::AFCKeyValueClose;
func_AFCConnectionOpen				iTunesApi::AFCConnectionOpen;

func_AFCFileRefOpen					iTunesApi::AFCFileRefOpen;
func_AFCFileRefRead					iTunesApi::AFCFileRefRead;
func_AFCFileRefWrite				iTunesApi::AFCFileRefWrite;
func_AFCFileRefTell					iTunesApi::AFCFileRefTell;
func_AFCFileRefClose				iTunesApi::AFCFileRefClose;

func_AFCDirectoryOpen				iTunesApi::AFCDirectoryOpen;
func_AFCDirectoryClose				iTunesApi::AFCDirectoryClose;
func_AFCDirectoryRead				iTunesApi::AFCDirectoryRead;