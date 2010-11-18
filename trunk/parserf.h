/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSERF_H
#define PARSERF_H

#include <wx/string.h>
#include <wx/event.h>
#include <wx/file.h>
#include "tokenf.h"
#include "tokenizerf.h"
#include "cbeditor.h"
#include <set>
#include <vector>

typedef std::vector<FortranSourceForm> ArrayOfFortranSourceForm;
typedef std::set<wxString> StringSet;


class ParserF
{
    public:
        ParserF();
        ~ParserF();
        bool Parse(const wxString& filename, FortranSourceForm fsForm);
        bool Reparse(const wxString& filename, FortranSourceForm fsForm);
        bool BatchParse(const wxArrayString& filenames, ArrayOfFortranSourceForm& fileForms);
        bool RemoveFile(const wxString& filename);
        TokensArrayF* GetTokens(){return m_pTokens;};
        bool FindMatchTokenInSameModule(const TokenFlat& procedureToken, const wxString& search, TokensArrayFlat& result, int tokenKindMask, int noChildrenOf);
        size_t FindMatchTokensDeclared(const wxString& search, TokensArrayFlat& result, int tokenKindMask, bool partialMatch=false, int noChildrenOf=0);
        void FindMatchChildrenDeclared(TokensArrayF &m_Children, wxString search, TokensArrayFlat& result, int tokenKindMask, bool partialMatch=false, int noChildrenOf=0);
        size_t FindMatchTokens(wxString filename, wxString search, TokensArrayF& result);
        void Clear();
        void ObtainUsedDeclaredModules(const wxString& fileName, StringSet* fileUseModules, StringSet* fileDeclaredModules, StringSet* fileIncludes);
        bool Done();
        bool IsFileFortran(const wxString& filename, FortranSourceForm& fsForm);
        void FindMatchDeclarationsInCurrentScope(const wxString& search, cbEditor* ed, TokensArrayFlat& result, bool partialMatch, int endPos=-1);
        void FindMatchVariablesInModules(const wxString& search, TokensArrayFlat& result, bool partialMatch);
        bool FindMatchTypeComponents(cbEditor* ed, const wxString& line, TokensArrayFlat& result, bool partialMatch, bool& isAfterPercent, bool getAsProcedure=false);
        void FindMatchTokensForToolTip(const wxString& nameUnder, int posEndOfWord, cbEditor* ed, TokensArrayFlat& result, bool& isAfterPercent);
        void FindMatchTokensForJump(cbEditor* ed, TokensArrayFlat& result);
        bool FindMatchTokensForCodeCompletion(bool useSmartCC, const wxString& nameUnderCursor, cbEditor* ed, TokensArrayFlat& result, bool& isAfterPercent, int& tokKind);
        bool FindWordsBefore(cbEditor* ed, int numberOfWords, wxString &curLine, wxArrayString &firstWords);
        void RereadOptions();
        bool FindTokenDeclaration(TokenFlat& token, const wxString& argName, wxString& argDecl, wxString& argDescription);
        bool FindTokenRange(TokenFlat& token, wxString& txtRange, wxString& buff, std::vector<int> &lineStarts, bool withDefinition=false, bool readFile=true);
        bool FindTokenRange(TokenFlat& token, wxString& txtRange);
        bool FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg);
        bool FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg, wxString& buff, bool readFile);
        bool FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg, wxString& argsNew);
        bool FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg, const wxString& argsNew, wxString& buff, bool readFile);
        bool FindTooltipForTypeBoundProc(wxString& msg, TokenFlat* token1, TokenFlat* token2);
        bool FindInfoLogForTypeBoundProc(TokensArrayFlat& tokenPair, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg);
        bool GetTokenStr(TokenFlat& token, wxString& msg);
        void FindChildrenOfInterface(TokenFlat* token, TokensArrayFlat& result);
        void GetPossitionOfDummyArgument(const wxString& args, const wxString& arg, int& start, int& end);
        void GetCallTipHighlight(const wxString& calltip, int commasWas, int& start, int& end);
    protected:
    private:
        void FindMatchChildren(TokensArrayF &m_Children, wxString search, TokensArrayF& result, bool exact=false);
        void FindChildrenOfTokenInWorkspace(wxString nameToken, TokensArrayF& result, int tokenKindMask, bool partialMatch);
        void FindChildrenOfToken(TokensArrayF &children, const wxString &nameToken, TokensArrayF& result,  int tokenKindMask, bool partialMatch);
        size_t GetFileIndex(const wxString& filename);
        TokensArrayF* FindFileTokens(const wxString& filename);
        void ObtainUDModulesToken(TokenF* token, StringSet* fileUseModules, StringSet* fileDeclaredModules, StringSet* fileIncludes);
        bool FindLineScope(unsigned int line, int& lineStart, int tokenKindMask, TokensArrayF& children);
        bool CutBlocks(const wxChar& ch, wxString& line);
        bool GetTypeOfComponent(const wxString& nameType, const wxString& nameComponent, wxString& nameTypeComponent);
        bool GetTypeOfChild(TokenF* pT, const wxString& nameComponent, wxString& nameTypeComponent);
        TokenF* GetType(const wxString& nameType);
        TokensArrayF* m_pTokens;
        bool m_Done;
        bool m_ExtDone;
        StringSet m_FortranExtFree;
        StringSet m_FortranExtFixed;
};

#endif // PARSERF_H
