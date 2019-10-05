#ifndef CLOUD_CONVERTER
#define CLOUD_CONVERTER 1

#include "ast.h"
#include "visitor.h"

class Converter : public Visitor
{
	public: 
		int level;
		void addJs(char* source);
		void identation();
		const char* initialize(Symbol_var* var);
		const char* get_args(Symbol_func* func);

		Converter();
		~Converter();
		void convert(node_program* program);
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