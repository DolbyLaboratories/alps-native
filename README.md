# How to Build

1. Install CMake.
2. Install Conan package manager.
3. If you don't have a Conan profile yet, run:  
   `conan profile detect --name <CONAN_PROFILE>`
4. Change directory to project's root:  
   `cd <project_root>`
5. Install dependencies:  
   `conan install . --profile=<CONAN_PROFILE> --build=missing`
6. List CMake presets that were created by Conan and choose one:  
   `cmake --list-presets`
7. Configure with chosen preset:  
   `cmake --preset <PRESET_NAME>`
8. Build:  
   `cmake --build --preset <PRESET_NAME>`

# Known Issues

## Content Limitations
The ALPS library has been tested using **CMAF (Common Media Application Format)**
compliant content. Using the ALPS library with content that uses features beyond
the standardized CMAF feature subset may cause unexpected behavior.
