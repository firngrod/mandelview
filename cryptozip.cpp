#include <cryptopp/gzip.h>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>

namespace FirnLibs
{
  namespace Crypto
  {
    void ZipEncode(std::string &zipEncoded, const unsigned char *datPtr, const size_t &datSize)
    {
      CryptoPP::ArraySource s(datPtr, datSize, true, new CryptoPP::Gzip(new CryptoPP::Base64Encoder(new CryptoPP::StringSink(zipEncoded)), CryptoPP::Gzip::MAX_DEFLATE_LEVEL));
    }

    void UnzipDecode(unsigned char *bufPtr, const size_t &bufSize, const std::string &zipEncoded)
    {
      CryptoPP::StringSource s(zipEncoded, true, new CryptoPP::Base64Decoder(new CryptoPP::Gunzip(new CryptoPP::ArraySink(bufPtr, bufSize))));
    }
  }
}


