#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokens.h"
#include "helper.hpp"
#include "converter.hpp"



Converter::Converter(){
	this->level = 1;
}

Converter::~Converter(){

}

void Converter::convert(node_program* program){
	program->accept(this);
}

void Converter::addJs(char* source) {
	fprintf(stdout, "%s", source);
}

void Converter::identation(){
	char* space = new char[20];
	strcpy(space, "   ");
    for(int i=1;i < level;i++) this->addJs(space);
    delete space;
}

const char* Converter::initialize(Symbol_var* var){
	if (var->type_struct != NULL) {
		char* str_new = new char[50];
		sprintf(str_new,"new str_%s()",var->type_struct->lexeme); 
		return str_new;
	} else {
		switch(var->type_primitive){
			case T_INT: 
				return "0";
			case T_FLOAT: 
				return "0.0";
			case T_CHAR:
				return "\"\"";
		}
	}
	return "null";
}

const char* Converter::get_args(Symbol_func* func) {
	char* str_args = new char[500];
	strcpy(str_args, "");
	Symbol_var* var;
	for (int i = 0; i < func->num_params; i++) {
		var = func->params[i];
		sprintf(str_args, "%s%s%s",str_args, var->lexeme, (i < func->num_params - 1) ? ", " : "");
	}
	return str_args;
}

void Converter::visit(node_assign* node){
	if (node->attr_expr_left)  node->attr_expr_left->accept(this); 
	char* instr = new char[6];
	strcpy(instr," = ");
	this->addJs(instr);
	if (node->attr_expr_right) node->attr_expr_right->accept(this); 
	strcpy(instr, ";\n");
	this->addJs(instr);
	delete instr;
} 
void Converter::visit(node_call* node){} 
void Converter::visit(node_expr_list* node){
	if (node->attr_expr) node->attr_expr->accept(this); 
    if (node->attr_next) node->attr_next->accept(this);
} 
void Converter::visit(node_if* node){
} 
void Converter::visit(node_func* node){
	char* instr = new char[200];
	Symbol_func* func = node->attr_func;
	bool is_main = Helper::str_eq(func->lexeme, "main");

	if (!is_main) {
		sprintf(instr, "function %s(%s){\n", func->lexeme, this->get_args(func));
		this->addJs(instr);
		level++;
	}
	if (node->attr_stmt_list) node->attr_stmt_list->accept(this);
	if (!is_main) {
		level--;
		sprintf(instr, "}\n\n");
		this->addJs(instr);
	}

	if (node->attr_next) node->attr_next->accept(this);
} 
void Converter::visit(node_name* node){
	Symbol_var* var = node->attr_var;
	char* instr = new char[200];
	sprintf(instr, "%s", var->lexeme);
	this->addJs(instr);
	delete instr;
} 
void Converter::visit(node_vector* node){} 
void Converter::visit(node_name_decl* node){
	
	Symbol_var* var = node->attr_var;
	char* instr = new char[200];
	this->identation();
	sprintf(instr, "var %s", var->lexeme);
	this->addJs(instr);
	if (var->type_struct != NULL) {
		sprintf(instr," = %s", this->initialize(var));
		this->addJs(instr);
	}
	sprintf(instr,";\n");
	this->addJs(instr);
	if (node->attr_assign) node->attr_assign->accept(this); 

	delete instr;
} 
void Converter::visit(node_negation* node){
      if (node->attr_expr) node->attr_expr->accept(this);
}         
void Converter::visit(node_op_bitwise* node){
      if (node->attr_expr_1) node->attr_expr_1->accept(this);
      if (node->attr_expr_2) node->attr_expr_2->accept(this);
} 
void Converter::visit(node_op_addition* node){

      if (node->attr_expr_1) node->attr_expr_1->accept(this);
      if (node->attr_expr_2) node->attr_expr_2->accept(this); 

} 
void Converter::visit(node_op_boolean* node){} 
void Converter::visit(node_op_multiplication* node){
      if (node->attr_expr_1) node->attr_expr_1->accept(this);
      if (node->attr_expr_2) node->attr_expr_2->accept(this);
} 
void Converter::visit(node_op_relational* node){
      if (node->attr_expr_1) node->attr_expr_1->accept(this);
      if (node->attr_expr_2) node->attr_expr_2->accept(this);
}  
void Converter::visit(node_primitive* node){
	char* value = node->attr_value;
	this->addJs(value);
} 
void Converter::visit(node_print* node){
	node_expr_list* expr_list = node->attr_expr_list;
	node_primitive* format = dynamic_cast<node_primitive*>(expr_list->attr_expr); 
	const char* value = format->attr_value;
	char* instr = new char[200];
	if (expr_list->attr_next == NULL) {
		sprintf(instr, "jqconsole.Write(%s, 'jqconsole-output');",value);
		this->addJs(instr);
	} else {
		char* formated = new char[500];
		sprintf(formated,"sprintf(%s",value);
		expr_list = expr_list->attr_next;
		int i=0;
		while(expr_list != NULL) {
			sprintf(instr, "local_%d  = ", i);
			this->addJs(instr);
			expr_list->accept(this);
			sprintf(instr, ";");
			this->addJs(instr);
			sprintf(formated, "%s,local_%d", formated, i++);
			expr_list = expr_list->attr_next;
		}
		sprintf(formated, "%s)", formated);
		sprintf(instr, "jqconsole.Write(%s, 'jqconsole-output')", formated);
		this->addJs(instr);
	}
	
} 
void Converter::visit(node_read* node){} 
void Converter::visit(node_return* node){} 
void Converter::visit(node_signal* node){} 
void Converter::visit(node_stmt_list* node){
	if (node->attr_stmt) node->attr_stmt->accept(this);
    if (node->attr_next) node->attr_next->accept(this);
} 
void Converter::visit(node_type* node){} 
void Converter::visit(node_var_list* node){
	if (node->attr_name) node->attr_name->accept(this);
    if (node->attr_next) node->attr_next->accept(this);
} 
void Converter::visit(node_while* node){}
void Converter::visit(node_sizeof* node){} 
void Converter::visit(node_case* node){} 
void Converter::visit(node_case_list* node){} 
void Converter::visit(node_switch* node){} 
void Converter::visit(node_do* node){} 
void Converter::visit(node_struct* node){
	char instr[200];
	Symbol_struct* str = node->attr_str;
	const char* name_str = Helper::str_cat("str_", str->lexeme);
	
	sprintf(instr, "function %s{\n", name_str);
	level++;
	this->addJs(instr);

	node_var_list* var_list = node->attr_fields;
	Symbol_var* var = NULL;
	while (var_list != NULL) {
		var = var_list->attr_name->attr_var;
		this->identation();
		sprintf(instr, "this.%s = %s;\n", var->lexeme, this->initialize(var));
		this->addJs(instr);

		var_list = var_list->attr_next;
	}
	level--;
	sprintf(instr, "}\n\n");
	this->addJs(instr);
} 
void Converter::visit(node_enum* node){} 
void Converter::visit(node_value* node){} 
void Converter::visit(node_address* node){} 
void Converter::visit(node_for* node){}
void Converter::visit(node_length* node){}
void Converter::visit(node_program* node){
	if(node->attr_struct) node->attr_struct->accept(this);
    if(node->attr_var_list) node->attr_var_list->accept(this);
    if(node->attr_func) node->attr_func->accept(this);
} 