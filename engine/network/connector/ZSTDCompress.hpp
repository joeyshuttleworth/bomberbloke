#pragma once

#include <string>
#include <vector>
#include <zdict.h>
#include <zstd.h>

class ZstdCompressor
{
public:
  ZstdCompressor();
  ~ZstdCompressor();
  
  std::string compress(const std::string& input);
  std::stringstream decompress(const std::string& compressed);

private:
  static ZSTD_CDict* cdict;
  static ZSTD_DDict* ddict;

  ZSTD_CCtx* cctx;
  ZSTD_DCtx* dctx;
};
