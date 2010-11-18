/*
 * This file is part of the FortranProject plugin for Code::Blocks IDE
 * and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PARSERTHREADF_H
#define PARSERTHREADF_H

#include "tokenf.h"
#include "tokenizerf.h"
#include <set>


class ParserThreadF
{
    public:
        ParserThreadF(const wxString& bufferOrFilename,
							 TokensArrayF* tokens,
							 FortranSourceForm fsForm,
							 bool isBuffer=false);
        virtual ~ParserThreadF();
        bool Parse();
        void ParseDeclarations(bool breakAtEnd=false, bool breakAtContains=false);
    protected:
    private:
        TokenF* DoAddToken(TokenKindF kind, const wxString& name, const wxString& args=wxEmptyString, const wxString& typeDefinition=wxEmptyString);
        TokenF* DoAddToken(TokenKindF kind, const wxString& name, const wxString& args, const unsigned int defStartLine);

		Tokenizerf m_Tokens;
		TokensArrayF* m_pTokens;
		TokenF* m_pLastParent;
		wxString m_Filename;

		wxString m_LastTokenName;

		unsigned int m_InterfaceOperator;
		unsigned int m_InterfaceAssignment;
		unsigned int m_InterfaceRead;
		unsigned int m_InterfaceWrite;

        void InitSecondEndPart();
		void HandleModProg(TokenKindF);
		void HandleFunction(TokenKindF);
		void HandleType();
		void HandleUse();
		void HandleInterface();
		void HandleBlockData();
		void HandleInclude();
		void HandleProcedureList();
		void GoThroughBody();
		bool IsEnd(wxString tok_low, wxString nex_low);
		bool ParseDeclarationsFirstPart(wxString& token, wxString& next);
		void ParseDeclarationsSecondPart(wxString& token);
		void CheckParseOneDeclaration(wxString& token, wxString& tok_low, wxString& next, wxString& next_low);
		void ParseTypeBoundProcedures();

        std::set<wxString> m_EndSecPart;
};

#endif // PARSERTHREADF_H
