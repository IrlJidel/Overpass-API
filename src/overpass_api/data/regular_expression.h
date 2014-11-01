/** Copyright 2008, 2009, 2010, 2011, 2012 Roland Olbricht
*
* This file is part of Overpass_API.
*
* Overpass_API is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Overpass_API is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Overpass_API.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DE__OSM3S___OVERPASS_API__DATA__REGULAR_EXPRESSION_H
#define DE__OSM3S___OVERPASS_API__DATA__REGULAR_EXPRESSION_H

#include "sys/types.h"
#include "locale.h"
#include "regex.h"

#include <iostream>
#include <pcre.h>
#include <string>


struct Regular_Expression_Error
{
  public:
    Regular_Expression_Error(int errno_) : error_no(errno_) {}
    int error_no;
};


class Regular_Expression
{
  public:
    Regular_Expression() {}
    Regular_Expression(const std::string& regex, bool case_sensitive) {}
    
    virtual ~Regular_Expression() {};
    
    virtual bool matches(const std::string& line) const = 0;
    
};

class Regular_Expression_POSIX : public Regular_Expression
{
  public:
    Regular_Expression_POSIX(const std::string& regex, bool case_sensitive)
    {
      setlocale(LC_ALL, "C.UTF-8");
      int case_flag = case_sensitive ? 0 : REG_ICASE;
      int error_no = regcomp(&preg, regex.c_str(), REG_EXTENDED|REG_NOSUB|case_flag);
      if (error_no != 0)
        throw Regular_Expression_Error(error_no);
    }

    ~Regular_Expression_POSIX() { regfree(&preg); }

    bool matches(const std::string& line) const
    {
      return (regexec(&preg, line.c_str(), 0, 0, 0) == 0);
    }

  private:
    regex_t preg;
};

class Regular_Expression_PCRE : public Regular_Expression
{

  public:
    Regular_Expression_PCRE(const std::string& regex, bool case_sensitive)
    {
      const char* error;
      int erroroffset;
      int case_flag = case_sensitive ? 0 : PCRE_CASELESS;

      m_pcre = pcre_compile(regex.c_str(), PCRE_UTF8|case_flag, &error, &erroroffset, NULL);

      if (m_pcre == NULL)
       throw Regular_Expression_Error(1);

      m_pcreExtra = pcre_study(m_pcre, 0, &error);

      if(error != NULL)
        throw Regular_Expression_Error(1);
    }

    ~Regular_Expression_PCRE()
    {
      if (m_pcre)
       pcre_free(m_pcre);

      if(m_pcreExtra)
        pcre_free(m_pcreExtra);
    }

    bool matches(const std::string& line) const
    {
      int options;
      int startOffset = 0;
      int outVec[1];

      int rc = pcre_exec(m_pcre, m_pcreExtra, line.c_str(), line.length(), startOffset, options, outVec, 1);

      if (rc < 0)
      {
        if (rc != PCRE_ERROR_NOMATCH)
          throw Regular_Expression_Error(1);
        else
          return false;
      }
      return true;
    }

  private:
    pcre* m_pcre;
    pcre_extra *m_pcreExtra;
};

#endif
