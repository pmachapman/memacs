@begin(verbatim, font times8)
@center(@b[Default Key Bindings for MicroEmacs 4.0])

 ^A   Move to start of line	      ESC A   Apropos (list some commands)
 ^B   Move backward by characters     ESC B   Backup by words
 ^C   Insert space		      ESC C   Initial capitalize word
 ^D   Forward delete		      ESC D   Delete forward word
 ^E   Goto end of line		      ESC E   Reset Encryption Key
 ^F   Move forward by characters      ESC F   Advance by words
 ^G   Abort out of things	      ESC G   Go to a line
 ^H   Backward delete		      
 ^I   Insert tab/Set tab stops
 ^J   Insert <NL>, then indent		    
 ^K   Kill forward		      ESC K   Bind Key to function
 ^L   Refresh the screen	      ESC L   Lower case word
 ^M   Insert <NL>		      ESC M   Add global mode
 ^N   Move forward by lines	      ESC N   Goto End paragraph
 ^O   Open up a blank line	      
 ^P   Move backward by lines	      ESC P   Goto Begining of paragraph
 ^Q   Insert literal		      ESC Q   Fill current paragraph
 ^R   Search backwards		      ESC R   Search and replace
 ^S   Search forward		      ESC S   Suspend (BSD only)
 ^T   Transpose characters		       
 ^U   Repeat command four times       ESC U   Upper case word
 ^V   Move forward by pages	      ESC V   Move backward by pages
 ^W   Kill region		      ESC W   Copy region to kill buffer
 ^Y   Yank back from killbuffer       ESC X   Execute named command
 ^Z   Move backward by pages	      ESC Z   Save all buffers and exit
 ^_   Undo last editing operation

 ESC ^C   Count words in region       ESC ~   Unmark current buffer
 ESC ^E   Execute named procedure
 ESC ^F   Goto matching fence	      ESC !   Reposition window
 ESC ^H   Delete backward word	      ESC <   Move to start of buffer
 ESC ^K   Unbind Key from function    ESC >   Move to end of buffer
 ESC ^L   Reposition window	      ESC .   Set mark
 ESC ^M   Delete global mode	      ESC space    Set mark
 ESC ^N   Rename current buffer       ESC rubout   Delete backward word
 ESC ^R   Search & replace w/query	  rubout   Backward delete
 ESC ^S   Source command file
 ESC ^U   Delete undo information
 ESC ^V   Scroll next window down
 ESC ^W   Delete Paragraph
 ESC ^X   Execute command line
 ESC ^Z   Scroll next window up

 ^X <	Narrow-to-region	   ^X ?   Describe a key
 ^X >	Widen-from-region	   ^X !   Run 1 command in a shell
 ^X =	Show the cursor position   ^X @@   Pipe shell command to buffer
 ^X ^	Enlarge display window	   ^X #   Filter buffer thru shell filter
 ^X 0	Delete current window	   ^X $   Execute an external program
 ^X 1	Delete other windows	   ^X (   Begin macro
 ^X 2	Split current window	   ^X )   End macro
				   ^X A   Set variable value
 ^X ^B	 Display buffer list	   ^X B   Switch a window to a buffer
 ^X ^C	 Exit MicroEMACS	   ^X C   Start a new command processor
 ^X ^D	 Detab line		   ^X D   Suspend MicroEMACS (UNIX only)
 ^X ^E	 Entab line		   ^X E   Execute macro
 ^X ^F	 Find file		   
 ^X ^I	 Insert file
				   ^X K   Delete buffer
 ^X ^L	 Lower case region
 ^X ^M	 Delete Mode		   ^X M   Add a mode
 ^X ^N	 Move window down	   ^X N   Rename current filename
 ^X ^O	 Delete blank lines	   ^X O   Move to the next window
 ^X ^P	 Move window up 	   ^X P   Move to the previous window
 ^X ^R	 Get a file from disk	   ^X R   Incremental reverse search
 ^X ^S	 Save current file	   ^X S   Incremental forward search
 ^X ^T	 Trim line		   
 ^X ^U	 Upper case region	   ^X U   list undo information
 ^X ^V	 View file
 ^X ^W	 Write a file to disk	   ^X W   resize Window
 ^X ^X	 Swap "." and mark	   ^X X   Use next buffer
 ^X ^Z	 Shrink window		   ^X Z   Enlarge display window

@u(Usable Modes)
ABBREV   Expand defined abbreviations while typing
WRAP	 Lines going past right margin "wrap" to a new line
VIEW	 Read-Only mode where no modifications are allowed
CMODE	 Change behavior of some commands to work better with C
EXACT	 Exact case matching on search strings
OVER	 Overwrite typed characters instead of inserting them
CRYPT	 Current buffer will be encrypted on write, decrypted on read
MAGIC	 Use regular expression matching in searches
ASAVE	 Save the file every 256 inserted characters
SPELL	 Invoke MicroSPELL to check for spelling errors
REP	 Similar to OVER, handles double-byte characters and tabs differently

WHITE/CYAN/MAGENTA/YELLOW/BLUE/RED/GREEN/BLACK/GREY/GRAY/LRED/LGREEN/LYELLO/LBLUE/LMAGENTA/LCYAN	Sets foreground color
white/cyan/magenta/yellow/blue/red/green/black/grey/gray/lred/lgreen/lyello/lblue/lmagenta/lcyan	Sets background color
@end(verbatim)

