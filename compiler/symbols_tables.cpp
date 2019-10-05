/**
* symbols_tables.cpp
* @author Claudson Oliveira
* @date   10 de abril de 2011
*
* Arquivo que implementa tudo do módulo de tabela de símbolos.
*
**/

#include "symbols_tables.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <typeinfo>
#include "helper.hpp"
#include "ast.h"
#include "error.hpp"
#include "semantic.hpp"

/*
* Classe Simbolo
*/
Symbol::Symbol(){
    this->position = 0;
}

Symbol::Symbol(char* lexeme){
    this->lexeme = (char*)malloc(sizeof(char)*strlen(lexeme));
    strcpy(this->lexeme,lexeme);
}

Symbol::~Symbol(){}

Symbol_id::Symbol_id(const char* lexeme){
    this->lexeme = Helper::str_cpy(lexeme);
}

const char* Symbol_id::get_lexeme() {
    return this->lexeme;
}

Symbol_var::Symbol_var(const char* lexeme){
    this->lexeme = Helper::str_cpy(lexeme);
    this->type_primitive = 0;
    this->type_struct = NULL;
    this->is_vector = false;
    this->dimension = 0;
    this->length = NULL;
}



Symbol_struct::Symbol_struct(const char* lexeme){
    this->lexeme = Helper::str_cpy(lexeme);
    array_fields = NULL;
    num_fields = 0;
}

Symbol_var* Symbol_struct::get_attr(const char* lexeme) {
    for (int i = 0; i < this->num_fields; i++){
        Symbol_var* var = this->array_fields[i];
        if (Helper::str_eq(var->lexeme, lexeme)) {
            return var;
        }
    }
    return NULL;
}

Symbol_func::Symbol_func(const char* lexeme) {
    this->lexeme = Helper::str_cpy(lexeme);
}

// magic numbers is sucks
int Symbol_func::is_correct_call(node_call* call, SemanticAnalyser* semantic) {
    node_expr_list* expr_list = call->attr_expr_list;
    Symbol_var** param = this->params;
    int i = 0;


    for (i; i < this->num_params; i++, expr_list = expr_list->attr_next) {

        if (expr_list == NULL) {
            return i + 1;
        }


        if (param[i]->type_struct != NULL ) {
            // expected struct, found struct ?  
            if (expr_list->attr_expr && typeid(*(expr_list->attr_expr)) == typeid(node_name)) {
                const char* name_lexeme = ((node_name*)expr_list->attr_expr)->attr_id->get_lexeme();
                Symbol_var* var = semantic->get_var(name_lexeme);
                if (var != NULL && var->type_struct != param[i]->type_struct) {
                    return i + 1;
                } else {
                    continue;
                }
            
            }

            if (expr_list->attr_expr && typeid(*(expr_list->attr_expr)) == typeid(node_call)) {
                node_call* call = ((node_call*)expr_list->attr_expr);
                const char* call_lexeme = call->attr_id->get_lexeme();
                Symbol_func* func = semantic->get_func(call_lexeme);
                if (func != NULL && func->return_struct != param[i]->type_struct) {
                    return i + 1;
                } else {
                    continue; 
                }
            }

            return i + 1;
        } else {
            // expected expression, 
            if (expr_list->attr_expr && typeid(*(expr_list->attr_expr)) == typeid(node_name)) {
                const char* name_lexeme = ((node_name*)expr_list->attr_expr)->attr_id->get_lexeme();
                Symbol_var* var = semantic->get_var(name_lexeme);
                if (var != NULL && var->type_struct != NULL) {
                    return i + 1;
                }else {
                    continue;
                }
            } 

            if (expr_list->attr_expr && typeid(*(expr_list->attr_expr)) == typeid(node_call)) {
                node_call* call = ((node_call*)expr_list->attr_expr);
                const char* call_lexeme = call->attr_id->get_lexeme();
                Symbol_func* func = semantic->get_func(call_lexeme);
                if (func != NULL && func->return_struct != NULL) {
                    return i + 1;
                } else {
                    continue; 
                }
            }
        }
    }

    if (expr_list != NULL) {
        return i + 1;
    }

    return 0;
}

const char* Symbol_func::get_signature(ErrorManager* error) {
    const char* lexeme_type = (this->return_struct) ? this->return_struct->lexeme : error->get_token_name(this->return_primitive);
    const char* signature = Helper::str_cat(lexeme_type, " ");
    signature = Helper::str_cat(signature, this->lexeme);
    signature = Helper::str_cat(signature, "(");

    Symbol_var** param = this->params;
    for (int i = 0; i < this->num_params; i++) {
       signature = Helper::str_cat(signature, param[i]->type_struct ? param[i]->type_struct->lexeme : error->get_token_name(param[i]->type_primitive));
       signature = Helper::str_cat(signature, " ");
       signature = Helper::str_cat(signature, param[i]->lexeme);
       signature = Helper::str_cat( signature, ( i == this->num_params - 1) ? "" : ", "); 
    }

    signature = Helper::str_cat(signature, ")");

    return signature;
}
