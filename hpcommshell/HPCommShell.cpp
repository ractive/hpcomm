// HPCommShell.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ClassFactory.h"
#include "resource.h"
#include <olectl.h>
/**************************************************************************
   GUID stuff
**************************************************************************/

//this part is only done once
//if you need to use the GUID in another file, just include Guid.h
#pragma data_seg(".text")
#define INITGUID
#include <initguid.h>
#include <shlguid.h>
#include "Guid.h"
#pragma data_seg()

 /* 
 * Globals
 */
long global_nLockCount;		
long global_nComponents;
HINSTANCE global_hModule;
HIMAGELIST global_hImageListSmall;
HIMAGELIST global_hImageListLarge;

void CreateImageLists();
void DestroyImageLists();

BOOL APIENTRY DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
			global_nLockCount=0;
			global_nComponents=0;
			global_hModule = (HMODULE)hModule;
			
			global_hImageListLarge = NULL;
			global_hImageListSmall = NULL;

			CreateImageLists();
		break;

	case DLL_THREAD_ATTACH:

			break;

	case DLL_PROCESS_DETACH:
			//destroy the global image lists
			DestroyImageLists();

			break;

	case DLL_THREAD_DETACH:

			break;
	}

    return TRUE;
}

/* 
 * Function to return the class factory
 */
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvObj)
{ 
	HRESULT hRes = E_OUTOFMEMORY;
	*ppvObj = NULL;
	
	if(!IsEqualCLSID(rclsid, CLSID_HPCommShell))
	   return CLASS_E_CLASSNOTAVAILABLE;

	ClassFactory *pClassFactory = new ClassFactory;
	if (pClassFactory != NULL)
	{
		hRes = pClassFactory->QueryInterface(riid, ppvObj);
		pClassFactory->Release();
	}
	return hRes;
}

/* 
 * Function which states whether DLL can be safely unloaded
 */
STDAPI DllCanUnloadNow()
{
	HRESULT hRes;
	
	// if no components and no locks, then return S_OK, 
	// which will result in this DLL being unloaded
	hRes = (HRESULT) ((global_nLockCount == 0) 
			&& (global_nComponents == 0) 
			? S_OK : S_FALSE);


	return hRes;
}

STDAPI DllRegisterServer()
{
	HKEY keyHandle, keyHandle1;
	DWORD disposition;
	const char name[]="HP Calculator";
	const char apartment[]="Apartment";
	const long attributes = SFGAO_FOLDER | SFGAO_HASSUBFOLDER;

	LONG ret;
	char path[MAX_PATH];

	if (!GetModuleFileName(global_hModule, path, MAX_PATH))
	{
		// We have to get the pathname
		return SELFREG_E_CLASS;
	}
	
	ret = RegCreateKeyEx(HKEY_CLASSES_ROOT, "CLSID\\{77CA1B92-486F-4bef-97C8-8B608822B266}",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &keyHandle, &disposition);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	ret = RegSetValueEx(keyHandle, NULL, 0, REG_SZ, (const BYTE *) name, strlen(name)+1);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}

	ret = RegCreateKeyEx(keyHandle, "DefaultIcon",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &keyHandle1, &disposition);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}

	ret = RegSetValueEx(keyHandle1, NULL, 0, REG_SZ, (const BYTE *) path, strlen(path)+1);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	RegCloseKey(keyHandle1);

	ret = RegCreateKeyEx(keyHandle, "InprocServer32",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &keyHandle1, &disposition);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	ret = RegSetValueEx(keyHandle1, NULL, 0, REG_SZ, (const BYTE *) path, strlen(path)+1);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	RegCloseKey(keyHandle1);

	ret = RegCreateKeyEx(keyHandle, "InprocServer32",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &keyHandle1, &disposition);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	ret = RegSetValueEx(keyHandle1, NULL, 0, REG_SZ, (const BYTE *) path, strlen(path)+1);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	ret = RegSetValueEx(keyHandle1, "ThreadingModel", 0, REG_SZ, (const BYTE *) apartment, strlen(apartment)+1);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	RegCloseKey(keyHandle1);

	ret = RegCreateKeyEx(keyHandle, "ShellFolder",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &keyHandle1, &disposition);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}

	ret = RegSetValueEx(keyHandle1, "Attributes", 0, REG_BINARY, (const BYTE *) &attributes, 4);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	RegCloseKey(keyHandle1);
	RegCloseKey(keyHandle);

	ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\NetworkNeighborhood\\NameSpace\\{77CA1B92-486F-4bef-97C8-8B608822B266}",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &keyHandle, &disposition);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	RegCloseKey(keyHandle);

	ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ShellExtensions\\Approved\\{77CA1B92-486F-4bef-97C8-8B608822B266}",
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &keyHandle, &disposition);
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	RegCloseKey(keyHandle);

	return S_OK;
}

