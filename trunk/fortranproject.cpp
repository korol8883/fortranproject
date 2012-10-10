/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Author: Darius Markauskas
 *
 */

#include <sdk.h> // Code::Blocks SDK
#include "fortranproject.h"
#include "fpoptionsdlg.h"
#include "jumptracker.h"
#include "changecase.h"
#include <configurationpanel.h>
#include <manager.h>
#include <logmanager.h>
#include <projectmanager.h>
#include <cbstyledtextctrl.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <vector>

#include "editor_hooks.h"
#include "cbeditor.h"

#include <wx/event.h>


// this auto-registers the plugin
namespace
{
    PluginRegistrant<FortranProject> reg(_T("FortranProject"));
}

// empty bitmap for use as Fortran keywords icon in code-completion list
/* XPM */
static const char * fortran_keyword_xpm[] = {
"16 16 2 1",
"     c None",
".    c #04049B",
"                ",
"                ",
"   .........    ",
"   .........    ",
"   ..           ",
"   ..           ",
"   ........     ",
"   ........     ",
"   ..           ",
"   ..           ",
"   ..           ",
"   ..           ",
"   ..           ",
"                ",
"                ",
"                "};

// bitmap for other-than-Fortran keywords
// it's pretty nice actually :)
/* XPM */
static const char * unknown_keyword_xpm[] = {
"16 16 7 1",
"     c None",
".    c #FF8800",
"+    c #FF8D0B",
"@    c #FF9115",
"#    c #FFA948",
"$    c #FFC686",
"%    c #FFFFFF",
"                ",
"                ",
"      ....      ",
"    ........    ",
"   ..+@+.....   ",
"   .+#$#+....   ",
"  ..@$%$@.....  ",
"  ..+#$#+.....  ",
"  ...+@+......  ",
"  ............  ",
"   ..........   ",
"   ..........   ",
"    ........    ",
"      ....      ",
"                ",
"                "};


int idGotoDeclaration      = wxNewId();
int idCodeCompleteTimer    = wxNewId();
int idMenuCodeComplete     = wxNewId();
int idMenuShowCallTip      = wxNewId();
int idMenuNextCallTipPage  = wxNewId();
int idMenuPrevCallTipPage  = wxNewId();
int idMenuGotoDeclaration  = wxNewId();
int idViewSymbolsBrowser   = wxNewId();
int idMenuGenerateMakefile = wxNewId();
int idMenuChangeCase       = wxNewId();
int idReparseEditorTimer   = wxNewId();

#ifndef __WXMSW__
int idMenuEditPaste = XRCID("idEditPaste");
#endif

BEGIN_EVENT_TABLE(FortranProject, cbCodeCompletionPlugin)
    EVT_UPDATE_UI(idViewSymbolsBrowser, FortranProject::OnUpdateUI)
    EVT_MENU(idMenuGotoDeclaration, FortranProject::OnGotoDeclaration)
    EVT_MENU(idMenuCodeComplete, FortranProject::OnCodeComplete)
    EVT_MENU(idMenuShowCallTip, FortranProject::OnShowCallTip)
    EVT_MENU(idMenuNextCallTipPage, FortranProject::OnNextPrevCallTipPage)
    EVT_MENU(idMenuPrevCallTipPage, FortranProject::OnNextPrevCallTipPage)
    EVT_MENU(idGotoDeclaration, FortranProject::OnGotoDeclaration)
    EVT_MENU(idViewSymbolsBrowser, FortranProject::OnViewWorkspaceBrowser)
    EVT_MENU(idMenuGenerateMakefile, FortranProject::OnGenerateMakefile)
    EVT_MENU(idMenuChangeCase, FortranProject::OnChangeCase)
#ifndef __WXMSW__
    EVT_MENU(idMenuEditPaste, FortranProject::OnMenuEditPaste)
#endif
    EVT_TIMER(idCodeCompleteTimer, FortranProject::OnCodeCompleteTimer)
    EVT_TIMER(idReparseEditorTimer, FortranProject::OnReparseEditorTimer)
    EVT_TOOL(XRCID("idFortProjBack"), FortranProject::OnJumpBack)
    EVT_TOOL(XRCID("idFortProjHome"), FortranProject::OnJumpHome)
    EVT_TOOL(XRCID("idFortProjForward"), FortranProject::OnJumpForward)
END_EVENT_TABLE()

FortranProject::FortranProject() :
    m_pNativeParser(0),
    m_EditorHookId(0),
    m_TimerCodeCompletion(this, idCodeCompleteTimer),
    m_pCodeCompletionLastEditor(0),
    m_pToolbar(0L),
    m_ShowedCallTip(false),
    m_CallTipPossition(0),
    m_WasCallTipActive(false),
    m_IsAutoPopup(false),
    m_ActiveCalltipsNest(0),
    m_CurrentLine(0),
    m_pFortranLog(0L),
    m_TimerReparseEditor(this, idReparseEditorTimer)
{
    if(!Manager::LoadResource(_T("FortranProject.zip")))
    {
        NotifyMissingFile(_T("FortranProject.zip"));
    }
}

FortranProject::~FortranProject()
{
}


void FortranProject::OnAttach()
{
    m_EditMenu = 0;
    m_EditMenuSeparator = 0;
    m_ViewMenu = 0;
    m_FortranToolsMenu = 0;

    m_pNativeParser = new NativeParserF(this);
    m_pNativeParser->CreateWorkspaceBrowser();
    m_LastPosForCodeCompletion = -1;

    m_pKeywordsParser = new KeywordsParserF();

    RereadOptions();

    // hook to editors
    EditorHooks::HookFunctorBase* myhook = new EditorHooks::HookFunctor<FortranProject>(this, &FortranProject::EditorEventHook);
    m_EditorHookId = EditorHooks::RegisterHook(myhook);

    // register event sinks
    Manager* pm = Manager::Get();

    pm->RegisterEventSink(cbEVT_EDITOR_SAVE, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnReparseActiveEditor));
    pm->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnEditorActivated));
    pm->RegisterEventSink(cbEVT_EDITOR_TOOLTIP, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnValueTooltip));
    pm->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnEditorClose));

    pm->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnAppDoneStartup));
    pm->RegisterEventSink(cbEVT_WORKSPACE_CHANGED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnWorkspaceChanged));
    pm->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnProjectActivated));
    pm->RegisterEventSink(cbEVT_PROJECT_CLOSE, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnProjectClosed));
    pm->RegisterEventSink(cbEVT_PROJECT_SAVE, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnProjectSaved));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_ADDED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnProjectFileAdded));
    pm->RegisterEventSink(cbEVT_PROJECT_FILE_REMOVED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnProjectFileRemoved));
    pm->RegisterEventSink(cbEVT_COMPILER_STARTED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnCompilerStarted));
    pm->RegisterEventSink(cbEVT_CLEAN_PROJECT_STARTED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnCleanProjectStarted));
    pm->RegisterEventSink(cbEVT_CLEAN_WORKSPACE_STARTED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnCleanWorkspaceStarted));

    pm->RegisterEventSink(cbEVT_DEBUGGER_STARTED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnDebuggerStarted));
    pm->RegisterEventSink(cbEVT_DEBUGGER_FINISHED, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::OnDebuggerFinished));

    pm->RegisterEventSink(cbEVT_COMPLETE_CODE, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::CompleteCodeEvt));
    pm->RegisterEventSink(cbEVT_SHOW_CALL_TIP, new cbEventFunctor<FortranProject, CodeBlocksEvent>(this, &FortranProject::ShowCallTipEvt));

    m_IsDebugging = false;
    m_InitDone = true;
}

