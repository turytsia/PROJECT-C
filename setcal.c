#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// constants for reserved words or symbols
#define MODE "r"
#define SET 'S'
#define RELATION 'R'
#define UNIVERSUM 'U'
#define COMMAND 'C'
#define MAX_LENGTH 30
#define PAIR 2
#define MAX_LINES 1000
#define SPACE " "
#define REL_SPACE "()"
// constants for set commands
#define EMPTY "empty"
#define CARD "card"
#define COMPLEMENT "complement"
#define UNION "union"
#define INTERSECT "intersect"
#define MINUS "minus"
#define SUBSETEQ "subseteq"
#define SUBSET "subset"
#define EQUALS "equals"
// constants for relation commands
#define REFLEXIVE "reflexive"
#define SYMMETRIC "symmetric"
#define ANTISYMMETRIC "antisymmetric"
#define TRANSITIVE "transitive"
#define FUNCTION "function"
#define DOMAIN "domain"
#define CODOMAIN "codomain"
#define INJECTIVE "injective"
#define SURJECTIVE "surjective"
#define BIJECTIVE "bijective"

#define _TRUE "true"
#define _FALSE "false"

// macro functions
#define reset(var) var = 0;                                             // reset macro (basically takes a variable and assigns 0 to it)
#define allocate(var, size) ((var = malloc(size)) == NULL)              // macro for allocating memory and checking if memory was allocated
#define reallocate(err, var, size) ((err = realloc(var, size)) == NULL) // macro for reallocating memory and checking if memory was reallocated
#define isValidChar(ch) ((str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || str[i] == ' ')

// macro to clean up set structures
#define FreeSet(set)                     \
    for (int i = 0; i < set->count; i++) \
        free(set->elements[i]);          \
    free(set->elements);                 \
    free(set);

#define FreeRelation(rel) \
    free(rel->pair);      \
    free(rel);

// custom types
typedef char *string; // custom string (char *)

#define RESTRICTED_COUNT 21

const string RESTRICTED[RESTRICTED_COUNT] = {
    EMPTY,
    CARD,
    COMPLEMENT,
    UNION,
    INTERSECT,
    MINUS,
    SUBSETEQ,
    SUBSET,
    EQUALS,
    _TRUE,
    _FALSE,
    REFLEXIVE,
    SYMMETRIC,
    ANTISYMMETRIC,
    TRANSITIVE,
    FUNCTION,
    DOMAIN,
    CODOMAIN,
    INJECTIVE,
    SURJECTIVE,
    BIJECTIVE,
};
enum ERROR_CODES
{
    MEMORY_ERR = 1,
    ARGS_ERR,
    FILE_ERR,
    UNIVERSUM_ERR,
    SAME_ELEMENT_ERR,
    TOO_FEW_ARGS_ERR,
    TOO_MANY_ARGS_ERR,
    REL_SYNTAX_ERR,
    SET_SYNTAX_ERR,
    COMMAND_SYNTAX_ERR,
    ELEM_NOT_DEFINED,
    LINE_LIMIT_ERR

};
typedef struct // struct for sets
{
    string *elements;
    int count;
} set_t;
typedef struct // struct for pairs in relation
{
    char elements[PAIR][MAX_LENGTH];
} pair_t;
typedef struct // struct for relations
{
    pair_t *pair;
    int count;
} rel_t;
typedef struct // struct for commands
{
    string command;     // type of command like "minus" or "intersect"
    int arguments[3];   // arguments (indexes)
    int argumentLength; // arguments count
} exp_t;
// error handler
void ThrowError(const int errorCode)
{
    switch (errorCode)
    {
    case MEMORY_ERR:
        fprintf(stderr, "Unable to allocate the memory");
        break;
    case ARGS_ERR:
        fprintf(stderr, "Invalid arguments");
        break;
    case FILE_ERR:
        fprintf(stderr, "Unable to open a file");
        break;
    case UNIVERSUM_ERR:
        fprintf(stderr, "You can't create second universum");
        break;
    case SAME_ELEMENT_ERR:
        fprintf(stderr, "You can't have 2 same elements in set or relation");
        break;
    case TOO_FEW_ARGS_ERR:
        fprintf(stderr, "Too few arguments");
        break;
    case TOO_MANY_ARGS_ERR:
        fprintf(stderr, "Too few arguments");
        break;
    case REL_SYNTAX_ERR:
        fprintf(stderr, "Relation syntax error");
        break;
    case SET_SYNTAX_ERR:
        fprintf(stderr, "Set syntax error");
        break;
    case COMMAND_SYNTAX_ERR:
        fprintf(stderr, "Command syntax error");
        break;
    case ELEM_NOT_DEFINED:
        fprintf(stderr, "Element is not defined in universum");
        break;
    case LINE_LIMIT_ERR:
        fprintf(stderr, "You have reached limit (1000 lines)");
        break;
    }
    exit(errorCode);
}

// prototypes

// functions to validate string before converting in structures
int ValidateStringSet(string str, set_t *universum);
int ValidateStringRelation(string str, set_t *universum);

// functions to convert string to specific structure
int StringToSet(string str, set_t *set); // converts string to set
int StringToRelation(string str, rel_t *dest);
int StringToExpression(string str, exp_t *dest); // converts string to expression

// extra functions
bool isUniversumElement(string str, set_t *universum);
bool EqualStrings(string *strs, int size);
bool isRestricted(string str);

int CommandExecution(string *lines, int size); // executes command

