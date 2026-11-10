-- CRYPTOTEST.LUA: automated checks for CRYPT mode file encryption.
--
-- Run from a console:   emacs @test/cryptotest.lua
--
-- The editor runs the checks, writes a report to mecrypt-test/results.txt
-- in the temp folder, and exits. The exit status is the number of
-- failed checks (0 means everything passed).

local base = (os.getenv("TEMP") or os.getenv("TMPDIR") or "/tmp"):gsub("\\", "/")
local dir = base .. "/mecrypt-test"
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

local function readbytes(path)
  local f = io.open(path, "rb")
  if not f then return nil end
  local s = f:read("a")
  f:close()
  return s
end

local function writebytes(path, s)
  local f = assert(io.open(path, "wb"))
  f:write(s)
  f:close()
end

local MAGIC = "\0MECRYPT"

-- A fresh, empty buffer in CRYPT mode
local nbuf = 0
local function cryptbuf()
  nbuf = nbuf + 1
  me.command("select-buffer", 1, "crypt" .. nbuf)
  me.command("add-mode", "crypt")
  return "crypt" .. nbuf
end

-- Copy a file, so find-file reads it into a new buffer
local ncopy = 0
local function copyof(path)
  ncopy = ncopy + 1
  local p = dir .. "/copy" .. ncopy .. ".txt"
  writebytes(p, readbytes(path))
  return p
end

-- Mark the current buffer changed, so save-file writes it
local function touch()
  me.insert("x")
  me.command("delete-previous-character")
end

-- Open a file in a new CRYPT buffer, answering the key prompt
local function openwith(path, key)
  me.command("add-global-mode", "crypt")
  local ok = me.command("find-file", path, key)
  me.command("delete-global-mode", "crypt")
  return ok
end