void FortranProject::OnRelease(bool appShutDown)
{
    // unregister hook
    // 'true' will delete the functor too
    EditorHooks::UnregisterHook(m_EditorHookId, true);

    // remove registered event sinks
    Manager::Get()->RemoveAllEventSinksFor(this);

    if (m_pNativeParser)
    {
        delete m_pNativeParser;
    }
    if (m_pKeywordsParser)
    {
        delete m_pKeywordsParser;
    }

    RemoveLogWindow(appShutDown);

    if (m_EditMenu)
    {
        m_EditMenu->Delete(idMenuCodeComplete);
        m_EditMenu->Delete(idMenuShowCallTip);
        if (m_EditMenuSeparator)
        {
            m_EditMenu->Delete(m_EditMenuSeparator);
        }
        if (m_ViewMenu)
        {
            m_ViewMenu->Delete(idViewSymbolsBrowser);
        }
    }
    if (m_FortranToolsMenu)
    {
        m_FortranToolsMenu->Delete(idMenuGotoDeclaration);
        m_FortranToolsMenu->Delete(idMenuNextCallTipPage);
        m_FortranToolsMenu->Delete(idMenuPrevCallTipPage);
        m_FortranToolsMenu->Delete(idMenuGenerateMakefile);
        m_FortranToolsMenu->Delete(idMenuChangeCase);
    }
} // end of OnRelease


void FortranProject::OnUpdateUI(wxUpdateUIEvent& event)
{
    if (m_ViewMenu)
    {
        bool isVis = IsWindowReallyShown((wxWindow*)m_pNativeParser->GetWorkspaceBrowser());
        m_ViewMenu->Check(idViewSymbolsBrowser, isVis);
    }

    event.Skip();
}


void FortranProject::OnAppDoneStartup(CodeBlocksEvent& event)
{
    if (IsAttached())
    {
        m_InitDone = false;
        // parse any projects opened through DDE or the command-line
        m_pNativeParser->ForceReparseWorkspace();
        m_InitDone = true;
    }

    if (m_pNativeParser->GetWorkspaceBrowser())
    {
        m_pNativeParser->GetWorkspaceBrowser()->UpdateSash();
    }
    event.Skip();
}

void FortranProject::OnWorkspaceChanged(CodeBlocksEvent& event)
{
    // EVT_WORKSPACE_CHANGED is a powerful event, it's sent after any project
    // has finished loading or closing. It's the *LAST* event to be sent when
    // the workspace has been changed, and it's not sent if the application is
    // shutting down. So it's the ideal time to parse files and update your
    // widgets.
    if (IsAttached() && m_InitDone && !Manager::IsAppShuttingDown())
    {
        m_InitDone = false;
        // Parse the projects
        m_pNativeParser->ForceReparseWorkspace();
        m_InitDone = true;
    }
    event.Skip();
}

void FortranProject::OnProjectActivated(CodeBlocksEvent& event)
{
    // The Class browser shouldn't be updated if we're in the middle of loading/closing
    // a project/workspace, because the class browser would need to be updated again.
    // So we need to update it with the EVT_WORKSPACE_CHANGED event, which gets
    // triggered after everything's finished loading/closing.

    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone)
    {
        m_pNativeParser->OnProjectActivated(event.GetProject());
    }
    event.Skip();
}

void FortranProject::OnProjectClosed(CodeBlocksEvent& event)
{
    // After this, the Class Browser needs to be updated. It will happen
    // when we receive the next EVT_PROJECT_ACTIVATED event.
//    if (IsAttached() && m_InitDone)
//    {
//        m_pNativeParser->RemoveFromParser(event.GetProject());
//    }
    event.Skip();
}

void FortranProject::OnProjectSaved(CodeBlocksEvent& event)
{
    // Do we need it for Fortran?
    event.Skip();
}

void FortranProject::OnProjectFileAdded(CodeBlocksEvent& event)
{
    if (IsAttached() && m_InitDone)
    {
        m_pNativeParser->AddFileToParser(event.GetString());
        m_pNativeParser->UpdateWorkspaceBrowser();
    }
    event.Skip();
}

void FortranProject::OnProjectFileRemoved(CodeBlocksEvent& event)
{
    if (IsAttached() && m_InitDone)
    {
        m_pNativeParser->RemoveFileFromParser(event.GetString());
        m_pNativeParser->UpdateWorkspaceBrowser();
    }
    event.Skip();
}

void FortranProject::OnReparseActiveEditor(CodeBlocksEvent& event)
{
    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone)
    {
        EditorBase* ed = event.GetEditor();
        if (!ed)
            return;

        m_pNativeParser->ReparseFile(ed->GetFilename());
        m_pNativeParser->UpdateWorkspaceBrowser();
    }
    event.Skip();
}

void FortranProject::OnEditorActivated(CodeBlocksEvent& event)
{
    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone)
    {
        EditorBase* eb = event.GetEditor();
        m_pNativeParser->OnEditorActivated(eb);

        if (m_TimerReparseEditor.IsRunning())
            m_TimerReparseEditor.Stop();
        cbEditor* editor = (eb && eb->IsBuiltinEditor()) ? static_cast<cbEditor*>(eb) : 0;
        if (editor && editor->GetModified())
            m_TimerReparseEditor.Start(1500, wxTIMER_ONE_SHOT);
    }

    event.Skip();
}

void FortranProject::OnEditorClose(CodeBlocksEvent& event)
{
    if (!ProjectManager::IsBusy() && IsAttached() && m_InitDone)
    {
        EditorBase* eb = event.GetEditor();
        m_pNativeParser->OnEditorClose(eb);
    }

    event.Skip();
}

void FortranProject::OnCompilerStarted(CodeBlocksEvent& event)
{
    event.Skip();
    m_pNativeParser->UpdateWorkspaceFilesDependency();
}

void FortranProject::OnCleanProjectStarted(CodeBlocksEvent& event)
{
    event.Skip();
    //Remove all *.mod files from obj folder
    wxString targetName = event.GetBuildTargetName();
    cbProject* pr = event.GetProject();
    if (!pr)
        return;
    if (pr->IsMakefileCustom())
        return;
    ProjectBuildTarget* bTarget = pr->GetBuildTarget(targetName);
    if (bTarget)
    {
        ProjectDependencies::RemoveModFiles(pr, bTarget, m_pNativeParser);
    }
}

void FortranProject::OnCleanWorkspaceStarted(CodeBlocksEvent& event)
{
    event.Skip();
    //Remove all *.mod files from obj folder
    ProjectDependencies::RemoveModFilesWS(m_pNativeParser);
}

void FortranProject::BuildMenu(wxMenuBar* menuBar)
{
    if (!IsAttached())
        return;

    // No code completion if CodeCompletion plugin is active
    cbPlugin* ccplug = Manager::Get()->GetPluginManager()->FindPluginByName(_T("CodeCompletion"));

    int pos = menuBar->FindMenu(_("&Edit"));
    if (pos != wxNOT_FOUND)
    {
        if (!ccplug) // don't create menu items if CC plugin found.
        {
            m_EditMenu = menuBar->GetMenu(pos);
            m_EditMenuSeparator = m_EditMenu->AppendSeparator();
            m_EditMenu->Append(idMenuCodeComplete, _("Complete code\tCtrl-Space"));
            m_EditMenu->Append(idMenuShowCallTip, _("Show call tip\tCtrl-Shift-Space"));
        }
    }
    else
        Manager::Get()->GetLogManager()->DebugLog(_T("FortranProject: Could not find Edit menu!"));

    // add the fsymbolsbrowser window in the "View" menu
    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        m_ViewMenu = menuBar->GetMenu(idx);
        wxMenuItemList& items = m_ViewMenu->GetMenuItems();
        bool inserted = false;

        // find the first separator and insert before it
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                m_ViewMenu->InsertCheckItem(i, idViewSymbolsBrowser, _("FSymbols browser"), _("Toggle displaying the fortran symbols browser"));
                inserted = true;
                break;
            }
        }

        // not found, just append
        if (!inserted)
            m_ViewMenu->AppendCheckItem(idViewSymbolsBrowser, _("FSymbols browser"), _("Toggle displaying the fortran symbols browser"));
    }
    else
        Manager::Get()->GetLogManager()->DebugLog(_T("FortranProject: Could not find View menu!"));


    pos = menuBar->FindMenu(_("&Fortran"));
    if (pos == wxNOT_FOUND)
    {
        pos = menuBar->FindMenu(_("&Tools"));
        if (pos != wxNOT_FOUND)
        {
            m_FortranToolsMenu = new wxMenu();
            menuBar->Insert(pos, m_FortranToolsMenu, _("&Fortran"));
        }
        else
            Manager::Get()->GetLogManager()->DebugLog(_T("FortranProject: Could not find Tools menu!"));
    }
    else
    {
        m_FortranToolsMenu = menuBar->GetMenu(pos);
    }
    if (m_FortranToolsMenu)
    {
        m_FortranToolsMenu->Append(idMenuGotoDeclaration, _("Jump to declaration\tCtrl-."));
        m_FortranToolsMenu->Append(idMenuNextCallTipPage, _("Next call tip\tCtrl-DOWN"));
        m_FortranToolsMenu->Append(idMenuPrevCallTipPage, _("Prev call tip\tCtrl-UP"));
        m_FortranToolsMenu->Append(idMenuGenerateMakefile, _("Generate Makefile"));
        m_FortranToolsMenu->Append(idMenuChangeCase, _("Change case"));
    }
}


