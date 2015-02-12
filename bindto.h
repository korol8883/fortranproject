#ifndef BINDTO_H
#define BINDTO_H

//(*Headers(Bindto)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "parserf.h"

enum BindToIn
{
    bindToProject,
    bindToFile,
};

typedef std::map<wxString,wxArrayString> TypeMap;
typedef std::set<wxString> StrSet;


class Bindto: public wxDialog
{
	public:

		Bindto(wxWindow* parent, ParserF* pParser);
		virtual ~Bindto();

		//(*Declarations(Bindto)
		wxButton* bt_Defaults;
		wxButton* bt_Edit;
		wxRadioButton* rb_ActiveProject;
		wxPanel* Panel1;
		wxCheckBox* cb_dtorStart;
		wxTextCtrl* tc_dtorStart;
		wxCheckBox* cb_ctorEnd;
		wxButton* bt_Add;
		wxStaticText* st_globalFilename;
		wxPanel* Panel2;
		wxPanel* Panel4;
		wxListView* lv_Types;
		wxCheckBox* cb_ctorStart;
		wxTextCtrl* tc_ctorStart;
		wxCheckBox* cb_ctorDef;
		wxButton* bt_Remove;
		wxTextCtrl* tc_dtorEnd;
		wxCheckBox* cb_dtorEnd;
		wxTextCtrl* tc_globalFilename;
		wxTextCtrl* tc_bindCName;
		wxTextCtrl* tc_ctorEnd;
		wxCheckBox* cb_globalToOne;
		wxRadioButton* rb_CurrentFile;
		//*)

	protected:

		//(*Identifiers(Bindto)
		static const long ID_BTOACTIVEPROJECT;
		static const long ID_BTOCURRENTFILE;
		static const long ID_TEXTCTRL1;
		static const long ID_CHECKBOX3;
		static const long ID_TEXTCTRL6;
		static const long ID_PANEL2;
		static const long ID_LV_TYPES;
		static const long ID_BUTTON_ADD;
		static const long ID_BUTTON_EDIT;
		static const long ID_BUTTON_REMOVE;
		static const long ID_BUTTON_DEFAULTS;
		static const long ID_PANEL1;
		static const long ID_CHECKBOX4;
		static const long ID_TEXTCTRL4;
		static const long ID_CHECKBOX5;
		static const long ID_TEXTCTRL5;
		static const long ID_CHECKBOX6;
		static const long ID_PANEL3;
		static const long ID_CHECKBOX1;
		static const long ID_TEXTCTRL2;
		static const long ID_CHECKBOX2;
		static const long ID_TEXTCTRL3;
		static const long ID_PANEL4;
		static const long ID_NOTEBOOK1;
		//*)

	private:

		//(*Handlers(Bindto)
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnRemove(wxCommandEvent& event);
		void OnDefaults(wxCommandEvent& event);
		void OnClick_cbCtorStart(wxCommandEvent& event);
		void OnClick_cbCtorEnd(wxCommandEvent& event);
		void OnClick_cbDtorStart(wxCommandEvent& event);
		void OnClick_cbDtorEnd(wxCommandEvent& event);
		void Onrb_ActiveProjectSelect(wxCommandEvent& event);
		//*)

		enum Language
		{
		    Fortran,
		    C,
		};

		ParserF* m_pParser;
        TokenF*  m_pTokenCurrent;
        int m_Indent;
        int m_TabSize;
        TypeMap m_TypeMap;
        bool m_IsTypeMapDefault;
        bool m_OneGProcFile;
        wxString m_OneGProcFileName;
        wxString m_BindCName;
        wxString m_CtorStartsWith;
        wxString m_CtorEndsWith;
        bool m_AutoCreateCtor;
        wxString m_DtorStartsWith;
        wxString m_DtorEndsWith;

        bool m_UseOneGlobalFile;
        wxString m_GlobProceduresFile;
        wxString m_GlobProceduresFileH;
        wxString m_GlobProcWarnMessages;
        StrSet m_GlobProceduresCInclude;
        bool m_GlobWriteStrFtoC;
        bool m_GlobWriteStrCtoF;
        bool m_GlobWriteStrLen;
        bool m_GlobWriteIntToLog;
        bool m_GlobWriteLogToInt;
        wxString m_WarnMessage;
        StrSet m_NotFoundTypes;
        wxString m_CStructs;
        wxArrayString m_CreatedMsg;
        StrSet m_CInclude;
        bool m_WriteStrFtoC;
        bool m_WriteStrCtoF;
        bool m_WriteStrLen;
        bool m_WriteIntToLog;
        bool m_WriteLogToInt;
        StrSet m_DefinedTypes;
        StrSet m_AllocatedTypes;
        StrSet m_DeallocatedTypes;
        wxString m_CurProcedure;
        wxString m_CurModule;
        wxString m_CurFile;

        void FillTypeList();
        void LoadInitialValues();
        void FillTypeMapDefault();
        void LoadBindToConfig();
        void SaveBindToConfig();
        void OnOK(wxCommandEvent& event);
        void MakeBindTo(BindToIn btin);
        void FileBindTo(const wxString& filename);
        wxString GetIS();
        wxString CreateBindFilename(const wxString& filename, bool header);
        void BindProcedure(wxString& txtBind, wxString& txtHeaders, TokenF* token, const wxString& moduleName, bool isGlobal, wxString callName=wxEmptyString);
        wxArrayString GetBindType(TokenF* token, int& nDimVarAdd);
        wxArrayString GetBindType(const wxString& declar, int& nDimVarAdd);
        wxString GetToken(const wxString& txt, int iPos);
        wxString GetFunctionDeclaration(TokenF* token);
        wxString GetCDims(wxString vdim);
        wxString SplitLines(const wxString& txt, Language lang);
        void GetSubStrFtoC(wxArrayString& strFtoC);
        void GetSubStrCtoF(wxArrayString& strCtoF);
        void GetFunStrLen(wxArrayString& strLen);
        void GetFunIntToLog(wxArrayString& strArr);
        void GetFunLogToInt(wxArrayString& strArr);
        wxString GetHelperModule(bool useGlobal = false);
        void PrepareAssumedShapeVariables(wxString& txtBindSecond, wxArrayString& argArr, wxArrayString& dimVarNames,
                                          wxArrayString& additionalDeclar, wxArrayString& addVarNames, wxArrayString& addVarNamesC);
        void AddDimVariables(wxArrayString& argArr, wxArrayString& dimVarNames, int nDimVarAdd, wxString varFirstPart);
        void HideAssumedShape(const wxString& vdim, wxString& vdimHid, int& nAssumedDim);
        void AddDimVariablesFromDoc(wxArrayString& dimVarNames, int& nDimVarAdd, const wxString& docString);
        wxString GetCName(const wxString& procName, const wxString& moduleName);
        void AddDestructors(wxString& txtBind, wxString& txtHeadersMod, const wxString& moduleName);
        void AddConstructors(wxString& txtBind, wxString& txtHeadersMod, const wxString& moduleName);
        bool IsConstructor(TokenF* token);
        bool IsDestructor(TokenF* token);

		DECLARE_EVENT_TABLE()
};

#endif
