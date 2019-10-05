#ifndef VISITORS_H
#define VISITORS_H

#include "error.hpp"
#include "symbols_tables.h"

class node_assign;
class node_boolean;
class node_call;
class node_expr_list;
class node_if;
class node_literal;
class node_func;
class node_this;
class node_name;
class node_vector;
class node_name_decl;
class node_negation;
class node_new;
class node_number;
class node_op_bitwise;
class node_op_addition;
class node_op_boolean;
class node_op_multiplication;
class node_op_relational;
class node_primitive;
class node_print;
class node_program;
class node_read;
class node_return;
class node_signal;
class node_stmt_list;
class node_type;
class node_var_decl;
class node_var_list;
class node_while;
class node_length;
class node_sizeof;
class node_case;
class node_case_list;
class node_switch;
class node_do;
class node_type;
class node_struct;
class node_enum;
class node_value;
class node_address;
class node_for;


class Visitor{
    public:
        //~Visitor();
        virtual void visit( node_assign* node)=0; 
        //virtual void visit( node_boolean* node)=0; 
        virtual void visit( node_call* node)=0; 
        virtual void visit( node_expr_list* node)=0; 
        virtual void visit( node_if* node)=0; 
        //virtual void visit( node_literal* node)=0; 
        virtual void visit( node_func* node)=0; 
        virtual void visit( node_name* node)=0; 
        virtual void visit( node_vector* node)=0; 
        virtual void visit( node_name_decl* node)=0; 
        virtual void visit( node_negation* node)=0; 
        //virtual void visit( node_number* node)=0; 
        virtual void visit( node_op_bitwise* node)=0; 
        virtual void visit( node_op_addition* node)=0; 
        virtual void visit( node_op_boolean* node)=0; 
        virtual void visit( node_op_multiplication* node)=0; 
        virtual void visit( node_op_relational* node)=0; 
        virtual void visit( node_primitive* node)=0; 
        virtual void visit( node_print* node)=0; 
        virtual void visit( node_program* node)=0; 
        virtual void visit( node_read* node)=0; 
        virtual void visit( node_return* node)=0; 
        virtual void visit( node_signal* node)=0; 
        virtual void visit( node_stmt_list* node)=0; 
        virtual void visit( node_type* node)=0; 
       //virtual void visit( node_var_decl* node)=0; 
        virtual void visit( node_var_list* node)=0; 
        virtual void visit( node_while * node)=0;

        virtual void visit( node_sizeof* node)=0; 
        virtual void visit( node_case* node)=0; 
        virtual void visit( node_case_list* node)=0; 
        virtual void visit( node_switch* node)=0; 
        virtual void visit( node_do* node)=0; 
        virtual void visit( node_struct* node)=0; 
        virtual void visit( node_enum* node)=0; 
        virtual void visit( node_value* node)=0; 
        virtual void visit( node_address* node)=0; 
        virtual void visit(node_for* node)=0;
        virtual void visit(node_length* node)=0;
};


class Visitor_print: public Visitor{
    public:
         Visitor_print();
         ErrorManager error;
         void visit( node_assign* node); 
         //void visit( node_boolean* node); 
         void visit( node_call* node); 
         void visit( node_expr_list* node); 
         void visit( node_if* node); 
         //void visit( node_literal* node); 
         void visit( node_func* node); 
         void visit( node_name* node); 
         void visit( node_vector* node); 
         void visit( node_name_decl* node); 
         void visit( node_negation* node);         
         //void visit( node_number* node); 
         void visit( node_op_bitwise* node); 
         void visit( node_op_addition* node); 
         void visit( node_op_boolean* node); 
         void visit( node_op_multiplication* node); 
         void visit( node_op_relational* node); 
         void visit( node_primitive* node); 
         void visit( node_print* node); 
         void visit( node_program* node); 
         void visit( node_read* node); 
         void visit( node_return* node); 
         void visit( node_signal* node); 
         void visit( node_stmt_list* node); 
         void visit( node_type* node); 
         //void visit( node_var_decl* node); 
         void visit( node_var_list* node); 
         void visit( node_while* node);

         void visit( node_sizeof* node); 
         void visit( node_case* node); 
         void visit( node_case_list* node); 
         void visit( node_switch* node); 
         void visit( node_do* node); 
         void visit( node_struct* node); 
         void visit( node_enum* node); 
         void visit( node_value* node); 
         void visit( node_address* node); 
         void visit(node_for* node);
         void visit(node_length* node);
    private:
        void identation();
        int level;
        void print_symbol(Symbol_id* symbol);
        void print_token(int token);
};


#endif // VISITORS_H
