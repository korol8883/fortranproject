#ifndef USETOKENF_H
#define USETOKENF_H

#include "tokenf.h"
#include <wx/arrstr.h>
#include <list>

enum ModuleNature
{
    mnIntrinsic = 1,
    mnNonIntrinsic
};

class UseTokenF : public TokenF
{
    public:
        UseTokenF();
        UseTokenF(const wxString& name, const wxString& filename, unsigned int line);
        virtual ~UseTokenF();
        void SetOnly(bool hasOnly);
        void SetModuleNature(ModuleNature modNature);
        void AddToNamesList(wxString& localName, wxString& externalName);

        ModuleNature GetModuleNature() {return m_ModuleNature;};
        bool HasOnly() {return m_HasOnly;};
        std::list<wxArrayString>* GetNamesList() {return &m_NamesList;};
    //protected:
    private:
        ModuleNature m_ModuleNature;
        bool m_HasOnly;
        std::list<wxArrayString> m_NamesList;
};

#endif // USETOKENF_H
