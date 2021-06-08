	lw 0 1 num1    #load 3 in reg1
	lw 0 2 neg1    #load -1 in reg2
	lw 0 3 addr    #load pow address in reg3
	lw 0 4 num1    #load 3 in reg4
	jalr 3 6       #save next inst address in reg6, jump to reg3 contents (pow)
	sw 0 5 0       #save reg5 in mem0
	halt           #*This program saves (num1)^2 to mem0 at final state* 
pow	add 5 1 5    #saves reg5+reg1 to reg5
	add 4 2 4      #decrement reg4 by -1
	beq 0 4 done   #if reg4 equals 0, jump to done
	beq 0 0 pow    #if not, loop back to pow
done	jalr 6 7   #save next inst address in reg7, jump to reg6 content
num1 .fill 5
neg1 .fill -1   
addr .fill pow
