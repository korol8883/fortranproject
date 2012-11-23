/*
 * This file licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 */

#include <sdk.h>
#include "nativeparserf.h"
#include <manager.h>
#include <configmanager.h>
#include <projectmanager.h>
#include <pluginmanager.h>
#include <logmanager.h>
#include <cbeditor.h>
#include <cbproject.h>
#include <cbexception.h>
#include "workspacebrowserf.h"
#include "workspacebrowserbuilder.h"
#include "parserf.h"
#include "makefilegen.h"
#include "bufferparserthread.h"
#include <projectloader_hooks.h>
#include <wx/regex.h>
#include <wx/log.h> // for wxSafeShowMessage()
#include <tinyxml/tinyxml.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
//#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "cbstyledtextctrl.h"

#include <wx/wfstream.h>
#include <cctype>

#include <wx/stopwatch.h>

static wxCriticalSection s_CurrentBufferCritSect;


int idWSPThreadEvent        = wxNewId();
int idBPThreadEvent         = wxNewId();
int idWorkspaceReparseTimer = wxNewId();
BEGIN_EVENT_TABLE(NativeParserF, wxEvtHandler)
    EVT_COMMAND(idWSPThreadEvent, wxEVT_COMMAND_ENTER, NativeParserF::OnUpdateWorkspaceBrowser)
    EVT_COMMAND(idBPThreadEvent, wxEVT_COMMAND_ENTER, NativeParserF::OnUpdateCurrentFileTokens)
    EVT_TIMER(idWorkspaceReparseTimer, NativeParserF::OnReparseWorkspaceTimer)
END_EVENT_TABLE()

NativeParserF::NativeParserF(FortranProject* forproj)
    : m_pWorkspaceBrowser(0),
      m_WorkspaceBrowserIsFloating(false),
      m_pFortranProject(forproj),
      m_WorkspaceReparseTimer(this, idWorkspaceReparseTimer),
      m_ThreadPool(this, wxNewId(), 1, 2 * 1024 * 1024)
{
}

NativeParserF::~NativeParserF()
{
    RemoveWorkspaceBrowser();
    ClearParser();
}

void NativeParserF::CreateWorkspaceBrowser()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));
    m_WorkspaceBrowserIsFloating = cfg->ReadBool(_T("/as_floating_window"), false);

    if (cfg->ReadBool(_T("/use_symbols_browser"), true))
    {
        if (!m_pWorkspaceBrowser)
        {
            if (!m_WorkspaceBrowserIsFloating)
            {
                // make this a tab in projectmanager notebook
                m_pWorkspaceBrowser = new WorkspaceBrowserF(Manager::Get()->GetProjectManager()->GetNotebook(), this, &m_Parser);
                Manager::Get()->GetProjectManager()->GetNotebook()->AddPage(m_pWorkspaceBrowser, _("FSymbols"));
            }
            else
            {
                // make this a free floating/docking window
                m_pWorkspaceBrowser = new WorkspaceBrowserF(Manager::Get()->GetAppWindow(), this, &m_Parser);
                CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);

                evt.name = _T("FSymbolsBrowser");
                evt.title = _("FSymbols browser");
                evt.pWindow = m_pWorkspaceBrowser;
                evt.dockSide = CodeBlocksDockEvent::dsRight;
                evt.desiredSize.Set(200, 250);
                evt.floatingSize.Set(200, 250);
                evt.minimumSize.Set(150, 150);
                evt.shown = true;
                evt.hideable = true;
                Manager::Get()->ProcessEvent(evt);
            }
            m_pWorkspaceBrowser->UpdateSash();
        }
    }
}

WorkspaceBrowserF* NativeParserF::GetWorkspaceBrowser()
{
    return m_pWorkspaceBrowser;
}

void NativeParserF::RemoveWorkspaceBrowser()
{
    if (m_pWorkspaceBrowser)
    {
        if (!m_WorkspaceBrowserIsFloating)
        {
            int idx = Manager::Get()->GetProjectManager()->GetNotebook()->GetPageIndex(m_pWorkspaceBrowser);
            if (idx != -1)
                Manager::Get()->GetProjectManager()->GetNotebook()->RemovePage(idx);
        }
        else
        {
            CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
            evt.pWindow = m_pWorkspaceBrowser;
            Manager::Get()->ProcessEvent(evt);
        }
        m_pWorkspaceBrowser->Destroy();
    }
    m_pWorkspaceBrowser = 0L;
}

