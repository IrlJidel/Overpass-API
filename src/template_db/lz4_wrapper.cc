#include "lz4_wrapper.h"

#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

LZ4_Deflate::LZ4_Deflate() { }

LZ4_Deflate::~LZ4_Deflate() { }

int LZ4_Deflate::compress(const void* in, int in_size, void* out, int out_buffer_size)
{
     int ret = LZ4_compress_limitedOutput((const char*) in, (char *) out + 4, in_size, out_buffer_size - 4);

   if (ret == 0)    // buffer is too small, skip compression
   {
     if (in_size > out_buffer_size - 4)
     {
       std::cout << "compress: " << in_size << " -- " << out_buffer_size << "\n";
       std::ofstream output ("smallbuffer.bin", std::ios::binary|std::ios::out);
       output.write((const char*) in, in_size);
       output.close();
       throw Error (-5);     // buffer too small, abort processing
     }

     *(int*)out = in_size * -1;
     std::memcpy ((char *) out + 4, (const char*)in, in_size);
     ret = in_size;
   }
   else
     *(int*)out = ret;

   return ret;
}

LZ4_Inflate::LZ4_Inflate() { }

LZ4_Inflate::~LZ4_Inflate() { }


int LZ4_Inflate::decompress(const void* in, int in_size, void* out, int out_buffer_size)
{
  int ret;
  int in_buffer_size = *(int*)in;

  if (in_buffer_size > 0)
  {
    ret = LZ4_decompress_safe((const char*) in + 4, (char*) out, in_buffer_size, out_buffer_size);
    if (ret < 0)
      throw Error (ret);
  }
  else
  {
    if ((in_buffer_size * -1) > out_buffer_size)
      throw Error (1);

    std::memcpy ((char*) out, (const char*) in + 4, in_buffer_size * -1);
    ret = in_buffer_size * -1;
  }
  return ret;
}


const char* LZ4_Deflate::Error::what() const throw()
{
  std::ostringstream out;
  out<<"LZ4_Deflate: "<<error_code;
  return out.str().c_str();
}


const char* LZ4_Inflate::Error::what() const throw()
{
  std::ostringstream out;
  out<<"LZ4_Inflate: "<<error_code;
  return out.str().c_str();
}
