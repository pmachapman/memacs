--	EMACS.LUA:	Standard MicroEMACS Startup program, in Lua
--			(the Lua version of emacs.rc)
--
--	When this file is found (as emacs.lua, or .emacs.lua on UNIX)
--	it is run instead of emacs.rc. See doc/lua.md for the "me"
--	library used here.

me.var.discmd = false
me.message("[Setting up....]")

-- If your screen "SNOWS", comment this line
me.var.flicker = false

-- To use an IBM-PC EGA card, uncomment the following line
-- me.var.sres = "EGA"

-- If you hate clocks or position counters, comment these
me.var.timeflag = true
me.var.posflag = true

-- Set default global modes
me.command("add-global-mode", "blue")
me.command("add-global-mode", "WHITE")
-- me.bind("`", "meta-prefix")	-- for annoying keyboards with ` at the top left

local FKEYS = "Function Keys"

-- Toggle the function key window display. The state is kept in
-- %rcfkeys because the .cmd pages set it to FALSE to force the window on.

me.defcommand("toggle-fkeys", function()
	if me.uvar.rcfkeys == "TRUE" then
		-- toggle the function key window off
		me.command("save-window")
		me.command("next-window", 1)
		if me.var.cbufname == FKEYS then
			me.command("delete-window")
		end
		me.command("restore-window")
		me.message("[Function key window OFF]")
		me.uvar.rcfkeys = false
		return
	end

	-- toggle the function key window on
	me.command("save-window")
	me.command("next-window", 1)
	if me.var.cbufname == "emacs.hlp" then
		me.command("delete-window")
	end
	if me.var.cbufname ~= FKEYS then
		me.command("split-current-window", 1)
		me.command("select-buffer", 1, FKEYS)
		me.command("add-mode", "red")
		me.command("resize-window", 5)
		me.command("goto-line", 1)
	end
	me.uvar.rcfkeys = true
	me.command("restore-window")
	if me.var.cbufname == FKEYS then
		me.command("next-window")
	end
	me.message("[Function key window ON]")
end)

-- Bring up the online help system

me.defcommand("get-help", function()
	me.var.discmd = false
	me.command("execute-file", "ehelp.cmd")
	me.var.discmd = true
end)

-- Load a new page

me.defcommand("get-page-loader", function()
	if not me.findfile("newpage.cmd") then
		me.message("[Can not find NEWPAGE.CMD]")
		return
	end
	me.command("execute-file", "newpage.cmd")
end)

-- Set the mode of a buffer from its file name when it is read in

local extmodes = {
	c = "cmode", h = "cmode", cpp = "cmode", hpp = "cmode",
	mss = "wrap", txt = "wrap",
}

me.hook("read", function(fname)
	local ext = fname:match("%.([^%.\\/:]+)$")
	local mode = ext and extmodes[ext:lower()]
	if mode then
		me.command("add-mode", mode)
	end
end)

-- Make the function key window a place to call up function keys
-- with the mouse

me.defcommand("mouse-clicks", function()
	-- remember where we started, and do the mouse movement
	me.command("save-window")
	me.command("mouse-move-down")

	-- if not in the function key window... leave
	if me.var.cbufname ~= FKEYS then
		return
	end

	-- first position is a screen reposition, let it through
	if me.var.xpos == 0 and me.var.ypos == 0 then
		me.command("restore-window")
		return
	end

	-- find the function key label at or just before the click
	local line = me.line()
	local _, col = me.point()
	local pos
	for p in line:sub(1, col + 2):gmatch("()[fF]%d") do
		if p <= col + 1 then pos = p end
	end
	if not pos then
		return
	end

	-- "f1".."f10" are function keys, "F1".."F10" shifted ones
	local num = tonumber(line:sub(pos + 1, pos + 2):match("%d+"))
	local key = "FN" .. (num == 10 and "0" or tostring(num))
	if line:sub(pos, pos) == "F" then
		key = "S-" .. key
	end

	-- swallow the up-button, go back, and run the key's command
	me.getkey()
	me.command("restore-window")
	me.runkey(key)
end)
me.bind("MSa", "mouse-clicks")

-- Rebind the function key group

me.bind("FN1", "search-forward")
me.bind("FN2", "search-reverse")
me.bind("FN3", "hunt-forward")
me.bind("FN4", "hunt-backward")
me.bind("FN5", "toggle-fkeys")
me.bind("FN6", "get-help")
me.bind("FN7", "next-window")
me.bind("FN8", "get-page-loader")
me.bind("FN9", "save-file")
me.bind("FN0", "exit-emacs")

-- Build the function key window

local bar = "\179"	-- line drawing character on the IBM-PC

me.command("split-current-window", 1)
me.command("select-buffer", 1, FKEYS)
me.insert("f1 search-> f2 <-search " .. bar .. "    MicroEMACS:  Text Editor\n",
	  "f3 hunt->   f4 <-hunt   " .. bar .. " \n",
	  "f5 fkeys    f6 help     " .. bar .. "  Available function key Pages include:\n",
	  "f7 nxt wind f8 pg[    ] " .. bar .. "    Word  Box  Emacs  Pascal  C  cObol Lisp\n",
	  "f9 save     f10 exit    " .. bar .. "  [use the f8 key to load Pages]\n")
me.command("unmark-buffer")
me.command("delete-window")
me.uvar.rcfkeys = false

if me.var.os == "UNIX" then
	-- Allow mainframes to simulate function keys
	-- with ^C<n> and ^C shifted-<n>

	me.defcommand("emulate-fkeys", function()
		if not me.var.pending then
			me.message("FN-")
		end
		local k = me.getkey()
		local i = #k == 1 and ("1234567890!@#$%^&*()"):find(k, 1, true)
		if not i then
			me.message("[Not Bound]")
			return
		end
		me.command("clear-message-line")
		local key = "FN" .. ("12345678901234567890"):sub(i, i)
		if i > 10 then
			key = "S-" .. key
		end
		me.runkey(key)
	end)
	me.bind("^C", "emulate-fkeys")
end

if me.var.os == "MSWIN" then
	me.command("execute-file", "mewin.cmd")
else
	me.command("toggle-fkeys")
end

if me.var.os == "OS2" then
	me.var.sres = "VGA"
	me.var.sres = "CGA"
end

me.var.discmd = true
