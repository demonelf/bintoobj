#-L./ -Wl,--whole-archive -lresource -Wl,--no-whole-archive
bintoobj: bintoobj.c
	gcc bintoobj.c -o bintoobj

clean:
	rm bintoobj libresource.a resource.o resource.c resource_tmp.c .tmp/* GPATH GTAGS GRTAGS cscope.files
