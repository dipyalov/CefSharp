#include "stdafx.h"
#pragma once
#include "ScriptingObjectAttribute.h"

namespace CefSharp
{
	using namespace System;
	using namespace System::Runtime::InteropServices;  

	public interface class IScriptingObject
    {
    public:
        bool GetValue(String^ name, [Out] Object^% value);
		void SetValue(String^ name, Object^ value);
		void RemoveValue(String^ name);
    };

	[ScriptingObject]
	public ref class ScriptingPropertyContainer : public IScriptingObject
	{
	public:
		virtual bool GetValue(String^ name, [Out] Object^% value);
		virtual void SetValue(String^ name, Object^ value);
		virtual void RemoveValue(String^ name);
	};
}