// invariant : on return true : NameUnderCursor is NOT empty
bool EditorHasNameUnderCursor(wxString& NameUnderCursor)
{
    bool ReturnValue = false;
    if(cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor())
    {
        cbStyledTextCtrl* control = ed->GetControl();
        const int pos = control->GetCurrentPos();

        const int ws = control->WordStartPosition(pos, true);
        const int we = control->WordEndPosition(pos, true);
        const wxString txt = control->GetTextRange(ws, we);
        if (!txt.IsEmpty())
        {
            NameUnderCursor = txt;
            ReturnValue = true;
        }
    }
    return ReturnValue;
} // end of EditorHasNameUnderCursor

void FortranProject::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    if (!menu || !IsAttached() || !m_InitDone)
        return;
    if (type == mtEditorManager)
    {
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (!ed || !m_pNativeParser->IsFileFortran(ed->GetFilename()))
            return;

        wxString NameUnderCursor;
        if(EditorHasNameUnderCursor(NameUnderCursor))
        {
            wxString msg;
            msg.Printf(_("Jump to '%s'"), NameUnderCursor.c_str());
            menu->Insert(0, idGotoDeclaration, msg);

            menu->Insert(1, wxID_SEPARATOR, wxEmptyString);
        }
    }

}

void FortranProject::OnGotoDeclaration(wxCommandEvent& event)
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    wxString NameUnderCursor;
    if(!EditorHasNameUnderCursor(NameUnderCursor))
    {
        return;
    }

    // get the matching set
    ParserF* pParser = m_pNativeParser->GetParser();
    TokensArrayFlatClass tokensTmp;
    TokensArrayFlat* result = tokensTmp.GetTokens();

    wxString includeFilename = GetIncludeFilename(ed->GetControl());
    if (!includeFilename.IsEmpty())
    {
        // was asked to jump to include file.
        pParser->FindFile(includeFilename, *result);
    }
    else
    {
        pParser->FindMatchTokensForJump(ed, m_LogOnlyUseAssoc, false, *result);
        // don't jump to intrinsic module
        size_t ri = 0;
        while (ri<result->GetCount())
        {
            if (result->Item(ri)->m_Filename.EndsWith(UnixFilename(_T("/images/fortranproject/fortran_intrinsic_modules.f90"))))
            {
                result->Item(ri)->Clear();
                delete result->Item(ri);
                result->RemoveAt(ri);
            }
            else
                ri++;
        }
    }

    size_t count = std::min(result->GetCount(),m_MaxMatch);

    TokenFlat* pToken = 0;
    // one match
    if (count == 1)
    {
		pToken = result->Item(0);
    }
    // if more than one match, display a selection dialog
    else if (count > 1)
    {
        wxArrayString selections;
        std::vector<int> idxItems;
        for (size_t i=0; i<count; ++i)
        {
            wxFileName fn = wxFileName(result->Item(i)->m_Filename);
            wxString inf;
            if (result->Item(i)->m_TokenKind == tkUse && !result->Item(i)->m_Rename.IsEmpty())
            {
                inf = _T("use :: ") + result->Item(i)->m_DisplayName + _T(", ") + result->Item(i)->m_Rename;
            }
            else
            {
                inf = result->Item(i)->m_DisplayName + _T(" :: ") + result->Item(i)->GetTokenKindString();
            }
            inf += _T(" : ") + fn.GetFullName() + _T(" : ");
            inf += wxString::Format(_T("%d"), result->Item(i)->m_LineStart);

            if (selections.Index(inf) == wxNOT_FOUND)
            {
                selections.Add(inf);
                idxItems.push_back(i);
            }
        }
        if (selections.Count() > 1)
        {
            int sel = wxGetSingleChoiceIndex(_("Please make a selection:"), _("Multiple matches"), selections);
            if (sel == -1)
                return;
            pToken = result->Item(idxItems[sel]);
        }
        else
        {
            pToken = result->Item(0);
        }
    }

    if (pToken)
    {
        LineAddress jumpStart;
        LineAddress jumpFinish;
        if(ed)
        {
            cbStyledTextCtrl* control = ed->GetControl();
            int curLine = control->LineFromPosition(control->GetCurrentPos());
            jumpStart.Init(ed->GetFilename(), curLine, false);
        }

        if (cbEditor* ed = Manager::Get()->GetEditorManager()->Open(pToken->m_Filename))
        {
            ed->GotoLine(pToken->m_LineStart - 1);

            // Track jump history
            cbStyledTextCtrl* control = ed->GetControl();
            int curLine = control->LineFromPosition(control->GetCurrentPos());
            jumpFinish.Init(ed->GetFilename(), curLine, true);
            m_pNativeParser->GetJumpTracker()->TakeJump(jumpStart, jumpFinish);
            CheckEnableToolbar();
        }
        else
        {
            cbMessageBox(wxString::Format(_("Declaration not found: %s"), NameUnderCursor.c_str()), _("Warning"), wxICON_WARNING);
        }
    }
    else
    {
        cbMessageBox(wxString::Format(_("Not found: %s"), NameUnderCursor.c_str()), _("Warning"), wxICON_WARNING);
    }

} // end of OnGotoDeclaration


void FortranProject::CodeCompletePreprocessor()
{
    if (!IsAttached() || !m_InitDone)
        return;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    cbStyledTextCtrl* control = ed->GetControl();
    const int curPos = control->GetCurrentPos();
    const int start = control->WordStartPosition(curPos, true);
    const int end = control->WordEndPosition(curPos, true);

    wxArrayString tokens;
    tokens.Add(_T("include"));
    tokens.Add(_T("if"));
    tokens.Add(_T("ifdef"));
    tokens.Add(_T("ifndef"));
    tokens.Add(_T("elif"));
    tokens.Add(_T("elifdef"));
    tokens.Add(_T("elifndef"));
    tokens.Add(_T("else"));
    tokens.Add(_T("endif"));
    tokens.Add(_T("define"));
    tokens.Add(_T("undef"));
    tokens.Add(_T("pragma"));
    tokens.Add(_T("error"));
    tokens.Add(_T("line"));
    tokens.Sort();
    ed->GetControl()->ClearRegisteredImages();
    ed->GetControl()->AutoCompSetIgnoreCase(false);
    ed->GetControl()->AutoCompShow(end - start, GetStringFromArray(tokens, _T(" ")));
}


void FortranProject::DoCodeComplete()
{
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return;

    cbStyledTextCtrl* control = ed->GetControl();
    const int pos = control->GetCurrentPos();
    const int lineIndentPos = control->GetLineIndentPosition(control->GetCurrentLine());
    const wxChar lineFirstChar = control->GetCharAt(lineIndentPos);

    int lineCur = control->LineFromPosition(pos);
    int lineStartPos = control->PositionFromLine(lineCur);
    wxString curLine = control->GetTextRange(lineStartPos,pos).Trim(false);

    if (lineFirstChar == _T('#'))
    {
        const int end = control->WordEndPosition(lineIndentPos + 1, true);
        if (end >= pos)
            CodeCompletePreprocessor();
        return;
    }
    else if (lineFirstChar == _T('!'))
    {
        wxString curLineLw = curLine.Lower();
        if (!curLineLw.StartsWith(_T("!$ ")) && !curLineLw.StartsWith(_T("!$\t")) && !curLineLw.StartsWith(_T("!$omp")) && !curLineLw.StartsWith(_T("!$acc")))
            return;
    }
    else
    {
        if (curLine.Find('!') != wxNOT_FOUND)
            return;
    }

    int style = control->GetStyleAt(control->GetCurrentPos());
    if (style != wxSCI_F_DEFAULT && style != wxSCI_F_WORD && style != wxSCI_F_WORD2 && style != wxSCI_F_WORD3
        && style != wxSCI_F_OPERATOR && style != wxSCI_F_IDENTIFIER && style != wxSCI_F_OPERATOR2
        && style != wxSCI_F_PREPROCESSOR )
        return;

    CodeComplete();
}


