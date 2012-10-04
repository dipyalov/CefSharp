#pragma once

namespace CefSharp
{
	public ref class ScriptCustomEvent
	{
	public:
		ScriptCustomEvent();

		property String^ Name 
		{
			String^ get() { return this->name; }
			void set(String^ value) { this->name = value; }
		}
		property Object^ Detail 
		{
			Object^ get() { return this->detail; }
			void set(Object^ value) { this->detail = value; }
		}

		property Object^ Tag
		{
			Object^ get() { return this->tag; }
			void set(Object^ value) { this->tag = value; }
		}

		event EventHandler^ EventFired;

		virtual void OnEventFired();
	private:
		String^ name;
		Object^ detail;
		Object^ tag;
	};
}