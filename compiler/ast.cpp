/**
 * @author Claudson Oliveira
 * @date   2012/03/24
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "helper.hpp"

node::~node(){}
node_stmt::~node_stmt(){}
node_expr::~node_expr(){}

node_program::node_program(node_include* node_include_list, node_var_list* var_list, node_func* method_list,int line){
    this->attr_include_list = node_include_list;
    this->attr_var_list    = var_list;
    this->attr_func = method_list;
    this->attr_struct = NULL;
    this->attr_line        = line;
}

node_program::~node_program(){
	
    delete this->attr_var_list;
    delete this->attr_func;
}

node_include::node_include(const char* file){
	this->attr_file = Helper::str_cpy(file);
    this->attr_next = NULL;
}

node_include::~node_include(){
	delete this->attr_file;
}




node_primitive::node_primitive(int token,int line){
    attr_token = token;
    attr_value = NULL;
    attr_line = line;
}

node_primitive::node_primitive(int token, const char* value,int line){
    attr_value = Helper::str_cpy(value);
    attr_token = token;
    attr_line = line;
}

node_primitive::~node_primitive(){
    free(attr_value);
}

node_name::node_name(Symbol_id* id,int line){
    attr_id = id;
    attr_expr = NULL;
    attr_line = line;
    attr_var = NULL;
}

node_name::node_name(Symbol_id* id,node_expr* expr,int line){
    attr_id = id;
    attr_expr = expr;
    attr_line = line;
    attr_var = NULL;
}

node_name:: ~node_name(){
    delete attr_expr;
}


node_vector::node_vector(node_name* name,node_expr_list* expr_list,int line){
    attr_name = name;
    attr_expr_list = expr_list;
    attr_line = line;
}

int node_vector::get_dimension(){
    int count = 0;
    node_expr_list* expr_list = this->attr_expr_list;
    while(expr_list != NULL) {
        expr_list = expr_list->attr_next;
        count++;
    }
    return count;
}

node_vector::~node_vector(){
    delete attr_expr_list;
    delete attr_name;
}


node_expr_list::node_expr_list(node_expr* expr, node_expr_list* expr_list,int line){
    attr_expr = expr;
    attr_next = expr_list;
    attr_line = line;
}

node_expr_list::~node_expr_list(){
    delete attr_expr;
    delete attr_next;
}

node_call::node_call(Symbol_id* id, node_expr_list* expr_list,int line){
    attr_id = id;
    attr_expr_list = expr_list;
    attr_line = line;
    attr_func = NULL;
}

node_call::~node_call(){
    delete attr_id;
    delete attr_expr_list;
}

node_negation::node_negation(node_expr* expr,int line){
    attr_expr = expr;
    attr_line = line;
}

node_negation::~node_negation(){
    delete attr_expr;
}

node_signal::node_signal(node_expr* expr,int line){
    attr_expr = expr;
    attr_line = line;
}


node_signal::~node_signal(){
    delete attr_expr;
}

node_op_addition::node_op_addition(int operat,node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_addition::~node_op_addition(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_op_boolean::node_op_boolean(int operat,node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_boolean::~node_op_boolean(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_op_relational::node_op_relational(int operat,node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_relational::~node_op_relational(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_op_bitwise::node_op_bitwise(int operat, node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_bitwise::~node_op_bitwise(){
    delete attr_expr_1;
    delete attr_expr_2;
}

node_op_multiplication::node_op_multiplication(int operat, node_expr* expr_1,node_expr* expr_2,int line){
    attr_operator = operat;
    attr_expr_1   = expr_1;
    attr_expr_2   = expr_2;
    attr_line = line;
}

node_op_multiplication::~node_op_multiplication(){
    delete attr_expr_1;
    delete attr_expr_2;
}



node_stmt_list::node_stmt_list(node_stmt* stmt,node_stmt_list* stmt_list,int line){
    attr_stmt      = stmt;
    attr_next = stmt_list;
    attr_line = line;
}

node_stmt_list::~node_stmt_list(){
    delete attr_stmt;
    delete attr_next;
}

int node_assign::INIT_ARRAY  = 1;
int node_assign::INIT_OBJECT = 2;
node_assign::node_assign(node_expr* expr_left, node_expr* expr_right,int line){
    attr_expr_left = expr_left;
    attr_expr_right = expr_right;
    attr_line = line;
    type_assing = 0; //0 - primario, 1- atribuição de objetos, 2 - instanciação de objetos
}


node_assign::~node_assign(){
    delete attr_expr_left;
    delete attr_expr_right;
}

node_return::node_return(node_expr* expr,int line){
    attr_expr = expr;
    attr_line = line;
}

node_return::~node_return(){
    delete attr_expr;
}

node_print::node_print(node_expr_list* expr_list,int line){
    attr_expr_list = expr_list;
    attr_line = line;
}

node_print::~node_print(){
    delete attr_expr_list;
}

node_read::node_read(node_expr* format, node_expr* expr,int line){
    attr_format = format;
    attr_expr = expr;
    attr_line = line;
}

node_read::~node_read(){
    delete attr_format;
    delete attr_expr;
}

node_if::node_if(node_expr* expr,node_stmt_list* stmt,node_stmt_list* else_,int line){
    attr_expr = expr;
    attr_stmt = stmt;
    attr_stmt_else = else_;
    attr_line = line;
}

node_if::~node_if(){
    delete attr_expr;
    delete attr_stmt;
    delete attr_stmt_else;
}

node_while::node_while(node_expr* expr,node_stmt* stmt,int line){
    attr_expr = expr;
    attr_stmt = stmt;
    attr_line = line;
}

node_while::~node_while(){
    delete attr_expr;
    delete attr_stmt;
}

node_type::node_type(Symbol_id* id,int line){
    attr_id = id;
    attr_primitive = 0; // não é um tipo primitivo
    attr_array = false; // não é um array
    attr_line = line;
}

node_type::node_type(int primitive, int line){
    attr_primitive = primitive;
    attr_id = NULL; // is not a struct
    attr_array = false; // is not a array
    attr_struct = false;
    attr_line = line;
    attr_pointer_level = 0;
    attr_length = 0; // is not a array
}

node_type::node_type(int primitive,int pointer_level, int array,int line){
    attr_primitive = primitive;
    attr_id = NULL; // is not a struct
    attr_array = array;
    attr_line = line;
    attr_pointer_level = pointer_level;
    attr_length = 0;
    attr_struct = false;
}

node_type::~node_type(){
    delete this->attr_id;
    
}

node_name_decl::node_name_decl(node_type* type, Symbol_id* id, node_assign* assign,int line){
    attr_type   = type;
    attr_id     = id;
    attr_assign = assign; //@TODO se for null criar um objeto assign, testar o tipo e atribuir um default
    attr_line = line;
}

node_name_decl::node_name_decl(node_type* type, Symbol_id* id,int line){
    attr_type   = type;
    attr_id     = id;
    attr_assign = NULL;
    attr_line = line;
}

node_name_decl::~node_name_decl(){
    delete attr_type;
    delete attr_assign;
}

node_var_list::node_var_list(node_name_decl* name,node_var_list* var_list,int line){
    attr_name     = name;
    attr_next = var_list;
    attr_line = line;
}

node_var_list::~node_var_list(){
    delete attr_name;
    delete attr_next;
}

node_length::node_length(node_expr* expr, int line){
    attr_expr = expr; 
    attr_line = line;
    attr_next = NULL;
}

node_length::~node_length()
{
    delete attr_next;
    delete attr_expr;
}

node_func::node_func(node_type* type,Symbol_id* id,node_var_list* param_list,node_stmt_list* stmt_list,node_func* func,int line){
    this->attr_type        = type;
    this->attr_id          = id;
    this->attr_param_list  = param_list;
  //  attr_scope   = scope;
    this->attr_stmt_list = stmt_list;
    this->attr_next = func;
    this->attr_line = line;
}

node_func::~node_func(){
   /* delete attr_type;
    delete attr_param_list;
    delete attr_stmt_list;
    delete attr_next;*/
}