STDAPI DllUnregisterServer()
{
LONG ret;

	ret = RegDeleteKey(HKEY_CLASSES_ROOT, "CLSID\\{77CA1B92-486F-4bef-97C8-8B608822B266}\\DefaultIcon");
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	ret = RegDeleteKey(HKEY_CLASSES_ROOT, "CLSID\\{77CA1B92-486F-4bef-97C8-8B608822B266}\\InprocServer32");
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}

	ret = RegDeleteKey(HKEY_CLASSES_ROOT, "CLSID\\{77CA1B92-486F-4bef-97C8-8B608822B266}\\ShellFolder");
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}

	ret = RegDeleteKey(HKEY_CLASSES_ROOT, "CLSID\\{77CA1B92-486F-4bef-97C8-8B608822B266}");
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	ret = RegDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\NetworkNeighborhood\\NameSpace\\{77CA1B92-486F-4bef-97C8-8B608822B266}");
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}

	ret = RegDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ShellExtensions\\Approved\\{77CA1B92-486F-4bef-97C8-8B608822B266}");
	if (ret != ERROR_SUCCESS)
	{
		return SELFREG_E_CLASS;
	}
	return S_OK;
}

void CreateImageLists()
{
	if (global_hImageListSmall) {
		ImageList_Destroy(global_hImageListSmall);
	}

	// todo calc-type recoginition......
	HBITMAP hBitMap = LoadBitmap(global_hModule, MAKEINTRESOURCE(IDB_HPITEMS));
	// Load the items to the bitmap
	//bitmap.LoadBitmap(IDB_HPITEMS);
	// Create an appropriate image-list.
	global_hImageListSmall = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 50, 50);
	// Set the "windows" background color.
	ImageList_SetBkColor(global_hImageListSmall, GetSysColor(COLOR_WINDOW));
	// Add the "icons" to the image list.
	ImageList_AddMasked(global_hImageListSmall, hBitMap, ImageList_GetBkColor(global_hImageListSmall));
	DeleteObject(hBitMap);

	if (global_hImageListLarge) {
		ImageList_Destroy(global_hImageListLarge);
	}

	HBITMAP hBigBitMap = LoadBitmap(global_hModule, MAKEINTRESOURCE(IDB_HPITEMSBIG));

	// Create an appropriate image-list.
	global_hImageListLarge = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 50, 50);
	// Set the "windows" background color.
	ImageList_SetBkColor(global_hImageListLarge, GetSysColor(COLOR_WINDOW));
	// Add the "icons" to the image list.
	ImageList_AddMasked(global_hImageListLarge, hBigBitMap, ImageList_GetBkColor(global_hImageListLarge));
	DeleteObject(hBigBitMap);
}

void DestroyImageLists(VOID) {
	if(global_hImageListSmall) {
		ImageList_Destroy(global_hImageListSmall);
		global_hImageListSmall = NULL;
	}

	if(global_hImageListLarge) {
		ImageList_Destroy(global_hImageListLarge);
		global_hImageListLarge = NULL;
	}
}

int WideCharToLocal(LPTSTR pLocal, LPWSTR pWide, DWORD dwChars)
{
	*pLocal = 0;

	#ifdef UNICODE
	lstrcpyn(pLocal, pWide, dwChars);
	#else
	WideCharToMultiByte( CP_ACP, 0, pWide, -1, pLocal, dwChars, NULL, NULL);
	#endif

	return lstrlen(pLocal);
}
