
## Fetch deps
```sh
./deps.sh
```

#On linux:
```sh
sudo apt-get install libasound-dev libx11-dev libxft-dev
```

## Build via GN
1. `git clone --depth 1 --single-branch -b cpp14 https://github.com/dyu/gn-build`

2. `echo 'buildconfig = "//gn-build/config/BUILDCONFIG.gn"' > .gn`
   On windows, exclude the single quote.

3. On linux:
   ```sh
   gn gen gn-out --args='gcc_cc="gcc" gcc_cxx="g++" symbol_level=0 is_debug=false is_clang=false is_official_build=true'
   ```
   On windows:
   ```sh
   gn gen gn-out --args="visual_studio_path=\"C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\" visual_studio_version=\"2015\" symbol_level=0 is_debug=false is_clang=false is_official_build=true"
   ```

4. `ninja -C gn-out`
