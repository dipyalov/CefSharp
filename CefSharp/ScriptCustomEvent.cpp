#include "StdAfx.h"
#include "ScriptCustomEvent.h"

namespace CefSharp
{
	ScriptCustomEvent::ScriptCustomEvent()
	{
	}

	void ScriptCustomEvent::OnEventFired()
	{
		this->EventFired(this, EventArgs::Empty);
	}	
}