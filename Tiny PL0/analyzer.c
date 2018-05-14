/*
 * Brandon Gotay - br109653 - 3858803
 * COP 3402-1 Fall 2017
 * 11/18/2017
 */

#include "core.h"
#include <string.h>

int lexeme_count = 0;
char charset[256];
lexeme *lexeme_list;

void analyzer()
{
    if(fin == NULL)
    {
        HALT = 1;
        return;
    }

    int len = 0, cap = 2, i;
    char *code = (char *)malloc(sizeof(char) * 2);
    init();

    // Go through the input file and store the characters in a string.
    char temp;
    fprintf(fout, "Program:\n");

    while((temp = fgetc(fin)) != EOF)
    {
        fputc(temp, fout);

        // Double the capacity if
        if(len == cap) //
        {
            cap *= 2;
            code = (char *) realloc(code, sizeof(char) * cap);
        }

        // Replace all white space with ' '. Otherwise, store the character as is.
        if(charset[temp])
            code[len++] = temp;
        else
            code[len++] = ' ';
    }
    fprintf(fout, "\n\n");
    fclose(fin);

    parse(code, len);
    lexeme_list = (lexeme *) realloc(lexeme_list, sizeof(lexeme) * lexeme_count);

    // Print lexeme_list.
    if(l)
    {

        fprintf(fout, "Lexeme List:\n");
        for(i = 0; i < lexeme_count; i++)
        {
            fprintf(fout, "%d ", lexeme_list[i].type);
            if(lexeme_list[i].type == numbersym || lexeme_list[i].type == identsym)
                fprintf(fout, "%s ", lexeme_list[i].lexeme);
        }
    }

    free(code);
}

// Initialize charset to contain all letters in the PL/0 language
void init()
{
    int i;
    for(i = 0; i < 256; i++) charset[i] = 0;
    charset['+'] = '+';
    charset['-'] = '-';
    charset['*'] = '*';
    charset['/'] = '/';
    charset['('] = '(';
    charset[')'] = ')';
    charset['='] = '=';
    charset[','] = ',';
    charset['.'] = '.';
    charset['<'] = '<';
    charset['>'] = '>';
    charset[';'] = ';';
    charset[':'] = ':';
    charset['\n'] = '\n';

    for(i = 'a'; i <= 'z'; i++) charset[i] = i;
    for(i = 'A'; i <= 'Z'; i++) charset[i] = i;
    for(i = '0'; i <= '9'; i++) charset[i] = i;
}

// Scan through the code and break it into lexemes
void parse(char *code, int len)
{
    int i, line = 1, cap = 1;
    lexeme_list = (lexeme *) malloc(sizeof(lexeme));

    for(i = 0; i < len;)
    {
        int err = 0;

        // Skip white space
        if(isspace(code[i]) && code[i] != '\n')
        {
            i++;
            continue;
        }

        int lex_len = 1;
        char *temp = (char *)malloc(sizeof(char) * 2);
        temp[1] = '\0';

        if(lexeme_count == cap)
        {
            cap *= 2;
            lexeme_list = (lexeme *) realloc(lexeme_list, sizeof(lexeme) * cap);
        }

        // Parse words
        if(isalpha(code[i]))
        {
            temp[0] = code[i++];

            // Gather characters in code until a special character or white space is reached.
            while(!isspace(code[i]) && special(code[i]) == 0 && i < len)
            {
                temp = (char *)realloc(temp, sizeof(char) * (++lex_len + 1));
                temp[lex_len - 1] = code[i++];
                temp[lex_len] = '\0';

                if(MAX_IDENTIFIER_LENGTH == lex_len - 1) err = 3;
            }

            // Add to the lexeme list.
            lexeme_list[lexeme_count].lexeme = temp;
            lexeme_list[lexeme_count].line = line;
            if(err == 0)
                lexeme_list[lexeme_count].type = lex_type(temp, lex_len);
            else
                lexeme_list[lexeme_count].type = nulsym;

            lexeme_count++;
        }
        // Parse number.
        else if(isdigit(code[i]))
        {
            temp[0] = code[i++];
            // Gather characters in code until a special character or white space is reached.
            while(!isspace(code[i]) && special(code[i]) == 0 && i < len)
            {
                if(isalpha(code[i])) err = 1;

                temp = (char *)realloc(temp, sizeof(char) * (++lex_len + 1));
                temp[lex_len - 1] = code[i++];
                temp[lex_len] = '\0';

                if(MAX_NUMBER_LENGTH <= lex_len - 1 && HALT == 0) err = 2;
            }

            // Add to the lexeme list.
            lexeme_list[lexeme_count].lexeme = temp;
            lexeme_list[lexeme_count].value = atoi(temp);
            lexeme_list[lexeme_count].line = line;
            if(err == 0)
                lexeme_list[lexeme_count].type = numbersym;
            else
                lexeme_list[lexeme_count].type = nulsym;

            lexeme_count++;
        }
        // Parse symbols
        else
        {
            if(code[i] == '\n')
            {
                i++;
                line++;
                free(temp);
                continue;
            }

            // Skip comments
            if(code[i] == '/' && i + 1 < len && code[i + 1] == '*')
            {
                int ended = 0;
                for(; i < len - 1; i++)
                {
                    if(code[i] == '*' && code[i + 1] == '/')
                    {
                        i += 2;
                        ended = 1;
                        break;
                    }
                }

                free(temp);
                if(ended) continue;
                else break;
            }
            temp[0] = code[i++];

            // Add the next characters to temp if it is a special character.
            while(special(code[i]) && code[i] != '(' && code[i] != ')' && code[i] != ',' && code[i] != '.' && code[i] != ';' && code[i] != '\n')
            {
                // Add if the next 2 characters do not start a comment
                if(!(code[i] == '/' && i + 1 < len && code[i + 1] == '*'))
                {
                    temp = (char *)realloc(temp, sizeof(char) * (++lex_len + 1));
                    temp[lex_len - 1] = code[i++];
                    temp[lex_len] = '\0';
                }
                else break;
            }

            short type = optype(temp, lex_len);

            if(type == rparentsym && lex_len == 2)
            {
                temp[1] = '\0';
                temp = realloc(temp, sizeof(char) * 2);
                i--;
            }
            // Return error code 4 if the symbol is not recognized.
            if(type == nulsym) err = 4;

            // Add to the lexeme list
            lexeme_list[lexeme_count].lexeme = temp;
            lexeme_list[lexeme_count].type = type;
            lexeme_list[lexeme_count].line = line;
            lexeme_count++;
        }
        switch(err)
        {
        case 1:
            fprintf(fout, "Error 25, Line %d: %s is a variable that starts with a number!\n\n", line, temp);
            printf("Error 25, Line %d: %s is a variable that starts with a number!\n", line, temp);
            HALT = 1;
            break;
        case 2:
            ;
        case 3:
            fprintf(fout, "Error 26, Line %d: %s is too long!\n\n", line, temp);
            printf("Error 26, Line %d: %s is too long!\n", line, temp);
            HALT = 1;
            break;
        case 4:
            fprintf(fout, "Error 27, Line %d: %s is an invalid symbol!\n\n", line, temp);
            printf("Error 27, Line %d: %s is an invalid symbol!\n", line, temp);
            HALT = 1;
        }
    }
}

