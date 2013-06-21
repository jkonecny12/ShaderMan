#ifndef UNIFORMCELL_H
#define UNIFORMCELL_H

#include <QString>
#include <QStringList>
#include <QStack>
#include <QList>
#include <QPair>
#include <QVariant>
#include <cmath>

#include "uniform/storage/specialvariableaction.h"
#include "uniform/storage/specialvariableactionpressed.h"
#include "uniform/storage/specialvariabletime.h"

#define PI 3.14159265359

template <class T>
/**
 * @brief The UniformCell class Uniform cell variable.
 * Using lexycal and syntax analysis in this class for getting steps for final calculation.
 * Create database of special variables and create these special variables classes.
 */
class UniformCell
{
private:
    enum Token {PLUS = 0, MINUS = 1, TIMES = 2, DIVIDE = 3, POWER = 4, FNC = 5, LBRACKET = 6, IDENTIFIER = 7,
                RBRACKET = 8, EMPTY = 9,
                ERROR_CELL, EQUAL, GT, LT, NONTERM};
    enum SpecVar {TIME, ACTION, ACTION_PRESSED, NONE};
    enum FncType {SIN, COS, TAN, ASIN, ACOS, ATAN, EMPTY_FNC};
    /**
     * @brief The Token_struct struct Structure for saving tokens from lexical analysis
     */
    struct Token_struct {

        Token_struct() {this->token = EMPTY; this->terminal = true; this->isSpecVar = false;}
        Token_struct(Token token, FncType type = EMPTY_FNC)
        {
            this->token = token;
            if(token == NONTERM || token == GT || token == LT || token == ERROR_CELL || token == EQUAL)
                terminal = false;
            else
                terminal = true;

            isSpecVar = false;
            fncType = type;
        }

        Token token;
        bool terminal;

        T number;
        int nontermId;

        bool isSpecVar;
        int specVarId;
        SpecVar type;

        FncType fncType;
    };

    /**
     * @brief The Nonterminal struct Save nonterminal for later computing.
     */
    struct Nonterminal {

        Nonterminal() {isCalculated = false; isSpecVariable = false;}

        QList<int> idNonterm;
        T number;
        int ruleNumber;
        int id;
        bool isCalculated;
        bool isSpecVariable;
        int specVarId;

        FncType fncType;
    };

    /**
     * @brief The SpecVariable struct Structure for saving special variables.
     */
    struct SpecVariable {
        int id;
        SpecVar type;
        QList<T> numbers;
    };

public:
    enum Type {INT, UINT, FLOAT, DOUBLE};

