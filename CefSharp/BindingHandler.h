#include "stdafx.h"
#pragma once

using namespace System::Reflection;
using namespace System::Collections::Generic;

namespace CefSharp
{
    class BindingData : public CefBase
    {
    protected:
        gcroot<Object^> _obj;

    public:
        BindingData(Object^ obj)
        {
            _obj = obj;
        }

        Object^ Get()
        {
            return _obj;
        }

        IMPLEMENT_REFCOUNTING(BindingData);
    };

    class BindingHandler : public CefV8Handler
    {
		gcroot<Dictionary<Object^, IntPtr>^> _objectCache;
		
		// Type Converter
        static bool IsNullableType(Type^ type);
        static int GetChangeTypeCost(Object^ value, Type^ conversionType);
        static Object^ ChangeType(Object^ value, Type^ conversionType);		
		static CefRefPtr<CefV8Value> ResolveCefObject(Object^ obj, CefRefPtr<CefV8Value> window, Dictionary<Object^, IntPtr>^ cache);

        CefRefPtr<CefV8Value> ConvertToCef(Object^ obj, Type^ type);
        Object^ ConvertFromCef(CefRefPtr<CefV8Value> obj);
        virtual bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception);		
    public:
		BindingHandler() {}
		BindingHandler(Dictionary<Object^, IntPtr>^ objectCache) : _objectCache(objectCache) {}

		static CefRefPtr<CefV8Value> ConvertToCefWithScripting(Object^ obj, Type^ type, CefRefPtr<CefV8Value> window, Dictionary<Object^, IntPtr>^ cache);
        static void Bind(String^ name, Object^ obj, CefRefPtr<CefV8Value> window);
		static void BindCached(String^ name, Object^ obj, CefRefPtr<CefV8Value> window, Dictionary<Object^, IntPtr>^ cache);

        IMPLEMENT_REFCOUNTING(BindingHandler);
    };
}