void NativeParserF::UpdateWorkspaceBrowser()
{
    if (m_pWorkspaceBrowser && !Manager::IsAppShuttingDown())
    {
        wxCriticalSectionLocker locker(s_CritSect);

        m_pWorkspaceBrowser->UpdateView();
    }
    MarkCurrentSymbol();
}

void NativeParserF::AddParser(cbProject* project)
{
    if (!project)
        return;

    ParseProject(project);
}

void NativeParserF::ClearParser()
{
    m_Parser.Clear();
}

void NativeParserF::RemoveFromParser(cbProject* project)
{
    if (Manager::Get()->GetProjectManager()->GetProjects()->GetCount() == 0)
    {
        m_Parser.Clear();
        UpdateWorkspaceBrowser();

        return;
    }
    if (!project)
        return;

    for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;
        m_Parser.RemoveFile(pf->file.GetFullPath());
    }
    RemoveProjectFilesDependency(project);
}

bool NativeParserF::IsFileFortran(const wxString& filename)
{
    FortranSourceForm fsForm;
    return IsFileFortran(filename, fsForm);
}

bool NativeParserF::IsFileFortran(const wxString& filename, FortranSourceForm& fsForm)
{
   return m_Parser.IsFileFortran(filename, fsForm);
}

void NativeParserF::AddFileToParser(const wxString& filename)
{
    FortranSourceForm fsForm;
    if (IsFileFortran(filename, fsForm))
    {
        m_Parser.Reparse(filename, fsForm);
    }
}

void NativeParserF::RemoveFileFromParser(const wxString& filename)
{
    m_Parser.RemoveFile(filename);
}

void NativeParserF::ParseProject(cbProject* project)
{
    wxArrayString files;
    FortranSourceForm fsForm;
    ArrayOfFortranSourceForm fileForms;

    for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;

        if (IsFileFortran(pf->relativeFilename, fsForm))
        {
            files.Add(pf->file.GetFullPath());
            fileForms.push_back(fsForm);
        }
    }
    if (!files.IsEmpty())
    {
        m_Parser.BatchParse(files, fileForms);
    }
}

void NativeParserF::ReparseFile(const wxString& filename)
{
    FortranSourceForm fsForm;
    if (IsFileFortran(filename, fsForm))
        m_Parser.Reparse(filename, fsForm);
}

void NativeParserF::ReparseProject(cbProject* project)
{
    wxStopWatch sw;

    if (project && !Manager::IsAppShuttingDown())
    {
        for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;
            ReparseFile(pf->file.GetFullPath());
        }
    }

    Manager::Get()->GetLogManager()->DebugLog(F(_T("NativeParserF::ReparseProject: Reparse poject took %d ms."), sw.Time()));
}

void NativeParserF::ForceReparseWorkspace()
{
    if (Manager::IsAppShuttingDown())
        return;

    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (project && m_pWorkspaceBrowser)
        m_pWorkspaceBrowser->SetActiveProject(project);
    m_WorkspaceReparseTimer.Start(500, wxTIMER_ONE_SHOT);
}

void NativeParserF::OnReparseWorkspaceTimer(wxTimerEvent& event)
{
    if (Manager::IsAppShuttingDown())
        return;

    if (s_WorkspaceParserMutex.TryLock() == wxMUTEX_NO_ERROR)
    {
        MakeWSFileList();
        s_WorkspaceParserMutex.Unlock();

        WorkspaceParserThread* thread = new WorkspaceParserThread(this, idWSPThreadEvent);
        m_ThreadPool.AddTask(thread, true);
    }
}

