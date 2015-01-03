#ifndef BINDTO_H
#define BINDTO_H

//(*Headers(Bindto)
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
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
		wxButton* bt_Add;
		wxListView* lv_Types;
		wxButton* bt_Remove;
		wxRadioButton* rb_CurrentFile;
		//*)

	protected:

		//(*Identifiers(Bindto)
		static const long ID_BTOACTIVEPROJECT;
		static const long ID_BTOCURRENTFILE;
		static const long ID_LV_TYPES;
		static const long ID_BUTTON_ADD;
		static const long ID_BUTTON_EDIT;
		static const long ID_BUTTON_REMOVE;
		static const long ID_BUTTON_DEFAULTS;
		//*)

	private:

		//(*Handlers(Bindto)
		void OnAdd(wxCommandEvent& event);
		void OnEdit(wxCommandEvent& event);
		void OnRemove(wxCommandEvent& event);
		void OnDefaults(wxCommandEvent& event);
		//*)

		ParserF* m_pParser;
        TokenF*  m_pTokenCurrent;
        int m_Indent;
        int m_TabSize;
        TypeMap m_TypeMap;
        bool m_IsTypeMapDefault;
        wxString m_WarnMessage;
        StrSet m_NotFoundTypes;
        wxString m_CStructs;
        wxArrayString m_CreatedMsg;
        StrSet m_CInclude;
        bool m_WriteStrFtoC;
        bool m_WriteStrCtoF;
        bool m_WriteStrLen;

        void FillTypeList();
        void FillTypeMap();
        void FillTypeMapDefault();
        void LoadBindToConfig();
        void SaveBindToConfig();
        void OnOK(wxCommandEvent& event);
        void MakeBindTo(BindToIn btin);
        void FileBindTo(const wxString& filename);
        wxString GetIS();
        wxString CreateBindFilename(const wxString& filename, bool header);
        void BindProcedure(wxString& txtBind, wxString& txtHeaders, TokenF* token, bool isGlobal, wxString callName=wxEmptyString);
        wxArrayString GetBindType(TokenF* token, int& nDimVarAdd);
        wxArrayString GetBindType(const wxString& declar, int& nDimVarAdd);
        wxString GetToken(const wxString& txt, int iPos);
        wxString GetFunctionDeclaration(TokenF* token);
        wxString GetCDims(wxString vdim);
        wxString SplitLines(const wxString& txt, const wxString& lang);
        void GetSubStrFtoC(wxArrayString& strFtoC);
        void GetSubStrCtoF(wxArrayString& strCtoF);
        void GetFunStrLen(wxArrayString& strLen);
        wxString GetHelperModule();
        void PrepareAssumedShapeVariables(wxString& txtBindSecond, wxArrayString& argArr, wxArrayString& dimVarNames,
                                          wxArrayString& additionalDeclar, wxArrayString& addVarNames, wxArrayString& addVarNamesC);
        void AddDimVariables(wxArrayString& argArr, wxArrayString& dimVarNames, int nDimVarAdd, wxString varFirstPart);
        void HideAssumedShape(const wxString& vdim, wxString& vdimHid, int& nAssumedDim);
        void AddDimVariablesFromDoc(wxArrayString& dimVarNames, int& nDimVarAdd, const wxString& docString);

		DECLARE_EVENT_TABLE()
};

#endif
