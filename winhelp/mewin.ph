!ELSE
!ENDIF
!ENDM
!FORCE
!GOTO
!else
!endif
!endm
" and
" and is displayed in either a
" followed by
" or
" to
". It is displayed as a
">&Search>&Character
"Bloomington"
"C:\WINDOWS
"FALSE"
"beginning"
"previous-line"
"shell
"window
$acount
$asave
$bufhook
$cbflags
$cfname
$cmdhook
$cmode
$curcol
$curwidth
$deskcolor
$diagflag
$dispundo
$exbhook
$fillcol
$flicker
$fmtlead
$gflags
$hardtab
$hilight
$hscrlbar
$mmove
$numwind
$oldcrypt
$orgcol
$orgrow
$overlap
$pagelen
$palette
$paralead
$popflag
$readhook
$replace
$search
$searchpnt
$softtab
$ssave
$sscroll
$status
$sterm
$time
$tpause
$undoflag
$vscrlbar
$wchar
$wraphook
$writehook
$xpos
$yankflag
$ypos
%conditions
%filename
%linenum
%seekstring
%wintyp
&cat
&greater
&group
&indirect
&sequal
's end, the command fails but the point is still moved to the end of the buffer.
's left column on the desktop
's top row on the desktop
(317) 742-5
(Alt+
(Release of
(as
(created by the
(for
(horizontal
(i.e.
(initially: Escape character)
(invoked with the numeric argument
(it is usually the
(its default behavior) and the tab stop interval is reset to its default value of 8.
(light
(like
(or
(or, if that window is not associated to a filename, to the last visited directory).
(see
(starting
(the
(this
(usually
(which
(which is usually the case under Microsoft Windows), the above action is performed by pressing the
(with
) are used in a row, the point is kept at the same column it was at before the first of the line moves. If that column lies beyond the end of the current line the point is temporarily brought back to the end of that line.
) at the time this command is invoked, it is restored to its non-maximized size first.
) command to
) command to copy the text from the kill buffer to the current point.
) command.
) is greater than the number of windows in the screen.
). Also, both strings may have default values (which are stored in the
, MicroEMACS
, allowing you to scroll the text in the current
, are affected. If
, causing
, depending on the value of the
, each successive character leaves the
, giving it
, leaving a copy of
, overwriting any previous clipboard data.
, saving it into the
, tabs stops are reset to every
, the
, this
, thus creating a
. After the command has executed, the point is located just after the last processed word.
. After the insertion, the point
. If
. If the
. If the point was located within a word before invoking the command, that word counts as the first one (thus, if
. Initially set to 8, this determines how far each tab stop is placed from the previous one.
. It
. It contains the following items:
. Otherwise, the file is searched for on disk. If it is found, a new buffer is created and the contents of the file are read into it. If the file does not exist, a new empty buffer is created. In all cases, the buffer is brought up in the current window
. The
. The interpretation of the
. The list is stored in a buffer named "
. They can be used wherever an
. This
. This function is the opposite of
. This is appropriate if, for instance, that file is located in the directory where the MicroEMACS executable resides.
2 delete-previous-character
386-enhanced
= 1.
A line starting with one of the characters in the $
ASAVE
ASCII
Additional commands
After
After this command has executed, the point is left at the beginning of the last affected line. The
Also,
Argument
Attempts to set this variable are ignored.
Bind to Command
Bind to Macro
Binding
Bitwise
Block of Text
Boolean
Bring the
Buffer
CMODE
CRYPT
CUA.CMD
Case Control
Chang
Change
Clipboard
Color
Command
Commands
Control
Controlling Tabs
Copying a Region
Copyright
Count
Creating New Commands
Ctrl
Ctrl,
Current
Customizing Command Keys
Customizing Menus
Cutting and Pasting
DEV.CMD
DOS box
Daniel Lawrence
Default binding
Default binding:
Default bindings:
Default value:
Default value: FALSE
Default value: TRUE
Default value: empty
Default value: the last key bound to
Delete
Describe
Directives
Display
Display a string on the
EMACS
EMACS.HLP
EMACS.RC
EXACT
Each
Edit menu
Encryption
Enter
Environmental
Escape
Execut
Execute
Execute a
Execute a file as a
Execute menu
FALSE
FALSE,
FALSE.
File
File menu
Font
For
Function
Functions
Getting at Files
Global
Help
Help menu
HelpFile
Horizontal scroll
However,
Hunt
If 
If a
If a matching text is found in the
If a negative numeric argument
If a positive
If a positive numeric argument
If a strictly positive
If an attempt to delete past the end or beginning of the
If no
If no numeric argument is specified,
If no numeric argument is specified, all the lines belonging to the current
If the
If the current screen contains only one window, the command fails.
If the item is
If the mouse pointer was
If the move would take the point beyond the boundaries of the buffer, this command fails and the point is left at said boundary.
If the named file is already loaded somewhere in the editor, this
If the numeric argument
If the search fails, a beep sounds and the search stalls until the search string is edited back into something that exists (or until the operation is aborted).
If the specified item
If the window
If there is only one window displayed in the current screen, this command is equivalent to the
If this
If this boolean
If this command is used with a positive numeric argument
If this profile does not appear in the [MicroEMACS] section of the WIN.INI file, the
If used without a
If you are using the
In 
In all cases, even if a numeric argument of 0 is given, the
In interactive mode, th
Incremental search
Index
Initial
Initial value:
InitialSize
Insert
Insert a
Inter
Interactive
Introduction
Issuing Commands
It is unaffected by
Key bindings
Keyboard
Keys
Kill
Killing a Region
Lafayette
Lawrence
Line
List all
List all existing
Lower case
M-^
MAGIC
MDI.CMD
MS-Windows
MS-Windows Specifics
MS-Windows,
MS-Windows:
Macro
Macro Language
Macro language
Macros
Make the current
Manager
Mark
Memory Usage
Menu
MicroEMACS
MicroEMACS command line
MicroEMACS's
MicroEMACS,
MicroEMACS.
MicroEMACS:
Microsoft
Miscellaneous
Miscellaneous menu
Mode
Modes of Operation
Mouse
Mouse commands
Move
Move the
Move the point to the place you want the text to appear.
Move to the
Move to the beginning of the
Move to the end of the current
Move up
Moving
Multipl
Narrowing Your Scope
Next
No default binding
No default binding.
Normal
Note
Note that
Note that end of lines are counted as one character each for the purpose of deletion.
Note:
Notice that the MicroEMACS usage of the word window is different from the meaning used in window-based systems:
Number of
Numeric
OVER
Operati
Operating
Operating System
Other
Otherwise,
Paragraph
Pasting
Perret
Pierre Perret
Popup Buffer
Position the
Positioning
Pressing
Previous
Procedure
Profile
Programmer's
Read
Region
Repeat steps 5 and 6 to insert more copies of the same text.
Repetitive Tasks
Replace
Reposition
Returns
Returns TRUE if
S-MS
Save
Screen
Screen menu
Scroll the current
Scroll the next
Scrolling Text Inside a Window
Search
Search menu
Searching and Replacing
See also:
Select
Set the
Setting
Setting th
Setting this variable does not have any effect on the editor beyond changing the variable's value.
Setting this variable is equivalent to
Shell
ShellExecOption
Shift
Size
Standard commands
Start
Store the following
Syntax:
System
TRUE
TRUE,
TRUE.
Text
That
The
The Outside World
The Programmer's Room
The characters composing the search string are always interpreted literally.
The command
The command fails if
The command fails if the
The current
The default
The director
The distance which a true tab character moves the cursor is reflected by the value of the
The following
The list is actually built in a special buffer named "
The placement of the point after the execution of this command is determined by the value of the
The value of this
There
These
They
This
This boolean
This command
This command cannot be used to
This command does not modify the modes/colors of the current buffer/window. To do so, use the
This command is unaffected by
This is a dummy
TimeSlice
To change the
To change the size of the current window by specifying an absolute value, use the
To restore the current screen to the size and position it had before invoking this command, use the
To set the default modes/colors for all future buffers/windows, use the
Transforms
Translate the first char in
Unde
Under M
Under MS-Windows
Under MS-Windows, the value of this variable is irrelevant.
Under Windows
Under some operating systems, th
Under the MS-Windows version of MicroEMACS, the
Undoing Your Edits
Unless
Upper case
Use the
Using
Using the Mouse
Using this command is equivalent to setting the
Usually
VIEW
Variable
WIN.INI
WRAP
When
When line move commands (
When th
While
Window
Window submenu
Windows
Windows),
Windows,
Windows.
With an non-zero argument
With no argument,
Word
You
[MicroEMACS]
^X^
abort
abort-command
aborts the operation. Pressing the Backspace key (or using
about
above
absent,
absolute
access
accessed
action
activate
active
acts
actual
actually
add-global-mode
add-mode
added
addition
affected
after
after the
all
allow
allowing
allows
along
alphabetic
already
also
also:
alter
always
amount
ampersand
and
and the
animat
another
any
appear
append-file
appends
application
appropriate
apropos
are
are affected. If no region is defined, the command modifies no line.
are:
argument
argument is a string. Note that if it contains spaces (as would be necessary to specify command line options), the string should be quoted.
arguments
around
arranges all non-iconic
arrow
aspects
associat
associated
at the
attempt
attribute
author
auto-save
automatic
automatically
available
back
background
backlash
backward
backward-character
basic
become
becomes the value of the
been
before
begin
begin-macro
beginning
beginning-of-file
beginning-of-line
behave
behavior
being
belonging
below
between
beyond
bind-to-key
bind-to-menu
binding
binding.
binding:
bindings
black
blank
boolean
both
bottom
bound
boundar
box
bracket
bring
brings
brings up a dialog box to change the
brings up this help file, on the
brought
buffer
buffer's
buffer,
buffer-position
buffer.
buffer/window.
buffers
buffers/windows,
built
but
button
button.
button:
calle
can
cannot
capabilit
cascade-screens
case
case-region-lower
case-region-upper
case-word-capitalize
case-word-lower
case-word-upper
cases,
cause
causes
causes the
causes the current
causes the equivalent of a
causes the next occurrence of the string to be searched for (where the next occurrence does not overlap the current occurrence). A
causes the specified
causing
center
change
change-file-name
change-screen-column
change-screen-row
change-screen-size
change-screen-width
changed
changes
changing
character
character.
characters
check
clear-and-redraw
clear-message-line
click
clip-region
clipboard
closing
color
colors
column
columns
combination
command
command had been invoked with an argument of
command had been invoked with the corresponding positive number (-
command had been invoked, with a numeric argument of -
command is invoked
command line
command name
command to be performed on the
command was invoked
command was issued. The interpretation of the search string is dependant on whether
command with a
command)
command), pressing the
command). It can also be used to specify the foreground or background
command,
command, the search happens and the display is updated as each new search character is typed.
command.
command:
commands
commands.
commands:
comment
complet
complex
compose
computer
confirmation
considered
consist
constant
construct
contact
contain
containing
contains
contains the
contains the current
contains the fi
contains the following items:
contains the name of
contains the number of
contains the number of lines displayed in the current
contains the position of the current
contents
control
converts all the selected text to
coordinate where the mouse was located the last time a
coordinates
copied
copies
copies the contents of the current
copy
copy-region
corner
corresponding
count
count-words
counterpart.
created
creates
creates a list of all the
creates a menu item
creating
ctlx-prefix
current
currently
cursor
customiz
cut-region
cycle-ring
cycle-screens
decimal
decreas
default
define
defined
delete
delete-blank-lines
delete-buffer
delete-global-mode
delete-kill-ring
delete-mode
delete-next-character
delete-next-word
delete-other-windows
delete-previous-character
delete-previous-word
delete-screen
delete-window
deleted
deletes
deletes a menu item
deletes the text from the
deletes, and stores into the
deleting
deletion
dependant
depending
depends
depends on the
depressed
descri
describe-binding
describe-functions
describe-key
describe-variables
designates a menu item that already exists, the command fails.
desktop
dest
detab-region
determine
development
diagonal
dialog
different
digit
direction
directive
directly
directories
directory
disabled
discard
disk
disk.
display
displayed
displaying
displays
displays, on the
distributed
document
does
done
double
down
down.
drag
dragged
dragging
drop
dropped
during
each
edit
edited
editing
editor
effect
effect.
effects
either
elements
eliminate
emacs.rc
empty
enabled
enclose
encryption
end
end of lines count as one character.
end-macro
end-of-file
end-of-line
end-of-word
entab-region
enter
entire
environment
environmental
equal
equivalent
error
evaluate
even
every
example,
example:
examples
exchange-point-and-mark
exchanges the
executable
execute
execute-buffer
execute-command-line
execute-file
execute-macro
execute-named-command
execute-procedure
execute-program
executed
executes
executes is set to the directory of the file in the current
executes the
executing
execution
exist
exist,
existing
exists
exit-emacs
explanation...
expression
exten
extensive
external
fails
fails.
feature
fence
file
file name
file,
file.
filename
files
files.
fill
fill-paragraph
filter-buffer
find-file
find-screen
first
flag
follow
followed
following
font
for
for a more practical explanation...
for future
for the current
force
foreground
form
formatter
forward
forward-character
found
frame
from
from the
front
function
functions
future
genera
get
given
global
goto-line
goto-mark
goto-matching-fence
graphic
greater
green
grinder
group
grow-window
had
handle-tab
handles
happen
happens.
has
has two different behaviors, depending on the presence or absence of a
have
height
help
help-engine
highlight
hold
holding
horizontal
how
hunt-backward
hunt-forward
i-shell
iconize
ignored
ignored.
immediately
implement
in sufficient number to get to the next tab stop. This also sets the
in the current
include
incorporate
incremental
incremental-search
incrementally
indent-region
indentation
indented
independent
index
indicate
information
initial
input
insert
insert-clip
insert-file
insert-space
insert-string
inserted
insertion,
inserts
instance
instance,
instead
integer
inter
interactive
interpretation
interpreted
interval
intervening
into
into the
into the kill buffer.
invisible
invocation
invoke
invoked
invokes the
invoking
irrelevant
is 
is 0, nothing happens.
is 1, the point moves to the first character
is TRUE
is a
is a negative number, this command acts as if the
is a positive number, this
is absent, it is taken as 1.
is accessed via the
is always interactive. It prompts the user for  a search string but, unlike what happens with the
is available only under Microsoft Windows. It causes
is available only under Microsoft Windows. It creates a menu item associated with the specified
is dependant on whether
is distributed with MicroEMACS for Windows and loaded by the
is e
is lo
is loaded, the menu is modified by the
is made the current one (window numbering starts at 1).
is made, the command fails.
is marked as modified, even if no modification actually took place.
is meant to be associated with a mouse action. It depends on the
is moved to the first character
is negative or if it goes beyond the end of the buffer. If
is negative or if it is positive but would cause the
is negative or null, nothing happens.
is negative or null, the command has no effect.
is negative)
is negative, it causes the command to behave like
is negative, next-paragraph behaves as if the
is negative, the
is negative, the command fails.
is negative, the point is moved to the
is negative.
is not
is not defined in the current
is not specified, it is taken
is not specified, on
is not specified, the
is null, nothing happens. If the numeric argument is not specified, only one word is affected.
is null, the command has no effect.
is null, the command modifies no line.
is positive,
is present and
is present,
is present, the buffer is marked as being invisible (hidden from the
is reconfigured so that it inserts
is reconfigured to insert
is specified using the
is specified,
is specified, all the matching pieces of text are processed until the end of the buffer is reached. If a positive numeric argument is used, only the first
is specified, it is equivalent to
is specified, or if the numeric argument is null, it is equivalent to
is specified, the
is specified, the text is scrolled by one line.
is specified, the window's view into it's buffer is moved
is specified, this
is used
issue
it's
it.
item
item index
item text
items
items:
its
itself
jump
just
key
key and the
key)
key,
keyboard
keyboard macro
keys
keystroke
keystroke syntax
kill
kill buffer
kill ring
kill-paragraph
kill-region
kill-to-end-of-line
label
language
large
last
later
launch
leaving
left
length
let
letter
level
like
limit
line
line,
line.
lines
lines, starting from the one containing the
lines.
lines. The window located immediately below the current window (or, if the current window is at the bottom of the
list
list-buffers
list-screens
list-undos
listed
literally
loaded
loaded,
located
located just below the current one (or the top window if the current one is at the bottom of the
locking
log1   log2
look
looking
looks
lower
macro
macro language
macro-to-key
macro-to-menu
macros
made
magenta
make
makes
makes the
manipulate
many
mark
marked
match
matched
matches
matching
maximize-screen
maximized
may
meaning
means
meant
meant to be used in combination with the
mechanism
memory
menu
menu name
menu name syntax
menu.
menus
message
message line
messages
meta key
meta-prefix
method
milliseconds
minimize-screen
mode
mode has no effect on incremental searches.
mode is enabled in the current
mode is set or not in the current
mode line
mode,
mode.
modes
modes/colors
modification
modified
modifies
modifies th
modifies the offset of the current
modify
monochrome
more
most
mouse
mouse button
mouse button instead of the left one.
mouse button together instead of just the right mouse button.
mouse syntax
mouse-move
mouse-move-down
mouse-move-up
mouse-region-down
mouse-region-up
mouse-resize-screen
move
move-window-down
move-window-up
moved
movement
moves
moves the
moving
multipl
must
name
name-buffer
name.
named
named in this
names
narrow-to-region
narrowed
necessary
need
needed
needs
negative
negative,
negative.
new
new name
newline
newline characters at the
newline-and-indent
next
next-buffer
next-line
next-page
next-paragraph
next-window
next-word
non-iconic
non-maximized
non-zero
nop
normal
normally
not
nothing
null,
num
num1
num1   num2
num2
number
number,
numbered
numeric
numeric argument
numeric arguments
occupie
occurred
occurrence
of the
offset
often
on that screen. The list is stored in a buffer named "
on the
one
one.
only
onward, each piece of text that matches the
open-file
open-line
opening
operate
operating
operation
opposite
options
or 
or a
or a regular
or in a normal
or the
or:
order
original
other
out
output
outside
over
overlap
override
overwrite-string
overwriting
package
page
palette
paragraph
parenthesis
part
particular
parts
past
path
path name of a PIF file for
path name of th
pattern
pause
perform
performed
performs
piece
pipe-command
place
placed
placement
places
point
point,
pointer
pop-buffer
popup
popup buffer
port
portion
position
positive
possible
pound
practic
precede
preceding
prefix
prese
presen
present,
press
pressing
prevent
previous
previous-line
previous-page
previous-paragraph
previous-window
previous-word
print
procedure
process
processed
processing
processor
profile
program
prompt
prompts
proper
purpose
querie
query-replace-string
quick-exit
quote
quote-character
rather
reache
read
read-file
rearranged
rebind
recompiling.
reconfigured
recorded
recording
redefine
redraw-display
reference
regardless
region
regular
relat
release
remain
remove
remove-mark
rename-screen
repetitive
replace
replace-string
replaced
replacement
replaces
replacing
repositioned
represent
require
reset
resides
resize-window
resized
resizing
resolution
restor
restore-screen
restore-window
result
return
returned
reverse-incremental-search
right
ring
run
run-makefile
running
same
sample
save
save-file
save-window
saved
saves
saving
scheme
screen
screen name
screen,
screen.
screens
scroll
scroll-next-down
scroll-next-up
scrolled
scrolling
scrolls out of view, it is repositioned on the first character of the line located at the center of the window.
search
search string
search-forward
search-reverse
searched
searches
searches backwards for the
searches forward for the
searching
section
select
select-buffer
selected
selection
semicolon
separat
sequence
set
set %
set %filename #list
set-encryption-key
set-mark
sets
setting
shell
shell-command
should
show-files
shrink
shrink-window
sign
significant.
similar
simple
simply
simultaneous
since
single
size
small
so that
software
some
something
somewhe
sound
source
space
space characters
spaces
spawns
special
specific
specifie
specified
specified,
specifies menus that do not exist yet, they are created as part of the creation of the menu item.
specifies that offset in number of characters. If
specifies the
specifies the path name of
specify
specifying
split-current-window
standard
starname
start
start-up
start-up file
started
starting
starts
startup
statement
statements from the specified
still
stop
stops
storage
store-macro
store-procedure
stored
stores the commands and
str
str1   str2
str2
strictly
strike
striking
string
string by the
string is interpreted literally, unless
string is specified by the
string,
string, striking
string.
strings
submenu
succe
such
sufficient
supplied
support
surrounded
switch
synchroniz
syntax
system
systems,
systems:
tab
tab characters in front of each line within the current
table
tabs
take
taken
takes
target
task
temporarily
temporary
terminat
terminate
terminates the search and
text
text,
text.
th occurrence of the
th occurrence of the search string. That search string is the one that was used the last time a
than
that
that follow it, up to the next
the
the current selection
the frame window fills the whole display
the number of words, characters and lines that compose the selected text
the preceding character (or "
their
them
them.
then
there
there with the
these
they
this
those
three
through
thus
tilde
tile-screens
tiling
time
time.
times. If
to 
to a
to a menu. That is performed by the
to be
to be changed into their
to indicate the new current window, and places the blinking cursor at the
to the
to the Windows
to the beginning
to the end of the text.
to the first char
together
too
top
topic
towards
trailing
trans
transpose-characters
trim-region
true tab characters
truncated
two
type
typed
typing
unaffected
unbind-key
unbind-menu
undent-region
under
underscor
undo
universal
universal-argument
unless
unmark-buffer
until
up in the current
update-screen
updated
updates
upper
usage
use
used
used.
useful
user
user variable
uses
using
usually
value
value.
value:
values
variable
variable.
variables
variables). If you want to replace a string with nothing, and there is a non-empty default for the
various
version
vertical
very
via
view
view into it's
view-file
virtual
visible
visited
waiting
want
was
was pressed or released.
way
were
what
when
whenever
where
wherever
whether
which
which are specific to a particular file or file type.
while
white
whole
whose name contains the s
widen-from-region
width
will
will override that default and enter an empty string instead.
window
window's
window,
window-
window.
windows
with
with the other end of the selection
with, for each
within
within that window.
within the
without
word
word.
words
working
works
would
wrap-word
wrapping
write
write-file
write-message
writes the contents of the current
writing
written
yank
yank-pop
you
your
zero
zero,
zero.
