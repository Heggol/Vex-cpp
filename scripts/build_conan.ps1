Set-Location ..
conan profile detect --force
conan install . --output-folder=build --build=missing
Set-Location build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" 
