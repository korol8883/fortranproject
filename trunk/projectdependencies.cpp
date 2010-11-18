/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Author: Darius Markauskas
 *
 */

#include "projectdependencies.h"
#include <projectmanager.h>
#include <compiler.h>
#include <compilerfactory.h>
#include <macrosmanager.h>
#include <wx/filefn.h>
#include <wx/dir.h>

#include "nativeparserf.h"


ProjectDependencies::ProjectDependencies(cbProject* project)
{
    m_Project = project;
    //ctor
}

ProjectDependencies::~ProjectDependencies()
{
    //dtor
    Clear();
}

void ProjectDependencies::Clear()
{
    m_prFilesArr.clear();
    m_FileIndexMap.clear();
    for (size_t i=0; i<m_pUseModules.size(); i++)
    {
        delete m_pUseModules[i];
    }
    m_pUseModules.clear();
    for (size_t i=0; i<m_pDeclaredModules.size(); i++)
    {
        delete m_pDeclaredModules[i];
    }
    m_pDeclaredModules.clear();

    for (size_t i=0; i<m_pIncludes.size(); i++)
    {
        delete m_pIncludes[i];
    }
    m_pIncludes.clear();

    m_ModuleFileIdxMap.clear();
    m_IncludeFileIdxMap.clear();

    for (size_t i=0; i<m_ChildrenTable.size(); i++)
    {
        delete m_ChildrenTable[i];
    }
    m_ChildrenTable.clear();
    m_WasInfiniteLoop = false;
}

void ProjectDependencies::MakeProjectFilesDependencies(ProjectFilesArray& prFilesArr, ParserF& parser)
{
    Clear();

    m_prFilesArr = prFilesArr;
    wxArrayString fnames;

    size_t nfil = m_prFilesArr.size();
	for (size_t i = 0; i < nfil; ++i)
	{
        ProjectFile* pf = m_prFilesArr[i];
        wxString ffp = pf->file.GetFullPath();
        m_FileIndexMap.insert(std::make_pair(ffp,i));

        wxString fname = pf->file.GetName() + _T(".") + pf->file.GetExt();
        fnames.Add(fname);
	}

	for (size_t i = 0; i < nfil; ++i)
	{
        StringSet* fileUseModules = new StringSet;
        StringSet* fileDeclaredModules = new StringSet;
        StringSet* fileIncludes = new StringSet;

        parser.ObtainUsedDeclaredModules(m_prFilesArr[i]->file.GetFullPath(), fileUseModules, fileDeclaredModules, fileIncludes);
        m_pUseModules.push_back(fileUseModules);
        m_pDeclaredModules.push_back(fileDeclaredModules);
        m_pIncludes.push_back(fileIncludes);

        StringSet::iterator pos;
	    for (pos = fileDeclaredModules->begin(); pos != fileDeclaredModules->end(); ++pos)
	    {
	        if (!m_ModuleFileIdxMap.count(*pos))
	        {
                m_ModuleFileIdxMap[*pos] = i;
	        }
	    }

	    for (pos = fileIncludes->begin(); pos != fileIncludes->end(); ++pos)
	    {
	        wxString inc = *pos;
	        for (size_t j=0; j < nfil; ++j)
	        {
	            wxString fn = fnames.Item(j);
	            if (inc.IsSameAs(fn) || inc.IsSameAs(fn.BeforeLast('.')))
	            {
	                m_IncludeFileIdxMap[*pos] = j;
	                break;
	            }
	        }
	    }
	}

	for (size_t i = 0; i < nfil; ++i)
	{
	    IntSet* children = new IntSet;
	    m_Deep = 0;
	    m_BreakChain = false;
	    MakeFileChildren(children, i);
	    m_ChildrenTable.push_back(children);
	}
}


bool ProjectDependencies::HasInfiniteDependences()
{
    return m_WasInfiniteLoop;
}