void NativeParserF::MakeWSFileList()
{
    FortranSourceForm fsForm;
    m_WSFiles.clear();
    m_WSFileForms.clear();

    ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < projects->GetCount(); ++i)
    {
        cbProject* proj = projects->Item(i);

        for (FilesList::iterator it = proj->GetFilesList().begin(); it != proj->GetFilesList().end(); ++it)
        {
            ProjectFile* pf = *it;

            if (IsFileFortran(pf->relativeFilename, fsForm))
            {
                m_WSFiles.Add(pf->file.GetFullPath());
                m_WSFileForms.push_back(fsForm);
            }
        }
    }
}

wxArrayString* NativeParserF::GetWSFiles()
{
    return &m_WSFiles;
}

ArrayOfFortranSourceForm* NativeParserF::GetWSFileForms()
{
    return &m_WSFileForms;
}

void NativeParserF::OnUpdateWorkspaceBrowser(wxCommandEvent& event)
{
    m_Parser.ConnectToNewTokens();
    UpdateWorkspaceBrowser();
}

void NativeParserF::OnProjectActivated(cbProject* prj)
{
    if (!m_pWorkspaceBrowser)
        return;

    m_pWorkspaceBrowser->SetActiveProject(prj);
    UpdateWorkspaceBrowser();
}

void NativeParserF::OnEditorActivated(EditorBase* editor)
{
    if (!m_pWorkspaceBrowser)
        return;
    cbEditor* ed = editor && editor->IsBuiltinEditor() ? static_cast<cbEditor*>(editor) : 0;
    if (ed)
    {
        wxString filename = ed->GetFilename();
        if (m_pWorkspaceBrowser->GetBrowserDisplayFilter() == bdfFile && !m_pWorkspaceBrowser->GetActiveFilename().IsSameAs(filename))
        {
            UpdateWorkspaceBrowser();
        }
    }
}

void NativeParserF::OnEditorClose(EditorBase* editor)
{
    cbEditor* ed = editor && editor->IsBuiltinEditor() ? static_cast<cbEditor*>(editor) : 0;
    if (ed)
    {
        m_Parser.RemoveBuffer(ed->GetFilename());
    }
}

void NativeParserF::UpdateWorkspaceFilesDependency()
{
    ClearWSDependency();
    ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();

    for (size_t i = 0; i < projects->GetCount(); ++i)
    {
        cbProject* proj = projects->Item(i);
        if (!proj->IsMakefileCustom())
            UpdateProjectFilesDependency(proj);
    }
}

void NativeParserF::UpdateProjectFilesDependency(cbProject* project)
{
    project->SaveAllFiles();

    std::vector<ProjectFile*> pfs;
    for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
    {
        ProjectFile* pf = *it;
        if (IsFileFortran(pf->relativeFilename))
        {
            pfs.push_back(pf);
        }
    }

    wxString fn = project->GetFilename();
    WSDependencyMap::iterator pos;
    pos = m_WSDependency.find(fn);
    if (pos == m_WSDependency.end())
    {
        pos = m_WSDependency.insert(std::make_pair(fn,new ProjectDependencies(project))).first;
    }
    if (pfs.size() > 0)
    {
        pos->second->MakeProjectFilesDependencies(pfs, m_Parser);
        pos->second->EnsureUpToDateObjs();

        for (size_t i=0; i<pfs.size(); i++)
        {
            wxString fn = pfs[i]->file.GetFullPath();
            unsigned short int wt = pos->second->GetFileWeight(fn);
            pfs[i]->weight = wt;
        }
        if (pos->second->HasInfiniteDependences())
        {
            Manager::Get()->GetLogManager()->Log( _T("Warning. FortranProject plugin:\n     'It seems you have a circular dependency in Fortran files. Check your USE or INCLUDE statements.'") );
            cbMessageBox(_("It seems you have a circular dependency in Fortran files. Check your USE or INCLUDE statements."), _("Warning"));
        }
    }
}

void NativeParserF::ClearWSDependency()
{
    WSDependencyMap::iterator pos=m_WSDependency.begin();
    while(pos != m_WSDependency.end())
    {
        ProjectDependencies* pd = pos->second;
        pd->Clear();
        delete pd;
        pos++;
    }
    m_WSDependency.clear();
}

void NativeParserF::RemoveProjectFilesDependency(cbProject* project)
{
    if (m_WSDependency.count(project->GetFilename()))
    {
        ProjectDependencies* pd = m_WSDependency[project->GetFilename()];
        pd->Clear();
        delete pd;
    }
}

