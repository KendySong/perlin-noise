workspace "perlin-noise"
    location "build"
    architecture "x64"
    configurations { "Debug", "Release" }

project "perlin-noise"
    kind "ConsoleApp"
    language "C++"
    location "build"    

    defines "SFML_STATIC"
    files { "src/**.cpp", "src/**.hpp", "src/**.h", "libraries/include/ImGui/**.cpp" }

    includedirs "libraries/*"
    libdirs "libraries/lib"
    links {
        "opengl32",
        "winmm"
    }

filter "configurations:Debug"
    defines "DEBUG"
    symbols "On"
    links {
        "sfml-graphics-s-d",
        "sfml-window-s-d",
        "sfml-system-s-d"
    }

filter "configurations:Release"
    defines "RELEASE"
    optimize "Speed"
    links {
        "sfml-graphics-s",
        "sfml-window-s",
        "sfml-system-s"
    }