unsigned short int ProjectDependencies::GetFileWeight(wxString& fileName)
{
    if (!m_FileIndexMap.count(fileName))
        return 50; //error. default value

    m_Deep = 0;
    m_BreakChain = false;
    unsigned short int wt = GetFileWeightByIndex(m_FileIndexMap[fileName]);
    return wt;
}


unsigned short int ProjectDependencies::GetFileWeightByIndex(size_t idx)
{
    if (m_Deep > 99)
    {
        m_WasInfiniteLoop = true;
        m_BreakChain = true;
        return 0;
    }
    else if (m_BreakChain)
    {
        return 0;
    }
    unsigned short int wt;
    unsigned short int wt_max = 0;
    StringSet* fileUseModules = m_pUseModules[idx];
    StringSet::iterator pos;
    for (pos = fileUseModules->begin(); pos != fileUseModules->end(); ++pos)
    {
        if (m_ModuleFileIdxMap.count(*pos) != 1)
        {
            continue;
        }
        else
        {
            size_t fidx = m_ModuleFileIdxMap[*pos];
            if (fidx == idx)
                continue; // module defined and is used in the same file.
            m_Deep++;
            wt = 1 + GetFileWeightByIndex(fidx);
            m_Deep--;

            if (wt > wt_max)
                wt_max = wt;
        }
    }

    StringSet* fileIncludes = m_pIncludes[idx];
    for (pos = fileIncludes->begin(); pos != fileIncludes->end(); ++pos)
    {
        if (m_IncludeFileIdxMap.count(*pos) != 1)
        {
            continue;
        }
        else
        {
            size_t fidx = m_IncludeFileIdxMap[*pos];
            if (fidx == idx)
                continue; // error, includes self
            m_Deep++;
            wt = 1 + GetFileWeightByIndex(fidx);
            m_Deep--;

            if (wt > wt_max)
                wt_max = wt;
        }
    }
    return wt_max;
}

void ProjectDependencies::MakeFileChildren(IntSet* children, size_t fileIndex)
{
    if (m_Deep > 99)
    {
        m_BreakChain = true;
        return; // may be infinite reference loop?
    }
    else if (m_BreakChain)
    {
        return;
    }
    StringSet* fileDeclaredModules = m_pDeclaredModules[fileIndex];
    StringSet::iterator pos;
    for (pos = fileDeclaredModules->begin(); pos != fileDeclaredModules->end(); ++pos)
    {
        wxString modName = *pos;
        for (size_t k=0; k < m_pUseModules.size(); ++k)
        {
            if (fileIndex==k)
                continue; // declared and used in the same file

            if (m_pUseModules[k]->count(modName))
            {
                children->insert(k);
                m_Deep++;
                MakeFileChildren(children, k);
                m_Deep--;
            }
        }
    }

    ProjectFile* pf = m_prFilesArr[fileIndex];
    wxString fname = pf->file.GetName();
    wxString fnameExt = fname + _T(".") + pf->file.GetExt();
    for (size_t k=0; k < m_pIncludes.size(); ++k)
    {
        if (fileIndex==k)
            continue;

        if (m_pIncludes[k]->count(fname) || m_pIncludes[k]->count(fnameExt))
        {
            children->insert(k);
            m_Deep++;
            MakeFileChildren(children, k);
            m_Deep--;
        }
    }
}

