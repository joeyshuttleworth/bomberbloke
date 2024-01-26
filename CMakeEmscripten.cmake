# Define target
add_executable(bomberbloke_client "client/bomberbloke.cpp" "engine/client.cpp" "engine/network/connector/EmscriptenConnector.cpp" ${SHARED_SOURCES})
target_compile_features(bomberbloke_client PRIVATE cxx_std_11)

# Resources
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/assets.cmake ASSET_FILES)
string (REPLACE ";" "\n" ASSET_FILES "${ASSET_FILES}") # new line delimited
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/assets/web_assets.txt" ${ASSET_FILES})

# Add Emscripten-specific compile/link options
#-sSDL2_IMAGE_FORMATS="[""png""]"
target_compile_options(bomberbloke_client PRIVATE
    -sUSE_SDL=2
    -sUSE_SDL_MIXER=2
    -sUSE_SDL_IMAGE=2
    -sUSE_SDL_TTF=2
)
target_link_options(bomberbloke_client PRIVATE
    #-O3
    -O0
    
    -g
    #-sASSERTIONS=2
    #-fsanitize=address
    #-sSAFE_HEAP=1

    -sUSE_SDL=2
    -sUSE_SDL_MIXER=2
    -sUSE_SDL_IMAGE=2
    -sUSE_SDL_TTF=2

    -sSTACK_SIZE=100MB
    -sASYNCIFY 
    -sALLOW_MEMORY_GROWTH
    -sINITIAL_MEMORY=3GB
    -sMAXIMUM_MEMORY=4GB
    --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
)
target_link_libraries(bomberbloke_client PRIVATE websocket.js)
set_target_properties(bomberbloke_client PROPERTIES SUFFIX ".html")