void FortranProject::OnCodeCompleteTimer(wxTimerEvent& event)
{
    if (Manager::Get()->GetEditorManager()->FindPageFromEditor(m_pCodeCompletionLastEditor) == -1)
        return; // editor is invalid (probably closed already)

    // ask for code-completion *only* if the editor is still after the "%" operator
    if (m_pCodeCompletionLastEditor->GetControl()->GetCurrentPos() == m_LastPosForCodeCompletion)
    {
        DoCodeComplete();
        m_LastPosForCodeCompletion = -1; // reset it
    }
}


void FortranProject::EditorEventHook(cbEditor* editor, wxScintillaEvent& event)
{
    if (!IsAttached() || !m_InitDone)
    {
        event.Skip();
        return;
    }

    if (!m_pNativeParser->IsFileFortran(editor->GetShortName()))
    {
        event.Skip();
        return;
    }

    cbStyledTextCtrl* control = editor->GetControl();

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));
    if (!cfg->ReadBool(_T("/use_code_completion"), true))
    {
        event.Skip();
        return;
    }

    if (((event.GetKey() == '.') || (event.GetKey() == '%') || (event.GetKey() == '(')) && control->AutoCompActive())
        control->AutoCompCancel();

    if (event.GetEventType() == wxEVT_SCI_AUTOCOMP_SELECTION)
    {
        wxString itemText = event.GetText();
        control->AutoCompCancel();
        int pos = control->GetCurrentPos();
        int start = control->WordStartPosition(pos, true);
        control->SetTargetStart(start);
        control->SetTargetEnd(pos);
        control->ReplaceTarget(itemText);
        pos = control->GetCurrentPos();
        control->GotoPos(pos + itemText.size());
    }

    if (m_WasCallTipActive && !control->AutoCompActive())
    {
        m_WasCallTipActive = false;
        ShowCallTip();
    }

    if (   (event.GetEventType() == wxEVT_SCI_CHARADDED)
        && (!control->AutoCompActive()) ) // not already active autocompletion
    {
        // a character was just added in the editor
        m_TimerCodeCompletion.Stop();
        wxChar ch = event.GetKey();
        int pos = control->GetCurrentPos();
        int wordstart = control->WordStartPosition(pos, true);

        // if more than 2 chars have been typed, invoke CC
        int autoCCchars = cfg->ReadInt(_T("/auto_launch_chars"), 2);
        bool autoCC = cfg->ReadBool(_T("/auto_launch"), true) &&
                    pos - wordstart >= autoCCchars;

        // update calltip highlight while we type
        if (control->CallTipActive())
            ShowCallTip();
        else
            m_ActiveCalltipsNest = 0;

        // start calltip
        if (ch == _T('('))
        {
            if (control->CallTipActive())
                ++m_ActiveCalltipsNest;
            ShowCallTip();
        }
        // end calltip
        else if (ch == _T(')'))
        {
            // cancel any active calltip
            control->CallTipCancel();
            if (m_ActiveCalltipsNest > 0)
            {
                --m_ActiveCalltipsNest;
                ShowCallTip();
            }
        }
        else if (autoCC ||
            (ch == _T('%')))
        {
            int style = control->GetStyleAt(pos);
            if (style != wxSCI_F_DEFAULT && style != wxSCI_F_OPERATOR && style != wxSCI_F_IDENTIFIER
                && style != wxSCI_F_OPERATOR2 && style != wxSCI_F_PREPROCESSOR)
            {
                event.Skip();
                return;
            }

            int timerDelay = cfg->ReadInt(_T("/cc_delay"), 500);
            if (autoCC || timerDelay == 0)
            {
                if (autoCC)
                    m_IsAutoPopup = true;
                DoCodeComplete();
                if (autoCC)
                    m_IsAutoPopup = false;
            }
            else
            {
                m_LastPosForCodeCompletion = pos;
                m_pCodeCompletionLastEditor = editor;
                m_TimerCodeCompletion.Start(timerDelay, wxTIMER_ONE_SHOT);
            }
        }
    }
    else if( m_ShowedCallTip && control->CallTipActive() &&
             (m_CallTipPossition != control->GetCurrentPos()) &&
             (m_CurrentLine == control->GetCurrentLine()) &&
             (!control->AutoCompActive()) )
    {
        ShowCallTip();
    }

    if( control->GetCurrentLine() != m_CurrentLine )
    {
        m_CurrentLine = control->GetCurrentLine();
        m_pNativeParser->MarkCurrentSymbol();
    }

    if (event.GetEventType() == wxEVT_SCI_MODIFIED && !m_TimerReparseEditor.IsRunning())
        m_TimerReparseEditor.Start(1500, wxTIMER_ONE_SHOT);

    // allow others to handle this event
    event.Skip();
}


void FortranProject::OnViewWorkspaceBrowser(wxCommandEvent& event)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));
    if (!cfg->ReadBool(_T("/use_symbols_browser"), true))
    {
        cbMessageBox(_("The Fortran symbols browser is disabled in FortranProject options.\n"
                        "Please enable it there first..."), _("Information"), wxICON_INFORMATION);
        return;
    }
    CodeBlocksDockEvent evt(event.IsChecked() ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = (wxWindow*)m_pNativeParser->GetWorkspaceBrowser();
    Manager::Get()->ProcessEvent(evt);
}


bool FortranProject::BuildToolBar(wxToolBar* toolBar)
{
    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    //NotImplemented(_T("FortranProject::BuildToolBar()"));

    //Build toolbar
    if (!IsAttached() || !toolBar)
    {
        return false;
    }
    wxString is16x16 = Manager::isToolBar16x16(toolBar) ? _T("_16x16") : _T("");
    Manager::Get()->AddonToolBar(toolBar,_T("fortran_project_toolbar") + is16x16);
    toolBar->Realize();
    m_pToolbar = toolBar;
    m_pToolbar->EnableTool(XRCID("idFortProjBack"), false);
    m_pToolbar->EnableTool(XRCID("idFortProjHome"), false);
    m_pToolbar->EnableTool(XRCID("idFortProjForward"), false);
    m_pToolbar->SetInitialSize();

    return true;
}

wxArrayString FortranProject::GetCallTips()
{
    //NotImplemented(_T("FortranProject::GetCallTips()"));
    wxArrayString str;
    return str;
}

void FortranProject::OnCodeComplete(wxCommandEvent& event)
{
    event.Skip();
    MakeCompleteCode();
}

void FortranProject::CompleteCodeEvt(CodeBlocksEvent& event)
{
    event.Skip();
    MakeCompleteCode();
}

void FortranProject::MakeCompleteCode()
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed
        || !m_pNativeParser->IsFileFortran(ed->GetShortName())
        || !Manager::Get()->GetConfigManager(_T("fortran_project"))->ReadBool(_T("/use_code_completion"), true))
    {
        return;
    }
    if (IsAttached() && m_InitDone)
        DoCodeComplete();
}

void FortranProject::OnShowCallTip(wxCommandEvent& event)
{
    if (IsAttached() && m_InitDone)
        ShowCallTip();
    event.Skip();
}

void FortranProject::ShowCallTipEvt(CodeBlocksEvent& event)
{
    if (IsAttached() && m_InitDone)
        ShowCallTip();
    event.Skip();
}