    /**
     * @brief UniformCell Create uniform cell object. This object will test variable for validity and return result number.
     * If special variable is used here, this object will work with this variable.
     * @param uniform Uniform variable original string from user.
     */
    UniformCell(QString uniform)  :
        maxButtonId(15),
        origin(uniform),
        hasVariables(false),
        isValid(false)
    {
        T a = 0;

        type = getType(a);
        // fill up priorityTable
        // first - stack, second - input
        priorityTable[PLUS][PLUS] = GT;
        priorityTable[PLUS][MINUS] = GT;
        priorityTable[PLUS][TIMES] = LT;
        priorityTable[PLUS][DIVIDE] = LT;
        priorityTable[PLUS][POWER] = LT;
        priorityTable[PLUS][FNC] = LT;
        priorityTable[PLUS][LBRACKET] = LT;
        priorityTable[PLUS][IDENTIFIER] = LT;
        priorityTable[PLUS][RBRACKET] = GT;
        priorityTable[PLUS][EMPTY] = GT;

        priorityTable[MINUS][PLUS] = GT;
        priorityTable[MINUS][MINUS] = GT;
        priorityTable[MINUS][TIMES] = LT;
        priorityTable[MINUS][DIVIDE] = LT;
        priorityTable[MINUS][POWER] = LT;
        priorityTable[MINUS][FNC] = LT;
        priorityTable[MINUS][LBRACKET] = LT;
        priorityTable[MINUS][IDENTIFIER] = LT;
        priorityTable[MINUS][RBRACKET] = GT;
        priorityTable[MINUS][EMPTY] = GT;

        priorityTable[TIMES][PLUS] = GT;
        priorityTable[TIMES][MINUS] = GT;
        priorityTable[TIMES][TIMES] = GT;
        priorityTable[TIMES][DIVIDE] = GT;
        priorityTable[TIMES][POWER] = LT;
        priorityTable[TIMES][FNC] = LT;
        priorityTable[TIMES][LBRACKET] = LT;
        priorityTable[TIMES][IDENTIFIER] = LT;
        priorityTable[TIMES][RBRACKET] = GT;
        priorityTable[TIMES][EMPTY] = GT;

        priorityTable[DIVIDE][PLUS] = GT;
        priorityTable[DIVIDE][MINUS] = GT;
        priorityTable[DIVIDE][TIMES] = GT;
        priorityTable[DIVIDE][DIVIDE] = GT;
        priorityTable[DIVIDE][POWER] = LT;
        priorityTable[DIVIDE][FNC] = LT;
        priorityTable[DIVIDE][LBRACKET] = LT;
        priorityTable[DIVIDE][IDENTIFIER] = LT;
        priorityTable[DIVIDE][RBRACKET] = GT;
        priorityTable[DIVIDE][EMPTY] = GT;

        priorityTable[POWER][PLUS] = GT;
        priorityTable[POWER][MINUS] = GT;
        priorityTable[POWER][TIMES] = GT;
        priorityTable[POWER][DIVIDE] = GT;
        priorityTable[POWER][POWER] = LT;
        priorityTable[POWER][FNC] = LT;
        priorityTable[POWER][LBRACKET] = LT;
        priorityTable[POWER][IDENTIFIER] = LT;
        priorityTable[POWER][RBRACKET] = GT;
        priorityTable[POWER][EMPTY] = GT;

        priorityTable[FNC][PLUS] = GT;
        priorityTable[FNC][MINUS] = GT;
        priorityTable[FNC][TIMES] = GT;
        priorityTable[FNC][DIVIDE] = GT;
        priorityTable[FNC][POWER] = GT;
        priorityTable[FNC][FNC] = ERROR_CELL;
        priorityTable[FNC][LBRACKET] = LT;
        priorityTable[FNC][IDENTIFIER] = LT;
        priorityTable[FNC][RBRACKET] = GT;
        priorityTable[FNC][EMPTY] = GT;

        priorityTable[LBRACKET][PLUS] = LT;
        priorityTable[LBRACKET][MINUS] = LT;
        priorityTable[LBRACKET][TIMES] = LT;
        priorityTable[LBRACKET][DIVIDE] = LT;
        priorityTable[LBRACKET][POWER] = LT;
        priorityTable[LBRACKET][FNC] = LT;
        priorityTable[LBRACKET][LBRACKET] = LT;
        priorityTable[LBRACKET][IDENTIFIER] = LT;
        priorityTable[LBRACKET][RBRACKET] = EQUAL;
        priorityTable[LBRACKET][EMPTY] = ERROR_CELL;

        priorityTable[IDENTIFIER][PLUS] = GT;
        priorityTable[IDENTIFIER][MINUS] = GT;
        priorityTable[IDENTIFIER][TIMES] = GT;
        priorityTable[IDENTIFIER][DIVIDE] = GT;
        priorityTable[IDENTIFIER][POWER] = GT;
        priorityTable[IDENTIFIER][FNC] = ERROR_CELL;
        priorityTable[IDENTIFIER][LBRACKET] = ERROR_CELL;
        priorityTable[IDENTIFIER][IDENTIFIER] = ERROR_CELL;
        priorityTable[IDENTIFIER][RBRACKET] = GT;
        priorityTable[IDENTIFIER][EMPTY] = GT;

        priorityTable[RBRACKET][PLUS] = GT;
        priorityTable[RBRACKET][MINUS] = GT;
        priorityTable[RBRACKET][TIMES] = GT;
        priorityTable[RBRACKET][DIVIDE] = GT;
        priorityTable[RBRACKET][POWER] = GT;
        priorityTable[RBRACKET][FNC] = ERROR_CELL;
        priorityTable[RBRACKET][LBRACKET] = ERROR_CELL;
        priorityTable[RBRACKET][IDENTIFIER] = ERROR_CELL;
        priorityTable[RBRACKET][RBRACKET] = GT;
        priorityTable[RBRACKET][EMPTY] = GT;

        priorityTable[EMPTY][PLUS] = LT;
        priorityTable[EMPTY][MINUS] = LT;
        priorityTable[EMPTY][TIMES] = LT;
        priorityTable[EMPTY][DIVIDE] = LT;
        priorityTable[EMPTY][POWER] = LT;
        priorityTable[EMPTY][FNC] = LT;
        priorityTable[EMPTY][LBRACKET] = LT;
        priorityTable[EMPTY][IDENTIFIER] = LT;
        priorityTable[EMPTY][RBRACKET] = ERROR_CELL;
        priorityTable[EMPTY][EMPTY] = ERROR_CELL;

        // fill up rules
        // 1. E -> E + E
        QList<Token_struct> rule1;
        rule1.append(Token_struct(NONTERM));
        rule1.append(Token_struct(PLUS));
        rule1.append(Token_struct(NONTERM));
        rules.append(qMakePair(Token_struct(NONTERM),rule1));

        // 2. E -> E - E
        QList<Token_struct> rule2;
        rule2.append(Token_struct(NONTERM));
        rule2.append(Token_struct(MINUS));
        rule2.append(Token_struct(NONTERM));
        rules.append(qMakePair(Token_struct(NONTERM),rule2));

        // 3. E -> E * E
        QList<Token_struct> rule3;
        rule3.append(Token_struct(NONTERM));
        rule3.append(Token_struct(TIMES));
        rule3.append(Token_struct(NONTERM));
        rules.append(qMakePair(Token_struct(NONTERM),rule3));

        // 4. E -> E / E
        QList<Token_struct> rule4;
        rule4.append(Token_struct(NONTERM));
        rule4.append(Token_struct(DIVIDE));
        rule4.append(Token_struct(NONTERM));
        rules.append(qMakePair(Token_struct(NONTERM),rule4));

        // 5. E -> E ^ E
        QList<Token_struct> rule5;
        rule5.append(Token_struct(NONTERM));
        rule5.append(Token_struct(POWER));
        rule5.append(Token_struct(NONTERM));
        rules.append(qMakePair(Token_struct(NONTERM),rule5));

        // 6. E -> ( E )
        QList<Token_struct> rule6;
        rule6.append(Token_struct(LBRACKET));
        rule6.append(Token_struct(NONTERM));
        rule6.append(Token_struct(RBRACKET));
        rules.append(qMakePair(Token_struct(NONTERM),rule6));

        // 7. E -> + E
        QList<Token_struct> rule7;
        rule7.append(Token_struct(PLUS));
        rule7.append(Token_struct(NONTERM));
        rules.append(qMakePair(Token_struct(NONTERM),rule7));

        // 8. E -> - E
        QList<Token_struct> rule8;
        rule8.append(Token_struct(MINUS));
        rule8.append(Token_struct(NONTERM));
        rules.append(qMakePair(Token_struct(NONTERM),rule8));

        // 9. E -> fnc E
        QList<Token_struct> rule9;
        rule9.append(Token_struct(FNC));
        rule9.append(Token_struct(NONTERM));
        rules.append(qMakePair(Token_struct(NONTERM),rule9));

        // 10. E -> i
        QList<Token_struct> rule10;
        rule10.append(Token_struct(IDENTIFIER));
        rules.append(qMakePair(Token_struct(NONTERM),rule10));

        if(origin.isEmpty())
        {
            isValid = false;
            return;
        }

        lexicalAnalysis();
    }

