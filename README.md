# bintoobj
1. 打包任何二进制资源到obj 并合并为 .a
2. ./bintoobj ./resource_dir
3. 编译时只需加
   resource.o -Wl,--whole-archive -lresource -Wl,--no-whole-archive 
4. 使用方法为
   find_resouce_by_path();