// command functions (prototypes)
int Empty(set_t *set1); // is Empty
int Card(set_t *set);
int Complement(set_t *universum, set_t *set);
int Union(set_t *set1, set_t *set2);
int Intersect(set_t *set1, set_t *set2);
int Minus(set_t *set1, set_t *set2);
int Subseteq(set_t *set1, set_t *set2);
int Subset(set_t *set1, set_t *set2);
int Equals(set_t *set1, set_t *set2);

int Reflexive(rel_t *rel, set_t *universum);
int Symmetric(rel_t *rel);
int Antisymmetric(rel_t *rel);
int Transitive(rel_t *rel);
int Function(rel_t *rel);
int Domain(rel_t *rel);
int Codomain(rel_t *rel);
int Injective(rel_t *rel, set_t *set1, set_t *set2);
int Surjective(rel_t *rel, set_t *set1, set_t *set2);
int Bijective(rel_t *rel, set_t *set1, set_t *set2);

// start here.
int main(int argc, char **argv)
{
    FILE *fp;      // file pointer
    string *lines; // all the lines from the file

    int linesCount = 0; // count of lines
    int strLength = 0;  // line's length
    int chr;            // character for reading file by symbol

    void *error; // variable to hold the reference of the memory that is being allocated (to check if memory was allocated)
    int errorCode = 0;

    int commandBegin = 0;
    int setBegin = 0;

    if (argc != 2)            // if we have more or less than 2 arguments
        ThrowError(ARGS_ERR); // throws error

    fp = fopen(argv[1], MODE);

    if (fp == NULL)           // if we can't open file (wrong name or something like this, fp will recieve value NULL from fopen function)
        ThrowError(FILE_ERR); // throws error

    if (allocate(lines, sizeof(string))) // allocates memory for array of strings
        ThrowError(MEMORY_ERR);          // throws error

    if (allocate(lines[linesCount], sizeof(char))) // allocates memory for temp string
    {
        free(lines);            // frees memory of lines
        fclose(fp);             // closes the file
        ThrowError(MEMORY_ERR); // throws error
    }
    while ((chr = getc(fp)) != EOF) // reads file by symbol
    {
        if (chr != '\n') // when symbol is equal to konec radku
        {

            lines[linesCount][strLength++] = chr;                      // reading file
            if (reallocate(error, lines[linesCount], (strLength + 1))) // resizes str
            {
                errorCode = MEMORY_ERR;
                break;
            }

            lines[linesCount] = error; // if there is no error set pointer back to str
        }
        else // otherwise
        {
            if (linesCount > MAX_LINES) // limit
            {
                errorCode = LINE_LIMIT_ERR;
                break;
            }
            reset(lines[linesCount][strLength]); // reset function just assigns 0 to a passed argument
            reset(strLength);

            if (lines[linesCount][0] == COMMAND)
            {
                commandBegin = linesCount;
            }
            else if (lines[linesCount][0] == SET || lines[linesCount][0] == RELATION)
            {
                setBegin = linesCount;
                if (setBegin > commandBegin && commandBegin)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
            }

            linesCount++;

            // resizes memory for lines (adds one more element to it)
            if (reallocate(error, lines, (linesCount + 1) * sizeof(string)))
            {
                errorCode = MEMORY_ERR;
                break;
            }
            // error - is a variable to store error if there is one
            lines = error;                                 // if there was no error (error wasn't equal to NULL, than it has reference to newly reallocated memory)
            if (allocate(lines[linesCount], sizeof(char))) // allocates memory for a new characters in a line by index
            {
                errorCode = MEMORY_ERR;
                break;
            }
            // if somewhere was an error, loop will break
        }
    }
    // at this poind we have all the file stored by lines in "lines" variable or got an error
    if (!commandBegin || !setBegin)
        errorCode = ARGS_ERR;

    if (!errorCode)                                      // if there was no error we will get to main code
        errorCode = CommandExecution(lines, linesCount); // executing commands

    // frees each pointer to a line in lines (array of lines)
    for (int i = 0; i < linesCount + 1; i++) // linesCount + 1 because on last iteration we allocate memory for another line (which is not necessary and should be fixed..)
        free(lines[i]);
    free(lines);   // frees
    fclose(fp);    // closes the file
    if (errorCode) // if there is an error it will throw it with a message
        ThrowError(errorCode);
    return EXIT_SUCCESS;
}

int ValidateStringSet(string str, set_t *universum)
{
    int errorCode = 0; // variable for errorCodes (to be more precise about type of an error)

    if (universum != NULL && str[0] == UNIVERSUM) // if universum isn't null and the string is for universum it means that it's the second universum
        return ARGS_ERR;
    if (strlen(str) == 1) //
        return EXIT_SUCCESS;
    if (str[1] != ' ' || str[strlen(str) - 1] == ' ') // if the second character or the last one isn't or IS a MEZERO
        return ARGS_ERR;
    for (int i = 1; i < (int)strlen(str); i++)
        if (!isValidChar(str[i]) || (str[i - 1] == ' ' && str[i - 1] == str[i])) // if string doesn't have any symbols or 2 MEZERA in a row
            return ARGS_ERR;

    set_t *set;                       // temp set
    if (allocate(set, sizeof(set_t))) // allocates memory for set
        return MEMORY_ERR;

    if (StringToSet(str, set)) // converts it to set
    {
        FreeSet(set); // frees set (macro)
        return MEMORY_ERR;
    }

    // checks if set has the same elements
    if (EqualStrings(set->elements, set->count))
    {
        FreeSet(set); // frees set (macro)
        return ARGS_ERR;
    }

    // loop to check if set has restricted words assigned as elements
    for (int i = 0; i < set->count; i++)
    {
        if (isRestricted(set->elements[i]))
        {
            FreeSet(set);
            return ARGS_ERR;
        }
    }

    // loop to check if all the values also exist at universum if it exists
    if (universum != NULL)
    {
        for (int i = 0; i < set->count; i++)
        {
            if (!isUniversumElement(set->elements[i], universum))
            {
                FreeSet(set);
                return ARGS_ERR;
            }
        }
    }

    // frees set
    FreeSet(set);
    return errorCode;
}

