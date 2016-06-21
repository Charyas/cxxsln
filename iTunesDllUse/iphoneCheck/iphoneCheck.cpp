// iphoneCheck.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iTunesApi.h"

int _tmain(int argc, _TCHAR* argv[])
{
	iTunesApi* iTunes = new iTunesApi();
	iTunes->InitApi();
	iTunes->startShareMem();
	struct am_device_notification *notif;
	iTunes->AMDeviceNotificationSubscribe(iTunes->device_callback, 0, 0, NULL, &notif);
	iTunes->SendLoop();
	iTunes->CFRunLoopRun();

	return 0;
}