static int SortCCList(const wxString& first, const wxString& second)
{
    const wxChar* a = first.c_str();
    const wxChar* b = second.c_str();
    while (*a && *b)
    {
        if (*a != *b)
        {
            if      ((*a == _T('?')) && (*b != _T('?')))
                return -1;
            else if ((*a != _T('?')) && (*b == _T('?')))
                return 1;
            else if ((*a == _T('?')) && (*b == _T('?')))
                return 0;

            if      ((*a == _T('_')) && (*b != _T('_')))
                return 1;
            else if ((*a != _T('_')) && (*b == _T('_')))
                return -1;

            wxChar lowerA = wxTolower(*a);
            wxChar lowerB = wxTolower(*b);

            if (lowerA != lowerB)
                return lowerA - lowerB;
        }
        a++;
        b++;
    }
    // Either *a or *b is null
    return *a - *b;
}


int FortranProject::CodeComplete()
{
    if (!IsAttached() || !m_InitDone)
        return -1;

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));
    EditorManager* edMan = Manager::Get()->GetEditorManager();
    cbEditor* ed = edMan->GetBuiltinActiveEditor();
    if (!ed)
        return -3;

    FileType ft = FileTypeOf(ed->GetShortName());

    ParserF* pParser = m_pNativeParser->GetParser();
    TokensArrayFlatClass tokensTmp;
    TokensArrayFlat* result = tokensTmp.GetTokens();

    cbStyledTextCtrl* control = ed->GetControl();
    const int pos   = control->GetCurrentPos();
    const int start = control->WordStartPosition(pos, true);
    wxString NameUnderCursor = control->GetTextRange(start,pos);
    wxString NameUnderCursorLw = NameUnderCursor.Lower();

    bool isDirective = false;
    CompilerDirective pdir;
    int lineCur = control->LineFromPosition(pos);
    int lineStartPos = control->PositionFromLine(lineCur);
    wxString curLine = control->GetTextRange(lineStartPos,pos).Trim(false).Lower();

    if (curLine.StartsWith(_T("!$")))
    {
        isDirective = true;
        if (curLine.StartsWith(_T("!$omp")))
            pdir = cdOpenMP;
        else if (curLine.StartsWith(_T("!$acc")))
            pdir = cdOpenACC;
        else
            pdir = cdOther;
    }

    bool isAfterPercent;
    int tokenKind;

    if (!pParser->FindMatchTokensForCodeCompletion(m_UseSmartCC, m_LogOnlyUseAssoc, m_LogOnlyPublicNames, NameUnderCursor, ed, *result, isAfterPercent, tokenKind))
        return -1;

    if (result->size() <= m_MaxMatch)
    {
        wxImageList* ilist = m_pNativeParser->GetImageList();
        if (!ilist)
            return -6;
        control->ClearRegisteredImages();

        wxArrayString items; items.Alloc(result->size());
        wxArrayInt already_registered;
        std::set< wxString, std::less<wxString> > unique_strings; // check against this before inserting a new string in the list
        for (size_t i=0; i<result->GetCount(); ++i)
        {
            TokenF* token = result->Item(i);
            if (token->m_Name.StartsWith(_T("%%")) || token->m_Name.IsEmpty())
                continue;
            // check for unique_strings
            if (unique_strings.find(token->m_Name) != unique_strings.end())
                continue;

            unique_strings.insert(token->m_Name);
            int iidx = m_pNativeParser->GetTokenKindImageIdx(token);
            if (already_registered.Index(iidx) == wxNOT_FOUND)
            {
                if (iidx != -1)
                {
                    control->RegisterImage(iidx, ilist->GetBitmap(iidx));
                    already_registered.Add(iidx);
                }
            }
            wxString tmp;
            if (iidx != -1)
                tmp << token->m_DisplayName << wxString::Format(_T("?%d"), iidx);
            else
                tmp << token->m_DisplayName;
            items.Add(tmp);
        }

        EditorColourSet* theme = ed->GetColourSet();
        if (theme && !isAfterPercent && (!isDirective || (isDirective && pdir == cdOther)) )
        {
            int iidx = ilist->GetImageCount();
            bool isF = ft==ftSource;
            control->RegisterImage(iidx, wxBitmap(isF ? fortran_keyword_xpm : unknown_keyword_xpm));
            // theme keywords
            HighlightLanguage lang = theme->GetLanguageForFilename(_T(".")+wxFileName(ed->GetFilename()).GetExt());

            int kwcase = cfg->ReadInt(_T("/keywords_case"), 0);
            for (int i = 0; i <= wxSCI_KEYWORDSET_MAX; ++i)
            {
                if (!m_LexerKeywordsToInclude[i])
                    continue;

                wxString keywords = theme->GetKeywords(lang, i);
                wxStringTokenizer tkz(keywords, _T(" \t\r\n"), wxTOKEN_STRTOK);
                while (tkz.HasMoreTokens())
                {
                    wxString kw = tkz.GetNextToken();

                    if ( (m_UseSmartCC && kw.Lower().StartsWith(NameUnderCursorLw) && m_pKeywordsParser->HasTokenSuitableKind(kw,tokenKind))
                         || (!m_UseSmartCC && kw.Lower().StartsWith(NameUnderCursorLw)) )
                    {
                        switch (kwcase)
                        {
                            case 0:
                            {
                                break;
                            }
                            case 1:
                            {
                                kw = kw.MakeUpper();
                                break;
                            }
                            case 2:
                            {
                                kw = kw.Mid(0,1).MakeUpper() + kw.Mid(1).MakeLower();
                                break;
                            }
                            default :
                            {
                                kw = kw.MakeLower();
                                break;
                            }
                        }
                        kw.Append(wxString::Format(_T("?%d"), iidx));
                        items.Add(kw);
                    }
                }
            }
        }
        else if (isDirective && (pdir == cdOpenMP || pdir == cdOpenACC))
        {
            int iidx = ilist->GetImageCount();
            control->RegisterImage(iidx, wxBitmap(fortran_keyword_xpm));

            int kwcase = cfg->ReadInt(_T("/keywords_case"), 0);
            const wxArrayString* kwOMP = m_pKeywordsParser->GetKeywords(pdir);
            for (size_t i=0; i<kwOMP->size(); i++)
            {
                wxString kw = kwOMP->Item(i);

                if (kw.Lower().StartsWith(NameUnderCursorLw))
                {
                    switch (kwcase)
                    {
                        case 0:
                        {
                            break;
                        }
                        case 1:
                        {
                            kw = kw.MakeUpper();
                            break;
                        }
                        case 2:
                        {
                            kw = kw.Mid(0,1).MakeUpper() + kw.Mid(1).MakeLower();
                            break;
                        }
                        default :
                        {
                            kw = kw.MakeLower();
                            break;
                        }
                    }
                    kw.Append(wxString::Format(_T("?%d"), iidx));
                    items.Add(kw);
                }
            }
        }

        if (items.GetCount() == 0)
        {
            return -2;
        }
        items.Sort(SortCCList);

        // Remove duplicate items
        size_t i=0;
        size_t count=items.Count() - 1;
        while (i < count)
        {
            if (items.Item(i)==items.Item(i+1))
            {
                items.RemoveAt(i);
                count--;
            }
            else
                i++;
        }

        if (control->CallTipActive())
        {
            m_WasCallTipActive = true;
        }

        control->AutoCompSetIgnoreCase(true);
        control->AutoCompSetCancelAtStart(true);
        control->AutoCompSetFillUps(wxEmptyString);
        control->AutoCompSetChooseSingle(cfg->ReadBool(_T("/auto_select_one"), false));
        control->AutoCompSetAutoHide(true);
        control->AutoCompSetDropRestOfWord(m_IsAutoPopup ? false : true);
        wxString final = GetStringFromArray(items, _T(" "));
        final.Trim(); //  remove last space

        control->AutoCompShow(pos - start, final);

        return 0;
    }
    else if (!control->CallTipActive())
    {
        wxString msg = _("Too many results.\n"
                         "Please type at least one more character to narrow the scope down.");
        control->CallTipShow(control->GetCurrentPos(), msg);
        return -2;
    }

    return -5;
}