// Next 2 functions are for validation

int ValidateStringRelation(string str, set_t *universum)
{
    int errorCode = 0;        // variable for errorCodes (to be more precise about type of an error)
    int length = strlen(str); // length of string passed in

    if (strlen(str) == 1) //
        return EXIT_SUCCESS;
    if (str[1] != ' ' || str[length - 1] == ' ') // if the second symbol in string or the last one is equal to a MEZERO
        return ARGS_ERR;
    for (int i = 2; i < length; i++)
    {
        // if string has digits, any ( ) or 2 MEZERA in a row
        if (((str[i] != ')' && str[i] != '(') && !isValidChar(str[i])) || (str[i - 1] == ' ' && str[i - 1] == str[i]))
            return ARGS_ERR;
        // if after ) no MEZERA
        if (i + 1 != length && str[i] == ')' && str[i + 1] != ' ')
            return ARGS_ERR;
        // if before ) is a MEZERO
        if (str[i] == ')' && str[i - 1] == ' ')
            return ARGS_ERR;
        // if after ( is a MEZERO
        if (i + 1 != length && str[i] == '(' && str[i + 1] == ' ')
            return ARGS_ERR;
    }

    string tempstr; // temp variable for string
    string element; // varible to store elements separatly

    if (allocate(tempstr, length + 1)) // allocates memory for temp string
    {
        return MEMORY_ERR;
    }
    strcpy(tempstr, str); // here temp string is the same as str (now we can manipulate str, without changing it)

    element = strtok(tempstr, SPACE);
    for (int i = 0; (element = strtok(NULL, SPACE)) != NULL; i++)
    {
        if (i % 2 == 0 ? element[0] != '(' : element[strlen(element) - 1] != ')')
        {
            errorCode = ARGS_ERR;
            break;
        }
    }

    free(tempstr); // frees temp string

    if (errorCode)
        return ARGS_ERR;

    rel_t *rel; // temp relation

    if (allocate(rel, sizeof(rel_t)))
        return MEMORY_ERR;
    if ((errorCode = StringToRelation(str, rel)))
    {
        FreeRelation(rel);
        return errorCode;
    }

    // loop to check if pair has restricted elements
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < PAIR; j++)
        {
            if (isRestricted(rel->pair[i].elements[j]))
            {
                FreeRelation(rel);
                return ARGS_ERR;
            }
        }
    }

    // loop to check if elements from relation exist in universum
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < PAIR; j++)
        {
            if (!isUniversumElement(rel->pair[i].elements[j], universum))
            {
                FreeRelation(rel);
                return ARGS_ERR;
            }
        }
    }

    // frees relation
    FreeRelation(rel);
    return errorCode;
}

