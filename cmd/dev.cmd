; More example macros for MicroEMACS 3.11c for Windows
; These are geared for Windows application development

; Pierre Perret - July 1992

; the following variables need to be adjusted to point to the
; proper files. The pathnames may not be appropriate on your
; system
set %sdk30hlp "c:\sdk30\bin\sdkwin.hlp"
set %sdk31hlp "c:\c700\bin\win31wh.hlp"
set %sdk32hlp "d:\mstools\bin\api32wh.hlp"
set %turbohlp "c:\bc30\bin\tcwhelp.hlp"

; set this variable to point to the make command you want to be used
set %make "nmake /f makefile"

; utility macro to isolate the current help topic
; note that it modifies mark0 and mark7
store-procedure get-help-keyword
    7   set-mark
    forward-character
    !force previous-word
    0   set-mark
    end-of-word
    set %Keyword $region
    7   goto-mark
!endm

; This macro calls up the Win 3.0 SDK help on the curent topic
; it is complicated because it needs to append the type of the topic
; for proper matching in the help file.
store-procedure Win30-Help
    run get-help-keyword
    !if &sequal %Keyword &upper %Keyword
        ; keyword is all uppercase
        set %tmp &sindex %Keyword "_"
        !if &not &equal %tmp 0
            ; there is an underscore in there...
            !if &or &equal %tmp 3 &sequal &mid %Keyword 2 3 "BN_"
                ; the underscore is the 3rd character or
                ; the second to 4th characters are "BN_"
                set %KeywordType " message"
            !else
                set %tmp 0
            !endif
        !endif
        !if &equal %tmp 0
*GetAdvice
            write-message "What is it ? (S=data Structure, M=utility Macro, P=Printer escape)"
            set %tmp &upper &gtk
            !if &equal &sindex "SMP" %tmp 
                !goto GetAdvice
            !endif
            !if &sequal %tmp "S"
                set %KeywordType " data structure"
            !else
                !if &sequal %tmp "M"
                    set %KeywordType " utility macro"
                !else
                    set %KeywordType " printer escape"
                !endif
            !endif
        !endif
    !else
    ; keyword contains lower case characters
        set %KeywordType " function"
    !endif
    write-message &cat "Looking for: " &cat %Keyword %KeywordType
    help-engine %sdk30hlp &cat %Keyword %KeywordType
!endm

; This macro calls up the Win 3.1 help on the curent topic
store-procedure Win31-Help
    run get-help-keyword
    write-message &cat "Looking for: " &cat %Keyword
    help-engine %sdk31hlp &cat %Keyword
!endm

; This macro calls up the Win32 help on the curent topic
store-procedure Win32-Help
    run get-help-keyword
    write-message &cat "Looking for: " &cat %Keyword
    help-engine %sdk32hlp &cat %Keyword
!endm

; This macro calls up the Turbo C for Windows help on the curent topic
store-procedure TCW-Help
    run get-help-keyword
    write-message &cat "Looking for: " &cat %Keyword
    help-engine %turbohlp &cat %Keyword
!endm

; add the SDK help menu items at position 5 in the Help menu
; ("@" positions start at 0)
macro-to-menu   Win30-Help    ">&Help>Windows 3.&0 API@5"
macro-to-menu   Win31-Help      "Windows 3.&1 API"
macro-to-menu   Win32-Help      "Windows 3&2 API"
macro-to-menu   TCW-Help        "&Turbo C++ help"
; follow by a separator (note that nop is used as a placeholder)
bind-to-menu    nop             "-"

;------------------------------------------------------

store-procedure run-makefile
    find-screen "Results"
    pipe-command %make
    set $cbufname "Results"
!endm

bind-to-menu nop ">E&xecute>-"
macro-to-menu run-makefile "&Make"
