/**
* visitors.cpp
* @author Claudson Oliveira
* @date   26 de março de 2012
*
* Arquivo que implementa as void Visitor_print::visit(es do padrão visitor
*
**/
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>
#include <string.h>
#include "visitor.h" 
#include "symbols_tables.h"  
#include "ast.h"
#include "parser.hpp"
#include "error.hpp"


Visitor_print::Visitor_print(){
    fprintf(stdout, "\n=================== AST ===================\n" );
    this->level = 0; // nivel raiz da árvore
    this->error = ErrorManager::getInstance();
}

void Visitor_print::print_symbol(Symbol_id* symbol)
{
    fprintf(stdout,"->Symbol('%s')",symbol->lexeme);
}

void Visitor_print::print_token(int token)
{
    fprintf(stdout, "->%s\n", this->error.get_token_name(token)); 
}

void Visitor_print::identation(){
    for(int i=1;i < level;i++) fprintf(stdout,"    ");
}

void Visitor_print::visit(node_assign* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_ASSIGN\n");
        if(node->attr_expr_left  != NULL) node->attr_expr_left->accept(this);
        if(node->attr_expr_right != NULL) node->attr_expr_right->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_primitive* node){
    if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_PRIMITIVE\n");
        level++;
        this->identation();
        this->print_token(node->attr_token);
       // if(node->attr_token != T_TRUE && node->attr_token != T_FALSE){
            this->identation();
            fprintf(stdout,"->%s\n",node->attr_value);
       // }
        level-=2;
    }
}

void Visitor_print::visit(node_call* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_CALL\n");
        level++;
        this->identation();
        level--;
        if(node->attr_id != NULL){
            this->print_symbol(node->attr_id);
            fprintf(stdout,"\n");
        }
        if(node->attr_expr_list != NULL) node->attr_expr_list->accept(this);
        if(node->attr_expr != NULL) node->attr_expr->accept(this);
        level--;
    }
}


void Visitor_print::visit(node_expr_list* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_EXPR_LIST\n");
        if(node->attr_expr != NULL) node->attr_expr->accept(this);
        if(node->attr_next  != NULL) node->attr_next->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_if* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_IF\n");
        if(node->attr_expr      != NULL) node->attr_expr->accept(this);
        if(node->attr_stmt      != NULL) node->attr_stmt->accept(this);
        if(node->attr_stmt_else != NULL) node->attr_stmt_else->accept(this);
        level--;
    }
}


void Visitor_print::visit(node_func* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_FUNC\n");
        if(node->attr_type  != NULL) node->attr_type->accept(this);
        level++;
        if(node->attr_id != NULL){
           // Symbols_table_id* table = GET_symbol_table_id();
            this->identation();
            this->print_symbol(node->attr_id);
            fprintf(stdout,"\n");
        }
        level--;
        if(node->attr_param_list  != NULL) node->attr_param_list->accept(this);
        if(node->attr_stmt_list       != NULL) node->attr_stmt_list->accept(this);
        if(node->attr_next != NULL) node->attr_next->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_name* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_NAME\n");
        if(node->attr_id != NULL){
            level++;
            this->identation();
           // Symbols_table_id* table = GET_symbol_table_id();
            this->print_symbol(node->attr_id);
            fprintf(stdout,"\n");
            level--;
        }
        if(node->attr_expr  != NULL) node->attr_expr->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_vector* node){
    if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_VECTOR\n");
        if(node->attr_name  != NULL) node->attr_name->accept(this);
        if(node->attr_expr_list  != NULL) node->attr_expr_list->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_name_decl* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_NAME_DECL\n");
        if(node->attr_type != NULL) node->attr_type->accept(this);
        if(node->attr_id   != NULL){
            level++;
            this->identation();
            this->print_symbol(node->attr_id);
            fprintf(stdout,"\n");
            level--;
        }
        if(node->attr_assign != NULL) node->attr_assign->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_negation* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_NEGATION\n");
        if(node->attr_expr != NULL) node->attr_expr->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_op_bitwise* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_OP_BITWISE\n");
        level++;
        this->identation();
        this->print_token(node->attr_operator);
        level--;
        if(node->attr_expr_1  != NULL)  node->attr_expr_1->accept(this);
        if(node->attr_expr_2  != NULL) node->attr_expr_2->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_op_addition* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_OP_ADDITION\n");
        level++;
        this->identation();
        this->print_token(node->attr_operator);
        level--;
        if(node->attr_expr_1  != NULL) node->attr_expr_1->accept(this);
        if(node->attr_expr_2 != NULL) node->attr_expr_2->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_op_boolean* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_OP_BOOLEAN\n");
        level++;
        this->identation();
        fprintf(stdout,"->%s\n",this->error.get_token_name(node->attr_operator));
        level--;
        if(node->attr_expr_1  != NULL) node->attr_expr_1->accept(this);
        if(node->attr_expr_2  != NULL) node->attr_expr_2->accept(this);
        level--;

    }
}