// Function to execute commands
int CommandExecution(string *lines, int lineCount)
{
    exp_t *expression;  // struct to store expression
    set_t *set1, *set2; // variables of structs to store set1 and set2 (if we have to)
    set_t *universum;   // struct to store universum
    rel_t *rel;         // struct to store relation

    int errorCode = 0; // error code to specify error by its code

    if (lines[0][0] != UNIVERSUM) // checks if the first character of the first line is Universum
        return ARGS_ERR;
    if (allocate(universum, sizeof(set_t))) // allocates memory for universum
        return MEMORY_ERR;
    if ((errorCode = ValidateStringSet(lines[0], NULL))) // validates string for universum
    {
        free(universum);
        return errorCode;
    }
    if ((errorCode = StringToSet(lines[0], universum))) // converts string to set
    {
        free(universum);
        return errorCode;
    }

    printf("%s\n", lines[0]); // prints out universum

    if (lineCount == 1)
    {
        FreeSet(universum);
        return ARGS_ERR;
    }

    if (allocate(expression, sizeof(exp_t))) // allocates memory for expression
    {
        FreeSet(universum); // frees the memory
        return MEMORY_ERR;
    }

    for (int i = 1; i < lineCount; i++) // goes from the second line till the last one
    {
        if (lines[i][0] == COMMAND) // if character in a line is C (Command)
        {
            if ((errorCode = StringToExpression(lines[i], expression))) // converts this line into expression
                break;
            /* from now there is the same algorithm.
             * 1 - we allocate memory for our sets (if there is more than one) or relation
             * 2 - convert string into specific structure
             * 3 - call the function for the specific command
             *4 - free all the memory we needed
             */

            if (!strcmp(expression->command, EMPTY))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;

                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                    break;
                if ((errorCode = Empty(set1)))
                    break;
            }
            else if (!strcmp(expression->command, CARD))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                    break;
                if ((errorCode = Card(set1)))
                    break;
            }
            else if (!strcmp(expression->command, COMPLEMENT))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                    break;
                if ((errorCode = Complement(universum, set1)))
                    break;
            }
            else if (!strcmp(expression->command, UNION))
            {
                free(expression->command);
                if (expression->argumentLength != 2)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;

                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                {
                    free(set2);
                    break;
                }
                if (((errorCode = StringToSet(lines[expression->arguments[1]], set2))))
                {
                    FreeSet(set1);
                    break;
                }

                if ((errorCode = Union(set1, set2)))
                    break;
            }
            else if (!strcmp(expression->command, INTERSECT))
            {
                free(expression->command);
                if (expression->argumentLength != 2)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;

                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                {
                    free(set2);
                    break;
                }
                if (((errorCode = StringToSet(lines[expression->arguments[1]], set2))))
                {
                    FreeSet(set1);
                    break;
                }

                if ((errorCode = Intersect(set1, set2)))
                    break;
            }
            else if (!strcmp(expression->command, MINUS))
            {
                free(expression->command);
                if (expression->argumentLength != 2)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;

                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                {
                    free(set2);
                    break;
                }
                if (((errorCode = StringToSet(lines[expression->arguments[1]], set2))))
                {
                    FreeSet(set1);
                    break;
                }

                if ((errorCode = Minus(set1, set2)))
                    break;
            }
            else if (!strcmp(expression->command, SUBSETEQ))
            {
                free(expression->command);
                if (expression->argumentLength != 2)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;

                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[1]], set2)))
                    break;

                if ((errorCode = Subseteq(set1, set2)))
                    break;
            }
            else if (!strcmp(expression->command, SUBSET))
            {
                free(expression->command);
                if (expression->argumentLength != 2)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;

                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[1]], set2)))
                    break;

                if ((errorCode = Subset(set1, set2)))
                    break;
            }
            else if (!strcmp(expression->command, EQUALS))
            {
                free(expression->command);
                if (expression->argumentLength != 2)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;

                if ((errorCode = StringToSet(lines[expression->arguments[0]], set1)))
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[1]], set2)))
                    break;

                if ((errorCode = Equals(set1, set2)))
                    break;
            }
            else if (!strcmp(expression->command, REFLEXIVE))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = Reflexive(rel, universum)))
                    break;
            }
            else if (!strcmp(expression->command, SYMMETRIC))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = Symmetric(rel)))
                    break;
            }
            else if (!strcmp(expression->command, ANTISYMMETRIC))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = Antisymmetric(rel)))
                    break;
            }
            else if (!strcmp(expression->command, TRANSITIVE))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = Transitive(rel)))
                    break;
            }
            else if (!strcmp(expression->command, FUNCTION))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }

                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = Function(rel)))
                    break;
            }
            else if (!strcmp(expression->command, DOMAIN))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = Domain(rel)))
                    break;
            }
            else if (!strcmp(expression->command, CODOMAIN))
            {
                free(expression->command);
                if (expression->argumentLength != 1)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = Codomain(rel)))
                    break;
            }
            else if (!strcmp(expression->command, INJECTIVE))
            {
                // printf("\t%d %d %d\n", expression->arguments[0], expression->arguments[1], expression->arguments[2]);

                free(expression->command);
                if (expression->argumentLength != 3)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel))) // 2
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[1]], set1))) // 3
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[2]], set2))) // 5
                    break;
                if ((errorCode = Injective(rel, set1, set2)))
                    break;
            }
            else if (!strcmp(expression->command, SURJECTIVE))
            {
                free(expression->command);
                if (expression->argumentLength != 3)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[1]], set1)))
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[2]], set2)))
                    break;
                if ((errorCode = Surjective(rel, set1, set2)))
                    break;
            }
            else if (!strcmp(expression->command, BIJECTIVE))
            {
                free(expression->command);
                if (expression->argumentLength != 3)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
                if (allocate(rel, sizeof(rel_t)))
                    return MEMORY_ERR;
                if (allocate(set1, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if (allocate(set2, sizeof(set_t)))
                    errorCode = MEMORY_ERR;
                if ((errorCode = StringToRelation(lines[expression->arguments[0]], rel)))
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[1]], set1)))
                    break;
                if ((errorCode = StringToSet(lines[expression->arguments[2]], set2)))
                    break;
                if ((errorCode = Bijective(rel, set1, set2)))
                    break;
            }
            else
            {
                errorCode = ARGS_ERR;
                break;
            }
            if (i + 1 != lineCount)
                printf("\n");
        }
        else
        {
            switch (lines[i][0]) // if the character is not C
            {
            case SET:                                               // if its S
                errorCode = ValidateStringSet(lines[i], universum); // validates string of set
                break;
            case RELATION:                                               // if its R
                errorCode = ValidateStringRelation(lines[i], universum); // validates string of relation
                break;
            default: // if this is something else
                errorCode = ARGS_ERR;
                break;
            }
            if (errorCode)
                break;

            printf("%s\n", lines[i]); // prints out the line if there was no error
        }
    }

    free(expression);
    FreeSet(universum);

    return errorCode;
}
// function to convert string into Expression
int StringToExpression(string str, exp_t *dest)
{
    string element; // temp string for strtok elements
    string tempstr; // temp string for string passed as argument

    int length = strlen(str);

    if (length == 1)
        return COMMAND_SYNTAX_ERR;
    if (str[1] != ' ' || str[length - 1] == ' ')
        return COMMAND_SYNTAX_ERR;

    for (int i = 2; i < length; i++)
        if (str[i] == ' ' && str[i - 1] == ' ')
            return COMMAND_SYNTAX_ERR;

    if (allocate(tempstr, length + 1)) // allocates the same size of temp string as str
        return MEMORY_ERR;
    strcpy(tempstr, str); // copies str into tempstr

    element = strtok(tempstr, SPACE); // first call - we get C
    element = strtok(NULL, SPACE);    // second call we get some command (like minus or intersect)

    // if element is unknown command
    if (!isRestricted(element))
    {
        free(tempstr);
        return ARGS_ERR;
    }

    if (allocate(dest->command, strlen(element) + 1)) // allocates memory for command
        return MEMORY_ERR;
    strcpy(dest->command, element); // copies command into a structure
    dest->argumentLength = 0;
    while ((element = strtok(NULL, SPACE)) != NULL)
    {
        dest->arguments[dest->argumentLength++] = atoi(element) - 1;
        if (dest->argumentLength > 3) // if we have more than 2 elements
        {
            free(tempstr);
            return ARGS_ERR;
        }
    }
    free(tempstr); // frees temp string

    if (!dest->argumentLength) // if we have 0 elements
    {
        free(dest->command);
        return ARGS_ERR;
    }

    return EXIT_SUCCESS;
}
// converts string into set
int StringToSet(string str, set_t *dest)
{
    if (str[0] != SET && str[0] != UNIVERSUM) // if string is not set or universum
    {
        free(dest);
        return ARGS_ERR;
    }
    reset(dest->count); // resets its count

    string element; // variable to store return variables from strtok
    string tempstr; // temp string to store passed string

    int errorCode = 0; // variable to specify error
    void *error;       // variable to store pointer or an error when reallocating

    if (allocate(dest->elements, sizeof(string))) // allocates memory for elements
        return MEMORY_ERR;
    if (allocate(tempstr, strlen(str) + 1)) // allocates memory for temp string
        return MEMORY_ERR;

    strcpy(tempstr, str); // copies str to tempstr

    element = strtok(tempstr, SPACE);

    while ((element = strtok(NULL, SPACE)) != NULL)
    {
        if (strlen(element) > MAX_LENGTH)
        {
            errorCode = ARGS_ERR;
            break;
        }
        if (allocate(dest->elements[dest->count], strlen(element) + 1)) // allocates memory for specific element
        {
            errorCode = MEMORY_ERR;
            break;
        }

        strcpy(dest->elements[dest->count++], element); // copies element to an element of set

        if (element != NULL) // last element
        {
            if (reallocate(error, dest->elements, (dest->count + 1) * sizeof(string))) // resizes memory for another element
            {
                errorCode = MEMORY_ERR;
                break;
            }
            dest->elements = error;
        }
    }

    free(tempstr); // frees memory

    return errorCode;
}

