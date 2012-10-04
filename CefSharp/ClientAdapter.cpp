#include "stdafx.h"

#include "BindingHandler.h"
#include "ClientAdapter.h"
#include "CefSharp.h"
#include "StreamAdapter.h"
#include "IWebBrowser.h"
#include "ILifeSpanHandler.h"
#include "ILoadHandler.h"
#include "IRequestHandler.h"
#include "IMenuHandler.h"
#include "IKeyboardHandler.h"
#include "IDisplayHandler.h"
#include "HandlerErrorCode.h"

namespace CefSharp
{
    bool ClientAdapter::OnBeforePopup(CefRefPtr<CefBrowser> parentBrowser, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, const CefString& url, CefRefPtr<CefClient>& client, CefBrowserSettings& settings)
    {
        ILifeSpanHandler^ handler = _browserControl->LifeSpanHandler;
        if (handler == nullptr)
        {
            return false;
        }

        return handler->OnBeforePopup(_browserControl, toClr(url),
            windowInfo.m_x, windowInfo.m_y, windowInfo.m_nWidth, windowInfo.m_nHeight);
    }

    void ClientAdapter::OnAfterCreated(CefRefPtr<CefBrowser> browser)
    {
        if(!browser->IsPopup())
        {
            _browserHwnd = browser->GetWindowHandle();
            _cefBrowser = browser;

            _browserControl->OnInitialized();
        }
    }

    void ClientAdapter::OnBeforeClose(CefRefPtr<CefBrowser> browser)
    {	
        if (_browserHwnd == browser->GetWindowHandle())
        {
            _cefBrowser = nullptr;
        }
    }

    void ClientAdapter::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
    {
        if (frame->IsMain())
        {
            _browserControl->Address = toClr(url);
        }
    }

