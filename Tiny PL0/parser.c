/*
 * Brandon Gotay - br109653 - 3858803
 * COP 3402-1 Fall 2017
 * 11/18/2017
 */

#include "core.h"
#include <string.h>

char *name;
int token = nulsym, num, line;

int lx = 0; // Lexeme table index
int sx = 0; // Symbol table index
int cx = 0; // Code index

int level = 0;
int scopeCount = 0;

symbol table[1000];
int scopes[1000];
int _register = 0;

void parser()
{
    program();
    if(HALT) return;
    cx--;

    if(a)
    {
        fprintf(fout, "No errors, program is syntactically correct\n");
        printf("\nNo errors, program is syntactically correct\nExecuting:\n");

        fprintf(fout, "\nAssembly Code\n");

        int i;
        for(i = 0; i < cx; i++)
        {
            fprintf(fout, "%s %d %d %d\n", funcName(code[i].OP), code[i].R, code[i].L, code[i].M);
        }
        fprintf(fout, "%s %d %d %d\n\n", funcName(code[i].OP), code[i].R, code[i].L, code[i].M);
    }
}

void program()
{
    get_token();

    block(scopeCount);
    if(HALT) return;

    if(check_error(periodsym, 9)) return; // Error: Missing period

    emit(SIO_Code, 0, 0, 3);
}

void block(int scope)
{
    if(HALT) return;
    if(level > MAX_LEXI_LEVELS)
    {
        HALT = 1;
        return;
    }

    int jmp = cx; // Save code index of process' jump code to update later
    int address = 4; // Address to store variables

    emit(JMP_Code, 0, 0, 0); // Initial jump code

    do
    {
        if(token == constsym) // Constants declaration
        {
            do
            {
                const_dec(scope);
                if(HALT) return;
            }
            while(token == commasym && lx != lexeme_count);

            if(token != semicolonsym)
            {
                if(token == identsym) error(5); // Error: Missing comma or semicolon
                else error(17); // Error Missing semicolon
                return;
            }

            get_token();
        }
        if(token == varsym) // Variable declaration
        {
            do
            {
                var_dec(address++, scope);
                if(HALT) return;
            }
            while(token == commasym);

            if(token != semicolonsym)
            {
                if(token == identsym) error(5); // Error: Missing comma or semicolon
                else error(17); // Error Missing semicolon
                return;
            }

            get_token();
        }
        if(token == procsym)
        {
            proc_dec(cx, scope);
            if(HALT) return;
        }
    }
    while(token == constsym || token == varsym || token == procsym);

    code[jmp].M = cx; // Update process' jump code location

    emit(INC_Code, 0, 0, address); // Add code to move the stack pointer to accommodate process variables

    statement(address, scope);
}

// Declares a constant
void const_dec(int scope)
{
    get_token();
    if(check_error(identsym, 4)) return; // Error: Constant declaration does not begin with a constant name

    get_token();
    if(token != eqsym) // Error: Symbol following constant name is not =
    {
        if(token == becomessym)
            error(1);
        else
            error(3);
        return;
    }

    get_token();
    if(check_error(numbersym, 2)) return; // Error: Symbol following = is not a number

    if(lookup(name, scope) != -1) // Error: Cannot redefine a constant
        error(12);
    if(HALT) return;

    addSymbol(constant, name, num, level, 0, scope);
    get_token();
}

// Declares a variable
void var_dec(int address, int scope)
{
    get_token();
    if(check_error(identsym, 4)) return; // Error: Variable declaration does not begin with a variable name

    addSymbol(variable, name, 0, level, address, scope);
    get_token();
}

// Declares a procedure
void proc_dec(int address, int scope)
{
    get_token();
    if(check_error(identsym, 4)) return; // Error: Process declaration does not begin with a process name

    if(lookup(name, scope) != -1) // Error: Cannot redefine process
    {
        error(12);
        return;
    }

    addSymbol(proccess, name, 0, level, address + 1, scope); // Add process to symbol table

    get_token();
    if(check_error(semicolonsym, 17)) return; // Error: Missing semicolon
    get_token();

    int jmp = cx;
    emit(JMP_Code, 0, 0, 0);

    level++;
    scopes[scopeCount + 1] = scope;
    block(++scopeCount);
    if(HALT) return;
    level--;

    if(check_error(semicolonsym, 6)) return; // Error: Missing semicolon
    emit(RTN_Code, 0, 0, 0);

    code[jmp].M = cx;
    get_token();
}

