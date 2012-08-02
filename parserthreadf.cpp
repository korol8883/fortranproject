/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include "parserthreadf.h"
#include "usetokenf.h"
#include <set>

//#include <logmanager.h>

ParserThreadF::ParserThreadF(const wxString& bufferOrFilename,
							 TokensArrayF* tokens,
							 FortranSourceForm fsForm,
							 bool isBuffer,
							 IncludeDB* includeDB)
	:
	m_pTokens(tokens),
	m_pLastParent(0L),
	m_pIncludeDB(includeDB)
{
    m_InterfaceOperator = 0;
    m_InterfaceAssignment = 0;
    m_InterfaceRead = 0;
    m_InterfaceWrite = 0;

    if (!isBuffer)
    {
        if (!bufferOrFilename.IsEmpty())
        {
            m_Filename = bufferOrFilename;
            m_Tokens.Init(m_Filename, fsForm);
            wxFileName fn(bufferOrFilename);
            m_pLastParent = DoAddToken(tkFile, fn.GetFullName());
        }
    }
    else
    {
        m_Tokens.InitFromBuffer(bufferOrFilename, fsForm);
    }
	InitSecondEndPart();
}

ParserThreadF::ParserThreadF(const wxString& filename,
							 TokensArrayF* tokens,
							 FortranSourceForm fsForm,
							 IncludeDB* includeDB,
							 const wxString& buffer)
:
	m_pTokens(tokens),
	m_pLastParent(0L),
	m_pIncludeDB(includeDB)
{
    m_InterfaceOperator = 0;
    m_InterfaceAssignment = 0;
    m_InterfaceRead = 0;
    m_InterfaceWrite = 0;

    m_Filename = filename;
    m_Tokens.InitFromBuffer(buffer, fsForm);
    m_Tokens.SetFilename(filename);
    wxFileName fn(filename);
    m_pLastParent = DoAddToken(tkFile, fn.GetFullName());

	InitSecondEndPart();
}

ParserThreadF::~ParserThreadF()
{
    //dtor
}

void ParserThreadF::InitSecondEndPart()
{
    //m_EndSecPart.insert(_T("associate"));
    m_EndSecPart.insert(_T("do"));
    m_EndSecPart.insert(_T("enum"));
    m_EndSecPart.insert(_T("file"));
    m_EndSecPart.insert(_T("forall"));
    m_EndSecPart.insert(_T("if"));
    m_EndSecPart.insert(_T("select"));
    m_EndSecPart.insert(_T("where"));
    //m_EndSecPart.insert(_T("block"));
    m_EndSecPart.insert(_T("critical"));
    m_EndSecPart.insert(_T("template"));
}

bool ParserThreadF::Parse()
{
    if (!m_pTokens)
        return false;

	if (!m_Tokens.IsOK())
	{
		return false;
    }

	while (1)
	{
		wxString token = m_Tokens.GetToken();
		if (token.IsEmpty())
			break;

        wxString tok_low = token.Lower();
        wxString next = m_Tokens.PeekToken();
        wxString nex_low = next.Lower();

        if (tok_low.Matches(_T("use")))
        {
            HandleUse();
        }
        else if (tok_low.Matches(_T("module")) && !nex_low.Matches(_T("procedure"))
                  && !nex_low.Matches(_T("function"))  && !nex_low.Matches(_T("subroutine")))
        {
            HandleModule();
        }
        else if (tok_low.Matches(_T("submodule")) && !nex_low.Matches(_T("procedure")))
        {
            HandleSubmodule();
        }
        else if (tok_low.Matches(_T("program")))
        {
            HandleFunction(tkProgram);
        }
        else if (tok_low.Matches(_T("function")))
        {
            HandleFunction(tkFunction);
        }
        else if (tok_low.Matches(_T("subroutine")))
        {
            HandleFunction(tkSubroutine);
        }
        else if (tok_low.Matches(_T("type")) && !nex_low(0,1).Matches(_T("(")) && !nex_low.Matches(_T("is")))
        {
            HandleType();
        }
        else if (tok_low.Matches(_T("block")))
        {
            if (nex_low.Matches(_T("data")))
            {
                token = m_Tokens.GetToken();
                tok_low = token.Lower();
                next = m_Tokens.PeekToken();
                nex_low = next.Lower();
                HandleBlockData();
            }
            else
            {
                HandleBlockConstruct();
            }
        }
        else if (tok_low.Matches(_T("blockdata")))
        {
            HandleBlockData();
        }
        else if (tok_low.Matches(_T("include")))
        {
            HandleInclude();
        }
        else if (tok_low.Matches(_T("interface")))
        {
        	HandleInterface();
        }
        else if (tok_low.Matches(_T("associate")))
        {
        	HandleAssociateConstruct();
        }
        else if (tok_low.Matches(_T("end")))
        {
            // something is wrong with code or parser
            m_Tokens.SkipToOneOfChars(";", true);
        }
        else
        {
            bool needDefault = true;
            TokensArrayF tokTmpArr;
            CheckParseOneDeclaration(token, tok_low, next, nex_low, needDefault, tokTmpArr);
        }
	}

    if (!m_Filename.IsEmpty() && m_pIncludeDB)
    {
        //update IncludeDB
        wxFileName fn(m_Filename);
        m_pIncludeDB->SetInclude(fn.GetFullName(), m_IncludeList);
    }
	return true;
}

TokenF* ParserThreadF::DoAddToken(TokenKindF kind, const wxString& name, const wxString& args, const wxString& typeDefinition)
{
	TokenF* newToken = new TokenF;
	newToken->m_Name = name.Lower();

	newToken->m_TokenKind = kind;
	newToken->m_pParent = m_pLastParent;
	newToken->m_Filename = m_Tokens.GetFilename();
	newToken->m_LineStart = m_Tokens.GetLineNumber();
	newToken->m_DisplayName = name;
	newToken->m_Args = args;
	newToken->m_TypeDefinition = typeDefinition;
	newToken->m_DefinitionLength = 1;

    if (m_pLastParent)
        m_pLastParent->AddChild(newToken);
    else
        m_pTokens->Add(newToken);

	return newToken;
}

