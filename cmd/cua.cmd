; CUA-feel macros for MicroEMACS 4.0
;
; Pierre Perret - April 1993

; Uses mark 12 as CUA anchor and mark14 as a temporary (for mark swapping).
; Uses mark 13 as the other end of the CUA selection (usually shadows the point).
; Uses the Clipboard for CUA copy/cut/paste in MS-Windows version,
; uses the kill buffer for same in other implementations.

; NOTE:
; - some key sequences are not supported by MicroEMACS-DOS:
;     Shift+Arrow, Ctrl+Ins/Del and Shift+Ins/Del/Home/End/PageUp/PageDown.
; - Shift+Ctrl+Arrow/Home/End not supported by MicroEMACS-DOS


store-procedure CUA-cmdhook
; NOTE: unfortunately, %keepanchor is always set to FALSE for
;       M- and ^X prefixed keystrokes, regardless of the binding
    set %discmd $discmd
    set $discmd FALSE
    !if %keepanchor
        set %keepanchor FALSE
    !else
        ; previous command was not an anchor-preserving one
        !force 12 remove-mark
        set $hilight 255
    !endif
    set $discmd %discmd
!endm
set $cmdhook CUA-cmdhook

store-procedure CUA-load
; Loads the anchor (mark12) into the mark0, saving
; the previous mark0 into mark14.
; Loads the selection end (mark13) into the point
; (not saving the previous point is intentionnal, to avoid
;  user confusion when the selection has moved out of the
;  window, due to abuse of the scroll bars)
; The flag %anchor is set if there is a non-empty selection
; Used by most deletion/copying actions
    set %discmd $discmd
    set $discmd FALSE
    !force 0 exchange-point-and-mark
    !if $status
        14 set-mark
        0 exchange-point-and-mark
    !else
        !force 14 remove-mark
    !endif
    !force 12 exchange-point-and-mark
    !if $status
        0 set-mark
        12 exchange-point-and-mark
        !if &seq $region ""
            set %anchor FALSE
        !else
            set %anchor TRUE
            13 goto-mark
        !endif
    !else
        !force 0 remove-mark
        set %anchor FALSE
    !endif
    set $discmd %discmd
!endm

store-procedure CUA-restore
; restores mark0 from mark14 after a CUA-load call
    set %discmd $discmd
    set $discmd FALSE
    !force 14 exchange-point-and-mark
    !if $status
        0 set-mark
        14 exchange-point-and-mark
    !else
        !force 0 remove-mark
    !endif
    set $discmd %discmd
!endm

store-procedure CUA-Del
    run CUA-load
    !if %anchor
        kill-region
    !else
        !force 1 delete-next-character
    !endif
    run CUA-restore
!endm
macro-to-key    CUA-Del     FND

store-procedure CUA-C-Ins
    run CUA-load
    !if %anchor
        set %keepanchor TRUE
        !if &seq $os MSWIN
            clip-region
        !else
            copy-region
        !endif
    !endif
    run CUA-restore
!endm
macro-to-key    CUA-C-Ins	FN^C
!if &seq $os MSWIN
    unbind-menu     ">&Edit>&Clipboard>&Copy region"
    macro-to-menu   CUA-C-Ins	"&Copy@1"
!endif

store-procedure CUA-S-Del
    run CUA-load
    !if %anchor
        !if &seq $os MSWIN
            cut-region
        !else
            kill-region
        !endif
    !endif
    run CUA-restore
!endm
macro-to-key    CUA-S-Del	S-FND
!if &seq $os MSWIN
    unbind-menu     ">&Edit>&Clipboard>Cu&t region"
    macro-to-menu   CUA-S-Del   "Cu&t@0"
!else
bind-to-key     yank            S-FNC   ; Shift+Ins
!endif


store-procedure CUA-case-upper
    run CUA-load
    !if %anchor
        set %keepanchor TRUE
        case-region-upper
    !endif
    run CUA-restore
!endm
macro-to-key    CUA-case-upper  A-U ; Alt+U
!if &seq $os MSWIN
    macro-to-menu   CUA-case-upper  ">&Edit>&Selection@4>&Upper case"
!endif

store-procedure CUA-case-lower
    run CUA-load
    !if %anchor
        set %keepanchor TRUE
        case-region-lower
    !endif
    run CUA-restore
!endm
macro-to-key    CUA-case-lower  A-L ; Alt+L
!if &seq $os MSWIN
    macro-to-menu   CUA-case-lower  "&Lower case"
!endif

store-procedure CUA-count-words
; this procedure is not really necessary but it demonstrates
; how to add CUA-based functionality
    run CUA-load
    !if %anchor
        set %keepanchor TRUE
        count-words
    !endif
    run CUA-restore
!endm
macro-to-key    CUA-count-words A-W ; Alt+W
!if &seq $os MSWIN
    macro-to-menu   CUA-count-words "Count &words"
!endif

store-procedure CUA-flip-selection
; a sort of exchange-point-and-anchor, could be used to
; visualize the selection by going back and forth to its
; boundaries
    set %discmd $discmd
    set $discmd FALSE
    !force 13 goto-mark
    !force 12 exchange-point-and-mark
    !if $status
        13 set-mark
        set %keepanchor TRUE
    !endif
    set $discmd %discmd
!endm
macro-to-key    CUA-flip-selection A-=
!if &seq $os MSWIN
    bind-to-menu    nop "-"
    macro-to-menu   CUA-flip-selection "&Flip"
