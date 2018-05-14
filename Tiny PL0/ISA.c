/*
 * Brandon Gotay - br109653 - 3858803
 * COP 3402-1 Fall 2017
 * 11/18/2017
 */

#include "core.h"
#include <string.h>

// Gives the starting position for M to offset from L lexicographical levels down.
int base(int L, int BP)
{
    while (L > 0)
    {
        BP = stack[BP + 1];
        L--;
    }
    return BP;
}

// Loads M into R[R]
void LIT()
{
    Reg.R[Reg.IR.R] = Reg.IR.M;
}

// Moves 1 lexicographical level down from the current one
void RTN()
{
    Reg.SP = Reg.BP - 1;
    Reg.BP = stack[Reg.SP + 3];
    Reg.PC = stack[Reg.SP + 4];
}

// Store value from stack in register
void LOD()
{
    Reg.R[Reg.IR.R] = stack[base(Reg.IR.L, Reg.BP) + Reg.IR.M];
}

// Store value from register in stack
void STO()
{
    stack[base(Reg.IR.L, Reg.BP) + Reg.IR.M] = Reg.R[Reg.IR.R];
}

// Create a new activation record
void CAL()
{
    stack[Reg.SP + 1] = 0;
    stack[Reg.SP + 2] = base(Reg.IR.L, Reg.BP);
    stack[Reg.SP + 3] = Reg.BP;
    stack[Reg.SP + 4] = Reg.PC;
    Reg.BP = Reg.SP + 1;
    Reg.PC = Reg.IR.M;
}

// Add M to stack pointer
void INC()
{
    Reg.SP += Reg.IR.M;
}

// Move program counter to M
void JMP()
{
    Reg.PC = Reg.IR.M;
}

// Move program counter to M if R[R] == 0
void JPC()
{
    if(Reg.R[Reg.IR.R] == 0)
        Reg.PC = Reg.IR.M;
}

// Output to console, input from console, stop the program execution
void SIO()
{
    switch(Reg.IR.M)
    {
    case 1:
        printf("%d\n", Reg.R[Reg.IR.R]);
        break;
    case 2:
        scanf("%d", &Reg.R[Reg.IR.R]);
        break;
    case 3:
        HALT = 1;
        break;
    }
}

// R[R] = -L
void NEG()
{
    Reg.R[Reg.IR.R] = 0 - Reg.R[Reg.IR.L];
}

// R[R] = R[L] + R[M]
void ADD()
{
    Reg.R[Reg.IR.R] = Reg.R[Reg.IR.L] + Reg.R[Reg.IR.M];
}

// R[R] = R[L] - R[M]
void SUB()
{
    Reg.R[Reg.IR.R] = Reg.R[Reg.IR.L] - Reg.R[Reg.IR.M];
}

// R[R] = R[L] * R[M]
void MUL()
{
    Reg.R[Reg.IR.R] = Reg.R[Reg.IR.L] * Reg.R[Reg.IR.M];
}

// R[R] = R[L] / R[M]
void DIV()
{
    Reg.R[Reg.IR.R] = Reg.R[Reg.IR.L] / Reg.R[Reg.IR.M];
}

// R[R] = R[R] % 2
void ODD()
{
    Reg.R[Reg.IR.R] = Reg.R[Reg.IR.R] % 2;
}

// R[R] = R[L] % R[M]
void MOD()
{
    Reg.R[Reg.IR.R] = Reg.R[Reg.IR.L] % Reg.R[Reg.IR.M];
}

// R[R] = R[L] == R[M], True = 1, False = 0
void EQL()
{
    if(Reg.R[Reg.IR.L] == Reg.R[Reg.IR.M])
        Reg.R[Reg.IR.R] = 1;
    else
        Reg.R[Reg.IR.R] = 0;
}

// R[R] = R[L] != R[M], True = 1, False = 0
void NEQ()
{
    if(Reg.R[Reg.IR.L] != Reg.R[Reg.IR.M])
        Reg.R[Reg.IR.R] = 1;
    else
        Reg.R[Reg.IR.R] = 0;
}

// R[R] = R[L] < R[M], True = 1, False = 0
void LSS()
{
    if(Reg.R[Reg.IR.L] < Reg.R[Reg.IR.M])
        Reg.R[Reg.IR.R] = 1;
    else
        Reg.R[Reg.IR.R] = 0;
}

// R[R] = R[L] <= R[M], True = 1, False = 0
void LEQ()
{
    if(Reg.R[Reg.IR.L] <= Reg.R[Reg.IR.M])
        Reg.R[Reg.IR.R] = 1;
    else
        Reg.R[Reg.IR.R] = 0;
}

// R[R] = R[L] > R[M], True = 1, False = 0
void GTR()
{
    if(Reg.R[Reg.IR.L] > Reg.R[Reg.IR.M])
        Reg.R[Reg.IR.R] = 1;
    else
        Reg.R[Reg.IR.R] = 0;
}

// R[R] = R[L] >= R[M], True = 1, False = 0
void GEQ()
{
    if(Reg.R[Reg.IR.L] >= Reg.R[Reg.IR.M])
        Reg.R[Reg.IR.R] = 1;
    else
        Reg.R[Reg.IR.R] = 0;
}
