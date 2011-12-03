/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TOKENF_H
#define TOKENF_H

#include <wx/string.h>
#include <wx/dynarray.h>
#include <wx/thread.h>
#include <wx/arrstr.h>

#include <list>

WX_DEFINE_ARRAY_SIZE_T(size_t, ArrOfSizeT);

static wxCriticalSection s_CritSect;

class TokenF;
WX_DEFINE_ARRAY(TokenF*, TokensArrayF);

class TokenFlat;
WX_DEFINE_ARRAY(TokenFlat*, TokensArrayFlat);


enum TokenKindF
{
    tkUse = 0x0001,
    tkModule = 0x0002,
    tkSubroutine = 0x0004,
    tkFunction = 0x0008,
    tkProgram = 0x00010,
    tkBlockData = 0x00020,
    tkType = 0x00040,
    tkInclude  = 0x00080,
    tkCommonblock = 0x00100,
    tkPreprocessor = 0x00200,
    tkFile = 0x00400,
    tkVariable = 0x00800,
    tkOther = 0x01000,
    tkInterfaceSubroutine = 0x02000,
    tkInterfaceFunction = 0x04000,
    tkInterface = 0x08000,
    tkInterfaceExplicit = 0x10000,
    tkProcedure = 0x20000,
    tkAccessList = 0x40000,
};

enum TokenAccessKind
{
    taPublic = 1,
    taPrivate,
    taProtected
};


class TokenF
{
	public:
		TokenF();
		TokenF(const wxString& name, const wxString& filename, unsigned int line);
		virtual ~TokenF();

		void Clear();
		void AddChild(TokenF* child);
		wxString GetTokenKindString();
		void AddLineEnd(int end);

		void AddPartFirst(wxString& str){ m_PartFirst = str;};
		void AddPartLast(wxString& str){ m_PartLast = str;};
		void AddResultVariable(wxString& str){ m_ResultVariable = str;};

		wxString m_Name;
		wxString m_DisplayName;
		wxString m_Args;
		wxString m_Filename;
		wxString m_TypeDefinition;
		unsigned int m_LineStart;
		unsigned int m_LineEnd;
		unsigned int m_DefinitionLength;
		TokenKindF m_TokenKind;
		TokenAccessKind m_TokenAccess;

		//For function only
		wxString m_ResultVariable;
		wxString m_PartFirst;
		wxString m_PartLast;

		//For tkType only
		wxString m_ExtendsType;

		//For tkProcedure
		bool m_Pass;

		TokenF* m_pParent;
		TokensArrayF m_Children;

	protected:
	private:
};

class TokensArrayClass
{
    public:
        TokensArrayClass();
        ~TokensArrayClass();
        TokensArrayF* GetTokens(){return &m_Tokens;};
    protected:
    private:
        TokensArrayF m_Tokens;
};

class TokenFlat : public TokenF
{
    public:
        TokenFlat(const TokenF* tok);
        TokenFlat(const TokenFlat* tok);
		~TokenFlat();
		void Rename(const wxString& newName);
		void ChangeDisplayName(const wxString& newName);

		wxString m_ParentName;
		wxString m_ParentDisplayName;
		TokenKindF m_ParentTokenKind;

        wxString m_Rename; // rename through use association
};

class TokensArrayFlatClass
{
    public:
        TokensArrayFlatClass();
        ~TokensArrayFlatClass();
        TokensArrayFlat* GetTokens(){return &m_Tokens;};
        TokensArrayFlat m_Tokens;
        bool HasTokensWithName(const wxString&, ArrOfSizeT&);
        void DelTokensWithName(const wxString&);
    protected:
    private:
};


#endif // TOKENF_H