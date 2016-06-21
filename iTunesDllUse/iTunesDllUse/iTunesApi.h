#pragma once
#include <vector>


enum err{
	SUCC = 0,
	E_PNULL,
	E_NOTFIND,
	E_REGKEY,
	E_EXIST,
	E_NOEXIST,
	E_AMDCON,
	E_ISPAIR,
	E_VALIDATEPAIRING,
};

enum { IS_DIR = 0, IS_FIL, IS_UNK};
enum { AMDFM_READ = 1, AMDFM_WRITE, AMDFM_READWRITE };
enum conn_stat
{
	ADNCI_MSG_CONNECTED = 1,
	ADNCI_MSG_DISCONNECTED = 2,
	ADNCI_MSG_UNKNOWN = 3,
};

enum CFStringEncodings {
	kCFStringEncodingMacRoman = 0,
	kCFStringEncodingWindowsLatin1 = 0x0500,
	kCFStringEncodingISOLatin1 = 0x0201,
	kCFStringEncodingNextStepLatin = 0x0B01,
	kCFStringEncodingASCII = 0x0600,
	kCFStringEncodingUnicode = 0x0100,
	kCFStringEncodingUTF8 = 0x08000100,
	kCFStringEncodingNonLossyASCII = 0x0BFF,

	kCFStringEncodingUTF16 = 0x0100,
	kCFStringEncodingUTF16BE = 0x10000100,
	kCFStringEncodingUTF16LE = 0x14000100,
	kCFStringEncodingUTF32 = 0x0c000100,
	kCFStringEncodingUTF32BE = 0x18000100,
	kCFStringEncodingUTF32LE = 0x1c000100
};
enum {
	/* Fixed-width types */
	kCFNumberSInt8Type = 1,
	kCFNumberSInt16Type = 2,
	kCFNumberSInt32Type = 3,
	kCFNumberSInt64Type = 4,
	kCFNumberFloat32Type = 5,
	kCFNumberFloat64Type = 6,	/* 64-bit IEEE 754 */
	/* Basic C types */
	kCFNumberCharType = 7,
	kCFNumberShortType = 8,
	kCFNumberIntType = 9,
	kCFNumberLongType = 10,
	kCFNumberLongLongType = 11,
	kCFNumberFloatType = 12,
	kCFNumberDoubleType = 13,
	/* Other */
	kCFNumberCFIndexType = 14,
#if MAC_OS_X_VERSION_10_5 <= MAC_OS_X_VERSION_MAX_ALLOWED
	kCFNumberNSIntegerType = 15,
	kCFNumberCGFloatType = 16,
	kCFNumberMaxType = 16
#else
	kCFNumberMaxType = 14
#endif
};


#define AMSVC_AFC                   func___CFStringMakeConstantString("com.apple.afc")
#define AMSVC_BACKUP                func___CFStringMakeConstantString("com.apple.mobilebackup")
#define AMSVC_CRASH_REPORT_COPY     func___CFStringMakeConstantString("com.apple.crashreportcopy")
#define AMSVC_DEBUG_IMAGE_MOUNT     func___CFStringMakeConstantString("com.apple.mobile.debug_image_mount")
#define AMSVC_NOTIFICATION_PROXY    func___CFStringMakeConstantString("com.apple.mobile.notification_proxy")
#define AMSVC_PURPLE_TEST           func___CFStringMakeConstantString("com.apple.purpletestr")
#define AMSVC_SOFTWARE_UPDATE       func___CFStringMakeConstantString("com.apple.mobile.software_update")
#define AMSVC_SYNC                  func___CFStringMakeConstantString("com.apple.mobilesync")
#define AMSVC_SCREENSHOT            func___CFStringMakeConstantString("com.apple.screenshotr")
#define AMSVC_SYSLOG_RELAY          func___CFStringMakeConstantString("com.apple.syslog_relay")
#define AMSVC_SYSTEM_PROFILER       func___CFStringMakeConstantString("com.apple.mobile.system_profiler")


#define BUF_LEN       2048

#define LoadLibraryModule(dllHandle, fun_pointer, fun_type, fun_name)  \
	fun_pointer = (fun_type)GetProcAddress(dllHandle, fun_name); \
	if (!fun_pointer) { return E_PNULL; }

typedef unsigned int uint;

typedef struct iPhoneFile_tag
{
	long	 handle;
	int      mode;
	long     size;
}iPhoneFile;