void FortranProject::ShowCallTip()
{
    if (!IsAttached() || !m_InitDone)
        return;

    if (!Manager::Get()->GetEditorManager())
        return;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    // calculate the size of the calltips window
    int pos = ed->GetControl()->GetCurrentPos();
    wxPoint p = ed->GetControl()->PointFromPosition(pos); // relative point
    int pixelWidthPerChar = ed->GetControl()->TextWidth(wxSCI_STYLE_LINENUMBER, _T("W"));
    int maxCalltipLineSizeInChars = (ed->GetSize().x - p.x) / pixelWidthPerChar;
    if (maxCalltipLineSizeInChars < 64)
    {
        // if less than a threshold in chars, recalculate the starting position (instead of shrinking it even more)
        p.x -= (64 - maxCalltipLineSizeInChars) * pixelWidthPerChar;
        // but if it goes out of range, continue shrinking
        if (p.x >= 0)
        {
            maxCalltipLineSizeInChars = 64;
            pos = ed->GetControl()->PositionFromPoint(p);
        }
        // else, out of range
    }

    int start = 0;
    int end = 0;
    int commas; // how many commas has the user typed so far?
    int commasPos; // how many commas until current position?
    bool isempt;
    wxArrayString callTipsOneLine;
    wxArrayInt idxFuncSub;
    TokensArrayFlatClass tokensTmp;
    TokensArrayFlat* result = tokensTmp.GetTokens();
    TokenFlat* token;
    bool isAfterPercent = false;

    wxString lastName;
    m_pNativeParser->CollectInformationForCallTip(commas, commasPos, lastName, isempt, isAfterPercent, result);

    if (isAfterPercent)
    {
        if (result->GetCount() > 0 && result->Item(0)->m_TokenKind == tkProcedure)
        {
            m_pNativeParser->GetCallTipsForTypeBoundProc(result, callTipsOneLine);
            idxFuncSub.Add(1);
        }
        else if (result->GetCount() > 0 && result->Item(0)->m_TokenKind == tkInterface)
            m_pNativeParser->GetCallTipsForGenericTypeBoundProc(result, callTipsOneLine, idxFuncSub);
    }
    else if (!lastName.IsEmpty())
    {
        m_pNativeParser->GetCallTips(lastName, m_LogOnlyUseAssoc, m_LogOnlyPublicNames, callTipsOneLine, result);
        m_pKeywordsParser->GetCallTips(lastName, callTipsOneLine, result);
    }
    wxArrayString callTips;
    for (unsigned int i = 0; i < callTipsOneLine.GetCount(); ++i)
    {
        wxString s;
        m_pNativeParser->BreakUpInLines(s, callTipsOneLine.Item(i), maxCalltipLineSizeInChars);
        callTips.Add(s);
    }

    bool isUnique = true;
    wxString definition;
    for (unsigned int i = 0; i < callTips.GetCount(); ++i)
    {
        bool empOk = true;
        if (callTips[i].IsSameAs(_T("()")) && !isempt)
            empOk = false;

        if (!callTips[i].IsEmpty() && // non-empty
            empOk)
        {
            if (!definition.IsEmpty())
            {
                isUnique = false;
                break;
            }
            definition << callTips[i];
            token = result->Item(i);

            int nCommas = m_pNativeParser->CountCommas(callTips[i], 1, false);
            int commasDif = commas - nCommas;
            if (commasDif > 0)
            {
                for (int i=0; i< commasDif; i++)
                {
                    definition << _T(", *???*");
                }
                definition << _T(" ");
            }
        }
    }

    if (!definition.IsEmpty() && isUnique && (!isAfterPercent || ( isAfterPercent && result->GetCount() >= 2 && (result->Item(0)->m_TokenKind == tkProcedure) )))
    {
        m_pNativeParser->GetCallTipHighlight(definition, commasPos, start, end);
        if (isAfterPercent)
            token = result->Item(1);

        if (token->m_TokenKind == tkSubroutine || token->m_TokenKind == tkFunction)
        {
            wxString argName = definition.Mid(start,end-start);
            argName = argName.BeforeFirst(_T(','));
            argName = argName.BeforeFirst(_T(')'));
            argName.Replace(_T("["),_T(" "));
            argName.Replace(_T("]"),_T(" "));
            argName.Trim().Trim(false);
            if (isUnique)
            {
                wxString argDecl;
                wxString argDescription;
                bool found = m_pNativeParser->GetParser()->FindTokenDeclaration(*token, argName, argDecl, argDescription);
                if (!found)
                    found = m_pKeywordsParser->GetParser()->FindTokenDeclaration(*token, argName, argDecl, argDescription);
                if (found)
                {
                    definition << _T('\n') << argDecl;
                    if (m_ComVariab && !argDescription.IsEmpty())
                        definition << _T('\n') << argDescription;
                }
            }
        }
    }
    else if(!isUnique)
    {
        if (   (isAfterPercent && (callTips.GetCount() != idxFuncSub.GetCount()))
            || (!isAfterPercent && (callTips.GetCount() != result->GetCount())) )
            return;

        if (lastName.IsEmpty())
            return;

        int idxPage = 0;
        if (m_IdxCallTipPage.count(lastName) == 1)
        {
            idxPage = m_IdxCallTipPage[lastName];
            if (idxPage+1 > int(callTips.GetCount()))
                idxPage = 0;
            else if (idxPage < 0)
                idxPage = callTips.GetCount() - 1;
        }
        m_IdxCallTipPage[lastName] = idxPage;

        definition = _T("");

        definition << wxString::Format(_T("\u02C6%d of %d\u02C7 "),idxPage+1,callTips.GetCount());

        if (isAfterPercent)
            definition << result->Item(idxFuncSub[idxPage])->m_DisplayName << _T('\n');
        else
            definition << result->Item(idxPage)->m_DisplayName << _T('\n');
        int mStart = definition.length() + 2;

        wxString ctdef = callTips.Item(idxPage);
        int nCommas = m_pNativeParser->CountCommas(ctdef, 1, false);
        int commasDif = commas - nCommas;
        if (commasDif > 0)
        {
            for (int i=0; i< commasDif; i++)
            {
                ctdef << _T(", *???*");
            }
            ctdef << _T(" ");
        }
        definition << ctdef;

        m_pNativeParser->GetCallTipHighlight(ctdef, commasPos, start, end);
        if (isAfterPercent)
            token = result->Item(idxFuncSub[idxPage]);
        else
            token = result->Item(idxPage);
        if (token->m_TokenKind == tkSubroutine || token->m_TokenKind == tkFunction)
        {
            wxString argName = callTips.Item(idxPage).Mid(start,end-start);
            argName = argName.BeforeFirst(_T(','));
            argName = argName.BeforeFirst(_T(')'));
            argName.Replace(_T("["),_T(" "));
            argName.Replace(_T("]"),_T(" "));
            argName.Trim().Trim(false);

            wxString argDecl;
            wxString argDescription;
            if (m_pNativeParser->GetParser()->FindTokenDeclaration(*token, argName, argDecl, argDescription))
            {
                definition << _T('\n') << argDecl;
                if (m_ComVariab && !argDescription.IsEmpty())
                    definition << _T('\n') << argDescription;
            }
        }
        start += mStart;
        end   += mStart;
    }

    if (!definition.IsEmpty())
    {
        ed->GetControl()->CallTipShow(pos, definition);
        ed->GetControl()->CallTipSetHighlight(start, end);
    }
    else if (ed->GetControl()->CallTipActive())
        ed->GetControl()->CallTipCancel();

    if (m_LogUseWindow && (!m_WasCallTipInfoLog || !m_LastCallTipName.IsSameAs(lastName)))
        ShowInfoLog(result, isAfterPercent);

    m_ShowedCallTip = true;
    m_CallTipPossition = ed->GetControl()->GetCurrentPos();
    m_LastCallTipName = lastName;
    m_WasCallTipInfoLog = true;
}

void FortranProject::OnNextPrevCallTipPage(wxCommandEvent& event)
{
    event.Skip();

    if (!IsAttached() || !m_InitDone)
        return;

    if (!Manager::Get()->GetEditorManager())
        return;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;

    if (!ed->GetControl()->CallTipActive())
        return;

    int deltaIdx = -1;
    if (event.GetId() == idMenuNextCallTipPage)
        deltaIdx = 1;

    if (m_IdxCallTipPage.count(m_LastCallTipName))
    {
        int idxPage = m_IdxCallTipPage[m_LastCallTipName];
        m_IdxCallTipPage[m_LastCallTipName] = idxPage+deltaIdx;
        ed->GetControl()->CallTipCancel();
        ShowCallTip();
    }
}

