/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include "parserthreadf.h"
#include <set>


ParserThreadF::ParserThreadF(const wxString& bufferOrFilename,
							 TokensArrayF* tokens,
							 FortranSourceForm fsForm, bool isBuffer)
	:
	m_pTokens(tokens),
	m_pLastParent(0L)
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
            m_pLastParent = DoAddToken(tkFile, bufferOrFilename);
        }
    }
    else
    {
        m_Tokens.InitFromBuffer(bufferOrFilename, fsForm);
    }
	InitSecondEndPart();
}

ParserThreadF::~ParserThreadF()
{
    //dtor
}

void ParserThreadF::InitSecondEndPart()
{
    m_EndSecPart.insert(_T("associate"));
	m_EndSecPart.insert(_T("do"));
	m_EndSecPart.insert(_T("enum"));
	m_EndSecPart.insert(_T("file"));
	m_EndSecPart.insert(_T("forall"));
	m_EndSecPart.insert(_T("if"));
	m_EndSecPart.insert(_T("select"));
	m_EndSecPart.insert(_T("where"));
	m_EndSecPart.insert(_T("block"));
	m_EndSecPart.insert(_T("critical"));
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
        else if (tok_low.Matches(_T("module")))
        {
            HandleModProg(tkModule);
        }
        else if (tok_low.Matches(_T("program")))
        {
            HandleModProg(tkProgram);
        }
        else if (tok_low.Matches(_T("function")))
        {
            HandleFunction(tkFunction);
        }
        else if (tok_low.Matches(_T("subroutine")))
        {
            HandleFunction(tkSubroutine);
        }
        else if (tok_low.Matches(_T("type")) && !nex_low(0,1).Matches(_T("(")))
        {
            HandleType();
        }
        else if (tok_low.Matches(_T("block")) && nex_low.Matches(_T("data")))
        {
            token = m_Tokens.GetToken();
            tok_low = token.Lower();
            next = m_Tokens.PeekToken();
            nex_low = next.Lower();
            HandleBlockData();
        }
        else if (tok_low.Matches(_T("blockdata")))
        {
            HandleBlockData();
        }
        else if (tok_low.Matches(_T("include")))
        {
            HandleInclude();
        }
        else if (tok_low.Matches(_T("end")))
        {
            // something is wrong with code or parser
            m_Tokens.SkipToOneOfChars(";", true);
        }
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
    int idx = lineTok.Index(_T("::"));
    if (idx != wxNOT_FOUND)
    {
        if (idx+1 < int(lineTok.GetCount()))
        {
            modName = lineTok.Item(idx+1);
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
            modName = lineTok.Item(0);
        }
        else
        {
            //something wrong
            return;
        }
    }
    DoAddToken(tkUse, modName);
}

void ParserThreadF::HandleModProg(TokenKindF kind)
{
    TokenF* old_parent = m_pLastParent;

    wxString token = m_Tokens.GetTokenSameLine();
    if (token.IsEmpty())
        m_pLastParent = DoAddToken(kind, _T("unnamed"));
    else
        m_pLastParent = DoAddToken(kind, token);

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
        else if (kind == tkModule)
        {
            CheckParseOneDeclaration(token, tok_low, next, nex_low);
        }
    }
    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}

