;	Example startup program for Micro-EMACS 4.00 for Windows
;
; Pierre Perret - April 1993

set $mmove 1            ; do not generate too many mouse events
set $posflag TRUE       ; show line positionning info in modeline
set $flicker FALSE	; This animates the grinder cursor

store-procedure save-all-buffers
; This macro saves all the modified buffers to their respective
; files
    set %tmp $cbufname
    !while TRUE
        !force save-file
        !force next-buffer
        !if &seq %tmp $cbufname
            !return
        !endif
    !endwhile
!endm
macro-to-key save-all-buffers M-^Z  ; bind to ESC Ctrl+Z

macro-to-menu save-all-buffers ">&File>Save a&ll@8"

; setup some cool colors
add-mode blue          ; background
add-global-mode blue
add-mode WHITE          ; foreground
add-global-mode WHITE

; resize the screen for 100 columns (not appropriate for pure VGA)
; 100  change-screen-width

; this macro allows easy resizing of the current screen
store-procedure Set-Screen-Size
    set %tmp @&cat &cat "Screen width (" $curwidth "): "
    !if &not &sequal %tmp ""
        set $curwidth %tmp
    !endif
    set %tmp @&cat &cat "Screen height (" $pagelen "): "
    !if &not &sequal %tmp ""
        set $pagelen %tmp
    !endif
!endm

; this macro allows easy setting of the fill column
store-procedure  Set-Fill-Column
    set %tmp @&cat &cat "Fill column (" $fillcol "): "
    !if &not &sequal %tmp ""
        set $fillcol %tmp
    !endif
!endm

; get rid of some menu entries
; to replace them by a nicer macro that actually prompts the user
unbind-menu     ">S&creen>Si&ze>= &Width"
unbind-menu     "= &Height"
macro-to-menu   Set-Screen-Size "&Set:"
;unbind-menu     ">&Edit>= &Fill column"
macro-to-menu   Set-Fill-Column "&Fill column:"

; load Windows-specific macros
write-message "[Loading MDI macros...]"
execute-file "mdi.cmd"

; uncomment the following lines to load the sample development macros
; write-message "[loading DEV macros...]"
; execute-file "dev.cmd"

write-message "[Loading CUA-feel macros...]"
execute-file "cua.cmd"

!if &not &seq &find "custom.cmd" ""
    write-message "[loading CUSTOM macros...]
    execute-file "custom.cmd"
!endif
write-message "[Still setting up...]"

store-procedure	extension
; figure out the filename extension. returns lowercase result in %ext
; if no extension, returns ""
    set %tmp &rig $cfname 4
    set %ext &sin %tmp "."
    !if &equ %ext 0
        set %ext ""
    !else
        set %ext &low &mid %tmp &add %ext 1 3
    !endif
!endm

store-procedure readhook-proc
    run extension
    !if &sin "|c|h|cpp|hpp|dlg|def|rc|cmd|" &cat &cat "|" %ext "|"
        add-mode cmode
    !else
        !if &sin "||me|1st|doc|txt|" &cat &cat "|" %ext "|"
            add-mode wrap
        !endif
    !endif
!endm
set $readhook readhook-proc

set $ssave FALSE

;4	handle-tab		;Pierre's preferences!
;bind-to-key end-of-word M-F
;bind-to-key end-of-word FN^F
clear-message-line