-- The old DLH-POLY-86-B cipher from crypt.c, to make old-format files
local function oldcrypt(pw, text)
  local key, salt = 0, 0
  local function run(s)
    local t = {}
    for i = 1, #s do
      local cc = s:byte(i)
      if cc >= 32 and cc <= 126 then
        key = (key & 0x1FFFFFFF) ~ ((key >> 29) & 3)
        local c2 = ((key % 95) - (cc - 32)) % 95 + 32
        salt = salt + 1
        if salt >= 20857 then salt = 0 end
        key = key + key + (c2 ~ cc) + salt
        cc = c2
      end
      t[#t + 1] = string.char(cc)
    end
    return table.concat(t)
  end
  run(pw)
  return run(text)
end

local TEXT = "Secret line one\n\tcaf\233 and tabs\n\nlast line"

try("save and reopen", function()
  local path = dir .. "/a.txt"
  os.remove(path)
  cryptbuf()
  check("set key (typed twice)", me.command("set-encryption-key", "pw1", "pw1"))
  me.insert(TEXT)
  local t0 = os.clock()
  check("write-file", me.command("write-file", path))
  out:write(("     (saving took %.2f s)\n"):format(os.clock() - t0))
  local raw = readbytes(path)
  eq("file starts with marker", raw and raw:sub(1, 8), MAGIC)
  eq("file size", raw and #raw, 60 + #TEXT + 1 + 16)
  check("text not visible", raw and not raw:find("Secret", 1, true))
  check("tabs/newlines not visible", raw and not raw:find("\n\tcaf", 1, true))

  check("open with key", openwith(path, "pw1"))
  eq("text matches", me.text(), TEXT .. "\n")
  eq("crypt mode on", me.var.cmode & 0x80, 0x80)

  -- a second save keeps the salt but uses a new nonce
  touch()
  check("re-save", me.command("save-file"))
  local raw2 = readbytes(path)
  eq("same salt after re-save", raw2:sub(21, 36), raw:sub(21, 36))
  check("new nonce after re-save", raw2:sub(37, 60) ~= raw:sub(37, 60))
  check("open again", openwith(copyof(path), "pw1"))
  eq("text after re-save", me.text(), TEXT .. "\n")
end)

try("mismatched keys", function()
  cryptbuf()
  check("set key with typo fails", not me.command("set-encryption-key", "abc", "abd"))
end)

try("wrong key", function()
  local path = copyof(dir .. "/a.txt")
  check("open with wrong key fails", not openwith(path, "nope"))
  eq("buffer empty", me.text(), "")
  check("read-file with right key", me.command("read-file", path, "pw1"))
  eq("text after retry", me.text(), TEXT .. "\n")
end)

try("tampered file", function()
  local path = dir .. "/a.txt"
  local raw = readbytes(path)
  local i = 70
  local bad = raw:sub(1, i - 1) .. string.char(raw:byte(i) ~ 1) .. raw:sub(i + 1)
  writebytes(dir .. "/bad.txt", bad)
  check("changed text detected", not openwith(dir .. "/bad.txt", "pw1"))
  eq("nothing loaded", me.text(), "")
  -- the header is checked too
  bad = raw:sub(1, 16) .. string.char(raw:byte(17) ~ 1) .. raw:sub(18)
  writebytes(dir .. "/bad2.txt", bad)
  check("changed header detected", not openwith(dir .. "/bad2.txt", "pw1"))
end)

try("empty file", function()
  local path = dir .. "/empty.txt"
  os.remove(path)
  cryptbuf()
  me.command("set-encryption-key", "e", "e")
  check("write empty", me.command("write-file", path))
  eq("empty file size", #readbytes(path), 60 + 16)
  check("open empty", openwith(path, "e"))
  eq("empty text", me.text(), "")
end)

try("old format", function()
  local path = dir .. "/old.txt"
  local plain = "Old secret line\nSecond line, with ~ and \"quotes\"\n"
  local enc = oldcrypt("oldpw", plain)
  check("old cipher changed the text", enc ~= plain)
  writebytes(path, enc)
  check("open old file", openwith(path, "oldpw"))
  eq("old text decrypted", me.text(), plain)
  -- the old format can't check the key, so saving asks for it again
  touch()
  check("save upgrades", me.command("save-file", "oldpw"))
  check("upgrade message", (me.var.lastmesg or ""):find("new encrypted format", 1, true) ~= nil,
        me.var.lastmesg)
  eq("now new format", readbytes(path):sub(1, 8), MAGIC)
  check("reopen upgraded", openwith(copyof(path), "oldpw"))
  eq("upgraded text", me.text(), plain)
end)

try("old format, wrong retype", function()
  local path = dir .. "/old2.txt"
  writebytes(path, oldcrypt("k2", "abc\n"))
  check("open", openwith(path, "k2"))
  touch()
  check("save with wrong retype fails", not me.command("save-file", "zz"))
  eq("file not changed", readbytes(path), oldcrypt("k2", "abc\n"))
end)

try("append", function()
  local path = dir .. "/app.txt"
  os.remove(path)
  cryptbuf()
  me.command("set-encryption-key", "ap", "ap")
  me.insert("one")
  check("write", me.command("write-file", path))
  check("append", me.command("append-file", path))
  check("open appended", openwith(path, "ap"))
  eq("appended text", me.text(), "one\none\n")
end)

try("insert-file", function()
  local path = dir .. "/a.txt"
  cryptbuf()
  me.command("set-encryption-key", "pw1", "pw1")
  check("insert encrypted file", me.command("insert-file", path))
  eq("inserted text", me.text(), TEXT .. "\n")
end)

try("recognized without CRYPT mode", function()
  -- find-file of an encrypted file turns CRYPT mode on by itself
  local path = copyof(dir .. "/a.txt")
  check("find-file asks for key", me.command("find-file", path, "pw1"))
  eq("recognized text", me.text(), TEXT .. "\n")
  eq("crypt mode turned on", me.var.cmode & 0x80, 0x80)
  touch()
  check("save recognized file", me.command("save-file"))
  eq("still encrypted", readbytes(path):sub(1, 8), MAGIC)
  check("text still hidden", not readbytes(path):find("Secret", 1, true))

  -- a wrong key leaves the buffer empty but in CRYPT mode
  local p2 = copyof(dir .. "/a.txt")
  check("wrong key fails", not me.command("find-file", p2, "nope"))
  eq("nothing loaded", me.text(), "")
  check("read-file retry", me.command("read-file", p2, "pw1"))
  eq("text after retry", me.text(), TEXT .. "\n")

  -- insert-file into a plain buffer decrypts, and the buffer stays plain
  me.command("select-buffer", 1, "plainins")
  check("insert into plain buffer", me.command("insert-file", dir .. "/a.txt", "pw1"))
  eq("inserted text", me.text(), TEXT .. "\n")
  eq("buffer still plain", me.var.cmode & 0x80, 0)
  me.command("select-buffer", 1, "plainins2")
  check("insert with wrong key fails", not me.command("insert-file", dir .. "/a.txt", "bad"))

  -- appending plain text to an encrypted file is refused
  local p3 = copyof(dir .. "/a.txt")
  local before = readbytes(p3)
  me.command("select-buffer", 1, "plainapp")
  me.insert("plain")
  check("append refused", not me.command("append-file", p3))
  eq("encrypted file unchanged", readbytes(p3), before)

  -- old-format files have no marker, so they open as they are
  local p4 = dir .. "/old3.txt"
  writebytes(p4, oldcrypt("k", "abc\n"))
  check("old file opens plain", me.command("find-file", p4))
  eq("old file not recognized", me.var.cmode & 0x80, 0)
end)

try("plain files unaffected", function()
  local path = dir .. "/plain.txt"
  writebytes(path, "plain text\n")
  check("open plain", me.command("find-file", path))
  eq("plain text", me.text(), "plain text\n")
  me.insert("more ")
  check("save plain", me.command("save-file"))
  eq("saved as plain", readbytes(path):gsub("\r", ""), "more plain text\n")
  writebytes(dir .. "/run.cmd", "set %plainran \"y\"\n")
  check("run .cmd file", me.command("execute-file", dir .. "/run.cmd"))
  eq(".cmd ran", me.uvar.plainran, "y")
end)

out:write(("DONE %d failure(s)\n"):format(fails))
out:close()

-- leave without the "modified buffers" question
me.command("exit-emacs", fails)
