#include "ZSTDCompress.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

ZstdCompressor::ZstdCompressor()
{
  std::ifstream dictionary("dictionary.dict", std::ios::binary);

  if (!dictionary.is_open()) {
    std::cout << "Could not find ZSTD dictionary";
  }

  std::streamsize dict_size = dictionary.tellg();
  dictionary.seekg(0, std::ios::beg);
  std::vector<char> dictionaryBuffer(dict_size);

  cctx = ZSTD_createCCtx();
  dctx = ZSTD_createDCtx();

  ZSTD_CCtx_loadDictionary(
    cctx, dictionaryBuffer.data(), dictionaryBuffer.size());
  ZSTD_DCtx_loadDictionary(
    dctx, dictionaryBuffer.data(), dictionaryBuffer.size());
}

ZstdCompressor::~ZstdCompressor()
{
  ZSTD_freeCCtx(cctx);
  ZSTD_freeDCtx(dctx);
}

std::string
ZstdCompressor::compress(const std::string& input)
{
  size_t compressed_size = ZSTD_compressBound(input.size());

  std::vector<char> compressed_data(compressed_size);
  // compressed_size = ZSTD_compress(compressed_data.data(), compressed_size,
  // input.c_str(), input.size(), 1);
  compressed_size = ZSTD_compressCCtx(cctx,
                                      compressed_data.data(),
                                      compressed_size,
                                      input.c_str(),
                                      input.size(),
                                      1);
  if (ZSTD_isError(compressed_size)) {
    std::cout << "Compression failed";
  }

  return std::string(compressed_data.data(), compressed_size);
}

std::stringstream
ZstdCompressor::decompress(const std::string& input)
{
  size_t decompressed_size =
    ZSTD_getFrameContentSize(input.c_str(), input.size());
  if (decompressed_size == ZSTD_CONTENTSIZE_UNKNOWN) {
    std::cout << "Could not decompress";
  }

  std::vector<char> decompressed_data(decompressed_size);
  // size_t result = ZSTD_decompress(decompressed_data.data(),
  // decompressed_size, input.c_str(), input.size());
  size_t result = ZSTD_decompressDCtx(dctx,
                                      decompressed_data.data(),
                                      decompressed_size,
                                      input.c_str(),
                                      input.size());

  if (ZSTD_isError(result)) {
    std::cout << "decompression failed: "
              << std::string(ZSTD_getErrorName(result));
  }

  std::stringstream data_in;
  data_in.write(decompressed_data.data(), decompressed_size);

  return data_in;
}
