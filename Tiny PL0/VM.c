/*
 * Brandon Gotay - br109653 - 3858803
 * COP 3402-1 Fall 2017
 * 11/18/2017
 */

#include "core.h"

Registers Reg;
int stack[MAX_STACK_HEIGHT];
instruction code[MAX_CODE_LENGTH];
short HALT = 0;

int depth = 0;

void vm()
{
    int i;

    // Default values for registers
    for(i = 0; i < NUMBER_OF_REGISTERS; i++) Reg.R[i] = 0;
    Reg.BP = 1;
    Reg.SP = 0;
    Reg.PC = 0;

    // Default values for stack positions
    for(i = 0; i < MAX_STACK_HEIGHT; i++) stack[i] = 0;
    stack[1] = 1;

    // P-Machine Cycles
    while(HALT == 0)
    {
        fetch();
        execute();
        if(v) print(fout);
    }
}

// Loads instruction into the IR register.
void fetch()
{
    Reg.IR = code[Reg.PC];
    Reg.PC++;
}

// Executes the function associated with the op code.
void execute()
{
    switch(Reg.IR.OP)
    {
    case 1:
        LIT();
        break;
    case 2:
        RTN();
        depth--;
        break;
    case 3:
        LOD();
        break;
    case 4:
        STO();
        break;
    case 5:
        CAL();
        depth++;
        break;
    case 6:
        INC();
        break;
    case 7:
        JMP();
        break;
    case 8:
        JPC();
        break;
    case 9:
        SIO();
        break;
    case 10:
        NEG();
        break;
    case 11:
        ADD();
        break;
    case 12:
        SUB();
        break;
    case 13:
        MUL();
        break;
    case 14:
        DIV();
        break;
    case 15:
        ODD();
        break;
    case 16:
        MOD();
        break;
    case 17:
        EQL();
        break;
    case 18:
        NEQ();
        break;
    case 19:
        LSS();
        break;
    case 20:
        LEQ();
        break;
    case 21:
        GTR();
        break;
    case 22:
        GEQ();
        break;
    }
}

// Used for printing the name of the corresponding op code.
char *funcName(int OP)
{
    switch(OP)
    {
    case 1:
        return "LIT";
    case 2:
        return "RTN";
    case 3:
        return "LOD";
    case 4:
        return "STO";
    case 5:
        return "CAL";
    case 6:
        return "INC";
    case 7:
        return "JMP";
    case 8:
        return "JPC";
    case 9:
        return "SIO";
    case 10:
        return "NEG";
    case 11:
        return "ADD";
    case 12:
        return "SUB";
    case 13:
        return "MUL";
    case 14:
        return "DIV";
    case 15:
        return "ODD";
    case 16:
        return "MOD";
    case 17:
        return "EQL";
    case 18:
        return "NEQ";
    case 19:
        return "LSS";
    case 20:
        return "LEQ";
    case 21:
        return "GTR";
    case 22:
        return "GEQ";
    }
    return "";
}

// Prints the stack trace and registers after the execution of the current instruction.
void print(FILE *fout)
{
    // Prints instruction name and the values in the restricted registers.
    fprintf(fout, "%s %4d %4d %4d %4d %4d %4d       ", funcName(Reg.IR.OP), Reg.IR.R, Reg.IR.L, Reg.IR.M, Reg.PC, Reg.BP, Reg.SP);

    // Special print if the program has been halted.
    if(HALT == 1)
    {
        fprintf(fout, "0\n  RF:   0\n");
        return;
    }

    // Prints the stack values.
    int i, j;
    for(i = 0; i < Reg.SP + 1; i++)
    {
        for(j = depth; j >= 0 && i > 3; j--)
        {
            if(i == bar(j, Reg.BP))
            {
                fprintf(fout, "| ");
                break;
            }
        }
        fprintf(fout, "%d ", stack[i]);
    }

    // Prints the rest of the registers.
    fprintf(fout, "\n  RF:   ");
    for (i = 0; i < NUMBER_OF_REGISTERS - 1; i++)
    {
        fprintf(fout, "%d,", Reg.R[i]);
    }
    fprintf(fout, "%d\n", Reg.R[NUMBER_OF_REGISTERS - 1]);
}

int bar(int L, int BP)
{
    while (L > 0)
    {
        BP = stack[BP + 2];
        L--;
    }
    if(BP == 0) return BP;
    return BP;
}
