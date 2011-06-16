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

NativeParserF::NativeParserF(FortranProject* forproj)
    : m_pWorkspaceBrowser(0),
      m_WorkspaceBrowserIsFloating(false),
      m_pFortranProject(forproj)
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
    if (m_pWorkspaceBrowser && !Manager::isappShuttingDown())
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

    for (int i = 0; i < project->GetFilesCount(); ++i)
    {
        ProjectFile* pf = project->GetFile(i);
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
    ParserF* parser = &m_Parser;

    wxArrayString files;
    FortranSourceForm fsForm;
    ArrayOfFortranSourceForm fileForms;

    for (int i = 0; i < project->GetFilesCount(); ++i)
	{
		ProjectFile* pf = project->GetFile(i);

        if (IsFileFortran(pf->relativeFilename, fsForm))
        {
            files.Add(pf->file.GetFullPath());
            fileForms.push_back(fsForm);
        }
	}
	if (!files.IsEmpty())
    {
        parser->BatchParse(files, fileForms);
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
    if (project && !Manager::IsAppShuttingDown())
    {
        for (int i = 0; i < project->GetFilesCount(); ++i)
        {
            ReparseFile(project->GetFile(i)->file.GetFullPath());
        }
    }
}

void NativeParserF::ForceReparseWorkspace()
{
    if (Manager::IsAppShuttingDown())
        return;

    m_pWorkspaceBrowser->DeleteAllItems();
    m_Parser.Clear();

    ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < projects->GetCount(); ++i)
    {
        AddParser(projects->Item(i));
    }
    UpdateWorkspaceBrowser();
}

void NativeParserF::OnProjectActivated(cbProject* prj)
{
    if (!m_pWorkspaceBrowser)
        return;

    ParserF* parser = &m_Parser;
    if (parser)
    {
        UpdateWorkspaceBrowser();
    }
}

void NativeParserF::OnEditorActivated(EditorBase* editor)
{
    if (!m_pWorkspaceBrowser)
        return;
    cbEditor* ed = editor && editor->IsBuiltinEditor() ? static_cast<cbEditor*>(editor) : 0;
    if (ed)
    {
        ParserF* parser = &m_Parser;;
        if (parser && m_pWorkspaceBrowser->GetBrowserDisplayFilter() == bdfFile)
        {
            UpdateWorkspaceBrowser();
        }
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
    for (int j = 0; j < project->GetFilesCount(); ++j)
    {
        ProjectFile* pf = project->GetFile(j);
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
        else
        {
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
    if(!m_Parser.Done())
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
                int idx = lineTextPast.Find('&');
                if (idx == wxNOT_FOUND)
                {
                    break;
                }
                else
                {
                    lineText = lineTextPast.BeforeFirst('&') + lineText;
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
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
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
        m_Parser.FindMatchTokensDeclared(name, *result, tokKind, false, tkInterface | tkModule); // take global procedures only
    }
    else
    {
        m_Parser.FindMatchTokensDeclared(name, *result, tokKind, false);
    }
    int resCount = result->GetCount();

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

void NativeParserF::GetCallTipsForGenericTypeBoundProc(TokensArrayFlat* result, wxArrayString& callTips)
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
        }
    }
}

void NativeParserF::GetCallTipsForTypeBoundProc(TokensArrayFlat* result, wxArrayString& callTips)
{
    if (result->GetCount() > 0 && result->Item(0)->m_TokenKind == tkProcedure)
    {
        if (result->GetCount() != 2)
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
    int last_space = -1;
    while (pos < original_str.Length())
    {
        wxChar c = original_str.GetChar(pos);

        if      (c == _T(','))
            last_comma = pos;
        else if (   (c == _T(' '))
                 || (c == _T('\t')) )
            last_space = pos;

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
    if (!m_pWorkspaceBrowser || Manager::isappShuttingDown())
    {
        return;
    }
    wxString activeFilename;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        //m_ActiveFilename = ed->GetFilename().BeforeLast(_T('.'));
        // the above line is a bug (see https://developer.berlios.de/patch/index.php?func=detailpatch&patch_id=1559&group_id=5358)
        activeFilename = ed->GetFilename().AfterLast(wxFILE_SEP_PATH);
        activeFilename = ed->GetFilename().BeforeLast(wxFILE_SEP_PATH) + wxFILE_SEP_PATH + activeFilename; //.BeforeLast(_T('.'));
    }
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
