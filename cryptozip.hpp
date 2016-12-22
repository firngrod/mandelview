
namespace FirnLibs
{
  namespace Crypto
  {
    void ZipEncode(std::string &zipEncoded, const unsigned char *datPtr, const size_t &datSize);
    void UnzipDecode(unsigned char *bufPtr, const size_t &bufSize, const std::string &zipEncoded);
  }
}
