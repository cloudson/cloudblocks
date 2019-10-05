#ifndef CLOUD_AST_H
#define CLOUD_AST_H 1

#include "symbols_tables.h"
#include "visitor.h"


class node{
    public:
        virtual ~node();    
};

class node_stmt: public node{
    public:
         virtual  ~node_stmt();
         virtual  void accept(Visitor* visitor)=0;
         int attr_line;
};

class node_expr: public node {
    public:
        virtual ~node_expr();
        virtual void accept(Visitor* visitor)=0;
        int attr_line;
};

class node_primitive: public node_expr{
    public:
        node_primitive(int token,int line);
        node_primitive(int token, const char* value,int line);
        ~node_primitive();
        char* attr_value;
        int attr_token;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_expr_list{
    public:
        node_expr_list(node_expr* expr, node_expr_list* expr_list,int line);
        ~node_expr_list();
        node_expr* attr_expr;
        node_expr_list* attr_next;
        void accept(Visitor* visitor);
        int attr_line;
};

class node_negation: public node_expr{
    public:
        ~node_negation();
        node_negation(node_expr* expr,int line);
        node_expr* attr_expr;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_signal: public node_expr{
    public:
        ~node_signal();
        node_signal(node_expr* expr,int line);
        node_expr* attr_expr ;
        int attr_line;
        void accept(Visitor* visitor);
};


class node_op_addition: public node_expr{
    public:
        ~node_op_addition();
        node_op_addition(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        int attr_line;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_op_multiplication: public node_expr{
    public:
        ~node_op_multiplication();
        node_op_multiplication(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        int attr_line;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_op_relational: public node_expr{
    public:
        ~node_op_relational();
        node_op_relational(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        int attr_line;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_op_boolean: public node_expr{
    public:
        ~node_op_boolean();
        node_op_boolean(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        int attr_line;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_op_bitwise: public node_expr{
    public:
        ~node_op_bitwise();
        node_op_bitwise(int operat, node_expr* expr_1,node_expr* expr_2,int line);
        int attr_operator;
        int attr_line;
        node_expr* attr_expr_1 ;
        node_expr* attr_expr_2 ;
        void accept(Visitor* visitor);
};

class node_name: public node_expr, public node_stmt{
    public:
        ~node_name();
        node_name(Symbol_id* id,int line); // para variavel simples
        node_name(Symbol_id* id, node_expr* expr,int line); // atributo
        Symbol_id* attr_id; // entrada pra tabela de ID
        Symbol_var* attr_var;
        node_expr* attr_expr;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_vector: public node_expr {
    public:
        node_vector(node_name* name,node_expr_list* expr,int line);
        ~node_vector();
        int get_dimension();
        node_name* attr_name;
        node_expr_list* attr_expr_list;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_call: public node_expr, public node_stmt{
    public:
        ~node_call();
        node_call(Symbol_id* id, node_expr_list* expr_list,int line);
        Symbol_id* attr_id;
        Symbol_func* attr_func;
        node_expr* attr_expr;
        node_expr_list* attr_expr_list;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_stmt_list: public node_stmt{
    public:
        ~node_stmt_list();
        node_stmt_list(node_stmt* stmt,node_stmt_list* stmt_list,int line);
        node_stmt* attr_stmt;
        node_stmt_list* attr_next;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_assign: public node_stmt, public node_expr{
    public:
        ~node_assign();
        node_assign(node_expr* expr_left, node_expr* expr_right,int line);
        node_expr* attr_expr_left;
        node_expr* attr_expr_right;
        int type_assing;
        int attr_line;
        static int INIT_ARRAY;
        static int INIT_OBJECT;
        void accept(Visitor* visitor);
};

class node_return: public node_stmt{
    public:
        ~node_return();
        node_return(node_expr* expr,int line);
        node_expr* attr_expr;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_print: public node_stmt{
    public:
        ~node_print();
        node_print(node_expr_list* expr_list,int line);
        node_expr_list* attr_expr_list;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_read: public node_stmt{
    public:
        ~node_read();
        node_read(node_expr* format, node_expr* expr, int line);
        node_expr* attr_format;
        node_expr* attr_expr;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_sizeof: public node_expr{
    public:
        node_sizeof(node_expr* expr, int line);
        node_expr* attr_expr;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_if: public node_stmt{
    public:
        ~node_if();
        node_if(node_expr* expr,node_stmt_list* stmt,node_stmt_list* stmt_else,int line);
        node_expr* attr_expr;
        node_stmt_list* attr_stmt;
        node_stmt_list* attr_stmt_else;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_case: public node_stmt{
    public:
        ~node_case();
        node_case(node_primitive* constant,node_stmt* stmt,int line);
        node_primitive* attr_constant;
        node_stmt*      attr_stmt;   
        int attr_line;         
        void accept(Visitor* visitor);
};

class node_case_list: public node_stmt{
    public:
        ~node_case_list();
        node_case_list(node_case* case_,node_case_list* case_list,int line);
        node_case_list* attr_case_list;
        node_case* attr_case;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_switch: public node_stmt{
    public:
        ~node_switch();
        node_switch(node_case_list* case_list,int line);
        node_case_list* attr_case_list;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_do: public node_stmt{
    public:
        node_do(node_expr* expr,node_stmt* stmt,int line);
        ~node_do();
        node_expr* attr_expr;
        node_stmt* attr_stmt;
        void accept(Visitor* visitor);
};

class node_while: public node_stmt{
    public:
        node_while(node_expr* expr,node_stmt* stmt,int line);
        ~node_while();
        node_expr* attr_expr;
        node_stmt* attr_stmt;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_for: public node_stmt{
    public:
        node_for(node_stmt* init,node_expr* expr,node_expr_list* steps,node_stmt_list* stmt,int line);
        ~node_for();
        node_stmt* attr_init;
        node_expr* attr_expr;
        node_expr_list* attr_steps;
        node_stmt_list* attr_stmt;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_type{
    public:
        ~node_type();
        node_type(Symbol_id* id,int line);
        node_type(int primitive,int line);
        node_type(int primitive,int pointer_level, int array,int line);
        Symbol_id* attr_id; 
        int attr_primitive;
        bool attr_array;
        bool attr_struct;
        int attr_line;
        int attr_pointer_level;
        node_length* attr_length;
        void accept(Visitor* visitor);
};

class node_length: node_expr {
    public:
        ~node_length();
        node_length(node_expr* expr, int line);
        node_expr* attr_expr;
        node_length* attr_next;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_name_decl: public node_stmt{
    public:
        ~node_name_decl();
        node_name_decl(node_type* type,Symbol_id* id,node_assign* assign, int line);
        node_name_decl(node_type* type,Symbol_id* id,int line);
        node_type* attr_type;
        Symbol_id* attr_id;
        Symbol_var* attr_var;
        // @TODO atributo para representar se é ponteiro e outro pra dizer o "grau" disso (ponteiro de ponteiro de ponteiro)
        node_assign* attr_assign;
        bool attr_struct;
        int attr_line;
        void accept(Visitor* visitor);
};

class node_var_list: public node_stmt{
    public:
        ~node_var_list();
        node_var_list(node_name_decl* nome, node_var_list* var_list, int line);
        node_name_decl* attr_name;
        node_var_list* attr_next;
        int attr_line;
        void accept(Visitor* visitor);

};

class node_struct {
    public:
        ~node_struct();
        node_struct(Symbol_id* id,node_var_list* fields,bool is_typedef, int line);
        Symbol_id* attr_id;
        Symbol_struct* attr_str;
        node_var_list* attr_fields ;
        bool attr_typedef; 
        int attr_line;
        node_struct* attr_next;
        void accept(Visitor* visitor);
};

class node_enum {
    public:
        ~node_enum();
        node_enum(Symbol_id* id,node_var_list* fields, int line);
        Symbol_id* attr_id;
        node_var_list* attr_fields ;
        int attr_line;
        void accept(Visitor* visitor);
};



class node_func {
    public:
        ~node_func();
        node_func(node_type* type,Symbol_id* id,node_var_list* param_list, node_stmt_list* stmt_list,node_func* func,int line);
        node_type* attr_type;
        Symbol_id* attr_id;
        Symbol_func* attr_func;
        node_var_list* attr_param_list;
        node_stmt_list* attr_stmt_list;
        node_func* attr_next;
        int attr_line;
        int attr_access;
        void accept(Visitor* visitor);
};

// Exemplo (*$var) <=> acessa valor guardado no frame $var, usa-o como ponteiro e retorna o valor deste endereco
class node_value{
    public:
        ~node_value();
        node_value(node_name* name, int line);
        node_name* attr_name;
        int attr_line;
        void accept(Visitor* visitor);
};

// exemplo (&$var) <=> retorna o endereço no frame onde está guardo o valor de $var
class node_address{
    public:
        ~node_address();
        node_address(node_name* name, int line);
        node_name* attr_name;
        int attr_line;
        void accept(Visitor* visitor);
};

// @todo semantic action: search if current node include is in list of inclues possibles 
class node_include{
	public:
		~node_include();
		node_include(const char* file);
		char* attr_file;
		node_include* attr_next;
};


class node_program{
    public:
        ~node_program();
        node_program(node_include* node_include_list, node_var_list* var_list, node_func* method_list,int line);
        void insert_func(node_func* node);
        void insert_var(node_var_list* node);
        void insert_struct(node_struct* node);
        node_var_list* attr_var_list;
        node_func* attr_func;
        node_include* attr_include_list;
        node_struct* attr_struct;
        int attr_line;
        void accept(Visitor* visitor);
};

#endif // 
