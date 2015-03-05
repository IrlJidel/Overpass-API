#include "lz4_wrapper.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <vector>

LZ4_Deflate::LZ4_Deflate() { }

LZ4_Deflate::~LZ4_Deflate() { }

int LZ4_Deflate::compress(const void* in, int in_size, void* out, int out_buffer_size)
{
   int ret = LZ4_compress_limitedOutput((const char*) in, (char *) out + 4, in_size, out_buffer_size - 4);
   if (ret == 0)
     throw Error(-5); // Too few output space

   *(unsigned int*)out = ret;

   //std::cout << "compress: " << in_size << " --> " << ret << "\n";
   return ret;
}

LZ4_Inflate::LZ4_Inflate() { }

LZ4_Inflate::~LZ4_Inflate() { }


int LZ4_Inflate::decompress(const void* in, int in_size, void* out, int out_buffer_size)
{
  int in_size_compressed = *(unsigned int*)in;
  int ret = LZ4_decompress_safe((const char*) in + 4, (char*) out, in_size_compressed, out_buffer_size);
//  std::cout << "decompress: " << in_size << " ~ " << in_size_compressed << " --> " << ret << "\n";
  if (ret < 0)
    throw Error (ret);

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
