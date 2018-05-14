Input:
	File containing the program written in the PL0 language
	Optional output file name
	Optional compiler directives -l -a -v
	Input asked for by the program after compilation
Output:
	-l will print the lexicographic list
	-a print assembly code
	-v print stack trace from virtural machine

	These will be printed to the output file if one was specified or log.txt if there was not.
	Errors will be printed to the screen and the output file. 
	Output from program after execution
Compiling:
	gcc -o compiler analyzer.c compiler.c ISA.c parser.c VM.c
	
	== OR ==
	
	run build.cmd if on windows
Operation:
	compiler.exe <input file path> <output file path> <op 1> <op 2> <op 3>