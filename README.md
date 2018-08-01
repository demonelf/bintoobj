# bintoobj
1. 打包任何二进制资源到obj 并合并为 .a
2. ./bintoobj ./resource_dir
3. 编译时只需加
   resource.o -Wl,--whole-archive -lresource -Wl,--no-whole-archive 
4. 使用方法为
   find_resouce_by_path();
   
5. 通过此工具可以把所有外部资源打包到一个bin文件中
   使发布程序时就一个可执行程序