typedef struct am_device_notification_callback_info {
	struct am_device *dev;
	unsigned int msg;
} am_device_notification_callback_info;

typedef struct IphoneInfo{
	struct am_device *dev;
	char*  strDeviceName;
	char*  strUniqueDeviceID;
	char*  strProductType;
	char*  strSerialNumber;
	char*  strDeviceColor;
	char*  strProductVersion;
	
	int    isJarlBreaking;
}IPHONEINFO;

typedef struct afc_connection {
	unsigned int handle;            /* 0 */
	unsigned int unknown0;          /* 4 */
	unsigned char unknown1;         /* 8 */
	unsigned char padding[3];       /* 9 */
	unsigned int unknown2;          /* 12 */
	unsigned int unknown3;          /* 16 */
	unsigned int unknown4;          /* 20 */
	unsigned int fs_block_size;     /* 24 */
	unsigned int sock_block_size;   /* 28: always 0x3c */
	unsigned int io_timeout;        /* 32: from AFCConnectionOpen, usu. 0 */
	void *afc_lock;                 /* 36 */
	unsigned int context;           /* 40 */
};

typedef void	(*functransfer_callback)(void* dict, int arg);
typedef	int		(*func_CFGetTypeID)(void* type);
typedef	int		(*func_CFStringGetTypeID)();
typedef	int		(*func_CFStringGetLength)(void* hStr);
typedef void*	(*func___CFStringMakeConstantString)(char* s);
typedef void*   (*func_CFStringCreateWithCString)(void*, char*, uint encode);
typedef void*   (*func_CFURLCreateWithFileSystemPath)(void*, void*, uint encode, int);
typedef void*   (*func_CFURLCopyAbsoluteURL)(void*);
typedef void*   (*func_CFDictionaryGetValue)(void*, void*);
typedef void*   (*func_CFNumberGetValue)(void*, int, int*);
typedef int		(*func_CFEqual)(void*, void*);
typedef void*   (*func_CFStringCreateCopy)(void*, void*);
typedef void*   (*func_CFDictionaryCreate)(void*, void **keys, void **values, long numValues, void *keyCallBacks, void *valueCallBacks);
typedef void	(*func_CFDictionaryGetKeysAndValues)(void* theDict, void **keys, void **values);

typedef int		(*func_CFStringGetCString)(void* cfstring, char* byteString, uint len, uint encode);

typedef void    (*func_am_device_notification_callback)(struct am_device_notification_callback_info *, void* args);
typedef void	(*func_am_restore_device_notification_callback)(struct   am_recovery_device *);

typedef int		(*func_AMDeviceNotificationSubscribe)(func_am_device_notification_callback callback, unsigned int unused0, unsigned int unused1,
															void* dn_unknown3, struct am_device_notification **notification);
typedef int		(*func_AMDeviceConnect)(struct am_device *device);
typedef int		(*func_AMDeviceDisconnect)(struct am_device *device);
typedef int		(*func_AMDeviceIsPaired)(struct am_device *device);
typedef int		(*func_AMDeviceValidatePairing)(struct am_device *device);
typedef int		(*func_AMDeviceTransferApplication)(void* conn, void* path, void* options, functransfer_callback callback, void* unknow1);
typedef int		(*func_AMDeviceStartService)(struct am_device *device, void* service_name, void** handle, unsigned int **unknown);
typedef int		(*func_AFCDeviceInfoOpen)(struct afc_connection *conn, struct afc_dictionary **info);
typedef int		(*func_AFCRemovePath)(struct afc_connection *conn, char *dirname);
typedef int		(*func_AFCRenamePath)(struct afc_connection *conn, char *oldpath, char *newpath);
typedef uint	(*func_AFCConnectionGetContext)(struct afc_connection *conn);
typedef uint	(*func_AFCConnectionGetFSBlockSize)(struct afc_connection *conn);
typedef uint	(*func_AFCConnectionGetIOTimeout)(struct afc_connection *conn);
typedef uint	(*func_AFCConnectionGetSocketBlockSize)(struct afc_connection *conn);
typedef int		(*func_AFCConnectionClose)(struct afc_connection *conn);
typedef long	(*func_CFDictionaryGetCount)(void* dict);


//typedef uint		(*func__AMRestoreRegisterForDeviceNotifications)(am_restore_device_notification_callback dfu_connect_callback,
//	am_restore_device_notification_callback recovery_connect_callback, 
//	am_restore_device_notification_callback dfu_disconnect_callback, 
//	am_restore_device_notification_callback recovery_disconnect_callback, 
//	unsigned int unknown0, void *user_info);