void FortranProject::OnValueTooltip(CodeBlocksEvent& event)
{
    event.Skip();

    if (!IsAttached() || !m_InitDone || wxGetKeyState(WXK_CONTROL))
        return;

    EditorBase* base = event.GetEditor();
    cbEditor* ed = base && base->IsBuiltinEditor() ? static_cast<cbEditor*>(base) : 0;
    if (!ed || ed->IsContextMenuOpened())
        return;

    if (!m_pNativeParser->IsFileFortran(ed->GetShortName()))
        return;

    if ((m_ShowedCallTip && ed->GetControl()->CallTipActive()) || ed->GetControl()->AutoCompActive())
        return;

    if (ed->GetControl()->CallTipActive())
        ed->GetControl()->CallTipCancel();

    if (wxWindow::FindFocus() != static_cast<wxWindow*>(ed->GetControl()))
        return;

    if (!Manager::Get()->GetConfigManager(_T("fortran_project"))->ReadBool(_T("eval_tooltip"), true))
        return;

    cbStyledTextCtrl* control = ed->GetControl();

    if (wxWindow::FindFocus() != static_cast<wxWindow*>(control))
        return;

    int style = event.GetInt();

    if (style != wxSCI_F_DEFAULT && style != wxSCI_F_OPERATOR && style != wxSCI_F_IDENTIFIER
            && style != wxSCI_F_OPERATOR2 && style != wxSCI_F_WORD && style != wxSCI_F_WORD2
            && style != wxSCI_F_WORD3)
        return;

    int pos = control->PositionFromPointClose(event.GetX(), event.GetY());
    if (pos < 0 || pos >= control->GetLength())
        return;
    int endOfWord = control->WordEndPosition(pos, true);
    int startOfWord = control->WordStartPosition(pos, true);
    wxString nameUnder = control->GetTextRange(startOfWord, endOfWord);
    if (nameUnder.IsEmpty())
        return;

    ParserF* pParser = m_pNativeParser->GetParser();
    TokensArrayFlatClass tokensTmp;
    TokensArrayFlat* result = tokensTmp.GetTokens();

    bool isAfterPercent = false;
    pParser->FindMatchTokensForToolTip(nameUnder, endOfWord, ed, m_LogOnlyUseAssoc, m_LogOnlyPublicNames, *result, isAfterPercent);
    if (result->IsEmpty())
        m_pKeywordsParser->GetTokensForToolTip(nameUnder, *result);

    if (result->size() > 32 || result->size() == 0)
        return;

    bool type_bound = false;
    wxString msg;
    for (size_t i=0; i<result->GetCount(); ++i)
    {
        TokenFlat* token = result->Item(i);
        if (!token->m_Rename.IsEmpty())
        {
            msg << token->m_Rename << _T(" => ") << token->m_DisplayName << _T("\n");
        }
        if (token->m_TokenKind == tkVariable)
        {
            msg << token->m_TypeDefinition << _T(" :: ") << token->m_DisplayName << token->m_Args << _T("\n");
            if (m_ComVariab && !token->m_PartLast.IsEmpty())
                msg  << token->m_PartLast << _T("\n");
        }
        else if (token->m_TokenKind == tkType)
        {
            msg << _T("type: ") << token->m_DisplayName << _T("\n");
        }
        else if (token->m_TokenKind == tkSubroutine)
        {
            msg << _T("subroutine ") << token->m_DisplayName << token->m_Args << _T("\n");
        }
        else if (token->m_TokenKind == tkFunction)
        {
            if (!token->m_PartFirst.IsEmpty())
            {
                msg << token->m_PartFirst << _T(" ");
            }
            msg << _T("function ") << token->m_DisplayName << token->m_Args << _T("\n");
        }
        else if (token->m_TokenKind == tkProcedure)
        {
            TokenFlat* token2 = 0;
            if (result->GetCount() > i+1)
            {
                i++;
                token2 = result->Item(i);
            }
            pParser->FindTooltipForTypeBoundProc(msg, token, token2);
            type_bound = true;
        }
        else if (isAfterPercent && token->m_TokenKind == tkInterface)
        {
            while (i < result->GetCount())
            {
                token = result->Item(i);
                if (token->m_TokenKind == tkInterface)
                {
                    wxString specNames = token->m_PartLast;
                    specNames.Replace(_T(" "),_T(", "));
                    msg << _T("generic :: ") << token->m_DisplayName << _T(" => ") << specNames << _T("\n");
                }
                i++;
            }
            type_bound = true;
        }
        else
        {
            msg << token->GetTokenKindString() << _T(" ") << token->m_DisplayName << token->m_Args << _T("\n");
        }
    }
    if (result->GetCount() == 1 && !type_bound)
    {
        if (!result->Item(0)->m_Filename.IsEmpty())
        {
            if (result->Item(0)->m_ParentTokenKind == tkModule)
            {
                msg << result->Item(0)->m_ParentDisplayName << _T(", ");
            }
            msg << result->Item(0)->m_Filename.AfterLast(wxFILE_SEP_PATH) << _T(":") << result->Item(0)->m_LineStart;
        }
        else
            msg.Trim();
    }
    else
        msg.Trim();

    if (!m_IsDebugging)
    {
        control->CallTipShow(pos, msg);
        m_ShowedCallTip = false;
    }

    if (result->GetCount() >= 1 && m_LogUseWindow)
    {
        ShowInfoLog(result, isAfterPercent);
        m_WasCallTipInfoLog = false;
    }
}

void FortranProject::ShowInfoLog(TokensArrayFlat* result, bool isAfterPercent)
{
    if (!m_LogUseWindow)
        return;
    if (result->GetCount() == 0)
        return;

    wxString logMsg;
    wxString fileNameOld;

    if (!isAfterPercent)
    {
        unsigned int countMax = 20<result->GetCount() ? 20 : result->GetCount();
        for (unsigned int i=0; i < countMax; i++)
        {
            TokenFlat* token = result->Item(i);
            wxString logMsg1;
            bool readFile;
            if (token->m_Filename.IsSameAs(fileNameOld))
            {
                readFile = false;
            }
            else
            {
                readFile = true;
                fileNameOld = token->m_Filename;
            }

            if (!token->m_Rename.IsEmpty())
            {
                logMsg << token->m_Rename << _T(" => ") << token->m_DisplayName << _T("\n");
            }

            if (token->m_TokenKind == tkSubroutine || token->m_TokenKind == tkFunction)
            {
                if (m_pNativeParser->GetParser()->FindInfoLog(*token,m_LogComAbove,m_LogComBelow,m_LogDeclar,m_LogComVariab,logMsg1,readFile))
                {
                    logMsg << logMsg1 << _T("\n\n");
                }
            }
            else if (token->m_TokenKind == tkInterface)
            {
                if (m_pNativeParser->GetParser()->GetTokenStr(*token, logMsg1))
                    logMsg << logMsg1 << _T("\n\n");

                fileNameOld.Empty();
            }
        }
        if (!logMsg.IsEmpty())
        {
            if (countMax < result->GetCount())
            {
                logMsg << wxString::Format(_T("!*********** %d more interfaces was not showed *************"),result->GetCount()-countMax);
            }
            WriteToLog(logMsg);
        }
    }
    else //isAfterPercent
    {
        if (result->Item(0)->m_TokenKind == tkProcedure)
            m_pNativeParser->GetParser()->FindInfoLogForTypeBoundProc(*result,m_LogComAbove,m_LogComBelow,m_LogDeclar,m_LogComVariab,logMsg);
        else if (result->Item(0)->m_TokenKind == tkInterface)
            m_pNativeParser->GetParser()->FindInfoLogForGenericTBProc(*result,m_LogComAbove,m_LogComBelow,m_LogDeclar,m_LogComVariab,logMsg);
        if (!logMsg.IsEmpty())
        {
            WriteToLog(logMsg);
        }
    }
}