    ~UniformCell()
    {
        qDeleteAll(nonterminals);
        qDeleteAll(specVars);
    }

    /**
     * @brief getOriginal Get original string.
     * @return Return original uniform string.
     */
    QString getOriginal() const
    {
        return origin;
    }

    /**
     * @brief getResult Get result number from uniform.
     * @return Return result number.
     */
    T getResult() const
    {
        Nonterminal* n = nonterminals.last();

        return getNonTermNumber(n);
    }

    /**
     * @brief testValidity Test if this uniform variable is valid.
     * @return True if variable is valid, false otherwise.
     */
    bool testValidity()
    {
        return isValid;
    }

    /**
     * @brief getSpecVarCount Get number of special variables in this uniform cell.
     * @return Number of special variables here.
     */
    int getSpecVarCount()
    {
        return specVars.size();
    }

    /**
     * @brief getSpecialVariable Get pointer to special variable.
     * @param pos Position of wanted special variable in internal list.
     * @return Special variable as SpecialVariableAbstract class.
     */
    SpecialVariableAbstract<T>* getSpecialVariable(int pos)
    {
        return specVars.value(pos);
    }

    /**
     * @brief calculate Calculate result number from saved nonterm variables.
     */
    void calculate()
    {
        Nonterminal* n1;
        Nonterminal* n2;
        foreach(Nonterminal* n, nonterminals)
        {
            switch(n->ruleNumber)
            {
            case 0: // E -> E + E
                n1 = nonterminals.value(n->idNonterm.value(0));
                n2 = nonterminals.value(n->idNonterm.value(1));
                n->number = getNonTermNumber(n1) + getNonTermNumber(n2); //add(n1,n2);
                //n->isCalculated = true;
                break;
            case 1: // E -> E - E
                n1 = nonterminals.value(n->idNonterm.value(0));
                n2 = nonterminals.value(n->idNonterm.value(1));
                n->number = getNonTermNumber(n1) - getNonTermNumber(n2); //minus(n1,n2);
                //n->isCalculated = true;
                break;
            case 2: // E -> E * E
                n1 = nonterminals.value(n->idNonterm.value(0));
                n2 = nonterminals.value(n->idNonterm.value(1));
                n->number = getNonTermNumber(n1) * getNonTermNumber(n2); //times(n1,n2);
                //n->isCalculated = true;
                break;
            case 3: // E -> E / E
                n1 = nonterminals.value(n->idNonterm.value(0));
                n2 = nonterminals.value(n->idNonterm.value(1));
                n->number = getNonTermNumber(n1) / getNonTermNumber(n2); //divide(n1,n2);
                //n->isCalculated = true;
                break;
            case 4: // E -> E ^ E
                n1 = nonterminals.value(n->idNonterm.value(0));
                n2 = nonterminals.value(n->idNonterm.value(1));
                n->number = static_cast<T>(pow(static_cast<float>(getNonTermNumber(n1)),static_cast<float>(getNonTermNumber(n2))));
                break;
            case 5: // E -> ( E )
                n1 = nonterminals.value(n->idNonterm.value(0));
                n->number = getNonTermNumber(n1);
                //n->isCalculated = true;
                break;
            case 6: // E -> + E
                n1 = nonterminals.value(n->idNonterm.value(0));
                n->number = getNonTermNumber(n1);
                //n->isCalculated = true;
                break;
            case 7: // E -> - E
                n1 = nonterminals.value(n->idNonterm.value(0));
                n->number = - getNonTermNumber(n1);
                break;
                //n->isCalculated = true;
            case 8: // E -> fnc E
                n1 = nonterminals.value(n->idNonterm.value(0));
                n->number = useFnc(n1,n->fncType);
                break;
            //case 10: // E -> i
                //if(n->isSpecVariable)
                    //return; // TODO work with variables
                //n->isCalculated = true;
                //break;
            default:
                break;
            }
        }
    }

private:

