#pragma once

#include "libomilicqt_global.h"

#ifdef __cplusplus
extern "C"
{
#endif
	/// <summary>
	/// show license dialog
	/// </summary>
	/// <param name="productName">[xxxx].exe, xxxx is the productName</param>
	void LIBOMILICQT_EXPORT __stdcall ShowRegDialogVC(char* productName);

	/// <summary>
	/// show license modal dialog
	/// </summary>
	/// <param name="productName">[xxxx].exe, xxxx is the productName</param>
	void LIBOMILICQT_EXPORT __stdcall ShowRegModalDialogVC(char* productName);

	/// <summary>
	/// get license's status
	/// </summary>
	/// <param name="productName">[xxxx].exe, xxxx is the productName</param>
	/// <returns>license's status,see below meaning</returns>
	/// <seealso>
	/// LICENSE_OK = 0, 						//OK
	/// LICENSE_FILE_NOT_FOUND = 1, 		//license file not found
	/// LICENSE_SERVER_NOT_FOUND = 2, 		//license server can't be contacted
	/// ENVIRONMENT_VARIABLE_NOT_DEFINED = 3, //environment variable not defined
	/// FILE_FORMAT_NOT_RECOGNIZED = 4,	//license file has invalid format (not .ini file)
	/// LICENSE_MALFORMED = 5, //some mandatory field are missing, or data can't be fully read.
	/// PRODUCT_NOT_LICENSED = 6,		//this product was not licensed
	/// PRODUCT_EXPIRED = 7,
	/// LICENSE_CORRUPTED = 8,//License signature didn't match with current license
	/// IDENTIFIERS_MISMATCH = 9, //Calculated identifier and the one provided in license didn't match
	/// LICENSE_FILE_FOUND = 100,
	/// LICENSE_VERIFIED = 101
	/// </seealso>
	int LIBOMILICQT_EXPORT __stdcall GetLicenseStatusVC(char* productName);

#ifdef __cplusplus
}
#endif