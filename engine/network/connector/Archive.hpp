#pragma once

// Macro to configure whether to use binary or json as the message type
//#define BLOKE_PROTOCOL_USE_JSON 1

/*
* Archive class defined here
*/

#ifdef BLOKE_PROTOCOL_USE_JSON 

#include <cereal/archives/json.hpp>
#define CEREAL_INPUT_ARCHIVE cereal::JSONInputArchive
#define CEREAL_OUTPUT_ARCHIVE cereal::JSONOutputArchive

#else

#include <cereal/archives/portable_binary.hpp>
#define CEREAL_INPUT_ARCHIVE cereal::PortableBinaryInputArchive
#define CEREAL_OUTPUT_ARCHIVE cereal::PortableBinaryOutputArchive

#endif

