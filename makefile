#-L./.tmp -Wl,--whole-archive -lresource -Wl,--no-whole-archive
#gcc -c resource.c -o resource.o

bintoobj: bintoobj.c
	gcc bintoobj.c -o bintoobj

clean:
	rm bintoobj libresource.a resource.o resource.c resource.h resource_tmp.c .tmp/* GPATH GTAGS GRTAGS cscope.files
