#include <windows.h>


GUID _CLSID_WSCProductList = { 0x17072f7b,0x9abe,0x4a74,{0xa2,0x61,0x1e,0xb7,0x6b,0x55,0x10,0x7a} };
GUID _IID_IWSCProductList = { 0x722a338c,0x6e8e,0x4e72,{0xac,0x27,0x14,0x17,0xfb,0x0c,0x81,0xc2} };

typedef
enum WSC_SECURITY_PRODUCT_STATE
{
	WSC_SECURITY_PRODUCT_STATE_ON = 0,
	WSC_SECURITY_PRODUCT_STATE_OFF = 1,
	WSC_SECURITY_PRODUCT_STATE_SNOOZED = 2,
	WSC_SECURITY_PRODUCT_STATE_EXPIRED = 3
} 	WSC_SECURITY_PRODUCT_STATE;

typedef
enum _WSC_SECURITY_SIGNATURE_STATUS
{
	WSC_SECURITY_PRODUCT_OUT_OF_DATE = 0,
	WSC_SECURITY_PRODUCT_UP_TO_DATE = 1
} 	WSC_SECURITY_SIGNATURE_STATUS;

typedef enum _WSC_SECURITY_PROVIDER
{
	// Represents the aggregation of all firewalls for this computer.
	WSC_SECURITY_PROVIDER_FIREWALL = 0x1,
	// Represents the Automatic updating settings for this computer.
	WSC_SECURITY_PROVIDER_AUTOUPDATE_SETTINGS = 0x2,
	// Represents the aggregation of all antivirus products for this comptuer.
	WSC_SECURITY_PROVIDER_ANTIVIRUS = 0x4,
	// Represents the aggregation of all antispyware products for this comptuer.
	WSC_SECURITY_PROVIDER_ANTISPYWARE = 0x8,
	// Represents the settings that restrict the access of web sites in each of the internet zones.
	WSC_SECURITY_PROVIDER_INTERNET_SETTINGS = 0x10,
	// Represents the User Account Control settings on this machine.
	WSC_SECURITY_PROVIDER_USER_ACCOUNT_CONTROL = 0x20,
	// Represents the running state of the Security Center service on this machine.
	WSC_SECURITY_PROVIDER_SERVICE = 0x40,

	WSC_SECURITY_PROVIDER_NONE = 0,

	// Aggregates all of the items that Security Center monitors.
	WSC_SECURITY_PROVIDER_ALL = WSC_SECURITY_PROVIDER_FIREWALL |
	WSC_SECURITY_PROVIDER_AUTOUPDATE_SETTINGS |
	WSC_SECURITY_PROVIDER_ANTIVIRUS |
	WSC_SECURITY_PROVIDER_ANTISPYWARE |
	WSC_SECURITY_PROVIDER_INTERNET_SETTINGS |
	WSC_SECURITY_PROVIDER_USER_ACCOUNT_CONTROL |
	WSC_SECURITY_PROVIDER_SERVICE
} WSC_SECURITY_PROVIDER, * PWSC_SECURITY_PROVIDER;

EXTERN_C const IID IID_IWscProduct;

MIDL_INTERFACE("8C38232E-3A45-4A27-92B0-1A16A975F669")
IWscProduct : public IDispatch
{
public:
	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProductName(
		/* [retval][out] */ __RPC__deref_out_opt BSTR * pVal) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProductState(
		/* [retval][out] */ __RPC__out WSC_SECURITY_PRODUCT_STATE* pVal) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SignatureStatus(
		/* [retval][out] */ __RPC__out WSC_SECURITY_SIGNATURE_STATUS* pVal) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RemediationPath(
		/* [retval][out] */ __RPC__deref_out_opt BSTR* pVal) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProductStateTimestamp(
		/* [retval][out] */ __RPC__deref_out_opt BSTR* pVal) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProductGuid(
		/* [retval][out] */ __RPC__deref_out_opt BSTR* pVal) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProductIsDefault(
		/* [retval][out] */ __RPC__out BOOL* pVal) = 0;
};

EXTERN_C const IID IID_IWSCProductList;

MIDL_INTERFACE("722A338C-6E8E-4E72-AC27-1417FB0C81C2")
IWSCProductList : public IDispatch
{
public:
	virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Initialize(
		/* [in] */ ULONG provider) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count(
		/* [retval][out] */ __RPC__out LONG* pVal) = 0;

	virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item(
		/* [in] */ ULONG index,
		/* [retval][out] */ __RPC__deref_out_opt IWscProduct** pVal) = 0;

};

