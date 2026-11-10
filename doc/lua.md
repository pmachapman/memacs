# Lua scripting in MicroEMACS

MicroEMACS has Lua 5.4 built in as its main scripting language. Scripts can run any editor command, define new commands, bind keys, read and change text, read and set the `$` environment variables, ask questions on the message line, and run code when files are read or written.

The original procedure language (Chapter 15 of the manual: `store-procedure`, `!if`, `&add`, `%vars` and the rest) still works. `.cmd` files, `emacs.rc` and the help system run as before, and each language can call commands written in the other.

## Quick start

Put this in `emacs.lua` (`.emacs.lua` on UNIX), in your home directory or anywhere MicroEMACS looks for `emacs.rc`:

```lua
me.var.fillcol = 72

me.defcommand("insert-date", function()
  me.insert(os.date("%Y-%m-%d"))
end)
me.bind("C-x d", "insert-date")
```

Start the editor and press `^X D` to insert the date. `cmd/emacs.lua` is the standard startup file rewritten in Lua and makes a longer example.

## Running Lua

| How | What it does |
|---|---|
| `emacs.lua` / `.emacs.lua` | Run at startup **instead of** `emacs.rc` when found. To run both, add `me.command("execute-file", "emacs.rc")` to it. |
| `emacs @script.lua` | Run a script at startup (instead of the startup file). `emacs @name` tries `name.lua` before `name.cmd`. |
| `M-:` (`eval-lua`) | Run one line of Lua. Results are shown on the message line, so `M-: me.var.fillcol` shows the fill column. |
| `execute-file` | Files ending in `.lua` run as Lua. With no extension, `name.lua` is tried before `name.cmd`. |
| `execute-buffer` | A buffer whose file or buffer name ends in `.lua` runs as Lua. |

If a script fails, the error is shown on the message line and the editor carries on. In the console versions, press `^G` to stop a script stuck in a loop (it raises the error `"interrupted"`). The MS-Windows GUI version can't interrupt a script yet.

`print()` writes to the message line.

## Commands and keys

| Function | Meaning |
|---|---|
| `me.command(name, [n], replies...)` | Run a command: built-in, Lua, or an old-language procedure. A number as the second argument is the numeric argument, as if typed with `^U`. Strings after it answer the command's prompts in order, so `me.command("find-file", "notes.txt")` opens a file without asking. With no replies the command prompts the user as usual. Returns true if the command succeeded. Raises an error if there is no such command. |
| `me.cmd.next_word(3)` | The same as `me.command("next-word", 3)`. Underscores become dashes. |
| `me.defcommand(name, fn)` | Define a command. `fn(n, f)` gets the numeric argument (1 by default) and whether the user gave one. Returning `false` means the command failed. The command can be run with `M-X`, `run` (`M-^E`), from `.cmd` files and command lines, and bound to keys. `me.defcommand(name, nil)` removes it. Built-in names can't be redefined. |
| `me.bind(key, target)` | Bind a key to a command name or a Lua function. |
| `me.unbind(key)` | Remove a binding. Returns false if the key wasn't bound. |
| `me.binding(key)` | Name of the command a key runs, `"a Lua function"`, or nil. Old-language procedures show as `[name]`. |
| `me.runkey(key, [n])` | Run whatever a key is bound to. |
| `me.keyname(key)` | Translate a key name to MicroEMACS form. |
| `me.commands()` | Sorted list of all built-in and Lua command names. |

Keys can be written the MicroEMACS way, as in `bind-to-key`: `"^A"`, `"M-F"`, `"^X^F"`, `"^X2"`, `"FN1"`, `"S-FN1"`, `"MSa"`. Emacs-style names also work: `"C-a"`, `"M-f"`, `"C-x C-f"`, `"C-x 2"`, `"M-C-h"`, and `SPC`, `TAB`, `RET`, `LFD`, `ESC`, `DEL`.

`macro-to-key` and `$readhook`-style hook variables also accept Lua command names, so old scripts can bind and hook Lua commands.

## Text

