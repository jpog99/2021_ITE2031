	lw 0 1 ten     #load reg1 with 10
	lw 0 2 twenty  #load reg2 with 20
	sw 0 1 1       #save reg1 contents to mem1
	sw 0 2 2       #save reg2 contents to mem2
	lw 0 2 1       #load mem1 contents to reg2
	lw 0 1 2       #load mem2 contents to reg1
done halt        #reg1 and reg2 contents swapped at the end
ten .fill 10
twenty .fill 20
