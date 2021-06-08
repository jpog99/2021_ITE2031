	lw 0 1 num1    #load 10 into reg1
	lw 0 2 num2    #load 20 into reg2
	lw 0 4 end    #load done address into reg4
	beq 1 2 sub    #if reg1==reg2, jump to sub
	add 1 2 3      #add 10 and 20, store in reg3
done	sw 0 3 0   #store reg3 to mem0
	halt           #This program calculates mem0 = (num1==num2)? num1-10: num1+num2
sub lw 0 5 neg10 #load -10 into reg2
	add 1 5 3      #reg1-10 into reg3
	jalr 4 7       #jump to done
num1 .fill 10
num2 .fill 20
neg10 .fill -10
end .fill done
