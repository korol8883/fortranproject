/***************************************************************
 * Name:      fortranproject
 * Purpose:   Code::Blocks plugin
 * Author:    Darius Markauskas, darmar.lt@gmail.com (code based on Code-completion plugin)
 * Created:   2009-07-11
 * Copyright:
 * License:   GPL
 **************************************************************/

#ifndef FORTRANPROJECT_H_INCLUDED
#define FORTRANPROJECT_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <cbplugin.h> // for "class cbToolPlugin"
#include "nativeparserf.h"
#include "keywordsparserf.h"
#include <wx/timer.h>
#include <list>
#include <map>
#include "loggers.h"
#include "finfowindow.h"
#include "lineaddress.h"


class FortranProject : public cbCodeCompletionPlugin
{
    public:

        FortranProject();
        ~FortranProject();

        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);

        void OnUpdateUI(wxUpdateUIEvent& event);
        void BuildMenu(wxMenuBar* menuBar);
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);
        void OnViewWorkspaceBrowser(wxCommandEvent& event);
        virtual bool BuildToolBar(wxToolBar* toolBar);
        virtual wxArrayString GetCallTips();
        virtual int CodeComplete();
        virtual void ShowCallTip();
        void OnCodeComplete(wxCommandEvent& event);
        void OnShowCallTip(wxCommandEvent& event);
        void CompleteCodeEvt(CodeBlocksEvent& event);
        void ShowCallTipEvt(CodeBlocksEvent& event);
        void MakeCompleteCode();


        void EditorEventHook(cbEditor* editor, wxScintillaEvent& event);

        virtual int GetConfigurationGroup() const { return cgEditor; }
        void ShowInfoLog(TokensArrayFlat* result, bool isAfterPercent);
        cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        int Configure();
        void RereadOptions();
        void CheckEnableToolbar();

        //virtual int Execute();

    private:

        void OnAppDoneStartup(CodeBlocksEvent& event);
        void OnWorkspaceChanged(CodeBlocksEvent& event);
        void OnProjectActivated(CodeBlocksEvent& event);
        void OnProjectClosed(CodeBlocksEvent& event);
        void OnProjectSaved(CodeBlocksEvent& event);
        void OnProjectFileAdded(CodeBlocksEvent& event);
        void OnProjectFileRemoved(CodeBlocksEvent& event);
        void OnCompilerStarted(CodeBlocksEvent& event);
        void OnCleanProjectStarted(CodeBlocksEvent& event);
        void OnCleanWorkspaceStarted(CodeBlocksEvent& event);
        void OnGotoDeclaration(wxCommandEvent& event);
        void OnReparseActiveEditor(CodeBlocksEvent& event);
        void OnEditorActivated(CodeBlocksEvent& event);
        void OnNextPrevCallTipPage(wxCommandEvent& event);
//        void OnEditorOpen(CodeBlocksEvent& event);
//        void OnEditorClose(CodeBlocksEvent& event);
        void OnCodeCompleteTimer(wxTimerEvent& event);
        void OnReparseEditorTimer(wxTimerEvent& event);
        void CodeCompletePreprocessor();
        void DoCodeComplete();
        void OnValueTooltip(CodeBlocksEvent& event);
        void WriteToLog(const wxString& text);
        void CreateLogWindow();
        void RemoveLogWindow(bool appShutDown);
        void OnJumpBack(wxCommandEvent& event);
        void OnJumpHome(wxCommandEvent& event);
        void OnJumpForward(wxCommandEvent& event);
        void JumpToLine(const LineAddress& adr);

        void OnDebuggerStarted(CodeBlocksEvent& event);
        void OnDebuggerFinished(CodeBlocksEvent& event);
        bool m_IsDebugging;

        void OnGenerateMakefile(wxCommandEvent& event);
        void OnChangeCase(wxCommandEvent& event);

        void RegisterFileExtensions();

        bool m_InitDone;

        NativeParserF* m_pNativeParser;

        int                                m_EditorHookId;
        int                                m_LastPosForCodeCompletion;
        wxTimer                            m_TimerCodeCompletion;
        cbEditor*                          m_pCodeCompletionLastEditor;
        wxMenu*                            m_EditMenu;
        wxMenuItem*                        m_EditMenuSeparator;
        wxMenu*                            m_ViewMenu;
        wxMenu*                            m_FortranToolsMenu;

        wxToolBar*                         m_pToolbar;

        bool                               m_ShowedCallTip;
        int                                m_CallTipPossition;

        bool                               m_WasCallTipActive;

        bool                               m_IsAutoPopup;
        int                                m_ActiveCalltipsNest;
        int                                m_CurrentLine;
        bool                               m_LexerKeywordsToInclude[9];
        bool                               m_UseSmartCC;
        size_t                             m_MaxMatch;

        bool                               m_LogUseWindow;
        bool                               m_LogComAbove;
        bool                               m_LogComBelow;
        bool                               m_LogDeclar;
        bool                               m_LogComVariab;
        bool                               m_LogOnlyUseAssoc;
        bool                               m_LogOnlyPublicNames;

        bool                               m_ComVariab;

        FInfoWindow*                       m_pFortranLog;

        KeywordsParserF*                   m_pKeywordsParser;
        wxString                           m_LastCallTipName;
        bool                               m_WasCallTipInfoLog;

        std::map<wxString,int>             m_IdxCallTipPage;
        wxTimer                            m_TimerReparseEditor;

        DECLARE_EVENT_TABLE()
};

#endif // FORTRANPROJECT_H

