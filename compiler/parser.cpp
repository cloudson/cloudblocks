#include <iostream>
#include "tokens.h"
#include "error.hpp"
#include "scanner.h"
#include "ast.h"
#include "parser.hpp"
#include "helper.hpp"
#include "symbols_tables.h"
#include <typeinfo>

extern int yylex();
extern int get_current_line();

char stack_last[255];
void Parser::stack_print(const char* method){
	strcpy(stack_last, method);
	if (CLOUD_DEBUG) fprintf(stdout,"%s\n", method);
}

Parser::Parser(){
	if (CLOUD_DEBUG) fprintf(stdout,"criando o parser \n");
	this->get_next_token();
	this->error = ErrorManager::getInstance();
}

Parser::~Parser(){
	if (CLOUD_DEBUG) printf("destruindo o parser");
}

int Parser::get_current_token(){
	return this->current_token;
}

int Parser::get_next_token(){
	this->current_token = yylex();
	
	return this->current_token;
}

const char* Parser::get_lexeme() {
	return yytext;
}

void Parser::eat() {
	if (CLOUD_DEBUG) {
		if (this->get_current_token() == T_ID || 
			this->get_current_token() == T_CONSTANT || 
			this->get_current_token() == T_STRING_LITERAL) {
			printf("- eating %s (%s) \n", this->error.get_token_name(this->get_current_token()), this->get_lexeme()); 
		} else {
			fprintf(stdout,"- eating %s \n", this->error.get_token_name(this->get_current_token())); 	
		}	
	}
	
	this->get_next_token();
}

void Parser::skip(int* synch){
	while(this->get_current_token() != T_EOF){
        for(int i=0; synch[i] != T_EOF; i++)
            if(synch[i] == this->get_current_token()) return;
        this->get_next_token();
    }
}

void Parser::eat_or_skip(int token, int* synch){
	if (this->get_current_token() == token) {
		this->eat();
		return;
	}
	this->syntax_error(token);
	skip(synch);
}

node_program* Parser::get_AST(){
	return this->g_program();
}

void Parser::syntax_error(int token)
{
	this->error.throw_syntax(token, this->get_current_token(), get_current_line());
}

void Parser::syntax_error(int* tokens)
{
	this->error.throw_syntax(tokens, this->get_current_token(), get_current_line());
}


// =================================================== Gramática ==================================================
static int synch_g_program[] = { T_EOF };  
node_program* Parser::g_program(){
	this->stack_print("g_program");
	this->ast = new node_program(NULL,NULL,NULL, get_current_line());
	node_include* include_list = this->g_include();
	this->g_main(this->ast);

	this->eat_or_skip(T_EOF, synch_g_program);
	return this->ast;
}

static int synch_g_include[] = { T_INT, T_CHAR, T_ID, T_FLOAT, T_VOID, T_EOF }; 
node_include* Parser::g_include(){
	this->stack_print("g_include");
	if (this->get_current_token() == T_INCLUDE) {
		this->eat();
		return this->g_include_int();		
	}
	return NULL;
}

static int synch_g_include_int[] = { T_INT, T_CHAR, T_ID, T_FLOAT, T_VOID, T_EOF }; 
node_include* Parser::g_include_int(){
	this->stack_print("g_include_int");
	node_include* include;
	if (this->get_current_token() == T_LESS) {
		this->eat();
		include = new node_include(get_lexeme());
		this->eat_or_skip(T_ID, synch_g_include_int);
		this->g_include_root(include);
		this->eat_or_skip(T_GREATER, synch_g_include_int);		
		include->attr_next = this->g_include();

		return include;
	}

	if (this->get_current_token() == T_STRING_LITERAL) {
		include = new node_include(get_lexeme());
		this->eat_or_skip(T_STRING_LITERAL, synch_g_include_int);
		include->attr_next = this->g_include();

		return include; 
	}
	this->syntax_error(T_LESS);
	this->skip(synch_g_include_int);
	return NULL;
} 

static int synch_g_include_root[] = { T_INCLUDE, T_INT, T_CHAR, T_VOID, T_FLOAT, T_ID, T_EOF};
void Parser::g_include_root(node_include* include){
	this->stack_print("g_include_root");
	if (this->get_current_token() == T_DOT) {
		this->eat();
		include->attr_file = Helper::str_cat(include->attr_file, ".");
		include->attr_file = Helper::str_cat(include->attr_file, get_lexeme());
		this->eat_or_skip(T_ID, synch_g_include_root);
	}
}

