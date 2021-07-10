shell: main.c headers.o prompt/prompt.o builtins/cd.o builtins/pwd.o builtins/echo.o builtins/env.o ls/ls.o nph/nph.o pinfo/pinfo.o history/history.o nightswatch/nightswatch.o
	gcc -o shell main.c prompt/prompt.o builtins/cd.o builtins/pwd.o builtins/echo.o builtins/env.o ls/ls.o nph/nph.o pinfo/pinfo.o history/history.o nightswatch/nightswatch.o headers.o -lm

headers.o:	headers.c headers.h
	gcc -c headers.c -o headers.o

prompt/prompt.o: prompt/prompt.c prompt/prompt.h
	gcc -c prompt/prompt.c -o prompt/prompt.o

builtins/cd.o: builtins/cd.c builtins/cd.h
	gcc -c builtins/cd.c -o builtins/cd.o

builtins/pwd.o: builtins/pwd.c builtins/pwd.h
	gcc -c builtins/pwd.c -o builtins/pwd.o

builtins/echo.o: builtins/echo.c builtins/echo.h
	gcc -c builtins/echo.c -o builtins/echo.o

builtins/env.o: builtins/env.c builtins/env.h
	gcc -c builtins/env.c -o builtins/env.o

ls/ls.o: ls/ls.c ls/ls.h
	gcc -c ls/ls.c -o ls/ls.o -lm

nph/nph.o: nph/nph.c nph/nph.h
	gcc -c nph/nph.c -o nph/nph.o

pinfo/pinfo.o: pinfo/pinfo.c pinfo/pinfo.h
	gcc -c pinfo/pinfo.c -o pinfo/pinfo.o -lm

history/history.o: history/history.c history/history.h
	gcc -c history/history.c -o history/history.o

nightswatch/nightswatch.o: nightswatch/nightswatch.c nightswatch/nightswatch.h
	gcc -c nightswatch/nightswatch.c -o nightswatch/nightswatch.o

clean:
	rm shell prompt/prompt.o ls/ls.o nph/nph.o pinfo/pinfo.o history/history.o nightswatch/nightswatch.o builtins/cd.o builtins/pwd.o builtins/echo.o builtins/env.o headers.o