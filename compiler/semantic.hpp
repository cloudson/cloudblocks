#ifndef CLOUD_SEMANTIC
#define CLOUD_SEMANTIC 1

#include "visitor.h"
#include "ast.h"
#include "error.hpp"
#include <string>
#include <map>
#include <iostream>


class Visitor;
class Scope;

class Scope {
	public:
		Scope();
		~Scope();
		std::map<std::string, Symbol_var*> table;
		Scope* prev;
            Scope* next;
};

class SemanticAnalyser: public Visitor{
    private:
      node_func* current_func;
      node_stmt* last_stmt;
      Symbol_struct* current_str;
      Symbol_struct* last_type;

    public:
      SemanticAnalyser(ErrorManager* error);
      void analyse(node_program* ast);
      Symbol_var* declaring(node_name_decl* name); 
      Symbol_func* declaring(node_func* func); 
      Symbol_struct* declaring(node_struct* str); 
      Symbol_var* var_exists(const char* lexeme);
      Symbol_var* get_var(const char* lexeme);
      Symbol_func* get_func(const char* lexeme);
      Symbol_struct* get_struct(const char* lexeme);
      void begin();
      void end();

      std::map<std::string, Symbol_func*> table_func;
      std::map<std::string, Symbol_struct*> table_struct;
      Scope* current_scope;
      Scope* scopes;
      ErrorManager* error;
      void visit(node_assign* node); 
      void visit(node_call* node); 
      void visit(node_expr_list* node); 
      void visit(node_if* node); 
      void visit(node_func* node); 
      void visit(node_name* node); 
      void visit(node_vector* node); 
      void visit(node_name_decl* node); 
      void visit(node_negation* node);         
      void visit(node_op_bitwise* node); 
      void visit(node_op_addition* node); 
      void visit(node_op_boolean* node); 
      void visit(node_op_multiplication* node); 
      void visit(node_op_relational* node); 
      void visit(node_primitive* node); 
      void visit(node_print* node); 
      void visit(node_read* node); 
      void visit(node_return* node); 
      void visit(node_signal* node); 
      void visit(node_stmt_list* node); 
      void visit(node_type* node); 
      void visit(node_var_list* node); 
      void visit(node_while* node);
      void visit(node_sizeof* node); 
      void visit(node_case* node); 
      void visit(node_case_list* node); 
      void visit(node_switch* node); 
      void visit(node_do* node); 
      void visit(node_struct* node); 
      void visit(node_enum* node); 
      void visit(node_value* node); 
      void visit(node_address* node); 
      void visit(node_for* node);
      void visit(node_length* node);
      void visit( node_program* node); 
};


#endif