# How to Build
1. Install CMake
2. Install Conan package manager
3. If you don't have a Conan profile yet, run:  
   `conan profile detect --name <CONAN_PROFILE>`
4. Change directory to project's root:  
   `cd <project_root>`
5. Install dependencies:  
   `conan install . --profile=<CONAN_PROFILE> --build=missing`
6. List CMake presets that were created by Conan:
   `cmake --list-presets`
6. Configure (preset may hav a different name depending on your Conan profile):  
   `cmake --preset <PRESET_NAME>`
7. Build:  
   `cmake --build --preset <PRESET_NAME>`