TokenF* ParserThreadF::DoAddToken(TokenKindF kind, const wxString& name, const wxString& args, const unsigned int defStartLine)
{
	TokenF* newToken = new TokenF;
	newToken->m_Name = name.Lower();

	newToken->m_TokenKind = kind;
	newToken->m_pParent = m_pLastParent;
	newToken->m_Filename = m_Tokens.GetFilename();
	newToken->m_DisplayName = name;
	newToken->m_Args = args;
	newToken->m_TypeDefinition = wxEmptyString;

	newToken->m_LineStart = defStartLine;
	newToken->m_DefinitionLength = m_Tokens.GetLineNumber() - defStartLine + 1;


    if (m_pLastParent)
        m_pLastParent->AddChild(newToken);
    else
        m_pTokens->Add(newToken);

	return newToken;
}

void ParserThreadF::HandleUse()
{
    wxString modName;
    wxArrayString lineTok = m_Tokens.GetTokensToEOL();
    ModuleNature modNature = mnNonIntrinsic;
    int ltCount = lineTok.GetCount();
    int idx = lineTok.Index(_T("::"));
    if (idx != wxNOT_FOUND)
    {
        if (idx > 0)
        {
            if (lineTok.Item(idx-1).Lower().IsSameAs(_T("intrinsic")))
            {
                modNature = mnIntrinsic;
            }
        }
        idx++;
    }
    else
    {
        idx = 0;
    }
    if (ltCount > idx)
    {
        modName = lineTok.Item(idx);
    }
    else
    {
        return; //something wrong
    }
    UseTokenF* pUseTok = DoAddUseToken(modName);
    pUseTok->SetModuleNature(modNature);

    idx++;
    if (ltCount <= idx)
    {
        return; // no more on the line
    }
    if (lineTok.Item(idx).Lower().IsSameAs(_T("only")))
    {
        pUseTok->SetOnly(true);
        idx++;
        while (true)
        {
            idx++;
            if (ltCount <= idx)
                break;
            wxString localName = lineTok.Item(idx);
            wxString externalName;

            if (localName.Lower().IsSameAs(_T("operator")))
            {
                idx += 4; // operator (.st.) => operator (.kt.)
                continue;
            }
            if (ltCount > idx+1 && lineTok.Item(idx+1).IsSameAs(_T("=>")))
            {
                //it is rename
                if (ltCount > idx+2)
                {
                    idx += 2;
                    externalName = lineTok.Item(idx);
                }
                else
                {
                    break; // '=>' on end of line
                }
            }
            if (externalName.IsEmpty())
                pUseTok->AddToNamesList(localName);
            else
                pUseTok->AddToRenameList(localName, externalName);
        }
    }
    else
    {
        pUseTok->SetOnly(false);
        // rename-list
        while (true)
        {
            if (lineTok.Item(idx).Lower().IsSameAs(_T("operator")))
            {
                idx += 5; // operator (.st.) => operator (.kt.)
            }
            if (ltCount > idx+1 && lineTok.Item(idx+1).IsSameAs(_T("=>")))
            {
                wxString localName = lineTok.Item(idx);
                wxString externalName;
                if (ltCount > idx+2)
                {
                    idx += 2;
                    externalName = lineTok.Item(idx);
                }
                else
                {
                    break; // '=>' on end of line
                }
                pUseTok->AddToRenameList(localName, externalName);
                idx++;
                if (ltCount <= idx)
                    break;
            }
            else
            {
                break;
            }
        }
    }
}


UseTokenF* ParserThreadF::DoAddUseToken(const wxString& modName)
{
    UseTokenF* newToken = new UseTokenF();
	newToken->m_Name = modName.Lower();

	newToken->m_TokenKind = tkUse;
	newToken->m_pParent = m_pLastParent;
	newToken->m_Filename = m_Tokens.GetFilename();
	newToken->m_DisplayName = modName;
	newToken->m_TypeDefinition = wxEmptyString;

	newToken->m_LineStart = m_Tokens.GetLineNumber();
	newToken->m_DefinitionLength = 1;

    if (m_pLastParent)
        m_pLastParent->AddChild(newToken);
    else
        m_pTokens->Add(newToken);

	return newToken;
}