int StringToRelation(string str, rel_t *dest)
{
    if (str[0] != RELATION) // if string is not relation
    {
        free(dest);
        return ARGS_ERR;
    }

    reset(dest->count); // resets relation's count of elements

    string element;
    string strtemp;

    string *pairs_t; // variable to store pairs from relation like "a b" "c d"

    int pairsCount = 0; // number of pairs

    int errorCode = 0;
    void *error;

    if (allocate(pairs_t, sizeof(string))) // allocates memory for pairs variable
        return MEMORY_ERR;
    if (allocate(strtemp, strlen(str) + 1))
        return MEMORY_ERR;

    strcpy(strtemp, str);

    element = strtok(strtemp, REL_SPACE);

    // this loop finds all the pairs from relation
    while ((element = strtok(NULL, REL_SPACE)) != NULL)
    {
        if (strcmp(element, SPACE))
        {
            if (strlen(element) > MAX_LENGTH)
            {
                errorCode = ARGS_ERR;
                break;
            }
            if (allocate(pairs_t[pairsCount], strlen(element) + 1))
            {
                errorCode = MEMORY_ERR;
                break;
            }
            strcpy(pairs_t[pairsCount], element);
            pairsCount++;

            if (reallocate(error, pairs_t, (pairsCount + 1) * sizeof(pairs_t)))
            {
                errorCode = MEMORY_ERR;
                break;
            }
            pairs_t = error;
        }
    }
    free(strtemp);

    // checks if relation doesn't have the same pair
    if (EqualStrings(pairs_t, pairsCount))
        errorCode = ARGS_ERR;

    if (allocate(dest->pair, sizeof(pair_t))) // allocates memory for pairs in relation
        errorCode = MEMORY_ERR;

    if (!errorCode) // if there is no error
    {
        // this loop separates pairs like "a b" into "a" and "b"
        for (int i = 0; i < pairsCount; i++)
        {
            int counter = 0;
            element = strtok(pairs_t[i], SPACE);
            for (; element != NULL; counter++)
            {
                if (counter >= PAIR)
                {
                    errorCode = ARGS_ERR;
                    break;
                }
                strcpy(dest->pair[dest->count].elements[counter], element);
                element = strtok(NULL, SPACE);
            }
            if (counter == 1 || errorCode) // if in paired string we have only one element
            {
                errorCode = ARGS_ERR;
                break;
            }
            if (reallocate(error, dest->pair, (++(dest->count) + 1) * sizeof(pair_t)))
            {
                errorCode = MEMORY_ERR;
                break;
            }
            dest->pair = error;
        }
    }

    // frees the memory
    for (int i = 0; i < pairsCount; i++)
        free(pairs_t[i]);
    free(pairs_t);
    return errorCode;
}