void ParserThreadF::HandleType()
{
    wxString typeName;
    wxString exTypeName;
    wxArrayString lineTok = m_Tokens.GetTokensToEOL();
    int idx = lineTok.Index(_T("::"));
    if (idx != wxNOT_FOUND)
    {
        if (idx+1 < int(lineTok.GetCount()))
        {
            typeName = lineTok.Item(idx+1);
            int idex = lineTok.Index(_("extends"),false);
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

    ParseDeclarations(true, true);
    if (m_LastTokenName.IsSameAs(_T("contains")))
        ParseTypeBoundProcedures();

    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}

void ParserThreadF::CheckParseOneDeclaration(wxString& token, wxString& tok_low, wxString& next, wxString& next_low)
{
    if ( tok_low.IsSameAs(_T("integer")) || tok_low.IsSameAs(_T("real"))
            || tok_low.IsSameAs(_T("doubleprecision")) || tok_low.IsSameAs(_T("character"))
            || tok_low.IsSameAs(_T("complex")) || tok_low.IsSameAs(_T("logical"))
            || ( tok_low.IsSameAs(_T("double")) && next_low.IsSameAs(_T("precision")) )
            || ( tok_low.IsSameAs(_T("type")) && next_low.StartsWith(_T("(")) )
        )
        {
            wxArrayString lineTok = m_Tokens.PeekTokensToEOL();
            if (lineTok.Index(_T("function"), false) == wxNOT_FOUND)
            {
                bool found = ParseDeclarationsFirstPart(token, next);
                if (found)
                {
                    ParseDeclarationsSecondPart(token);
                }
            }
            else
            {
                // this line is function declaration line
            }
        }
}


void ParserThreadF::ParseDeclarations(bool breakAtEnd, bool breakAtContains)
{
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
        else if (IsEnd(m_LastTokenName, next.Lower()) && breakAtEnd)
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if (breakAtContains && m_LastTokenName.IsSameAs(_T("contains")))
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }

        bool found = ParseDeclarationsFirstPart(token, next);
        if (found)
        {
            ParseDeclarationsSecondPart(token);
        }
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
        || tok_low.IsSameAs(_T("complex")) || tok_low.IsSameAs(_T("logical")) )
    {
        if (next_low.StartsWith(_T("(")))
        {
            token.Append(next);
            m_Tokens.GetToken();
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
        else if (tok_low.IsSameAs(_T("type")))
        {
            // we found type definition
            HandleType();
        }
    }
    return found;
}


void ParserThreadF::ParseDeclarationsSecondPart(wxString& token)
{
    wxString defT = token;
    wxArrayString linesArr;
    wxArrayString lineTok = m_Tokens.GetTokensToEOL(&linesArr);
    int idx = lineTok.Index(_T("::"));
    if (idx != wxNOT_FOUND)
    {
        for (int i=0; i<idx; i++)
        {
            if (!lineTok.Item(i).StartsWith(_T("(")))
            {
                defT.Append(_T(", "));
            }
            defT.Append(lineTok.Item(i));
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
        wxString comStr;
        int comInd = linesArr.Item(i).Find('!');
        if (comInd != wxNOT_FOUND)
        {
            comStr = linesArr.Item(i).Mid(comInd);
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
    }
	return;
}


void ParserThreadF::HandleFunction(TokenKindF kind)
{
    wxString token;
    token = m_Tokens.GetTokenSameFortranLine();
    unsigned int defStartLine = m_Tokens.GetLineNumber();
    TokenF* old_parent = m_pLastParent;
    wxString args = m_Tokens.PeekTokenSameFortranLine();

    if (args.IsEmpty() || !args(0,1).Matches(_T("(")))
        args = _T("()");
    else
        args = m_Tokens.GetTokenSameFortranLine();
    m_pLastParent = DoAddToken(kind, token, args, defStartLine);

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

void ParserThreadF::HandleInterface()
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

    GoThroughBody();

    m_pLastParent->AddLineEnd(m_Tokens.GetLineNumber());
    m_pLastParent = old_parent;
}

void ParserThreadF::HandleBlockData()
{
    wxString token = m_Tokens.GetTokenSameLine();
    if (token.IsEmpty())
        DoAddToken(tkBlockData, _T("BD_unnamed"));
    else
        DoAddToken(tkBlockData, token);

    while (1)
    {
    	token = m_Tokens.GetToken();
    	if (token.IsEmpty())
            break;
        wxString tok_low = token.Lower();

        if (tok_low.Matches(_T("end")))
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
        }
        else if (tok_low.Matches(_T("include")))
        {
            HandleInclude();
        }
    }
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
    }
}

void ParserThreadF::GoThroughBody()
{
    while (1)
    {
    	wxString token = m_Tokens.GetToken();
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
        else if (tok_low.Matches(_T("procedure")) && m_pLastParent->m_TokenKind == tkInterface)
        {
            HandleProcedureList();
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
    while (1)
    {
        wxString firstTokenLw = m_Tokens.GetToken().Lower();
        if (firstTokenLw.IsEmpty())
            break;
        unsigned int lineNum = m_Tokens.GetLineNumber();
        wxArrayString curLineArr = m_Tokens.GetTokensToEOL();
        if (curLineArr.Count() > 0 && firstTokenLw.IsSameAs(_T("procedure")) &&
            !curLineArr.Item(0).StartsWith(_T("(")) ) // not interface-name
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
                }
                startList = idx + 1;
            }
            else
            {
                startList = 0;
            }
            int countArr = curLineArr.GetCount();
            int ic=startList;
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
                TokenF* token = DoAddToken(tkProcedure, bindName, wxEmptyString, lineNum);
                token->m_Pass = pass;
                token->m_Args = passArg;
                token->m_PartLast = procName.Lower();
            }
        }
        else if ( ( firstTokenLw.IsSameAs(_T("end"))) ||
                  ( curLineArr.Count() >= 1 && IsEnd(firstTokenLw,curLineArr.Item(0).Lower()) ) ||
                  ( IsEnd(firstTokenLw, wxEmptyString) ) )
        {
            m_Tokens.SkipToOneOfChars(";", true);
            break;
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
          tok_low.Matches(_T("endtype")) ||
          tok_low.Matches(_T("endinterface")) ||
          tok_low.Matches(_T("endprogram"))
           )
    {
        isend = true;
    }
    return isend;
}

