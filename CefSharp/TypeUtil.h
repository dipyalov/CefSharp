#include "stdafx.h"
#pragma once

#include "include/cef_v8.h"

namespace CefSharp
{
	using namespace System::Collections::Generic;

    bool tryConvertToCef(Object^ obj, Type^ type, CefRefPtr<CefV8Value>& result);
	CefRefPtr<CefV8Value> convertToCef(Object^ obj, Type^ type);
	CefRefPtr<CefV8Value> convertToCef(Object^ obj, Type^ type, Dictionary<Object^, IntPtr>^ cache);
    Object^ convertFromCef(CefRefPtr<CefV8Value> obj);
}