/*
 * Returns true if set is empty
 *
 */
int Empty(set_t *set)
{
    printf(set->count ? _FALSE : _TRUE);

    FreeSet(set);
    return EXIT_SUCCESS;
}

/*
 * Returns length of set elements
 *
 */
int Card(set_t *set)
{
    printf("%d", set->count);

    FreeSet(set);
    return EXIT_SUCCESS;
}

/*
 *  prints complement of set
 *
 */
int Complement(set_t *universum, set_t *set)
{
    putchar(SET);
    if (!set->count)
        for (int i = 0; i < universum->count; i++)
            printf(" %s", universum->elements[i]);

    for (int i = 0; i < universum->count; i++)
    {
        for (int j = 0; j < set->count; j++)
        {
            if (!strcmp(set->elements[j], universum->elements[i])) // strings are the same
                break;                                             // go to next element in universum
            if (j == (set->count - 1))                             // we went through all the elements in set
                printf(" %s", universum->elements[i]);
        }
    }
    FreeSet(set);
    return EXIT_SUCCESS;
}

/*
 *  prints union of 2 sets
 *
 */
int Union(set_t *set1, set_t *set2)
{
    putchar(SET);
    for (int i = 0; i < set1->count; i++)
        printf(" %s", set1->elements[i]);
    if (set1->count)
    {
        for (int i = 0; i < set2->count; i++)
        {
            for (int j = 0; j < set1->count; j++)
            {
                if (!strcmp(set1->elements[j], set2->elements[i]))
                {
                    break;
                }
                if ((set1->count - 1) == j)
                {
                    printf(" %s", set2->elements[i]);
                }
            }
        }
    }
    else
        for (int i = 0; i < set2->count; i++)
            printf(" %s", set2->elements[i]);

    FreeSet(set1);
    FreeSet(set2);
    return EXIT_SUCCESS;
}

/*
 *  prints intersect of 2 sets
 *
 */
int Intersect(set_t *set1, set_t *set2)
{
    putchar(SET);
    for (int i = 0; i < set1->count; i++)
        for (int j = 0; j < set2->count; j++)
            if (!strcmp(set1->elements[i], set2->elements[j]))
                printf(" %s", set1->elements[i]);
    FreeSet(set1);
    FreeSet(set2);
    return EXIT_SUCCESS;
}

/*
 *  prints set A minus B
 *
 */
int Minus(set_t *set1, set_t *set2)
{
    putchar(SET);
    for (int i = 0; i < set1->count; i++)
    {
        bool flag = false;
        for (int j = 0; j < set2->count; j++)
            if ((flag = !strcmp(set1->elements[i], set2->elements[j])))
                break;
        if (!flag)
            printf(" %s", set1->elements[i]);
    }
    FreeSet(set1);
    FreeSet(set2);
    return EXIT_SUCCESS;
}

/*
 *  prints true if set A if subset equal of B
 *
 */
int Subseteq(set_t *subset, set_t *set)
{
    if (set->count == 0)
    {
        printf(subset->count == set->count ? _TRUE : _FALSE);

        FreeSet(subset);
        FreeSet(set);
        return EXIT_SUCCESS;
    }
    for (int i = 0; i < subset->count; i++)
        for (int j = 0; j < set->count; j++)
        {
            if (!strcmp(set->elements[j], subset->elements[i])) // are equal
                break;
            if (j == (set->count - 1))
            {
                printf(_FALSE);
                FreeSet(subset);
                FreeSet(set);
                return EXIT_SUCCESS;
            }
        }
    printf(_TRUE);
    FreeSet(subset);
    FreeSet(set);
    return EXIT_SUCCESS;
}

/*
 *  prints true if set A if subset of B
 *
 */
int Subset(set_t *subset, set_t *set)
{
    if (subset->count == set->count)
    {
        printf(_FALSE);
        FreeSet(subset);
        FreeSet(set);
        return EXIT_SUCCESS;
    }
    if (set->count == 0)
    {
        printf(_FALSE);
        FreeSet(subset);
        FreeSet(set);
        return EXIT_SUCCESS;
    }
    for (int i = 0; i < subset->count; i++)
    {
        for (int j = 0; j < set->count; j++)
        {
            if (!strcmp(set->elements[j], subset->elements[i])) // are equal
                break;
            if (j == (set->count - 1))
            {
                printf(_FALSE);
                FreeSet(subset);
                FreeSet(set);
                return EXIT_SUCCESS;
            }
        }
    }
    printf(_TRUE);
    FreeSet(subset);
    FreeSet(set);
    return EXIT_SUCCESS;
}

/*
 *  prints true if sets A and B are equal
 *
 */
int Equals(set_t *set1, set_t *set2)
{
    if (!(set1->count == set2->count))
    {
        printf(_FALSE);
        FreeSet(set1);
        FreeSet(set2);
        return EXIT_SUCCESS;
    }
    for (int i = 0; i < set1->count; i++)
    {
        for (int j = 0; j < set2->count; j++)
        {
            if (!strcmp(set1->elements[i], set2->elements[j])) // are equal
                break;
            if (j == (set2->count - 1)) // last element of set2
            {
                printf(_FALSE);
                FreeSet(set1);
                FreeSet(set2);
                return EXIT_SUCCESS;
            }
        }
    }
    printf(_TRUE);
    FreeSet(set1);
    FreeSet(set2);
    return EXIT_SUCCESS;
}

/*
 *  prints true if relation is reflexive
 *
 */