ParserF* NativeParserF::GetParser()
{
    return &m_Parser;
}

wxImageList* NativeParserF::GetImageList()
{
    if (m_pWorkspaceBrowser)
        return m_pWorkspaceBrowser->GetImageList();
    return 0;
}

int NativeParserF::GetTokenKindImageIdx(TokenF* token)
{
    if (m_pWorkspaceBrowser)
        return m_pWorkspaceBrowser->GetTokenKindImageIdx(token);
    return 0;
}

// count commas in lineText (nesting parentheses)
int NativeParserF::CountCommas(const wxString& lineText, int start, bool nesting)
{
    int commas = 0;
    int nest = 0;
    while (true)
    {
        wxChar c = lineText.GetChar(start);
        start++;
        if (c == '\0')
            break;
        else if (nesting && (c == '(' || c == '['))
            ++nest;
        else if (nesting && (c == ')' || c == ']'))
            --nest;
        else if (c == ',' && nest == 0)
            ++commas;
    }
    return commas;
}

wxString NativeParserF::GetLastName(const wxString& line)
{
    wxString name;
    wxString tmp = line;
    tmp.Trim();
    if (tmp.IsEmpty())
        return name;
    int cur = tmp.Len() - 1;

    while (cur >= 0)
    {
        wxChar cch = tmp.GetChar(cur);
        if (!isalnum(cch) && (cch != '_'))
        {
            cur++;
            break;
        }
        else
            cur--;
    }
    if (cur < 0)
        cur = 0;
    name = tmp.Mid(cur);

    return name;
}

void NativeParserF::CollectInformationForCallTip(int& commasAll, int& commasUntilPos, wxString& lastName, bool& isempty,
                                                 bool& isAfterPercent, TokensArrayFlat* result)
{
    wxString lineText = wxEmptyString; // string before '('
    CountCommasInEditor(commasAll, commasUntilPos, lastName, isempty, lineText);
    if (lastName.IsEmpty())
        return;

    isAfterPercent = false;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if(!ed)
        return;

    m_Parser.ChangeLineIfRequired(ed, lineText);

    TokensArrayFlatClass tokensTemp;
    TokensArrayFlat* resultTemp = tokensTemp.GetTokens();
    if (!m_Parser.FindMatchTypeComponents(ed, lineText, *resultTemp, false, false, isAfterPercent, true))
        return;
    if (resultTemp->GetCount() > 0)
    {
        TokenFlat* token = resultTemp->Item(0); // we take only first added item
        result->Add( new TokenFlat(token) );
        if (token->m_TokenKind == tkProcedure)
        {
            wxString tokName;
            if (!token->m_PartLast.IsEmpty())
                tokName = token->m_PartLast;
            else
                tokName = token->m_Name;

            TokensArrayFlatClass tokensTmp;
            TokensArrayFlat* resultTmp = tokensTmp.GetTokens();
            int kindMask = tkFunction | tkSubroutine;
            int noInChildren = tkInterface | tkFunction | tkSubroutine;
            bool found = m_Parser.FindMatchTokenInSameModule(token, tokName, *resultTmp, kindMask, noInChildren);
            if (!found)
                m_Parser.FindMatchTokensDeclared(tokName, *resultTmp, kindMask, false, noInChildren);
            if (resultTmp->GetCount() > 0)
                result->Add( new TokenFlat(resultTmp->Item(0)) );
        }
        else if (token->m_TokenKind == tkInterface)
        {
            m_Parser.FindGenericTypeBoudComponents(token, *result);
            for (size_t i=1; i<resultTemp->GetCount(); i++)
            {
                if (resultTemp->Item(i)->m_TokenKind == tkInterface)
                {
                    result->Add( new TokenFlat(resultTemp->Item(i)));
                    m_Parser.FindGenericTypeBoudComponents(resultTemp->Item(i), *result);
                }
            }
        }
    }

}