    /**
     * @brief getType Get setted type T for this class. Call this only if int is set.
     * @return Return type INT.
     */
    Type getType(int /*a*/)
    {
        return INT;
    }

    /**
     * @brief getType Get setted type T for this class. Call this only if unsigned int is set.
     * @return Return type UINT.
     */
    Type getType(uint /*a*/)
    {
        return UINT;
    }

    /**
     * @brief getType Get setted type T for this class. Call this only if float is set.
     * @return Return type FLOAT.
     */
    Type getType(float /*a*/)
    {
        return FLOAT;
    }

    /**
     * @brief getType Get setted type T for this class. Call this only if double is set.
     * @return Return type DOUBLE.
     */
    Type getType(double /*a*/)
    {
        return DOUBLE;
    }

    /**
     * @brief useFnc If fnc rule was used, then use this function and calculate new number.
     * @param n On what we use function.
     * @return Result number.
     */
    T useFnc(Nonterminal* n, FncType type)
    {
        switch(type)
        {
        case SIN:
            return sin(static_cast<float>(getNonTermNumber(n)));
            break;
        case COS:
            return cos(static_cast<float>(getNonTermNumber(n)));
            break;
        case TAN:
            return tan(static_cast<float>(getNonTermNumber(n)));
            break;
        case ASIN:
            return asin(static_cast<float>(getNonTermNumber(n)));
            break;
        case ACOS:
            return acos(static_cast<float>(getNonTermNumber(n)));
            break;
        case ATAN:
            return atan(static_cast<float>(getNonTermNumber(n)));
            break;
        default:
            return 0;
            break;
        }
    }

