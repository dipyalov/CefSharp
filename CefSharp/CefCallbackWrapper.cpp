#include "StdAfx.h"
#include "BindingHandler.h"
#include "CefCallbackWrapper.h"
#include "include/cef_runnable.h"

using namespace System;
using namespace System::Threading;

namespace CefSharp
{
	static void _call(gcroot<CefCallbackWrapper^> wrapper, gcroot<array<Object^>^> args)
	{
		try
		{
			wrapper->cbInfo->context->Enter();

			CefV8ValueList arguments = CefV8ValueList(args->Length);		
			CefRefPtr<CefV8Value> window = wrapper->cbInfo->context->GetCurrentContext()->GetGlobal();
			for(int i = 0; i < args->Length; i++)
			{
				arguments[i] = BindingHandler::ConvertToCefWithScripting(args[i], args[i]->GetType(), window, nullptr); //  convertToCef(args[i], args[i]->GetType());
			}

			wrapper->cbInfo->callback->ExecuteFunction(CefV8Value::CreateUndefined() , arguments);

			wrapper->cbInfo->context->Exit();
		}
		finally
		{
			wrapper->Release();
		}
	}

	CefCallbackWrapper::CefCallbackWrapper(CefRefPtr<CefV8Value> callback)
	{
		this->cbInfo = new CEF_CALLBACK_INFO();
		this->cbInfo->callCount = 0;
		this->cbInfo->callback = callback.get();
		this->cbInfo->callback->AddRef();

		CefRefPtr<CefV8Context> pC = CefV8Context::GetEnteredContext();
		this->cbInfo->context = pC.get();
		this->cbInfo->context->AddRef();

		Retain();
	}

	CefCallbackWrapper::~CefCallbackWrapper()
	{
		if(!disposed && cbInfo)
		{			
			Release();		
		}
	}

	void CefCallbackWrapper::Call(...array<Object^> ^args)
	{
 		Retain();
		
		if(CefCurrentlyOn(TID_UI))
		{
			_call(this, args);
		}
		else
		{
			CefPostTask(TID_UI, NewCefRunnableFunction(&_call, (gcroot<CefCallbackWrapper^>)this, (gcroot<array<Object^>^>) args));
		}
	}

	void CefCallbackWrapper::Retain()
	{
		if (disposed || System::Threading::Interlocked::Increment(cbInfo->callCount) <= 0)
		{
			throw gcnew InvalidOperationException("CefCallbackWrapper is already disposed");
		}			
	}

	void CefCallbackWrapper::Release()
	{				
		int i = System::Threading::Interlocked::Decrement(cbInfo->callCount);
		if (i < 0)
		{	
			throw gcnew InvalidOperationException("Too many release calls for CefCallbackWrapper");			
		}
		if (i == 0)
		{
			Cleanup();
		}
	}

	void CefCallbackWrapper::Cleanup()
	{
		disposed = true;
		cbInfo->callback->Release();
		cbInfo->context->Release();
		delete cbInfo;
		cbInfo = 0;
	}
}