#pragma once

namespace CefSharp
{
	[AttributeUsage(AttributeTargets::Class)]
	public ref class ScriptingObjectAttribute sealed : public Attribute
	{
	public:
		ScriptingObjectAttribute(void);
	};
}