void ParserThreadF::HandleModule()
{
    TokenKindF kind = tkModule;
    TokenF* old_parent = m_pLastParent;

    int countAccessList = 0;
    wxString token = m_Tokens.GetTokenSameLine();
    TokenAccessKind taDefKind = taPublic;
    ModuleTokenF* modToken;
    if (token.IsEmpty())
        modToken = DoAddModuleToken(_T("unnamed"));
    else
        modToken = DoAddModuleToken(token);
    m_pLastParent = modToken;
    wxArrayString privateNameList;
    wxArrayString publicNameList;
    wxArrayString protectedNameList;

    TokensArrayF typeTokens;
    TokensArrayF decklTokens;

    while (1)
    {
        token = m_Tokens.GetToken();
        if (token.IsEmpty())
            break;
        wxString tok_low = token.Lower();

        wxString next = m_Tokens.PeekToken();
        wxString nex_low = next.Lower();
        if ( ((m_Tokens.GetLineNumber() == m_Tokens.GetPeekedLineNumber()) && IsEnd(tok_low, nex_low)) ||
             ((m_Tokens.GetLineNumber() != m_Tokens.GetPeekedLineNumber()) && IsEnd(tok_low, _T(""))) )
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if (tok_low.Matches(_T("type")) && !nex_low(0,1).Matches(_T("(")))
        {
            bool needDefault=true;
            TokenF* tokTmp = 0;
            HandleType(needDefault, tokTmp);
            if (needDefault && tokTmp)
            {
                typeTokens.Add(tokTmp);
            }
        }
        else if (tok_low.Matches(_T("subroutine")))
        {
            HandleFunction(tkSubroutine, taDefKind);
        }
        else if (tok_low.Matches(_T("function")))
        {
            HandleFunction(tkFunction, taDefKind);
        }
        else if (tok_low.Matches(_T("use")))
        {
            HandleUse();
        }
        else if (tok_low.Matches(_T("interface")))
        {
        	HandleInterface(taDefKind);
        }
        else if (tok_low.Matches(_T("include")))
        {
            HandleInclude();
        }
        else if (tok_low.Matches(_T("private")))
        {
            bool changeDefault;
            HandleAccessList(taPrivate, changeDefault, countAccessList, privateNameList);
            if (changeDefault)
            {
                modToken->SetDefaultPublic(false);
                taDefKind = taPrivate;
            }
        }
        else if (tok_low.Matches(_T("public")))
        {
            bool changeDefault;
            HandleAccessList(taPublic, changeDefault, countAccessList, publicNameList);
            if (changeDefault)
            {
                modToken->SetDefaultPublic(true);
                taDefKind = taPublic;
            }
        }
        else if (tok_low.Matches(_T("protected")))
        {
            bool tmpB;
            HandleAccessList(taProtected, tmpB, countAccessList, protectedNameList);
        }
        else if (kind == tkModule)
        {
            bool needDefault=true;
            TokensArrayF tokTmpArr;
            CheckParseOneDeclaration(token, tok_low, next, nex_low, needDefault, tokTmpArr);
            if (needDefault)
            {
                for (size_t i=0; i<tokTmpArr.Count(); i++)
                {
                    decklTokens.Add(tokTmpArr.Item(i));
                }
            }
        }
    }
    modToken->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;

    for (size_t i=0; i<typeTokens.GetCount(); i++)
    {
        typeTokens.Item(i)->m_TokenAccess = taDefKind;
    }

    for (size_t i=0; i<decklTokens.GetCount(); i++)
    {
        decklTokens.Item(i)->m_TokenAccess = taDefKind;
    }

    for (size_t i=0; i<publicNameList.GetCount(); i++)
    {
        modToken->AddToPublicList(publicNameList.Item(i));
    }
    for (size_t i=0; i<privateNameList.GetCount(); i++)
    {
        modToken->AddToPrivateList(privateNameList.Item(i));
    }

    TokensArrayF* toks = &modToken->m_Children;
    if (toks)
    {
        for (size_t i=0; i<toks->GetCount(); i++)
        {
            ChangeTokenAccess(modToken, toks->Item(i));

            if (protectedNameList.Index(toks->Item(i)->m_Name) != wxNOT_FOUND)
            {
                toks->Item(i)->m_TokenAccess = taProtected;
            }
            else if (toks->Item(i)->m_TokenKind == tkInterfaceExplicit)
            {
                TokensArrayF* chs = &toks->Item(i)->m_Children;
                if (chs)
                {
                    for (size_t j=0; j<chs->GetCount(); j++)
                    {
                        ChangeTokenAccess(modToken, chs->Item(j));
                    }
                }
            }
        }
    }

}

void ParserThreadF::HandleSubmodule()
{
    TokenF* old_parent = m_pLastParent;
    unsigned int defStartLine = m_Tokens.GetLineNumber();
    wxString ancestorModule;
    wxString parentSubmodule;
    wxString submName;

    wxString token = m_Tokens.GetTokenSameFortranLine();

    if (!token.IsEmpty() && token(0,1).Matches(_T("(")))
    {
        token = token.Mid(1).BeforeFirst(')');
        int i = token.Find(':');
        if (i != wxNOT_FOUND)
        {
            ancestorModule = token.Mid(0,i).Trim().Trim(false);
            if (i+1 < int(token.Length()))
                parentSubmodule = token.Mid(i+1).Trim().Trim(false);
        }
        else
            ancestorModule = token.Trim().Trim(false);

        token = m_Tokens.GetTokenSameFortranLine();
        if (!token.IsEmpty())
            submName = token;
        else
            submName = _T("unnamed");
    }
    else if(token.IsEmpty())
        submName = _T("unnamed");
    else
        submName = token;

    SubmoduleTokenF* pSubmodToken = DoAddSubmoduleToken(submName, ancestorModule, parentSubmodule, defStartLine);
    m_pLastParent = pSubmodToken;

    while (1)
    {
        token = m_Tokens.GetToken();
        if (token.IsEmpty())
            break;
        wxString tok_low = token.Lower();

        wxString next = m_Tokens.PeekToken();
        wxString nex_low = next.Lower();
        if ( ((m_Tokens.GetLineNumber() == m_Tokens.GetPeekedLineNumber()) && IsEnd(tok_low, nex_low)) ||
             ((m_Tokens.GetLineNumber() != m_Tokens.GetPeekedLineNumber()) && IsEnd(tok_low, _T(""))) )
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if (tok_low.Matches(_T("type")) && !nex_low(0,1).Matches(_T("(")))
        {
            HandleType();
        }
        else if (tok_low.Matches(_T("subroutine")))
        {
            HandleFunction(tkSubroutine);
        }
        else if (tok_low.Matches(_T("function")))
        {
            HandleFunction(tkFunction);
        }
        else if (tok_low.Matches(_T("use")))
        {
            HandleUse();
        }
        else if (tok_low.Matches(_T("interface")))
        {
        	HandleInterface();
        }
        else if (tok_low.Matches(_T("include")))
        {
            HandleInclude();
        }
        else if (tok_low.Matches(_T("module")) && nex_low.Matches(_T("procedure")))
        {
            m_Tokens.GetToken();
            HandleSubmoduleProcedure();
        }
        else
        {
            bool needDefault=true;
            TokensArrayF tokTmpArr;
            CheckParseOneDeclaration(token, tok_low, next, nex_low, needDefault, tokTmpArr);
        }
    }
    pSubmodToken->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}

ModuleTokenF* ParserThreadF::DoAddModuleToken(const wxString& modName)
{
    ModuleTokenF* newToken = new ModuleTokenF();
	newToken->m_Name = modName.Lower();

	newToken->m_TokenKind = tkModule;
	newToken->m_pParent = m_pLastParent;
	newToken->m_Filename = m_Tokens.GetFilename();
	newToken->m_DisplayName = modName;
	newToken->m_TypeDefinition = wxEmptyString;

	newToken->m_LineStart = m_Tokens.GetLineNumber();
	newToken->m_DefinitionLength = 1;

    if (m_pLastParent)
        m_pLastParent->AddChild(newToken);
    else
        m_pTokens->Add(newToken);

	return newToken;
}