void Visitor_print::visit(node_op_multiplication* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_OP_MULTIPLICATION\n");
        level++;
        this->identation();
        this->print_token(node->attr_operator);
        level--;
        if(node->attr_expr_1  != NULL) node->attr_expr_1->accept(this);
        if(node->attr_expr_2  != NULL) node->attr_expr_2->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_op_relational* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_OP_RELATIONAL\n");
        level++;
        this->identation();
        this->print_token(node->attr_operator);
        level--;
        if(node->attr_expr_1  != NULL) node->attr_expr_1->accept(this);
        if(node->attr_expr_2  != NULL) node->attr_expr_2->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_print* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_PRINT\n");
        if(node->attr_expr_list != NULL) node->attr_expr_list->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_program* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_PROGRAM\n");
        if(node->attr_var_list != NULL) node->attr_var_list->accept(this);
        if(node->attr_struct != NULL) node->attr_struct->accept(this);
        if(node->attr_func != NULL) node->attr_func->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_read* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_READ\n");
        if(node->attr_expr != NULL) node->attr_expr->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_return* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_RETURN\n");
        if(node->attr_expr  != NULL) node->attr_expr->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_signal* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_SIGNAL\n");
        if(node->attr_expr  != NULL) node->attr_expr->accept(this);
        level--;
    }
}



void Visitor_print::visit(node_stmt_list* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_STMT_LIST\n");
        if(node->attr_stmt  != NULL) node->attr_stmt->accept(this);
        if(node->attr_next  != NULL) node->attr_next->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_type* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_TYPE\n");
        level++;
        this->identation();
        if (node->attr_id == NULL) {
           fprintf(stdout,"->%s",this->error.get_token_name(node->attr_primitive));
            if (node->attr_pointer_level) {
                int i = 1; 
                while(i <= node->attr_pointer_level){
                    fprintf(stdout,"*");
                    i++;
                }
            }
            fprintf(stdout, "\n");
            level--;
            if(node->attr_length) node->attr_length->accept(this); 
            level++;
        } else {
            level--;
            fprintf(stdout, "->is_struct\n");
            level++;
            this->identation();
            this->print_symbol(node->attr_id);
            if (node->attr_pointer_level) {
                int i = 1;
                while(i <= node->attr_pointer_level){
                    fprintf(stdout,"*");
                    i++;
                }
            }
            fprintf(stdout,"\n");
        }
        level-= 2;
    }
}

void Visitor_print::visit(node_length* node){
    if(node != NULL ){
        //this->identation();
        //fprintf(stdout, "->level1");
        //level++;
        if(node->attr_expr != NULL) node->attr_expr->accept(this);
        //level--;
        if(node->attr_next  != NULL) node->attr_next->accept(this);
    }
}


void Visitor_print::visit(node_var_list* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_VAR_LIST\n");
        if(node->attr_name  != NULL) node->attr_name->accept(this);
        if(node->attr_next  != NULL) node->attr_next->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_while* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_WHILE\n");
        if(node->attr_expr  != NULL) node->attr_expr->accept(this);
        if(node->attr_stmt  != NULL) node->attr_stmt->accept(this);
        level--;
    }
}

void Visitor_print::visit(node_for* node){
    if(node != NULL ){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_FOR\n");
        if(node->attr_init  != NULL) node->attr_init->accept(this);
        if(node->attr_expr  != NULL) node->attr_expr->accept(this);
        if(node->attr_steps  != NULL) node->attr_steps->accept(this);
        if(node->attr_stmt  != NULL) node->attr_stmt->accept(this);
        level--;
    }
}

void Visitor_print::visit( node_sizeof* node){
    if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_PRINT");
        if(node->attr_expr != NULL) node->attr_expr->accept(this);
        level--;
    }
}

void Visitor_print::visit( node_case* node){
    if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_CASE");
        if(node->attr_constant != NULL) node->attr_constant->accept(this);
        if(node->attr_stmt != NULL) node->attr_stmt->accept(this);
        level--;
    }
}


void Visitor_print::visit( node_case_list* node){
    if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_CASE_LIST");
        if(node->attr_case != NULL) node->attr_case->accept(this);
        if(node->attr_case_list != NULL) node->attr_case_list->accept(this);
        level--;
    }
}

void Visitor_print::visit( node_switch* node){
    if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_SWITCH");
        if(node->attr_case_list != NULL) node->attr_case_list->accept(this);
        level--;
    }
}
void Visitor_print::visit( node_do* node){
    if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_DO");
        if(node->attr_expr != NULL) node->attr_expr->accept(this);
        if(node->attr_stmt != NULL) node->attr_stmt->accept(this);
        level--;
    }    
}
void Visitor_print::visit( node_struct* node){
    if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_STRUCT\n");
        if (node->attr_typedef) {
            level++;
            this->identation();
            fprintf(stdout, "->is_typedef\n");
            level--;
        }
        if(node->attr_id != NULL){
            level++;
            this->identation();
            this->print_symbol(node->attr_id);  
            fprintf(stdout, "\n");
            level--;
        } 
        if(node->attr_fields != NULL) node->attr_fields->accept(this);
        if(node->attr_next != NULL) node->attr_next->accept(this);
        level--;
    }
}

void Visitor_print::visit( node_enum* node){
     if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_ENUM");
        if(node->attr_fields != NULL) node->attr_fields->accept(this);
        level--;
    }
}

void Visitor_print::visit( node_value* node){
     if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_VALUE");
        if(node->attr_name != NULL) node->attr_name->accept(this);
        level--;
    }
}

void Visitor_print::visit( node_address* node){
     if(node != NULL){
        level++;
        this->identation();
        fprintf(stdout,"->NODE_ADDRESS");
        if(node->attr_name != NULL) node->attr_name->accept(this);
        level--;
    }
}
