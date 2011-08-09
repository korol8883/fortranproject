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
#include "farrays.h"

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
        bool FindTypeBoundProcedures(const TokenFlat& interToken, const wxArrayString& searchArr, TokensArrayFlat& resTokenArr);
        bool FindMatchTokenInSameModule(const TokenFlat& procedureToken, const wxString& search, TokensArrayFlat& result, int tokenKindMask, int noChildrenOf);
        size_t FindMatchTokensDeclared(const wxString& search, TokensArrayFlat& result, int tokenKindMask, bool partialMatch=false, int noChildrenOf=0, bool onlyPublicNames=false);
        void FindMatchChildrenDeclared(TokensArrayF &m_Children, wxString search, TokensArrayFlat& result, int tokenKindMask, bool partialMatch=false, int noChildrenOf=0, bool onlyPublicNames=false);
        size_t FindMatchTokens(wxString filename, wxString search, TokensArrayF& result);
        void Clear();
        void ObtainUsedDeclaredModules(const wxString& fileName, StringSet* fileUseModules, StringSet* fileDeclaredModules, StringSet* fileIncludes);
        bool Done();
        bool IsFileFortran(const wxString& filename, FortranSourceForm& fsForm);
        void FindMatchDeclarationsInCurrentScope(const wxString& search, cbEditor* ed, TokensArrayFlat& result, bool partialMatch, int endPos=-1);
        void FindMatchVariablesInModules(const wxString& search, TokensArrayFlat& result, bool partialMatch);
        bool FindMatchTypeComponents(cbEditor* ed, const wxString& line, TokensArrayFlat& result, bool partialMatch, bool onlyPublicNames, bool& isAfterPercent, bool getAsProcedure=false);
        void FindMatchTokensForToolTip(const wxString& nameUnder, int posEndOfWord, cbEditor* ed, bool onlyUseAssoc, bool onlyPublicNames, TokensArrayFlat& result, bool& isAfterPercent);
        void FindGenericTypeBoudComponents(TokenFlat* token, TokensArrayFlat& result);
        void FindMatchTokensForJump(cbEditor* ed, bool onlyUseAssoc, bool onlyPublicNames, size_t maxMatch, TokensArrayFlat& result);
        bool FindMatchTokensForCodeCompletion(bool useSmartCC, bool onlyUseAssoc, bool onlyPublicNames, size_t maxMatch, const wxString& nameUnderCursor, cbEditor* ed, TokensArrayFlat& result, bool& isAfterPercent, int& tokKind);
        bool FindWordsBefore(cbEditor* ed, int numberOfWords, wxString &curLine, wxArrayString &firstWords);
        void RereadOptions();
        bool FindTokenDeclaration(TokenFlat& token, const wxString& argName, wxString& argDecl, wxString& argDescription);
        bool FindTokenRange(TokenFlat& token, wxString& txtRange, wxString& buff, std::vector<int> &lineStarts, bool withDefinition=false, bool readFile=true);
        bool FindTokenRange(TokenFlat& token, wxString& txtRange);
        bool FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg);
        bool FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg, bool readFile);
        bool FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg, wxString& argsNew);
        bool FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg, const wxString& argsNew, bool readFile);
        bool FindTooltipForTypeBoundProc(wxString& msg, TokenFlat* token1, TokenFlat* token2);
        bool FindInfoLogForTypeBoundProc(TokensArrayFlat& tokenPair, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg,
                                         wxString* buff=NULL, std::vector<int>* lineStarts=NULL);
        bool FindInfoLogForGenericTBProc(TokensArrayFlat& tokens, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg);
        bool GetTokenStr(TokenFlat& token, wxString& msg);
        void FindChildrenOfInterface(TokenFlat* token, TokensArrayFlat& result);
        void GetPossitionOfDummyArgument(const wxString& args, const wxString& arg, int& start, int& end);
        void GetCallTipHighlight(const wxString& calltip, int commasWas, int& start, int& end);
        void FindUseAssociatedTokens(bool onlyPublicNames, cbEditor* ed, const wxString& search, bool partialMatch, TokensArrayFlat& result, int tokenKindMask, bool changeDisplayName, TokensArrayFlat* useWithRenameTok=NULL);
    protected:
    private:
        void FindMatchChildren(TokensArrayF &m_Children, wxString search, TokensArrayF& result, bool exact=false);
        void FindChildrenOfTokenInWorkspace(wxString nameToken, TokensArrayF& result, int tokenKindMask, bool partialMatch);
        void FindChildrenOfToken(TokensArrayF &children, const wxString &nameToken, TokensArrayF& result,  int tokenKindMask, bool partialMatch);
        size_t GetFileIndex(const wxString& filename);
        TokensArrayF* FindFileTokens(const wxString& filename);
        TokenF* FindModuleToken(const wxString& moduleName);
        void ObtainUDModulesToken(TokenF* token, StringSet* fileUseModules, StringSet* fileDeclaredModules, StringSet* fileIncludes);
        bool FindLineScope(unsigned int line, int& lineStart, int tokenKindMask, TokensArrayF& children, TokenF* &pToken);
        void FindLineScopeLN(cbEditor* ed, int& lineStart, TokenFlat* &token, int endPos);
        bool CutBlocks(const wxChar& ch, wxString& line);
        bool GetTypeOfComponent(const wxString& nameType, const wxString& nameComponent, wxString& nameTypeComponent);
        bool GetTypeOfChild(TokenF* pT, const wxString& nameComponent, wxString& nameTypeComponent);
        TokenF* GetType(const wxString& nameType);
        void FindUseAssociatedTokens(TokenF* useToken, const wxString& searchLw, TokensArrayFlat& result, int tokenKindMask, bool partialMatch, bool changeDisplayName, bool onlyPublicNames, TokensArrayFlat* useWithRenameTok=NULL);
        void FindMatchTokensInModuleAndUse(const wxString &modName, const wxString& searchLw, TokensArrayFlat& result, int tokenKindMask, int noChildrenOf, bool partialMatch, bool onlyPublicNames, bool changeDisplayName, TokensArrayFlat* useWithRenameTok);
        void FindUseAssociatedTokens2(TokenF* useToken, const wxString &searchLw, ArrOfSizeT &resChildrenIdx, BoolArray2D &resCanBeSeen2D, int tokenKindMask, bool partialMatch, bool changeDisplayName,
                                      bool onlyPublicNames, TokensArrayFlat& renamedTokens, TokensArrayFlat* useWithRenameTok);
        void FindMatchTokensInModuleAndUse2(const wxString& modName, const wxString& searchLw, ArrOfSizeT* &childrenIdx, BoolArray2D* &canBeSeen2D, int tokenKindMask,
                                             int noChildrenOf, bool partialMatch, bool onlyPublicNames, bool changeDisplayName, TokensArrayFlat* useWithRenameTok);
        void FindAddress(cbEditor* ed, wxArrayString& address);
        void FindTokensForUse(const wxString& search, wxArrayString& firstWords, TokensArrayFlat& result, int& tokKind, bool onlyPublicNames);
        void AddUniqueResult(TokensArrayFlat& result, const TokenF* token);
        void AddUniqueResult(TokensArrayFlat& result, const TokenFlat* token);

        TokensArrayF* m_pTokens;
        bool m_Done;
        bool m_ExtDone;
        StringSet m_FortranExtFree;
        StringSet m_FortranExtFixed;

        wxString m_Buff;
        std::vector<int> m_LineStarts;

        int recursiveDeep;
        //size_t maxResultCount;
        //bool reachedResultCountLimit;

        wxArrayString m_VisitedModules;
        PassedTokensArray2D m_PassedTokensVisited;
        ArrOfSizeT2D m_ChildrenIdxVisited;
        BoolArray3D m_CanBeSeenVisited;

        bool m_UseRenameArrays;
        int m_RenameDeep;

        wxArrayString m_VisitedModulesRen;
        PassedTokensArray2D m_PassedTokensVisitedRen;
        ArrOfSizeT2D m_ChildrenIdxVisitedRen;
        BoolArray3D m_CanBeSeenVisitedRen;

};

#endif // PARSERF_H
