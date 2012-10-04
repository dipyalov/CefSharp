#include "stdafx.h"
#pragma once

using namespace System;

namespace CefSharp
{
	public interface class IDisplayHandler
	{
	public:
		bool OnTooltip(IWebBrowser^ browser, String^ text);
		void OnTitleChange(IWebBrowser^ browser, String^ title);
	};
}