cbConfigurationPanel* FortranProject::GetConfigurationPanel(wxWindow* parent)
{
    FPOptionsDlg* dlg = new FPOptionsDlg(parent, m_pNativeParser, this);
    return dlg;
}

int FortranProject::Configure()
{
    return 0;
}

void FortranProject::RereadOptions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));

    m_LexerKeywordsToInclude[0] = cfg->ReadBool(_T("/lexer_keywords_set1"), true);
    m_LexerKeywordsToInclude[1] = cfg->ReadBool(_T("/lexer_keywords_set2"), true);
    m_LexerKeywordsToInclude[2] = cfg->ReadBool(_T("/lexer_keywords_set3"), false);
    m_LexerKeywordsToInclude[3] = cfg->ReadBool(_T("/lexer_keywords_set4"), false);
    m_LexerKeywordsToInclude[4] = cfg->ReadBool(_T("/lexer_keywords_set5"), false);
    m_LexerKeywordsToInclude[5] = cfg->ReadBool(_T("/lexer_keywords_set6"), false);
    m_LexerKeywordsToInclude[6] = cfg->ReadBool(_T("/lexer_keywords_set7"), false);
    m_LexerKeywordsToInclude[7] = cfg->ReadBool(_T("/lexer_keywords_set8"), false);
    m_LexerKeywordsToInclude[8] = cfg->ReadBool(_T("/lexer_keywords_set9"), false);

    m_MaxMatch = cfg->ReadInt(_T("/max_matches"), 1000);
    if (m_MaxMatch < 1)
        m_MaxMatch = 1;

    m_UseSmartCC = cfg->ReadBool(_T("/use_smart_code_completion"), true);
    m_LogOnlyUseAssoc = cfg->ReadBool(_T("/only_use_associated"), true);
    m_LogOnlyPublicNames = !cfg->ReadBool(_T("/show_hidden_entities"), false);

    m_LogUseWindow = cfg->ReadBool(_T("/use_log_window"), true);
    m_LogComAbove = cfg->ReadBool(_T("/include_comments_above"), true);
    m_LogComBelow = cfg->ReadBool(_T("/include_comments_below"), true);
    m_LogDeclar = cfg->ReadBool(_T("/include_declarations_log"), true);
    m_LogComVariab = cfg->ReadBool(_T("/include_log_comments_variable"), true);

    m_ComVariab = cfg->ReadBool(_T("/include_comments_variable"), true);

    if (!m_pFortranLog && m_LogUseWindow)
    {
        CreateLogWindow();
    }
    else if (m_pFortranLog && !m_LogUseWindow)
    {
        RemoveLogWindow(false);
    }
    RegisterFileExtensions();
}

void FortranProject::WriteToLog(const wxString& text)
{
    if (m_pFortranLog)
    {
        m_pFortranLog->WriteToInfoWindow(text);
    }
}

void FortranProject::CreateLogWindow()
{
    m_pFortranLog = new FInfoWindow();
}

void FortranProject::RemoveLogWindow(bool appShutDown)
{
    if (appShutDown)
        return;

    if (m_pFortranLog)
    {
        m_pFortranLog->RemoveFromNotebook();
        m_pFortranLog->Destroy();
        m_pFortranLog = 0L;
    }
}

void FortranProject::OnJumpBack(wxCommandEvent& event)
{
    JumpTracker* jTr = m_pNativeParser->GetJumpTracker();

    if (!jTr->IsJumpBackEmpty())
    {
        jTr->MakeJumpBack();
        CheckEnableToolbar();
        JumpToLine(jTr->GetHomeAddress());
    }
}

void FortranProject::OnJumpHome(wxCommandEvent& event)
{
    JumpTracker* jTr = m_pNativeParser->GetJumpTracker();

    if (!jTr->IsJumpHomeEmpty())
        JumpToLine(jTr->GetHomeAddress());
}

void FortranProject::OnJumpForward(wxCommandEvent& event)
{
    JumpTracker* jTr = m_pNativeParser->GetJumpTracker();
    if (!jTr->IsJumpForwardEmpty())
    {
        jTr->MakeJumpForward();
        CheckEnableToolbar();
        JumpToLine(jTr->GetHomeAddress());
    }
}

void FortranProject::CheckEnableToolbar()
{
    m_pToolbar->EnableTool(XRCID("idFortProjBack"), !m_pNativeParser->GetJumpTracker()->IsJumpBackEmpty());
    m_pToolbar->EnableTool(XRCID("idFortProjHome"), !m_pNativeParser->GetJumpTracker()->IsJumpHomeEmpty());
    m_pToolbar->EnableTool(XRCID("idFortProjForward"), !m_pNativeParser->GetJumpTracker()->IsJumpForwardEmpty());
}

void FortranProject::JumpToLine(const LineAddress& adr)
{
    if (!IsAttached() || Manager::IsAppShuttingDown())
        return;

    EditorManager* edMan = Manager::Get()->GetEditorManager();
    if (cbEditor* ed = edMan->Open(adr.GetFilename()))
    {
        ed->GotoLine(adr.GetLineNumber());
    }
}

void FortranProject::OnDebuggerStarted(CodeBlocksEvent& event)
{
    event.Skip();
    m_IsDebugging = true;
}

void FortranProject::OnDebuggerFinished(CodeBlocksEvent& event)
{
    event.Skip();
    m_IsDebugging = false;
}

void FortranProject::OnGenerateMakefile(wxCommandEvent& event)
{
    m_pNativeParser->GenMakefile();
}

void FortranProject::OnChangeCase(wxCommandEvent& event)
{
    ChangeCase changCaseDlg(Manager::Get()->GetAppWindow());
    changCaseDlg.ShowModal();
}

void FortranProject::OnReparseEditorTimer(wxTimerEvent& event)
{
    m_pNativeParser->ReparseCurrentEditor();
}

void FortranProject::RegisterFileExtensions()
{
    PluginManager* plugman = Manager::Get()->GetPluginManager();
    if (!plugman)
        return;
    StringSet fileExts;
    m_pNativeParser->GetFortranFileExts(fileExts);
    plugman->RegisterCCFileExts(_T("FortranProject"), fileExts);
}

wxString FortranProject::GetIncludeFilename(cbStyledTextCtrl* control)
{
    if (!control)
        return wxEmptyString;
    wxString strName;
    int style = control->GetStyleAt(control->GetCurrentPos());
    if (style == wxSCI_F_STRING1 || style == wxSCI_F_STRING2)
    {
        wxString curLine = control->GetCurLine().Lower();
        if (!curLine.Trim(false).StartsWith(_T("include")))
            return wxEmptyString;

        int pos   = control->GetCurrentPos();
        int lineCur = control->LineFromPosition(pos);
        int lineStartPos = control->PositionFromLine(lineCur);
        wxString strBefore = control->GetTextRange(lineStartPos, pos).Lower().Trim(false);
        int idx1 = strBefore.Find('"', true);
        int idx2 = strBefore.Find('\'', true);
        if ((idx1 == wxNOT_FOUND && idx2 == wxNOT_FOUND) ||
            (idx1 != wxNOT_FOUND && idx2 != wxNOT_FOUND))
            return wxEmptyString;
        int idx = (idx1 != wxNOT_FOUND) ? idx1 : idx2;
        if (strBefore.Mid(0,idx).Trim().Trim(false) != _T("include"))
            return wxEmptyString;

        wxChar ch = curLine[idx];
        wxString strLast = curLine.Mid(strBefore.size());
        int idxL = strLast.Find(ch);
        if (idxL == wxNOT_FOUND)
            return wxEmptyString;
        int idxE = strBefore.size() + idxL;
        strName = curLine.Mid(idx+1, idxE-(idx+1)).Trim().Trim(false);
    }
    return strName;
}

void FortranProject::OnMenuEditPaste(wxCommandEvent& event)
{
    // Process clipboard data only if we have the focus
    if (!IsAttached() || !m_InitDone)
    {
        event.Skip();
        return;
    }
    m_pNativeParser->GetWorkspaceBrowser()->OnMenuEditPaste(event);
}



