#include "StdAfx.h"
#include "IScriptingObject.h"

namespace CefSharp
{
	bool ScriptingPropertyContainer::GetValue(String^ name, [Out] Object^% value)
	{
		value = nullptr;
		return false;
	}

	void ScriptingPropertyContainer::SetValue(String^ name, Object^ value)
	{

	}

	void ScriptingPropertyContainer::RemoveValue(String^ name)
	{
	}
}