void Parser::g_main(node_program* program){
	this->stack_print("g_main");
	this->g_decl(program);
	if (this->get_current_token() == T_VOID ||
		this->get_current_token() == T_ID  || 
		this->get_current_token() == T_CHAR || 
		this->get_current_token() == T_INT ||
		this->get_current_token() == T_FLOAT || 
		this->get_current_token() == T_ID || 
		this->get_current_token() == T_STRUCT || 
		this->get_current_token() == T_TYPEDEF ) {

		this->g_main(program);
	}
} 
static int synch_g_decl[] = { T_INT, T_CHAR, T_VOID, T_FLOAT, T_EOF};
static int synch_g_decl2[] = { T_SEMICOLON, T_INT, T_CHAR, T_VOID, T_FLOAT, T_EOF};
void Parser::g_decl(node_program* program){
	this->stack_print("g_decl");
	if (this->get_current_token() == T_STRUCT) {
		program->insert_struct(this->g_struct());
		this->eat_or_skip(T_SEMICOLON, synch_g_decl);
		return;
	}

	if (this->get_current_token() == T_TYPEDEF) {
		this->eat();
		node_struct* str = this->g_struct();
		str->attr_typedef = true;
		str->attr_id = new Symbol_id(get_lexeme());
		this->eat_or_skip(T_ID, synch_g_decl2);
		program->insert_struct(str);
		this->eat_or_skip(T_SEMICOLON, synch_g_decl);
		return;	
	}

	node_type* type = this->g_type();
	type->attr_pointer_level = this->g_pointer();
	Symbol_id* symbol = new Symbol_id(get_lexeme());
	this->eat_or_skip(T_ID, synch_g_decl);
	this->g_func_or_var(program, type, symbol);	
} 

node_type* Parser::g_type(){
	this->stack_print("g_type");
	if (this->get_current_token() == T_ID  || 
		this->get_current_token() == T_CHAR || 
		this->get_current_token() == T_INT ||
		this->get_current_token() == T_FLOAT || 
		this->get_current_token() == T_ID || 
		this->get_current_token() == T_VOID ) {
		int token_type = this->get_current_token();
		node_type* type;
		if (token_type == T_ID) {
			Symbol_id* symbol = new Symbol_id(get_lexeme());
			type = new node_type(symbol, get_current_line());
			type->attr_struct = true;
		} else {
			type = new node_type(token_type, get_current_line());
		}
		this->eat();
		return type;
	}
	
	int first[6] = {T_ID, T_CHAR, T_VOID, T_FLOAT, T_INT, -1};
	this->syntax_error(first);

	return NULL;
}

int Parser::g_pointer(int level){
	this->stack_print("g_pointer");
	if (this->get_current_token() == T_MULT) {
		this->eat();
		return this->g_pointer(level + 1);
	}
	return level;
}

static int synch_g_struct[] = {T_BEGIN, T_EOF};
static int synch_g_struct1[] = {T_SEMICOLON, T_STRUCT, T_END, T_INT, T_FLOAT, T_VOID, T_CHAR, T_ID, T_EOF};
static int synch_g_struct2[] = {T_SEMICOLON, T_STRUCT, T_INT, T_FLOAT, T_VOID, T_CHAR, T_ID, T_EOF};
static int synch_g_struct3[] = {T_STRUCT, T_INT, T_FLOAT, T_VOID, T_CHAR, T_ID, T_EOF};
node_struct* Parser::g_struct() {
	node_struct* str = new node_struct(NULL, NULL, false, get_current_line());
	this->eat_or_skip(T_STRUCT, synch_g_struct);
	str->attr_id = new Symbol_id(get_lexeme());
	this->eat_or_skip(T_ID, synch_g_struct1);
	this->eat_or_skip(T_BEGIN, synch_g_struct1);
	str->attr_fields = this->g_struct_fields();
	this->eat_or_skip(T_END, synch_g_struct2);

	return str;
}