SubmoduleTokenF* ParserThreadF::DoAddSubmoduleToken(const wxString& submName, const wxString& ancestorModule,
                                                 const wxString& parentSubmodule, unsigned int defStartLine)
{
    SubmoduleTokenF* newToken = new SubmoduleTokenF();
	newToken->m_Name = ancestorModule.Lower();
    newToken->m_Name << _T(":") << submName.Lower();
	newToken->m_TokenKind = tkSubmodule;
	newToken->m_pParent = m_pLastParent;
	newToken->m_Filename = m_Tokens.GetFilename();
	newToken->m_DisplayName = submName;
	newToken->m_DisplayName << _T(" (") << ancestorModule;
	if (!parentSubmodule.IsEmpty())
        newToken->m_DisplayName << _T(":") << parentSubmodule;
    newToken->m_DisplayName << _T(")");
	newToken->m_TypeDefinition = wxEmptyString;

	newToken->m_LineStart = defStartLine;
	newToken->m_DefinitionLength = 1;

	newToken->m_AncestorModuleName = ancestorModule.Lower();
	newToken->m_ParentSubmoduleName = parentSubmodule.Lower();

    if (m_pLastParent)
        m_pLastParent->AddChild(newToken);
    else
        m_pTokens->Add(newToken);
//
//    TokenF* oldParent = m_pLastParent;
//    m_pLastParent = newToken;
//    wxString useName = ancestorModule.Lower();
//    useName << _T(":") << parentSubmodule.Lower();
//    UseTokenF* pUseTok = DoAddUseToken(useName);
//    pUseTok->SetModuleNature(mnNonIntrinsic);
//    m_pLastParent = oldParent;

	return newToken;
}

void ParserThreadF::HandleType()
{
    bool needDefault;
    TokenF* newToken = 0;
    HandleType(needDefault, newToken);
}

void ParserThreadF::HandleType(bool& needDefault, TokenF* &newToken)
{
    needDefault = true;
    TokenAccessKind taKind = taPublic;
    wxString typeName;
    wxString exTypeName;
    wxArrayString lineTok = m_Tokens.GetTokensToEOL();
    wxArrayString lineTokLw;
    MakeArrayStringLower(lineTok, lineTokLw);
    int idx = lineTok.Index(_T("::"));
    if (idx != wxNOT_FOUND)
    {
        if (idx+1 < int(lineTok.GetCount()))
        {
            typeName = lineTok.Item(idx+1);
            int idex = lineTokLw.Index(_T("extends"),false);
            if (idex != wxNOT_FOUND)
            {
                if (idex <= idx-2)
                {
                    wxString ex = lineTok.Item(idex+1);
                    int idx_a = ex.Find('(');
                    int idx_b = ex.Find(')', true);
                    if ( idx_a != wxNOT_FOUND && idx_b != wxNOT_FOUND && idx_a < (idx_b-1) )
                    {
                        exTypeName = ex.Mid(idx_a+1,idx_b-idx_a-1).Trim().Trim(false);
                    }
                }
            }

            idex = lineTokLw.Index(_T("private"));
            if (idex != wxNOT_FOUND && idex < idx)
            {
                taKind = taPrivate;
                needDefault = false;
            }
            else
            {
                idex = lineTokLw.Index(_T("public"));
                if (idex != wxNOT_FOUND && idex < idx)
                {
                    taKind = taPublic;
                    needDefault = false;
                }
            }
        }
        else
        {
            //something wrong
            return;
        }
    }
    else
    {
        if (lineTok.GetCount() > 0)
        {
            typeName = lineTok.Item(0);
        }
        else
        {
            //something wrong
            return;
        }
    }
    TokenF* old_parent = m_pLastParent;
    m_pLastParent = DoAddToken(tkType, typeName);
    m_pLastParent->m_ExtendsType = exTypeName;
    m_pLastParent->m_TokenAccess = taKind;

    ParseDeclarations(true, true);

    if (m_LastTokenName.IsSameAs(_T("contains")))
        ParseTypeBoundProcedures();

    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    newToken = m_pLastParent;
    m_pLastParent = old_parent;
}

void ParserThreadF::CheckParseOneDeclaration(wxString& token, wxString& tok_low, wxString& next, wxString& next_low,
                                             bool& needDefault, TokensArrayF& newTokenArr)
{
    if ( tok_low.IsSameAs(_T("integer")) || tok_low.IsSameAs(_T("real"))
            || tok_low.IsSameAs(_T("doubleprecision")) || tok_low.IsSameAs(_T("character"))
            || tok_low.IsSameAs(_T("complex")) || tok_low.IsSameAs(_T("logical"))
            || ( tok_low.IsSameAs(_T("double")) && next_low.IsSameAs(_T("precision")) )
            || ( tok_low.IsSameAs(_T("type")) && next_low.StartsWith(_T("(")) )
            || ( tok_low.IsSameAs(_T("class")) && next_low.StartsWith(_T("(")) )
            || tok_low.IsSameAs(_T("enumerator"))
        )
        {
            wxArrayString lineTok = m_Tokens.PeekTokensToEOL();
            if (lineTok.Index(_T("function"), false) == wxNOT_FOUND)
            {
                bool found = ParseDeclarationsFirstPart(token, next);
                if (found)
                {
                    ParseDeclarationsSecondPart(token, needDefault, newTokenArr);
                }
            }
        }
}


void ParserThreadF::ParseDeclarations(bool breakAtEnd, bool breakAtContains)
{
    TokenAccessKind taDefKind = taPublic;
    TokensArrayF tokArr;
	while (1)
	{
		wxString token = m_Tokens.GetToken();
		m_LastTokenName = token.Lower();
		wxString next = m_Tokens.PeekToken();
        if (m_LastTokenName.IsEmpty())
		{
			break;
		}
		else if (m_LastTokenName.IsSameAs(_T("include")))
        {
            HandleInclude();
        }
        else if (m_LastTokenName.IsSameAs(_T("interface")))
        {
        	HandleInterface(taDefKind);
        }
        else if (breakAtEnd && IsEnd(m_LastTokenName, next.Lower()))
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if (breakAtContains && m_LastTokenName.IsSameAs(_T("contains")))
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if (m_LastTokenName.IsSameAs(_T("private")))
        {
            bool changeDefault;
            int cal=0;
            wxArrayString pnList;
            HandleAccessList(taPrivate, changeDefault, cal, pnList);
            if (changeDefault)
            {
                taDefKind = taPrivate;
            }
        }
        else if (m_LastTokenName.IsSameAs(_T("public")))
        {
            bool changeDefault;
            int cal=0;
            wxArrayString pnList;
            HandleAccessList(taPublic, changeDefault, cal, pnList);
            if (changeDefault)
            {
                taDefKind = taPublic;
            }
        }
        else if (m_LastTokenName.IsSameAs(_T("block")) && !next.Lower().IsSameAs(_T("data")))
        {
            HandleBlockConstruct();
        }

        bool found = ParseDeclarationsFirstPart(token, next);
        if (found)
        {
            bool nDef=true;
            TokensArrayF tokArrTmp;
            ParseDeclarationsSecondPart(token, nDef, tokArrTmp);
            if (nDef)
            {
                for (size_t i=0; i<tokArrTmp.Count(); i++)
                {
                    tokArr.Add(tokArrTmp.Item(i));
                }
            }
        }
	}
	for (size_t i=0; i<tokArr.Count(); i++)
    {
        tokArr.Item(i)->m_TokenAccess = taDefKind;
    }
	return;
}


