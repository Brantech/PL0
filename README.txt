Syntax

program               ::= block "."

block 		      ::= const-declaration var-declaration procedure-declaration statement

const-declaration     ::= ["const" ident "=" number {"," ident "=" number} ";"]

var-declaration       ::= ["var" ident {"," ident} ";"]

procedure-declaration ::= ["procedure" ident ";" block ";"]

statement             ::= [ident ":=" expression
			   | "call" ident
			   | "begin" statement {";" statement} "end"
			   | "if" condition "then" statement {"else" statement}
			   | "while" condition "do" statement
			   | "read" ident
			   | "write" expression
			   | e]

condition 	      ::= "odd" expression
			  | expression rel-op expression

rel-op 		      ::= "=" | "<>" | "<" | "<=" | ">" | ">="

expression 	      ::= ["+" | "-"] term [{"+" | "-"} term]

term 		      ::= factor [{"*" | "/"} factor]

factor 		      ::= ident | number | "(" expression ")"

number 		      ::= digit {digit}

ident 		      ::= letter {letter | digit}

digit 		      ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

letter 		      ::= "a" | "b" | ... | "y" | "z" | "A" | "B" | ... | "Y" | "Z"