static int synch_g_struct_fields[] = {T_ID, T_MULT, T_SEMICOLON, T_EOF};
static int synch_g_struct_fields2[] = {T_SEMICOLON, T_MULT, T_EOF};
static int synch_g_struct_fields3[] = {T_STRUCT, T_INT, T_FLOAT, T_VOID, T_CHAR, T_ID, T_EOF};
node_var_list*  Parser::g_struct_fields() {
	node_var_list *var_list;
	node_var_list* p = var_list;
	int first[6] = {T_INT, T_CHAR, T_FLOAT, T_STRUCT, T_ID, -1};
	if(this->get_current_token() == T_INT || 
		this->get_current_token() == T_CHAR || 
		this->get_current_token() == T_FLOAT || 
		this->get_current_token() == T_STRUCT || 
		this->get_current_token() == T_ID) {
		
		if (this->get_current_token() == T_STRUCT) {
			this->eat();
			Symbol_id* name_of_type = new Symbol_id(get_lexeme());
			node_type* type = new node_type(name_of_type, get_current_line());
			type->attr_struct = true;
			this->eat_or_skip(T_ID, synch_g_struct_fields);
			type->attr_pointer_level = this->g_pointer();
			Symbol_id* id = new Symbol_id(get_lexeme());
			this->eat_or_skip(T_ID, synch_g_struct_fields2);
			node_name_decl* name = new node_name_decl(type,id,get_current_line());
			var_list = new node_var_list(name, NULL, get_current_line());
			this->eat_or_skip(T_SEMICOLON, synch_g_struct_fields3);
		} else {
			node_type* type = this->g_type();
			type->attr_pointer_level = this->g_pointer();
			Symbol_id* symbol = new Symbol_id(get_lexeme());
			this->eat_or_skip(T_ID, synch_g_decl);
			var_list = this->g_rvar(type, symbol);
		}

		if (this->get_current_token() == T_INT || 
			this->get_current_token() == T_CHAR || 
			this->get_current_token() == T_FLOAT || 
			this->get_current_token() == T_STRUCT || 
			this->get_current_token() == T_ID) {
			var_list->attr_next = this->g_struct_fields();			
		}

		return var_list;

	}
	this->syntax_error(first);
	return NULL;
}


static int synch_g_func_or_var[] = {T_INT, T_CHAR, T_VOID, T_FLOAT, T_EOF};
void Parser::g_func_or_var(node_program* program, node_type* type, Symbol_id* symbol){
	this->stack_print("g_func_or_var");
	if (this->get_current_token() == T_PARENT_L) {
		int line = get_current_line();
		this->eat();
		node_var_list* params = this->g_params();
		this->eat_or_skip(T_PARENT_R, synch_g_func_or_var);
		node_stmt_list* stmt_list = this->g_scope();
		program->insert_func(new node_func(type, symbol, params, stmt_list, NULL, line));
		return;
	}

	node_var_list* var_list = this->g_rvar(type, symbol);
	program->insert_var(var_list);
}


static int synch_g_rvar[] = { T_SEMICOLON, T_EOF};
static int synch_g_rvar2[] = { T_ID, T_INT, T_CHAR, T_FLOAT, T_IF, T_SWITCH, T_FOR, T_WHILE, T_SCANF, T_PRINTF, T_EOF};
static int synch_g_rvar3[] = { T_SEMICOLON, T_ASSIGN, T_BRACKET_L, T_EOF};
node_var_list* Parser::g_rvar(node_type* type, Symbol_id* symbol){
	this->stack_print("g_rvar");
	node_name_decl* decl = new node_name_decl( type, symbol,get_current_line());
	int first[5] = {T_SEMICOLON, T_COMMA, T_ASSIGN, T_BRACKET_L, -1};
	switch (this->get_current_token()) {
		case T_SEMICOLON:
			this->eat();
			return new node_var_list(decl,NULL, get_current_line());;

		case T_COMMA:{
			this->eat();
			node_var_list* var_list = new node_var_list(decl,NULL, get_current_line());
			Symbol_id* symbol2 = new Symbol_id(get_lexeme());
			this->eat_or_skip(T_ID, synch_g_rvar);
			var_list->attr_next = this->g_rvar(type, symbol2);
			return var_list;
		}
		case T_ASSIGN:{
			this->eat();
			node_name* name = new node_name(symbol, get_current_line());
			node_expr* expr = this->g_expr(); 
			decl->attr_assign = new node_assign(name, expr, get_current_line());
			node_var_list* var_list2 = new node_var_list(decl,NULL, get_current_line());
			if (this->get_current_token() == T_COMMA) {
				var_list2->attr_next = this->g_rvar(type, symbol);
				return var_list2;
			}
			this->eat_or_skip(T_SEMICOLON, synch_g_rvar2);
			return var_list2;
		}
		case T_BRACKET_L: {
			// a array is similar to pointer, so we will pointer_level++
			this->eat();
			node_expr* expr = this->g_expr();
			this->eat_or_skip(T_BRACKET_R, synch_g_rvar3);
			type->attr_pointer_level++;
			type->attr_length = new node_length(expr, get_current_line());
			type->attr_array = true;
			node_length* p = type->attr_length;
			while(this->get_current_token() == T_BRACKET_L){
				this->eat();
				p->attr_next = new node_length(this->g_expr(), get_current_line());
				p = p->attr_next;
				if (this->get_current_token() != T_BRACKET_R) {
					this->syntax_error(first);
					break;
				}
				this->eat();
			}
			return this->g_rvar(type, symbol);
		}
	}
	this->syntax_error(first);

	return NULL;
} 