    /**
     * @brief lexicalAnalysis Use lexical analyser for origin string a create tokens for further work.
     */
    void lexicalAnalysis()
    {
        QString number;
        bool wasNumber = false;
        isValid = true;
        int counter = 0;
        bool isSpecVar = false;
        SpecVar type = NONE;
        QString specTemp = "";
        QStringList specNums;
        QString memory;

        foreach(QChar c, origin)
        {
            if(isSpecVar) // special variable part
            {
                if(type == NONE) // we do not know type of the special variable
                {
                    if(c == '{')
                    {
                        specNums << ""; // create new number space

                        if(specTemp == "Time")
                        {
                            type = TIME;
                            specTemp.clear();
                        }
                        else if(specTemp == "Action")
                        {
                            type = ACTION;
                            specTemp.clear();
                        }
                        else if(specTemp == "ActionPressed")
                        {
                            type = ACTION_PRESSED;
                            specTemp.clear();
                        }
                        else
                        {
                            isValid = false;
                            return;
                        }
                    }
                    else if(c.isLetter())
                    {
                        specTemp += c;
                    }
                    else if(!c.isLetter() || specTemp.size() > 13)
                    {
                        isValid = false;
                        return;
                    }
                }
                else
                {
                    if(specNums.size() > 5) // max argument number is 5
                    {
                        isValid = false;
                        return;
                    }

                    switch(c.toLatin1())
                    {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        specNums.append(specNums.takeLast() += c);
                        break;
                    case '.':
                        if(this->type == INT || this->type == UINT)
                        {
                            isValid = false;
                            return;
                        }

                        specNums.append(specNums.takeLast() += c);
                        break;

                    case ' ':
                        continue;
                        break;

                    case '+':
                        if(specNums.last().size() != 0)
                        {
                            isValid = false;
                            return;
                        }
                        break;

                    case '-':
                        if(specNums.last().size() != 0)
                        {
                            isValid = false;
                            return;
                        }
                        specNums.append(specNums.takeLast() += c);
                        break;

                    case ',': //separator, save num and continue read another
                        specNums.append(""); // space for new number
                        break;

                    case '}':
                    {
                        if(type == TIME) // test number of parameters and create new specVariable
                        {
                            if(specNums.size() > 4 || specNums.size() < 2)
                            {
                                isValid = false;
                                return;
                            }

                            if(saveSpecVariable(specNums, TIME) == -1)
                                return;
                        }
                        else if(type == ACTION)
                        {
                            if(specNums.size() != 3)
                            {
                                isValid = false;
                                return;
                            }

                            if(saveSpecVariable(specNums, ACTION) == -1)
                                return;
                        }
                        else if(type == ACTION_PRESSED)
                        {
                            if(specNums.size() > 5 || specNums.size() < 4)
                            {
                                isValid = false;
                                return;
                            }

                            if(saveSpecVariable(specNums, ACTION_PRESSED) == -1)
                                return;
                        }

                        specNums.clear();
                        isSpecVar = false;
                        type = NONE;
                        Token_struct s(IDENTIFIER);
                        s.isSpecVar = true;
                        s.specVarId = specVars.size()-1;

                        tokens.append(s);
                        break;
                    }
                    default:
                        isValid = false;
                        return;
                        break;
                    }
                }

            }
            else // normal matematic part
            {
                switch(c.toLatin1())
                {
                case '(':
                    isNumber(number,wasNumber);
                    wasNumber = false;
                    number.clear();

                    tokens.append(Token_struct(LBRACKET));
                    break;
                case ')':
                    isNumber(number,wasNumber);
                    wasNumber = false;
                    number.clear();

                    tokens.append(Token_struct(RBRACKET));
                    break;
                case '+':
                    isNumber(number,wasNumber);
                    wasNumber = false;
                    number.clear();

                    tokens.append(Token_struct(PLUS));
                    break;
                case '-':
                    isNumber(number,wasNumber);
                    wasNumber = false;
                    number.clear();

                    tokens.append(Token_struct(MINUS));
                    break;
                case '*':
                    isNumber(number,wasNumber);
                    wasNumber = false;
                    number.clear();

                    tokens.append(Token_struct(TIMES));
                    break;
                case '/':
                    isNumber(number,wasNumber);
                    wasNumber = false;
                    number.clear();

                    tokens.append(Token_struct(DIVIDE));
                    break;

                case '^':
                    isNumber(number,wasNumber);
                    wasNumber = false;
                    number.clear();

                    tokens.append(Token_struct(POWER));
                    break;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    number += c;
                    wasNumber = true;
                    break;
                case '.':
                    if(this->type == INT || this->type == UINT)
                    {
                        isValid = false;
                        return;
                    }

                    number += c;
                    wasNumber = true;
                    break;
                case '$':
                    isNumber(number,wasNumber);
                    wasNumber = false;
                    number.clear();

                    isSpecVar = true;
                    break;

                    // functions test
                case 'a': // tan, atan, asin, acos
                    isNumber(number, wasNumber);
                    wasNumber = false;
                    number.clear();

                    if(memory.isEmpty() || memory == "t" || memory == "at")
                        memory += 'a';
                    else
                    {
                        isValid = false;
                        return;
                    }

                    break;
                case 's': // sin, asin, cos, acos
                    isNumber(number, wasNumber);
                    wasNumber = false;
                    number.clear();

                    if(memory == "co")
                    {
                        tokens.append(Token_struct(FNC,COS));
                        memory.clear();
                    }
                    else if(memory == "aco")
                    {
                        tokens.append(Token_struct(FNC,ACOS));
                        memory.clear();
                    }
                    else if(memory.isEmpty() || memory == "a")
                        memory += 's';
                    else
                    {
                        isValid = false;
                        return;
                    }
                    break;
                case 'i': // sin, asin, pi
                    isNumber(number, wasNumber);
                    wasNumber = false;
                    number.clear();

                    if(memory == "p")
                    {
                        memory.clear();
                        Token_struct s(IDENTIFIER);
                        s.number = static_cast<float>(PI);
                        tokens.append(s);
                    }
                    else if(memory == "as" || memory == "s")
                        memory += 'i';
                    else
                    {
                        isValid = false;
                        return;
                    }
                    break;

                case 'n': // sin, tan, asin, atan
                    isNumber(number, wasNumber);
                    wasNumber = false;
                    number.clear();

                    if(memory == "si")
                        tokens.append(Token_struct(FNC,SIN));
                    else if(memory == "asi")
                        tokens.append(Token_struct(FNC,ASIN));
                    else if(memory == "ta")
                        tokens.append(Token_struct(FNC,TAN));
                    else if(memory == "ata")
                        tokens.append(Token_struct(FNC,ATAN));
                    else
                    {
                        isValid = false;
                        return;
                    }

                    memory.clear();
                    break;

                case 'c': // cos, acos
                    isNumber(number, wasNumber);
                    wasNumber = false;
                    number.clear();

                    if(memory == "a" || memory.isEmpty())
                        memory += 'c';
                    else
                    {
                        isValid = false;
                        return;
                    }

                    break;

                case 'o': // cos, acos
                    isNumber(number, wasNumber);
                    wasNumber = false;
                    number.clear();

                    if(memory == "ac" || memory == "c")
                        memory += 'o';
                    else
                    {
                        isValid = false;
                        return;
                    }
                    break;

                case 't': // tan, atan
                    isNumber(number, wasNumber);
                    wasNumber = false;
                    number.clear();

                    if(memory == "a" || memory.isEmpty())
                        memory += 't';
                    else
                    {
                        isValid = false;
                        return;
                    }
                    break;

                case 'p':
                    isNumber(number, wasNumber);
                    wasNumber = false;
                    number.clear();

                    if(memory.isEmpty())
                        memory += 'p';
                    else
                    {
                        isValid = false;
                        return;
                    }
                    break;
                case ' ':
                    break;
                default:
                    isValid = false;
                    return;
                    break;
                }
                ++counter;
            }
        }

        if(!specTemp.isEmpty() || !memory.isEmpty())
        {
            isValid = false;
            return;
        }

        isNumber(number,wasNumber);

        if(isValid)
            syntaxAnalysis();
    }

