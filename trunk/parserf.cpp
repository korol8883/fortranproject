/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include <wx/tokenzr.h>
#include <wx/string.h>
#include <wx/thread.h>
#include <wx/arrstr.h>
#include "parserf.h"
#include <globals.h>
#include "parserthreadf.h"
#include "cbstyledtextctrl.h"
#include <configmanager.h>
#include "ccsmartfilter.h"


ParserF::ParserF()
{
    m_pTokens = new TokensArrayF();
    m_Done = false;
}

ParserF::~ParserF()
{
    //dtor
    Clear();
    delete m_pTokens;
}

bool ParserF::Parse(const wxString& filename, FortranSourceForm fsForm)
{
    wxCriticalSectionLocker locker(s_CritSect);
    ParserThreadF* thread = new ParserThreadF(UnixFilename(filename),m_pTokens, fsForm);
    bool res = thread->Parse();
    delete thread;
    return res;
}

bool ParserF::Reparse(const wxString& filename, FortranSourceForm fsForm)
{
    m_Done = false;
    RemoveFile(filename);
    bool res = Parse(filename, fsForm);
    m_Done = true;
    return res;
}

bool ParserF::BatchParse(const wxArrayString& filenames, ArrayOfFortranSourceForm& fileForms)
{
    m_Done = false;
    bool res = true;
    if (filenames.size() != fileForms.size())
        return false;
    for (size_t i=0; i<filenames.size(); i++)
    {
        if(!Parse(filenames[i], fileForms[i]))
        {
            res = false;
            //break;
        }
    }
    m_Done = true;
    return res;
}

bool ParserF::RemoveFile(const wxString& filename)
{
	wxString file = UnixFilename(filename);
    m_Done = false;
    wxCriticalSectionLocker locker(s_CritSect);

    unsigned int i = 0;
    while (i < m_pTokens->GetCount())
    {
        if (m_pTokens->Item(i)->m_Filename.IsSameAs(file))
        {
            m_pTokens->Item(i)->Clear();
            delete m_pTokens->Item(i);
            m_pTokens->RemoveAt(i);
        }
        else
            ++i;
    }
    m_Done = true;
	return true;
}

bool ParserF::Done()
{
    return m_Done;
}

bool ParserF::FindTypeBoundProcedures(const TokenFlat& interToken, const wxArrayString& searchArr, TokensArrayFlat& resTokenArr)
{
    wxCriticalSectionLocker locker(s_CritSect);
    bool foundType = false;

    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        if (m_pTokens->Item(i)->m_Filename == interToken.m_Filename)
        {
            // find module
            TokenF* module;
            TokensArrayF* fileChildren = &m_pTokens->Item(i)->m_Children;
            for (size_t j=0; j<fileChildren->GetCount(); j++)
            {
                if (fileChildren->Item(j)->m_TokenKind == tkModule)
                {
                    module = fileChildren->Item(j);
                    for (size_t k=0; k < module->m_Children.GetCount(); k++)
                    {
                        if (interToken.m_ParentName.IsSameAs(module->m_Children.Item(k)->m_Name) &&
                             interToken.m_ParentTokenKind == module->m_Children.Item(k)->m_TokenKind)
                        {
                            // type was found
                            TokenF* typeTok = module->m_Children.Item(k);
                            for (size_t m=0; m < typeTok->m_Children.GetCount(); m++)
                            {
                                for (size_t l=0; l < searchArr.GetCount(); l++)
                                {
                                    if (typeTok->m_Children.Item(m)->m_TokenKind == tkProcedure &&
                                        typeTok->m_Children.Item(m)->m_Name.IsSameAs(searchArr.Item(l)) )
                                    {
                                        resTokenArr.Add(new TokenFlat(typeTok->m_Children.Item(m)));
                                    }
                                }
                            }
                            foundType = true;
                            break;
                        }
                    }
                    if (foundType)
                        break;
                }
            }
            break;
        }
    }
    return foundType;
}

