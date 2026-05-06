# Engine library (object library)

add_library(Engine OBJECT
    Component.cpp
    Controller.cpp
    Font.cpp
    FPSComponent.cpp
    GameObject.cpp
    InputManager.cpp
    Minigin.cpp
    Renderer.cpp
    ResourceManager.cpp
    RotatorComponent.cpp
    Scene.cpp
    SceneManager.cpp
    SoundSystem.cpp
    TextComponent.cpp
    Texture2D.cpp
    TextureComponent.cpp
    Transform.cpp
    Command.h
    DeltaTime.h
    Event.h
    GameObjectCommand.h
    IObserver.h
    ISoundSystem.h
    LoggingSoundSystem.h
    NullSoundSystem.h
    ServiceLocator.h
    Singleton.h
    Subject.h
)

# Use C++20
target_compile_features(Engine PUBLIC cxx_std_20)

# Include paths
target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${glm_SOURCE_DIR}
    ${sdl3_SOURCE_DIR}/include
    ${sdl3_ttf_SOURCE_DIR}/include
    ${VLD_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(Engine PUBLIC
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
    ${VLD_LIBRARIES}
)

# Windows system libs
if(WIN32)
    target_link_libraries(Engine PUBLIC xinput winmm)
endif()

# Enable warnings
if(MSVC)
    target_compile_options(Engine PRIVATE /W4 /WX)
else()
    target_compile_options(Engine PRIVATE -Wall -Wextra -Werror)
endif()