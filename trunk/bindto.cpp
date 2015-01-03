#include "bindto.h"

//(*InternalHeaders(Bindto)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#include <editormanager.h>
#include <cbstyledtextctrl.h>
#include <configmanager.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include "bindtonewtype.h"

//(*IdInit(Bindto)
const long Bindto::ID_BTOACTIVEPROJECT = wxNewId();
const long Bindto::ID_BTOCURRENTFILE = wxNewId();
const long Bindto::ID_LV_TYPES = wxNewId();
const long Bindto::ID_BUTTON_ADD = wxNewId();
const long Bindto::ID_BUTTON_EDIT = wxNewId();
const long Bindto::ID_BUTTON_REMOVE = wxNewId();
const long Bindto::ID_BUTTON_DEFAULTS = wxNewId();
//*)

BEGIN_EVENT_TABLE(Bindto,wxDialog)
	//(*EventTable(Bindto)
	//*)
	EVT_BUTTON  (wxID_OK, Bindto::OnOK)
END_EVENT_TABLE()

wxString DIM_VAR_KEY = _T("<<@%%@>>");

Bindto::Bindto(wxWindow* parent, ParserF* pParser)
{
	//(*Initialize(Bindto)
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer2;
	wxStaticText* StaticText1;
	wxStaticText* StaticText3;
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer1;
	wxStaticText* StaticText2;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;
	wxBoxSizer* BoxSizer5;

	Create(parent, wxID_ANY, _("Bind To"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	StaticText1 = new wxStaticText(this, wxID_ANY, _("This tool generates a wrapping for Fortran code to be called from the C language."), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	BoxSizer2->Add(StaticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, wxID_ANY, _("Generate wrapping for:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	BoxSizer2->Add(StaticText2, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer3->Add(30,0,0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	rb_ActiveProject = new wxRadioButton(this, ID_BTOACTIVEPROJECT, _("Active project"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, _T("ID_BTOACTIVEPROJECT"));
	BoxSizer3->Add(rb_ActiveProject, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	rb_CurrentFile = new wxRadioButton(this, ID_BTOCURRENTFILE, _("Current file"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BTOCURRENTFILE"));
	BoxSizer3->Add(rb_CurrentFile, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(BoxSizer3, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	StaticText3 = new wxStaticText(this, wxID_ANY, _("Binding types"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	BoxSizer4->Add(StaticText3, 0, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	lv_Types = new wxListView(this, ID_LV_TYPES, wxDefaultPosition, wxSize(500,300), wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES|wxSUNKEN_BORDER, wxDefaultValidator, _T("ID_LV_TYPES"));
	BoxSizer4->Add(lv_Types, 1, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	bt_Add = new wxButton(this, ID_BUTTON_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_ADD"));
	bt_Add->SetToolTip(_("Add a new type"));
	BoxSizer5->Add(bt_Add, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	bt_Edit = new wxButton(this, ID_BUTTON_EDIT, _("Edit"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_EDIT"));
	bt_Edit->SetToolTip(_("Edit selected type"));
	BoxSizer5->Add(bt_Edit, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	bt_Remove = new wxButton(this, ID_BUTTON_REMOVE, _("Remove"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_REMOVE"));
	bt_Remove->SetToolTip(_("Remove selected type"));
	BoxSizer5->Add(bt_Remove, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer5->Add(-1,-1,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	bt_Defaults = new wxButton(this, ID_BUTTON_DEFAULTS, _("Defaults"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_DEFAULTS"));
	bt_Defaults->SetToolTip(_("Restore default binding types"));
	BoxSizer5->Add(bt_Defaults, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4->Add(BoxSizer5, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(BoxSizer4, 1, wxTOP|wxBOTTOM|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BUTTON_ADD,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Bindto::OnAdd);
	Connect(ID_BUTTON_EDIT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Bindto::OnEdit);
	Connect(ID_BUTTON_REMOVE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Bindto::OnRemove);
	Connect(ID_BUTTON_DEFAULTS,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&Bindto::OnDefaults);
	//*)

    lv_Types->InsertColumn(0,_T("Fortran"));
    lv_Types->InsertColumn(1,_T("Fortran Bind(C)"));
    lv_Types->InsertColumn(2,_T("C"));

	rb_CurrentFile->SetValue(true);
    m_pParser = pParser;

    m_TabSize = -1;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (ed)
    {
        cbStyledTextCtrl* control = ed->GetControl();
        if (control)
            m_TabSize = control->GetTabWidth();
    }
    if (m_TabSize == -1)
        m_TabSize = 4;

    FillTypeMap();
    FillTypeList();

    for (int i=0; i< lv_Types->GetColumnCount(); i++)
    {
        lv_Types->SetColumnWidth(i,wxLIST_AUTOSIZE);
        if (lv_Types->GetColumnWidth(i) > 200)
            lv_Types->SetColumnWidth(i,200);
    }
}

Bindto::~Bindto()
{
	//(*Destroy(Bindto)
	//*)
}
void Bindto::FillTypeList()
{
    if (!lv_Types)
        return;

    lv_Types->DeleteAllItems();
    int idx = 0;
    for ( TypeMap::iterator it=m_TypeMap.begin(); it != m_TypeMap.end(); ++it)
    {
        lv_Types->InsertItem(idx, it->first);
        lv_Types->SetItem(idx, 1, it->second[0]);
        lv_Types->SetItem(idx, 2, it->second[1]);
        idx++;
    }
}

void Bindto::FillTypeMap()
{
    m_IsTypeMapDefault = false;
    LoadBindToConfig();

    if (m_TypeMap.size() == 0)
        FillTypeMapDefault();
}

void Bindto::FillTypeMapDefault()
{
    wxArrayString fTypes;
    wxArrayString bTypes;
    wxArrayString cTypes;
    fTypes.Add(_T("integer"));
    bTypes.Add(_T("integer(c_int)"));
    cTypes.Add(_T("int"));

    fTypes.Add(_T("integer(8)"));
    bTypes.Add(_T("integer(c_int64_t)"));
    cTypes.Add(_T("int64_t"));

    fTypes.Add(_T("integer(4)"));
    bTypes.Add(_T("integer(c_int32_t)"));
    cTypes.Add(_T("int32_t"));

    fTypes.Add(_T("integer(2)"));
    bTypes.Add(_T("integer(c_int16_t)"));
    cTypes.Add(_T("int16_t"));

    fTypes.Add(_T("integer(1)"));
    bTypes.Add(_T("integer(c_int8_t)"));
    cTypes.Add(_T("int8_t"));

    fTypes.Add(_T("integer(c_int)"));
    bTypes.Add(_T("integer(c_int)"));
    cTypes.Add(_T("int"));

    fTypes.Add(_T("real"));
    bTypes.Add(_T("real(c_float)"));
    cTypes.Add(_T("float"));

    fTypes.Add(_T("real(4)"));
    bTypes.Add(_T("real(c_float)"));
    cTypes.Add(_T("float"));

    fTypes.Add(_T("real(8)"));
    bTypes.Add(_T("real(c_double)"));
    cTypes.Add(_T("double"));

    fTypes.Add(_T("doubleprecision"));
    bTypes.Add(_T("real(c_double)"));
    cTypes.Add(_T("double"));

    fTypes.Add(_T("real(c_float)"));
    bTypes.Add(_T("real(c_float)"));
    cTypes.Add(_T("float"));

    fTypes.Add(_T("real(c_double)"));
    bTypes.Add(_T("real(c_double)"));
    cTypes.Add(_T("double"));

    fTypes.Add(_T("complex"));
    bTypes.Add(_T("complex(c_float_complex)"));
    cTypes.Add(_T("float complex"));

    fTypes.Add(_T("complex(4)"));
    bTypes.Add(_T("complex(c_float_complex)"));
    cTypes.Add(_T("float complex"));

    //requires <complex.h>
    fTypes.Add(_T("complex(8)"));
    bTypes.Add(_T("complex(c_double_complex)"));
    cTypes.Add(_T("double complex"));

    // requires <stdbool.h> and logical() function to convert from and to
    fTypes.Add(_T("logical(1)"));
    bTypes.Add(_T("logical(c_bool)"));
    cTypes.Add(_T("bool"));

    fTypes.Add(_T("character"));
    bTypes.Add(_T("character(kind=c_char)"));
    cTypes.Add(_T("char"));

    fTypes.Add(_T("character(kind=c_char)"));
    bTypes.Add(_T("character(kind=c_char)"));
    cTypes.Add(_T("char"));

    m_TypeMap.clear();
    for (size_t i=0; i<fTypes.size(); i++)
    {
        wxArrayString ct;
        ct.Add(bTypes[i]);
        ct.Add(cTypes[i]);
        m_TypeMap[fTypes[i]] = ct;
    }
    m_IsTypeMapDefault = true;
}

void Bindto::LoadBindToConfig()
{
    m_IsTypeMapDefault = false;
    m_TypeMap.clear();
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));
    if (!cfg)
        return;

    wxArrayString list = cfg->EnumerateSubPaths(_T("/bind_to"));
    for (unsigned int i = 0; i < list.GetCount(); ++i)
    {
        if (!list[i].StartsWith(_T("type")))
            continue;
        wxString fT = cfg->Read(_T("/bind_to/") + list[i] + _T("/f_type"), wxEmptyString);
        wxString bT = cfg->Read(_T("/bind_to/") + list[i] + _T("/b_type"), wxEmptyString);
        wxString cT = cfg->Read(_T("/bind_to/") + list[i] + _T("/c_type"), wxEmptyString);

        if (fT.IsEmpty())
            continue;

        wxArrayString bct;
        bct.Add(bT);
        bct.Add(cT);
        m_TypeMap[fT] = bct;
    }
}

void Bindto::SaveBindToConfig()
{

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));
    if (!cfg)
        return;
    cfg->DeleteSubPath(_T("/bind_to"));

    if (m_IsTypeMapDefault)
        return; // no need to save default types

    TypeMap::iterator it;
    int count = 0;
    for (it = m_TypeMap.begin(); it != m_TypeMap.end(); ++it)
    {
        wxString fT = it->first;
        wxString bT = it->second[0];
        wxString cT = it->second[1];

        ++count;
        wxString key;
        key.Printf(_T("/bind_to/type%d/f_type"), count);
        cfg->Write(key, fT);
        key.Printf(_T("/bind_to/type%d/b_type"), count);
        cfg->Write(key, bT);
        key.Printf(_T("/bind_to/type%d/c_type"), count);
        cfg->Write(key, cT);
    }
}

void Bindto::OnOK(wxCommandEvent& event)
{
    BindToIn btin;
    if (rb_ActiveProject->GetValue())
        btin = bindToProject;
    else
        btin = bindToFile;

    SaveBindToConfig();
    MakeBindTo(btin);

    if (m_CreatedMsg.size() > 0)
    {
        wxString msg;
        for (size_t i=0; i< m_CreatedMsg.size(); i++)
        {
            msg << m_CreatedMsg.Item(i) << _T("\n");
        }
        cbMessageBox( msg, _("Bindto Info"), wxICON_INFORMATION);
    }

    EndModal(wxID_OK);
}

void Bindto::MakeBindTo(BindToIn btin)
{
    if (!Manager::Get()->GetEditorManager() || !m_pParser)
        return;

    if (btin == bindToProject)
    {
//        cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
//        if (!project)
//            return;
//
//        wxArrayString nonFFiles;
//        for (FilesList::iterator it = project->GetFilesList().begin(); it != project->GetFilesList().end(); ++it)
//        {
//            ProjectFile* pf = *it;
//            FortranSourceForm fsForm;
//            if (g_FortranFileExt.IsFileFortran(pf->file.GetFullPath(), fsForm))
//                FileBindTo(pf->file.GetFullPath(), chin, tabSize);
//            else
//                nonFFiles.Add(pf->file.GetFullName());
//        }
//
//        if (nonFFiles.size() > 0)
//        {
//            wxString mstr;
//            if (nonFFiles.size() == 1)
//            {
//                mstr = _("File \"") + nonFFiles[0] + _("\" was not recognized as a Fortran file.");
//                mstr << _(" The BindTo was not applied for it.");
//            }
//            else
//            {
//                mstr = _("Files");
//                size_t i=0;
//                size_t imax=5;
//                while (i < nonFFiles.size() && i < imax)
//                {
//                    mstr << _("\n\"") << nonFFiles[i] << _T("\"");
//                    i++;
//                }
//                if (nonFFiles.size() > imax)
//                    mstr << _T("...\n");
//                else
//                    mstr << _T("\n");
//                mstr << wxString::Format(_T("(%d "), nonFFiles.size()) << _("files) ");
//                mstr << _("were not recognized as the Fortran files.");
//                mstr << _(" The BindTo was not applied for them.");
//                cbMessageBox(mstr, _("Info"), wxICON_INFORMATION);
//            }
//        }

    }
    else
    {
        // Bind current file
        cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if (!ed)
            return;
        FileBindTo(ed->GetFilename());
    }
}

void Bindto::FileBindTo(const wxString& filename)
{
    FortranSourceForm fsForm;
    if (!g_FortranFileExt.IsFileFortran(filename, fsForm))
    {
        cbMessageBox( _("The file \n") + filename +
                      _("\n is not recognized as a Fortran Source File."), _("Info"),
                      wxICON_INFORMATION);
        return;
    }
    TokenF* fileToken = m_pParser->FindFile(filename);

    if (!fileToken)
        return;

    m_CInclude.clear();
    m_CStructs = _T("");
    m_WarnMessage = _T("");
    wxString txtBindGM;
    wxString txtBindMod;
    wxString txtHeadersGM;
    wxString txtHeadersMod;
    bool inModuleGM = false;
    m_WriteStrCtoF = false;
    m_WriteStrFtoC = false;
    m_WriteStrLen = false;
    m_Indent  = 0;

    TokensArrayF* fchen = &fileToken->m_Children;
    for (size_t i=0; i<fchen->GetCount(); i++)
    {
        if (fchen->Item(i)->m_TokenKind == tkSubroutine ||
            fchen->Item(i)->m_TokenKind == tkFunction)
        {
            if (!inModuleGM)
            {
                wxFileName fn(fileToken->m_Filename);
                wxString modName = fn.GetName() + _T("_proc_bc");
                txtBindGM << _T("module ") << modName << _T("\n");
                m_Indent++;
                txtBindGM << GetIS() << _T("use, intrinsic :: iso_c_binding\n");
                txtBindGM << GetIS() << _T("$#$#%^@@place for helper module$#@%\n");
                txtBindGM << GetIS() << _T("implicit none\n");
                txtBindGM << _T("contains\n\n");
                inModuleGM = true;
                txtHeadersGM << _T("// Global procedures\n");
            }
            m_Indent = 1;
            BindProcedure(txtBindGM, txtHeadersGM, fchen->Item(i), true);
        }
        else if (fchen->Item(i)->m_TokenKind == tkModule)
        {
            txtBindMod << _T("module ") << fchen->Item(i)->m_Name << _T("_bc\n");
            m_Indent = 1;
            txtBindMod << GetIS() << _T("use :: ") << fchen->Item(i)->m_Name << _T("\n");
            txtBindMod << GetIS() << _T("use, intrinsic :: iso_c_binding\n");
            txtBindMod << GetIS() << _T("$#$#%^@@place for helper module$#@%\n");
            txtBindMod << GetIS() << _T("implicit none\n");
            txtBindMod << _T("contains\n\n");

            txtHeadersMod << _T("\n// Module '") << fchen->Item(i)->m_Name << _T("' procedures\n");

            TokensArrayF* mchil = &fchen->Item(i)->m_Children;
            for (size_t j=0; j<mchil->GetCount(); j++)
            {
                if ((mchil->Item(j)->m_TokenKind == tkSubroutine ||
                    mchil->Item(j)->m_TokenKind == tkFunction) &&
                    mchil->Item(j)->m_TokenAccess == taPublic)
                {
                    BindProcedure(txtBindMod, txtHeadersMod, mchil->Item(j), false);
                }
                else if (mchil->Item(j)->m_TokenKind == tkInterfaceExplicit &&
                         mchil->Item(j)->m_TokenAccess == taPublic)
                {
                    TokensArrayF* intchs = &mchil->Item(j)->m_Children;
                    for (size_t k=0; k<intchs->GetCount(); k++)
                    {
                        if ((intchs->Item(k)->m_TokenKind == tkSubroutine ||
                            intchs->Item(k)->m_TokenKind == tkFunction) &&
                            intchs->Item(k)->m_TokenAccess == taPublic)
                        {
                            BindProcedure(txtBindMod, txtHeadersMod, intchs->Item(k), false);
                        }
                    }
                }
                else if (mchil->Item(j)->m_TokenKind == tkInterface &&
                         mchil->Item(j)->m_TokenAccess == taPublic &&
                         !mchil->Item(j)->m_Name.IsEmpty())
                {
                    TokensArrayF* intchs = &mchil->Item(j)->m_Children;
                    for (size_t k=0; k<intchs->GetCount(); k++)
                    {
                        wxString iname = intchs->Item(k)->m_Name;
                        for (size_t l=0; l<mchil->GetCount(); l++)
                        {
                            if ((mchil->Item(l)->m_TokenKind == tkSubroutine ||
                                mchil->Item(l)->m_TokenKind == tkFunction) &&
                                mchil->Item(l)->m_TokenAccess == taPrivate &&   // items with taPublic are called separately
                                mchil->Item(l)->m_Name.IsSameAs(iname))
                            {
                                BindProcedure(txtBindMod, txtHeadersMod, mchil->Item(l), false, mchil->Item(j)->m_DisplayName);
                            }
                        }
                    }
                }
            }
            txtBindMod << _T("end module\n\n");
        }
    }
    if (inModuleGM)
        txtBindGM << _T("end module\n\n");

    wxString helperMod = GetHelperModule();
    if (!helperMod.empty() && !txtBindGM.empty())
        txtBindGM.Replace(_T("$#$#%^@@place for helper module$#@%"),_T("use :: bindc_helper_bc"));
    else if (!txtBindGM.empty())
        txtBindGM.Replace(_T("$#$#%^@@place for helper module$#@%\n"),_T(""));

    if (!helperMod.empty() && !txtBindMod.empty())
        txtBindMod.Replace(_T("$#$#%^@@place for helper module$#@%"),_T("use :: bindc_helper_bc"));
    else if (!txtBindMod.empty())
        txtBindMod.Replace(_T("$#$#%^@@place for helper module$#@%\n"),_T(""));

    wxString bfname = CreateBindFilename(filename, false);
    if (bfname.IsEmpty())
        return;
    wxString hname = CreateBindFilename(filename, true);
    if (hname.IsEmpty())
        return;

    wxFile f(bfname, wxFile::write);
    cbWrite(f, m_WarnMessage + SplitLines(helperMod,_T("Fortran")) + SplitLines(txtBindGM,_T("Fortran")) +
            SplitLines(txtBindMod,_T("Fortran")) + GetEOLStr(), wxFONTENCODING_UTF8);

    wxFileName hfname(hname);
    wxString hstr1;
    hstr1 << _T("#ifndef ") << hfname.GetName().Upper() << _T("_H") << _T("\n");
    hstr1 << _T("#define ") << hfname.GetName().Upper() << _T("_H") << _T("\n\n");
    if (!m_CInclude.empty())
    {
        StrSet::iterator it;
        for (it=m_CInclude.begin(); it != m_CInclude.end(); ++it)
        {
            hstr1 << *it << _T("\n");
        }
    }
    wxString hstr2 = _T("\n#endif");

    wxFile hf(hname, wxFile::write);
    cbWrite(hf, hstr1 + m_CStructs + _T("\n") + SplitLines(txtHeadersGM,_T("C")) + SplitLines(txtHeadersMod,_T("C")) +
             hstr2 + GetEOLStr(), wxFONTENCODING_UTF8);

    m_CreatedMsg.Add(_("Files ") + bfname.AfterLast('/') + _T(", ") + hname.AfterLast('/') + _(" were created in ")
                     + hname.BeforeLast('/') + _(" folder."));
    if (!m_WarnMessage.IsEmpty())
    {
        m_CreatedMsg.Add(_("\nThere were problems met during the generation of wrapping. Message was added to the beginning of ")
                         +bfname.AfterLast('/')+_(" file."));
    }
}

/** \brief Get Indent Spaces
 *
 * \return wxString Returns required number of spaces
 *
 */
wxString Bindto::GetIS()
{
    wxString spaces;
    return spaces.Append(' ',m_TabSize*m_Indent);
}

wxString Bindto::CreateBindFilename(const wxString& filename, bool header)
{
    wxFileName fname(filename);
    if (header)
        fname.SetExt(_T("h"));
    else
    {
        wxString ext = fname.GetExt();
        if (ext != _T("f90") && ext != _T("f95") && ext != _T("f03") && ext != _T("f08"))
            fname.SetExt(_T("f90"));
    }
    wxString name = fname.GetName() << _T("_bc");
    fname.SetName(name);

    while (fname.FileExists())
    {
        wxString query_overwrite;
        query_overwrite.Printf(
          _T("Warning:\n")
          _T("This tool is about OVERWRITE the following existing file:\n")+
          fname.GetFullPath()+_T("\n\n") +
          _T("Are you sure that you want to OVERWRITE the file?\n\n")+
          _T("(If you answer 'No' the existing file will be kept.)"));
        int answ = cbMessageBox(query_overwrite, _T("Confirmation"),
                         wxICON_QUESTION | wxYES_NO | wxNO_DEFAULT);
        if (answ == wxID_NO)
        {
            bool n_changed = false;
            name = fname.GetName();
            wxRegEx reEnd(_T("_([0-9]*$)"));
            if (reEnd.Matches(name))
            {
                // increase file number
                wxString sn = reEnd.GetMatch(name, 1);
                long fn;
                if (sn.ToLong(&fn))
                {
                    fn++;
                    wxString snn;
                    snn << fn;
                    name = name.Mid(0,name.size()-sn.size()) + snn;
                    n_changed = true;
                }
            }

            if (!n_changed)
            {
                name << _T("_1");
            }
            fname.SetName(name);
        }
        else if (answ == wxID_YES)
            break;
        else
            return wxEmptyString;
    }
    return fname.GetFullPath();
}


void Bindto::BindProcedure(wxString& txtBind, wxString& txtHeaders, TokenF* token, bool isGlobal, wxString callName)
{
    wxString txtBindProc;
    wxString txtBindFirst;
    wxString txtBindSecond;
    wxString funResVar;
    wxString addFunVariable;
    wxArrayString funInterface;
    wxString cFunResVar;
    wxString funTypeDec;
    wxArrayString additionalDeclar;
    wxArrayString additionalCalls;
    wxArrayString additionalCalls2;
    std::map<wxString,wxString> changedNamesMap;
    wxString procName = token->m_Name + _T("_bc");
    wxString cName = token->m_Name;
    bool bindKindSubroutine = true;
    if (token->m_TokenKind == tkSubroutine)
        txtBindFirst << GetIS() << _T("subroutine ");
    else if (token->m_TokenKind == tkFunction)
    {
        txtBindFirst << GetIS() << _T("function ");
        bindKindSubroutine = false;
    }

    txtBindFirst << procName;
    m_Indent++;
    bool wasChlen = false;
    wxArrayString argArr;
    wxStringTokenizer tkz(token->m_Args.Lower(), _T("(),[] \t\r\n"), wxTOKEN_STRTOK );
    while ( tkz.HasMoreTokens() )
    {
        argArr.Add(tkz.GetNextToken());
    }

    txtBindFirst << _T("(");
    for (size_t i=0; i<argArr.GetCount(); i++)
    {
        txtBindFirst << argArr.Item(i);
        if (i+1 < argArr.GetCount())
            txtBindFirst << _T(", ");
    }

    if (token->m_TokenKind == tkFunction)
    {
        funInterface.Add(_T("interface"));
        funInterface.Add(_T("function ") + token->m_Name + token->m_Args.Lower());

        wxString funT = GetFunctionDeclaration(token);
        m_pTokenCurrent = token;
        int itmp;
        wxArrayString tya = GetBindType(funT, itmp);
        funTypeDec = tya[0];
        if (tya[3].IsEmpty())
        {
            funResVar = procName;
            txtBindSecond << GetIS() << tya[1] << _T(" :: ") << funResVar << _T("\n");
            txtHeaders << tya[2] << _T(" ") << cName << _T("(");
        }
        else
        {
            // If function returns an array, change it to subroutine (void function) with the last array argument.
            bindKindSubroutine = true;
            txtBindFirst.Replace(_T("function"), _T("subroutine"), false);
            funResVar = token->m_Name + _T("_res");
            txtBindSecond << GetIS() << tya[1] << _T(", intent(out) :: ") << funResVar << _T("\n");
            addFunVariable << _T(", ") + funResVar;
            txtHeaders << _T("void ") << cName << _T("(");
            cFunResVar = tya[2] + _T(" ") + funResVar + tya[3];
        }
        funInterface.Add(funTypeDec + _T(" :: ") + token->m_DisplayName);
        if (funTypeDec.StartsWith(_T("character")))
        {
            additionalDeclar.Add(funTypeDec + _T(" :: ") + funResVar + _T("_f"));
            additionalCalls2.Add(_T("call string_copy_f_c(") + funResVar + _T("_f,") + funResVar + _T(")"));
            m_WriteStrFtoC = true;
        }
        else if (!tya[0].StartsWith(_T("type(c_ptr)")) && tya[1].StartsWith(_T("type(c_ptr)")))
        {
            wxString fName = funResVar + _T("_fp");
            additionalDeclar.Add(funTypeDec + _T(", pointer :: ") + fName);
            additionalCalls.Add(_T("allocate(") + fName + _T(")"));
            additionalCalls2.Add(funResVar + _T(" = c_loc(") + fName + _T(")"));
            funResVar = fName;
        }
    }
    else if (token->m_TokenKind == tkSubroutine)
    {
        txtHeaders << _T("void ") << cName << _T("(");
    }

    wxArrayString dimVarNames;
    wxArrayString dimVarNamesFP;
    for (size_t i=0; i<argArr.GetCount(); i++)
    {
        if (i > 0)
            txtHeaders << _T(", ");
        TokenF* argToken = m_pParser->FindTokenBetweenChildren(token, argArr.Item(i));
        if (argToken)
        {
            if (argToken->m_TokenKind == tkVariable)
            {
                m_pTokenCurrent = argToken;
                int nDimVarAdd;
                wxArrayString tya = GetBindType(argToken, nDimVarAdd);
                if (nDimVarAdd > 0)
                    AddDimVariablesFromDoc(dimVarNames, nDimVarAdd, argToken->m_DocString);
                if (nDimVarAdd > 0)
                    AddDimVariables(argArr, dimVarNames, nDimVarAdd, _T("m"));
                if (!tya[0].StartsWith(_T("type(c_ptr)")) && tya[1].StartsWith(_T("type(c_ptr)")))
                {
                    wxString fDec = tya[0];
                    if (fDec.StartsWith(_T("class(")))
                        fDec.Replace(_T("class("), _T("type("), false);
                    fDec.Replace(_T(", intent(in)"),_T(""));
                    fDec.Replace(_T(", intent(out)"),_T(""));
                    fDec.Replace(_T(", intent(inout)"),_T(""));
                    wxString bvName = argToken->m_Name + _T("_fp");
                    wxString fDecHid;
                    int nAD;
                    HideAssumedShape(fDec, fDecHid, nAD);
                    if (nAD > 0)
                    {
                        size_t nVNini = dimVarNamesFP.size();
                        AddDimVariablesFromDoc(dimVarNamesFP, nAD, argToken->m_DocString);
                        if (nAD > 0)
                            AddDimVariables(argArr, dimVarNamesFP, nAD, _T("mdt"));

                        wxString varShape = _T(", [");
                        for (size_t ivn = nVNini; ivn<dimVarNamesFP.size(); ivn++)
                        {
                            varShape << dimVarNamesFP.Item(ivn) << _T(",");
                        }
                        varShape = varShape.Mid(0,varShape.size()-1) + _T("]");
                        additionalCalls.Add(_T("call c_f_pointer(") + argToken->m_Name + _T(", ") + bvName + varShape + _T(")"));
                    }
                    else
                        additionalCalls.Add(_T("call c_f_pointer(") + argToken->m_Name + _T(", ") + bvName + _T(")"));

                    additionalDeclar.Add(fDec + _T(", pointer :: ") + bvName);
                    changedNamesMap[argToken->m_Name] = bvName;
                }
                else if (tya[0].StartsWith(_T("character")))
                {
                    //character(len=:), allocatable :: fname_f

                    if (tya[0].Find(_T("len=*")) != wxNOT_FOUND)
                    {
                        wxString fDec = tya[0];
                        fDec.Replace(_T(", intent(in)"),_T(""));
                        fDec.Replace(_T(", intent(out)"),_T(""));
                        fDec.Replace(_T(", intent(inout)"),_T(""));
                        wxString str = fDec + _T(", allocatable :: ") + argToken->m_Name + _T("_f");
                        str.Replace(_T("len=*"), _T("len=:"));
                        additionalDeclar.Add(str);
                        if (!wasChlen)
                        {
                            additionalDeclar.Add(_T("integer :: chlen_bc"));
                            wasChlen = true;
                        }
                        additionalCalls.Add(_T("chlen_bc = string_len(") + argToken->m_Name + _T(")"));
                        additionalCalls.Add(_T("allocate(character(len=chlen_bc)::") + argToken->m_Name + _T("_f)"));
                        m_WriteStrLen = true;
                    }
                    else
                        additionalDeclar.Add(tya[0] + _T(" :: ") + argToken->m_Name + _T("_f") + argToken->m_Args.Lower());
                    if (tya[0].Find(_T("intent(out)")) == wxNOT_FOUND)
                    {
                        additionalCalls.Add(_T("call string_copy_c_f(") + argToken->m_Name + _T(", ") + argToken->m_Name + _T("_f)"));
                        m_WriteStrCtoF = true;
                    }
                    if (tya[0].Find(_T("intent(in)")) == wxNOT_FOUND)
                    {
                        additionalCalls2.Add(_T("call string_copy_f_c(") + argToken->m_Name + _T("_f, ") + argToken->m_Name + _T(")"));
                        m_WriteStrFtoC = true;
                    }
                    changedNamesMap[argToken->m_Name] = argToken->m_Name + _T("_f");
                }
                txtBindSecond << GetIS() << tya[1] << _T(" :: ") << argToken->m_Name;
                txtHeaders << tya[2] << _T(" ") << argToken->m_Name << tya[3];
                funInterface.Add(tya[0] + _T(" :: ") + argToken->m_Name);
            }
            else
            {
                // it may be a procedure. What then to do?
            }
            txtBindSecond << _T("\n");
        }
        else
        {
            // Should implicit declaration be assumed?
            txtHeaders << argArr.Item(i);
        }
    }
    // Deal with assumed-shape arrays
    wxArrayString addVarNames;
    wxArrayString addVarNamesC;
    PrepareAssumedShapeVariables(txtBindSecond, argArr, dimVarNames, additionalDeclar, addVarNames, addVarNamesC);
    PrepareAssumedShapeVariables(txtBindSecond, argArr, dimVarNamesFP, additionalDeclar, addVarNames, addVarNamesC);
    for (size_t i=0; i<addVarNames.size(); i++)
        txtBindFirst << _T(", ") << addVarNames.Item(i);

    txtBindFirst << addFunVariable;
    txtBindFirst << _T(") bind(c,name='") << cName << _T("')\n");

    for (size_t i=0; i<addVarNamesC.size(); i++)
        txtHeaders << _T(", ") << addVarNamesC.Item(i);
    if (token->m_TokenKind == tkFunction && bindKindSubroutine)
        txtHeaders << _T(", ") << cFunResVar;

    if (isGlobal && token->m_TokenKind == tkFunction)
    {
        if (bindKindSubroutine)
        {
            funInterface.Add(_T("end function"));
            funInterface.Add(_T("end interface"));

            txtBindSecond << _T("\n");
            for (size_t i=0; i<funInterface.size(); i++)
            {
                if (i==1 || i==2)
                    m_Indent++;
                else if (i==funInterface.size()-2 || i==funInterface.size()-1)
                    m_Indent--;
                txtBindSecond << GetIS() << funInterface.Item(i) << _T("\n");
            }
        }
        else
            txtBindSecond << GetIS() << funTypeDec << _T(" :: ") << token->m_DisplayName << _T("\n");
    }

    for (size_t i=0; i<additionalDeclar.size(); i++)
        txtBindSecond << GetIS() << additionalDeclar.Item(i) << _T("\n");

    txtBindSecond << _T("\n");
    txtHeaders << _T(");\n");
    for (size_t i=0; i<additionalCalls.size(); i++)
        txtBindSecond << GetIS() << additionalCalls.Item(i) << _T("\n");

    if (callName.IsEmpty())
        callName = token->m_DisplayName;
    if (token->m_TokenKind == tkSubroutine)
    {
        txtBindSecond << GetIS() << _T("call ") << callName << _T("(");
    }
    else if (token->m_TokenKind == tkFunction)
    {
        if (funTypeDec.StartsWith(_T("character")))
            txtBindSecond << GetIS() << funResVar << _T("_f = ") << callName << _T("(");
        else
            txtBindSecond << GetIS() << funResVar << _T(" = ") << callName << _T("(");
    }

    for (size_t i=0; i<argArr.GetCount(); i++)
    {
        if (changedNamesMap.count(argArr.Item(i)) == 0)
            txtBindSecond << argArr.Item(i);
        else
            txtBindSecond << changedNamesMap[argArr.Item(i)];
        if (i+1 < argArr.GetCount())
            txtBindSecond << _T(", ");
    }
    txtBindSecond << _T(")\n");

    for (size_t i=0; i<additionalCalls2.size(); i++)
        txtBindSecond << GetIS() << additionalCalls2.Item(i) << _T("\n");

    m_Indent--;
    if (bindKindSubroutine)
        txtBindSecond << GetIS() << _T("end subroutine\n\n");
    else
        txtBindSecond << GetIS() << _T("end function\n\n");
    txtBind << txtBindFirst << txtBindSecond;
}


wxArrayString Bindto::GetBindType(TokenF* token, int& nDimVarAdd)
{
    nDimVarAdd = 0;
    wxArrayString retArr = GetBindType(token->m_TypeDefinition.Lower(), nDimVarAdd);
    if (token->m_Args.StartsWith(_T("(")))
    {
        wxString vDim = GetToken(token->m_Args.Lower(),0);
        wxString vDimHid;
        int nAssumedDim;
        HideAssumedShape(vDim, vDimHid, nAssumedDim);
        if (retArr[0].Find(_T("dimension(")) == wxNOT_FOUND)
        {
            int itn = retArr[0].Find(_T(", intent("));
            int itn2 = retArr[1].Find(_T(", intent("));
            if (itn == wxNOT_FOUND || itn2 == wxNOT_FOUND)
            {
                retArr[0] << _T(", dimension") << vDim;
                if (retArr[0].StartsWith(_T("type(c_ptr)")) || !retArr[1].StartsWith(_T("type(c_ptr)")))
                    retArr[1] << _T(", dimension") << vDimHid;
            }
            else
            {
                retArr[0].insert(itn,_T(", dimension")+vDim);
                if (retArr[0].StartsWith(_T("type(c_ptr)")) || !retArr[1].StartsWith(_T("type(c_ptr)")))
                    retArr[1].insert(itn2,_T(", dimension")+vDimHid);
            }
            nDimVarAdd = nAssumedDim;
        }
        else
        {
            int idxDim = retArr[0].Find(_T("dimension("));
            wxString vdimOld;
            if (idxDim != wxNOT_FOUND)
                vdimOld = GetToken(retArr[0],idxDim+9);
            int idxDim2 = retArr[1].Find(_T("dimension("));
            wxString vdimOld2;
            if (idxDim2 != wxNOT_FOUND)
                vdimOld2 = GetToken(retArr[1],idxDim2+9);
            retArr[0].Replace(vdimOld,vDim);
            if (!vdimOld2.IsEmpty())
                retArr[1].Replace(vdimOld2,vDimHid);
            nDimVarAdd = nAssumedDim;
        }
        retArr[3] = GetCDims(vDim);
    }

    if (retArr[3].IsEmpty())
        retArr[2] << _T("*"); // variable as C pointer

    return retArr;
}


wxArrayString Bindto::GetBindType(const wxString& declar, int& nDimVarAdd)
{
    wxString declarLw = declar;
    declarLw.Replace(_T(" "),_T(""));
    nDimVarAdd = 0;

    wxString ftype;
    wxString fCharLen;
    int iPos = declarLw.Find(',');
    int jPos = declarLw.Find('(');
    if (declarLw.StartsWith(_T("character")))
    {
        // deal with character type
        if (iPos == wxNOT_FOUND && jPos == wxNOT_FOUND)
        {
            ftype = _T("character");
            if (declarLw.GetChar(9) == '*' && declarLw.Len() > 10)
            {
                fCharLen = declarLw.Mid(10);
            }
            else
                fCharLen = _T("1");
        }
        else if ((iPos != wxNOT_FOUND && jPos != wxNOT_FOUND && iPos < jPos) ||
                 (iPos != wxNOT_FOUND && jPos == wxNOT_FOUND))
        {
                ftype = _T("character");
                if (declarLw.GetChar(9) == '*' && declarLw.Len() > 10)
                {
                    fCharLen = declarLw.Mid(10,iPos-10);
                }
                else
                    fCharLen = _T("1");
        }
        else if ((iPos != wxNOT_FOUND && jPos != wxNOT_FOUND && iPos > jPos) ||
                 (iPos == wxNOT_FOUND && jPos != wxNOT_FOUND))
        {
            wxString klstr = GetToken(declarLw,jPos);
            int iLen = klstr.Find(_T("len="));
            if (iLen != wxNOT_FOUND)
            {
                int lnd = klstr.Mid(iLen+4).Find(_T(","));
                if (lnd == wxNOT_FOUND)
                {
                    lnd = klstr.Mid(iLen+4).Find(_T(")"));
                    if (lnd == wxNOT_FOUND)
                        lnd = klstr.size();
                }
                fCharLen = klstr.Mid(iLen+4,lnd);
            }
            else
            {
                fCharLen = klstr.Mid(1,klstr.size()-2);
            }

            wxString fCharKind;
            int iKin = klstr.Find(_T("kind="));
            if (iKin != wxNOT_FOUND)
            {
                int lnd = klstr.Mid(iKin+5).Find(_T(","));
                if (lnd == wxNOT_FOUND)
                {
                    lnd = klstr.size()-iKin-6;
                }
                fCharKind = klstr.Mid(iKin+5,lnd);
            }
            if (fCharKind.IsEmpty())
                ftype = _T("character");
            else
                ftype = _T("character(kind=") + fCharKind + _T(")");
        }
    }
    else
    {
        if (iPos != wxNOT_FOUND && jPos != wxNOT_FOUND)
        {
            if (iPos < jPos)
                ftype = declarLw.Mid(0,iPos);
            else
            {
                ftype = declarLw.Mid(0,jPos);
                wxString vkind = GetToken(declarLw,jPos);
                vkind.Replace(_T("kind="),_T(""));
                ftype << vkind;
            }
        }
        else if (iPos == wxNOT_FOUND && jPos != wxNOT_FOUND)
        {
            ftype = declarLw.Mid(0,jPos);
            wxString vkind = GetToken(declarLw,jPos);
            vkind.Replace(_T("kind="),_T(""));
            ftype << vkind;
        }
        else if (iPos != wxNOT_FOUND && jPos == wxNOT_FOUND)
        {
            ftype = declarLw.Mid(0,iPos);
        }
        else //if (iPos == wxNOT_FOUND && jPos == wxNOT_FOUND)
            ftype = declarLw;
    }

    wxArrayString retArr;
    bool wasNotFound = false;
    if (m_TypeMap.count(ftype) == 0)
    {
        wasNotFound = true;
        if (ftype.StartsWith(_T("type(")) || ftype.StartsWith(_T("class(")))
        {
            int st = ftype.Find('(');
            int en = ftype.Find(')');
            if (st != wxNOT_FOUND && en != wxNOT_FOUND)
            {
                wxString tname = ftype.Mid(st+1,en-st-1);
                wxArrayString address;
                m_pParser->GetAddress(m_pTokenCurrent->m_pParent, address);
                TokensArrayFlatClass tokensTmp;
                TokensArrayFlat* resultTmp = tokensTmp.GetTokens();
                m_pParser->FindUseAssociatedTokens(true, address, tname, false, *resultTmp, tkType, false);
                if (resultTmp->size() > 0)
                {
                    TokenF* typeTok = m_pParser->FindToken(resultTmp->Item(0));
                    if (typeTok)
                    {
                        wxString tdef = typeTok->m_TypeDefinition.Lower();
                        tdef.Replace(_T(" "),_T(""));
                        if (tdef.Find(_T("bind(c)")) != wxNOT_FOUND)
                        {
                            // type with bind(c)
                            wxArrayString ct;
                            ct.Add(ftype);
                            wxString c_type;
                            if (en>st)
                                c_type = ftype.Mid(st+1,en-st-1);
                            else
                                c_type = ftype; // something gone wrong
                            ct.Add(c_type);
                            m_TypeMap[ftype] = ct;
                            wasNotFound = false;

                            m_CStructs << _T("\ntypedef struct {\n");
                            TokensArrayF* pChildren = &typeTok->m_Children;
                            for (size_t i=0; i<pChildren->GetCount(); i++)
                            {
                                if (pChildren->Item(i)->m_TokenKind == tkVariable)
                                {
                                    int itmp;
                                    wxArrayString bindTArr = GetBindType(pChildren->Item(i), itmp);
                                    m_CStructs.Append(' ',m_TabSize);
                                    m_CStructs << bindTArr[2] << _T(" ") << pChildren->Item(i)->m_Name << bindTArr[3];
                                    m_CStructs << _T("\n");
                                }
                            }
                            m_CStructs << _T("} ") << typeTok->m_Name << _T(";\n");
                        }
                        else
                        {
                            // type without bind(c)
                            wxArrayString ct;
                            ct.Add(_T("type(c_ptr)"));
                            ct.Add(_T("void*"));
                            m_TypeMap[ftype] = ct;
                            wasNotFound = false;
                        }
                    }
                }
            }
        }
    }

    if (wasNotFound)
    {
        if (m_NotFoundTypes.count(ftype) == 0)
        {
            m_WarnMessage << _T("WARNING: Fortran type '") << ftype << _T("' was not found between bind types.\n");
            m_NotFoundTypes.insert(ftype);
        }
        wxArrayString emptArr;
        emptArr.Add(ftype);
        emptArr.Add(wxEmptyString,3);
        return emptArr;
    }
    else
    {
        retArr = m_TypeMap[ftype]; //size==2
        retArr.Insert(ftype,0); //size==3
        retArr.Add(wxEmptyString); //size==4
    }

    if (retArr[2].StartsWith(_T("int8_t")) ||
        retArr[2].StartsWith(_T("int16_t")) ||
        retArr[2].StartsWith(_T("int32_t")) ||
        retArr[2].StartsWith(_T("int64_t")))
    {
        m_CInclude.insert(_T("#include <stdint.h>"));
    }
    else if (retArr[2].StartsWith(_T("float complex")) ||
             retArr[2].StartsWith(_T("double complex")))
    {
        m_CInclude.insert(_T("#include <complex.h>"));
    }

    iPos = declarLw.Find(_T("dimension("));
    if (iPos != wxNOT_FOUND)
    {
        wxString vdim = GetToken(declarLw,iPos+9);
        retArr[0] << _T(", dimension") << vdim;
        int nAssumedDim;
        wxString vdimHid;
        HideAssumedShape(vdim, vdimHid, nAssumedDim);
        if (!retArr[0].StartsWith(_T("type(c_ptr)")) && retArr[1].StartsWith(_T("type(c_ptr)")))
            ;
        else
        {
            retArr[1] << _T(", dimension") << vdimHid;
            retArr[3] << GetCDims(vdim);
            nDimVarAdd += nAssumedDim;
        }
    }

    if (retArr[0].StartsWith(_T("character(")))
    {
        int pos = retArr[0].Find(')',true);
        if (pos != wxNOT_FOUND)
            retArr[0] = retArr[0].Mid(0,pos) + _T(",len=")+fCharLen+_T(")");
    }
    else if (retArr[0].StartsWith(_T("character")))
    {
        retArr[0].Replace(_T("character"), _T("character(len=")+fCharLen+_T(")"));
    }

    if (retArr[0].StartsWith(_T("character")))
    {
        long numCharLen;
        wxString cDim;
        if (fCharLen.ToLong(&numCharLen))
        {
            fCharLen = wxString::Format(wxT("%i"),numCharLen+1);
            cDim = GetCDims(_T("(")+fCharLen+_T(")"));
        }
        else if (!fCharLen.IsSameAs(_T("*")))
        {
            fCharLen << _T("+1");
            cDim = _T("");
        }
        else
            cDim = _T("");
        retArr[1] << _T(", dimension") << _T("(") << fCharLen << _T(")");
        retArr[3] << cDim;
    }

    iPos = declarLw.Find(_T("intent("));
    if (iPos != wxNOT_FOUND)
    {
        wxString vinout = GetToken(declarLw,iPos+6);
        retArr[0] << _T(", intent") << vinout;
        retArr[1] << _T(", intent") << vinout;
    }
    return retArr;
}

wxString Bindto::GetFunctionDeclaration(TokenF* token)
{
    wxString funType;
    if (!token->m_PartFirst.IsEmpty())
    {
        wxString strLw = token->m_PartFirst.Lower().Trim(true).Trim(false);
        strLw.Replace(_T(" "),_T(""));
        wxArrayString ftarr;
        ftarr.Add(_T("integer("));
        ftarr.Add(_T("real("));
        ftarr.Add(_T("doubleprecision("));
        ftarr.Add(_T("complex("));
        for (size_t i=0; i<ftarr.size(); i++)
        {
            int iPos = strLw.Find(ftarr.Item(i));
            if (iPos != wxNOT_FOUND)
            {
                int tl = ftarr.Item(i).Length()-1;
                wxString vkind = GetToken(strLw,iPos+tl);
                vkind.Replace(_T("kind="),_T(""));
                funType << ftarr.Item(i).Mid(0,tl);
                funType << vkind;
                return funType;
            }
        }
        ftarr.Empty();
        ftarr.Add(_T("integer"));
        ftarr.Add(_T("real"));
        ftarr.Add(_T("doubleprecision"));
        ftarr.Add(_T("complex"));
        for (size_t i=0; i<ftarr.size(); i++)
        {
            int iPos = strLw.Find(ftarr.Item(i));
            if (iPos != wxNOT_FOUND)
            {
                funType << ftarr.Item(i);
                return funType;
            }
        }
    }

    for (int i=0; i<2; i++)
    {
        wxString resVName;
        if (i == 0)
        {
            if(!token->m_ResultVariable.IsEmpty())
                resVName = token->m_ResultVariable;
            else
                continue;
        }
        else
            resVName = token->m_Name;

        TokenF* argToken = m_pParser->FindTokenBetweenChildren(token, resVName);
        if (argToken)
        {
            if (argToken->m_TokenKind == tkVariable)
            {
                m_pTokenCurrent = argToken;
                int itmp;
                wxArrayString tya = GetBindType(argToken, itmp);
                funType = tya[0];
                if (!argToken->m_Args.IsEmpty())
                {
                    funType << _T(", dimension") << argToken->m_Args.Lower();
                }
                break;
            }
        }
        else
        {
            // assume implicit declaration
        }
    }
    return funType;
}


wxString Bindto::GetToken(const wxString& txt, int iPos)
{
    wxChar openChar;
    wxChar closeChar;
    if (txt.GetChar(iPos) == '(')
    {
        openChar = '(';
        closeChar = ')';
    }
    else
        return wxEmptyString;

    wxString retTxt;
    int level = 1;
    for (size_t i=iPos+1; i<txt.Length(); i++)
    {
        if (txt.GetChar(i) == openChar)
            level++;
        else if (txt.GetChar(i) == closeChar)
        {
            level--;
            if (level == 0)
            {
                retTxt << txt.Mid(iPos, i-iPos+1);
                break;
            }
        }
    }
    return retTxt;
}

wxString Bindto::GetCDims(wxString vdim)
{
    //input: (*), (10),    (10,5)  (5,*), (5,3,*), (m,n), (size(a,2),n), (:,:)
    //output: "", "[10]", "[5][10]", "",   "",        "",        "",      ""

    if (vdim.Find(_T("size(")) != wxNOT_FOUND)
        return _T("");

    wxArrayString dimArr;
    wxStringTokenizer tkz(vdim, _T("(), "), wxTOKEN_STRTOK );
    while ( tkz.HasMoreTokens() )
    {
        dimArr.Add(tkz.GetNextToken());
    }

    wxString cdims;
    for (int i=dimArr.GetCount()-1; i>=0; i--)
    {
        wxString ds = dimArr.Item(i);
        long dl;
        if (!ds.ToLong(&dl))
            return _T("");
        else
            cdims << _T("[") << ds << _T("]");
    }
    return cdims;
}



//output: "", "[10]", [][5], [][3][5], [n][m], [n][size(a,2)], [:][:]

//    wxArrayString sizArr;
//    int idx = vdim.Find(_T("size("));
//    while (idx != wxNOT_FOUND)
//    {
//        wxString siz = _T("size") + GetToken(vdim, idx+4);
//        vdim.Replace(siz,_T("s?"), false);
//        sizArr.Add(siz);
//
//        idx = vdim.Find(_T("size("));
//    }
//    wxArrayString dimArr;
//    wxStringTokenizer tkz(vdim, _T("(), "), wxTOKEN_STRTOK );
//    while ( tkz.HasMoreTokens() )
//    {
//        dimArr.Add(tkz.GetNextToken());
//    }
//
//    wxString cdims;
//    for (int i=dimArr.GetCount()-1; i>=0; i--)
//    {
//        wxString d1;
//        if (dimArr.Item(i).IsSameAs(_T("s?")) && sizArr.size()>0)
//        {
//            d1 = sizArr.Item(sizArr.size()-1);
//            sizArr.RemoveAt(sizArr.size()-1);
//        }
//        else if (!dimArr.Item(i).IsSameAs('*'))
//            d1 = dimArr.Item(i);
//        cdims << _T("[") << d1 << _T("]");
//    }
//    return cdims;
//}

void Bindto::OnAdd(wxCommandEvent& event)
{
    BindtoNewType addNewType(this);
    while (true)
    {
        if (addNewType.ShowModal() == wxID_OK)
        {
            wxString ft = addNewType.GetFortranType();
            wxString bt = addNewType.GetBindCType();
            wxString ct = addNewType.GetCType();
            ft.Replace(_T(" "),_T(""));
            if (ft.StartsWith(_T("character(")))
            {
                int idx = ft.Find(_T("kind="));
                if (idx != wxNOT_FOUND)
                {
                    wxString kn = ft.Mid(idx+5);
                    int idx1 = kn.Find(',');
                    int idx2 = kn.Find(')');
                    if (idx1 != wxNOT_FOUND && idx2 != wxNOT_FOUND)
                    {
                        if (idx1 > idx2)
                            kn = kn.Mid(0,idx2);
                        else
                            kn = kn.Mid(0,idx1);
                    }
                    else if (idx1 != wxNOT_FOUND)
                        kn = kn.Mid(0,idx1);
                    else if (idx2 != wxNOT_FOUND)
                        kn = kn.Mid(0,idx2);
                    ft = _T("character(kind=") + kn + _T(")");
                }
                else
                    ft = _T("character");
            }
            else if (ft.StartsWith(_T("character")))
                ft = _T("character");
            else if (ft.StartsWith(_T("integer(")) ||
                     ft.StartsWith(_T("real(")) ||
                     ft.StartsWith(_T("complex(")))
            {
                ft.Replace(_T("kind="),_T(""));
            }

            if (m_TypeMap.count(ft) == 0)
            {
                bt.Replace(_T(" "),_T(""));
                ct.Replace(_T(" "),_T(""));
                wxArrayString bcta;
                bcta.Add(bt);
                bcta.Add(ct);
                m_TypeMap[ft] = bcta;
                m_IsTypeMapDefault = false;
                FillTypeList();
                break;
            }
            else
            {
                wxString mstr = _T("Binding for \"") + ft + _T("\" already defined!");
                cbMessageBox(mstr, _("Error"), wxICON_ERROR);
            }
        }
        else
            break;
    }
}

void Bindto::OnEdit(wxCommandEvent& event)
{
    long sel = lv_Types->GetFirstSelected();
    if (sel == -1)
        return;
    wxString ft_old = lv_Types->GetItemText(sel);
    wxArrayString bcArr = m_TypeMap[ft_old];
    wxString bt_old = bcArr[0];
    wxString ct_old = bcArr[1];

    BindtoNewType editNewType(this);
    editNewType.SetEditType(ft_old,bt_old,ct_old);
    while (true)
    {
        if (editNewType.ShowModal() == wxID_OK)
        {
            wxString ft = editNewType.GetFortranType();
            wxString bt = editNewType.GetBindCType();
            wxString ct = editNewType.GetCType();
            ft.Replace(_T(" "),_T(""));
            bt.Replace(_T(" "),_T(""));
            ct.Replace(_T(" "),_T(""));
            if (ft.IsSameAs(ft_old) && bt.IsSameAs(bt_old) && ct.IsSameAs(ct_old))
                break;
            else
            {
                // type was changed
                m_TypeMap.erase(ft_old);
                wxArrayString bcta;
                bcta.Add(bt);
                bcta.Add(ct);
                m_TypeMap[ft] = bcta;
                m_IsTypeMapDefault = false;
                FillTypeList();
                break;
            }
        }
        else
            break;
    }
}

void Bindto::OnRemove(wxCommandEvent& event)
{
    long sel = lv_Types->GetFirstSelected();
    if (sel == -1)
        return;
    m_TypeMap.erase(lv_Types->GetItemText(sel));
    m_IsTypeMapDefault = false;
    FillTypeList();
}

void Bindto::OnDefaults(wxCommandEvent& event)
{
    FillTypeMapDefault();
    FillTypeList();
}

wxString Bindto::SplitLines(const wxString& txt, const wxString& lang)
{
    size_t llen;
    wxString csym;
    wxString comment;
    if (lang == _T("Fortran"))
    {
        llen = 132 - 2;
        csym = _T(" &");
        comment = _T("!");
    }
    else if (lang == _T("C"))
    {
        llen = 100;
        csym = _T("");
        comment = _T("//");
    }
    else
        return _T("Programming error. This should not happen.");

    wxArrayString txtArrShort;
    wxArrayString txtArr;
    wxStringTokenizer tkz(txt, _T("\n"), wxTOKEN_RET_EMPTY_ALL);
    while ( tkz.HasMoreTokens() )
        txtArr.Add(tkz.GetNextToken());

    for (size_t i=0; i<txtArr.size(); i++)
    {
        wxString codeStr;
        wxString comStr;
        int idxcom = txtArr[i].Find(comment);
        if (idxcom == wxNOT_FOUND)
            codeStr = txtArr[i].Trim();
        else
        {
            codeStr = txtArr[i].Mid(0,idxcom).Trim();
            comStr = txtArr[i].Mid(idxcom+comment.size());
        }
        wxString noSpaceCodeStr = codeStr;
        noSpaceCodeStr.Trim(false);
        size_t nSpace = codeStr.length() - noSpaceCodeStr.length();
        if (nSpace > llen/3)
            nSpace = llen/3;
        while (true)
        {
            bool isShortStr = true;
            if (codeStr.length() > llen)
            {
                isShortStr = false;
                wxString leftStr = codeStr.Mid(0,llen);
                wxString rightStr = codeStr.Mid(llen);
                int idx1 = leftStr.Find(',', true);
                int idx2 = leftStr.Find(' ', true);
                int idx3 = leftStr.Find('=', true);
                int idx = std::max(idx1,idx2);
                idx = std::max(idx,idx3);
                if (idx == wxNOT_FOUND)
                    isShortStr = true;
                else
                {
                    txtArrShort.Add(leftStr.Mid(0,idx+1) + csym);
                    wxString spaces;
                    codeStr = spaces.Append(' ',nSpace+4) + leftStr.Mid(idx+1) + rightStr;
                }
            }

            if (isShortStr)
            {
                wxString shortStr = codeStr;

                if (shortStr.find_first_not_of(_T(" \n")) != wxString::npos && !comStr.IsEmpty())
                    shortStr.Append(_T(" ") + comment + comStr);
                else if (!comStr.IsEmpty())
                    shortStr.Append(comment + comStr);
                txtArrShort.Add(shortStr);
                break;
            }
        }
    }
    wxString txtAll;
    for (size_t i=0; i<txtArrShort.size(); i++)
    {
        txtAll.Append(txtArrShort[i] + _T("\n"));
    }
    return txtAll;
}

void Bindto::GetSubStrFtoC(wxArrayString &strFtoC)
{
    wxString tab;
    tab.Append(' ',m_TabSize);
    strFtoC.Add(_T("subroutine string_copy_f_c(f_string, c_string)"));
    strFtoC.Add(tab + _T("character(len=*), intent(in) :: f_string"));
    strFtoC.Add(tab + _T("character(len=1,kind=c_char), dimension(*), intent(out) :: c_string(*)"));
    strFtoC.Add(tab + _T("integer :: i, chlen\n"));
    strFtoC.Add(tab + _T("chlen = len(f_string)"));
    strFtoC.Add(tab + _T("do i = 1, chlen"));
    strFtoC.Add(tab + tab + _T("c_string(i) = f_string(i:i)"));
    strFtoC.Add(tab + _T("end do"));
    strFtoC.Add(tab + _T("c_string(chlen+1) = c_null_char"));
    strFtoC.Add(_T("end subroutine"));
}

void Bindto::GetSubStrCtoF(wxArrayString &strCtoF)
{
    wxString tab;
    tab.Append(' ',m_TabSize);
    strCtoF.Add(_T("subroutine string_copy_c_f(c_string, f_string)"));
    strCtoF.Add(tab + _T("character(len=1,kind=c_char), dimension(*), intent(in) :: c_string"));
    strCtoF.Add(tab + _T("character(len=*), intent(out) :: f_string"));
    strCtoF.Add(tab + _T("integer :: i, chlen\n"));
    strCtoF.Add(tab + _T("i = 1"));
    strCtoF.Add(tab + _T("chlen = len(f_string)"));
    strCtoF.Add(tab + _T("do while(c_string(i)/=c_null_char .and. i<=chlen)"));
    strCtoF.Add(tab + tab + _T("f_string(i:i) = c_string(i)"));
    strCtoF.Add(tab + tab + _T("i = i + 1"));
    strCtoF.Add(tab + _T("end do"));
    strCtoF.Add(tab + _T("if (i<=chlen) f_string(i:) = ' '"));
    strCtoF.Add(_T("end subroutine"));
}

void Bindto::GetFunStrLen(wxArrayString &strLen)
{
    wxString tab;
    tab.Append(' ',m_TabSize);
    strLen.Add(_T("function string_len(cstr)"));
    strLen.Add(tab + _T("integer :: string_len"));
    strLen.Add(tab + _T("character(kind=c_char,len=1), dimension(*) :: cstr\n"));
    strLen.Add(tab + _T("string_len = 1"));
    strLen.Add(tab + _T("do while(cstr(string_len) /= c_null_char)"));
    strLen.Add(tab + tab + _T("string_len = string_len + 1"));
    strLen.Add(tab + _T("end do"));
    strLen.Add(tab + _T("string_len = string_len - 1"));
    strLen.Add(_T("end function"));
}

wxString Bindto::GetHelperModule()
{
    wxString help;
    if (!m_WriteStrCtoF && !m_WriteStrFtoC && !m_WriteStrLen)
        return wxEmptyString;

    wxString tab;
    tab.Append(' ',m_TabSize);
    help << _T("module bindc_helper_bc\n");
    help << tab << _T("use, intrinsic :: iso_c_binding\n");
    help << tab << _T("implicit none\n");
    help << _T("contains\n");
    if (m_WriteStrLen)
    {
        help << _T("\n");
        wxArrayString strLen;
        GetFunStrLen(strLen);
        for (size_t i=0;i<strLen.size();i++)
            help << tab << strLen.Item(i) << _T("\n");
    }
    if (m_WriteStrCtoF)
    {
        help << _T("\n");
        wxArrayString strCtoF;
        GetSubStrCtoF(strCtoF);
        for (size_t i=0;i<strCtoF.size();i++)
            help << tab << strCtoF.Item(i) << _T("\n");
    }
    if (m_WriteStrFtoC)
    {
        help << _T("\n");
        wxArrayString strFtoC;
        GetSubStrFtoC(strFtoC);
        for (size_t i=0;i<strFtoC.size();i++)
            help << tab << strFtoC.Item(i) << _T("\n");
    }
    help << _T("end module\n");
    return help;
}

void Bindto::PrepareAssumedShapeVariables(wxString& txtBindSecond, wxArrayString& argArr, wxArrayString& dimVarNames,
                                          wxArrayString& additionalDeclar, wxArrayString& addVarNames, wxArrayString& addVarNamesC)
{
    for (size_t i=0; i<dimVarNames.size(); i++)
    {
        wxString var = dimVarNames.Item(i);
        txtBindSecond.Replace(DIM_VAR_KEY, var, false);
        if (argArr.Index(var) == wxNOT_FOUND && addVarNames.Index(var) == wxNOT_FOUND)
        {
            additionalDeclar.Add(_T("integer(c_int), intent(in) :: ") + var);
            addVarNames.Add(var);
            addVarNamesC.Add(_T("int* ") + var);
        }
    }
}

void Bindto::AddDimVariables(wxArrayString& argArr, wxArrayString& dimVarNames, int nDimVarAdd, wxString varFirstPart)
{
    wxString n1 = varFirstPart + _T("%i");
    wxString vname;
    int i=0;
    for (int nd=0; nd<nDimVarAdd; nd++)
    {
        while (vname.IsEmpty())
        {
            i++;
            wxString vn1 = wxString::Format(n1,i);
            if (argArr.Index(vn1) == wxNOT_FOUND && dimVarNames.Index(vn1) == wxNOT_FOUND)
                vname = vn1;
        }
        dimVarNames.Add(vname);
        vname = wxEmptyString;
    }
}

void Bindto::HideAssumedShape(const wxString& vdim, wxString& vdimHid, int& nAssumedDim)
{
    vdimHid = vdim;
    nAssumedDim = vdimHid.Replace(_T(":"), DIM_VAR_KEY);
}

void Bindto::AddDimVariablesFromDoc(wxArrayString& dimVarNames, int& nDimVarAdd, const wxString& docString)
{
    // Get dimensions of allocatable array from doc string e.g. dimension(m,n)
    wxString dimsStr;
    wxString lwDocString = docString.Lower().Trim(true);
    wxRegEx reDim1(_T("^dimension[ \\t]*\\(([[:alnum:]_, ]+)"));
    wxRegEx reDim2(_T("[ \\t!><]+dimension[ \\t]*\\(([[:alnum:]_, ]+)"));
    if (reDim1.Matches(lwDocString))
    {
        dimsStr = reDim1.GetMatch(lwDocString,1);
    }
    if (dimsStr.IsEmpty() && reDim2.Matches(lwDocString))
    {
        dimsStr = reDim2.GetMatch(lwDocString,1);
    }
    if (dimsStr.IsEmpty())
        return;

    wxArrayString dimArr;
    wxStringTokenizer tkz(dimsStr, _T("(), "), wxTOKEN_STRTOK );
    while ( tkz.HasMoreTokens() )
    {
        dimArr.Add(tkz.GetNextToken());
    }
    if (int(dimArr.size()) != nDimVarAdd)
        return; // wrong number of variables

    for (size_t i=0; i<dimArr.size(); i++)
        dimVarNames.Add(dimArr.Item(i));
    nDimVarAdd = 0;
}

