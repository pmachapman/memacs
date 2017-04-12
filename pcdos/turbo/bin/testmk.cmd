store-procedure testml
	local %line

	set %line &mkline @"mark number? "

	!if &gre %line 0
		write-message &cat " line #" %line
	!else
		write-message "That mark is not set"
	!endif
!endm
store-procedure testmc
	local %col

	set %col &mkcol @"mark number? "

	!if &gre %col -1
		write-message &cat " col #" %col
	!else
		write-message "That mark is not set"
	!endif
!endm

macro-to-key testml FN^1
macro-to-key testmc FN^2

