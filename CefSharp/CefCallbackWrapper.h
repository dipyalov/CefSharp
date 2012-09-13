#pragma once

using namespace System;
using namespace System::Threading;
namespace CefSharp
{
	struct CEF_CALLBACK_INFO {
		int callCount;
		CefV8Value *callback;
		CefV8Context *context;
	};

	public ref class CefCallbackWrapper : IDisposable
	{
	public:
		CefCallbackWrapper(CefRefPtr<CefV8Value> callback);
		~CefCallbackWrapper();
		
		void Call(...array<Object^> ^args);
	internal:
		CEF_CALLBACK_INFO *cbInfo;
		void Cleanup();	
	private:
		bool disposed;
	};
}