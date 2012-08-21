#include "stdafx.h"
#include "MenuInfo.h"
#pragma once

using namespace System;

namespace CefSharp
{
    public interface class IMenuHandler
    {
    public:
        bool OnBeforeMenu(IWebBrowser^ browser, MenuInfo^ menuInfo);
    };
}