bool ParserThreadF::ParseDeclarationsFirstPart(wxString& token, wxString& next)
{
    wxString tok_low = token.Lower();
    wxString next_low = next.Lower();
    bool found = false;

    if ( tok_low.IsSameAs(_T("integer")) || tok_low.IsSameAs(_T("real"))
        || tok_low.IsSameAs(_T("doubleprecision")) || tok_low.IsSameAs(_T("character"))
        || tok_low.IsSameAs(_T("complex")) || tok_low.IsSameAs(_T("logical"))
        || tok_low.IsSameAs(_T("enumerator")) )
    {
        if (next_low.StartsWith(_T("(")))
        {
            token.Append(next);
            m_Tokens.GetToken();
        }
        else if (next_low.StartsWith(_T("*")))
        {
            token.Append(m_Tokens.GetToken());
            token.Append(m_Tokens.GetTokenSameFortranLine());
        }
        found = true;
    }
    else if (tok_low.IsSameAs(_T("double")))
    {
        if (next_low.IsSameAs(_T("precision")))
        {
            found = true;
            token.Append(_T(" "));
            token.Append(next);
            m_Tokens.GetToken();
            next = m_Tokens.PeekToken();
            if (next.StartsWith(_T("(")))
            {
                token.Append(next);
                m_Tokens.GetToken();
            }
        }
    }
    else if (tok_low.IsSameAs(_T("type")) || tok_low.IsSameAs(_T("class")))
    {
        if (next_low.StartsWith(_T("(")))
        {
            if (next_low.EndsWith(_T(")")))
            {
                wxString token_s = m_Tokens.GetToken();
                token_s = token_s.Mid(1,token_s.Len()-2).Trim().Trim(false);
                token.Append(_T("("));
                token.Append(token_s);
                token.Append(_T(")"));
                found = true;
            }
            else
            {
                //something wrong
                m_Tokens.SkipToOneOfChars(";", true);
            }
        }
        else if (tok_low.IsSameAs(_T("type"))  && !next_low.IsSameAs(_T("is")))
        {
            // we found type definition
            HandleType();
        }
    }
    return found;
}


void ParserThreadF::ParseDeclarationsSecondPart(wxString& token, bool& needDefault, TokensArrayF& newTokenArr)
{
    needDefault = true;
    TokenAccessKind taKind = taPublic;
    wxString defT = token;
    wxArrayString linesArr;
    m_Tokens.SetDetailedParsing(true);
    wxArrayString lineTok = m_Tokens.GetTokensToEOL(&linesArr);
    m_Tokens.SetDetailedParsing(false);
    int idx = lineTok.Index(_T("::"));
    if (idx != wxNOT_FOUND)
    {
        for (int i=0; i<idx; i++)
        {
            if (lineTok.Item(i).IsSameAs(_T(",")))
                continue;

            if (!lineTok.Item(i).StartsWith(_T("(")))
            {
                defT.Append(_T(", "));
            }
            defT.Append(lineTok.Item(i));

            wxString tokLw = lineTok.Item(i).Lower();
            if (tokLw.IsSameAs(_T("private")))
            {
                taKind = taPrivate;
                needDefault = false;
            }
            else if (tokLw.IsSameAs(_T("protected")))
            {
                taKind = taProtected;
                needDefault = false;
            }
            else if (tokLw.IsSameAs(_T("public")))
            {
                taKind = taPublic;
                needDefault = false;
            }
        }
    }
    else
    {
        idx = -1;
    }

    wxArrayString varNames;
    wxArrayString varArgs;
    wxArrayString varComs;
    for (size_t i=idx+1; i<lineTok.GetCount(); )
    {
        wxString var1= lineTok.Item(i);
        if (var1.IsSameAs(_T(",")))
        {
            i++;
            continue;
        }
        wxString arg1;
        if (i+1 < lineTok.GetCount())
        {
            wxString s = lineTok.Item(i+1);
            if (s.StartsWith(_T("(")) && s.EndsWith(_T(")")))
            {
                arg1 = s;
                i++;
            }
        }
        if (i+1 < lineTok.GetCount() && (lineTok.Item(i+1).IsSameAs(_T("=>")) || lineTok.Item(i+1).IsSameAs(_T("=")) || lineTok.Item(i+1).IsSameAs(_T("*"))) )
        {
            i += 1;
            for (;i<lineTok.GetCount();i++)
            {
                if (lineTok.Item(i).IsSameAs(_T(",")))
                    break;
                else
                    arg1 << lineTok.Item(i);
            }
//            if (i+1 < lineTok.GetCount())
//            {
//                wxString s = lineTok.Item(i+1);
//                if (s.StartsWith(_T("(")) && s.EndsWith(_T(")")))
//                {
//                    i++;
//                }
//            }
            if(i >= lineTok.GetCount())
            {
                i = lineTok.GetCount() - 1;
            }
        }
        wxString comStr;
        int comInd = linesArr.Item(i).Find('!');
        if (comInd != wxNOT_FOUND)
        {
            comStr = linesArr.Item(i).Mid(comInd).Trim();
        }
        varNames.Add(var1);
        varArgs.Add(arg1);
        varComs.Add(comStr);
        i++;
    }
    for (size_t i=0; i<varNames.GetCount(); i++)
    {
        TokenF* tok = DoAddToken(tkVariable, varNames[i], varArgs[i], defT);
        tok->m_PartLast = varComs.Item(i);
        tok->m_TokenAccess = taKind;
        newTokenArr.Add(tok);
    }
	return;
}


