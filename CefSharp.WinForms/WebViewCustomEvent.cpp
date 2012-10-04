#include "StdAfx.h"
#include "WebViewCustomEvent.h"

namespace CefSharp
{
namespace WinForms
{
	WebViewCustomEvent::WebViewCustomEvent()
	{
	}
	
	CefSharp::ScriptCustomEvent^ WebViewCustomEvent::MakeScriptEvent(Control^ control)
	{
		CustomEvent^ e = gcnew CustomEvent(this, control);
		e->Name = this->Name;
		e->Detail = this->Detail;		
		return e; 
	}
	
	void WebViewCustomEvent::OnEventFired()
	{
		this->EventCompleted(this, EventArgs::Empty);
	}


	void WebViewCustomEvent::CustomEvent::OnEventFired()
	{
		if (control->InvokeRequired)
		{
			control->BeginInvoke(gcnew MethodInvoker(parent, &WebViewCustomEvent::OnEventFired));
			return;
		}
		parent->OnEventFired();
	}
}
}