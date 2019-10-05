#ifndef CLOUD_PARSER
#define CLOUD_PARSER 1

#define CLOUD_DEBUG 0

#include "ast.h"
#include "error.hpp"

class Parser
{
	private:
		int current_token;
		// symbol current_symbol;
		void syntax_error(int token);
		void syntax_error(int* tokens); // proxy to ErrorManager::throwSyntax()
		node_program* ast;
		node_program* g_program();
		void stack_print(const char* method);
		node_include* g_include();
		node_include* g_include_int(); 
		void g_include_root(node_include* include);
		void g_main(node_program* program); 
		void g_decl(node_program* program); 
		node_struct* g_struct();
		node_var_list* g_struct_fields();
		node_type* g_type();
		int g_pointer(int level = 0);
		void g_func_or_var(node_program* program, node_type* type, Symbol_id* symbol);
		node_stmt_list* g_scope();
		node_var_list* g_rvar(node_type* type, Symbol_id* symbol); 
		node_stmt_list* g_else();
		node_stmt_list* g_stmt_list();
		node_stmt* g_stmt();
		void g_printf_int();
		node_stmt* g_for_init();
		node_type* g_for_int_type();
		node_expr* g_for_int_assign();
		node_expr_list* g_for_int_inc();
		void g_for_int_inc_int();
		void g_cases();
		void g_case_value();
		node_var_list* g_params();
		node_var_list* g_params_int();
		int g_array(int level = 0);
		node_stmt* g_id(Symbol_id* symbol);
		node_stmt* g_id_or_semicolon(Symbol_id* symbol);
		node_vector* g_array_access_optional();
		node_vector* g_array_access();
		node_expr_list* g_expr_list();
		node_expr* g_expr();
		node_expr* g_cast();
		node_expr* g_cast_int();
		node_expr* g_expr_();
		node_expr* g_b2();
		node_expr* g_b3();
		node_expr* g_b4();
		node_expr* g_b5();
		node_expr* g_b6();
		node_expr* g_b7();
		node_expr* g_b8();
		node_expr* g_primary();
		node_expr* g_cast_int_or_g_exp();
		node_stmt* g_decl_id(node_type* type);
	public:
		Parser();
		~Parser();
		int get_next_token();
		int get_current_token();
		const char* get_lexeme();
		node_program* get_AST();
		ErrorManager error;
		void eat_or_skip(int expected, int* synch);
		void eat();
		void skip(int* synch);
};

#endif 