void ParserThreadF::HandleSubmoduleProcedure()
{
    wxString token;
    token = m_Tokens.GetTokenSameFortranLine();

    TokenF* old_parent = m_pLastParent;
    m_pLastParent = DoAddToken(tkProcedure, token);

    GoThroughBody();

    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}


void ParserThreadF::HandleFunction(TokenKindF kind, TokenAccessKind taKind)
{
    wxString token;
    token = m_Tokens.GetTokenSameFortranLine();

    if (token.IsEmpty() && kind == tkProgram)
        token = _T("unnamed");

    unsigned int defStartLine = m_Tokens.GetLineNumber();
    TokenF* old_parent = m_pLastParent;
    wxString args = m_Tokens.PeekTokenSameFortranLine();

    if (args.IsEmpty() || !args(0,1).Matches(_T("(")))
        args = _T("()");
    else
        args = m_Tokens.GetTokenSameFortranLine();
    m_pLastParent = DoAddToken(kind, token, args, defStartLine);
    m_pLastParent->m_TokenAccess = taKind;

    if (kind == tkFunction)
    {
        wxString funkLine = m_Tokens.GetLineFortran();
        wxString funkLineLow = funkLine.Lower();
        int i_fun = funkLineLow.Find(_T("function"));
        if (i_fun != wxNOT_FOUND)
        {
            m_pLastParent->AddPartFirst(funkLine.Mid(0,i_fun).Trim().Trim(false));
            wxString secPart = funkLine.Mid(i_fun+8);
            i_fun = secPart.Find(')');
            if (i_fun != wxNOT_FOUND && (int)secPart.Len() > i_fun+1)
            {
                wxString lastPart = secPart.Mid(i_fun+1).Trim().Trim(false);
                m_pLastParent->AddPartLast(lastPart);
                wxString lastPartLow = lastPart.Lower();
                i_fun = lastPartLow.Find(_T("result"));
                if (i_fun != wxNOT_FOUND)
                {
                    wxString el = lastPartLow.Mid(i_fun+6);
                    int is = el.Find('(');
                    int ie = el.Find(')');
                    if (is != wxNOT_FOUND && ie != wxNOT_FOUND && ie > is)
                        m_pLastParent->AddResultVariable(el.Mid(is+1,ie-is-1).Trim().Trim(false));
                }
            }
        }
    }
    GoThroughBody();
    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());

    m_pLastParent = old_parent;
}


void ParserThreadF::HandleBlockConstruct()
{
    unsigned int defStartLine = m_Tokens.GetLineNumber();
    TokenF* old_parent = m_pLastParent;
    m_pLastParent = DoAddToken(tkBlockConstruct, wxEmptyString, wxEmptyString, defStartLine);

    GoThroughBody();
    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}


void ParserThreadF::HandleAssociateConstruct()
{
    TokenF* old_parent = m_pLastParent;
    wxString args = m_Tokens.PeekTokenSameFortranLine();

    if (args.IsEmpty() || !args(0,1).Matches(_T("(")))
        args = _T("()");
    else
        args = m_Tokens.GetTokenSameFortranLine();
    m_pLastParent = DoAddToken(tkAssociateConstruct, wxEmptyString, args, wxEmptyString);

    GoThroughBody();
    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}


void ParserThreadF::HandleInterface(TokenAccessKind taKind)
{
    TokenF* old_parent = m_pLastParent;
    unsigned int defStartLine = m_Tokens.GetLineNumber();
    wxArrayString curLineArr = m_Tokens.GetTokensToEOL();
    wxString name;
    TokenKindF tokKin;
    if (curLineArr.GetCount() > 0)
    {
        wxString low = curLineArr.Item(0).Lower();
        if (low.IsSameAs(_T("operator")))
        {
            name.Append(_T("%%"));
            name.Append(curLineArr.Item(0));
            for (unsigned int i=1; i<curLineArr.GetCount(); i++)
            {
                name.Append(_T(" "));
                name.Append(curLineArr.Item(i));
            }
            m_InterfaceOperator += 1;
            if (m_InterfaceOperator > 1)
            {
                name << _T(" #") << m_InterfaceOperator;
            }
        }
        else if (low.IsSameAs(_T("assignment")))
        {
            name.Append(_T("%%"));
            name.Append(curLineArr.Item(0));
            for (unsigned int i=1; i<curLineArr.GetCount(); i++)
            {
                name.Append(_T(" "));
                name.Append(curLineArr.Item(i));
            }
            m_InterfaceAssignment += 1;
            if (m_InterfaceAssignment > 1)
            {
                name << _T(" #") << m_InterfaceAssignment;
            }
        }
        else if (low.IsSameAs(_T("read")))
        {
            name.Append(_T("%%"));
            name.Append(curLineArr.Item(0));
            for (unsigned int i=1; i<curLineArr.GetCount(); i++)
            {
                name.Append(_T(" "));
                name.Append(curLineArr.Item(i));
            }
            m_InterfaceRead += 1;
            if (m_InterfaceRead > 1)
            {
                name << _T(" #") << m_InterfaceRead;
            }
        }
        else if (low.IsSameAs(_T("write")))
        {
            name.Append(_T("%%"));
            name.Append(curLineArr.Item(0));
            for (unsigned int i=1; i<curLineArr.GetCount(); i++)
            {
                name.Append(_T(" "));
                name.Append(curLineArr.Item(i));
            }
            m_InterfaceWrite += 1;
            if (m_InterfaceWrite > 1)
            {
                name << _T(" #") << m_InterfaceWrite;
            }
        }
        else
        {
            // generic procedure name
            name.Append(curLineArr.Item(0));
            for (unsigned int i=1; i<curLineArr.GetCount(); i++)
            {
                name.Append(_T(" "));
                name.Append(curLineArr.Item(i));
            }
        }
        tokKin = tkInterface;
    }
    else
    {
        tokKin = tkInterfaceExplicit;
    }

    m_pLastParent = DoAddToken(tokKin, name, wxEmptyString, defStartLine);
    m_pLastParent->m_TokenAccess = taKind;

    GoThroughBody();

    if (tokKin == tkInterfaceExplicit)
    {
        TokensArrayF* toks = &m_pLastParent->m_Children;
        if (toks)
        {
            for (size_t i=0; i<toks->GetCount(); i++)
            {
                toks->Item(i)->m_TokenAccess = taKind;
            }
        }
    }

    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}