    /**
     * @brief syntaxAnalysis Use tokens from lexical analysis here. Test syntax and create steps for computing variable.
     */
    void syntaxAnalysis()
    {
        QList<Token_struct> input = tokens;
        QStack<Token_struct> stack;
        Token_struct inVar(EMPTY); // input variable (first terminal from input)
        Token_struct stVar(EMPTY); // stack variable (first terminal from stack)

        input.append(Token_struct(EMPTY));
        stack.push(Token_struct(EMPTY));

        do
        {
            inVar = input.first();
            stVar = stack.top();

            if(!stVar.terminal)
            {
                QStack<Token_struct> temp = stack;
                while(!stVar.terminal)
                {
                    temp.pop();
                    stVar = temp.top();
                }
            }

            if(inVar.token == EMPTY && stVar.token == EMPTY) // syntax analysis completed
            {
                isValid = true;
                break;
            }
            else if(priorityTable[stVar.token][inVar.token] == EQUAL)
            {
                input.removeFirst();
                stack.push(inVar);
            }
            else if(priorityTable[stVar.token][inVar.token] == LT)
            {
                QStack<Token_struct> temp;

                while(!stack.top().terminal)
                {
                    temp.push(stack.top());
                    stack.pop();
                }

                stack.push(Token_struct(LT));

                while(!temp.isEmpty())
                {
                    stack.push(temp.top());
                    temp.pop();
                }

                stack.push(inVar);
                input.removeFirst();
            }
            else if(priorityTable[stVar.token][inVar.token] == GT)
            {
                QList<Token_struct> temp;

                while(stack.top().token != LT)
                {
                    if(stack.isEmpty())
                    {
                        isValid = false;
                        return;
                    }


                    temp.push_front(stack.top());
                    stack.pop();
                }

                stack.pop();
                stack.push(testRules(temp));

                if(stack.top().token == ERROR_CELL)
                {
                    isValid = false;
                    return;
                }
            }
            else if(priorityTable[stVar.token][inVar.token] == ERROR_CELL)
            {
                isValid = false;
                return;
            }
        }while(priorityTable[stVar.token][inVar.token] != ERROR_CELL);

        calculate();
    }



