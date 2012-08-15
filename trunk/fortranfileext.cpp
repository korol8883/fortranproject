#include "fortranfileext.h"
#include <configmanager.h>
#include <wx/tokenzr.h>

FortranFileExt::FortranFileExt()
{
    m_ExtDone = false;
}

FortranFileExt::~FortranFileExt()
{
    //dtor
}

bool FortranFileExt::IsFileFortran(const wxString& filename, FortranSourceForm& fsForm)
{
    if (!m_ExtDone)
    {
        RereadFileExtensions();
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

void FortranFileExt::RereadFileExtensions()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));
    if (!cfg)
        return;

    m_FortranExtFixed.clear();
    wxString extl = cfg->Read(_T("/extension_fixed"), _T("for, f77, f, fpp"));
    wxStringTokenizer tkz(extl, _T(" ;,*.\t\r\n"), wxTOKEN_STRTOK);
    while ( tkz.HasMoreTokens() )
    {
        wxString token = tkz.GetNextToken();
        m_FortranExtFixed.insert(token);
    }

    m_FortranExtFree.clear();
    extl = cfg->Read(_T("/extension_free"), _T("f90, f95, f03, f2k"));
    tkz.SetString(extl, _T(" ;,*.\t\r\n"), wxTOKEN_STRTOK);
    while ( tkz.HasMoreTokens() )
    {
        wxString token = tkz.GetNextToken();
        m_FortranExtFree.insert(token);
    }
}

void FortranFileExt::RereadOptions()
{
    m_ExtDone = false;
}

void FortranFileExt::GetFortranFileExts(StringSet& fileExts)
{
    RereadFileExtensions();
    fileExts.clear();
    StringSet::iterator it;
    for( it = m_FortranExtFixed.begin(); it != m_FortranExtFixed.end(); ++it )
    {
        fileExts.insert(*it);
    }
    for( it = m_FortranExtFree.begin(); it != m_FortranExtFree.end(); ++it )
    {
        fileExts.insert(*it);
    }
}


