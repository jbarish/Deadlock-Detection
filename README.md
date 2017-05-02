This program will figure out if there is a deadlock, given:  
	A Request Matrix   
	An Allocation Matrix   
	An Available Vector   
	
If there is a deadlock, it will try to resolve the deadlock using preemption.  
  
  
	
The program is run with the following command:

```
#!c

       > ./deadlock allocation_matrix_file request_matrix_file "Space_separated_Available_Vector"
```

	
Sample command to run program:
	
```
#!c

       > ./deadlock allocation.txt request.txt "0 1 0 2 1"
```


The program can also be run by typing
	
```
#!c

       > make run
```