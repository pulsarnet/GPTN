# GPTN

# Build Qt-Advanced-Docking-System

```shell
git clone https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System.git
cd Qt-Advanced-Docking-System
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=</path/to/qt> -DCMAKE_INSTALL_PREFIX:PATH=</path/to/install> ..

# For debug build
cmake --build . --target install --config Debug
# For release build
cmake --build . --target install --config Release
```