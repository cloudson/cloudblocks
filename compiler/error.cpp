#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "error.hpp"
#include <vector>




// singleton is a pattern that define once intance of 
// a class, we need this to use ErrorManager in 
// syntax, semantic ...


ErrorManager& ErrorManager::getInstance()
{
	static ErrorManager instance;
	instance.set_syntax_ok(true);
	instance.set_semantic_ok(true);
	return instance;
}

bool ErrorManager::is_syntax_ok() {
	return syntax_ok;
}

bool ErrorManager::is_semantic_ok() {
	return semantic_ok;
}

void ErrorManager::set_syntax_ok(bool val) {
	this->syntax_ok = val;
}

void ErrorManager::set_semantic_ok(bool val) {
	this->semantic_ok = val;
}


void ErrorManager::throw_syntax(int token_expected, int token_found, int line)
{
	this->syntax_ok = false;
	char error[255];
	sprintf(error, "Syntax error at line %d : Expected %s and found %s \n",line, this->get_token_name(token_expected), this->get_token_name(token_found));
	// todo, save erros to return as json
	this->errors.push_back(error);
	fprintf(stdout, "%s", error);
}

void ErrorManager::throw_syntax(int* tokens, int token_found, int line)
{
	// we need get each token name and generate a string to the message error
	this->syntax_ok = false;
	char error[255];
	char token_names[255] = "";
	char current_token[255] = "";
	int count = 1 ;
	for (int i=0; tokens[i] != -1; i++) {
		if (count > 1) {
			if (tokens[i + 1] == -1) {
				sprintf(current_token, " or %s", this->get_token_name(tokens[i]));	
			} else {
				sprintf(current_token, ", %s", this->get_token_name(tokens[i]));		
			}
			
		} else {
			sprintf(current_token, "%s", this->get_token_name(tokens[i]));	
		}
		strcat(token_names, current_token);
		count++;
	}
	sprintf(error, "Syntax error at line %d : Expected (%s) and found %s \n",line, token_names, this->get_token_name(token_found));
	this->errors.push_back(error);
	fprintf(stdout, "%s", error);
}

void ErrorManager::throw_semantic(int errorno, int line, const char* info1, const char* info2, const char* info3) 
{
	this->semantic_ok = false;
	char error[255] = "";
	switch(errorno) {
		case E_REDECLARING_VAR: sprintf(error, "Redeclaring var '%s' on line %d\n", info1, line); break;
		case E_INVALID_ASSIGN:  sprintf(error, "Expected a '%s' type and found a '%s' type in the assign on line %d\n", info1, info2, line); break; 
		case E_UNDECLARED_VAR:  sprintf(error, "Var '%s' undecladed on line %d\n", info1, line); break;
		case E_UNDECLARED_ATTR:  sprintf(error, "Attribute '%s' undecladed to '%s' type on line %d\n", info1, info2, line); break;
		case E_REDECLARING_FUNC: sprintf(error, "Redeclaring function '%s' on line %d\n", info1, line); break;
		case E_INVALID_RETURN: sprintf(error, "Invalid return, expected '%s' and found '%s' type  on line %d\n", info1, info2, line); break;
		case E_REDECLARING_STRUCT: sprintf(error, "Redeclaring type '%s' on line %d\n", info1, line); break;
		case E_UNDECLARED_STRUCT:  sprintf(error, "Type '%s' undecladed on line %d\n", info1, line); break;
		case E_UNDECLARED_FUNC:  sprintf(error, "Function '%s' undecladed on line %d\n", info1, line); break;
		case E_EXPECTED_RETURN:  sprintf(error, "Expected return '%s' type on line %d\n", info1,line); break;
		case E_IF_EXPRESSION:  sprintf(error, "Expected expression in if statement on line %d\n", line); break;
		case E_INVALID_CALL:  sprintf(error, "No matching function for call, candidate is %s on line %d\n",info1, line); break;
		case E_UNDEFINED_VECTOR:  sprintf(error, "Var '%s' is a '%s' type instead of vector on line %d\n",info1, info2, line); break;
		case E_INVALID_DIMENSION:  sprintf(error, "Vector '%s' using invalid dimension on line %d\n",info1, line); break;
		case E_INVALID_FORMAT_SCANF:  sprintf(error, "Invalid format in scanf on line %d\n", line); break;
		case E_INVALID_ACCESS:  sprintf(error, "Invalid access with var '%s' on line %d\n",info1, line); break;
		case E_ATTRIBUTTE_INVALID:  sprintf(error, "Invalid attribute '%s' in type %s on line %d\n",info1, info2, line); break;
	}
	this->errors.push_back(error);
	fprintf(stdout, "%s", error);
}