static int synch_g_scope[] = { T_IF, T_WHILE, T_FOR, T_SWITCH, T_ID, T_INT, T_CHAR, T_FLOAT, T_SCANF, T_PRINTF, T_EOF };
node_stmt_list* Parser::g_scope(){
	this->stack_print("g_scope");
	if (this->get_current_token() == T_BEGIN) {
		this->eat();
		node_stmt_list* stmt_list = this->g_stmt_list();
		this->eat_or_skip(T_END, synch_g_scope);
		return stmt_list;
	}
	node_stmt* stmt = this->g_stmt();
	return new node_stmt_list(stmt, NULL, get_current_line());
}

node_stmt_list* Parser::g_stmt_list(){
	this->stack_print("g_stmt_list");
	node_stmt_list* stmt_list = new node_stmt_list(NULL, NULL, get_current_line());
	switch (this->get_current_token()) {
		case T_IF:
		case T_WHILE:
		case T_FOR:
		case T_ID:
		case T_SWITCH:
		case T_PRINTF:
		case T_SCANF:
		case T_INT:
		case T_FLOAT:
		case T_VOID:
		case T_CHAR:
		case T_RETURN:
			stmt_list->attr_stmt = this->g_stmt();
			stmt_list->attr_next = this->g_stmt_list();
			
			return stmt_list;
	}

	return NULL;
}

static int synch_g_stmt_5[] = { T_ID, T_STRING_LITERAL, T_PARENT_R, T_EOF};
static int synch_g_stmt_4[] = { T_ID, T_PARENT_R, T_BEGIN, T_SEMICOLON, T_EOF};
static int synch_g_stmt_3[] = { T_INT, T_FLOAT, T_CHAR, T_ID, T_SEMICOLON, T_EOF};
static int synch_g_stmt_2[] = { T_WHILE, T_FOR, T_RETURN, T_SCANF, T_PRINTF, T_INT, T_FLOAT, T_CHAR, T_SWITCH, T_IF,T_BEGIN, T_EOF};
static int synch_g_stmt_1[] = { T_PARENT_R, T_STRING_LITERAL, T_CONSTANT, T_ID, T_ADD, T_SUB, T_NEG, T_EOF};
node_stmt* Parser::g_stmt(){
	this->stack_print("g_stmt");
	switch (this->get_current_token()) {
		case T_IF: {
			this->eat();
			this->eat_or_skip(T_PARENT_L, synch_g_stmt_1);	 
			node_expr* expr = this->g_expr();
			this->eat_or_skip(T_PARENT_R,synch_g_stmt_2);
			node_stmt_list* stmt_list = this->g_scope();
			node_stmt_list* stmt_list_else = this->g_else();
			return new node_if(expr, stmt_list, stmt_list_else, get_current_line());
		}
		case T_WHILE: {
			this->eat();
			this->eat_or_skip(T_PARENT_L, synch_g_stmt_1);
			node_expr* expr = this->g_expr();
			this->eat_or_skip(T_PARENT_R,synch_g_stmt_2);
			node_stmt_list* stmt_list = this->g_scope();

			return new node_while(expr, stmt_list, get_current_line());
		}
		case T_FOR: {
			this->eat();
			this->eat_or_skip(T_PARENT_L, synch_g_stmt_3);
			node_stmt* init = this->g_for_init();
			this->eat_or_skip(T_SEMICOLON, synch_g_stmt_1);
			node_expr* expr = this->g_expr();
			this->eat_or_skip(T_SEMICOLON, synch_g_stmt_4);
			node_expr_list* inc = this->g_for_int_inc();
			this->eat_or_skip(T_PARENT_R,synch_g_stmt_2);
			node_stmt_list* scope = this->g_scope();
			
			return new node_for(init, expr, inc, scope, get_current_line());
		}
		case T_ID: {
			Symbol_id* symbol = new Symbol_id(get_lexeme());
			node_type* type = new node_type(symbol, get_current_line());
			type->attr_struct = true;
			this->eat();
			return this->g_decl_id(type);
		}

		// case T_STRUCT: {
		// 	this->eat();
		// 	Symbol_id* symbol = new Symbol_id(get_lexeme());
		// 	node_type* type = new node_type(symbol, get_current_line());
		// 	type->attr_struct = true;
		// 	this->eat_or_skip(T_ID, synch_g_stmt_2);
		// 	return this->g_decl_id(type);
		// }
		case T_SWITCH:
		case T_PRINTF: {
			this->eat();
			this->eat_or_skip(T_PARENT_L,synch_g_stmt_5);	
			node_expr_list* expr_list = new node_expr_list(NULL, NULL, get_current_line());
			expr_list->attr_expr = new node_primitive(T_STRING_LITERAL, get_lexeme(), get_current_line());
			this->eat_or_skip(T_STRING_LITERAL, synch_g_stmt_5);
			if (this->get_current_token() == T_COMMA) {
				this->eat();
				expr_list->attr_next = this->g_expr_list();
			}
			this->eat_or_skip(T_PARENT_R, synch_g_stmt_2);
			this->eat_or_skip(T_SEMICOLON, synch_g_stmt_2);
			return new node_print(expr_list, get_current_line());
		}
		case T_SCANF: {
			this->eat();
			this->eat_or_skip(T_PARENT_L,synch_g_stmt_5);
			node_expr* format = new node_primitive(T_STRING_LITERAL, get_lexeme(), get_current_line());
			this->eat_or_skip(T_STRING_LITERAL, synch_g_stmt_5);
			this->eat_or_skip(T_COMMA, synch_g_stmt_5);
			int address = 0;
			if (this->get_current_token() == T_ADDRESS) {
				this->eat();
				address = 1;
			}
			Symbol_id* symbol = new Symbol_id(get_lexeme());
			node_expr* expr = new node_name(symbol, get_current_line());
			if (address) {
				// @todo which differences of pointer or value in javascript ? 
			}
			this->eat_or_skip(T_ID,synch_g_stmt_4);
			this->eat_or_skip(T_PARENT_R, synch_g_stmt_2);
			this->eat_or_skip(T_SEMICOLON, synch_g_stmt_2);
			
			return new node_read(format, expr, get_current_line());
		}
		case T_INT:
		case T_FLOAT:
		case T_VOID:
		case T_CHAR: {
			int token = this->get_current_token();
			this->eat();
			int pointer = this->g_pointer();
			node_type* type = new node_type(token, pointer, 0, get_current_line());
			Symbol_id* symbol = new Symbol_id(get_lexeme());
			this->eat_or_skip(T_ID, synch_g_stmt_2);
			
			return this->g_rvar(type, symbol);		
		}
		case T_RETURN: {
			this->eat();
			node_expr* expr = NULL;
			if (this->get_current_token() != T_SEMICOLON) {
				expr = this->g_expr();
			}
			this->eat_or_skip(T_SEMICOLON, synch_g_stmt_2);
			return new node_return(expr, get_current_line());
		}
	}
	return NULL;
}

