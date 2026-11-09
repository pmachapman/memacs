-- LUATEST.LUA: automated checks for MicroEMACS's Lua scripting.
--
-- Run from a console:   emacs @test/luatest.lua
--
-- The editor runs the checks, writes a report to melua-test/results.txt
-- in the temp folder, and exits. The exit status is the number of
-- failed checks (0 means everything passed).

local self = debug.getinfo(1, "S").source:sub(2)
local base = (os.getenv("TEMP") or os.getenv("TMPDIR") or "/tmp"):gsub("\\", "/")
local dir = base .. "/melua-test"
os.execute('mkdir "' .. dir .. '"' .. (package.config:sub(1, 1) == "\\" and " 2>nul" or " 2>/dev/null"))
local out = assert(io.open(dir .. "/results.txt", "w"))
local fails = 0

local function check(name, cond, extra)
  if not cond then fails = fails + 1 end
  out:write((cond and "PASS " or "FAIL ") .. name ..
    (extra ~= nil and ("  -- " .. tostring(extra)) or "") .. "\n")
  out:flush()
end

local function eq(name, got, want)
  check(name, got == want, ("got %q want %q"):format(tostring(got), tostring(want)))
end

local function try(name, f)
  local ok, err = pcall(f)
  if not ok then check(name .. " (raised)", false, err) end
end

try("basics", function()
  eq("version", me.version, "5.00")
  eq("cbufname", me.var.cbufname, "main")
  me.var.fillcol = 64
  eq("fillcol set/get", me.var.fillcol, 64)
  eq("fillcol type", math.type(me.var.fillcol), "integer")
  eq("discmd boolean", type(me.var.discmd), "boolean")
  me.var.posflag = true
  eq("posflag true", me.var.posflag, true)
  me.var.posflag = false
  eq("posflag false", me.var.posflag, false)
  check("unknown var errors", not pcall(function() return me.var.nosuchvar end))
end)

try("text", function()
  check("insert", me.insert("hello\nworld"))
  eq("text", me.text(), "hello\nworld\n")
  local l, c = me.point()
  eq("point line", l, 2); eq("point col", c, 6)
  check("moveto", me.moveto(1, 1))
  eq("char", me.char(), "h")
  eq("line", me.line(), "hello")
  me.setline("HELLO")
  eq("setline", me.line(), "HELLO")
  check("moveto past end false", me.moveto(99) == false)
  me.moveto(1, 3)
  check("delete", me.delete(2))
  eq("after delete", me.line(), "HEO")
  me.delete(-1)
  eq("after backward delete", me.line(), "HO")
  me.setline("HELLO")
  eq("var.line", me.var.line, "HELLO")
end)

try("commands", function()
  me.moveto(1, 1)
  check("end-of-line", me.command("end-of-line"))
  local _, c = me.point()
  eq("eol col", c, 6)
  check("cmd table", me.cmd.beginning_of_file())
  check("search-forward found", me.command("search-forward", "world"))
  local l, c2 = me.point()
  eq("search line", l, 2); eq("search col", c2, 6)
  check("search-forward missing", me.command("search-forward", "zzzz") == false)
  eq("$status after failure", me.var.status, false)
  check("no such command raises", not pcall(me.command, "no-such-command"))
  check("numeric arg", me.command("next-line", 0) ~= nil)
end)

try("defcommand", function()
  me.command("end-of-file")
  me.defcommand("test-double", function(n, f)
    me.insert(string.rep("x", n))
  end)
  me.defcommand("test-flag", function(n, f) return f end)
  check("lua cmd with arg", me.command("test-double", 3))
  eq("lua cmd inserted", me.line(), "xxx")
  check("lua cmd without arg", me.command("test-double"))
  eq("lua cmd inserted once", me.line(), "xxxx")
  check("f is false without arg", me.command("test-flag") == false)
  check("f is true with arg", me.command("test-flag", 1) == true)
  check("builtin name rejected", not pcall(me.defcommand, "forward-character", function() end))
  local found = false
  for _, n in ipairs(me.commands()) do if n == "test-double" then found = true end end
  check("commands() lists lua command", found)
  -- old-language command line calling a Lua command
  me.setline("")
  check("execute-command-line", me.command("execute-command-line", "2 test-double"))
  eq("from command line", me.line(), "xx")
  -- run / execute-procedure
  me.setline("")
  check("execute-procedure", me.command("execute-procedure", "test-double"))
  eq("from execute-procedure", me.line(), "x")
  -- errors inside a command are reported, not raised
  me.defcommand("test-error", function() error("boom") end)
  check("failing command returns false", me.command("test-error") == false)
  check("error shown", (me.var.lastmesg or ""):find("boom") ~= nil, me.var.lastmesg)
end)

