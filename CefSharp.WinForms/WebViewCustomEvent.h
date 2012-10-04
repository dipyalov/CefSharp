#pragma once

using namespace System;
using namespace System::Windows::Forms;
using namespace CefSharp;

namespace CefSharp
{
namespace WinForms
{
	public ref class WebViewCustomEvent
	{
	public:
		WebViewCustomEvent();

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

		event System::EventHandler^ EventCompleted;
	internal:
		ScriptCustomEvent^ MakeScriptEvent(Control^ control);
	private:
		String^ name;
		Object^ detail;
		Object^ tag;
		void OnEventFired();	

		ref class CustomEvent : public ScriptCustomEvent
		{
		public:
			CustomEvent(WebViewCustomEvent^ parent, Control^ control)
				: parent(parent), control(control)
			{				
			}
			virtual void OnEventFired() override;
		private:
			WebViewCustomEvent^ parent;
			Control^ control;
		};
	};
}
}