node_stmt_list* Parser::g_else(){
	this->stack_print("g_else");
	if (this->get_current_token() == T_ELSE) {
		this->eat();
		return this->g_scope();
	}
	return NULL;
}
void Parser::g_printf_int(){
	// @deprecated
	this->stack_print("g_printf_int");
}

node_stmt* Parser::g_for_init(){
	this->stack_print("g_for_init");
	node_stmt_list* stmt_list = new node_stmt_list(NULL, NULL, get_current_line());
	node_type* type = this->g_for_int_type();
	if (this->get_current_token() == T_ID) {
		Symbol_id* symbol = new Symbol_id(get_lexeme());
		node_name* name = new node_name(symbol, get_current_line());
		this->eat();
		node_expr* expr = this->g_for_int_assign();
		if (expr == NULL && type == NULL) {
			// here, for had initialization without assign
			return name;
		}
		node_assign* assign = NULL;
		if (expr != NULL) {
			assign = new node_assign(name, expr, get_current_line());	
		} 
		return new node_name_decl(type, symbol, assign, get_current_line());
	}

	return NULL;
}

node_type* Parser::g_for_int_type(){
	this->stack_print("g_for_int_type");
	if (this->get_current_token() == T_CHAR  || 
		this->get_current_token() == T_INT   ||
		this->get_current_token() == T_VOID  || 
		this->get_current_token() == T_FLOAT ) {

		return this->g_type();
	}

	return NULL;
}

node_expr* Parser::g_for_int_assign(){
	this->stack_print("g_for_int_assign");
	if (this->get_current_token() == T_ASSIGN) {
		this->eat();
		return this->g_expr();
	}
	
	return NULL;
}

static int synch_g_for_int_inc[] = {T_PARENT_R, T_SEMICOLON, T_EOF};
node_expr_list* Parser::g_for_int_inc(){
	this->stack_print("g_for_int_inc");
	node_expr_list* expr_list = new node_expr_list(NULL, NULL, get_current_line()); 
	Symbol_id* symbol = new Symbol_id(get_lexeme());
	this->eat_or_skip(T_ID, synch_g_for_int_inc);	
	if (this->get_current_token() == T_INC || 
		this->get_current_token() == T_DEC) {
		int op = (this->get_current_token() == T_INC) ? T_ADD: T_SUB;
		node_primitive* primitive = new node_primitive(T_INT, "1", get_current_line());
		node_name* name = new node_name(symbol, get_current_line()); 
		node_op_addition* binop = new node_op_addition(op, name, primitive, get_current_line());
		expr_list->attr_expr = new node_assign(name, binop, get_current_line()); 
		this->g_for_int_inc_int();
		if (this->get_current_token() == T_COMMA) {
			this->eat();
			expr_list->attr_next = this->g_for_int_inc();
		}
		return expr_list;
		
	}
	int first[3] = {T_INC, T_DEC,-1};
	this->syntax_error(first);
	
	return NULL;	
}