void NativeParserF::CountCommasInEditor(int& commasAll, int& commasUntilPos, wxString& lastName, bool& isempty, wxString& lineText)
{
    commasAll = 0;
    commasUntilPos = 0;
    lastName = wxEmptyString;
    int end = 0;
    isempty = false;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if(!ed)
        return;

    cbStyledTextCtrl* control = ed->GetControl();
    if (!control)
        return;
    int line = control->GetCurrentLine();
    lineText = control->GetLine(line);
    end = control->GetCurrentPos() - control->PositionFromLine(line);

    lineText = lineText.BeforeFirst('!');
    if (int(lineText.Len()) < end)
        return; // we are in comments
    //joun lines before if we are in continuation line
    FortranSourceForm fsForm;
    IsFileFortran(ed->GetShortName(), fsForm);

    if (fsForm == fsfFree)
    {
        int line2 = line - 1;
        while (line2 > 0)
        {
            wxString lineTextPast = control->GetLine(line2).BeforeFirst('!');
            lineTextPast = lineTextPast.Trim();
            if (!lineTextPast.IsEmpty())
            {
                int idx = lineTextPast.Find('&', true);
                if (idx == wxNOT_FOUND)
                {
                    break;
                }
                else
                {
                    lineText = lineTextPast.Mid(0,idx) + lineText;
                    end += idx;
                }
            }
            line2--;
        }
    }
    else //fsfFixed
    {
        if (lineText.Len() >= 6)
        {
            wxChar contS = lineText.GetChar(5);
            if (contS != ' ' && contS != '0')
            {
                lineText = lineText.Mid(6);
                end -= 6;
                int line2 = line - 1;
                while (line2 > 0)
                {
                    wxString lineTextPast = control->GetLine(line2).BeforeFirst('!');
                    lineTextPast = lineTextPast.Trim();
                    if (!lineTextPast.IsEmpty())
                    {
                        lineText = lineTextPast + lineText;
                        end += lineTextPast.Len();
                        if (lineTextPast.Len() >= 6)
                        {
                            wxChar contS2 = lineTextPast.GetChar(5);
                            if (contS2 == ' ' || contS2 == '0')
                                break;
                            else
                            {
                                lineText = lineText.Mid(6);
                                end -= 6;
                            }
                        }
                        else
                            break;
                    }
                    line2--;
                }
            }
        }
        else
        {
            return;
        }
    }

    wxString lineTextUntilPos = lineText.Mid(0,end);
    int nest = 0;

    while (end > 0)
    {
        --end;
        if (lineText.GetChar(end) == ')')
            --nest;
        else if (lineText.GetChar(end) == '(')
        {
            ++nest;
            if (nest > 0)
            {
                // count commas (nesting parentheses again) to see how far we 're in arguments
                commasAll = CountCommas(lineText, end + 1);
                wxString restLine = lineText.Mid(end+1).BeforeFirst(')');
                if (restLine.Trim().IsEmpty())
                    isempty = true;  //no characters except of spacies
                commasUntilPos = CountCommas(lineTextUntilPos, end + 1);
                break;
            }
        }
    }
    if (!end)
        return;

    lineText.Remove(end);
    lastName = GetLastName(lineText);
}

void NativeParserF::GetCallTips(const wxString& name, bool onlyUseAssoc, bool onlyPublicNames, wxArrayString& callTips, TokensArrayFlat* result)
{
    int tokKind = tkFunction | tkSubroutine | tkInterface;
    int resCountOld = result->GetCount();
    if (onlyUseAssoc)
    {
        cbEditor* ed =  Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (!ed)
            return;
        m_Parser.FindUseAssociatedTokens(onlyPublicNames, ed, name, false, *result, tokKind, false);
        int noChildrenOf = tkInterface | tkModule | tkSubmodule | tkFunction | tkSubroutine | tkProgram;
        m_Parser.FindMatchTokensDeclared(name, *result, tokKind, false, noChildrenOf, false, true); // take global procedures only
    }
    else
    {
        int noChildrenOf = tkInterface | tkFunction | tkSubroutine | tkProgram;
        m_Parser.FindMatchTokensDeclared(name, *result, tokKind, false, noChildrenOf, onlyPublicNames);
    }

    int tokkindFS = tkFunction | tkSubroutine;
    int resCount = result->GetCount();
    for (int i=resCountOld; i<resCount; ++i)
    {
        if (result->Item(i)->m_ParentTokenKind == tkSubmodule && (result->Item(i)->m_TokenKind & tokkindFS))
        {
            for (int j=i+1; j<resCount; ++j)
            {
                if (result->Item(j)->m_ParentTokenKind == tkInterfaceExplicit &&
                    result->Item(j)->m_TokenKind == result->Item(i)->m_TokenKind &&
                    result->Item(j)->m_Name.IsSameAs(result->Item(i)->m_Name) )
                {
                    result->RemoveAt(i);
                    resCount--;
                    i--;
                    break;
                }
            }
        }
    }

    resCount = result->GetCount();
    for (int i=resCountOld; i<resCount; ++i)
    {
        if (result->Item(i)->m_TokenKind == tkInterface)
        {
            m_Parser.FindChildrenOfInterface(result->Item(i), *result);
            result->RemoveAt(i);
            resCount--;
            i--;
        }
    }
    for (int i=resCountOld; i<int(result->GetCount()); ++i)
    {
        callTips.Add(result->Item(i)->m_Args);
    }
}