typedef uint		(*func_AMRestoreEnableFileLogging)(char *path);
typedef void*		(*func_AMRestoreCreateDefaultOptions)(void* allocator);
typedef int			(*func_AFCGetFileInfo)(struct afc_connection *conn, char *path, char **buffer, int *length);

typedef uint		(*func_AMRestorePerformRecoveryModeRestore)(struct am_recovery_device *rdev, void* opts, void *callback, void *user_info);
typedef uint		(*func_AMRestorePerformRestoreModeRestore)(struct am_restore_device *rdev, void* opts, void *callback, void *user_info);
typedef struct am_restore_device * (*func_AMRestoreModeDeviceCreate)(unsigned int unknown0, unsigned int connection_id, unsigned int unknown1);
typedef uint		(*func_AMRestoreCreatePathsForBundle)(void* restore_bundle_path,
	void* kernel_cache_type, void* boot_image_type, unsigned int
	unknown0, void* *firmware_dir_path, void* *
	kernelcache_restore_path, unsigned int unknown1, void* *
	ramdisk_path);

typedef uint		(*func_AMDeviceGetConnectionID)(struct am_device *device);
typedef int			(*func_AMDeviceEnterRecovery)(struct am_device *device);
typedef int			(*func_AMDeviceDisconnect)(struct am_device *device);
typedef int			(*func_AMDeviceRelease)(struct am_device *device);
typedef int			(*func_AMDeviceCopyDeviceIdentifier)(struct am_device *device);
typedef int			(*func_AMDShutdownNotificationProxy)(void *);
typedef int			(*func_AMDeviceDeactivate)(struct am_device *device);
typedef int			(*func_AMDeviceActivate)(struct am_device *device, void*);
typedef int			(*func_AMDeviceRemoveValue)(struct am_device *device, unsigned int, const void* *cfstring);
typedef int			(*func_USBMuxListenerCreate)(struct usbmux_listener_1 *esi_fp8, struct usbmux_listener_2 **eax_fp12);
typedef int			(*func_USBMuxListenerHandleData)(void *);
typedef uint		(*func_t_AMRUSBInterfaceReadPipe)(unsigned int readwrite_pipe, unsigned int read_pipe, unsigned char *data, unsigned int *len);

typedef int			(*func_AMDeviceLookupApplications)(struct am_device *device, void* apptype, void** result);

typedef int			(*func_AMDeviceCopyDeviceIdentifier)(struct am_device *device);
typedef void*		(*func_AMDeviceCopyValue)(struct am_device *device, unsigned int, void* cfstring);
typedef		int(*func_AMDeviceStartSession)(struct am_device *device);
typedef		int(*func_AMDeviceStopSession)(struct am_device *device);


typedef		int(*func_AMRestoreRegisterForDeviceNotifications)(

	);
typedef		int(*func_AMDServiceConnectionSend)(HANDLE conn, char* data, UINT len);
typedef		int(*func_AMDServiceConnectionReceive)(HANDLE conn, char* data, UINT* len);
typedef		void(*func_AMDServiceConnectionInvalidate)(HANDLE conn);
typedef		int(*func_AMDeviceGetInterfaceType)(HANDLE handle);
typedef		HANDLE(*func_AMDeviceRetain)(HANDLE handle);


typedef		int(*func_AMDeviceSetValue)(HANDLE handle, HANDLE cfstring1, HANDLE cfstring2);

//cf functions
typedef		HANDLE(*func_CFStringMakeConstantString)(char* data);
//cf functions
typedef		BOOL(*func_CFStringGetCString)(HANDLE cfstring, char* byteString, UINT len, UINT encode);
typedef		int(*func_CFGetTypeID)(HANDLE type);
typedef		int(*func_CFStringGetTypeID)();
typedef		int(*func_CFStringGetLength)(HANDLE hStr);
typedef		void(*func_CFRunLoopRun)(void);


typedef		long(*func_AFCFileInfoOpen)(void *device, char*path, void**data);
typedef		int(*func_AFCKeyValueRead)(void* data, void** key, void** val);
typedef		int(*func_AFCKeyValueClose)(void* val);
typedef		int(*func_AFCConnectionOpen)(void* in, int, void** out);