    void ClientAdapter::OnContentsSizeChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int width, int height)
    {
        if (frame->IsMain())
        {
            _browserControl->ContentsWidth = width;
            _browserControl->ContentsHeight = height;
        }
    }

    void ClientAdapter::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
    {
		String^ clrtitle = toClr(title);
        _browserControl->Title = clrtitle;

		IDisplayHandler^ handler = _browserControl->DisplayHandler;
        if (handler != nullptr)
        {
            handler->OnTitleChange(_browserControl, clrtitle);
        }
    }

    bool ClientAdapter::OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text)
    {
        String^ tooltip = toClr(text);

        if (tooltip != _tooltip)
        {
            _tooltip = tooltip;
            _browserControl->TooltipText = _tooltip;
        }

		IDisplayHandler^ handler = _browserControl->DisplayHandler;
        if (handler != nullptr)
        {
            return handler->OnTooltip(_browserControl, tooltip);
        }
        return true;
    }

    bool ClientAdapter::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
    {
        String^ messageStr = toClr(message);
        String^ sourceStr = toClr(source);
        _browserControl->OnConsoleMessage(messageStr, sourceStr, line);

        return true;
    }

    bool ClientAdapter::OnKeyEvent(CefRefPtr<CefBrowser> browser, KeyEventType type, int code, int modifiers, bool isSystemKey, bool isAfterJavaScript)
    {
        IKeyboardHandler^ handler = _browserControl->KeyboardHandler;
        if (handler == nullptr)
        {
            return false;
        }

        return handler->OnKeyEvent(_browserControl, (KeyType)type, code, modifiers, isSystemKey, isAfterJavaScript);
    }

    void ClientAdapter::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
    {
        if (browser->IsPopup())
        {
            return;
        }

        AutoLock lock_scope(this);
        if (frame->IsMain())
        {
            _browserControl->SetNavState(true, false, false);
        }

        _browserControl->OnFrameLoadStart(toClr(frame->GetURL()));
    }

    void ClientAdapter::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
    {
        if(browser->IsPopup())
        {
            return;
        }

        AutoLock lock_scope(this);
        if (frame->IsMain())
        {
            _browserControl->SetNavState(false, browser->CanGoBack(), browser->CanGoForward());
        }

        _browserControl->OnFrameLoadEnd(toClr(frame->GetURL()));
    }

    bool ClientAdapter::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& failedUrl, CefString& errorText)
    {
        ILoadHandler^ handler = _browserControl->LoadHandler;
        if (handler == nullptr)
        {
            return false;
        }

        String^ errorString = nullptr;
        handler->OnLoadError(_browserControl, toClr(failedUrl), errorCode, errorString);

        if (errorString == nullptr)
        {
            return false;
        }
        else
        {
            errorText = toNative(errorString);
            return true;
        }
    }

    bool ClientAdapter::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, NavType navType, bool isRedirect)
    {
        IRequestHandler^ handler = _browserControl->RequestHandler;
        if (handler == nullptr)
        {
            return false;
        }

        CefRequestWrapper^ wrapper = gcnew CefRequestWrapper(request);
		try
		{
			NavigationType navigationType = (NavigationType)navType;

			return handler->OnBeforeBrowse(_browserControl, wrapper, navigationType, isRedirect);
		}
		finally
		{
			// release unmanaged resources
			wrapper->~CefRequestWrapper();
		}
    }

    bool ClientAdapter::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefRequest> request, CefString& redirectUrl, CefRefPtr<CefStreamReader>& resourceStream, CefRefPtr<CefResponse> response, int loadFlags)
    {
        IRequestHandler^ handler = _browserControl->RequestHandler;
        if (handler == nullptr)
        {
            return false;
        }

        CefRequestWrapper^ wrapper = gcnew CefRequestWrapper(request);
        RequestResponse^ requestResponse = gcnew RequestResponse(wrapper);
		bool ret;
		try
		{
			bool ret = handler->OnBeforeResourceLoad(_browserControl, requestResponse);

			if (requestResponse->Action == ResponseAction::Redirect)
			{
				redirectUrl = toNative(requestResponse->RedirectUrl);
			}
			else if (requestResponse->Action == ResponseAction::Respond)
			{
				CefRefPtr<StreamAdapter> adapter = new StreamAdapter(requestResponse->ResponseStream);
				resourceStream = CefStreamReader::CreateForHandler(static_cast<CefRefPtr<CefReadHandler>>(adapter));
				response->SetMimeType(toNative(requestResponse->MimeType));
			}

			return ret;
		}
		finally
		{
			// release unmanaged resources
			requestResponse->~RequestResponse();
			wrapper->~CefRequestWrapper();
		}        
    }

    void ClientAdapter::OnResourceResponse(CefRefPtr<CefBrowser> browser, const CefString& url, CefRefPtr<CefResponse> response, CefRefPtr<CefContentFilter>& filter)
    {
        IRequestHandler^ handler = _browserControl->RequestHandler;
        if (handler == nullptr)
        {
            return;
        }

        WebHeaderCollection^ headers = gcnew WebHeaderCollection();
        CefResponse::HeaderMap map;
        response->GetHeaderMap(map);
        for (CefResponse::HeaderMap::iterator it = map.begin(); it != map.end(); ++it)
        {
            try
            {
                headers->Add(toClr(it->first), toClr(it->second));
            }
            catch (Exception ^ex)
            {
                // adding a header with invalid characters can cause an exception to be
                // thrown. we will drop those headers for now.
                // we could eventually use reflection to call headers->AddWithoutValidate().
            }
        }

        handler->OnResourceResponse(
            _browserControl,
            toClr(url),
            response->GetStatus(),
            toClr(response->GetStatusText()),
            toClr(response->GetMimeType()),
            headers);
    }

    void ClientAdapter::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
    {
		CefV8Context* contextPtr = context.get();
		contextPtr->AddRef();		

		Dictionary<Object^, IntPtr>^ bindings = nullptr;
		for (int i = _contextBindings->Count - 1; i >= 0; i--)
		{
			KeyValuePair<IntPtr, Dictionary<Object^, IntPtr>^> p = _contextBindings->default[i];
			if (((CefV8Context*)p.Key.ToPointer())->IsSame(contextPtr))
			{
				bindings = p.Value;
				contextPtr->Release();
				break;
			}
		}
		if (bindings == nullptr)
		{
			_contextBindings->Add(KeyValuePair<IntPtr, Dictionary<Object^, IntPtr>^>(IntPtr(contextPtr), bindings = gcnew Dictionary<Object^, IntPtr>()));
		}

		
        for each(KeyValuePair<String^, Object^>^ kvp in CEF::GetBoundObjects())
        {
            BindingHandler::BindCached(kvp->Key, kvp->Value, context->GetGlobal(), bindings);
        }

        for each(KeyValuePair<String^, Object^>^ kvp in _browserControl->GetBoundObjects())
        {
            BindingHandler::BindCached(kvp->Key, kvp->Value, context->GetGlobal(), bindings);
        }
    }

	void ClientAdapter::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
    {
		CefV8Context *contextPtr = context.get();
		
		for (int i = _contextBindings->Count - 1; i >= 0; i--)
		{
			KeyValuePair<IntPtr, Dictionary<Object^, IntPtr>^> p = _contextBindings->default[i];
			if (!((CefV8Context*)p.Key.ToPointer())->IsSame(contextPtr))
			{
				continue;
			}
			
			((CefV8Context*)p.Key.ToPointer())->Release();
			_contextBindings->RemoveAt(i);

			Dictionary<Object^, IntPtr>^ bindings = p.Value;
			for each (KeyValuePair<Object^, IntPtr> b in bindings)
			{
				((CefV8Value*)b.Value.ToPointer())->Release();
			}
			bindings->Clear();
		}
    }

    bool ClientAdapter::OnBeforeMenu(CefRefPtr<CefBrowser> browser, const CefMenuInfo& menuInfo)
    {
        IMenuHandler^ handler = _browserControl->MenuHandler;
        if (handler == nullptr)
        {
            return false;
        }

		MenuInfo^ mi = gcnew MenuInfo(menuInfo);
		try
		{
			return handler->OnBeforeMenu(_browserControl, mi);
		}
		finally
		{
			delete mi;
		}       
    }

    void ClientAdapter::OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next)
    {
        _browserControl->OnTakeFocus(next);
    }
}