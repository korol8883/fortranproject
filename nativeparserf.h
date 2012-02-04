/*
 * This file is licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef NATIVEPARSERF_H
#define NATIVEPARSERF_H

#include <map>
#include <set>
#include <wx/string.h>
#include <wx/event.h>
#include "parserf.h"
#include "workspacebrowserf.h"
#include "projectdependencies.h"
#include "jumptracker.h"

typedef std::map<wxString,ProjectDependencies*>  WSDependencyMap;

// forward decls
class EditorBase;
class cbProject;
class TokenF;
class FortranProject;

class NativeParserF : public wxEvtHandler
{
    public:
        NativeParserF(FortranProject* forproj);
        ~NativeParserF();

        void AddParser(cbProject* project);
        void ClearParser();
        void RemoveFromParser(cbProject* project);
        void AddFileToParser(const wxString& filename);
        void RemoveFileFromParser(const wxString& filename);
        void ReparseFile(const wxString& filename);
        void ReparseProject(cbProject* project);
        void ParseProject(cbProject* project);
        void ForceReparseWorkspace();
        void UpdateProjectFilesDependency(cbProject* project);
        ParserF* GetParser();
        bool IsFileFortran(const wxString& filename);

        void CreateWorkspaceBrowser();
        WorkspaceBrowserF* GetWorkspaceBrowser();
        void RemoveWorkspaceBrowser();
        void UpdateWorkspaceBrowser();
        wxImageList* GetImageList();
        int GetTokenKindImageIdx(TokenF* token);
        void GetCallTips(const wxString& name, bool onlyUseAssoc, bool onlyPublicNames, wxArrayString& callTips, TokensArrayFlat* result);
        void GetCallTipsForGenericTypeBoundProc(TokensArrayFlat* result, wxArrayString& callTips);
        void GetCallTipsForTypeBoundProc(TokensArrayFlat* result, wxArrayString& callTips);
        int CountCommas(const wxString& lineText, int start, bool nesting=true);
        void CollectInformationForCallTip(int& commasAll, int& commasUntilPos, wxString& lastName, bool& isempty, bool& isAfterPercent, TokensArrayFlat* result);
        void CountCommasInEditor(int& commasAll, int& commasUntilPos, wxString& lastName, bool& isempty, wxString& lineText);
        void GetCallTipHighlight(const wxString& calltip, int commasWas, int& start, int& end);
        void MarkCurrentSymbol();
        void RereadOptions();
        JumpTracker* GetJumpTracker();
        FortranProject* GetFortranProject();
        void GenMakefile();

    protected:
    private:
        friend class FortranProject;

        void OnEditorActivated(EditorBase* editor);
        void OnProjectActivated(cbProject* project);
        void UpdateWorkspaceFilesDependency();
        void ClearWSDependency();
        void RemoveProjectFilesDependency(cbProject* project);

        bool IsFileFortran(const wxString& filename, FortranSourceForm& fsForm);
        void BreakUpInLines(wxString& str, const wxString& original_str, int chars_per_line);
        wxString GetLastName(const wxString& line);

        ParserF m_Parser;
        WorkspaceBrowserF* m_pWorkspaceBrowser;
        bool m_WorkspaceBrowserIsFloating;
        FortranProject* m_pFortranProject;

        WSDependencyMap m_WSDependency;

        JumpTracker m_JumpTracker;
};

#endif // NATIVEPARSERF_H

