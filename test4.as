  noop          #do nothing
	lw 0 1 num1   #load 10 in reg1
	lw 0 2 num2   #load 20 in reg2
	nor 1 2 3     #save ~(10|20) in reg3
	nor 1 1 4     #save ~(10|10) in reg4
done halt 
num1 .fill 10 
num2 .fill 20 