void statement(int address, int scope)
{
    if(token == identsym) // Variable assignment statement
    {
        int var = lookup(name, scope);
        if(check_error(var, 11)) return; // Error: Variable not found

        if(table[var].kind != variable) // Error: Not a variable
            error(12);
        if(HALT) return;

        get_token();
        if(token != becomessym) // Error: := does not follow variable name
        {
            error(13);
            return;
        }

        get_token();
        if(token != identsym && token != numbersym && token != lparentsym && token != plussym && token != minussym) // Error: Expression cannot start with this symbol
            error(24);
        if(HALT) return;

        expression(scope);
        if(HALT) return;

        _register--;
        emit(STO_Code, _register, level - table[var].level, table[var].addr); // Add code to store value in from the register containing the evaluated expression in the stack

    }
    else if(token == callsym) // Call statement
    {
        get_token();
        if(check_error(identsym, 14)) return; // Error: Call must be followed by an identifier
        int i = lookup(name, scope);

        if(check_error(i, 11)) return; // Error: Procedure not defined

        if(table[i].kind != proccess) // Error: Identifier is not a process
        {
            error(15);
            return;
        }

        emit(CAL_Code, 0, level - table[i].level, table[i].addr);
        get_token();

    }
    else if(token == beginsym) // Begin block
    {
        get_token();

        if(token == endsym)
        {
            error(7);
            return;
        }
        statement(address, scope);
        if(HALT) return;

        if(check_error(semicolonsym, 10)) return; // Error: Missing semicolon

        while(token == semicolonsym && lx != lexeme_count)
        {
            get_token();
            statement(address, scope);
            if(HALT) return;

            if(token != semicolonsym && token != endsym)
                error(10);
            if(HALT) return;
        }

        if(check_error(endsym, 8)) return; // Error: begin statements not followed by end

        get_token();
    }
    else if(token == ifsym) // If statement
    {
        get_token();
        condition(scope);
        if(HALT) return;

        if(check_error(thensym, 16)) return; // Error: then expected

        int ifjmp = cx, elsejmp; // Save jump instruction location to update later
        emit(JPC_Code, _register, 0, 0);

        get_token();
        statement(address, scope);
        if(HALT) return;
        code[ifjmp].M = cx; // Update instruction


        if(token == elsesym)
        {
            elsejmp = cx;
            emit(JMP_Code, 0, 0, 0);
            code[ifjmp].M = cx; // Update instruction to jump into the else statement

            get_token();
            statement(address, scope);
            if(HALT) return;

            code[elsejmp].M = cx;
        }

    }
    else if(token == whilesym) // While block
    {
        int jmp = cx; // Save location of the start of the loop

        get_token();
        condition(scope);
        if(HALT) return;

        int jpc = cx; // Save location of breakout jump
        emit(JPC_Code, 0, 0, 0);

        if(check_error(dosym, 18)) return; // Error: do expected

        get_token();
        statement(address, scope);
        if(HALT) return;

        emit(JMP_Code, 0, 0, jmp);
        code[jpc].M = cx; // Update breakout jump
    }
    else if(token == readsym) // Read statement
    {
        int var;

        get_token();
        if(check_error(identsym, 19)) return; // Error: Wrong symbol after read
        var = lookup(name, scope);

        if(check_error(var, 11)) return; // Error: Variable not found
        else if(table[var].kind != variable) // Error: Wrong symbol after read
            error(19);
        if(HALT) return;


        emit(SIO_Code, _register++, 0, 2);
        emit(STO_Code, --_register, level - table[var].level, table[var].addr);

        get_token();
    }
    else if(token == writesym) // Write statement
    {
        int var;

        get_token();
        var = lookup(name, scope);
        if(check_error(var, 11)) return; // Error: Variable not found
        else if(table[var].kind == proccess) // Error: Identifier is a process
            error(8);
        if(HALT) return;

        if(table[var].kind == constant)
            emit(LIT_Code, _register++, 0, table[var].val);
        else
            emit(LOD_Code, _register++, level - table[var].level, table[var].addr);

        emit(SIO_Code, --_register, 0, 1);

        get_token();
    }
}

void condition(int scope)
{
    if(token == oddsym) // Odd check
    {
        emit(ODD_Code, _register, 0, 0);

        get_token();
        expression(scope);
        if(HALT) return;
    }
    else // Boolean logic
    {
        expression(scope);

        if(token != lessym && token != gtrsym && token != leqsym && token != geqsym && token != eqsym && token != neqsym) // Error: Relational operator expected
            error(20);
        if(HALT) return;

        int opr = token;
        get_token();
        expression(scope);
        if(HALT) return;

        emit(relOP(opr), _register - 2, _register - 2, _register - 1);
        _register -= 2;
    }
}