void ProjectDependencies::EnsureUpToDateObjs()
{
    int btc = m_Project->GetBuildTargetsCount();
    for (int it=0; it<btc; ++it)
    {
        ProjectBuildTarget* bTarget = m_Project->GetBuildTarget(it);
        Compiler* compiler = CompilerFactory::GetCompiler(bTarget->GetCompilerID());
        if(!compiler)
            continue;
        for (size_t j=0; j<m_prFilesArr.size(); ++j)
        {
            ProjectFile* pf = m_prFilesArr[j];
            const pfDetails& pfd = pf->GetFileDetails(bTarget);

            if (!&pfd)
                continue;

            wxString objectAbs = (compiler->GetSwitches().UseFlatObjects)?pfd.object_file_flat_absolute_native:pfd.object_file_absolute_native;

            time_t time_src = wxFileModificationTime(pfd.source_file_absolute_native);
            bool delChildren = false;
            bool onlyOlder = false;
            if (wxFileExists(objectAbs))
            {
                time_t time_obj = wxFileModificationTime(objectAbs);
                if (time_src > time_obj)
                {
                    delChildren = true;
                }
            }
            else
            {
                delChildren = true;
                if(!pf->compile)
                {
                    onlyOlder = true;
                }
            }

            if (delChildren)
            {
                IntSet* children = m_ChildrenTable[j];
                IntSet::iterator pos;
                for (pos=children->begin(); pos != children->end(); ++pos)
                {
                    ProjectFile* pfChild = m_prFilesArr[*pos];
                    const pfDetails& pfdChild = pfChild->GetFileDetails(bTarget);
                    if (!&pfdChild)
                        continue;
                    wxString objectAbsChild = (compiler->GetSwitches().UseFlatObjects)?pfdChild.object_file_flat_absolute_native:pfdChild.object_file_absolute_native;

                    if (wxFileExists(objectAbsChild))
                    {
                        if (onlyOlder)
                        {
                            time_t time_obj = wxFileModificationTime(objectAbsChild);
                            if (time_src > time_obj)
                            {
                                wxRemoveFile(objectAbsChild);
                            }
                        }
                        else
                        {
                            wxRemoveFile(objectAbsChild);
                        }
                    }
                }
            }
        }
    }
}

void ProjectDependencies::RemoveModFiles(cbProject* pr, ProjectBuildTarget* bTarget, NativeParserF* nativeParser)
{
    //Remove all *.mod files
    if (!pr || !bTarget || pr->IsMakefileCustom())
        return;

    wxString comID = bTarget->GetCompilerID();
    if (!CompilerFactory::CompilerInheritsFrom(comID, _T("gfortran")) &&
        !CompilerFactory::CompilerInheritsFrom(comID, _T("g95")) &&
        !CompilerFactory::CompilerInheritsFrom(comID, _T("ifcwin")) &&
        !CompilerFactory::CompilerInheritsFrom(comID, _T("ifclin")) &&
        !CompilerFactory::CompilerInheritsFrom(comID, _T("pgfortran")) )
    {
        bool haveFortran = false;
        for (int i = 0; i < pr->GetFilesCount(); ++i)
        {
            if (nativeParser->IsFileFortran(pr->GetFile(i)->file.GetFullPath()))
            {
                haveFortran = true;
                break;
            }
        }
        if (!haveFortran)
            return;
    }

    wxString objDir;
    bool found = false;
    for (int i=0; i < pr->GetFilesCount(); i++)
    {
        ProjectFile* pf = pr->GetFile(i);
        const pfDetails& pfd = pf->GetFileDetails(bTarget);
        if (&pfd)
        {
            wxFileName objFname = pfd.object_file_absolute_native;
            objDir = objFname.GetPath();

            found = true;
            break;
        }
    }
    if (!found)
        return;

    wxDir odir;
    if (odir.Open(objDir))
    {
        wxString filename;
        wxFileName fname;
        fname.AssignDir(objDir);
        wxString filespec = _T("*.mod");
        bool cont = odir.GetFirst(&filename, filespec, wxDIR_FILES);
        while ( cont )
        {
            fname.SetFullName(filename);
            wxRemoveFile(fname.GetFullPath());
            cont = odir.GetNext(&filename);
        }
    }
}

void ProjectDependencies::RemoveModFilesWS(NativeParserF* nativeParser)
{
    //Remove all *.mod files in Workspace
    ProjectsArray* projects = Manager::Get()->GetProjectManager()->GetProjects();
    for (size_t i = 0; i < projects->GetCount(); ++i)
    {
        cbProject* pr = projects->Item(i);
        if (!pr->IsMakefileCustom())
        {
            ProjectBuildTarget* bTarget = pr->GetBuildTarget(pr->GetActiveBuildTarget());
            RemoveModFiles(pr, bTarget, nativeParser);
        }
    }
}