try("old language interop", function()
  local f = assert(io.open(dir .. "/old.cmd", "w"))
  f:write("store-procedure oldproc\n",
          "  set %fromold \"yes\"\n",
          "  test-double\n",
          "!endm\n",
          "set %loaded &add 40 2\n")
  f:close()
  check("execute-file .cmd", me.command("execute-file", dir .. "/old.cmd"))
  eq("uvar from old script", me.uvar.loaded, "42")
  me.setline("")
  check("run old procedure", me.command("oldproc"))
  eq("old proc set var", me.uvar.fromold, "yes")
  eq("old proc called lua", me.line(), "x")
  eq("eval", me.eval("&add 2 3"), "5")
  me.uvar.fromlua = "abc"
  eq("uvar to old", me.eval("%fromlua"), "abc")
  eq("eval string", me.eval("&cat \"a\" \"b\""), "ab")
end)

try("keys", function()
  eq("keyname C-x C-f", me.keyname("C-x C-f"), "^X^F")
  eq("keyname M-C-h", me.keyname("M-C-h"), "M-^H")
  eq("keyname C-x 2", me.keyname("C-x 2"), "^X2")
  eq("keyname native", me.keyname("^XF"), "^XF")
  eq("keyname C-x TAB", me.keyname("C-x TAB"), "^X^I")
  me.bind("C-x C-q", "test-double")
  eq("binding emacs form", me.binding("C-x C-q"), "test-double")
  eq("binding native form", me.binding("^X^Q"), "test-double")
  me.bind("M-z", function() end)
  eq("bind function", me.binding("M-z"), "a Lua function")
  me.bind("M-z", "forward-character")
  eq("rebind builtin", me.binding("M-z"), "forward-character")
  check("unbind", me.unbind("M-z"))
  eq("unbound", me.binding("M-z"), nil)
  check("bind bad target raises", not pcall(me.bind, "M-z", "no-such-thing"))
  eq("eval-lua key", me.binding("M-:"), "eval-lua")
  me.setline("")
  me.bind("M-z", "test-double")
  check("runkey", me.runkey("M-z", 2))
  eq("runkey ran command", me.line(), "xx")
  check("runkey unbound", me.runkey("M-C-q") == false)
  me.unbind("M-z")
  check("findfile", me.findfile(self) ~= nil)
  eq("findfile missing", me.findfile("no-such-file.xyz"), nil)
end)

try("hooks", function()
  local f = assert(io.open(dir .. "/sample.txt", "w"))
  f:write("one\ntwo\nthree\n")
  f:close()
  local seen
  local function onread(name) seen = name end
  me.hook("read", onread)
  check("find-file", me.command("find-file", dir .. "/sample.txt"))
  check("read hook ran", seen ~= nil and seen:find("sample.txt", 1, true) ~= nil, seen)
  eq("buffer text", me.text(), "one\ntwo\nthree\n")
  eq("buffer info", me.buffer().lines, 3)
  me.unhook("read", onread)
  eq("readhook restored", me.var.readhook, "nop")

  local entered = {}
  me.hook("buffer-enter", function(name) entered[#entered + 1] = name end)
  me.command("select-buffer", "main")
  eq("buffer-enter hook", entered[#entered], "main")
  me.unhook("buffer-enter")

  -- setting a hook variable to a Lua command name
  local count = 0
  me.defcommand("test-count", function() count = count + 1 end)
  me.var.exbhook = "test-count"
  eq("hook var shows lua command", me.var.exbhook, "test-count")
  me.command("select-buffer", "sample.txt")
  eq("exbhook ran lua command", count, 1)
  me.var.exbhook = "nop"
  me.command("select-buffer", "main")
end)

try("region and buffers", function()
  me.command("select-buffer", "sample.txt")
  me.moveto(1, 2)
  me.command("set-mark")
  me.moveto(2, 3)
  eq("region", me.region(), "ne\ntw")
  local names = table.concat(me.buffers(), ",")
  check("buffers()", names:find("sample.txt", 1, true) ~= nil, names)
  eq("text of other buffer", me.text("main"):sub(1, 5), "HELLO")
  me.command("select-buffer", "main")
end)

try("eval-lua and execute-buffer", function()
  check("eval-lua", me.command("eval-lua", "6 * 7"))
  eq("eval-lua result", me.var.lastmesg, "42")
  me.command("select-buffer", "t.lua")
  me.insert("me.uvar.bufran = 'y'\n")
  me.command("select-buffer", "main")
  check("execute-buffer lua", me.command("execute-buffer", "t.lua"))
  eq("buffer ran as lua", me.uvar.bufran, "y")
  local f = assert(io.open(dir .. "/script.lua", "w"))
  f:write("me.uvar.fileran = 'y'\n")
  f:close()
  check("execute-file lua", me.command("execute-file", dir .. "/script.lua"))
  eq("file ran as lua", me.uvar.fileran, "y")
end)

try("print", function()
  print("hello", 1, true)
  eq("print to message line", me.var.lastmesg, "hello  1  true")
  me.message("%d-%s", 5, "x")
  eq("message format", me.var.lastmesg, "5-x")
end)

out:write(("DONE %d failure(s)\n"):format(fails))
out:close()

-- leave without the "modified buffers" question
me.command("exit-emacs", fails)
