/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include <sdk.h>
#include "fpoptionsdlg.h"
#include "fortranproject.h"
#include <wx/intl.h>
#include <wx/listbox.h>
#include <wx/xrc/xmlres.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/treectrl.h>
#include <wx/slider.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/regex.h>
#include <configmanager.h>
#include <manager.h>
#include <globals.h>


BEGIN_EVENT_TABLE(FPOptionsDlg, wxPanel)
    EVT_UPDATE_UI(-1, FPOptionsDlg::OnUpdateUI)
    EVT_COMMAND_SCROLL(XRCID("sliderDelay"), FPOptionsDlg::OnSliderScroll)
END_EVENT_TABLE()

FPOptionsDlg::FPOptionsDlg(wxWindow* parent, NativeParserF* np, FortranProject* fp)
    : m_pNativeParser(np),
    m_pFortranProject(fp)
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));

    wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgFPSettings"));

    XRCCTRL(*this, "chkNoCC", wxCheckBox)->SetValue(!cfg->ReadBool(_T("/use_code_completion"), true));
    XRCCTRL(*this, "chkEvalTooltip", wxCheckBox)->SetValue(cfg->ReadBool(_T("/eval_tooltip"), true));
    XRCCTRL(*this, "chkAutoSelectOne", wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_select_one"), false));
    XRCCTRL(*this, "chkSmartCodeCompletion", wxCheckBox)->SetValue(cfg->ReadBool(_T("/use_smart_code_completion"), true));
    XRCCTRL(*this, "chkOnlyUseAssociated", wxCheckBox)->SetValue(cfg->ReadBool(_T("/only_use_associated"), true));
    XRCCTRL(*this, "chkShowHiddenEntities", wxCheckBox)->SetValue(cfg->ReadBool(_T("/show_hidden_entities"), false));
    XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->SetValue(cfg->ReadBool(_T("/auto_launch"), true));
    XRCCTRL(*this, "spnAutoLaunchChars", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/auto_launch_chars"), 2));
    XRCCTRL(*this, "spnMaxMatches", wxSpinCtrl)->SetValue(cfg->ReadInt(_T("/max_matches"), 16384));

    XRCCTRL(*this, "chkNoSB", wxCheckBox)->SetValue(!cfg->ReadBool(_T("/use_symbols_browser"), true));
    XRCCTRL(*this, "chkFloatSB", wxCheckBox)->SetValue(cfg->ReadBool(_T("/as_floating_window"), false));
    XRCCTRL(*this, "chkBottomTree", wxCheckBox)->SetValue(cfg->ReadBool(_T("/visible_bottom_tree"), true));
    XRCCTRL(*this, "chkSortSB", wxCheckBox)->SetValue(cfg->ReadBool(_T("/browser_sort_alphabetically"), true));

    XRCCTRL(*this, "chkKL_1", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set1"), true));
    XRCCTRL(*this, "chkKL_2", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set2"), true));
    XRCCTRL(*this, "chkKL_3", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set3"), false));
    XRCCTRL(*this, "chkKL_4", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set4"), false));
    XRCCTRL(*this, "chkKL_5", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set5"), false));
    XRCCTRL(*this, "chkKL_6", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set6"), false));
    XRCCTRL(*this, "chkKL_7", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set7"), false));
    XRCCTRL(*this, "chkKL_8", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set8"), false));
    XRCCTRL(*this, "chkKL_9", wxCheckBox)->SetValue(cfg->ReadBool(_T("/lexer_keywords_set9"), false));

    XRCCTRL(*this, "txtFExtFixed", wxTextCtrl)->ChangeValue(cfg-> Read(_T("/extension_fixed"), _T("for, f77, f, fpp")));
    XRCCTRL(*this, "txtFExtFree", wxTextCtrl)->ChangeValue(cfg->Read(_T("/extension_free"), _T("f90, f95, f2k, f03, f08")));

    XRCCTRL(*this, "rbCase", wxRadioBox)->SetSelection(cfg->ReadInt(_T("/keywords_case"), 0));

    int timerDelay = cfg->ReadInt(_T("/cc_delay"), 500);
    XRCCTRL(*this, "sliderDelay", wxSlider)->SetValue(timerDelay / 100);
    UpdateSliderLabel();

    XRCCTRL(*this, "chkComRight", wxCheckBox)->SetValue(cfg->ReadBool(_T("/include_comments_variable"), true));

    XRCCTRL(*this, "chkNoFortranInfo", wxCheckBox)->SetValue(!cfg->ReadBool(_T("/use_log_window"), true));
    XRCCTRL(*this, "chkComAbove", wxCheckBox)->SetValue(cfg->ReadBool(_T("/include_comments_above"), true));
    XRCCTRL(*this, "chkComBelow", wxCheckBox)->SetValue(cfg->ReadBool(_T("/include_comments_below"), true));
    XRCCTRL(*this, "chkDeclarLog", wxCheckBox)->SetValue(cfg->ReadBool(_T("/include_declarations_log"), true));
    XRCCTRL(*this, "chkLogComRight", wxCheckBox)->SetValue(cfg->ReadBool(_T("/include_log_comments_variable"), true));
}

FPOptionsDlg::~FPOptionsDlg()
{
}

void FPOptionsDlg::UpdateSliderLabel()
{
    int position = XRCCTRL(*this, "sliderDelay", wxSlider)->GetValue();
    wxString lbl;
    if (position >= 10)
        lbl.Printf(_("%d.%d sec"), position / 10, position % 10);
    else
        lbl.Printf(_("%d ms"), position * 100);
    XRCCTRL(*this, "lblDelay", wxStaticText)->SetLabel(lbl);
}

void FPOptionsDlg::OnSliderScroll(wxScrollEvent& event)
{
    UpdateSliderLabel();
}

void FPOptionsDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    bool en = !XRCCTRL(*this, "chkNoCC", wxCheckBox)->GetValue();
    bool auto_launch = XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->GetValue();

    XRCCTRL(*this, "chkEvalTooltip", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoSelectOne", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkSmartCodeCompletion", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkOnlyUseAssociated", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkShowHiddenEntities", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "spnAutoLaunchChars", wxSpinCtrl)->Enable(en && auto_launch);
    XRCCTRL(*this, "spnMaxMatches", wxSpinCtrl)->Enable(en);
    XRCCTRL(*this, "sliderDelay", wxSlider)->Enable(en);
    XRCCTRL(*this, "txtFExtFixed", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "txtFExtFree", wxTextCtrl)->Enable(en);
    XRCCTRL(*this, "rbCase", wxRadioBox)->Enable(en);
    XRCCTRL(*this, "chkKL_1", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_2", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_3", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_4", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_5", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_6", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_7", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_8", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkKL_9", wxCheckBox)->Enable(en);
    bool enA = en;


    en = !XRCCTRL(*this, "chkNoSB", wxCheckBox)->GetValue();
    XRCCTRL(*this, "chkFloatSB", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkBottomTree", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkSortSB", wxCheckBox)->Enable(en);

    en = !XRCCTRL(*this, "chkNoFortranInfo", wxCheckBox)->GetValue();
    XRCCTRL(*this, "chkComAbove", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkComBelow", wxCheckBox)->Enable(en);
    XRCCTRL(*this, "chkDeclarLog", wxCheckBox)->Enable(en);

    if (enA)
    {
        bool enUA = XRCCTRL(*this, "chkOnlyUseAssociated", wxCheckBox)->GetValue();
        XRCCTRL(*this, "chkShowHiddenEntities", wxCheckBox)->Enable(enUA);
    }
}

void FPOptionsDlg::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("fortran_project"));

    // force parser to read its options that we write in the config
    cfg->Write(_T("/use_code_completion"), (bool)!XRCCTRL(*this, "chkNoCC", wxCheckBox)->GetValue());
    int timerDelay = XRCCTRL(*this, "sliderDelay", wxSlider)->GetValue() * 100;
    cfg->Write(_T("/cc_delay"), (int)timerDelay);

    // set all other member options
    cfg->Write(_T("/auto_select_one"), (bool)XRCCTRL(*this, "chkAutoSelectOne", wxCheckBox)->GetValue());
    cfg->Write(_T("/use_smart_code_completion"), (bool)XRCCTRL(*this, "chkSmartCodeCompletion", wxCheckBox)->GetValue());
    cfg->Write(_T("/only_use_associated"), (bool)XRCCTRL(*this, "chkOnlyUseAssociated", wxCheckBox)->GetValue());
    cfg->Write(_T("/show_hidden_entities"), (bool)XRCCTRL(*this, "chkShowHiddenEntities", wxCheckBox)->GetValue());
    cfg->Write(_T("/auto_launch"), (bool)XRCCTRL(*this, "chkAutoLaunch", wxCheckBox)->GetValue());
    cfg->Write(_T("/auto_launch_chars"), (int)XRCCTRL(*this, "spnAutoLaunchChars", wxSpinCtrl)->GetValue());
    cfg->Write(_T("/max_matches"), (int)XRCCTRL(*this, "spnMaxMatches", wxSpinCtrl)->GetValue());
    cfg->Write(_T("/eval_tooltip"), (bool)XRCCTRL(*this, "chkEvalTooltip", wxCheckBox)->GetValue());

    cfg->Write(_T("/use_symbols_browser"), (bool)!XRCCTRL(*this, "chkNoSB", wxCheckBox)->GetValue());
    cfg->Write(_T("/as_floating_window"), (bool)XRCCTRL(*this, "chkFloatSB", wxCheckBox)->GetValue());
    cfg->Write(_T("/visible_bottom_tree"), (bool)XRCCTRL(*this, "chkBottomTree", wxCheckBox)->GetValue());
    cfg->Write(_T("/browser_sort_alphabetically"), (bool)XRCCTRL(*this, "chkSortSB", wxCheckBox)->GetValue());

    cfg->Write(_T("/lexer_keywords_set1"), (bool)XRCCTRL(*this, "chkKL_1", wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set2"), (bool)XRCCTRL(*this, "chkKL_2", wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set3"), (bool)XRCCTRL(*this, "chkKL_3", wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set4"), (bool)XRCCTRL(*this, "chkKL_4", wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set5"), (bool)XRCCTRL(*this, "chkKL_5", wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set6"), (bool)XRCCTRL(*this, "chkKL_6", wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set7"), (bool)XRCCTRL(*this, "chkKL_7", wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set8"), (bool)XRCCTRL(*this, "chkKL_8", wxCheckBox)->GetValue());
    cfg->Write(_T("/lexer_keywords_set9"), (bool)XRCCTRL(*this, "chkKL_9", wxCheckBox)->GetValue());

    wxString ext_fixed = XRCCTRL(*this, "txtFExtFixed", wxTextCtrl)->GetValue().Trim();
    if (ext_fixed.IsEmpty())
        ext_fixed = _T("for, f77, f, fpp");

    wxString ext_free = XRCCTRL(*this, "txtFExtFree", wxTextCtrl)->GetValue().Trim();
    if (ext_free.IsEmpty())
        ext_free = _T("f90, f95, f2k, f03, f08");

    cfg->Write(_T("/extension_fixed"), ext_fixed);
    cfg->Write(_T("/extension_free"), ext_free);

    cfg->Write(_T("/keywords_case"), (int)XRCCTRL(*this, "rbCase", wxRadioBox)->GetSelection());

    cfg->Write(_T("/include_comments_variable"), (bool)XRCCTRL(*this, "chkComRight", wxCheckBox)->GetValue());

    cfg->Write(_T("/use_log_window"), (bool)!XRCCTRL(*this, "chkNoFortranInfo", wxCheckBox)->GetValue());
    cfg->Write(_T("/include_comments_above"), (bool)XRCCTRL(*this, "chkComAbove", wxCheckBox)->GetValue());
    cfg->Write(_T("/include_comments_below"), (bool)XRCCTRL(*this, "chkComBelow", wxCheckBox)->GetValue());
    cfg->Write(_T("/include_declarations_log"), (bool)XRCCTRL(*this, "chkDeclarLog", wxCheckBox)->GetValue());
    cfg->Write(_T("/include_log_comments_variable"), (bool)XRCCTRL(*this, "chkLogComRight", wxCheckBox)->GetValue());

    m_pNativeParser->RereadOptions();
    m_pFortranProject->RereadOptions();
}