void Parser::g_for_int_inc_int(){
	this->stack_print("g_for_int_inc_int");
	if (this->get_current_token() == T_INC || 
		this->get_current_token() == T_DEC) {
		this->eat();
	}
}

void Parser::g_cases(){
	this->stack_print("g_cases");
}

void Parser::g_case_value(){
	this->stack_print("g_case_value");
}

static int synch_g_params[] = { T_PARENT_R };
node_var_list* Parser::g_params(){
	this->stack_print("g_params");
	if (this->get_current_token() == T_ID    || 
		this->get_current_token() == T_CHAR  || 
		this->get_current_token() == T_INT   ||
		this->get_current_token() == T_VOID  || 
		this->get_current_token() == T_FLOAT || 
		this->get_current_token() == T_ID ) {
		node_type* type = this->g_type();
		type->attr_pointer_level = this->g_pointer();
		Symbol_id* symbol = NULL; 
		if (this->get_current_token() == T_ID) {
			symbol = new Symbol_id(get_lexeme());
		}
		node_name_decl* decl = new node_name_decl(type, symbol, get_current_line());
		this->eat_or_skip(T_ID, synch_g_params);
		type->attr_array = this->g_array();
		node_var_list* var = new node_var_list(decl, NULL, get_current_line());
		var->attr_next = this->g_params_int();	

		return var;
	}

	return NULL;
}

node_var_list* Parser::g_params_int(){
	this->stack_print("g_params_int");
	if (this->get_current_token() == T_COMMA) {
		this->eat();
		this->stack_print("g_params");
		if (this->get_current_token() == T_ID    || 
			this->get_current_token() == T_CHAR  || 
			this->get_current_token() == T_INT   ||
			this->get_current_token() == T_VOID  || 
			this->get_current_token() == T_FLOAT ) {
			return this->g_params();
		}
		int first[6] = {T_ID, T_CHAR, T_VOID, T_FLOAT, T_INT, -1};
		this->syntax_error(first);
	}
	return NULL;
}

static int synch_g_array[] = { T_PARENT_R, T_COMMA, T_EOF};
int Parser::g_array(int level){
	this->stack_print("g_array");
	if (this->get_current_token() == T_BRACKET_L) {
		this->eat();
		this->eat_or_skip(T_BRACKET_R,synch_g_array);
		return this->g_array(level + 1);
	}
	return level; 
}

static int synch_g_id_1[] = { T_SEMICOLON, T_IF, T_WHILE, T_FOR, T_STRING_LITERAL, T_CONSTANT, T_ID, T_INT, T_CHAR, T_FLOAT, T_SCANF, T_PRINTF, T_EOF };
static int synch_g_id_2[] = { T_SCANF, T_PRINTF, T_IF, T_WHILE, T_FOR, T_STRING_LITERAL, T_CONSTANT, T_ID, T_INT, T_CHAR, T_FLOAT, T_EOF};
static int synch_g_id_3[] = { T_SEMICOLON, T_PARENT_L, T_ASSIGN, T_BRACKET_L, T_EOF};
node_stmt* Parser::g_id(Symbol_id* symbol){
	this->stack_print("g_id");
	if (this->get_current_token() == T_PARENT_L) {
		this->eat();
		node_expr_list* expr_list = this->g_expr_list();
		this->eat_or_skip(T_PARENT_R,synch_g_id_1);
		this->eat_or_skip(T_SEMICOLON, synch_g_id_2);
		return new node_call(symbol, expr_list, get_current_line()); 
	}
	
	if (this->get_current_token() == T_ASSIGN || 
		this->get_current_token() == T_BRACKET_L) {
		node_vector* vector = this->g_array_access_optional();
		this->eat();
		node_expr* expr = this->g_expr();
		node_name* name = new node_name(symbol, get_current_line());
		node_assign* assign = new node_assign(name, expr, get_current_line());
		if (vector != NULL) {
			vector->attr_name = name;
			assign->attr_expr_left = vector; 
		}
		
		this->eat_or_skip(T_SEMICOLON, synch_g_id_2);
		
		return assign;
	}

	if (this->get_current_token() == T_DOT || 
		this->get_current_token() == T_PTR) {
		this->eat();
		node_name* object = new node_name(symbol, NULL, get_current_line());
		Symbol_id* symbol2 = new Symbol_id(get_lexeme());
		this->eat_or_skip(T_ID,synch_g_id_3);
		node_stmt* stmt = this->g_id_or_semicolon(symbol2);
		if (typeid(*stmt) == typeid(node_assign)){
			object->attr_expr = ((node_assign*)stmt)->attr_expr_left;
			
			return new node_assign(object, ((node_assign*)stmt)->attr_expr_right, get_current_line());	 
		} 
		object->attr_expr = (node_expr*)stmt;
		return object;
	}
	
	int first[6] = {T_PARENT_L, T_ASSIGN, T_BRACKET_L, T_DOT, T_PTR, -1};
	this->syntax_error(first);

	return NULL;
}

