#include "StdAfx.h"
#include "CefCallbackWrapper.h"
#include "include/cef_runnable.h"

using namespace System;

namespace CefSharp
{
	static void _call(gcroot<CefCallbackWrapper^> wrapper, gcroot<array<Object^>^> args)
	{
		try
		{
			wrapper->cbInfo->context->Enter();

			CefV8ValueList arguments = CefV8ValueList(args->Length);
			for(int i = 0; i < args->Length; i++)
			{
				arguments[i] = convertToCef(args[i], args[i]->GetType());
			}

			wrapper->cbInfo->callback->ExecuteFunction(CefV8Value::CreateUndefined() , arguments);

			wrapper->cbInfo->context->Exit();
		}
		finally
		{
			if (System::Threading::Interlocked::Decrement(wrapper->cbInfo->callCount) == 0)
			{
				wrapper->Cleanup();
			}
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

		System::Threading::Interlocked::Increment(cbInfo->callCount);
	}

	CefCallbackWrapper::~CefCallbackWrapper()
	{
		if(!disposed && cbInfo)
		{
			disposed = true;
			if (System::Threading::Interlocked::Decrement(cbInfo->callCount) == 0)
			{
				Cleanup();
			}			
		}
	}

	void CefCallbackWrapper::Call(...array<Object^> ^args)
	{
		if (System::Threading::Interlocked::Increment(cbInfo->callCount) <= 0)
		{
			throw gcnew InvalidOperationException("CefCallbackWrapper is already disposed");
		}	
		
		if(CefCurrentlyOn(TID_UI)){
			_call(this, args);
		}
		else
		{
			CefPostTask(TID_UI, NewCefRunnableFunction(&_call, (gcroot<CefCallbackWrapper^>)this, (gcroot<array<Object^>^>) args));
		}
	}

	void CefCallbackWrapper::Cleanup()
	{
		cbInfo->callback->Release();
		cbInfo->context->Release();
		delete cbInfo;
		cbInfo = 0;
	}
}