!endif

store-procedure CUA-select-region
; makes the anchor equal to the mark
; (useful to build very large selections)
    set %discmd $discmd
    set $discmd FALSE
    13 set-mark
    !force 0 exchange-point-and-mark
    !if $status
        12 set-mark
        0 exchange-point-and-mark
        set $hilight 12
        set %keepanchor TRUE
    !else
        12 remove-mark
        set $hilight 255
    !endif
    update-screen
    set $discmd %discmd
!endm
macro-to-key    CUA-select-region A-^M  ; Alt+Enter
!if &seq $os MSWIN
    macro-to-menu   CUA-select-region   "Select &region"
!endif

store-procedure CUA-anchor
; makes sure we have a CUA anchor point (i.e. a mark12)
; used internally by extended selection keys before command processing
    set %discmd $discmd
    set $discmd FALSE
    !force 12 exchange-point-and-mark
    !if $status
        12 exchange-point-and-mark
    !else
        12 set-mark
        13 set-mark
    !endif
    set $discmd %discmd
!endm

store-procedure CUA-select
; sets hilighting of the selection
; used internally by extended selection keys after command processing
; always sets %keepanchor to TRUE and makes sure mark13 shadows the point
    set %discmd $discmd
    set $discmd FALSE
    set %keepanchor TRUE
    13 set-mark
    set $hilight 12
    update-screen
    set $discmd %discmd
!endm

store-procedure CUA-S-home
    run CUA-anchor
    beginning-of-line
    run CUA-select
!endm
macro-to-key    CUA-S-home          S-FN<

store-procedure CUA-S-end
    run CUA-anchor
    end-of-line
    run CUA-select
!endm
macro-to-key    CUA-S-end           S-FN>

store-procedure CUA-SC-home
    run CUA-anchor
    beginning-of-file
    run CUA-select
!endm
macro-to-key    CUA-SC-home         S-FN^<

store-procedure CUA-SC-end
    run CUA-anchor
    end-of-file
    run CUA-select
!endm
macro-to-key    CUA-SC-end          S-FN^>

store-procedure CUA-S-pageup
    run CUA-anchor
    previous-page
    run CUA-select
!endm
macro-to-key    CUA-S-pageup        S-FNZ

store-procedure CUA-S-pagedown
    run CUA-anchor
    next-page
    run CUA-select
!endm
macro-to-key    CUA-S-pagedown      S-FNV

store-procedure CUA-S-up
    run CUA-anchor
    !force previous-line
    run CUA-select
!endm
macro-to-key    CUA-S-up            S-FNP

store-procedure CUA-S-down
    run CUA-anchor
    !force next-line
    run CUA-select
!endm
macro-to-key    CUA-S-down          S-FNN

store-procedure CUA-S-left
    run CUA-anchor
    !force backward-character
    run CUA-select
!endm
macro-to-key    CUA-S-left          S-FNB

store-procedure CUA-S-right
    run CUA-anchor
    !force forward-character
    run CUA-select
!endm
macro-to-key    CUA-S-right         S-FNF

store-procedure CUA-SC-left
    run CUA-anchor
    !force previous-word
    run CUA-select
!endm
macro-to-key    CUA-SC-left         S-FN^B

store-procedure CUA-SC-right
    run CUA-anchor
    !force next-word
    run CUA-select
!endm
macro-to-key    CUA-SC-right        S-FN^F

bind-to-key     beginning-of-file   FN^<
bind-to-key     end-of-file         FN^>
bind-to-key     beginning-of-line   FN<
bind-to-key     end-of-line         FN>

; Transfer the standard MicroEMACS left mouse button commands onto the 
; right button (text scrolling, window resizing, screen move/resize...)
; and the old right button commands to Shift + right button
!force unbind-key  MS1  ; should have been MS^a anyway!
bind-to-key mouse-move-down     MSe     ; right_button_down
bind-to-key mouse-move-up       MSf     ; right_button_up
bind-to-key mouse-resize-screen MS^e    ; Ctrl + right_button_down
bind-to-key mouse-region-down   S-MSe     ; Shift + right_button_down
bind-to-key mouse-region-up     S-MSf     ; Shift + right_button_up

store-procedure MSleft-down
    set %discmd $discmd
    set $discmd FALSE
    !force mouse-move-down
    !if $status
        set %keepanchor TRUE
        12 set-mark
        13 set-mark
        set $hilight 12
        set %msbuf $cbufname
    !else
        set %msbuf ""
        !force 12 remove-mark
    !endif
    set $discmd %discmd
!endm
macro-to-key MSleft-down    MSa

store-procedure MSdrag
    !if &equ $hilight 12
        ; the LEFT button is down
        run CUA-anchor
        !force mouse-move-down
        run CUA-select
    !else
        !force mouse-move
    !endif
!endm
macro-to-key MSdrag    MSm

store-procedure MSleft-up
    set %discmd $discmd
    set $discmd FALSE
    !force mouse-move-down
    !if &and $status &seq %msbuf $cbufname
        set %keepanchor TRUE
    !else
        !force 12 remove-mark
    !endif
    set $discmd %discmd
!endm
macro-to-key MSleft-up      MSb

store-procedure S-MSleft
    run CUA-anchor
    !force mouse-move-down
    run CUA-select  
!endm
macro-to-key S-MSleft       S-MSa
macro-to-key S-MSleft       S-MSb