// Takes a string and returns the associated type value.
int lex_type(char *temp, int len)
{
    if(len == 1 || len > 9) return identsym;

    if(strcmp(temp, "if") == 0) return ifsym;
    if(strcmp(temp, "do") == 0) return dosym;
    if(strcmp(temp, "var") == 0) return varsym;
    if(strcmp(temp, "odd") == 0) return oddsym;
    if(strcmp(temp, "end") == 0) return endsym;
    if(strcmp(temp, "read") == 0) return readsym;
    if(strcmp(temp, "call") == 0) return callsym;
    if(strcmp(temp, "then") == 0) return thensym;
    if(strcmp(temp, "else") == 0) return elsesym;
    if(strcmp(temp, "const") == 0) return constsym;
    if(strcmp(temp, "begin") == 0) return beginsym;
    if(strcmp(temp, "while") == 0) return whilesym;
    if(strcmp(temp, "write") == 0) return writesym;
    if(strcmp(temp, "procedure") == 0) return procsym;

    return identsym;
}

// Returns the value of the character if it is a special symbol.
short special(char c)
{
    switch(c)
    {
    case '+':
        return '+';
    case '-':
        return '-';
    case '*':
        return '*';
    case '/':
        return '/';
    case '(':
        return '(';
    case ')':
        return ')';
    case '=':
        return '=';
    case ',':
        return ',';
    case '.':
        return '.';
    case '<':
        return '<';
    case '>':
        return '>';
    case ';':
        return ';';
    case ':':
        return ':';
    case '\n':
        return '\n';
    default:
        return 0;
    }
}

// Returns the lexeme type of the symbol sequence. Returns 0 if the given sequence is not a recognized sequence.
short optype(char *temp, int len)
{
    if(len > 2) return nulsym;

    // Cases where temp is a single symbol.
    if(len == 1)
    {
        switch(temp[0])
        {
        case '+':
            return plussym;
        case '-':
            return minussym;
        case '*':
            return multsym;
        case '/':
            return slashsym;
        case '(':
            return lparentsym;
        case ')':
            return rparentsym;
        case '=':
            return eqsym;
        case ',':
            return commasym;
        case '.':
            return periodsym;
        case '<':
            return lessym;
        case '>':
            return gtrsym;
        case ';':
            return semicolonsym;
        default:
            return nulsym;
        }
    }

    if(temp[0] == '=') return 0;
    else if(temp[0] == '<')
    {
        if(temp[1] == '>') return neqsym;
        if(temp[1] == '=') return leqsym;
        return nulsym;
    }
    else if(temp[0] == '>')
    {
        if(temp[1] == '=') return geqsym;
        return nulsym;
    }
    else if(temp[0] == ':')
    {
        if(temp[1] == '=') return becomessym;
        return nulsym;
    }
    else if(temp[0] == ')')
    {
        if(temp[1] == '=' || temp[1] == ':')
            return nulsym;
        else return rparentsym;
    }

    return nulsym;
}
