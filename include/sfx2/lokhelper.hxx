/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_SFX2_LOKHELPER_HXX
#define INCLUDED_SFX2_LOKHELPER_HXX

#include <vcl/IDialogRenderable.hxx>
#include <vcl/ITiledRenderable.hxx>
#include <vcl/event.hxx>
#include <sfx2/dllapi.h>
#include <sfx2/viewsh.hxx>
#include <cstddef>
#include <rtl/string.hxx>
#include <optional>

struct SFX2_DLLPUBLIC LokMouseEventData
{
    int mnType;
    Point maPosition;
    int mnCount;
    MouseEventModifiers meModifiers;
    int mnButtons;
    int mnModifier;
    std::optional<Point> maLogicPosition;

    LokMouseEventData(int nType, Point aPosition, int nCount, MouseEventModifiers eModifiers, int nButtons, int nModifier)
        : mnType(nType)
        , maPosition(aPosition)
        , mnCount(nCount)
        , meModifiers(eModifiers)
        , mnButtons(nButtons)
        , mnModifier(nModifier)
    {}
};

class SFX2_DLLPUBLIC SfxLokHelper
{
public:
    /// Create a new view shell from the current view frame.
    static int createView();
    /// Destroy a view shell from the global shell list.
    static void destroyView(int nId);
    /// Set a view shell as current one.
    static void setView(int nId);
    /// Get the currently active view.
    static int getView(SfxViewShell* pViewShell = nullptr);
    /// Get the number of views of the current object shell.
    static std::size_t getViewsCount();
    /// Get viewIds of all existing views.
    static bool getViewIds(int* pArray, size_t nSize);
    /// Get the default language that should be used for views
    static LanguageTag getDefaultLanguage();
    /// Set language of the given view.
    static void setViewLanguage(int nId, const OUString& rBcp47LanguageTag);
    /// Set the default language for views.
    static void setDefaultLanguage(const OUString& rBcp47LanguageTag);
    /// Set the locale for the given view.
    static void setViewLocale(int nId, const OUString& rBcp47LanguageTag);
    /// Iterate over any view shell, except pThisViewShell, passing it to the f function.
    template<typename ViewShellType, typename FunctionType>
    static void forEachOtherView(ViewShellType* pThisViewShell, FunctionType f);
    /// Invoke the LOK callback of all views except pThisView, with a payload of rKey-rPayload.
    static void notifyOtherViews(SfxViewShell* pThisView, int nType, const OString& rKey, const OString& rPayload);
    /// Same as notifyOtherViews(), but works on a selected "other" view, not on all of them.
    static void notifyOtherView(SfxViewShell* pThisView, SfxViewShell const* pOtherView, int nType, const OString& rKey, const OString& rPayload);
    /// Emits a LOK_CALLBACK_STATE_CHANGED
    static void sendUnoStatus(const SfxViewShell* pShell, const SfxPoolItem* pItem);
    /// Emits a LOK_CALLBACK_WINDOW
    static void notifyWindow(const SfxViewShell* pThisView,
                             vcl::LOKWindowId nWindowId,
                             const OUString& rAction,
                             const std::vector<vcl::LOKPayloadItem>& rPayload = std::vector<vcl::LOKPayloadItem>());
    /// Emits a LOK_CALLBACK_DOCUMENT_SIZE_CHANGED - if @bInvalidateAll - first invalidates all parts
    static void notifyDocumentSizeChanged(SfxViewShell const* pThisView, const OString& rPayload, vcl::ITiledRenderable* pDoc, bool bInvalidateAll = true);
    /// Emits a LOK_CALLBACK_DOCUMENT_SIZE_CHANGED for all views - if @bInvalidateAll - first invalidates all parts
    static void notifyDocumentSizeChangedAllViews(vcl::ITiledRenderable* pDoc, bool bInvalidateAll = true);
    /// Emits a LOK_CALLBACK_INVALIDATE_TILES, but tweaks it according to setOptionalFeatures() if needed.
    static void notifyInvalidation(SfxViewShell const* pThisView, const OString& rPayload);
    /// Emits a LOK_CALLBACK_INVALIDATE_VISIBLE_CURSOR, but tweaks it according to setOptionalFeatures() if needed.
    static void notifyVisCursorInvalidation(OutlinerViewShell const* pThisView, const OString& rRectangle, bool bMispelledWord = false, const OString& rHyperlink = "");
    /// Notifies all views with the given type and payload.
    static void notifyAllViews(int nType, const OString& rPayload);

    /// Notify about the editing context change.
    static void notifyContextChange(SfxViewShell const* pViewShell, const OUString& aApplication, const OUString& aContext);

    /// Helper for posting async key event
    static void postKeyEventAsync(const VclPtr<vcl::Window> &xWindow,
                                  int nType, int nCharCode, int nKeyCode, int nRepeat = 0);

    /// Helper for posting input event
    static void postExtTextEventAsync(const VclPtr<vcl::Window> &xWindow,
                                      int nType, const OUString &rText);

    /// Helper for posting async mouse event
    static void postMouseEventAsync(const VclPtr<vcl::Window> &xWindow, LokMouseEventData const & rLokMouseEventData);

    /// A special value to signify 'infinity'.
    /// This value is chosen such that sal_Int32 will not overflow when manipulated.
    static const long MaxTwips = 1e9;
};

template<typename ViewShellType, typename FunctionType>
void SfxLokHelper::forEachOtherView(ViewShellType* pThisViewShell, FunctionType f)
{
    SfxViewShell* pViewShell = SfxViewShell::GetFirst();
    while (pViewShell)
    {
        auto pOtherViewShell = dynamic_cast<ViewShellType*>(pViewShell);
        if (pOtherViewShell != nullptr && pOtherViewShell != pThisViewShell)
        {
            f(pOtherViewShell);
        }
        pViewShell = SfxViewShell::GetNext(*pViewShell);
    }
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
