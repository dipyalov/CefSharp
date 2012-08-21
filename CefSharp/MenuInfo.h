#pragma once
namespace CefSharp
{
	[Flags]
	public enum class MenuInfoTypeBits
	{
		///
		// No node is selected
		///
		None = 0x0,
		///
		// The top page is selected
		///
		Page = 0x1,
		///
		// A subframe page is selected
		///
		Frame = 0x2,
		///
		// A link is selected
		///
		Link = 0x4,
		///
		// An image is selected
		///
		Image = 0x8,
		///
		// There is a textual or mixed selection that is selected
		///
		Selection = 0x10,
		///
		// An editable element is selected
		///
		Editable = 0x20,
		///
		// A misspelled word is selected
		///
		MisspelledWord = 0x40,
		///
		// A video node is selected
		///
		Video = 0x80,
		///
		// A video node is selected
		///
		Audio = 0x100,
	};

	[Flags]
	public enum class MenuCapabilityBits 
	{
		// Values from WebContextMenuData::EditFlags in WebContextMenuData.h
		CanDoNone = 0x0,
		CanUndo = 0x1,
		CanRedo = 0x2,
		CanCut = 0x4,
		CanCopy = 0x8,
		CanPaste = 0x10,
		CanDelete = 0x20,
		CanSelectAll = 0x40,
		CanTranslate = 0x80,
		// Values unique to CEF
		CanGoForward = 0x10000000,
		CanGoBack = 0x20000000,
	};

	public ref class MenuInfo
	{
	public:
		MenuInfo(const CefMenuInfo& info) 
		{
			_info = &info;
		}
		~MenuInfo() 
		{ 
			_info = NULL; 
		}

		property MenuInfoTypeBits TypeFlags 
		{ 
			MenuInfoTypeBits get() 
			{ 
				return (MenuInfoTypeBits)_info->typeFlags; 
			} 
		}
		property int X 
		{ 
			int get() 
			{ 
				return _info->x; 
			} 
		}
        property int Y 
		{ 
			int get() 
			{ 
				return _info->y; 
			} 
		}

        property String^ LinkUrl 
		{ 
			String^ get()
			{ 
				return toClr(_info->linkUrl); 
			} 
		}
        property String^ ImageUrl
		{ 
			String^ get() 
			{ 
				return toClr(_info->imageUrl); 
			} 
		}
        property String^ PageUrl 
		{ 
			String^ get() 
			{ 
				return toClr(_info->pageUrl); 
			} 
		}
        property String^ FrameUrl 
		{ 
			String^ get() 
			{ 
				return toClr(_info->frameUrl); 
			} 
		}
        property String^ SelectionText 
		{ 
			String^ get() 
			{ 
				return toClr(_info->selectionText); 
			} 
		}
        property String^ MisspelledWord 
		{ 
			String^ get() 
			{ 
				return toClr(_info->misspelledWord); 
			} 
		}

        property MenuCapabilityBits EditFlags 
		{ 
			MenuCapabilityBits get() 
			{ 
				return (MenuCapabilityBits)_info->editFlags; 
			} 
		}

        property String^ SecurityInfo 
		{ 
			String^ get() 
			{ 
				return toClr(_info->securityInfo); 
			} 
		}
	private:
		const CefMenuInfo *_info;
	};
}