void ParserThreadF::HandleBlockData()
{
    TokenF* old_parent = m_pLastParent;
    wxString token = m_Tokens.GetTokenSameLine();
    if (token.IsEmpty())
        m_pLastParent = DoAddToken(tkBlockData, _T("BD_unnamed"));
    else
        m_pLastParent = DoAddToken(tkBlockData, token);

    while (1)
    {
    	token = m_Tokens.GetToken();
    	if (token.IsEmpty())
            break;
        wxString tok_low = token.Lower();

        wxString next = m_Tokens.PeekToken();
        wxString nex_low = next.Lower();
        if (IsEnd(tok_low, nex_low))
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if (tok_low.Matches(_T("include")))
        {
            HandleInclude();
        }
    }
    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}

void ParserThreadF::HandleInclude()
{
    wxString token;
    token = m_Tokens.GetTokenSameFortranLine();

    if (token.IsEmpty())
        return; // something wrong
    else if ((token.StartsWith(_T("\'")) || token.StartsWith(_T("\""))) && (token.EndsWith(_T("\'")) || token.EndsWith(_T("\""))))
    {
        token = token.Mid(1,token.Len()-2).Trim().Trim(false);
        DoAddToken(tkInclude, token);
        m_IncludeList.Add(token);
    }
}

void ParserThreadF::HandleAccessList(TokenAccessKind taKind, bool& changeDefault, int& countAccess, wxArrayString& nameList)
{
    changeDefault = false;
    wxString curLine = m_Tokens.GetLineFortran().Lower().Trim(false);
    int ipp;
    if (taKind == taPrivate)
        ipp = curLine.Find(_T("private"));
    else if (taKind == taPublic)
        ipp = curLine.Find(_T("public"));
    else if (taKind == taProtected)
        ipp = curLine.Find(_T("protected"));

    if (ipp == wxNOT_FOUND)
        return; // something is wrong
    else if (ipp != 0)
        return; // here private (public) is used as an atribute.

    unsigned int defStartLine = m_Tokens.GetLineNumber();
    wxArrayString curLineArr = m_Tokens.GetTokensToEOL();
    if (curLineArr.GetCount() == 0)
    {
        changeDefault = true;
        return;
    }
    countAccess++;
    wxString name;
    name = _T("AccessList");
    if (countAccess > 1)
        name << _T(" ") << countAccess;

    TokenF* token = DoAddToken(tkAccessList, name, wxEmptyString, defStartLine);
    token->AddLineEnd(m_Tokens.GetLineNumber());
    token->m_TokenAccess = taKind;

    size_t i=0;
    if (curLineArr.Item(0).IsSameAs(_T("::")))
        i=1;
    for (; i<curLineArr.GetCount(); i++)
    {
        nameList.Add(curLineArr.Item(i).Lower());
    }
}

void ParserThreadF::GoThroughBody()
{
    wxString tok_low;

    while (1)
    {
        unsigned int ln_tokold = m_Tokens.GetLineNumber();

    	wxString token = m_Tokens.GetToken();
    	if (token.IsEmpty())
            break;

        tok_low = token.Lower();

        if (tok_low.Matches(_T("::")))
        {
            m_Tokens.SkipToOneOfChars(";", true);
            continue;
        }

        wxString next = m_Tokens.PeekToken();
        wxString nex_low = next.Lower();
        if (IsEnd(tok_low, nex_low))
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if (tok_low.Matches(_T("type")) && !nex_low(0,1).Matches(_T("(")) && !nex_low.Matches(_T("is"))
                 && ln_tokold != m_Tokens.GetLineNumber())
        {
            HandleType();
        }
        else if (tok_low.Matches(_T("subroutine")))
        {
            HandleFunction(tkSubroutine);
        }
        else if (tok_low.Matches(_T("function")))
        {
            HandleFunction(tkFunction);
        }
        else if (tok_low.Matches(_T("use")))
        {
            HandleUse();
        }
        else if (tok_low.Matches(_T("interface")))
        {
        	HandleInterface();
        }
        else if (tok_low.Matches(_T("include")))
        {
            HandleInclude();
        }
        else if (tok_low.Matches(_T("procedure")) && m_pLastParent->m_TokenKind == tkInterface)
        {
            HandleProcedureList();
        }
        else if (tok_low.Matches(_T("block")))
        {
            if (nex_low.Matches(_T("data")))
            {
                token = m_Tokens.GetToken();
                tok_low = token.Lower();
                next = m_Tokens.PeekToken();
                nex_low = next.Lower();
                HandleBlockData();
            }
            else
            {
                HandleBlockConstruct();
            }
        }
        else if (tok_low.Matches(_T("blockdata")))
        {
            HandleBlockData();
        }
        else if (tok_low.Matches(_T("associate")))
        {
        	HandleAssociateConstruct();
        }
        else
        {
            bool needDefault = true;
            TokensArrayF tokTmpArr;
            CheckParseOneDeclaration(token, tok_low, next, nex_low, needDefault, tokTmpArr);
        }
    }
}

void ParserThreadF::HandleProcedureList()
{
    unsigned int lineNum = m_Tokens.GetLineNumber();
    wxArrayString curLineArr = m_Tokens.GetTokensToEOL();

    for (unsigned int i=0; i<curLineArr.GetCount(); i++)
    {
        DoAddToken(tkOther, curLineArr.Item(i), wxEmptyString, lineNum);
    }
}