typedef		int(*func_AFCFileRefOpen)(void *conn, char *path, unsigned long long int mode, long *ref);
typedef		int(*func_AFCFileRefRead)(void *conn, long ref, void *buf, long *len);
typedef		int(*func_AFCFileRefWrite)(void *conn, long ref, void *buf, long len);
typedef		int(*func_AFCFileRefTell)(void* conn, long reg, long* offset);
typedef		int(*func_AFCFileRefClose)(void *conn, long ref);

typedef		int(*func_AFCDirectoryOpen)(void* conn, char* path, void** dir);
typedef		int(*func_AFCDirectoryClose)(void* conn, void* dir);
typedef		int(*func_AFCDirectoryRead)(void*conn, void* dir, char** dirent);


class iTunesApi
{
public:
	iTunesApi();
	~iTunesApi();

public:
	static HMODULE iTunesMobileDeviceDllHandle;
	static HMODULE CoreFoundationDllHandle;
	static std::vector<IPHONEINFO> iphoneInfoVec;
public:
	static int InitApi();
	static int ReleaseApi();
	static void device_callback(struct am_device_notification_callback_info *info, void* args);
	static int  connect_device(struct am_device * device);
	static char* get_device_key_value(struct am_device * device, char* key);
	static bool  isJarlBreaking(struct am_device * device);
	static void transfer_callback(void* dict, int arg);

	static int  getFileAttr(void* afcConn, char* fileName);

	static int  readAllFile(void* afcConn, char* fileName);
	static long iphoneGetFileSize(void * device, char *path);
	static int copyFileFromDevice(void* afcConn, char *pathOnDevice, char *localPath);
	static int readDir(void* afcConn);

	static std::vector<IPHONEINFO> getIphoneInfo()
	{
		return iphoneInfoVec;
	}

public:
	static func_AMDeviceNotificationSubscribe	AMDeviceNotificationSubscribe;
	static func_AMDeviceConnect					AMDeviceConnect;
	static func_AMDeviceDisconnect				AMDeviceDisconnect;
	static func_AMDeviceIsPaired				AMDeviceIsPaired;
	static func_AMDeviceValidatePairing			AMDeviceValidatePairing;
	static func_AMDeviceCopyDeviceIdentifier	AMDeviceCopyDeviceIdentifier;
	static func_AMDeviceCopyValue				AMDeviceCopyValue;
	static func_AMDeviceStartSession			AMDeviceStartSession;
	static func_AMDeviceStopSession				AMDeviceStopSession;
	static func_AMDeviceStartService			AMDeviceStartService;
	static func_AMDeviceLookupApplications		AMDeviceLookupApplications;
	static func_AMDeviceTransferApplication		AMDeviceTransferApplication;

	static func_CFGetTypeID						CFGetTypeID;
	static func_CFStringGetTypeID				CFStringGetTypeID;
	static func_CFStringGetLength				CFStringGetLength;
	static func___CFStringMakeConstantString    __CFStringMakeConstantString;
	static func_CFStringGetCString				CFStringGetCString;
	static func_CFRunLoopRun					CFRunLoopRun;
	static func_CFStringCreateWithCString       CFStringCreateWithCString;
	static func_CFURLCreateWithFileSystemPath	CFURLCreateWithFileSystemPath;
	static func_CFURLCopyAbsoluteURL			CFURLCopyAbsoluteURL;
	static func_CFDictionaryGetValue			CFDictionaryGetValue;
	static func_CFNumberGetValue				CFNumberGetValue;
	static func_CFEqual							CFEqual;
	static func_CFStringCreateCopy				CFStringCreateCopy;
	static func_CFDictionaryCreate				CFDictionaryCreate;
	static func_CFDictionaryGetCount			CFDictionaryGetCount;
	static func_CFDictionaryGetKeysAndValues	CFDictionaryGetKeysAndValues;

	static func_AFCFileInfoOpen					AFCFileInfoOpen;
	static func_AFCKeyValueRead					AFCKeyValueRead;
	static func_AFCKeyValueClose				AFCKeyValueClose;
	static func_AFCConnectionOpen				AFCConnectionOpen;

	static func_AFCFileRefOpen					AFCFileRefOpen;
	static func_AFCFileRefRead					AFCFileRefRead;
	static func_AFCFileRefWrite					AFCFileRefWrite;
	static func_AFCFileRefClose					AFCFileRefClose;

	static func_AFCDirectoryOpen				AFCDirectoryOpen;
	static func_AFCDirectoryClose				AFCDirectoryClose;
	static func_AFCDirectoryRead				AFCDirectoryRead;

	static func_AFCFileRefTell					AFCFileRefTell;

};

