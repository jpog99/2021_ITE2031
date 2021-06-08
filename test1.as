			lw 0 1 five    #load 5 in reg1
			lw 0 2 three   #load 3 in reg2
			lw 0 3 neg1    #load -1 in reg3
start	add 4 1 4      #add 5 to reg4 contents
			add 2 3 2      #decrement reg2
			beq 0 2 done   #if reg2==0 then go to done
			beq 0 0 start  #else loop back to start
			noop           
done halt
five .fill 5
three .fill 3
neg1 .fill -1
stAddr .fill start   #this program will multiply reg1 and reg2, result stored in reg4.