void expression(int scope)
{
    int opr;
    if(token == plussym || token == minussym)
    {
        opr = token;
        get_token();
        term(scope);
        if(HALT) return;

        if(opr == minussym)
            emit(NEG_Code, _register - 1, _register - 1, 0);
    }
    else term(scope);
    if(HALT) return;

    while(token == plussym || token == minussym)
    {
        opr = token;
        get_token();
        term(scope);
        if(HALT) return;

        // Add instruction
        if(opr == plussym)
            emit(ADD_Code, _register - 2, _register - 2, _register - 1);
        else
            emit(SUB_Code, _register - 2, _register - 2, _register - 1);
        _register--;
    }
}

void term(int scope)
{
    int opr;

    factor(scope);
    if(HALT) return;

    while(token == multsym || token == slashsym)
    {
        opr = token;

        get_token();
        factor(scope);
        if(HALT) return;

        // Add instruction
        if(opr == multsym)
            emit(MUL_Code, _register - 2, _register -2, _register - 1);
        else
            emit(DIV_Code, _register - 2, _register -2, _register - 1);
        _register--;
    }
}

void factor(int scope)
{
    if(token == identsym) // Factor is a variable
    {
        int i = lookup(name, scope);
        if(check_error(i, 11)) return; // Error: Variable not declared

        get_token();

        if(table[i].kind == variable)
        {
            emit(LOD_Code, _register++, level - table[i].level, table[i].addr);
        }
        else if (table[i].kind == constant)
        {
            emit(LIT_Code, _register++, 0, table[i].val);
        }
        else
        {
            error(21); // Error: Identifier cannot be a process
            return;
        }
    }
    else if(token == numbersym) // Factor is a number
    {
        emit(LIT_Code, _register++, 0, num);
        get_token();
    }
    else if(token == lparentsym) // Factor is a left parenthesis
    {
        get_token();
        expression(scope);
        if(HALT) return;

        if(check_error(rparentsym, 22)) return; // Error: Missing right parenthesis

        get_token();
    }
    else // Unrecognized symbol
        error(23);
}