void NativeParserF::GetCallTipsForGenericTypeBoundProc(TokensArrayFlat* result, wxArrayString& callTips, wxArrayInt& idxFuncSub)
{
    if (result->GetCount() >= 3 && result->Item(0)->m_TokenKind == tkInterface)
    {
        int tokKind = tkFunction | tkSubroutine;
        for (size_t i=1; i<result->GetCount()-1; i+=2)
        {
            if (result->Item(i)->m_TokenKind == tkInterface)
                i++;
            if (i+1 >= result->GetCount())
                return;
            if (result->Item(i)->m_TokenKind != tkProcedure || !(result->Item(i+1)->m_TokenKind & tokKind))
                return;

            TokensArrayFlatClass tokensTmpCl;
            TokensArrayFlat* tokensTmp = tokensTmpCl.GetTokens();
            tokensTmp->Add(new TokenFlat(result->Item(i)));
            tokensTmp->Add(new TokenFlat(result->Item(i+1)));
            GetCallTipsForTypeBoundProc(tokensTmp, callTips);
            idxFuncSub.Add(i+1);
        }
    }
}

void NativeParserF::GetCallTipsForTypeBoundProc(TokensArrayFlat* result, wxArrayString& callTips)
{
    if (result->GetCount() != 2)
        return;
    if (!(result->Item(0)->m_TokenKind == tkProcedure))
        return;

    int tokKind = tkFunction | tkSubroutine;
    if (!(result->Item(1)->m_TokenKind & tokKind))
        return;

    if (result->Item(0)->m_Pass)
    {
        wxString pass_arg = result->Item(0)->m_Args;
        wxString args = result->Item(1)->m_Args;
        int start = 0;
        int end = 0;
        if (!pass_arg.IsEmpty())
            m_Parser.GetPossitionOfDummyArgument(args, pass_arg, start, end);
        else
            GetCallTipHighlight(args, 0, start, end);
        if (end <= start)
            return; // was not found?

        wxString fpart = args.Mid(0,start);
        int compos = fpart.Find(',',true);
        if (compos != wxNOT_FOUND)
            fpart = fpart.Mid(0,compos+1);

        wxString spart = args.Mid(start);
        compos = spart.Find(',');
        if (compos != wxNOT_FOUND)
            spart = spart.Mid(compos+1).Trim(false);
        else
            spart = args.Mid(end).Trim(false);

        callTips.Add(fpart.Append(spart));
    }
    else
    {
        callTips.Add(result->Item(1)->m_Args);
    }
}

void NativeParserF::BreakUpInLines(wxString& str, const wxString& original_str, int chars_per_line)
{
    if (chars_per_line == -1 || original_str.Length() <= (size_t)chars_per_line)
    {
        str = original_str;
        return;
    }

    // break it up in lines
    size_t pos = 0;
    size_t copy_start = 0;
    int last_comma = -1;
    while (pos < original_str.Length())
    {
        wxChar c = original_str.GetChar(pos);

        if      (c == _T(','))
            last_comma = pos;

        if (pos % chars_per_line == 0 && last_comma != -1)
        {
            str << original_str.Mid(copy_start, last_comma - copy_start + 1);
            str << _T('\n');
            copy_start = last_comma + 1;
        }
        else if (pos == original_str.Length() - 1)
            str << original_str.Mid(copy_start); // rest of the string
        ++pos;
    }
}