static int synch_g_decl_id[] = { T_PARENT_L, T_BRACKET_L,T_ASSIGN, T_SEMICOLON,T_EOF};
node_stmt* Parser::g_decl_id(node_type* type){
	this->stack_print("g_decl_id");
	if (this->get_current_token() == T_MULT || 
		this->get_current_token() == T_ID) {
		int pointer = this->g_pointer();
		Symbol_id* symbol = new Symbol_id(get_lexeme());
		this->eat_or_skip(T_ID, synch_g_decl_id);
		node_var_list* var_list = this->g_rvar(type, symbol);
		return var_list;
	}

	return this->g_id(type->attr_id);
}

node_stmt* Parser::g_id_or_semicolon(Symbol_id* symbol){
	this->stack_print("g_id_or_semicolon");
	if (this->get_current_token() == T_SEMICOLON) {
		this->eat();
		return new node_name(symbol, get_current_line());
	}

	return this->g_id(symbol);
}

node_vector* Parser::g_array_access_optional(){
	this->stack_print("g_array_access_optional");
	if (this->get_current_token() == T_BRACKET_L) {
		return this->g_array_access();
	}

	return NULL;
}

static int synch_g_array_access[] = { T_ASSIGN, T_EOF}; 
node_vector* Parser::g_array_access(){
	this->stack_print("g_array_access");
	if (this->get_current_token() == T_BRACKET_L) {
		this->eat();
		int line = get_current_line();
		node_expr* expr = this->g_expr();
		node_expr_list* expr_list = new node_expr_list(expr, NULL, line);
		node_vector* vector = new node_vector(NULL, expr_list, line);
		this->eat_or_skip(T_BRACKET_R, synch_g_array_access);
		node_vector* vector2 = this->g_array_access_optional();		
		vector->attr_expr_list->attr_next = (vector2) ? vector2->attr_expr_list : NULL;

		return vector;
	}

	return NULL;
}

node_expr_list* Parser::g_expr_list() {
	this->stack_print("g_expr_list");
	node_expr_list* expr_list = new node_expr_list(NULL, NULL, get_current_line()); 
	if (this->get_current_token() == T_ID  || 
		this->get_current_token() == T_STRING_LITERAL || 
		this->get_current_token() == T_CONSTANT ||
		this->get_current_token() == T_ADD || 
		this->get_current_token() == T_SUB || 
		this->get_current_token() == T_NEG  || 
		this->get_current_token() == T_PARENT_L) {
		expr_list->attr_expr = this->g_expr();
		if (this->get_current_token() == T_COMMA) {
			this->eat();
			expr_list->attr_next =this->g_expr_list();
		}
		return expr_list;
	}
	return NULL;
}

static int synch_g_expr[] = {T_ID, T_STRING_LITERAL, T_CONSTANT,T_EOF} ; 
node_expr* Parser::g_expr(){
	this->stack_print("g_expr");

	return this->g_b2();
}

static int synch_g_cast[] = { T_ID, T_STRING_LITERAL, T_CONSTANT, T_ADD, T_SUB, T_NEG, T_EOF};
node_expr* Parser::g_cast(){
	this->stack_print("g_cast");
	if (this->get_current_token() == T_PARENT_L) {
		this->eat();
		node_type* type = this->g_type();
		int pointer = this->g_pointer();
		this->eat_or_skip(T_PARENT_R, synch_g_cast);
		node_expr* expr = this->g_expr();
		this->g_cast_int();

		// @todo create node_cast
		return NULL;
	}

	return NULL;
}

node_expr* Parser::g_cast_int(){
	this->stack_print("g_cast_int");
	this->g_array_access();

	return NULL;
}

node_expr* Parser::g_expr_(){
	this->stack_print("g_expr_");
	if (this->get_current_token() == T_ASSIGN) {
		this->eat();
		this->g_b2();
		this->g_expr_();
	}

	return NULL;
}

node_expr* Parser::g_b2(){
	this->stack_print("g_b2");
	node_expr* expr = this->g_b3();
	if (this->get_current_token() == T_OR) {
		int op = this->get_current_token();
		int line = get_current_line();
		this->eat();
		node_expr* expr2 = this->g_b2();

		return new node_op_boolean(op, expr, expr2, line);
	}
	return expr;
}