    /**
     * @brief getNonTermNumber Get number from nonterminal variable, even if this nonterminal is special variable.
     * @param n Nonterminal variable.
     * @return Number of type T.
     */
    T getNonTermNumber(Nonterminal* n) const
    {
        if(n->isSpecVariable)
        {
            int id = n->specVarId;
            return specVars.value(id)->getValue();
        }
        else
        {
            return n->number;
        }
    }

    /**
     * @brief saveSpecVariable Save special variable from given string.
     * @param numString List of strings with numbers of special variables.
     * @param type Type of the special variable.
     * @return Return id of new special variable.
     */
    int saveSpecVariable(QStringList numString, SpecVar type)
    {
        //SpecVariable* sv = new SpecVariable();

        //sv->id = specVars.size();
        //sv->type = type;

        QList<T> nums;
        int buttonId;
        bool isNotButtonSet = true;

        foreach(QString num, numString)
        {
            if(type == ACTION || type == ACTION_PRESSED)
            {
                if(isNotButtonSet)
                {
                    bool isOk;
                    isNotButtonSet = false;

                    buttonId = num.toInt(&isOk);

                    if(!isOk)
                    {
                        isValid = false;
                        return -1;
                    }
                    else if(buttonId > maxButtonId || buttonId < 0)
                    {
                        isValid = false;
                        return -1;
                    }
                }
            }

            switch(this->type)
            {
            case INT:
                //sv->numbers.append(num.toInt());
                nums.append(num.toInt());
                break;

            case UINT:
                //sv->numbers.append(num.toUInt());
                nums.append(num.toUInt());
                break;

            case FLOAT:
                //sv->numbers.append(num.toFloat());
                nums.append(num.toFloat());
                break;

            case DOUBLE:
                //sv->numbers.append(num.toDouble());
                nums.append(num.toDouble());
                break;
            }
        }

        if(type == TIME)
        {
            switch(nums.size())
            {
            case 2:
                specVars.append(new SpecialVariableTime<T>(nums.value(0), nums.value(1)));
                break;
            case 3:
                specVars.append(new SpecialVariableTime<T>(nums.value(0), nums.value(1), nums.value(2)));
                break;
            default:
                specVars.append(new SpecialVariableTime<T>(nums.value(0), nums.value(1), nums.value(2), nums.value(3)));
                break;
            }
        }
        else if(type == ACTION)
        {
            specVars.append(new SpecialVariableAction<T>(nums.value(0), nums.value(1), nums.value(2)));
        }
        else if(type == ACTION_PRESSED)
        {
            if(nums.size() == 4)
                specVars.append(new SpecialVariableActionPressed<T>(nums.value(0), nums.value(1), nums.value(2), nums.value(3)));
            else
                specVars.append(new SpecialVariableActionPressed<T>(nums.value(0), nums.value(1), nums.value(2), nums.value(3),
                                                                 nums.value(4)));
        }

        return specVars.size()-1;
        //specVars.append(sv);
    }

