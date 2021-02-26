/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202102260038
 ********************************************************************************
 * Copyright (C) 2002-2021 by Tangram Team.   All Rights Reserved.
 * There are Three Key Features of Webruntime:
 * 1. Built-in Modern Web Browser;
 * 2. DOM Plus: DOMPlus is a natural extension of the standard DOM system.
 *    It allows the application system to support a kind of generalized web pages,
 *    which are composed of standard DOM elements and binary components supported
 *    by the application system;
 * 3. JavaScript for Application: Similar to VBA in MS office, JavaScript will
 *    become a built-in programmable language in the application system, so that
 *    the application system can be expanded and developed for the Internet based
 *    on modern javscript/Web technology.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
 *
 * CONTACT INFORMATION:
 * mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
 * https://www.tangram.dev
 *
 *******************************************************************************/

#include "../../stdafx.h"
#include "OutLookPlusEvents.h"

namespace OfficePlus
{
	namespace OutLookPlus
	{
		namespace OutLookPlusEvent
		{
			_ATL_FUNC_INFO ViewAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO ViewRemove = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };

			_ATL_FUNC_INFO SelectedAccountChange = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };

			_ATL_FUNC_INFO AutoDiscoverComplete = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };

			_ATL_FUNC_INFO ItemSend = { CC_STDCALL,VT_EMPTY,2,VT_DISPATCH,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO NewMail = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO Reminder = { CC_STDCALL,VT_EMPTY,1,VT_DISPATCH };
			_ATL_FUNC_INFO OptionsPagesAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO Startup = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO Quit = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO AdvancedSearchComplete = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO AdvancedSearchStopped = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO MAPILogonComplete = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO NewMailEx = { CC_STDCALL,VT_EMPTY,1,VT_BSTR };
			_ATL_FUNC_INFO AttachmentContextMenuDisplay = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO FolderContextMenuDisplay = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO StoreContextMenuDisplay = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO ShortcutContextMenuDisplay = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO ViewContextMenuDisplay = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO ItemContextMenuDisplay = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO ContextMenuClose = { CC_STDCALL,VT_EMPTY,1,VT_I4 };
			_ATL_FUNC_INFO ItemLoad = { CC_STDCALL,VT_EMPTY,1,VT_DISPATCH };
			_ATL_FUNC_INFO BeforeFolderSharingDialog = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };

			_ATL_FUNC_INFO FolderSwitch = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO BeforeFolderSwitch = { CC_STDCALL,VT_EMPTY,2,VT_DISPATCH,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO ViewSwitch = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO BeforeViewSwitch = { CC_STDCALL,VT_EMPTY,2,VT_VARIANT,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO SelectionChange = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO BeforeMaximize = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeMinimize = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeMove = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeItemCopy = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeItemCut = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeItemPaste = { CC_STDCALL,VT_EMPTY,3,VT_VARIANT | VT_BYREF,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO InlineResponse = { CC_STDCALL,VT_EMPTY,1,VT_DISPATCH };
			_ATL_FUNC_INFO InlineResponseClose = { CC_STDCALL,VT_EMPTY,0,NULL };

			_ATL_FUNC_INFO NewExplorer = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };

			_ATL_FUNC_INFO FolderAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO FolderChange = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO FolderRemove = { CC_STDCALL,VT_EMPTY,0,NULL };

			_ATL_FUNC_INFO Expanded = { CC_STDCALL,VT_EMPTY,1,VT_BOOL };

			_ATL_FUNC_INFO Activate = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO Deactivate = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO Close = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO BeforeSize = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO PageChange = { CC_STDCALL,VT_EMPTY,1,VT_BSTR /*| VT_BYREF*/ };
			_ATL_FUNC_INFO AttachmentSelectionChange = { CC_STDCALL,VT_EMPTY,0,NULL };

			_ATL_FUNC_INFO NewInspector = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };

			_ATL_FUNC_INFO Open = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO CustomAction = { CC_STDCALL,VT_EMPTY,3,VT_DISPATCH,VT_DISPATCH,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO CustomPropertyChange = { CC_STDCALL,VT_EMPTY,1,VT_BSTR };
			_ATL_FUNC_INFO Forward = { CC_STDCALL,VT_EMPTY,2,VT_DISPATCH,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO ItemClose = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO PropertyChange = { CC_STDCALL,VT_EMPTY,1,VT_BSTR };
			_ATL_FUNC_INFO Read = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO Reply = { CC_STDCALL,VT_EMPTY,2,VT_DISPATCH,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO ReplyAll = { CC_STDCALL,VT_EMPTY,2,VT_DISPATCH,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO Send = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO Write = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeCheckNames = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO AttachmentAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO AttachmentRead = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO BeforeAttachmentSave = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeDelete = { CC_STDCALL,VT_EMPTY,2,VT_DISPATCH,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO AttachmentRemove = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO BeforeAttachmentAdd = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeAttachmentPreview = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeAttachmentRead = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeAttachmentWriteToTempFile = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO Unload = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO BeforeAutoSave = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeRead = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO AfterWrite = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO ReadComplete = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };

			_ATL_FUNC_INFO ItemAdd = { CC_STDCALL,VT_EMPTY,1,VT_DISPATCH };
			_ATL_FUNC_INFO ItemChange = { CC_STDCALL,VT_EMPTY,1,VT_DISPATCH };
			_ATL_FUNC_INFO ItemRemove = { CC_STDCALL,VT_EMPTY,0,NULL };

			_ATL_FUNC_INFO BeforeFolderMove = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeItemMove = { CC_STDCALL,VT_EMPTY,3,VT_DISPATCH,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };

			_ATL_FUNC_INFO NameSpaceOptionsPagesAdd = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO NameSpaceAutoDiscoverComplete = { CC_STDCALL,VT_EMPTY,0,NULL };

			_ATL_FUNC_INFO SelectedChange = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO NavigationFolderAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO NavigationFolderRemove = { CC_STDCALL,VT_EMPTY,0,NULL };

			_ATL_FUNC_INFO ModuleSwitch = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };

			_ATL_FUNC_INFO Click = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO DoubleClick = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO MouseDown = { CC_STDCALL,VT_EMPTY,4,VT_I4,VT_I4,VT_I4,VT_I4 };
			_ATL_FUNC_INFO MouseMove = { CC_STDCALL,VT_EMPTY,4,VT_I4,VT_I4,VT_I4,VT_I4 };
			_ATL_FUNC_INFO MouseUp = { CC_STDCALL,VT_EMPTY,4,VT_I4,VT_I4,VT_I4,VT_I4 };
			_ATL_FUNC_INFO Enter = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO Exit = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO KeyDown = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 };
			_ATL_FUNC_INFO KeyPress = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO KeyUp = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_I4 };
			_ATL_FUNC_INFO Change = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO AfterUpdate = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO BeforeUpdate = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };

			_ATL_FUNC_INFO DropButtonClick = { CC_STDCALL,VT_EMPTY,0,NULL };

			_ATL_FUNC_INFO GroupAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO BeforeGroupAdd = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeGroupRemove = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };

			_ATL_FUNC_INFO BeforeNavigate = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeGroupSwitch = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };

			_ATL_FUNC_INFO ShortcutAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO BeforeShortcutAdd = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO BeforeShortcutRemove = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };

			_ATL_FUNC_INFO BeforeReminderShow = { CC_STDCALL,VT_EMPTY,1,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO ReminderAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO ReminderChange = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO ReminderFire = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };
			_ATL_FUNC_INFO ReminderRemove = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO Snooze = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };

			_ATL_FUNC_INFO BeforeStoreRemove = { CC_STDCALL,VT_EMPTY,2,VT_I4 | VT_BYREF,VT_BOOL | VT_BYREF };
			_ATL_FUNC_INFO StoreAdd = { CC_STDCALL,VT_EMPTY,1,VT_I4 | VT_BYREF };

			_ATL_FUNC_INFO SyncStart = { CC_STDCALL,VT_EMPTY,0,NULL };
			_ATL_FUNC_INFO Progress = { CC_STDCALL,VT_EMPTY,4,VT_I4,VT_BSTR,VT_I4,VT_I4 };
			_ATL_FUNC_INFO OnError = { CC_STDCALL,VT_EMPTY,2,VT_I4,VT_BSTR };
			_ATL_FUNC_INFO SyncEnd = { CC_STDCALL,VT_EMPTY,0,NULL };

			_ATL_FUNC_INFO BeforeViewSwitch2 = { CC_STDCALL,VT_EMPTY,2,VT_BSTR,VT_BOOL | VT_BYREF };
		}
	}
}