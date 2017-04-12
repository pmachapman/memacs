cd \usr\emacs
clean -r diff
mkdir diff
cd diff
pkunpak \usr\dist311\ue311c
pkunpak \usr\dist311\ue311h
diff -c amigados.c ..\src\amigados.c > amigados.dif
diff -c ansi.c ..\src\ansi.c > ansi.dif
diff -c aosvs.c ..\src\aosvs.c > aosvs.dif
diff -c basic.c ..\src\basic.c > basic.dif
diff -c bind.c ..\src\bind.c > bind.dif
diff -c buffer.c ..\src\buffer.c > buffer.dif
diff -c char.c ..\src\char.c > char.dif
diff -c cmdfix.c ..\src\cmdfix.c > cmdfix.dif
diff -c crypt.c ..\src\crypt.c > crypt.dif
diff -c dg10.c ..\src\dg10.c > dg10.dif
diff -c display.c ..\src\display.c > display.dif
diff -c dolock.c ..\src\dolock.c > dolock.dif
diff -c eval.c ..\src\eval.c > eval.dif
diff -c exec.c ..\src\exec.c > exec.dif
diff -c execpr.asm ..\src\execpr.asm > execpr.dif
diff -c file.c ..\src\file.c > file.dif
diff -c fileio.c ..\src\fileio.c > fileio.dif
diff -c fmr.c ..\src\fmr.c > fmr.dif
diff -c fmrdos.c ..\src\fmrdos.c > fmrdos.dif
diff -c history.c ..\src\history.c > history.dif
diff -c hp110.c ..\src\hp110.c > hp110.dif
diff -c hp150.c ..\src\hp150.c > hp150.dif
diff -c ibmpc.c ..\src\ibmpc.c > ibmpc.dif
diff -c input.c ..\src\input.c > input.dif
diff -c isearch.c ..\src\isearch.c > isearch.dif
diff -c keyboard.c ..\src\keyboard.c > keyboard.dif
diff -c line.c ..\src\line.c > line.dif
diff -c lock.c ..\src\lock.c > lock.dif
diff -c main.c ..\src\main.c > main.dif
diff -c mouse.c ..\src\mouse.c > mouse.dif
diff -c mpe.c ..\src\mpe.c > mpe.dif
diff -c msdos.c ..\src\msdos.c > msdos.dif
diff -c nec.c ..\src\nec.c > nec.dif
diff -c necdos.c ..\src\necdos.c > necdos.dif
diff -c os2.c ..\src\os2.c > os2.dif
diff -c os2npm.c ..\src\os2npm.c > os2npm.dif
diff -c random.c ..\src\random.c > random.dif
diff -c region.c ..\src\region.c > region.dif
diff -c screen.c ..\src\screen.c > screen.dif
diff -c search.c ..\src\search.c > search.dif
diff -c smg.c ..\src\smg.c > smg.dif
diff -c st52.c ..\src\st52.c > st52.dif
diff -c stmouse.s ..\src\stmouse.s > stmouse.dif
diff -c tags.c ..\src\tags.c > tags.dif
diff -c tcap.c ..\src\tcap.c > tcap.dif
diff -c tipc.c ..\src\tipc.c > tipc.dif
diff -c tos.c ..\src\tos.c > tos.dif
diff -c unix.c ..\src\unix.c > unix.dif
diff -c vms.c ..\src\vms.c > vms.dif
diff -c vt52.c ..\src\vt52.c > vt52.dif
diff -c window.c ..\src\window.c > window.dif
diff -c wmcs.c ..\src\wmcs.c > wmcs.dif
diff -c word.c ..\src\word.c > word.dif
diff -c z309.c ..\src\z309.c > z309.dif
del *.asm
del *.c
del *.s
diff -c dutch.h ..\h\dutch.h > dutch.dif
diff -c ebind.h ..\h\ebind.h > ebind.dif
diff -c edef.h ..\h\edef.h > edef.dif
diff -c efunc.h ..\h\efunc.h > efunc.dif
diff -c elang.h ..\h\elang.h > elang.dif
diff -c english.h ..\h\english.h > english.dif
diff -c epath.h ..\h\epath.h > epath.dif
diff -c eproto.h ..\h\eproto.h > eproto.dif
diff -c estruct.h ..\h\estruct.h > estruct.dif
diff -c evar.h ..\h\evar.h > evar.dif
diff -c french.h ..\h\french.h > french.dif
diff -c german.h ..\h\german.h > german.dif
diff -c japan.h ..\h\japan.h > japan.dif
diff -c latin.h ..\h\latin.h > latin.dif
diff -c platin.h ..\h\platin.h > platin.dif
diff -c spanish.h ..\h\spanish.h > spanish.dif
del *.h