void ParserThreadF::ParseTypeBoundProcedures()
{
    TokenAccessKind defAccKind = taPublic;
    while (1)
    {
        TokenAccessKind tokAccK = defAccKind;
        wxString firstTokenLw = m_Tokens.GetToken().Lower();
        if (firstTokenLw.IsEmpty())
            break;
        unsigned int lineNum = m_Tokens.GetLineNumber();
        wxArrayString curLineArr = m_Tokens.GetTokensToEOL();
        bool isGen = firstTokenLw.IsSameAs(_T("generic"));
        if (curLineArr.Count() > 0 && (firstTokenLw.IsSameAs(_T("procedure")) || isGen) ) // &&
            // !curLineArr.Item(0).StartsWith(_T("(")) ) // not interface-name
        {
            bool pass = true;
            wxString passArg;
            int idx = curLineArr.Index(_T("::"));
            int startList;
            if (idx != wxNOT_FOUND)
            {
                for (int i=0; i<idx; i++)
                {
                    wxString tok = curLineArr.Item(i).Lower();
                    if (tok.IsSameAs(_T("nopass")))
                    {
                        pass = false;
                        break;
                    }
                    else if (tok.IsSameAs(_T("pass")))
                    {
                        if (i < idx-1)
                        {
                            wxString strArg = curLineArr.Item(i+1);
                            int idx_a = strArg.Find(')',true);
                            int idx_b = strArg.Find('(');
                            if (idx_a != wxNOT_FOUND && idx_b != wxNOT_FOUND && idx_a > idx_b+1)
                            {
                                passArg = strArg.Mid(idx_b+1,idx_a-idx_b-1);
                            }
                        }
                        break;
                    }
                    else if (tok.IsSameAs(_T("private")))
                    {
                        tokAccK = taPrivate;
                    }
                    else if (tok.IsSameAs(_T("public")))
                    {
                        tokAccK = taPublic;
                    }
                }
                startList = idx + 1;
            }
            else
            {
                startList = 0;
            }
            int countArr = curLineArr.GetCount();
            int ic=startList;
            if (!isGen)
            {
                while (ic < countArr)
                {
                    wxString bindName = curLineArr.Item(ic);
                    wxString procName;
                    if (ic+2 < countArr)
                    {
                        if (curLineArr.Item(ic+1).IsSameAs(_T("=>")))
                        {
                            procName = curLineArr.Item(ic+2);
                            ic += 2;
                        }
                    }
                    ic++;
                    TokenF* token = DoAddToken(tkProcedure, bindName.Lower(), wxEmptyString, lineNum);
                    token->m_DisplayName = bindName;
                    token->m_Pass = pass;
                    token->m_Args = passArg;
                    token->m_PartLast = procName.Lower();
                    token->AddLineEnd(m_Tokens.GetLineNumber());
                    token->m_TokenAccess = tokAccK;
                }
            }
            else //isGen
            {
                while (ic < countArr-2)
                {
                    wxString curNamLw = curLineArr.Item(ic).Lower();
                    if (curNamLw.IsSameAs(_T("operator")) || curNamLw.IsSameAs(_T("assignment")))
                        break;
                    if (curLineArr.Item(ic+1).IsSameAs(_T("=>")))
                    {
                        wxString bindName = curLineArr.Item(ic);
                        TokenF* token = DoAddToken(tkInterface, bindName.Lower(), wxEmptyString, lineNum);
                        token->m_DisplayName = bindName;
                        ic += 2;
                        wxString specNames;
                        for (;ic < countArr; ic++)
                        {
                            specNames << curLineArr.Item(ic) << _T(" ");
                        }
                        token->m_PartLast = specNames.Trim();
                        token->AddLineEnd(m_Tokens.GetLineNumber());
                        token->m_TokenAccess = tokAccK;
                    }
                    ic++;
                }
            }
        }
        else if ( ( firstTokenLw.IsSameAs(_T("end"))) ||
                  ( curLineArr.Count() >= 1 && IsEnd(firstTokenLw,curLineArr.Item(0).Lower()) ) ||
                  ( IsEnd(firstTokenLw, wxEmptyString) ) )
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if ( firstTokenLw.IsSameAs(_T("private")) && curLineArr.Count() == 0 )
        {
            defAccKind = taPrivate;
        }
    }
}

bool ParserThreadF::IsEnd(wxString tok_low, wxString nex_low)
{
    bool isend = false;
    if ( (tok_low.Matches(_T("end")) && !m_EndSecPart.count(nex_low)) ||
          tok_low.Matches(_T("endsubroutine")) ||
          tok_low.Matches(_T("endfunction")) ||
          tok_low.Matches(_T("endmodule")) ||
          tok_low.Matches(_T("endsubmodule")) ||
          tok_low.Matches(_T("endtype")) ||
          tok_low.Matches(_T("endinterface")) ||
          tok_low.Matches(_T("endprogram")) ||
          tok_low.Matches(_T("endblock")) ||
          tok_low.Matches(_T("endblockdata")) ||
          tok_low.Matches(_T("endassociate"))
           )
    {
        isend = true;
    }
    return isend;
}

void ParserThreadF::MakeArrayStringLower(wxArrayString &arr, wxArrayString &arrLw)
{
    for(size_t i=0; i<arr.Count(); i++)
    {
        arrLw.Add(arr.Item(i).Lower());
    }
}

void ParserThreadF::ChangeTokenAccess(ModuleTokenF* modToken, TokenF* token)
{
    if (modToken->HasNameInPrivateList(token->m_Name))
    {
        token->m_TokenAccess = taPrivate;
    }
    else if (modToken->HasNameInPublicList(token->m_Name))
    {
        if (token->m_TokenAccess != taProtected)
            token->m_TokenAccess = taPublic;
    }
}


void ParserThreadF::SplitAssociateConstruct(const wxString& argLine, std::map<wxString,wxString>& assocMap)
{
    wxString args = argLine;
    int pos = args.Find(')',true);
    if (pos != wxNOT_FOUND)
        args.Remove(pos);
    args = args.AfterFirst('(');

    int cnt = 0;
    int sta = 0;
    for(size_t i=0; i<args.Len(); i++)
    {
        if (args.GetChar(i) == '(')
            cnt++;
        else if (args.GetChar(i) == ')')
            cnt--;

        if ( (args.GetChar(i) == ',' && cnt == 0) ||
             (i == args.Len()-1) )
        {
            wxString block;
            if (args.GetChar(i) == ',')
                block = args.Mid(sta, i-sta);
            else
                block = args.Mid(sta, i-sta+1);
            sta = i + 1;
            int sind = block.Find(_T("=>"));
            if (sind != wxNOT_FOUND)
            {
                wxString assocName = block.Mid(0,sind).Trim(true).Trim(false);
                wxString sourceExpr = block.Mid(sind+2).Trim(true).Trim(false);
                assocMap.insert(std::pair<wxString,wxString>(assocName, sourceExpr));
            }
        }
    }
}