Line numbers and columns start at 1. Columns count bytes in the line, so `me.line():sub(col)` is the text from the cursor to the end of the line. (`me.var.curcol` is MicroEMACS's own display column, which counts tabs as several columns.)

| Function | Meaning |
|---|---|
| `me.insert(s, ...)` | Insert text at the cursor. `"\n"` starts a new line. |
| `me.delete([n])` | Delete `n` characters forward (default 1), or backward if `n` is negative. Not saved in the kill buffer. |
| `me.line()` | Text of the current line. |
| `me.setline(s)` | Replace the current line's text. |
| `me.char()` | Character at the cursor, `"\n"` at the end of a line, nil at the end of the buffer. |
| `me.point()` | Current line and column. |
| `me.moveto(line, [col])` | Move the cursor. Returns false if there is no such line (the cursor goes to the end of the buffer). |
| `me.region()` | Text between the cursor and the mark, or nil if there is no mark. Unlike `$region`, it isn't cut off at 255 characters. |
| `me.text([buffer])` | Whole text of a buffer, with `"\n"` after every line. |

## Buffers

| Function | Meaning |
|---|---|
| `me.buffer([name])` | Table with `name`, `file`, `changed`, `invisible`, `mode` (the `$cmode` bits), `lines` and `windows` for a buffer (default: the current one), or nil. |
| `me.buffers([all])` | List of buffer names. Invisible buffers (procedures, lists) are included only if `all` is true. |

Switch buffers with `me.command("select-buffer", name)`.

## Talking to the user

| Function | Meaning |
|---|---|
| `me.message(fmt, ...)` | Show a message, formatted like `string.format`. Shown even when `$discmd` is off. |
| `me.prompt(text, [default])` | Ask for a line of text. Returns the reply, the default if the reply is empty, or nil if the user pressed `^G`. |
| `me.yesno(text)` | Ask a yes/no question. Returns true, false, or nil for `^G`. |
| `me.getkey()` | Wait for a key and return its name, such as `"^XF"`. |
| `me.update()` | Redraw the screen now. |
| `me.beep()` | Ring the bell. |

## Programs in buffers

`M-X shell` opens a live command prompt in the `*shell*` buffer. Type a command after the prompt and press Enter to run it. The output appears as it is produced, even while you work in other buffers. Pressing Enter on an earlier line runs that line again. `^C` stops the command that is running, but not the shell. Running `shell` again goes back to the same buffer. In other buffers, Enter and `^C` keep their usual commands.

The shell can't run programs that take over the whole screen or read keys from the console (`edit`, `more`, `choice`). If one of these is waiting, press `^C`. Programs that hold back their output when it goes to a file show it in larger pieces.

`me.shelloptions` sets up the shell: `program` (default `%COMSPEC%`, normally `cmd.exe`), `buffer` (default `"*shell*"`), `dir` (the folder to start in), and `echoes` (true if the program prints each command it reads, as `cmd.exe` does; the typed copy is then removed).

The shell is built on these functions, which can run any program in a buffer:

| Function | Meaning |
|---|---|
| `me.process(command, [options])` | Start a program, given as a Windows command line. Its output goes into a buffer. `options` can have `buffer` (a buffer name, made if needed; default the current buffer), `dir` (the folder to run in) and `onexit` (a function called with the process and its exit code when it finishes). Returns a process, or nil and a reason. A buffer can have only one running process. |
| `me.getprocess([buffer])` | The process in a buffer (default the current one), or nil. |
| `me.waitoutput([seconds])` | Let output arrive for up to `seconds` (default 1). Returns true as soon as any does. Output normally arrives while the editor waits for a key, so scripts only need this when they wait for a program themselves. |
| `p:send(s)` | Send text to the program's input. Send `"\r\n"` to end a line. |
| `p:eof()` | Close the program's input. |
| `p:interrupt()` | Stop the programs this one has started, such as the command a shell is running. Returns how many were stopped. |
| `p:kill()` | Stop the program and everything it started. |
| `p:running()` | True while the program is running. |
| `p:exitcode()` | Exit code once it has finished, nil before that. |
| `p:mark()` | Line and column where the next output will go. Output goes before anything typed after this point. |
| `p:setmark(line, [col])` | Move that point. |
| `p:buffer()` | Name of the process's buffer, or nil if the buffer has been deleted. |
| `p:id()` | A number that identifies the process. |

When a program finishes, `[Process finished, exit code N]` is added to its buffer. Deleting the buffer stops the program, and programs still running when the editor exits are stopped too. Output is shown as plain text: carriage returns, bells and terminal escape codes (colours, window titles) are removed.

Programs in buffers are only in the Windows console version for now. Elsewhere `me.process` returns nil and a reason.

## Variables

`me.var` reads and sets the `$` environment variables, without the `$`: `me.var.fillcol = 64` is `set $fillcol 64`. All of the variables in the manual are available. Values come back as Lua types:

* flags such as `discmd`, `posflag` and `status` are booleans,
* names and text such as `cbufname`, `line`, `search` and `version` are strings,
* the rest, such as `fillcol`, `curline` and `cmode`, are integers.

Setting a variable accepts booleans, numbers and strings. Using an unknown name is an error.

Ordinary Lua variables take the place of `%` user variables. To share values with old-language scripts, use `me.uvar`: `me.uvar.count = 5` is `set %count 5`, and `me.uvar.count` reads `%count` (as a string, or nil if it isn't set).

`me.eval(expr)` evaluates an old-language expression and returns its value as a string, or nil on an error, for example `me.eval("&bind FN1")` or `me.eval("#mylist")`.

`me.version` is the editor version.

## Hooks

`me.hook(event, fn)` runs `fn` when something happens. Several functions can hook the same event. `me.unhook(event, fn)` removes one, and `me.unhook(event)` removes them all and puts back the default.

| Event | Hook variable | When | Argument |
|---|---|---|---|
| `read` | `$readhook` | After a file is read into a buffer. | File name |
| `write` | `$writehook` | Before a buffer is written. Changes made here are saved. | File name |
| `buffer-enter` | `$bufhook` | When a buffer is entered. | Buffer name |
| `buffer-exit` | `$exbhook` | When a buffer is left. | Buffer name |
| `command` | `$cmdhook` | Before each command is read from the keyboard. | none |
| `wrap` | `$wraphook` | When text in WRAP mode passes the fill column. A wrap hook replaces the built-in `wrap-word`. | none |
| `exit` | `$exithook` | When the editor is about to exit. Return `false` to stay. | none |

Hooking an event takes over its hook variable, replacing whatever an old-language script set there. A hook that raises an error is reported, and the other hooks for the event still run.

You can also set a hook variable to a command name: `me.var.readhook = "my-lua-command"`.

## From the procedure language

| Procedure language | Lua |
|---|---|
| `store-procedure name` ... `!endm` | `me.defcommand("name", function(n, f) ... end)` |
| `store-procedure name %a %b` | `me.defcommand` gets only `n` and `f`; ask with `me.prompt`, or write a plain Lua function |
| `run name`, `execute-procedure` | `me.command("name")` |
| `execute-file`, `execute-buffer` | The same commands; `.lua` files and buffers run as Lua |
| `!if`, `!else`, `!endif`, `!while`, `!break`, `!return`, `!goto` | Lua `if`, `while`, `break`, `return` |
| `!force command` | `me.command(...)` returns false instead of stopping, so just ignore the result |
| `!return value` / `&call` | Write a Lua function and call it |
| `set $fillcol 64` | `me.var.fillcol = 64` |
| `set %count 0` / `local %count` | `local count = 0` (or `me.uvar.count` to share with `.cmd` files) |
| `#buffer` | `me.eval("#buffer")`, or read the text with `me.text("buffer")` |
| `@"Prompt: "` | `me.prompt("Prompt: ")` |
| `&add`, `&sub`, `&times`, `&divide`, `&mod`, `&neg`, `&abs` | `+`, `-`, `*`, `//`, `%`, unary `-`, `math.abs` |
| `&cat`, `&left`, `&right`, `&mid`, `&len` | `..`, `s:sub(1, n)`, `s:sub(-n)`, `s:sub(i, i + n - 1)`, `#s` |
| `&sindex`, `&upper`, `&lower`, `&reverse`, `&chr`, `&asc`, `&trim` | `s:find(t, 1, true)`, `s:upper()`, `s:lower()`, `s:reverse()`, `string.char`, `s:byte()`, `s:match("^(.-)%s*$")` |
| `&seq`, `&sless`, `&equal`, `&less`, `&greater`, `&not`, `&and`, `&or` | `==`, `<`, `==`, `<`, `>`, `not`, `and`, `or` |
| `&band`, `&bor`, `&bxor`, `&bnot` | `&`, `\|`, `~`, unary `~` |
| `&rnd`, `&env`, `&exist`, `&find` | `math.random(n)`, `os.getenv`, `io.open`, `me.findfile` |
| `&gtkey`, `&gtcmd` | `me.getkey()` |
| `&bind` | `me.binding(key)` |
| other `&` functions (`&group`, `&xlate`, `&abbrev`, ...) | `me.eval("&group 1")` |
| `$readhook`, `$writehook`, `$bufhook`, `$exbhook`, `$cmdhook`, `$wraphook`, `$exithook` | `me.hook(...)` |
| `bind-to-key`, `macro-to-key` | `me.bind(key, name_or_function)` |
| `emacs.rc` / `.emacsrc` | `emacs.lua` / `.emacs.lua` |

The `$debug` single-stepper works only with the procedure language.

## Building

Lua 5.4.8 is in `src/lua`, unchanged from the Lua release, and `src/elua.c` connects it to the editor. The `LUASCRIPT` option in `h/estruct.h` turns Lua on. It is switched off automatically for 16-bit MS-DOS, Windows 3.x and the other legacy systems (Amiga, Atari ST, OS/2, VMS and so on), and for the Borland C++ 5.5 builds; these keep only the procedure language.

The `xp/vs2017`, `mswin/vs2017`, `macosx` and `linux` builds include the Lua files. `xp/gcc/makefile` builds the Windows console version with GCC (MinGW or TDM-GCC):

```
make -C xp/gcc
```

On Linux the makefile now also links with `-lm`.

## Testing

`test/luatest.lua` checks the Lua interface. Run it from the top of the source tree:

```
emacs @test/luatest.lua
```

The editor runs the checks and exits. The exit status is the number of failures, and the full report is in `melua-test/results.txt` in the temp folder.

`test/shelltest.lua` checks programs in buffers and the shell in the same way (Windows console version only). Its report is in `meshell-test/results.txt`.