// set start and end to the calltip highlight region, based on commasWas (calculated in GetCallTips())
void NativeParserF::GetCallTipHighlight(const wxString& calltip, int commasWas, int& start, int& end)
{
    m_Parser.GetCallTipHighlight(calltip, commasWas, start, end);
}

void NativeParserF::MarkCurrentSymbol()
{
    if (!m_pWorkspaceBrowser || Manager::IsAppShuttingDown())
    {
        return;
    }
    wxString activeFilename;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return;
    activeFilename = ed->GetFilename();
    if (activeFilename.IsEmpty())
        return;
    cbStyledTextCtrl* control = ed->GetControl();
    int currentLine = control->GetCurrentLine() + 1;

    wxCriticalSectionLocker locker(s_CritSect);
    m_pWorkspaceBrowser->MarkSymbol(UnixFilename(activeFilename), currentLine);
}

void NativeParserF::RereadOptions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));
    // disabled?
    if (cfg->ReadBool(_("/use_symbols_browser"), true))
    {
        if (!m_pWorkspaceBrowser)
        {
            CreateWorkspaceBrowser();
        }
        // change class-browser docking settings
        else if (m_WorkspaceBrowserIsFloating != cfg->ReadBool(_T("/as_floating_window"), false))
        {
            RemoveWorkspaceBrowser();
            CreateWorkspaceBrowser();
        }
        else
        {
            m_pWorkspaceBrowser->RereadOptions();
        }
        UpdateWorkspaceBrowser();
    }
    else if (m_pWorkspaceBrowser)
    {
        RemoveWorkspaceBrowser();
    }
    else
    {
        m_pWorkspaceBrowser->RereadOptions();
    }

    m_Parser.RereadOptions();
}

JumpTracker* NativeParserF::GetJumpTracker()
{
    return &m_JumpTracker;
}

FortranProject* NativeParserF::GetFortranProject()
{
    return m_pFortranProject;
}

void NativeParserF::GenMakefile()
{
    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    if (!project)
    {
        Manager::Get()->GetLogManager()->Log(_T("No active project was found. Makefile was not generated."));
        cbMessageBox(_("No active project was found.\nMakefile was not generated."), _("Error"), wxICON_ERROR);
        return;
    }

    UpdateProjectFilesDependency(project);

    wxString fn = project->GetFilename();
    WSDependencyMap::iterator pos;
    pos = m_WSDependency.find(fn);
    if (pos == m_WSDependency.end())
        return;

    if (pos->second->GetSizeFiles() > 0)
        MakefileGen::GenerateMakefile(project, pos->second, this);
    else
    {
        Manager::Get()->GetLogManager()->Log(_T("Active project doesn't have Fortran files."));
        cbMessageBox(_("Active project doesn't have Fortran files.\nMakefile was not generated."), _("Information"), wxICON_INFORMATION);
    }
}

void NativeParserF::GetCurrentBuffer(wxString& buffer, wxString& filename)
{
    wxCriticalSectionLocker locker(s_CurrentBufferCritSect);
    buffer   = m_CurrentEditorBuffer;
    filename = m_CurrentEditorFilename;
}

void NativeParserF::ReparseCurrentEditor()
{
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if(!ed)
        return;
    cbStyledTextCtrl* control = ed->GetControl();
    if (!control)
        return;

    {
        wxCriticalSectionLocker locker(s_CurrentBufferCritSect);
        m_CurrentEditorBuffer = control->GetText();
        m_CurrentEditorFilename = ed->GetFilename();
    }

    if (BufferParserThread::s_BPTInstances <= 1)
    {
        BufferParserThread* thread = new BufferParserThread(this, idBPThreadEvent);
        m_ThreadPool.AddTask(thread, true);
    }
}

void NativeParserF::OnUpdateCurrentFileTokens(wxCommandEvent& event)
{
    m_Parser.ConnectToNewCurrentTokens();
}

