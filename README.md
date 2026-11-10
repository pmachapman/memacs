# MicroEMACS
This is an unofficial repository for the MicroEMACS Editor by Daniel Lawrence.

## Introduction
I have found it hard to track down the source code for MicroEMACS, 
and with Daniel's passing I have noticed that links from his home page are breaking.

This project seeks to collect the source code and binaries for MicroEMACS, and provide them all from one place

## Lua scripting

This version embeds Lua 5.4 as its main scripting language, alongside the original procedure language. Put your setup in `emacs.lua` (`.emacs.lua` on UNIX) instead of `emacs.rc`, and press `M-:` to run a line of Lua. See [doc/lua.md](doc/lua.md) for the details, and [cmd/emacs.lua](cmd/emacs.lua) for the standard startup file written in Lua.

## File encryption

CRYPT mode now encrypts files with XChaCha20-Poly1305 and Argon2id, from the [Monocypher](src/monocypher) library, in place of the original 1986 cipher. A wrong key or a changed file is detected instead of loading garbage. Encrypted files are recognized when you open them, so CRYPT mode turns on by itself and the editor asks for the key. Files encrypted by older versions are still read, and are saved in the new format. The 16-bit MS-DOS and Windows 3.x builds keep the old cipher. Run `emacs @test/cryptotest.lua` to check it.

## Readme

Please refer to [DOC/README](doc/readme) for the complete MicroEMACS readme file.

## Copyright

This code retains the copyright Daniel placed on this work (refer to [LICENSE.md](LICENSE.md)), and on his passing I assume it is now with his estate.

I will endeavour to respect this, and request that you do the same, until notified otherwise.
