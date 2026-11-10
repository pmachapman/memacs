-- SHELLTEST.LUA: automated checks for programs running in buffers
-- (me.process) and the shell command. Windows console version only.
--
-- Run from a console:   emacs @test/shelltest.lua
--
-- The editor runs the checks, writes a report to meshell-test/results.txt
-- in the temp folder, and exits. The exit status is the number of
-- failed checks (0 means everything passed).

local base = (os.getenv("TEMP") or os.getenv("TMPDIR") or "/tmp"):gsub("\\", "/")
local dir = base .. "/meshell-test"
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

-- Let output arrive until the buffer's text matches pattern
local function waitfor(pattern, bufname, secs)
  local t = os.clock()
  repeat
    if me.text(bufname):find(pattern) then return true end
    me.waitoutput(0.2)
  until os.clock() - t > (secs or 10)
  return me.text(bufname):find(pattern) ~= nil
end

local function count(s, plain)
  local n, i = 0, 1
  while true do
    local a, b = s:find(plain, i, true)
    if not a then return n end
    n, i = n + 1, b + 1
  end
end

local PROMPT = ">\n$"	-- cmd's prompt, waiting for input

try("shell command", function()
  check("shell starts", me.command("shell"))
  eq("in *shell*", me.buffer().name, "*shell*")
  check("prompt appears", waitfor(PROMPT))
  local p = me.getprocess()
  check("buffer has a process", p ~= nil)
  check("process running", p and p:running())
  eq("not marked changed", me.buffer().changed, false)

  -- type a command and press Enter
  me.insert("echo one two")
  check("enter sends it", me.runkey("^M"))
  check("output appears", waitfor("\none two\n.*" .. PROMPT))
  eq("command shown once", count(me.text(), "echo one two"), 1)
  eq("still not changed", me.buffer().changed, false)
  local l = me.point()
  eq("cursor follows output", l, me.buffer().lines)

  -- output that arrives while something is half typed goes before it
  me.insert("ping -n 2 127.0.0.1 >nul & echo late")
  me.runkey("^M")
  me.insert("echo par")
  check("late output", waitfor("\nlate\n"))
  local text = me.text()
  check("typing kept after the prompt", text:find(">echo par\n$") ~= nil, text:sub(-60))
  eq("cursor after the typing", me.char(), "\n")
  me.runkey("^M")
  check("finished typing runs", waitfor("\npar\n.*" .. PROMPT))

  -- Enter on an earlier line runs it again
  local lines = me.buffer().lines
  for i = 1, lines do
    me.moveto(i)
    if me.line():find(">echo one two$") then break end
  end
  check("found earlier command", me.line():find(">echo one two$") ~= nil)
  me.runkey("^M")
  check("earlier command ran again", waitfor("one two\n.*one two\n.*" .. PROMPT))

  -- ^C stops the running command, not the shell
  me.command("end-of-file")
  me.insert("ping -n 30 127.0.0.1")
  me.runkey("^M")
  me.waitoutput(1.5)
  local t0 = os.clock()
  check("^C interrupts", me.runkey("^C"))
  check("prompt back after ^C", waitfor("ping %-n 30 127.0.0.1\n.*" .. PROMPT, nil, 10))
  check("well before ping would end", os.clock() - t0 < 15)
  check("shell still running", p:running())

  -- running shell again just switches to it
  me.command("select-buffer", "main")
  check("shell again", me.command("shell"))
  eq("same buffer", me.buffer().name, "*shell*")
  check("same process", me.getprocess() == p)

  -- exit
  me.command("end-of-file")
  me.insert("exit 5")
  me.runkey("^M")
  check("finish message", waitfor("%[Process finished, exit code 5%]\n$"))
  check("not running", not p:running())
  eq("exit code", p:exitcode(), 5)

  -- once it has finished, Enter is a plain newline again
  me.insert("x")
  me.runkey("^M")
  check("plain newline after exit", me.text():find("x\n\n$") ~= nil)
  me.command("unmark-buffer")
end)

try("keys in other buffers", function()
  me.command("select-buffer", "plain")
  me.insert("ab")
  me.moveto(1, 2)
  me.runkey("^M")
  eq("enter is newline", me.text(), "a\nb\n")
  me.runkey("^C")
  eq("^C inserts a space", me.text(), "a\n b\n")
  me.command("unmark-buffer")
end)

try("buffer not on screen", function()
  me.command("select-buffer", "main")
  local p, err = me.process("cmd.exe /c echo hidden one& echo hidden two", { buffer = "hid" })
  check("started", p ~= nil, err)
  check("output arrived", waitfor("hidden two\n%[Process finished, exit code 0%]\n$", "hid"))
  eq("still in main", me.buffer().name, "main")
  eq("output", me.text("hid"), "hidden one\nhidden two\n[Process finished, exit code 0]\n")
  me.command("select-buffer", "hid")
  me.command("end-of-file")
  me.insert("ok")
  eq("buffer usable", me.text():sub(-3), "ok\n")
  me.command("unmark-buffer")
end)

try("onexit and exit codes", function()
  local got, gotp
  local p = me.process("cmd.exe /c exit 7", {
    buffer = "ex", onexit = function(q, code) gotp, got = q, code end })
  check("started", p ~= nil)
  waitfor("exit code 7", "ex")
  eq("onexit code", got, 7)
  check("onexit gets the process", gotp == p)
  eq("exitcode", p:exitcode(), 7)
  eq("buffer", p:buffer(), "ex")
  eq("tostring", tostring(p), "process " .. p:id())
end)

try("errors", function()
  local p, err = me.process("no-such-program-xyz.exe", { buffer = "bad" })
  check("bad program fails", p == nil)
  check("with a reason", type(err) == "string" and err:find("can't start") ~= nil, err)
  local q = me.process("cmd.exe", { buffer = "two" })
  check("first starts", q ~= nil)
  local r, err2 = me.process("cmd.exe", { buffer = "two" })
  check("second in same buffer refused", r == nil, err2)
  -- a finished process can be replaced
  q:kill()
  waitfor("Process finished", "two")
  check("killed", not q:running())
  local s = me.process("cmd.exe /c echo again", { buffer = "two" })
  check("new process after the old one ended", s ~= nil)
  waitfor("again", "two")
  check("old object now stale", q:exitcode() == nil and not q:running())
end)

try("deleting the buffer ends the process", function()
  local p = me.process("cmd.exe", { buffer = "kb" })
  check("running", p:running())
  check("delete buffer", me.command("delete-buffer", "kb"))
  check("process gone", not p:running())
  eq("no buffer", p:buffer(), nil)
end)

try("lots of output", function()
  local p = me.process("cmd.exe /c for /L %i in (1,1,3000) do @echo line %i", { buffer = "big" })
  check("all arrived", waitfor("line 3000\n%[Process finished", "big", 30))
  local text = me.text("big")
  eq("line count", count(text, "\n"), 3001)
  check("in order", text:find("line 1\nline 2\nline 3\n", 1, true) == 1)
end)

try("escape codes removed", function()
  local p = me.process('cmd.exe /c echo \27[31mred\27[0m plain& echo \27]0;title\7after', { buffer = "esc" })
  waitfor("Process finished", "esc")
  eq("text", me.text("esc"), "red plain\nafter\n[Process finished, exit code 0]\n")
end)

out:write(("DONE %d failure(s)\n"):format(fails))
out:close()

-- leave without the "modified buffers" question
me.command("exit-emacs", fails)