// Prints the error message in the output file and the console
void error(int error)
{
    HALT++;

    switch(error)
    {
    case 1:
        fprintf(fout, "Error 1, Line %d: Use = instead of :=.\n", line);
        printf("Error 1, Line %d: Use = instead of :=.\n", line);
        break;
    case 2:
        fprintf(fout, "Error 2, Line %d: = must be followed by a number.\n", line);
        printf("Error 2, Line %d: = must be followed by a number.\n", line);
        break;
    case 3:
        fprintf(fout, "Error 3, Line %d: Identifier must be followed by =.\n", line);
        printf("Error 3, Line %d: Identifier must be followed by =.\n", line);
        break;
    case 4:
        fprintf(fout, "Error 4, Line %d: const, var, procedure must be followed by an identifier.\n", line);
        printf("Error 4, Line %d: const, var, procedure must be followed by an identifier.\n", line);
        break;
    case 5:
        fprintf(fout, "Error 5, Line %d: Semicolon or comma missing.\n", line);
        printf("Error 5, Line %d: Semicolon or comma missing.\n", line);
        break;
    case 6:
        fprintf(fout, "Error 6, Line %d: Incorrect symbol after statement part in block.\n", line);
        printf("Error 6, Line %d: Incorrect symbol after statement part in block.\n", line);
        break;
    case 7:
        fprintf(fout, "Error 7, Line %d: Statement expected.\n", line);
        printf("Error 7, Line %d: Statement expected.\n", line);
        break;
    case 8:
        fprintf(fout, "Error 8, Line %d: Incorrect symbol after statement part in block.\n", line);
        printf("Error 8, Line %d: Incorrect symbol after statement part in block.\n", line);
        break;
    case 9:
        fprintf(fout, "Error 9, Line %d: Period expected.\n", line);
        printf("Error 9, Line %d: Period expected.\n", line);
        break;
    case 10:
        fprintf(fout, "Error 10, Line %d: Semicolon between statements missing.\n", line);
        printf("Error 10, Line %d: Semicolon between statements missing.\n", line);
        break;
    case 11:
        fprintf(fout, "Error 11, Line %d: Undeclared identifier.\n", line);
        printf("Error 11, Line %d: Undeclared identifier.\n", line);
        break;
    case 12:
        fprintf(fout, "Error 12, Line %d: Reassignment of constant or procedure is not allowed.\n", line);
        printf("Error 12, Line %d: Reassignment of constant or procedure is not allowed.\n", line);
        break;
    case 13:
        fprintf(fout, "Error 13, Line %d: Assignment operator expected.\n", line);
        printf("Error 13, Line %d: Assignment operator expected.\n", line);
        break;
    case 14:
        fprintf(fout, "Error 14, Line %d: call must be followed by an identifier.\n", line);
        printf("Error 14, Line %d: call must be followed by an identifier.\n", line);
        break;
    case 15:
        fprintf(fout, "Error 15, Line %d: Call of a constant or variable is meaningless.\n", line);
        printf("Error 15, Line %d: Call of a constant or variable is meaningless.\n", line);
        break;
    case 16:
        fprintf(fout, "Error 16, Line %d: then expected.\n", line);
        printf("Error 16, Line %d: then expected.\n", line);
        break;
    case 17:
        fprintf(fout, "Error 17, Line %d: Semicolon expected.\n", line);
        printf("Error 17, Line %d: Semicolon expected.\n", line);
        break;
    case 18:
        fprintf(fout, "Error 18, Line %d: do expected.\n", line);
        printf("Error 18, Line %d: do expected.\n", line);
        break;
    case 19:
        fprintf(fout, "Error 19, Line %d: Incorrect symbol following statement.\n", line);
        printf("Error 19, Line %d: Incorrect symbol following statement.\n", line);
        break;
    case 20:
        fprintf(fout, "Error 20, Line %d: Relational operator expected.\n", line);
        printf("Error 20, Line %d: Relational operator expected.\n", line);
        break;
    case 21:
        fprintf(fout, "Error 21, Line %d: Expression must not contain a procedure identifier.\n", line);
        printf("Error 21, Line %d: Expression must not contain a procedure identifier.\n", line);
        break;
    case 22:
        fprintf(fout, "Error 22, Line %d: Right parenthesis missing.\n", line);
        printf("Error 22, Line %d: Right parenthesis missing.\n", line);
        break;
    case 23:
        fprintf(fout, "Error 23, Line %d: The preceding factor cannot begin with this symbol.\n", line);
        printf("Error 23, Line %d: The preceding factor cannot begin with this symbol.\n", line);
        break;
    case 24:
        fprintf(fout, "Error 24, Line %d: An expression cannot begin with this symbol.\n", line);
        printf("Error 24, Line %d: An expression cannot begin with this symbol.\n", line);
        break;
    case 25: // Error is defined in analyzer.c
        break;
    case 26:   // Error is defined in analyzer.c
        break;
    case 27:   // Error is defined in analyzer.c
        break;
    }
}

// Checks if the current symbol is valid
int check_error(int valid_symbol, int errnum)
{
    if(errnum == 11)
    {
        if(valid_symbol == -1)
            error(errnum);
    }
    else if(token != valid_symbol) // Error: Missing right parenthesis
        error(errnum);

    return HALT;
}

// Returns the opcode for the token type
int relOP(int opr)
{
    switch(opr)
    {
    case lessym:
        return LSS_Code;
    case gtrsym:
        return GTR_Code;
    case leqsym:
        return LEQ_Code;
    case geqsym:
        return GEQ_Code;
    case eqsym:
        return EQL_Code;
    case neqsym:
        return NEQ_Code;
    default:
        return nulsym;
    }
}

// Gets the next token
void get_token()
{
    if(lx == lexeme_count) return;

    if(lx != 0)
        line = lexeme_list[lx - 1].line;
    else
        line = lexeme_list[lx].line;

    token = lexeme_list[lx].type;

    if(token == numbersym)
        num = lexeme_list[lx].value;
    if(token == varsym || token == identsym)
        name = lexeme_list[lx].lexeme;
    lx++;
}

int scopeCheck(int index, int scope)
{
    if(table[index].scope == 0) return 1;

    int i = scope;
    while(i != 0)
    {
        if(table[index].scope == i )
            return 1;
        i = scopes[i];
    }

    return 0;
}

int lookup(char *name, int scope)
{
    int i;
    for(i = sx - 1; i > -1; i--)
        if(strcmp(table[i].name, name) == 0 && scopeCheck(i, scope))
            return i;
    return -1;
}

void addSymbol(int kind, char *name, int val, int level, int addr, int scope)
{
    table[sx].kind = kind;
    table[sx].name = name;
    table[sx].val = val;
    table[sx].level = level;
    table[sx].addr = addr;
    table[sx].scope = scope;
    sx++;
}

void emit(int op, int r, int l, int m)
{
    code[cx].OP = op;
    code[cx].R = r;
    code[cx].L = l;
    code[cx].M = m;
    cx++;
}