void node_program::insert_var(node_var_list* node){
    if (node == NULL) {
        return;
    }
    node_var_list* p = this->attr_var_list;
    if(p == NULL) this->attr_var_list = node;
    else{
        while(p->attr_next != NULL) p = p->attr_next;
        p->attr_next = node;
    }
}

void node_program::insert_func(node_func* node){
    if (node == NULL) {
        return;
    }
    node_func* p = this->attr_func;
    if(p == NULL) this->attr_func = node;
    else{
        while(p->attr_next != NULL) p = p->attr_next;
        p->attr_next = node;
    }
}

void node_program::insert_struct(node_struct* node){
    if (node == NULL) {
        return;
    }
    node_struct* p = this->attr_struct;
    if(p == NULL) this->attr_struct = node;
    else{
        while(p->attr_next != NULL) p = p->attr_next;
        p->attr_next = node;
    }
}

node_for::node_for(node_stmt* init,node_expr* expr,node_expr_list* steps,node_stmt_list* stmt,int line)
{
    attr_init = init;
    attr_expr = expr; 
    attr_steps = steps;
    attr_line = line;
    attr_stmt = stmt;
}

node_for::~node_for()
{
    delete attr_init;
    delete attr_expr; 
    delete attr_steps;
}

// novos nodos 
/*
node_sizeof::node_sizeof(node_expr* expr, int line){
    attr_expr = expr;
    attr_line = line;
}

node_case::node_case(node_primitive* constant,node_stmt* stmt,int line){
    attr_constant = constant;
    attr_stmt     = stmt;
    attr_line     = line;
}

node_case::~node_case(){
    delete attr_constant;
    delete attr_stmt;
}

node_case_list::node_case_list(node_case* case_,node_case_list* case_list,int line){
    attr_case = case_;
    attr_case_list= case_list;
    attr_line = line;
}

node_case_list::~node_case_list(){
    delete attr_case;
    delete attr_case_list;
}

node_switch::node_switch(node_case_list* case_list,int line){
    attr_case_list = case_list;
    attr_line = line; 
}

node_switch::~node_switch(){
    delete attr_case_list;
}

node_do::node_do(node_expr* expr,node_stmt* stmt,int line){
    attr_expr = expr;
    attr_stmt = stmt;
    attr_line = line;
}

node_do::~node_do(){
    delete attr_expr;
    delete attr_stmt;
}
*/
node_struct::node_struct(Symbol_id* id,node_var_list* fields,bool is_typedef, int line){
    attr_id     = id;
    attr_fields = fields;
    attr_line   = line;
    attr_typedef = is_typedef;
}

node_struct::~node_struct(){
    delete attr_id;
    delete attr_fields;
}
/*
node_enum::node_enum(Symbol_id* id, node_var_list* fields,int line){
    attr_id     = id; 
    attr_fields = fields;
    attr_line   = line;
}

node_enum::~node_enum(){
    delete attr_id;
    delete attr_fields;
}


node_value::node_value(node_name* name, int line){
    attr_name = name;
    attr_line = line;
}

node_address::node_address(node_name* name, int line){
    attr_name = name;
    attr_line = line;
}
*/
// accepts


void node_assign::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_primitive::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}


void node_call::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_expr_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_if::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_func::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_name::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_vector::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_name_decl::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_negation::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_addition::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_bitwise::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_boolean::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_relational::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_op_multiplication::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_print::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_program::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_read::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_return::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_signal::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_stmt_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_type::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}


void node_var_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_while::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_sizeof::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_case::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_case_list::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_switch::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_do::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_struct::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_enum::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_value::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_address::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_for::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}

void node_length::accept(Visitor* visitor){
    if(visitor != NULL ) visitor->visit(this);
}