class DECLSPEC_UUID("17072F7B-9ABE-4A74-A261-1EB76B55107A")
	WSCProductList;



#define HAVOC_CONSOLE_GOOD 0x90
#define HAVOC_CONSOLE_INFO 0x91
#define HAVOC_CONSOLE_ERRO 0x92

extern "C"
{
	#include "beacon.h"

	void go(char* buff, int len);

	DECLSPEC_IMPORT HRESULT  WINAPI		OLE32$CoInitializeEx(LPVOID, DWORD);
	DECLSPEC_IMPORT	HRESULT  WINAPI	 	OLE32$CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv);
	DECLSPEC_IMPORT VOID     WINAPI     	OLEAUT32$SysFreeString(BSTR bstrString);
}

void go(char* buff, int len)
{
	datap  Parser        = { 0 };
	
	HRESULT hr = S_OK;
	IWSCProductList* prod_list = NULL;
	IWscProduct* product = NULL;
	BSTR product_name = NULL;
	WSC_SECURITY_PRODUCT_STATE product_name_state;
	LONG products_count = 0;
	WSC_SECURITY_PROVIDER type;
	
	BeaconDataParse(&Parser, buff, len);
	
	type = (WSC_SECURITY_PROVIDER)BeaconDataInt(&Parser);
	//BeaconPrintf(HAVOC_CONSOLE_INFO, "Type int %d\n", hr);

	hr = OLE32$CoInitializeEx(0, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
	{
		BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error CoInitializeEx: %d\n", hr);
		return;
	}

	hr = OLE32$CoCreateInstance(_CLSID_WSCProductList, NULL, CLSCTX_INPROC_SERVER, _IID_IWSCProductList, (LPVOID*)&prod_list);
	if (FAILED(hr))
	{
		BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error CoCreateInstance: %d\n", hr);
		return;
	}

	hr = prod_list->Initialize(type);
	if (FAILED(hr))
	{
		BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error IWSCProductList->Initialize: %d\n", hr);
		goto CLEAR;
	}

	hr = prod_list->get_Count(&products_count);
	if (FAILED(hr))
	{
		BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error IWSCProductList->get_Count: %d\n", hr);
		goto CLEAR;
	}

	for (LONG i = 0; i < products_count; i++)
	{
		hr = prod_list->get_Item(i, &product);
		if (FAILED(hr))
		{
			BeaconPrintf(HAVOC_CONSOLE_ERRO, "IWSCProductList->get_Item failed with error: %d\n", hr);
			break;
		}

		hr = product->get_ProductName(&product_name);
		if (FAILED(hr))
		{
			BeaconPrintf(HAVOC_CONSOLE_ERRO, "get_ProductName failed with error: %d\n", hr);
			break;
		}
		
		//BeaconPrintf(CALLBACK_OUTPUT, "Product name: %S\n", product_name);
		//OLEAUT32$SysFreeString(product_name);
		//product_name = nullptr;

		hr = product->get_ProductState(&product_name_state);
		if (FAILED(hr))
		{
			BeaconPrintf(HAVOC_CONSOLE_ERRO, "get_ProductState failed with error: %d\n", hr);
			OLEAUT32$SysFreeString(product_name);
			product_name = nullptr;
			break;
		}

		LPSTR wcs_product_state;
		if (product_name_state == WSC_SECURITY_PRODUCT_STATE_ON)
		{
			wcs_product_state = (LPSTR)"On";
		}
		else if (product_name_state == WSC_SECURITY_PRODUCT_STATE_OFF)
		{
			wcs_product_state = (LPSTR)"Off";
		}
		else if (product_name_state == WSC_SECURITY_PRODUCT_STATE_SNOOZED)
		{
			wcs_product_state = (LPSTR)"Snoozed";
		}
		else
		{
			wcs_product_state = (LPSTR)"Expired";
		}
		
		BeaconPrintf(CALLBACK_OUTPUT, "%S: %s\n", product_name, wcs_product_state);
		//BeaconPrintf(CALLBACK_OUTPUT, "Product state: %s\n", wcs_product_state);
		
		OLEAUT32$SysFreeString(product_name);
		product_name = nullptr;
		product->Release();
		product = NULL;
	}

CLEAR:
	if (product)
	{
		product->Release();
	}

	if (prod_list)
	{
		prod_list->Release();
	}

	return;
}
