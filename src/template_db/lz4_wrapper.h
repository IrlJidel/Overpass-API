#ifndef DE__OSM3S___TEMPLATE_DB__LZ4_WRAPPER_H
#define DE__OSM3S___TEMPLATE_DB__LZ4_WRAPPER_H

#include "../lib/lz4/lz4.h"

#include <exception>


class LZ4_Deflate
{
public:
  struct Error : public std::exception
  {
    Error(int error_code_) : error_code(error_code_) {}
    virtual const char* what() const throw();
    int error_code;
  };

  explicit LZ4_Deflate();
  ~LZ4_Deflate();

  int compress(const void* in, int in_size, void* out, int out_buffer_size);
};


class LZ4_Inflate
{
public:
  struct Error : public std::exception
  {
    Error(int error_code_) : error_code(error_code_) {}
    virtual const char* what() const throw();
    int error_code;
  };

  LZ4_Inflate();
  ~LZ4_Inflate();
  
  int decompress(const void* in, int in_size, void* out, int out_buffer_size);
};


#endif