const char* ErrorManager::get_token_name(int token) {
	switch(token) {
		case T_EOF   : return "T_EOF";
		case T_BREAK : return "T_BREAK";
		case T_CASE  : return "T_CASE";
		case T_CHAR  : return "T_CHAR";
		case T_CONTINUE: return "T_CONTINUE";
		case T_DEFAULT : return "T_DEFAULT";
		case T_DOUBLE  : return "T_DOUBLE";
		case T_ELSE    : return "T_ELSE";
		case T_FLOAT   : return "T_FLOAT";
		case T_FOR   : return "T_FOR";
		case T_IF   : return "T_IF";
		case T_INT  : return "T_INT";
		case T_RETURN : return "T_RETURN";
		case T_SIZEOF : return "T_SIZEOF";
		case T_SWITCH : return "T_SWITCH";
		case T_STRUCT : return "T_STRUCT";
		case T_ADD_ASSIGN : return "T_ADD_ASSIGN";
		case T_ADD   : return "T_ADD";
		case T_SUB_ASSIGN: return "T_SUB_ASSIGN";
		case T_SUB  : return "T_SUB";
		case T_MULT_ASSIGN : return "T_MULT_ASSIGN";
		case T_MULT : return "T_MULT";
		case T_DIV  : return "T_DIV";
		case T_DIV_ASSIGN  : return "T_DIV_ASSIGN";
		case T_MOD  : return "T_MOD";
		case T_MOD_ASSIGN  : return "T_MOD_ASSIGN";
		case T_INC  : return "T_INC";
		case T_DEC  : return "T_DEC";
		case T_PTR  : return "T_PTR";
		case T_AND  : return "T_AND";
		case T_OR  : return "T_OR";
		case T_BEGIN  : return "T_BEGIN";
		case T_END   : return "T_END";
		case T_COMMA : return "T_COMMA";
		case T_SEMICOLON: return "T_SEMICOLON";
		case T_QUEST  : return "T_QUEST";
		case T_LESS  : return "T_LESS";
		case T_LESS_EQ : return "T_LESS_EQ";
		case T_GREATER : return "T_GREATER";
		case T_GREATER_EQ : return "T_GREATER_EQ";
		case T_EQUAL  : return "T_EQUAL";
		case T_NOT_EQUAL: return "T_NOT_EQUAL";
		case T_DOT  : return "T_DOT";
		case T_BRACKET_L: return "T_BRACKET_L";
		case T_BRACKET_R: return "T_BRACKET_R";
		case T_PARENT_L : return "T_PARENT_L";
		case T_PARENT_R : return "T_PARENT_R";
		case T_WHILE  : return "T_WHILE";
		case T_VOID  : return "T_VOID";
		case T_LITERAL : return "T_LITERAL";
		case T_CONSTANT : return "T_CONSTANT";
		case T_ID  : return "T_ID";
		case T_ENUM : return "T_ENUM";
		case T_SHORT : return "T_SHORT";
		case T_SIGNED : return "T_SIGNED";
		case T_UNSIGNED  : return "T_UNSIGNED";
		case T_STRING_LITERAL : return "T_STRING_LITERAL";
		case T_LONG : return "T_LONG";
		case T_STATIC : return "T_STATIC";
		case T_TYPEDEF : return "T_TYPEDEF";
		case T_UNION : return "T_UNION";
		case T_DO : return "T_DO";
		case T_ADDRESS : return "T_ADDRESS";
		case T_DOT_DOUBLE : return "T_DOT_DOUBLE";
		case T_NEG : return "T_NEG";
		case T_ASSIGN : return "T_ASSIGN";
		case T_PIPE : return "T_PIPE";
		case T_XOR : return "T_XOR";
		case T_LEFT : return "T_LEFT";
		case T_RIGHT : return "T_RIGHT";
		case T_SHARP : return "T_SHARP";
		case T_INCLUDE : return "T_INCLUDE";
		case T_PRINTF : return "T_PRINTF";
		case T_SCANF :  return "T_SCANF";
		case T_MINUS :  return "T_MINUS";
	}
	return " not found ";
}