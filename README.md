Socksify
========

The finished product will be a tool that allows any command (curl,nmap,etc.) to make all requests via a SOCKS proxy.

compile with:

    gcc -c -fPIC -shared connect_socks.c -ldl
    gcc -c -fPIC -shared my_connect.c -ldl
    gcc -fPIC -shared -o libmyconnect.so my_connect.o connect_socks.o -ldl
    gcc -c parse_arg.c
    gcc -c socksify.c
    gcc socksify.o parse_arg.o connect_socks.o -o socksify -ldl

(Will make Makefile work eventually!)

As of right now, it compiles and executes fine, but I'm not sure if it really works properly.
