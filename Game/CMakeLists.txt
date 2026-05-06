# Game target setup

# Set target name based on platform
if(EMSCRIPTEN)
# Web build
    set(TARGET_NAME minigin_web)   
    set(TARGET_PARAM)
else()
# Desktop build
    set(TARGET_NAME BubbleBobble)      
    set(TARGET_PARAM WIN32)
endif()

# Create executable with game files
add_executable(${TARGET_NAME} ${TARGET_PARAM}
    Main.cpp
    DealDamageCommand.h
    HealthComponent.h
    LivesDisplayComponent.h
    MoveCommand.h
    PickUpCommand.h
    PlayerDiedDisplayComponent.h
    PlayerPointsComponent.h
    PlaySoundCommand.h
    PointsDisplayComponent.h
)

# Include current folder
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link engine library
target_link_libraries(${TARGET_NAME} PRIVATE Engine)

# Enable warnings
if(MSVC)
# High warnings (MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)  
else()
# GCC/Clang
    target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -Werror)  
endif()

# Web build settings
if(EMSCRIPTEN)
    set(EMSCRIPTEN_COMPILE_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -g")
    set(EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS} -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH --preload-file \"${CMAKE_SOURCE_DIR}/Data@/\"")

    # Use custom toolchain
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/EmscriptenToolchain.cmake)

    # Apply flags
    set_target_properties(${TARGET_NAME} PROPERTIES
        COMPILE_FLAGS "${EMSCRIPTEN_COMPILE_FLAGS}"
        LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}"
    )

    # Copy web files
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/web" "$<TARGET_FILE_DIR:${TARGET_NAME}>/"
    )

	# Output as HTML
    set(CMAKE_EXECUTABLE_SUFFIX ".html")  

# Windows build
elseif(WIN32)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
    set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>")

    # Link Windows libs
    target_link_libraries(${TARGET_NAME} PRIVATE xinput winmm)

    # Copy SDL DLLs
    set(SDL_RUNTIME_DLLS SDL3::SDL3 SDL3_ttf::SDL3_ttf)
    foreach(dllTarget IN LISTS SDL_RUNTIME_DLLS)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:${dllTarget}>"
                "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            VERBATIM
        )
    endforeach()

    # Steam support
    if(Steamworks_FOUND)
        target_link_libraries(${TARGET_NAME} PRIVATE Steamworks::Steamworks)
        target_compile_definitions(${TARGET_NAME} PRIVATE USE_STEAMWORKS)

        # Copy Steam DLL
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${Steamworks_RUNTIME_DLL}" "$<TARGET_FILE_DIR:${TARGET_NAME}>")

        # Create app id file
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${TARGET_NAME}>/steam_appid.txt"
            COMMENT "Create steam_appid.txt"
        )
    endif()

    # Copy assets
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Data"
    )

# macOS build
elseif(APPLE)
    # Bundle settings
    set_target_properties(${TARGET_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    # Copy frameworks
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3-shared>" "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks/"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>" "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks/"
        COMMAND install_name_tool -change "@rpath/libSDL3_ttf.0.dylib" "@executable_path/../Frameworks/libSDL3_ttf.0.2.2.dylib" "$<TARGET_FILE:${TARGET_NAME}>"
        COMMAND install_name_tool -change "@rpath/libSDL3.0.dylib" "@executable_path/../Frameworks/libSDL3.0.dylib" "$<TARGET_FILE:${TARGET_NAME}>"
    )

    # Steam support
    if(Steamworks_FOUND)
        target_link_libraries(${TARGET_NAME} PRIVATE Steamworks::Steamworks)
        target_compile_definitions(${TARGET_NAME} PRIVATE USE_STEAMWORKS)

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${Steamworks_LIBRARIES}" "$<TARGET_BUNDLE_DIR:${TARGET_NAME}>/Contents/Frameworks/"
        )

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${TARGET_NAME}>/steam_appid.txt"
            COMMENT "Create steam_appid.txt"
        )
    endif()

    # Copy assets
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Data"
    )

# Linux / other
else()
    # Steam support
    if(Steamworks_FOUND)
        target_link_libraries(${TARGET_NAME} PRIVATE Steamworks::Steamworks)
        target_compile_definitions(${TARGET_NAME} PRIVATE USE_STEAMWORKS)

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${Steamworks_LIBRARIES}" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
        )

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E echo "480" > "$<TARGET_FILE_DIR:${TARGET_NAME}>/steam_appid.txt"
            COMMENT "Create steam_appid.txt"
        )
    endif()

    # Copy assets
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/Data" "$<TARGET_FILE_DIR:${TARGET_NAME}>/Data"
    )
endif()