node_expr* Parser::g_b3(){
	this->stack_print("g_b3");
	node_expr* expr = this->g_b4();
	if (this->get_current_token() == T_AND) {
		int op = this->get_current_token();
		int line = get_current_line();
		this->eat();
		node_expr* expr2 = this->g_b3();
		return new node_op_boolean(op, expr, expr2, line);
	}
	return expr;
}

node_expr* Parser::g_b4(){
	this->stack_print("g_b4");
	node_expr* expr = this->g_b5();
	if (this->get_current_token() == T_EQUAL ||
		this->get_current_token() == T_NOT_EQUAL ) {
		int op = this->get_current_token();
		int line = get_current_line();
		this->eat();
		node_expr* expr2 = this->g_b4();
		return new node_op_boolean(op, expr, expr2, line);
	}
	return expr;
}


node_expr* Parser::g_b5(){
	this->stack_print("g_b5");
	node_expr* expr = this->g_b6();
	if (this->get_current_token() == T_LESS ||
		this->get_current_token() == T_LESS_EQ ||
		this->get_current_token() == T_GREATER ||
		this->get_current_token() == T_GREATER_EQ ) {
		int op = this->get_current_token();
		int line = get_current_line();
		this->eat();
		node_expr* expr2 = this->g_b5();
		return new node_op_relational(op, expr, expr2, line);
	}
	return expr;
}


node_expr* Parser::g_b6(){
	this->stack_print("g_b6");
	node_expr* expr = this->g_b7();
	if (this->get_current_token() == T_ADD ||
		this->get_current_token() == T_SUB ||
		this->get_current_token() == T_PIPE ) {
		int op = this->get_current_token();
		int line = get_current_line();
		this->eat();
		node_expr* expr2 = this->g_b6();
		return new node_op_addition(op, expr, expr2, line);
	}
	return expr;
}

node_expr* Parser::g_b7(){
	this->stack_print("g_b7");
	node_expr* expr = this->g_b8();
	if (this->get_current_token() == T_MULT ||
		this->get_current_token() == T_DIV ||
		this->get_current_token() == T_MOD || 
		this->get_current_token() == T_ADDRESS) {
		int op = this->get_current_token();
		int line = get_current_line();
		this->eat();
		node_expr* expr2 = this->g_b7();
		return new node_op_multiplication(op,expr,expr2,line);
	}
	return expr;
}

node_expr* Parser::g_b8(){
	this->stack_print("g_b8");
	node_expr* expr;
	int line = get_current_line();
	switch (this->get_current_token()) {
		// @todo insert node_address
		case T_ADD: 
			this->eat();
			return this->g_primary();
		case T_SUB:
			expr = new node_signal(this->g_primary(),line);
			this->eat();
			return expr;
		case T_NEG: 
			expr = new node_negation(this->g_primary(), line);
			this->eat();
			return expr;
	}
	return this->g_primary();
}

static int synch_g_primary[] = {T_ID, T_CONSTANT, T_STRING_LITERAL, T_EOF};
node_expr* Parser::g_primary(){
	this->stack_print("g_primary");
	switch (this->get_current_token()) {
		case T_ID: {
			int line = get_current_line();
			Symbol_id* symbol = new Symbol_id(get_lexeme());
			this->eat();
			if (this->get_current_token() == T_PARENT_L) {
				this->eat();
				node_expr_list* expr_list = this->g_expr_list();
				this->eat_or_skip(T_PARENT_R,synch_g_id_1);

				return new node_call(symbol, expr_list, line);
			}
			return new node_name(symbol, line);
		}
		case T_CONSTANT:
		case T_STRING_LITERAL: {
			node_primitive* primitive = new node_primitive(this->get_current_token(), get_lexeme(), get_current_line());
			this->eat();
			return primitive;
		}
		case T_PARENT_L:{
			this->eat();
			node_expr* expr = this->g_cast_int_or_g_exp(); 
			this->eat_or_skip(T_PARENT_R, synch_g_primary);
			return expr;
		}
	}
	int first[5] = {T_ID, T_STRING_LITERAL, T_CONSTANT, T_PARENT_L, -1};
	this->syntax_error(first);

	return NULL;
}

node_expr* Parser::g_cast_int_or_g_exp(){
	this->stack_print("g_cast_int_or_g_exp");
	switch (this->get_current_token() ) {
		case T_INT:
		case T_CHAR:
		case T_FLOAT:
		case T_VOID:
		//case T_ID:
		// cast is a problem! 
			//return g_cast();
			break;
	}	
	return this->g_expr();
}