int Reflexive(rel_t *rel, set_t *universum)
{
    int match = 0;
    for (int i = 0; i < rel->count; i++)
        if (!strcmp(rel->pair[i].elements[0], rel->pair[i].elements[1]))
            match++;

    printf(match != universum->count ? _FALSE : _TRUE);

    FreeRelation(rel);
    return EXIT_SUCCESS;
}

/*
 *  prints true if relation is symmetric
 *
 */
int Symmetric(rel_t *rel)
{
    int control = 0;
    for (int i = 0; i < rel->count; i++)
    {
        for (int l = 0; l < rel->count; l++)
        {
            if ((!strcmp(rel->pair[i].elements[0], rel->pair[l].elements[1])) && (!strcmp(rel->pair[i].elements[1], rel->pair[l].elements[0])))
            {
                break; // found a matching symmetric element
            }
            else if (l == rel->count - 1) // the end of relation, haven't found its symmetric match
                control = 1;
        }
        if (control)
        {
            printf(_FALSE);
            break;
        }
    }
    if (!control)
        printf(_TRUE);

    FreeRelation(rel);
    return EXIT_SUCCESS;
}

/*
 *  prints true if relation is antisymmetric
 *
 */
int Antisymmetric(rel_t *rel)
{

    for (int i = 0; i < rel->count; i++)
    {
        for (int j = i + 1; j < rel->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[0], rel->pair[j].elements[1]) && !strcmp(rel->pair[i].elements[1], rel->pair[j].elements[0]))
            {
                printf(_FALSE);
                FreeRelation(rel);
                return EXIT_SUCCESS;
            }
        }
    }
    printf(_TRUE);
    FreeRelation(rel);
    return EXIT_SUCCESS;
}

/*
 *  prints true if relation is transitive
 *
 */
int Transitive(rel_t *rel)
{
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < rel->count; j++)
        {
            if (i != j) // are not two same pairs
            {
                if (!strcmp(rel->pair[i].elements[1], rel->pair[j].elements[0]))
                {
                    for (int k = 0; k < rel->count; k++)
                    {
                        if (!strcmp(rel->pair[i].elements[0], rel->pair[k].elements[0])     // first elements of pairs are the same
                            && !strcmp(rel->pair[j].elements[1], rel->pair[k].elements[1])) // second elments of pairs are the same
                            break;
                        if ((rel->count - 1) == k) // we are at the last pair of relation and havent found the same as we created
                        {
                            printf(_FALSE);
                            FreeRelation(rel);
                            return EXIT_SUCCESS;
                        }
                    }
                }
            }
        }
    }
    printf(_TRUE);
    FreeRelation(rel);
    return EXIT_SUCCESS;
}

/*
 *  prints true if relation is a function
 *
 */
int Function(rel_t *rel)
{
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = i + 1; j < rel->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[0], rel->pair[j].elements[0]) && strcmp(rel->pair[i].elements[1], rel->pair[j].elements[1]))
            {
                printf(_FALSE);
                FreeRelation(rel);
                return EXIT_SUCCESS;
            }
        }
    }
    printf(_TRUE);
    FreeRelation(rel);
    return EXIT_SUCCESS;
}

// domain and codomain
int __domain_codomain(rel_t *rel, int param)
{
    string *elements;

    if (allocate(elements, sizeof(string) * rel->count))
    {
        return MEMORY_ERR;
    }

    putchar(SET);
    for (int i = 0; i < rel->count; i++)
    {
        if (allocate(elements[i], strlen(rel->pair[i].elements[param]) + 1))
            return MEMORY_ERR;
        strcpy(elements[i], rel->pair[i].elements[param]);
    }
    for (int l = 0; l < rel->count; l++)
    {
        bool found = false;
        for (int k = l + 1; k < rel->count; k++)
            if ((found = !strcmp(elements[l], elements[k])))
                break;

        if (!found)
            printf(" %s", elements[l]);
    }

    for (int i = 0; i < rel->count; i++)
        free(elements[i]);
    free(elements);

    FreeRelation(rel);
    return EXIT_SUCCESS;
}

/*
 *  prints set of all the first elements in relation
 *
 */
int Domain(rel_t *rel)
{
    return __domain_codomain(rel, 0);
}

/*
 *  prints set of all the second elements in relation
 *
 */
int Codomain(rel_t *rel) // same for domain, but the second of pair
{
    return __domain_codomain(rel, 1);
}

/*
 *  returns true if relation is Inejctive
 *
 */
int Injective(rel_t *rel, set_t *set1, set_t *set2) //
{
    if (rel->count != 0) // we solve problem with having empty set but rel with elements
    {
        if (set1->count == 0)
        {
            printf(_FALSE);
            FreeRelation(rel);
            FreeSet(set1);
            FreeSet(set2);
            return EXIT_SUCCESS;
        }
        if (set2->count == 0)
        {
            printf(_FALSE);
            FreeRelation(rel);
            FreeSet(set1);
            FreeSet(set2);
            return EXIT_SUCCESS;
        }
    }

    // all the first elements in rel are from set1
    for (int i = 0; i < rel->count; i++)
        for (int j = 0; j < set1->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[0], set1->elements[j]))
                break;
            if ((set1->count - 1) == j)
            {
                printf(_FALSE);
                FreeRelation(rel);
                FreeSet(set1);
                FreeSet(set2);
                return EXIT_SUCCESS;
            }
        }

    // all the second elements in rel are from set2

    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < set2->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[1], set2->elements[j]))
            {
                break;
            }
            if ((set2->count - 1) == j)
            {
                printf(_FALSE);
                FreeRelation(rel);
                FreeSet(set1);
                FreeSet(set2);
                return EXIT_SUCCESS;
            }
        }
    }
    // jeden vzor nema vice nez jeden obraz
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < rel->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[0], rel->pair[j].elements[0])) // same first elements
            {
                if (strcmp(rel->pair[i].elements[1], rel->pair[j].elements[1])) // different second element - we found a pair with same first element and different second element - not injective relation
                {
                    printf(_FALSE);
                    FreeRelation(rel);
                    FreeSet(set1);
                    FreeSet(set2);
                    return EXIT_SUCCESS;
                }
            }
        }
    }

    printf(_TRUE);
    FreeRelation(rel);
    FreeSet(set1);
    FreeSet(set2);
    return EXIT_SUCCESS;
}

