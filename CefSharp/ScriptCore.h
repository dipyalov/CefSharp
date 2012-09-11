#include "stdafx.h"
#pragma once

namespace CefSharp
{
	ref class ScriptCustomEvent;

    public class ScriptCore
    {
    private:
        HANDLE _event;

        gcroot<Object^> _result;
        gcroot<String^> _exceptionMessage;

        bool TryGetMainFrame(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame>& frame);
        void UIT_Execute(CefRefPtr<CefBrowser> browser, CefString script);
        void UIT_Evaluate(CefRefPtr<CefBrowser> browser, CefString script);
		void UIT_DispatchCustomEvent(CefRefPtr<CefBrowser> browser, gcroot<ScriptCustomEvent^> customEvent);
    public:
        ScriptCore()
        {
            _event = CreateEvent(NULL, FALSE, FALSE, NULL);
        }

		DECL void Execute(CefRefPtr<CefBrowser> browser, CefString script);
		DECL gcroot<Object^> Evaluate(CefRefPtr<CefBrowser> browser, CefString script, double timeout);		
		DECL void DispatchCustomEvent(CefRefPtr<CefBrowser> browser, gcroot<ScriptCustomEvent^> customEvent);

		IMPLEMENT_LOCKING(ScriptCore);
        IMPLEMENT_REFCOUNTING(ScriptCore);
    };
}