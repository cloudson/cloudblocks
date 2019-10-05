D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*
%{
#include <stdio.h>
#include "tokens.h"

void count();
void comment();
%}
%%


"break"			{ count(); return(T_BREAK); }
"case"			{ count(); return(T_CASE); }
"char"			{ count(); return(T_CHAR); }
"continue"		{ count(); return(T_CONTINUE); }
"default"		{ count(); return(T_DEFAULT); }
"else"			{ count(); return(T_ELSE); }
"do"			{ count(); return(T_DO); }
"double"		{ count(); return(T_DOUBLE); }
"enum"			{ count(); return(T_ENUM); }
"float"			{ count(); return(T_FLOAT); }
"for"			{ count(); return(T_FOR); }
"if"			{ count(); return(T_IF); }
"int"			{ count(); return(T_INT); }
"long"			{ count(); return(T_LONG); }
"return"		{ count(); return(T_RETURN); }
"short"			{ count(); return(T_SHORT); }
"signed"		{ count(); return(T_SIGNED); }
"sizeof"		{ count(); return(T_SIZEOF); }
"static"		{ count(); return(T_STATIC); }
"struct"		{ count(); return(T_STRUCT); }
"switch"		{ count(); return(T_SWITCH); }
"typedef"		{ count(); return(T_TYPEDEF); }
"union"			{ count(); return(T_UNION); }
"unsigned"		{ count(); return(T_UNSIGNED); }
"void"			{ count(); return(T_VOID); }
"while"			{ count(); return(T_WHILE); }
"#include"		{ count(); return(T_INCLUDE); }
"printf"		{ count(); return(T_PRINTF); }
"scanf"	 		{ count(); return(T_SCANF); }




{L}({L}|{D})*		{ count(); return(T_ID); }
0[xX]{H}+{IS}?		{ count(); return(T_CONSTANT); }
0{D}+{IS}?		{ count(); return(T_CONSTANT); }
{D}+{IS}?		{ count(); return(T_CONSTANT); }
L?'(\\.|[^\\'])+'	{ count(); return(T_CONSTANT); }

{D}+{E}{FS}?		{ count(); return(T_CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ count(); return(T_CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ count(); return(T_CONSTANT); }

L?\"(\\.|[^\\"])*\"	{ count(); return(T_STRING_LITERAL); }
\/\/.*\n { comment(); } 
"+="		{ count(); return(T_ADD_ASSIGN); }
"-="		{ count(); return(T_SUB_ASSIGN); }
"*="		{ count(); return(T_MULT_ASSIGN); }
"/="		{ count(); return(T_DIV_ASSIGN); }
"%="		{ count(); return(T_MOD_ASSIGN); }
"++"		{ count(); return(T_INC); }
"--"		{ count(); return(T_DEC); }
"->"		{ count(); return(T_PTR); }
"&&"		{ count(); return(T_AND); }
"||"		{ count(); return(T_OR); }
"<="		{ count(); return(T_LESS_EQ); }
">="		{ count(); return(T_GREATER_EQ); }
"=="		{ count(); return(T_EQUAL); }
"!="		{ count(); return(T_NOT_EQUAL); }
";"			{ count(); return(T_SEMICOLON); }
"{"		    { count(); return(T_BEGIN); }
"}"		    { count(); return(T_END); }
","			{ count(); return(T_COMMA); }
":"			{ count(); return(T_DOT_DOUBLE); }
"="			{ count(); return(T_ASSIGN); }
"("			{ count(); return(T_PARENT_L); }
")"			{ count(); return(T_PARENT_R); }
"["  		{ count(); return(T_BRACKET_L); }
"]"	    	{ count(); return(T_BRACKET_R); }
"."			{ count(); return(T_DOT); }
"&"			{ count(); return(T_ADDRESS); }
"!"			{ count(); return(T_NEG); }
"-"			{ count(); return(T_SUB); }
"+"			{ count(); return(T_ADD); }
"*"			{ count(); return(T_MULT); }
"/"			{ count(); return(T_DIV); }
"%"			{ count(); return(T_MOD); }
"<"			{ count(); return(T_LESS); }
">"			{ count(); return(T_GREATER); }
"|" 		{ count(); return(T_PIPE); }
"?"			{ count(); return(T_QUEST); }
"^"         {count();  return(T_XOR); } 
"<<"         {count();  return(T_LEFT); } 
">>"         {count();  return(T_RIGHT); } 


[ \t\v\n\f]		{ count(); }
.			{ /* ignore bad characters */ }

%%

int yywrap()
{
	return(1);
}

void comment()
{
	// its a comment
}

int column = 0;
int current_line = 1;
void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n'){
			column = 0;
			current_line++;
		}else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;

	//ECHO;
}

int get_current_line()
{
	return current_line;
}