/*
 *  returns true if relation is Surjective
 *
 */
int Surjective(rel_t *rel, set_t *set1, set_t *set2)
{
    // we solve problem with having empty set but rel with elements
    if (rel->count != 0)
    {
        if (set1->count == 0)
        {
            printf(_FALSE);
            FreeRelation(rel);
            FreeSet(set1);
            FreeSet(set2);
            return EXIT_SUCCESS;
        }
        if (set2->count == 0)
        {
            printf(_FALSE);
            FreeRelation(rel);
            FreeSet(set1);
            FreeSet(set2);
            return EXIT_SUCCESS;
        }
    }

    // all the first elements in rel are from set1
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < set1->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[0], set1->elements[j]))
                break;

            if ((set1->count - 1) == j)
            {
                printf(_FALSE);
                FreeRelation(rel);
                FreeSet(set1);
                FreeSet(set2);
                return EXIT_SUCCESS;
            }
        }
    }

    // all the second elements in rel are from set2
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < set2->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[1], set2->elements[j]))
                break;

            if ((set2->count - 1) == j)
            {
                printf(_FALSE);
                FreeRelation(rel);
                FreeSet(set1);
                FreeSet(set2);
                return EXIT_SUCCESS;
            }
        }
    }
    // checks if every codomain is being assigned
    // and so we wont forget any Y
    for (int i = 0; i < set2->count; i++)
    {
        for (int j = 0; j < rel->count; j++)
        {
            if (!strcmp(set2->elements[i], rel->pair[j].elements[1]))
                break;

            if ((rel->count - 1) == j)
            {
                printf(_FALSE);
                FreeRelation(rel);
                FreeSet(set1);
                FreeSet(set2);
                return EXIT_SUCCESS;
            }
        }
    }
    // checks if in every pair the first element exists
    // and is being assigned to Y
    for (int i = 0; i < rel->count; i++)
        if (rel->pair[i].elements[0] == NULL)
        {
            printf(_FALSE);
            FreeRelation(rel);
            FreeSet(set1);
            FreeSet(set2);
            return EXIT_SUCCESS;
        }

    printf(_TRUE);
    FreeRelation(rel);
    FreeSet(set1);
    FreeSet(set2);
    return EXIT_SUCCESS;
}

/*
 *  returns true if relation is Bijective
 *
 */
int Bijective(rel_t *rel, set_t *set1, set_t *set2)
{
    if (rel->count != set1->count || rel->count != set2->count) // has to have same count
    {
        printf(_FALSE);
        return EXIT_SUCCESS;
    }

    // all the first elements in rel are from set1
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < set1->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[0], set1->elements[j]))
                break;
            if ((rel->count - 1) == j)
            {
                printf(_FALSE);
                return EXIT_SUCCESS;
            }
        }
    }

    // all the second elements in rel are from set2
    for (int i = 0; i < rel->count; i++)
    {
        for (int j = 0; j < set2->count; j++)
        {
            if (!strcmp(rel->pair[i].elements[1], set2->elements[j]))
                break;
            if ((rel->count - 1) == j)
            {
                printf(_FALSE);
                return EXIT_SUCCESS;
            }
        }
    }

    // all first or second elements are not repeating
    for (int i = 0; i < rel->count - 1; i++)
    {
        for (int j = i + 1; j < rel->count; j++)
        {
            if ((!strcmp(rel->pair[i].elements[0], rel->pair[j].elements[0])) || (!strcmp(rel->pair[i].elements[1], rel->pair[j].elements[1])))
            {
                printf(_FALSE);
                return EXIT_SUCCESS;
            }
        }
    }

    FreeRelation(rel);
    FreeSet(set1);
    FreeSet(set2);
    printf(_TRUE);
    return EXIT_SUCCESS;
}

// returns true if array of strings has 2 same strings
bool EqualStrings(string *strs, int size)
{
    for (int i = 0; i < size; i++)
        for (int j = i + 1; j < size; j++)
            if (!strcmp(strs[i], strs[j]))
                return true;
    return false;
}

// returns true string is a restricted word
bool isRestricted(string str)
{
    bool found = false;
    for (int i = 0; i < RESTRICTED_COUNT; i++)
        if ((found = !strcmp(RESTRICTED[i], str)))
            break;

    return found;
}

// returns true if string is a universum element
bool isUniversumElement(string str, set_t *universum)
{
    bool found = false;
    for (int z = 0; z < universum->count; z++)
        if ((found = !strcmp(str, universum->elements[z])))
            break;

    return found;
}