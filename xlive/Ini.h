#ifndef _INI_H_
#define _INI_H_

#include "stdafx.h"
#include <string>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

using namespace std;


class Ini
{
public:
    Ini(){};
    virtual ~Ini(void) {};

    bool open(std::string filename)
    {
        if(PathFileExistsA(filename.c_str()) && PathIsDirectoryA(filename.c_str()) == FALSE)
        {
            m_inifile = filename;
            return true;
        }       
        return false;
    }

    std::string& GetString(const std::string strSection, const std::string strKey, std::string&& defVal = std::string())
    {
        m_strRet.clear();

        if(m_inifile.empty()) 
        {
            m_strRet = defVal;
            return m_strRet;
        }

        char* strOut = new char[MAX_PATH];
        DWORD size = GetPrivateProfileStringA(strSection.c_str(), strKey.c_str(), defVal.c_str(), strOut, MAX_PATH, m_inifile.c_str());

        if(size > 0 && *strOut != '\0')
        {
            char* pStr = strchr(strOut, '#');
            if (pStr) *pStr='\0';

            pStr = strchr(strOut, ';');
            if (pStr) *pStr='\0';
        }
        else
        {
            delete [] strOut;          
            m_strRet = defVal;

            return m_strRet;
        }
        m_strRet = strOut;
        delete [] strOut;

        return m_strRet;
    }

    LONG GetLong(const char* strSection, const char* strKey, LONG defVal = 0)
    {
        return (LONG) GetPrivateProfileIntA(strSection,strKey,defVal,m_inifile.c_str());
    }

    SHORT GetShort(const char* strSection, const char* strKey, SHORT defVal = 0)
    {
        return static_cast<SHORT>(GetLong(strSection, strKey, defVal));
    }

    CHAR GetSByte(const char* strSection, const char* strKey, CHAR defVal = 0)
    {
        return static_cast<CHAR>(GetLong(strSection, strKey, defVal));
    }

    DWORD GetDword(const char* strSection, const char* strKey, DWORD defVal = 0)
    {
        return GetPrivateProfileIntA(strSection,strKey,defVal,m_inifile.c_str());
    }

    WORD GetWord(const char* strSection, const char* strKey, WORD defVal = 0)
    {
        return static_cast<WORD>(GetDword(strSection, strKey, defVal));
    }

    BYTE GetUByte(const char* strSection, const char* strKey, BYTE defVal = 0)
    {
        return static_cast<BYTE>(GetDword(strSection, strKey, defVal));
    }

    bool GetBool(const char* strSection, const char* strKey, bool defVal = false)
    {
        return GetDword(strSection, strKey, defVal) != 0;
    }

    bool is_open()
    {
        return !m_inifile.empty();
    }

private:
    std::string m_inifile;
    std::string m_strRet;
};

#endif // _INI_H_
