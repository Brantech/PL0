/*
 * Brandon Gotay - br109653 - 3858803
 * COP 3402-1 Fall 2017
 * 11/18/2017
 */

#ifndef HEADER
#define HEADER

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
#define MAX_IDENTIFIER_LENGTH 11
#define MAX_NUMBER_LENGTH 5
#define NUMBER_OF_REGISTERS 8

// Global input/output
extern FILE *fin, *fout;

// Instructions formatted as OP R L M
typedef struct instruction
{
    int OP;
    int R;
    int L;
    int M;
} instruction;

// Stores information about each lexeme
typedef struct lexeme
{
    char *lexeme; // Lexeme name
    int value; // Value if lexeme is a number
    short type; // Type of lexeme
    int line; // Line lexeme is on
} lexeme;

// Registers used by the virtual machine
typedef struct Registers
{
    int BP; // Base pointer register
    int SP; // Stack pointer register
    int PC; // Program counter register
    instruction IR; // Instruction register
    int R[NUMBER_OF_REGISTERS]; // Registers for use by user
} Registers;

// Stores information about each symbol
typedef struct symbol
{
    int kind;
    char *name; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int scope;
} symbol;

typedef enum
{
    nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym, whilesym, dosym, callsym, constsym, varsym, procsym, writesym, readsym, elsesym
} token_type;

typedef enum
{
    LIT_Code = 1, RTN_Code, LOD_Code, STO_Code, CAL_Code, INC_Code, JMP_Code, JPC_Code, SIO_Code, NEG_Code, ADD_Code, SUB_Code, MUL_Code, DIV_Code, ODD_Code, MOD_Code, EQL_Code, NEQ_Code, LSS_Code, LEQ_Code, GTR_Code, GEQ_Code
} OP_Code;

typedef enum
{
    constant = 1, variable, proccess
} symbol_kind;

// Global registers
extern Registers Reg;
extern int stack[MAX_STACK_HEIGHT];
extern instruction code[MAX_CODE_LENGTH];
extern short HALT;

// Program lexemes
extern lexeme *lexeme_list;
extern int lexeme_count;

// Compiler directives
short l, a, v;

// vm.c function prototypes
void vm();
void fetch();
void execute();
char *funcName(int OP);
void print(FILE *fout);
int bar(int L, int BP);

// isa.c function prototypes
int base(int L, int B);
void LIT();
void RTN();
void LOD();
void STO();
void CAL();
void INC();
void JMP();
void JPC();
void SIO();
void NEG();
void ADD();
void SUB();
void MUL();
void DIV();
void ODD();
void MOD();
void EQL();
void NEQ();
void LSS();
void LEQ();
void GTR();
void GEQ();

// analyzer.c function prototypes
void analyzer();
void finish_word(char *code, char **temp, int i, int len, int lex_len);
void init();
int lex_type(char *temp, int len);
short optype(char *temp, int len);
void parse(char *code, int len);
short special(char c);

// compiler.c function prototypes
int directives(char *arg);
void quit();

// parser.c function prototypes
void parser();
void program();
void block(int scope);
void const_dec(int scope);
void var_dec(int address, int scope);
void proc_dec(int address, int scope);
void statement(int address, int scope);
void condition(int scope);
void expression(int scope);
void term(int scope);
void factor(int scope);
void error(int error);
int check_error(int valid_symbol, int errnum);
int relOP(int opr);
void get_token();
int scopeCheck(int index, int scope);
int lookup(char *name, int scope);
void addSymbol(int kind, char *name, int val, int level, int addr, int scope);
void emit(int op, int r, int l, int m);

#endif