    /**
     * @brief testRules Test given data in rules right values. If rule right value is equal to data, then return left value,
     * else return ERROR token.
     * @param data Data for compare with rules set.
     * @return Return left value of the rules if data and some rule is equal, otherwise return ERROR token.
     */
    Token_struct testRules(QList<Token_struct> data)
    {
        //foreach((QPair<Token_struct,Token_struct> p), rules)
        for(int i = 0; i < rules.size(); ++i)
        {
            QPair<Token_struct,QList<Token_struct> > p = rules.value(i);

            if(p.second.size() == data.size())
            {
                bool failed = false;

                for(int y = 0; y < data.size(); ++y)
                {
                    if(p.second.value(y).token != data.value(y).token)
                    {
                        failed = true;
                        break;
                    }
                }

                if(!failed)
                {
                    Nonterminal* n = new Nonterminal();
                    n->ruleNumber = i;
                    n->id = nonterminals.size();

                    if(i == 8) // E -> fnc E
                    {
                        n->fncType = data.first().fncType; // we need to store function type eg. sin, cos...
                    }

                    foreach(Token_struct s, data)
                    {
                        if(s.token == NONTERM)
                        {
                            n->idNonterm.append(s.nontermId);
                        }
                        else if(s.token == IDENTIFIER)
                        {
                            n->number = s.number;
                            n->specVarId = s.specVarId;
                            n->isSpecVariable = s.isSpecVar;
                        }
                    }

                    nonterminals.append(n);

                    // set id to new nonterminal
                    Token_struct ret = p.first;
                    ret.nontermId = n->id;

                    return ret;
                }
            }
        }

        return Token_struct(ERROR_CELL);
    }

    /**
     * @brief setNumber Set number in string to token struct union.
     * @param number Number in string what we want set.
     * @return Token struct where we want set number.
     */
    Token_struct setNumberToken(QString number)
    {
        Token_struct token(IDENTIFIER);

        switch(type)
        {
        case INT:
            token.number = number.toInt();
            break;
        case UINT:
            token.number = number.toUInt();
            break;
        case FLOAT:
            token.number = number.toFloat();
            break;
        case DOUBLE:
            token.number = number.toDouble();
            break;
        default:
            break;
        }

        return token;
    }

    /**
     * @brief isNumber Test if number is in given string. If it is save this number in tokens.
     * @param number String with number.
     * @param wasNumber If here is number.
     */
    inline void isNumber(QString number, bool wasNumber) {
        if(wasNumber)
            tokens.append(setNumberToken(number));
    }

    //Token_struct saveNumber(QString number);

private:
    const int maxButtonId;
    QString origin;
    bool hasVariables;
    bool isValid;
    Type type;
    QList<Token_struct> tokens;
    QVariant result;
    Token priorityTable[EMPTY+1][EMPTY+1];
    //QHash<Token_struct,QList<Token_struct> > rules;
    QList<QPair<Token_struct,QList<Token_struct> > > rules;
    QList<Nonterminal*> nonterminals;
    //QList<SpecVariable*> specVars;
    QList<SpecialVariableAbstract<T>* > specVars;
};

#endif // UNIFORMCELL_H