bool ParserF::FindMatchTokenInSameModule(const TokenFlat& procedureToken, const wxString& search, TokensArrayFlat& result, int tokenKindMask, int noChildrenOf)
{
    wxCriticalSectionLocker locker(s_CritSect);

    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        if (m_pTokens->Item(i)->m_Filename == procedureToken.m_Filename)
        {
            // find module
            TokenF* module;
            TokensArrayF* fileChildren = &m_pTokens->Item(i)->m_Children;
            bool foundModule = false;
            for (size_t j=0; j<fileChildren->GetCount(); j++)
            {
                if (fileChildren->Item(j)->m_TokenKind == tkModule)
                {
                    module = fileChildren->Item(j);
                    for (size_t k=0; k < module->m_Children.GetCount(); k++)
                    {
                        if (procedureToken.m_ParentName.IsSameAs(module->m_Children.Item(k)->m_Name) &&
                             procedureToken.m_ParentTokenKind == module->m_Children.Item(k)->m_TokenKind)
                        {
                            foundModule = true;
                            break;
                        }
                    }
                }
                if (foundModule)
                    break;
            }
            if (foundModule)
            {
                // find match token
                wxString searchLw = search;
                size_t rCount = result.GetCount();
                FindMatchChildrenDeclared(module->m_Children, searchLw, result, tokenKindMask, false, noChildrenOf);
                if (rCount < result.GetCount())
                {
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

size_t ParserF::FindMatchTokensDeclared(const wxString& search, TokensArrayFlat& result, int tokenKindMask, bool partialMatch, int noChildrenOf)
{
    wxString searchLw = search.Lower();

    wxCriticalSectionLocker locker(s_CritSect);

    if (partialMatch)
    {
        for (size_t i=0; i<m_pTokens->GetCount(); i++)
        {
            if (m_pTokens->Item(i)->m_Children.GetCount() > 0)
            {
                FindMatchChildrenDeclared(m_pTokens->Item(i)->m_Children, searchLw, result, tokenKindMask, partialMatch, noChildrenOf);
            }
        }
    }
    else
    {
        for (size_t i=0; i<m_pTokens->GetCount(); i++)
        {
            if (m_pTokens->Item(i)->m_Name.IsSameAs(search) && (m_pTokens->Item(i)->m_TokenKind & tokenKindMask))
            {
                result.Add(new TokenFlat(m_pTokens->Item(i)));
            }
            if (m_pTokens->Item(i)->m_Children.GetCount() > 0)
            {
                FindMatchChildrenDeclared(m_pTokens->Item(i)->m_Children, searchLw, result, tokenKindMask, partialMatch, noChildrenOf);
            }
        }
    }
    return result.GetCount();
}

void ParserF::FindMatchChildrenDeclared(TokensArrayF &children, wxString search, TokensArrayFlat& result, int tokenKindMask, bool partialMatch, int noChildrenOf)
{
    if (partialMatch)
    {
        for (size_t i=0; i<children.GetCount(); i++)
        {
            if (children.Item(i)->m_Name.StartsWith(search) && (children.Item(i)->m_TokenKind & tokenKindMask))
            {
                result.Add(new TokenFlat(children.Item(i)));
            }
            if (children.Item(i)->m_Children.GetCount() > 0 && !(children.Item(i)->m_TokenKind & noChildrenOf))
            {
                FindMatchChildrenDeclared(children.Item(i)->m_Children, search, result, tokenKindMask, partialMatch, noChildrenOf);
            }
        }
    }
    else
    {
        for (size_t i=0; i<children.GetCount(); i++)
        {
            if (children.Item(i)->m_Name.IsSameAs(search) && (children.Item(i)->m_TokenKind & tokenKindMask))
            {
                result.Add(new TokenFlat(children.Item(i)));
            }
            if (children.Item(i)->m_Children.GetCount() > 0 && !(children.Item(i)->m_TokenKind & noChildrenOf))
            {
                FindMatchChildrenDeclared(children.Item(i)->m_Children, search, result, tokenKindMask, partialMatch, noChildrenOf);
            }
        }
    }
}

void ParserF::FindMatchVariablesInModules(const wxString& search, TokensArrayFlat& result, bool partialMatch)
{
    wxString searchLw = search.Lower();

    wxCriticalSectionLocker locker(s_CritSect);

    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        TokensArrayF* children = &m_pTokens->Item(i)->m_Children;
        for (size_t j=0; j<children->GetCount(); j++)
        {
            if ( children->Item(j)->m_TokenKind == tkModule )
            {
                TokensArrayF* modChildren = &children->Item(j)->m_Children;
                for (size_t k=0; k<modChildren->GetCount(); k++)
                {
                    if ( (modChildren->Item(k)->m_TokenKind == tkVariable)
                        && ( ( partialMatch && modChildren->Item(k)->m_Name.StartsWith(searchLw) ) ||
                             ( !partialMatch && modChildren->Item(k)->m_Name.IsSameAs(searchLw) ) ) )
                    {
                        result.Add(new TokenFlat(modChildren->Item(k)));
                    }
                }
            }
        }
    }
}

bool ParserF::FindMatchTypeComponents(cbEditor* ed, const wxString& lineCur, TokensArrayFlat& result, bool partialMatch,
                                      bool& isAfterPercent, bool getAsProcedure)
{
    wxString line = lineCur.Lower();
    isAfterPercent = false;
    line = line.AfterLast(';');
    int idx = line.Find(_T("%"));
    if (idx == wxNOT_FOUND)
        return true;
    if (line.EndsWith(_T(" ")))
    {
        wxString tmpString = line.Trim();
        if (!tmpString.EndsWith(_T("%")))
            return true;
    }
    else if (line.EndsWith(_T(")")) || line.EndsWith(_T("(")) || line.EndsWith(_T(",")))
        return true;
    int idx_a = line.Find('(', true);
    int idx_b = line.Find(')', true);
    if ((idx_a != wxNOT_FOUND && idx_b == wxNOT_FOUND) || (idx_a > idx_b))
        line = line.Mid(idx_a+1);

    CutBlocks('(', line);

    idx_a = line.Find(',', true);
    if (idx_a != wxNOT_FOUND)
        line = line.Mid(idx_a+1);
    idx = line.Find('=',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find('>',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find('<',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find('.',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find('/',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find('*',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find('-',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find('+',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find(':',true);
    if (idx != wxNOT_FOUND)
        line = line.Mid(idx+1);
    idx = line.Find('%');
    if (idx == wxNOT_FOUND)
        return true;

    isAfterPercent = true;

    wxArrayString parts;
    wxStringTokenizer tkz(line, _T("%"), wxTOKEN_RET_EMPTY_ALL);
    while ( tkz.HasMoreTokens() )
    {
        wxString str = tkz.GetNextToken();
        wxStringTokenizer tkz2(str, _T(" \t\r\n"), wxTOKEN_STRTOK);
        if (tkz2.CountTokens() > 1)
        {
            // something is wrong. Try further
            while ( tkz2.HasMoreTokens() )
            {
                str = tkz2.GetNextToken();
            }
            if (parts.Count() > 0)
                parts.Empty();
        }
        parts.Add(str.Trim(false).Trim());
    }
    if (parts.Count() == 1)
        return false; // something wrong
    for (size_t i=0; i<parts.Count()-1; i++)
    {
        if (parts.Item(i).IsEmpty())
        {
            return false; // something wrong
        }
    }
    int nTypes = parts.Count() - 1;
    wxString searchName = parts.Item(parts.Count()-1);
    wxString name = parts.Item(0);

    TokensArrayFlatClass tokensTmp;
    TokensArrayFlat* resultTmp = tokensTmp.GetTokens();
    FindMatchDeclarationsInCurrentScope(name, ed, *resultTmp, false);

    if (resultTmp->Count() == 0)
    {
        //Is it global variable?
        FindMatchVariablesInModules(name, *resultTmp, false);
    }

    wxString nameType;
    bool nameType_found = false;
    if (resultTmp->Count() == 0)
    {
        return false; //Type was not found.
    }
    else
    {
        for (size_t i=0; i<resultTmp->Count(); i++)
        {
            TokenF* tok = resultTmp->Item(i);
            if ( tok->m_TokenKind == tkVariable )
            {
                wxString tDefLow = tok->m_TypeDefinition.Lower();
                if ( tDefLow.StartsWith(_T("type")) || tDefLow.StartsWith(_T("class")) )
                {
                    nameType = tDefLow;
                    idx_a = nameType.Find(_T(")"));
                    idx_b = nameType.Find(_T("("));
                    if (idx_a != wxNOT_FOUND && idx_b != wxNOT_FOUND && idx_a > idx_b+1)
                    {
                        nameType = nameType.Mid(idx_b+1,idx_a-idx_b-1);
                        nameType_found = true;
                        break;
                    }
                }
            }
        }
        if (!nameType_found)
            return false; // something is wrong
    }
    wxString nameTypeCom = nameType;
    for (int i=1; i<nTypes; i++)
    {
        bool found = GetTypeOfComponent(nameType, parts.Item(i), nameTypeCom);
        if (!found)
            return false; // something is wrong
        nameType = nameTypeCom;
    }
    TokenF* typeToken = GetType(nameTypeCom);
    if(!typeToken)
        return false; // type was not found
    for (int icyc=0; icyc < 30; icyc++)  // if icyc >= 30, definitely something is wrong
    {
        for (size_t i=0; i<typeToken->m_Children.GetCount(); i++)
        {
            TokenF* tokenCh = typeToken->m_Children.Item(i);
            if ( (partialMatch && (tokenCh->m_Name.StartsWith(searchName))) ||
                 (!partialMatch && (tokenCh->m_Name.IsSameAs(searchName))) )
            {
                if (tokenCh->m_TokenKind == tkVariable)
                {
                    result.Add(new TokenFlat(tokenCh));
                }
                else if (tokenCh->m_TokenKind == tkProcedure)
                {
                    TokenFlat* tokProc = new TokenFlat(tokenCh);
                    if (!getAsProcedure)
                    {
                        wxString tokName;
                        if (!tokenCh->m_PartLast.IsEmpty())
                            tokName = tokenCh->m_PartLast;
                        else
                            tokName = tokenCh->m_Name;

                        // what is kind of procedure ?
                        TokensArrayFlatClass tokensTmp;
                        TokensArrayFlat* resultTmp = tokensTmp.GetTokens();
                        int kindMask = tkFunction | tkSubroutine;

                        int noInChildren = tkInterface | tkFunction | tkSubroutine;
                        bool found = FindMatchTokenInSameModule(tokenCh, tokName, *resultTmp, kindMask, noInChildren);
                        if (!found)
                            FindMatchTokensDeclared(tokName, *resultTmp, kindMask, false, noInChildren);
                        if (resultTmp->GetCount() > 0)
                            tokProc->m_TokenKind = resultTmp->Item(0)->m_TokenKind;
                    }
                    result.Add(tokProc);
                }
                else if (tokenCh->m_TokenKind == tkInterface)
                {
                    result.Add(new TokenFlat(tokenCh));
                }
            }
        }
        if ( (partialMatch && !typeToken->m_ExtendsType.IsEmpty() && typeToken->m_ExtendsType.Lower().StartsWith(searchName)) ||
            (!partialMatch && !typeToken->m_ExtendsType.IsEmpty() && typeToken->m_ExtendsType.Lower().IsSameAs(searchName)) )
        {
            TokenF* newToken = new TokenF;
            newToken->m_Name = typeToken->m_ExtendsType.Lower();
            newToken->m_DisplayName = typeToken->m_ExtendsType;
            newToken->m_TokenKind = tkType;
            newToken->m_pParent = typeToken;
            result.Add(new TokenFlat(newToken));
        }
        if (!typeToken->m_ExtendsType.IsEmpty())
        {
            typeToken = GetType(typeToken->m_ExtendsType.Lower());
            if(!typeToken)
                break; // type was not found
        }
        else
        {
            break;
        }
    }

    return true;
}

void ParserF::FindChildrenOfTokenInWorkspace(wxString nameToken, TokensArrayF& result, int tokenKindMask, bool partialMatch)
{
    wxString searchLw = nameToken.Lower();

    for (size_t i=0; i < m_pTokens->GetCount(); i++)
    {
        if (m_pTokens->Item(i)->m_Children.GetCount() > 0)
        {
            FindChildrenOfToken(m_pTokens->Item(i)->m_Children, searchLw, result, tokenKindMask, partialMatch);
        }
    }
}

void ParserF::FindChildrenOfToken(TokensArrayF &children, const wxString &nameToken, TokensArrayF& result,  int tokenKindMask, bool partialMatch)
{
    if (partialMatch)
    {
        for (size_t i=0; i<children.GetCount(); i++)
        {
            if (children.Item(i)->m_Name.StartsWith(nameToken) && (children.Item(i)->m_TokenKind & tokenKindMask))
            {
                for (size_t j=0; j < children.Item(i)->m_Children.GetCount(); j++)
                {
                    result.Add(children.Item(i)->m_Children.Item(j));
                }
            }
            if (children.Item(i)->m_Children.GetCount() > 0)
            {
                FindChildrenOfToken(children.Item(i)->m_Children, nameToken, result, tokenKindMask, partialMatch);
            }
        }
    }
    else
    {
        for (size_t i=0; i<children.GetCount(); i++)
        {
            if (children.Item(i)->m_Name.IsSameAs(nameToken) && (children.Item(i)->m_TokenKind & tokenKindMask))
            {
                for (size_t j=0; j < children.Item(i)->m_Children.GetCount(); j++)
                {
                    result.Add(children.Item(i)->m_Children.Item(j));
                }
            }
            if (children.Item(i)->m_Children.GetCount() > 0)
            {
                FindChildrenOfToken(children.Item(i)->m_Children, nameToken, result, tokenKindMask, partialMatch);
            }
        }
    }
}


void ParserF::FindMatchDeclarationsInCurrentScope(const wxString& search, cbEditor* ed, TokensArrayFlat& result, bool partialMatch, int endPos)
{
    int lineStart = -1;
    wxString searchLw = search.Lower();

    wxString filename = ed->GetFilename();
    FortranSourceForm fsForm;
    if (!IsFileFortran(filename, fsForm))
        return;

    cbStyledTextCtrl* control = ed->GetControl();

    int curPos;
    if (endPos == -1)
    {
        curPos = control->GetCurrentPos();
    }
    else
    {
        curPos = endPos;
    }

    unsigned int curLine = control->LineFromPosition(curPos) + 1;
    int tokenKindMask = tkFunction | tkProgram | tkSubroutine | tkModule; // | tkType;

    //Parse to find a scope
    unsigned int parseStartLine;
    if (curLine <= 100)
        parseStartLine = 1;
    else
        parseStartLine = curLine - 100;

    wxString strRange;
    int linesUntil;
    if (parseStartLine == 1)
    {
        strRange = control->GetTextRange(0,curPos);
        linesUntil = 0;
    }
    else
    {
        strRange = control->GetTextRange(control->GetLineEndPosition(parseStartLine-2),curPos);
        linesUntil = parseStartLine - 2;
    }

    TokensArrayClass tTemp;
    TokensArrayF* pRes = tTemp.GetTokens();
    ParserThreadF parsTh = ParserThreadF(strRange, pRes, fsForm, true);
    bool res = parsTh.Parse();
    if (res)
    {
        FindLineScope(curLine, lineStart, tokenKindMask, *pRes);
    }

    if (lineStart == -1)
    {
        //Find scope between file tokens
        wxCriticalSectionLocker locker(s_CritSect);
        TokensArrayF* children = FindFileTokens(filename);
        if (!children)
            return;
        for (size_t i=0; i<children->GetCount(); i++)
        {
            if ((children->Item(i)->m_LineStart <= parseStartLine) && (children->Item(i)->m_TokenKind & tokenKindMask))
            {
                lineStart = children->Item(i)->m_LineStart;
                if (FindLineScope(parseStartLine, lineStart, tokenKindMask, children->Item(i)->m_Children))
                {
                    break;
                }
            }
            else if (children->Item(i)->m_LineStart > parseStartLine)
            {
                break;
            }
        }
    }
    else
    {
        lineStart += linesUntil;
    }

    if (lineStart == -1)
        return;

    wxString txtRange = control->GetTextRange(control->GetLineEndPosition(lineStart-1),curPos);

    //Parse
    TokensArrayClass tokensTmp;
    TokensArrayF* parsResult = tokensTmp.GetTokens();
    ParserThreadF thread = ParserThreadF(txtRange, parsResult, fsForm, true);
    thread.ParseDeclarations();

    //Add results
    if(partialMatch)
    {
        for (size_t i=0; i<parsResult->GetCount(); i++)
        {
            if (parsResult->Item(i)->m_Name.StartsWith(searchLw))
            {
                parsResult->Item(i)->m_Filename = filename;
                parsResult->Item(i)->m_LineStart += lineStart;
                result.Add(new TokenFlat(parsResult->Item(i)));
            }
        }
    }
    else
    {
        for (size_t i=0; i<parsResult->GetCount(); i++)
        {
            if (parsResult->Item(i)->m_Name.IsSameAs(searchLw))
            {
                parsResult->Item(i)->m_Filename = filename;
                parsResult->Item(i)->m_LineStart += lineStart;
                result.Add(new TokenFlat(parsResult->Item(i)));
            }
        }
    }
    return;
}

bool ParserF::FindLineScope(unsigned int line, int& lineStart, int tokenKindMask, TokensArrayF& children)
{
    bool found = false;
    for (size_t i=0; i<children.GetCount(); i++)
    {
        if ((children.Item(i)->m_LineStart <= line) && (children.Item(i)->m_TokenKind & tokenKindMask))
        {
            lineStart = children.Item(i)->m_LineStart;
            if (FindLineScope(line, lineStart, tokenKindMask, children.Item(i)->m_Children))
            {
                found = true;
                break;
            }
        }
        else if (children.Item(i)->m_LineStart > line)
        {
            found = true;
            break;
        }
    }
    return found;
}


TokensArrayF* ParserF::FindFileTokens(const wxString& filename)
{
    wxString fn = UnixFilename(filename);
    TokensArrayF* children=0;
    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        if (m_pTokens->Item(i)->m_TokenKind == tkFile && (m_pTokens->Item(i)->m_Filename.IsSameAs(fn)))
        {
            children = &m_pTokens->Item(i)->m_Children;
            break;
        }
    }
    return children;
}


size_t ParserF::FindMatchTokens(wxString filename, wxString search, TokensArrayF& result)
{
    filename = UnixFilename(filename);
    search = search.Lower();

    wxCriticalSectionLocker locker(s_CritSect);

    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        if (m_pTokens->Item(i)->m_Filename.IsSameAs(filename))
        {
            if (m_pTokens->Item(i)->m_Children.GetCount() > 0)
            {
                FindMatchChildren(m_pTokens->Item(i)->m_Children, search, result);
            }
        }
    }
    return result.GetCount();
}

void ParserF::FindMatchChildren(TokensArrayF &m_Children, wxString search, TokensArrayF& result, bool exact)
{
    for (size_t i=0; i<m_Children.GetCount(); i++)
    {
        if (exact)
        {
            if (m_Children.Item(i)->m_Name.IsSameAs(search))
            {
                result.Add(m_Children.Item(i));
            }
        }
        else
        {
            if (!(m_Children.Item(i)->m_Name.Find(search) == wxNOT_FOUND))
            {
                result.Add(m_Children.Item(i));
            }
        }
        if (m_Children.Item(i)->m_Children.GetCount() > 0)
        {
            FindMatchChildren(m_Children.Item(i)->m_Children, search, result, exact);
        }
    }
}

void ParserF::Clear()
{
    m_Done = false;
    wxCriticalSectionLocker locker(s_CritSect);

    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        m_pTokens->Item(i)->Clear();
        delete m_pTokens->Item(i);
    }
    m_pTokens->Clear();
    m_Done = true;
}

void ParserF::ObtainUsedDeclaredModules(const wxString& fileName, StringSet* fileUseModules, StringSet* fileDeclaredModules, StringSet* fileIncludes)
{
    wxCriticalSectionLocker locker(s_CritSect);

    int idx = GetFileIndex(fileName);
    if (idx == -1)
        return;
    TokenF* tok = m_pTokens->Item(idx);

    ObtainUDModulesToken(tok, fileUseModules, fileDeclaredModules, fileIncludes);
}

void ParserF::ObtainUDModulesToken(TokenF* token, StringSet* fileUseModules, StringSet* fileDeclaredModules, StringSet* fileIncludes)
{
    for (size_t i=0; i < token->m_Children.GetCount(); i++)
    {
        if (token->m_Children.Item(i)->m_TokenKind == tkUse)
        {
            fileUseModules->insert(token->m_Children.Item(i)->m_Name);
        }
        else if (token->m_Children.Item(i)->m_TokenKind == tkModule)
        {
            fileDeclaredModules->insert(token->m_Children.Item(i)->m_Name);
        }
        else if (token->m_Children.Item(i)->m_TokenKind == tkInclude)
        {
            fileIncludes->insert(token->m_Children.Item(i)->m_Name);
        }

        if (token->m_Children.Item(i)->m_Children.GetCount() > 0)
            ObtainUDModulesToken(token->m_Children.Item(i), fileUseModules, fileDeclaredModules, fileIncludes);
    }
}

size_t ParserF::GetFileIndex(const wxString& filename)
{
    wxString fn = UnixFilename(filename);
    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        if (m_pTokens->Item(i)->m_Filename.IsSameAs(fn))
        {
            return i;
        }
    }
    return -1;
}

bool ParserF::IsFileFortran(const wxString& filename, FortranSourceForm& fsForm)
{
    if (!m_ExtDone)
    {
//        EditorManager* edMan = Manager::Get()->GetEditorManager();
//        cbEditor* ed = edMan->GetBuiltinActiveEditor();
//        EditorColourSet* theme = ed->GetColourSet();
//        HighlightLanguage lang1 = _T("Fortran");
//        HighlightLanguage lang2 = _T("Fortran77");
//
//        const wxArrayString fileMasks1 = theme->GetFileMasks(lang1);
//        for(size_t i=0; i<fileMasks1.GetCount(); i++)
//        {
//            m_FortranExtFree.insert(fileMasks1.Item(i).AfterLast(_T('.')).Lower());
//        }
//        const wxArrayString fileMasks2 = theme->GetFileMasks(lang2);
//        for(size_t i=0; i<fileMasks2.GetCount(); i++)
//        {
//            m_FortranExtFixed.insert(fileMasks2.Item(i).AfterLast(_T('.')).Lower());
//        }
//        m_ExtDone = true;

        ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));

        wxString extl = cfg->Read(_T("/extension_fixed"), _T("for, f77, f, fpp"));
        wxStringTokenizer tkz(extl, _T(" ;,*.\t\r\n"), wxTOKEN_STRTOK);
        while ( tkz.HasMoreTokens() )
        {
            wxString token = tkz.GetNextToken();
            m_FortranExtFixed.insert(token);
        }

        extl = cfg->Read(_T("/extension_free"), _T("f90, f95, f03, f2k"));
        tkz.SetString(extl, _T(" ;,*.\t\r\n"), wxTOKEN_STRTOK);
        while ( tkz.HasMoreTokens() )
        {
            wxString token = tkz.GetNextToken();
            m_FortranExtFree.insert(token);
        }

        m_ExtDone = true;
    }

    bool isf;
    wxString ext = filename.AfterLast(_T('.')).Lower();

    if (m_FortranExtFree.count(ext))
    {
        fsForm = fsfFree;
        isf = true;
    }
    else if (m_FortranExtFixed.count(ext))
    {
        fsForm = fsfFixed;
        isf = true;
    }
    else
        isf = false;

   return isf;
}

void ParserF::RereadOptions()
{
    m_ExtDone = false;
    m_FortranExtFixed.clear();
    m_FortranExtFree.clear();
}

void ParserF::FindMatchTokensForToolTip(const wxString& nameUnder, int posEndOfWord, cbEditor* ed, TokensArrayFlat& result, bool& isAfterPercent)
{
    isAfterPercent = false;
    if (!ed)
        return;
    cbStyledTextCtrl* control = ed->GetControl();
    if (!control)
        return;
    int lineStartPos = control->GetLineEndPosition(control->LineFromPosition(posEndOfWord) - 1) + 1;
    wxString curLine = control->GetTextRange(lineStartPos,posEndOfWord);

    TokensArrayFlatClass tokensTemp;
    TokensArrayFlat* resultTemp = tokensTemp.GetTokens();
    if (!FindMatchTypeComponents(ed, curLine, *resultTemp, false, isAfterPercent, true))
        return;
    if (resultTemp->GetCount() > 0)
    {
        TokenFlat* token = resultTemp->Item(0); // we take only first added item
        result.Add( new TokenFlat(token) );
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
            bool found = FindMatchTokenInSameModule(token, tokName, *resultTmp, kindMask, noInChildren);
            if (!found)
                FindMatchTokensDeclared(tokName, *resultTmp, kindMask, false, noInChildren);
            if (resultTmp->GetCount() > 0)
                result.Add( new TokenFlat(resultTmp->Item(0)) );
        }
        else if (token->m_TokenKind == tkInterface)
        {
            FindGenericTypeBoudComponents(token, result);
            for (size_t i=1; i<resultTemp->GetCount(); i++)
            {
                if (resultTemp->Item(i)->m_TokenKind == tkInterface)
                {
                    result.Add( new TokenFlat(resultTemp->Item(i)));
                    FindGenericTypeBoudComponents(resultTemp->Item(i), result);
                }
            }
        }
    }

    if (!isAfterPercent)
    {
        int tokKind = tkModule | tkFunction | tkProgram | tkSubroutine | tkPreprocessor | tkInterface | tkBlockData | tkType;
        FindMatchTokensDeclared(nameUnder, result, tokKind, false);
        FindMatchDeclarationsInCurrentScope(nameUnder, ed, result, false, posEndOfWord);
        FindMatchVariablesInModules(nameUnder, result, false);
    }
}

void ParserF::FindGenericTypeBoudComponents(TokenFlat* token, TokensArrayFlat& result)
{
    if (token->m_TokenKind != tkInterface)
        return;

    if (token->m_PartLast.IsEmpty())
        return;

    wxArrayString specNames;
    wxStringTokenizer tkz(token->m_PartLast, _T(" \t\r\n"), wxTOKEN_STRTOK);
    while ( tkz.HasMoreTokens() )
    {
        specNames.Add(tkz.GetNextToken().Lower());
    }
    TokensArrayFlatClass procTokenArrTmp;
    TokensArrayFlat* procTokenArr = procTokenArrTmp.GetTokens();
    if (!FindTypeBoundProcedures(token, specNames, *procTokenArr))
        return;
    int kindMask = tkFunction | tkSubroutine;
    int noInChildren = tkInterface | tkFunction | tkSubroutine;
    for (size_t i=0; i<procTokenArr->Count(); i++)
    {
        wxString tokName;
        if (!procTokenArr->Item(i)->m_PartLast.IsEmpty())
            tokName = procTokenArr->Item(i)->m_PartLast;
        else
            tokName = procTokenArr->Item(i)->m_Name;

        TokensArrayFlatClass tokensTmp;
        TokensArrayFlat* resultTmp = tokensTmp.GetTokens();
        bool found = FindMatchTokenInSameModule(procTokenArr->Item(i), tokName, *resultTmp, kindMask, noInChildren);
        if (!found)
            FindMatchTokensDeclared(tokName, *resultTmp, kindMask, false, noInChildren);
        if (resultTmp->GetCount() > 0)
        {
            result.Add( new TokenFlat(procTokenArr->Item(i)) );
            result.Add( new TokenFlat(resultTmp->Item(0)) );
        }
    }
}


void ParserF::FindMatchTokensForJump(cbEditor* ed, TokensArrayFlat& result)
{
    bool isAfterPercent = false;
    if (!ed)
        return;
    cbStyledTextCtrl* control = ed->GetControl();
    if (!control)
        return;
    int pos = control->GetCurrentPos();
    int posEndOfWord = control->WordEndPosition(pos, true);
    int posStartOfWord = control->WordStartPosition(pos, true);
    wxString nameUnder = control->GetTextRange(posStartOfWord, posEndOfWord);
    if (nameUnder.IsEmpty())
        return;
    int lineStartPos = control->GetLineEndPosition(control->LineFromPosition(posEndOfWord) - 1) + 1;
    wxString curLine = control->GetTextRange(lineStartPos,posEndOfWord);

    if (!FindMatchTypeComponents(ed, curLine, result, false, isAfterPercent, true))
        return;

    if (!isAfterPercent)
    {
        int tokKind = tkModule | tkFunction | tkProgram | tkSubroutine | tkPreprocessor | tkInterface | tkBlockData | tkType;
        FindMatchTokensDeclared(nameUnder, result, tokKind, false);
        FindMatchDeclarationsInCurrentScope(nameUnder, ed, result, false, posEndOfWord);
        FindMatchVariablesInModules(nameUnder, result, false);
    }
}


bool ParserF::FindMatchTokensForCodeCompletion(bool useSmartCC, const wxString& nameUnderCursor, cbEditor* ed,
                                               TokensArrayFlat& result, bool& isAfterPercent, int& tokKind)
{

    wxString curLine;
    wxArrayString firstWords;
    if (!FindWordsBefore(ed, 100, curLine, firstWords))  //get words on the line
        return false;

    bool allowVariables;
    CCSmartFilter::GetTokenKind(firstWords, tokKind, allowVariables);

    isAfterPercent = false;
    if (!FindMatchTypeComponents(ed, curLine, result, true, isAfterPercent))
        return true;

    if (!isAfterPercent)
    {
        if (!useSmartCC)
        {
            tokKind = tkFunction | tkProgram | tkSubroutine | tkPreprocessor | tkInterface | tkBlockData | tkType;
            allowVariables = true;
        }
        FindMatchTokensDeclared(nameUnderCursor, result, tokKind, true, tkInterface);
        if (allowVariables)
        {
            FindMatchVariablesInModules(nameUnderCursor, result, true);
            FindMatchDeclarationsInCurrentScope(nameUnderCursor, ed, result, true);
        }
        else if (tokKind & tkSubroutine)
        {
            if (firstWords.GetCount() > 0 && firstWords.Item(0).Lower().IsSameAs(_T("call")))
            {
                TokensArrayFlatClass tokensTmp;
                TokensArrayFlat* resTmp = tokensTmp.GetTokens();

                FindMatchVariablesInModules(nameUnderCursor, *resTmp, true);
                FindMatchDeclarationsInCurrentScope(nameUnderCursor, ed, *resTmp, true);

                for (size_t i=0; i<resTmp->Count(); i++)
                {
                    TokenF* tok = resTmp->Item(i);
                    if ( tok->m_TokenKind == tkVariable )
                    {
                        wxString tDefLow = tok->m_TypeDefinition.Lower();
                        if ( tDefLow.StartsWith(_T("type")) || tDefLow.StartsWith(_T("class")) )
                        {
                            result.Add(new TokenFlat(tok));
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool ParserF::FindWordsBefore(cbEditor* ed, int numberOfWords, wxString &curLine, wxArrayString &firstWords)
{
    /* Finds word before current word (first word).
    */
    if (!ed)
        return false;
    cbStyledTextCtrl* control = ed->GetControl();
    if (!control)
        return false;
    int pos   = control->GetCurrentPos();
    int lineCur = control->LineFromPosition(pos);
    int lineStartPos = control->PositionFromLine(lineCur);
    curLine = control->GetTextRange(lineStartPos,pos);
    if (curLine.Find('!') != wxNOT_FOUND)
        return false; // we are in comments
    wxString line = curLine;

    for (int i=lineCur-1; i>=0; i--)
    {
        wxString tmpLine = control->GetLine(i).BeforeFirst('!').Trim();
        if (tmpLine.EndsWith(_T("&")))
        {
            // current line is continuation line
            tmpLine = tmpLine.BeforeLast('&').Trim();
            if (!tmpLine.IsEmpty())
            {
                line.Prepend(_T(" "));
                line.Prepend(tmpLine);
            }
        }
        else if (!tmpLine.IsEmpty())
        {
            break;
        }
    }

    // end of current word
    bool found = false;
    int idx;
    for (int i=line.Len()-1; i>=0; i--)
    {
        if (!isalnum(line.GetChar(i)) && (line.GetChar(i) != '_'))
        {
            found = true;
            idx = i;
            break;
        }
    }
    if (!found)
    {
        firstWords.Add(wxEmptyString);
        return true;
    }

    for (int nword=0; nword<numberOfWords; nword++)
    {
        // end of first word
        int idx_end = -1;
        for (int i=idx; i>=0; i--)
        {
            if (!isspace(line.GetChar(i)))
            {
                idx_end = i;
                break;
            }
        }
        if (idx_end == -1)
        {
            if (firstWords.Count() == 0)
                firstWords.Add(wxEmptyString);
            break;
        }
        else if (!isalnum(line.GetChar(idx_end)) && (line.GetChar(idx_end) != '_'))
        {
            firstWords.Add(line.GetChar(idx_end));
            idx = idx_end - 1;
        }
        else
        {
            // start of first word
            int idx_start = 0;
            for (int i=idx_end-1; i>=0; i--)
            {
                if (!isalnum(line.GetChar(i)) && (line.GetChar(i) != '_'))
                {
                    idx_start = i + 1;
                    break;
                }
            }
            firstWords.Add(line.Mid(idx_start, idx_end-idx_start+1));
            idx = idx_start - 1;
        }
    }
    return true;
}

bool ParserF::CutBlocks(const wxChar& ch, wxString& line)
{
	// cut blocks () [] {} <>
	wxChar match;
	switch (ch)
	{
		case '(': match = ')'; break;
		case '[': match = ']'; break;
		case '{': match = '}'; break;
		case '<': match = '>'; break;
		default : return false;
	}

	int count = 0; // counter for nested blocks (xxx())
	int i = 0;
	int start = i;
	int end;
	while (i < (int)line.length())
	{
	    while (i < (int)line.length())
	    {
            if (line.GetChar(i) == '"' || line.GetChar(i) == '\'')
            {
                // this is the case that match is inside a string!
                char cha = line.GetChar(i);
                i++;
                while (i < (int)line.length())
                {
                    if (line.GetChar(i) == cha)
                        break;
                    else
                        i++;
                }
                i++;
            }
            else
                break;
        }
		if (line.GetChar(i) == ch)
		{
		    if (count == 0)
                start = i;
			count++;
		}
		else if (line.GetChar(i) == match)
		{
		    if (count == 1)
		    {
                end = i;
                wxString line_new = line.Mid(0,start);
                if (end+1 < (int)line.length())
                    line_new.Append(line.Mid(end+1));
                line = line_new;
                i = start-1;
		    }
			count--;
		}
		i++;
	}
	return true;
}

bool ParserF::GetTypeOfComponent(const wxString& nameType, const wxString& nameComponent, wxString& nameTypeComponent)
{
    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        TokenF* pfToken = m_pTokens->Item(i);
        for (size_t j=0; j<pfToken->m_Children.GetCount(); j++)
        {
            TokenF* pToken = pfToken->m_Children.Item(j);
            if (pToken->m_TokenKind == tkModule)
            {
                for (size_t k=0; k<pToken->m_Children.GetCount(); k++)
                {
                    if (pToken->m_Children.Item(k)->m_TokenKind == tkType)
                    {
                        TokenF* pT = pToken->m_Children.Item(k);
                        if (pT->m_Name.IsSameAs(nameType))
                        {
                            if (GetTypeOfChild(pT, nameComponent, nameTypeComponent))
                                return true;

                            //May be nameComponent is parent type?
                            if (!pT->m_ExtendsType.IsEmpty() && pT->m_ExtendsType.Lower().IsSameAs(nameComponent))
                            {
                                nameTypeComponent = pT->m_ExtendsType.Lower();
                                return true;
                            }
                            else if (!pT->m_ExtendsType.IsEmpty())
                            {
                                for (size_t l=0; l<30; l++)
                                {
                                    TokenF* typeToken = GetType(pT->m_ExtendsType.Lower());
                                    if(!typeToken)
                                        break; // type was not found
                                    if (GetTypeOfChild(typeToken, nameComponent, nameTypeComponent))
                                    {
                                        return true;
                                    }
                                    else if (!typeToken->m_ExtendsType.IsEmpty() && typeToken->m_ExtendsType.Lower().IsSameAs(nameComponent))
                                    {
                                        nameTypeComponent = typeToken->m_ExtendsType.Lower();
                                        return true;
                                    }
                                    else if(!typeToken->m_ExtendsType.IsEmpty())
                                    {
                                        pT = typeToken;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool ParserF::GetTypeOfChild(TokenF* pT, const wxString& nameComponent, wxString& nameTypeComponent)
{
    for (size_t l=0; l<pT->m_Children.GetCount(); l++)
    {
        if ((pT->m_Children.Item(l)->m_Name.IsSameAs(nameComponent)) && (pT->m_Children.Item(l)->m_TokenKind == tkVariable))
        {
            wxString tdef = pT->m_Children.Item(l)->m_TypeDefinition;
            if (tdef.StartsWith(_T("type")))
            {
                int idx_a = tdef.Find(_T(")"));
                if (idx_a != wxNOT_FOUND)
                {
                    nameTypeComponent = tdef.Mid(5,idx_a-5);
                    return true;
                }
            }
            else
            {
                nameTypeComponent = tdef;
                return true;
            }
        }
    }
    return false;
}

TokenF* ParserF::GetType(const wxString& nameType)
{
    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        TokenF* pfToken = m_pTokens->Item(i);
        for (size_t j=0; j<pfToken->m_Children.GetCount(); j++)
        {
            TokenF* pToken = pfToken->m_Children.Item(j);
            if (pToken->m_TokenKind == tkModule)
            {
                for (size_t k=0; k<pToken->m_Children.GetCount(); k++)
                {
                    if (pToken->m_Children.Item(k)->m_TokenKind == tkType)
                    {
                        TokenF* pT = pToken->m_Children.Item(k);
                        if (pT->m_Name.IsSameAs(nameType))
                        {
                            return pT;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

bool ParserF::FindTokenDeclaration(TokenFlat& token, const wxString& argName, wxString& argDecl, wxString& argDescription)
{
    wxString txtRange;
    if (!FindTokenRange(token, txtRange))
        return false;

    FortranSourceForm fsForm;
    if (!IsFileFortran(token.m_Filename, fsForm))
        return false;
    //Parse
    TokensArrayClass tokensTmp;
    TokensArrayF* parsResult = tokensTmp.GetTokens();
    ParserThreadF thread = ParserThreadF(txtRange, parsResult, fsForm, true);
    thread.ParseDeclarations();

    bool found = false;
    wxString argNameLw = argName.Lower();
    for (size_t i=0; i<parsResult->GetCount(); i++)
    {
        if (parsResult->Item(i)->m_Name.IsSameAs(argNameLw))
        {
            argDecl << parsResult->Item(i)->m_TypeDefinition << _T(" :: ")
                    << parsResult->Item(i)->m_DisplayName << parsResult->Item(i)->m_Args;
            argDescription << parsResult->Item(i)->m_PartLast;
            found = true;
            break;
        }
    }
    return found;
}

bool ParserF::FindTokenRange(TokenFlat& token, wxString& txtRange)
{
    wxString buff;
    std::vector<int> lineStarts;
    return FindTokenRange(token, txtRange, buff, lineStarts);
}

bool ParserF::FindTokenRange(TokenFlat& token, wxString& txtRange, wxString& buff, std::vector<int> &lineStarts, bool withDefinition, bool readFile)
{
    if (!wxFileExists(token.m_Filename))
        return false;

    if (readFile)
    {
        lineStarts.clear();
        buff.Empty();
        wxFile file(token.m_Filename);
        if (!ReadFileToString(file,buff))
            return false;
        lineStarts.push_back(0);
    }
    else
    {
        //use buff content
    }

    //get range of token
    size_t pos_start=0;
    size_t pos_end=0;
    size_t line = 1;
    unsigned int lStart = token.m_LineStart;
    if (!withDefinition)
        lStart += token.m_DefinitionLength;


    bool startFound = false;
    bool endFound = false;
    for (size_t i=0; i<buff.Length(); i++)
    {
        if (!startFound && lStart <= line)
        {
            pos_start = i;
            startFound = true;
        }
        else if (!endFound && token.m_LineEnd < line)
        {
            pos_end = i;
            endFound = true;
            if (!readFile)
                break;
        }

        if (buff.GetChar(i) == '\n')
        {
            line++;
            if (readFile)
                lineStarts.push_back(i+1);
        }
    }
    if (!startFound)
        return false;

    if (pos_start > pos_end)
    {
        pos_end = buff.Length();
    }
    txtRange = buff.Mid(pos_start, pos_end - pos_start);
    return true;
}

bool ParserF::FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg)
{
    wxString argsNew = wxEmptyString;
    bool readFile = true;
    return FindInfoLog(token, logComAbove, logComBelow, logDeclar, logComVariab, msg, argsNew, readFile);
}

bool ParserF::FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg,
                          bool readFile)
{
    wxString argsNew = wxEmptyString;
    return FindInfoLog(token, logComAbove, logComBelow, logDeclar, logComVariab, msg, argsNew, readFile);
}

bool ParserF::FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg,
                          wxString& argsNew)
{
    bool readFile = true;
    return FindInfoLog(token, logComAbove, logComBelow, logDeclar, logComVariab, msg, argsNew, readFile);
}


bool ParserF::FindInfoLog(TokenFlat& token, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg,
                          const wxString& argsNew, bool readFile)
{
    wxString txtRange;
    if (!FindTokenRange(token, txtRange, m_Buff, m_LineStarts, false, readFile))
        return false;

    FortranSourceForm fsForm;
    if (!IsFileFortran(token.m_Filename, fsForm))
        return false;
    //Parse
    TokensArrayClass tokensTmp;
    TokensArrayF* parsResult = tokensTmp.GetTokens();
    ParserThreadF thread = ParserThreadF(txtRange, parsResult, fsForm, true);

    if (logComAbove)
    {
        // insert comments above
        wxArrayString comAbove;
        for (int i=token.m_LineStart-1; i>0; i--)
        {
            wxString str1 = m_Buff.Mid(m_LineStarts[i-1], m_LineStarts[i]-m_LineStarts[i-1]).Trim(false);
            if (str1.StartsWith(_T("!")))
            {
                comAbove.Add(str1);
            }
            else
            {
                break;
            }
        }

        for (int i=comAbove.GetCount()-1; i>=0; i--)
        {
            msg << comAbove.Item(i);
        }
    }

    thread.ParseDeclarations();

    if (token.m_TokenKind == tkSubroutine)
    {
        msg << _T("subroutine ") << token.m_DisplayName;
        if (argsNew.IsEmpty())
            msg << token.m_Args << _T("\n");
        else
            msg << argsNew << _T("\n");
    }
    else if (token.m_TokenKind == tkFunction)
    {
        if (!token.m_PartFirst.IsEmpty())
        {
            msg << token.m_PartFirst << _T(" ");
        }
        msg << _T("function ") << token.m_DisplayName;
        if (argsNew.IsEmpty())
            msg << token.m_Args;
        else
            msg << argsNew;
        if (!token.m_PartLast.IsEmpty())
        {
            msg << _T(" ") << token.m_PartLast;
        }
        msg << _T("\n");
    }

    if (logComBelow)
    {
        // insert comments below
        unsigned int lStart = token.m_LineStart + token.m_DefinitionLength;
        for (unsigned int i=lStart; i<token.m_LineEnd; i++)
        {
            wxString str1 = m_Buff.Mid(m_LineStarts[i-1], m_LineStarts[i]-m_LineStarts[i-1]).Trim(false);
            if (str1.StartsWith(_T("!")))
            {
                msg << _T("    ") << str1;
            }
            else
            {
                break;
            }
        }
    }

    if (logDeclar)
    {
        wxArrayString argArr;
        wxStringTokenizer tkz(token.m_Args, _T("(),[] \t\r\n"), wxTOKEN_STRTOK );
        while ( tkz.HasMoreTokens() )
        {
            argArr.Add(tkz.GetNextToken());
        }

        if (token.m_TokenKind == tkFunction)
        {
            wxString arg1;
            if (!token.m_ResultVariable.IsEmpty())
            {
                arg1 = token.m_ResultVariable.Lower();
            }
            else
            {
                arg1 = token.m_Name.Lower();
            }
            for (size_t i=0; i<parsResult->GetCount(); i++)
            {
                if (parsResult->Item(i)->m_Name.IsSameAs(arg1))
                {
                    msg << _T("    ") << parsResult->Item(i)->m_TypeDefinition << _T(" :: ")
                        << parsResult->Item(i)->m_DisplayName << parsResult->Item(i)->m_Args << _T("\n");
                    break;
                }
            }
        }

        wxArrayString argMsgArr;
        int maxLenArg = 0;
        std::vector<size_t> idxOrder;
        for (size_t j=0; j<argArr.Count(); j++)
        {
            wxString msg1;
            wxString arg1 = argArr.Item(j).Lower();
            for (size_t i=0; i<parsResult->GetCount(); i++)
            {
                if (parsResult->Item(i)->m_Name.IsSameAs(arg1))
                {
                    msg1 << _T("    ") << parsResult->Item(i)->m_TypeDefinition << _T(" :: ")
                        << parsResult->Item(i)->m_DisplayName << parsResult->Item(i)->m_Args;
                    idxOrder.push_back(i);
                    argMsgArr.Add(msg1);
                    int ln = msg1.Len();
                    if (ln > maxLenArg)
                        maxLenArg = ln;
                    break;
                }
            }
        }
        if (maxLenArg >= 60)
            maxLenArg = 60;

        for (size_t j=0; j<argMsgArr.Count(); j++)
        {
            msg << argMsgArr.Item(j);
            if (logComVariab)
            {
                wxString spaces;
                int nspaces = maxLenArg - argMsgArr.Item(j).Len() + 1;
                if (nspaces < 1)
                    nspaces = 1;
                spaces.Append(' ',nspaces);
                msg << spaces << parsResult->Item(idxOrder[j])->m_PartLast << _T("\n");
            }
            else
            {
                msg << _T("\n");
            }
        }
    }

    if (token.m_ParentTokenKind == tkModule)
    {
        msg << _("!Module: ") << token.m_ParentDisplayName << _(". File: ");
    }
    else
    {
        msg << _("!File: ");
    }
    msg << token.m_Filename.AfterLast(wxFILE_SEP_PATH) << _T(":") << token.m_LineStart;
    return true;
}

bool ParserF::FindTooltipForTypeBoundProc(wxString& msg, TokenFlat* token1, TokenFlat* token2)
{
    if (!token1 || token1->m_TokenKind != tkProcedure)
        return false;
    wxString txtRange;
    wxString buff;
    std::vector<int> lineStarts;
    if (!FindTokenRange(*token1, txtRange, buff, lineStarts, true))
        return false;
    int ic = txtRange.Find(_T("::"));
    if (ic == wxNOT_FOUND)
    {
        msg << _T("procedure ") << token1->m_DisplayName;
        if (!token1->m_Args.IsEmpty())
        {
            msg << _T("(") << token1->m_Args << _T(")");
        }
    }
    else
    {
        msg << txtRange.Mid(0,ic+2).Trim(false) << _T(" ") << token1->m_DisplayName;
    }
    if (!token1->m_PartLast.IsEmpty())
    {
        msg << _T(" => ") << token1->m_PartLast;
    }
    msg << _T("\n");

    if (token2)
    {
        if (token2->m_TokenKind == tkSubroutine || token2->m_TokenKind == tkFunction)
        {
            wxString pass_arg = token1->m_Args;
            int start = 0;
            int end = 0;
            if (token1->m_Pass && !pass_arg.IsEmpty())
            {
                GetPossitionOfDummyArgument(token2->m_Args, pass_arg, start, end);
            }
            else if (token1->m_Pass)
            {
                GetCallTipHighlight(token2->m_Args, 0, start, end);
            }
            wxString argNew;
            if (end > start)
            {
                argNew << token2->m_Args.Mid(0,start) << _T("[");
                wxString secPart = token2->m_Args.Mid(start);
                int icom = secPart.Find(_T(","));
                if (icom != wxNOT_FOUND)
                {
                    argNew << secPart.Mid(0,icom+1) << _T("]") << secPart.Mid(icom+1);
                }
                else
                {
                    argNew << token2->m_Args.Mid(start,end-start) << _T("]") << token2->m_Args.Mid(end);
                }
            }
            else
            {
                argNew = token2->m_Args;
            }
            if (token2->m_TokenKind == tkSubroutine)
            {
                msg << _T("subroutine ") << token2->m_DisplayName << argNew << _T("\n");
            }
            else if (token2->m_TokenKind == tkFunction)
            {
                if (!token2->m_PartFirst.IsEmpty())
                {
                    msg << token2->m_PartFirst << _T(" ");
                }
                msg << _T("function ") << token2->m_DisplayName << argNew << _T("\n");
            }
        }
    }
    if (!token1->m_Filename.IsEmpty())
    {
        msg << token1->m_Filename.AfterLast(wxFILE_SEP_PATH) << _T(":") << token1->m_LineStart;
    }
    return true;
}

bool ParserF::FindInfoLogForTypeBoundProc(TokensArrayFlat& tokenPair, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg,
                                          wxString* buff, std::vector<int>* lineStarts)
{
    if (tokenPair.GetCount() == 0)
    {
        return false;
    }

    TokenFlat* token1 = tokenPair.Item(0);
    if (token1->m_TokenKind != tkProcedure)
        return false;
    wxString txtRange;
    if (!buff)
    {
        buff= new wxString();
        lineStarts = new std::vector<int>;
        if (!FindTokenRange(*token1, txtRange, *buff, *lineStarts, true))
            return false;
        delete buff;
        delete lineStarts;
    }
    else
    {
        if (!FindTokenRange(*token1, txtRange, *buff, *lineStarts, true, false))
            return false;
    }
    int ic = txtRange.Find(_T("::"));
    if (ic == wxNOT_FOUND)
    {
        msg << _T("procedure ") << token1->m_DisplayName;
        if (!token1->m_Args.IsEmpty())
        {
            msg << _T("(") << token1->m_Args << _T(")");
        }
    }
    else
    {
        msg << txtRange.Mid(0,ic+2).Trim(false) << _T(" ") << token1->m_DisplayName;
    }
    if (!token1->m_PartLast.IsEmpty())
    {
        msg << _T(" => ") << token1->m_PartLast;
    }
    msg << _T("\n!File: ") << token1->m_Filename.AfterLast(wxFILE_SEP_PATH) << _T(":") << token1->m_LineStart << _T("\n");

    if (tokenPair.GetCount() > 1)
    {
        TokenFlat* token = tokenPair.Item(1);
        if (token->m_TokenKind == tkSubroutine || token->m_TokenKind == tkFunction)
        {
            wxString pass_arg = token1->m_Args;
            int start = 0;
            int end = 0;
            if (token1->m_Pass && !pass_arg.IsEmpty())
            {
                GetPossitionOfDummyArgument(token->m_Args, pass_arg, start, end);
            }
            else if (token1->m_Pass)
            {
                GetCallTipHighlight(token->m_Args, 0, start, end);
            }
            if (end > start)
            {
                wxString argNew;
                argNew << token->m_Args.Mid(0,start) << _T("[");
                wxString secPart = token->m_Args.Mid(start);
                int icom = secPart.Find(_T(","));
                if (icom != wxNOT_FOUND)
                {
                    argNew << secPart.Mid(0,icom+1) << _T("]") << secPart.Mid(icom+1);
                }
                else
                {
                    argNew << token->m_Args.Mid(start,end-start) << _T("]") << token->m_Args.Mid(end);
                }
                FindInfoLog(*token, logComAbove, logComBelow, logDeclar, logComVariab, msg, argNew);
            }
            else
            {
                FindInfoLog(*token, logComAbove, logComBelow, logDeclar, logComVariab, msg);
            }
        }
    }
    return true;
}

bool ParserF::FindInfoLogForGenericTBProc(TokensArrayFlat& tokens, bool logComAbove, bool logComBelow, bool logDeclar, bool logComVariab, wxString& msg)
{
    if (tokens.GetCount() == 0 || tokens.Item(0)->m_TokenKind != tkInterface)
    {
        return false;
    }

    wxString buff;
    std::vector<int> lineStarts;
    size_t iInt = 0;
    wxString filName;
    wxString msgProc;

    while ( iInt < tokens.GetCount() )
    {
        TokenFlat* token = tokens.Item(iInt);
        if (token->m_TokenKind != tkInterface)
            break;
        wxString tokRan;
        if (iInt == 0 || !filName.IsSameAs(token->m_Filename))
        {
            if (!FindTokenRange(*token, tokRan, buff, lineStarts, true))
                return false;
            filName = token->m_Filename;
        }
        else
        {
            if (!FindTokenRange(*token, tokRan, buff, lineStarts, true, false))
                return false;
        }
        msg.Append(_T("\n"));
        msg.Append( tokRan.Trim().Trim(false) );

        if (token->m_ParentTokenKind == tkType)
        {
            msg << _("\n!Type: ") << token->m_ParentDisplayName << _(". File: ");
        }
        msg << token->m_Filename.AfterLast(wxFILE_SEP_PATH) << _T(":") << token->m_LineStart;

        size_t i = iInt + 1;
        while ( i < tokens.GetCount()-1 )
        {
            if ( tokens.Item(i)->m_TokenKind == tkInterface )
                break;
            msgProc << _T("\n!---------------------\n");
            TokensArrayFlatClass tokensTmpCl;
            TokensArrayFlat* tokensTmp = tokensTmpCl.GetTokens();
            tokensTmp->Add(new TokenFlat(tokens.Item(i)));
            tokensTmp->Add(new TokenFlat(tokens.Item(i+1)));
            FindInfoLogForTypeBoundProc(*tokensTmp, logComAbove, logComBelow, logDeclar, logComVariab, msgProc, &buff, &lineStarts);
            i += 2;
        }
        iInt = i;
    }
    msg.Trim(false).Append(msgProc);

    return true;
}

bool ParserF::GetTokenStr(TokenFlat& token, wxString& msg)
{
    wxString buff;
    std::vector<int> lineStarts;
    if (!FindTokenRange(token, msg, buff, lineStarts, true))
        return false;

    if (token.m_ParentTokenKind == tkModule)
    {
        msg << _("\n!Module: ") << token.m_ParentDisplayName << _(". File: ");
    }
    else
    {
        msg << _("\n!File: ");
    }
    msg << token.m_Filename.AfterLast(wxFILE_SEP_PATH) << _T(":") << token.m_LineStart;
    return true;
}

void ParserF::FindChildrenOfInterface(TokenFlat* token, TokensArrayFlat& result)
{
    if (token->m_ParentTokenKind != tkModule)
        return;

    for (size_t i=0; i<m_pTokens->GetCount(); i++)
    {
        if (m_pTokens->Item(i)->m_Filename.IsSameAs(token->m_Filename))
        {
            TokensArrayF* pFileChildren = &m_pTokens->Item(i)->m_Children;
            for (size_t j=0; j < pFileChildren->GetCount(); j++)
            {
                if (pFileChildren->Item(j)->m_TokenKind == tkModule && pFileChildren->Item(j)->m_Name.IsSameAs(token->m_ParentName))
                {
                    TokensArrayF* pModChildren = &pFileChildren->Item(j)->m_Children;
                    for (size_t k=0; k < pModChildren->GetCount(); k++)
                    {
                        if (pModChildren->Item(k)->m_Name.IsSameAs(token->m_Name) && pModChildren->Item(k)->m_TokenKind == tkInterface)
                        {
                            TokensArrayF* pIntChildren = &pModChildren->Item(k)->m_Children;
                            for (size_t l=0; l < pIntChildren->GetCount(); l++)
                            {
                                if (pIntChildren->Item(l)->m_TokenKind == tkFunction || pIntChildren->Item(l)->m_TokenKind == tkSubroutine)
                                {
                                    result.Add(new TokenFlat(pIntChildren->Item(l)));
                                }
                                else
                                {
                                    wxString name = pIntChildren->Item(l)->m_Name;
                                    for (size_t m=0; m < pModChildren->GetCount(); m++)
                                    {
                                        if ( (pModChildren->Item(m)->m_TokenKind == tkFunction || pModChildren->Item(m)->m_TokenKind == tkSubroutine) &&
                                              pModChildren->Item(m)->m_Name.IsSameAs(name) )
                                        {
                                            result.Add(new TokenFlat(pModChildren->Item(m)));
                                            break;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
}

void ParserF::GetPossitionOfDummyArgument(const wxString& args, const wxString& arg, int& start, int& end)
{
    wxStringTokenizer tkz(args, _T(" ,\t\r\n()"), wxTOKEN_STRTOK);
    while ( tkz.HasMoreTokens() )
    {
        wxString token = tkz.GetNextToken();
        if (token.IsSameAs(arg))
        {
            end = tkz.GetPosition() - 1;
            start = end - token.Length();
            break;
        }
    }
}

void ParserF::GetCallTipHighlight(const wxString& calltip, int commasWas, int& start, int& end)
{
    int pos = 1; // skip opening parenthesis
    int nest = 0;
    int commas = 0;
    start = 1;
    end = 0;
    while (true)
    {
        wxChar c = calltip.GetChar(pos++);
        if (c == '\0')
            break;
        else if (c == '(')
            ++nest;
        else if (c == ')')
            --nest;
        else if (c == ',' && nest == 0)
        {
            ++commas;
            if (commas == commasWas)
            {
                start = pos;
            }
            else if (commas == commasWas + 1)
            {
                end = pos; // already incremented
                break;
            }
        }
    }
    if (end == 0)
        end = calltip.Length() - 1;
    if (commas < commasWas)
    {
        start = end; //no highlight
    }
}


