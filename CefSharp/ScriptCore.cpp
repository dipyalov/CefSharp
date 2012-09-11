#include "stdafx.h"
#include "include/cef_runnable.h"
#include "ScriptCore.h"
#include "ScriptException.h"
#include "ScriptCustomEvent.h"

namespace CefSharp
{
    bool ScriptCore::TryGetMainFrame(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame>& frame)
    {
        if (browser != nullptr)
        {
            frame = browser->GetMainFrame();
            return frame != nullptr;
        }
        else
        {
            return false;
        }
    }

    void ScriptCore::UIT_Execute(CefRefPtr<CefBrowser> browser, CefString script)
    {
        CefRefPtr<CefFrame> mainFrame;
        if (TryGetMainFrame(browser, mainFrame))
        {
            mainFrame->ExecuteJavaScript(script, "about:blank", 0);
        }
    }

    void ScriptCore::UIT_Evaluate(CefRefPtr<CefBrowser> browser, CefString script)
    {
        CefRefPtr<CefFrame> mainFrame;
        if (TryGetMainFrame(browser, mainFrame))
        {
            CefRefPtr<CefV8Context> context = mainFrame->GetV8Context();

            if (context.get() && context->Enter())
            {
                CefRefPtr<CefV8Value> result;
                CefRefPtr<CefV8Exception> exception;

                bool success = context->Eval(script, result, exception);
                if (success)
                {
                   try
                    {
                        _result = convertFromCef(result);
                    }
                    catch (Exception^ ex)
                    {
                        _exceptionMessage = ex->Message;
                    }
                }
                else if (exception.get())
                {
                    _exceptionMessage = toClr(exception->GetMessage());
                }
                else
                {
                    _exceptionMessage = "Failed to evaluate script";
                }

                context->Exit();
            }
        }
        else
        {
            _exceptionMessage = "Failed to obtain reference to main frame";
        }

        SetEvent(_event);
    }

	void ScriptCore::UIT_DispatchCustomEvent(CefRefPtr<CefBrowser> browser, gcroot<ScriptCustomEvent^> customEvent)
    {
        CefRefPtr<CefFrame> mainFrame;
        if (TryGetMainFrame(browser, mainFrame))
        {
            CefRefPtr<CefV8Context> context = mainFrame->GetV8Context();

            if (context.get() && context->Enter())
            {
				try
				{
					CefRefPtr<CefV8Value> detailValue;
					CefRefPtr<CefV8Value> function;
					CefRefPtr<CefV8Exception> exception;					
					
					bool success = true;
					try
					{
						detailValue = convertToCef(customEvent->Detail);
					}
					catch(Exception^ e)
					{
						success = false;
						_exceptionMessage = e->Message;
					}

					if (success)
					{
						success = context->Eval(
							toNative("(function(name, detail) { var e = window.document.createEvent('CustomEvent'); e.initCustomEvent(name, false, false, detail); 	window.dispatchEvent(e); })"),	
							function,
							exception
						);
						if (!success)
						{
							if (exception.get())
							{
								_exceptionMessage = toClr(exception->GetMessage());
							}
							else
							{
								_exceptionMessage = "Failed to dispatch custom event";
							}
						}
					}
					
					if (success)
					{
						CefV8ValueList arguments;
						arguments.push_back(CefV8Value::CreateString(toNative(customEvent->Name)));
						arguments.push_back(detailValue);
						function->ExecuteFunction(						
							NULL,
							arguments
						);

						if (function->HasException())
						{
							success = false;
							_exceptionMessage  = toClr(function->GetException()->GetMessage());
						}
					}
					
					if (!success)
					{
						
					}
				}
				finally
                {
					context->Exit();
				}
            }
        }
        else
        {
            _exceptionMessage = "Failed to obtain reference to main frame";
        }

        customEvent->OnEventFired();
    }

    void ScriptCore::Execute(CefRefPtr<CefBrowser> browser, CefString script)
    {
        if (CefCurrentlyOn(TID_UI))
        {
            UIT_Execute(browser, script);
        }
        else
        {
            CefPostTask(TID_UI, NewCefRunnableMethod(this, &ScriptCore::UIT_Execute, browser, script));
        }
    }

    gcroot<Object^> ScriptCore::Evaluate(CefRefPtr<CefBrowser> browser, CefString script, double timeout)
    {
        AutoLock lock_scope(this);
        _result = nullptr;
        _exceptionMessage = nullptr;

        if (CefCurrentlyOn(TID_UI))
        {
            UIT_Evaluate(browser, script);
        }
        else
        {
            CefPostTask(TID_UI, NewCefRunnableMethod(this, &ScriptCore::UIT_Evaluate,
                browser, script));
        }

        switch (WaitForSingleObject(_event, (DWORD)timeout))
        {
        case WAIT_TIMEOUT:
            throw gcnew ScriptException("Script timed out");
        case WAIT_ABANDONED:
        case WAIT_FAILED:
            throw gcnew ScriptException("Script error");
        }

        if (_exceptionMessage)
        {
            throw gcnew ScriptException(_exceptionMessage);
        }
        else
        {
            return _result;
        }
    }

	void ScriptCore::DispatchCustomEvent(CefRefPtr<CefBrowser> browser, gcroot<ScriptCustomEvent^> customEvent)
	{		
		if (!customEvent)
		{
			throw gcnew ArgumentNullException("customEvent");
		}

		if (CefCurrentlyOn(TID_UI))
        {
            UIT_DispatchCustomEvent(browser, customEvent);
        }
        else
        {
            CefPostTask(TID_UI, NewCefRunnableMethod(this, &ScriptCore::UIT_DispatchCustomEvent, browser, customEvent));
        }	
	}
}