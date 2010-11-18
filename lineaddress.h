/*
 * This file is licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Author: Darius Markauskas
 *
 */

#ifndef LINEADDRESS_H_INCLUDED
#define LINEADDRESS_H_INCLUDED

#include <wx/string.h>
#include <list>


class LineAddress;
typedef std::list<LineAddress> JumpAddressList;

class LineAddress
{
    public:
        wxString m_Filename;
        int m_LineNumber;
        bool IsSameAs(LineAddress &other)
        {
            if (m_Filename.IsSameAs(other.m_Filename) && (m_LineNumber == other.m_LineNumber))
                return true;
            else
                return false;
        };
};

#endif //LINEADDRESS_H_INCLUDED
