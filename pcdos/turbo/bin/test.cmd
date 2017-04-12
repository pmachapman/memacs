store-procedure nest1
	local %avar

	set %avar "nest1"
	insert-string &cat %avar " "
	nest2 &add 5 2 seventy
	insert-string &cat %avar " "
!endm

store-procedure nest2 %apar %bpar
	local %avar

	set %avar "nest2"
	insert-string &cat &cat &cat %apar %bpar "|" &cat %avar " "
	nest3
	insert-string &cat %avar " "
!endm

store-procedure nest3
	local %avar

	set %avar "nest3"
	insert-string &cat %avar " "
	nest4
	insert-string &cat %avar " "
!endm

store-procedure nest4
	local %avar

	set %avar "nest4"
	insert-string &cat %avar " "
	describe-variables
!endm

local %avar
set %avar "in test.cmd"
insert-string &cat %avar " "
nest1
insert-string &cat %avar " "




