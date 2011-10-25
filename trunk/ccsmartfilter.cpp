/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Author: Darius Markauskas
 *
 */
#include "ccsmartfilter.h"
#include "tokenf.h"
#include <wx/string.h>

void CCSmartFilter::GetTokenKind(wxArrayString& words, int& kindFilter, bool& allowVariables, kindOfCCList& kindCC)
{
    kindCC = kccOther;
    allowVariables = false;
    wxString wordLw;
    wxString wordLastLw;
    int woCount = words.GetCount();
    if (woCount > 0)
    {
        wordLw = words.Item(0).Lower();
        wordLastLw = words.Item(woCount-1).Lower();
    }
    if (woCount > 1)
    {
        if ( (wordLw.IsSameAs('(') && words.Item(1).Lower().IsSameAs(_T("type"))) ||
             (wordLw.IsSameAs('(') && words.Item(1).Lower().IsSameAs(_T("extends"))) ||
             (wordLw.IsSameAs('(') && words.Item(1).Lower().IsSameAs(_T("class"))))
            wordLw.Prepend( words.Item(1).Lower() );
    }
    if (wordLw.IsEmpty())
    {
        kindFilter = tkOther;
        allowVariables = true;
    }
    else if (wordLw.IsSameAs(_T("call")))
    {
        kindFilter = tkSubroutine | tkInterface;
    }
    else if (woCount > 1 &&
             ( words.Item(woCount-1).Lower().IsSameAs(_T("procedure")) ||
              ( words.Item(woCount-1).Lower().IsSameAs(_T("module")) && words.Item(woCount-2).Lower().IsSameAs(_T("procedure")) ) ))
    {
        kindFilter = tkSubroutine | tkFunction | tkInterface;
    }
    else if (wordLw.IsSameAs(_T("use")) || wordLw.IsSameAs(_T("module")))
    {
        kindFilter = tkModule;
    }
    else if (woCount > 1 && wordLastLw.IsSameAs(_T("use")))
    {
        if (woCount > 2 && wordLw.IsSameAs(_T(":")) && words.Item(1).IsSameAs(_T(":")))
        {
            kindFilter = tkModule;
        }
        else
        {
            kindFilter = tkVariable | tkSubroutine | tkFunction | tkInterface | tkOther;
            allowVariables = true;
            kindCC = kccUseAssocTokens;
        }
    }
    else if (wordLastLw.IsSameAs(_T("private")) ||
             wordLastLw.IsSameAs(_T("public")) ||
             wordLastLw.IsSameAs(_T("protected")) )
    {
        kindFilter = tkVariable | tkSubroutine | tkFunction | tkInterface | tkType | tkOther;
        allowVariables = true;
        kindCC = kccAccessList;
    }
    else if (wordLw.IsSameAs('=') || wordLw.IsSameAs('+') || wordLw.IsSameAs('-') || wordLw.IsSameAs('*') ||
             wordLw.IsSameAs('/') || wordLw.IsSameAs('>') || wordLw.IsSameAs('<') || wordLw.IsSameAs('.'))
    {
        kindFilter = tkFunction | tkInterface;
        allowVariables = true;
    }
    else if (wordLw.IsSameAs('(') || wordLw.IsSameAs(','))
    {
        kindFilter = tkFunction | tkInterface | tkOther;
        allowVariables = true;
    }
    else if ( wordLw.IsSameAs(_T("subroutine")) || wordLw.IsSameAs(_T("function")) || wordLw.IsSameAs(_T("interface"))
             || wordLw.IsSameAs(_T("procedure")) )
    {
        kindFilter = tkSubroutine | tkFunction | tkInterface;
    }
    else if (wordLw.IsSameAs(_T("type")) || wordLw.IsSameAs(_T("type(")) || wordLw.IsSameAs(_T("extends("))
             || wordLw.IsSameAs(_T("class(")))
    {
        kindFilter = tkType;
    }
    else if (wordLw.IsSameAs(';'))
    {
        kindFilter = tkOther;
        allowVariables = true;
    }
    else if (wordLw.IsSameAs(':'))
    {
        kindFilter = tkSubroutine | tkFunction | tkInterface;
        allowVariables = true;
    }
    else if (wordLw.IsSameAs(_T("do")))
    {
        kindFilter = tkOther;
        allowVariables = true;
    }
    else if (woCount > 1 && wordLw.IsSameAs(')') && (
                (words.Item(woCount-1).Lower().IsSameAs(_T("if")))
             || (words.Item(woCount-1).Lower().IsSameAs(_T("read")))
             || (words.Item(woCount-1).Lower().IsSameAs(_T("write"))) ))
    {
        kindFilter = tkOther | tkFunction | tkInterface;
        allowVariables = true;
    }
    else
    {
        kindFilter = tkOther;
    }
}

