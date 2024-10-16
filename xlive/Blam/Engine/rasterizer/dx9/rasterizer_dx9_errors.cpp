#include "stdafx.h"
#include "rasterizer_dx9_errors.h"

#include "cseries/cseries_strings.h"

/* public code */

void rasterizer_dx9_errors_log(HRESULT hr, const char* format, ...)
{
#ifdef ERRORS_ENABLED
	va_list va;
	va_start(va, format);

	const char* hr_string = "<unknown error>";
	char error_string[1024] = {};
	char buffer[1024] = {};
	cvsnzprintf(error_string, NUMBEROF(error_string), format, va);
	switch (hr)
	{
	case D3DERR_UNSUPPORTEDCOLOROPERATION:
		hr_string = "D3DERR_UNSUPPORTEDCOLOROPERATION";
		break;
	case D3DERR_UNSUPPORTEDCOLORARG:
		hr_string = "D3DERR_UNSUPPORTEDCOLORARG";
		break;
    case D3DERR_UNSUPPORTEDALPHAOPERATION:
        hr_string = "D3DERR_UNSUPPORTEDALPHAOPERATION";
        break;
    case D3DERR_UNSUPPORTEDALPHAARG:
        hr_string = "D3DERR_UNSUPPORTEDALPHAARG";
        break;
    case D3DERR_TOOMANYOPERATIONS:
        hr_string = "D3DERR_TOOMANYOPERATIONS";
        break;
    case D3DERR_CONFLICTINGTEXTUREFILTER:
        hr_string = "D3DERR_CONFLICTINGTEXTUREFILTER";
        break;
    case D3DERR_UNSUPPORTEDFACTORVALUE:
        hr_string = "D3DERR_UNSUPPORTEDFACTORVALUE";
        break;
    case D3DERR_CONFLICTINGRENDERSTATE:
        hr_string = "D3DERR_CONFLICTINGRENDERSTATE";
        break;
    case D3DERR_UNSUPPORTEDTEXTUREFILTER:
        hr_string = "D3DERR_UNSUPPORTEDTEXTUREFILTER";
        break;
    case D3DERR_CONFLICTINGTEXTUREPALETTE:
        hr_string = "D3DERR_CONFLICTINGTEXTUREPALETTE";
        break;
    case D3DERR_DRIVERINTERNALERROR:
        hr_string = "D3DERR_DRIVERINTERNALERROR";
        break;
    case D3DERR_NOTFOUND:
        hr_string = "D3DERR_NOTFOUND";
        break;
    case D3DERR_MOREDATA:
        hr_string = "D3DERR_MOREDATA";
        break;
    case D3DERR_DEVICELOST:
        hr_string = "D3DERR_DEVICELOST";
        break;
    case D3DERR_DEVICENOTRESET:
        hr_string = "D3DERR_DEVICENOTRESET";
        break;
    case D3DERR_NOTAVAILABLE:
        hr_string = "D3DERR_NOTAVAILABLE";
        break;
    case D3DERR_INVALIDDEVICE:
        hr_string = "D3DERR_INVALIDDEVICE";
        break;
    case D3DERR_INVALIDCALL:
        hr_string = "D3DERR_INVALIDCALL";
        break;
    case D3DERR_WRONGTEXTUREFORMAT:
        hr_string = "D3DERR_WRONGTEXTUREFORMAT";
        break;
    case D3DERR_OUTOFVIDEOMEMORY:
        hr_string = "D3DERR_OUTOFVIDEOMEMORY";
        break;
    case E_INVALIDARG:
        hr_string = "E_INVALIDARG";
        break;
    case E_FAIL:
        hr_string = "E_FAIL";
        break;
    case E_OUTOFMEMORY:
        hr_string = "E_OUTOFMEMORY";
        break;
    default:
        break;
	}

    error(2, "%s in %s (code=%d, error=%s)", hr_string, error_string, hr, buffer);
    if (hr == E_OUTOFMEMORY)
    {
        // TODO: implement this
        // debug_dump_out_of_memory(buffer);
    }

    va_end(va);
#endif
    return;
}
