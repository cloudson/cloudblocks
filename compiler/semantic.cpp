#include "semantic.hpp"
#include "tokens.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <typeinfo>

Scope::Scope(){
      this->next = NULL;
      this->prev = NULL;
}

Scope::~Scope() {

}

SemanticAnalyser::SemanticAnalyser(ErrorManager* error){
      this->last_stmt = NULL;
      this->last_type = NULL;
      this->current_func = NULL;
      this->current_str = NULL;
      this->error = error;
      this->current_scope = new Scope();
      this->scopes = this->current_scope;
}

void SemanticAnalyser::analyse(node_program* ast){
      this->visit(ast);
}

void SemanticAnalyser::begin(){
      Scope* scope = new Scope();
      scope->prev = this->current_scope;
      this->current_scope->next = scope;
      this->current_scope = scope;
}

void SemanticAnalyser::end(){
      Scope* scope = this->current_scope;
      this->current_scope = this->current_scope->prev;
      delete scope;
}

Symbol_var* SemanticAnalyser::declaring(node_name_decl* name){
      Symbol_var* var = new Symbol_var(name->attr_id->get_lexeme());

      var->type_struct = (name->attr_type->attr_struct) ? this->get_struct(name->attr_type->attr_id->get_lexeme()) : NULL;
      var->type_primitive = name->attr_type->attr_primitive;
      if (name->attr_type->attr_length != NULL) {
            var->is_vector = true;
            node_length* len = name->attr_type->attr_length;
            int count = 0;
            while(len != NULL){
                  count++;
                  len = len->attr_next;
            }
            var->dimension = count;
            var->length = name->attr_type->attr_length;
      }
      this->current_scope->table[var->lexeme] = var;

      return var;
} 

Symbol_func* SemanticAnalyser::declaring(node_func* node){
      Symbol_func* func = new Symbol_func(node->attr_id->get_lexeme());
      func->return_struct = (node->attr_type->attr_struct) ? this->get_struct(node->attr_type->attr_id->get_lexeme()) : NULL;
      func->return_primitive = node->attr_type->attr_primitive;

      node_var_list* var_list = node->attr_param_list;
      int count = 0;
      while(var_list != NULL) {
            count++;
            var_list = var_list->attr_next;
      }
      func->num_params = count;
      func->params = (Symbol_var**)malloc(count * sizeof(Symbol_var*));
      this->table_func[func->lexeme] = func;

      return func; 
} 

Symbol_struct* SemanticAnalyser::declaring(node_struct* node){
      Symbol_struct* str = new Symbol_struct(node->attr_id->get_lexeme());
      node_var_list* var_list = node->attr_fields;
      int count = 0;
      while(var_list != NULL){
            count++;
            var_list = var_list->attr_next;
      }
      str->num_fields = count;
      str->array_fields = (Symbol_var**)malloc(count*sizeof(Symbol_var*));
      
      this->table_struct[str->lexeme] = str;
      return str;
} 

Symbol_var* SemanticAnalyser::var_exists(const char* lexeme){
      return (Symbol_var*)this->current_scope->table[lexeme];
}

Symbol_var* SemanticAnalyser::get_var(const char* lexeme){
      Scope* scope = this->current_scope;
      while(scope != NULL) {
            if((Symbol_var*)scope->table[lexeme] != NULL) {
                  return (Symbol_var*)this->current_scope->table[lexeme];
            }
            scope = scope->prev;
      }
      return NULL;
}

Symbol_func* SemanticAnalyser::get_func(const char* lexeme){
      return (Symbol_func*)this->table_func[lexeme];
}
Symbol_struct* SemanticAnalyser::get_struct(const char* lexeme){
      return (Symbol_struct*)this->table_struct[lexeme];
}

void SemanticAnalyser::visit(node_assign* node){
      Symbol_struct* str_left = NULL; 
      Symbol_struct* str_right = NULL; 

      if (node->attr_expr_left) node->attr_expr_left->accept(this); 
      str_left = this->last_type;
      if (node->attr_expr_right) node->attr_expr_right->accept(this); 
      str_right = this->last_type;

      node_expr* target_left = node->attr_expr_left;

      if (typeid(*(target_left)) == typeid(node_vector)) {
            node_vector* vec = (node_vector*)(node->attr_expr_left);
            target_left = vec->attr_name;
            Symbol_var* var_left = this->get_var(vec->attr_name->attr_id->get_lexeme());
            if (var_left && !var_left->is_vector){
                 this->error->throw_semantic(E_UNDEFINED_VECTOR, node->attr_line, var_left->lexeme, (var_left->type_struct) ? var_left->type_struct->lexeme : this->error->get_token_name(var_left->type_primitive));             
            } else {
                  if (var_left && vec->get_dimension() != var_left->dimension) {
                    this->error->throw_semantic(E_INVALID_DIMENSION, node->attr_line, var_left->lexeme);    
                  }
            }
      }

      if (typeid(*(target_left)) == typeid(node_name)) {
            Symbol_var* var_left  = this->get_var(((node_name*)target_left)->attr_id->get_lexeme());
            if (var_left) {
                  if (str_left != NULL) {
                        if (typeid(*(node->attr_expr_right)) == typeid(node_name)) {
                              Symbol_var* var_right = this->get_var(((node_name*)node->attr_expr_right)->attr_id->get_lexeme());
                              if (var_right && str_left != str_right) {
                                    this->error->throw_semantic(E_INVALID_ASSIGN, node->attr_line, str_left->lexeme, str_right ? str_right->lexeme : this->error->get_token_name(var_right->type_primitive));
                              }
                        } else {
                              if (typeid(*(node->attr_expr_right)) == typeid(node_vector)) {

                              } else {
                                    if (typeid(*(node->attr_expr_right)) == typeid(node_call)) {
                                          Symbol_func* func = this->get_func(((node_call*)node->attr_expr_right)->attr_id->get_lexeme());
                                          if (func && func->return_struct != str_left) {
                                                this->error->throw_semantic(E_INVALID_ASSIGN, node->attr_line, str_left->lexeme, func->return_struct ? func->return_struct->lexeme : this->error->get_token_name(func->return_primitive));
                                          }
                                    } else {
                                          this->error->throw_semantic(E_INVALID_ASSIGN, node->attr_line, str_left->lexeme, "expression");      
                                    }
                                    
                              }
                        }
                  } else {
                        if (typeid(*(node->attr_expr_right)) == typeid(node_call)) {
                              Symbol_func* func = this->get_func(((node_call*)node->attr_expr_right)->attr_id->get_lexeme());
                              
                              if (func && func->return_struct != NULL) {
                                    this->error->throw_semantic(E_INVALID_ASSIGN, node->attr_line, "expression", func->return_struct ? func->return_struct->lexeme : this->error->get_token_name(func->return_primitive));
                              }     
                        } else {
                              if (typeid(*(node->attr_expr_right)) == typeid(node_name)) {
                                    Symbol_var* var_right = this->get_var(((node_name*)node->attr_expr_right)->attr_id->get_lexeme());
                                    if (var_right && str_left != str_right) {
                                          this->error->throw_semantic(E_INVALID_ASSIGN, node->attr_line, "expression", str_right ? str_right->lexeme : this->error->get_token_name(var_right->type_primitive));
                                    }
                              }
                        }
                  }
            } else {
                  // if (var_left) {
                  //       if (typeid(*(node->attr_expr_right)) == typeid(node_name)) {
                  //             Symbol_var* var_right = this->get_var(((node_name*)node->attr_expr_right)->attr_id->get_lexeme());
                  //             if (!var_right->is_vector) {
                  //                   this->error->throw_semantic(E_INVALID_ASSIGN, node->attr_line, "vector", (var_right->type_struct) ? var_right->type_struct->lexeme : this->error->get_token_name(var_right->type_primitive));
                  //             } else {
                  //                   if (var_right->type_struct != NULL) {
                  //                         this->error->throw_semantic(E_INVALID_ASSIGN, node->attr_line, "vector", (var_right->type_struct) ? var_right->type_struct->lexeme : this->error->get_token_name(var_right->type_primitive));      
                  //                   }
                  //             }
                  //       } else {
                  //             if (node->attr_expr_left == target_left) {
                  //                   this->error->throw_semantic(E_INVALID_ASSIGN, node->attr_line, "vector", "expression");                  
                  //             }
                              
                  //       }
                  // }
            }
      }

} 
void SemanticAnalyser::visit(node_call* node){
      const char* lexeme = node->attr_id->get_lexeme();
      Symbol_func* func  = this->get_func(lexeme);

      if (func == NULL) {
            this->error->throw_semantic(E_UNDECLARED_FUNC, node->attr_line, lexeme);
      } else {

            int error = func->is_correct_call(node, this);
            if (error > 0) {
                 this->error->throw_semantic(E_INVALID_CALL, node->attr_line, func->get_signature(this->error)); 
            }      
      }

      
} 
void SemanticAnalyser::visit(node_expr_list* node){
      if (node->attr_expr) node->attr_expr->accept(this); 
      if (node->attr_next) node->attr_next->accept(this); 
} 
void SemanticAnalyser::visit(node_if* node){} 
void SemanticAnalyser::visit(node_func* node){
      this->current_func = node;
      this->begin();
      if (node->attr_type) node->attr_type->accept(this); 

      const char* lexeme = node->attr_id->get_lexeme();
      Symbol_func* func = this->get_func(lexeme);
      if (func != NULL) {
            this->error->throw_semantic(E_REDECLARING_FUNC, node->attr_line, lexeme);
      } else {
            func = this->declaring(node);
      }
      node->attr_func = func;
      if (node->attr_param_list) node->attr_param_list->accept(this);
      // here we go point to params var
      node_var_list* var_list = node->attr_param_list;
      const char* lexeme_var_list;
      int  i =0;
      while(var_list != NULL){
            lexeme_var_list = var_list->attr_name->attr_id->get_lexeme();
            func->params[i++] = this->get_var(lexeme_var_list);
            var_list = var_list->attr_next;
      }
      if (node->attr_stmt_list) node->attr_stmt_list->accept(this);

      this->end();
      if (this->last_stmt == NULL || typeid(*(this->last_stmt)) != typeid(node_return)) {
            if(!(this->last_stmt != NULL && func->return_primitive == T_VOID)) {
                  this->error->throw_semantic(E_EXPECTED_RETURN, 
                                             (this->last_stmt) ? this->last_stmt->attr_line :  node->attr_line,
                                             func->return_struct ? func->return_struct->lexeme : this->error->get_token_name(func->return_primitive));
            }
      }
      if (node->attr_next) node->attr_next->accept(this);
} 
void SemanticAnalyser::visit(node_name* node){
      this->last_type = NULL;
      bool first_call = (this->current_str == NULL);
      const char* lexeme = node->attr_id->get_lexeme();
      Symbol_var* var = this->var_exists(lexeme);
      node->attr_var = var;
      if (this->current_str != NULL){
            // accessing a attribute
            var = this->current_str->get_attr(lexeme);
            if (var == NULL) {
                  this->error->throw_semantic(E_UNDECLARED_ATTR, node->attr_line, lexeme, this->current_str->lexeme);
                  return;
            }
      } else {
            if (var == NULL) {
                  this->error->throw_semantic(E_UNDECLARED_VAR, node->attr_line, lexeme);
            }       
      }
      
      if (node->attr_expr != NULL) {
            if (typeid(*(node->attr_expr)) != typeid(node_name) && 
                typeid(*(node->attr_expr)) != typeid(node_vector)) {
                  this->error->throw_semantic(E_INVALID_ACCESS, node->attr_line, lexeme);
            } else {
                  Symbol_struct* last = this->current_str;
                  this->current_str = var->type_struct;
                  if (var->type_struct == NULL) {
                        this->error->throw_semantic(E_INVALID_ACCESS, node->attr_line, lexeme);    
                  } else {
                        node->attr_expr->accept(this);
                  }
                  this->current_str = this->current_str;
                  
            }
      } else {
            this->last_type = (var) ? var->type_struct : NULL;
      }
      if (first_call) {
            this->current_str = NULL;       
      }
      

} 
void SemanticAnalyser::visit(node_vector* node){
      if (node->attr_name) node->attr_name->accept(this); 
      if (node->attr_expr_list) node->attr_expr_list->accept(this); 
} 
void SemanticAnalyser::visit(node_name_decl* node){
      if (node->attr_type) node->attr_type->accept(this);

      const char* lexeme = node->attr_id->get_lexeme();
      Symbol_var* var = this->var_exists(lexeme);
      if (var != NULL) {
            this->error->throw_semantic(E_REDECLARING_VAR, node->attr_line, lexeme);
      } else {
            node->attr_var = this->declaring(node);
      }

      if (node->attr_assign) node->attr_assign->accept(this);
} 
void SemanticAnalyser::visit(node_negation* node){
      if (node->attr_expr) node->attr_expr->accept(this);
}         
void SemanticAnalyser::visit(node_op_bitwise* node){
      if (node->attr_expr_1) node->attr_expr_1->accept(this);
      if (node->attr_expr_2) node->attr_expr_2->accept(this);
} 
void SemanticAnalyser::visit(node_op_addition* node){

      if (node->attr_expr_1) node->attr_expr_1->accept(this);
      if (node->attr_expr_2) node->attr_expr_2->accept(this); 

} 
void SemanticAnalyser::visit(node_op_boolean* node){} 
void SemanticAnalyser::visit(node_op_multiplication* node){
      if (node->attr_expr_1) node->attr_expr_1->accept(this);
      if (node->attr_expr_2) node->attr_expr_2->accept(this);
} 
void SemanticAnalyser::visit(node_op_relational* node){
      if (node->attr_expr_1) node->attr_expr_1->accept(this);
      if (node->attr_expr_2) node->attr_expr_2->accept(this);
} 
void SemanticAnalyser::visit(node_primitive* node){} 
void SemanticAnalyser::visit(node_print* node){

      if (node->attr_expr_list) node->attr_expr_list->accept(this);
} 
void SemanticAnalyser::visit(node_read* node){
      if (node->attr_format && 
          typeid(*(node->attr_format)) != typeid(node_name) && 
          typeid(*(node->attr_format)) != typeid(node_primitive) ) {

          this->error->throw_semantic(E_INVALID_FORMAT_SCANF, node->attr_line);

      }

      if (node->attr_format) node->attr_format->accept(this);
      if (node->attr_expr) node->attr_expr->accept(this);
} 
void SemanticAnalyser::visit(node_return* node){
      this->last_type = NULL;
      if (node->attr_expr) node->attr_expr->accept(this); 
      Symbol_id* id = this->current_func->attr_type->attr_id;
      if(id == NULL) {
            // expected expression
            if (node->attr_expr == NULL && this->current_func->attr_type->attr_primitive != T_VOID) {
                  this->error->throw_semantic(E_INVALID_RETURN, node->attr_line, "expression", this->error->get_token_name(T_VOID));
            }

            if (node->attr_expr != NULL && this->current_func->attr_type->attr_primitive == T_VOID) {
                  this->error->throw_semantic(E_INVALID_RETURN, node->attr_line, this->error->get_token_name(T_VOID), "expression");
            }

            if (node->attr_expr != NULL && typeid(*(node->attr_expr)) == typeid(node_name)) {
                  node_name* name = (node_name*)node->attr_expr;
                  Symbol_var* var = this->get_var(name->attr_id->get_lexeme());
                  if (var) {
                        Symbol_struct* str = (this->current_func->attr_type->attr_id) ? this->get_struct(this->current_func->attr_type->attr_id->get_lexeme()) : NULL;
                        if (this->last_type != str) {
                             this->error->throw_semantic(E_INVALID_RETURN, node->attr_line, this->last_type ? this->last_type->lexeme : this->error->get_token_name(var->type_primitive), 
                                                                                            str ? str->lexeme : this->error->get_token_name(this->current_func->attr_type->attr_primitive)  ); 
                        }
                  }
            }
      } else {
            const char* lexeme = id->get_lexeme();
            Symbol_struct* str = this->get_struct(lexeme);
            if (node->attr_expr && typeid(*(node->attr_expr)) == typeid(node_name)) {
                  Symbol_var* var  = this->get_var(((node_name*)node->attr_expr)->attr_id->get_lexeme());
                  if (var != NULL && var->type_struct != str) {
                        this->error->throw_semantic(E_INVALID_RETURN, node->attr_line, lexeme, var->type_struct ? var->type_struct->lexeme : "expression");
                  }
            } else {
                  if (node->attr_expr && typeid(*(node->attr_expr)) == typeid(node_vector)) {
                        
                  } else {
                        if (node->attr_expr && typeid(*(node->attr_expr)) == typeid(node_call)) {
                        Symbol_func* func  = this->get_func(((node_call*)node->attr_expr)->attr_id->get_lexeme());
                              if (func->return_struct == NULL) {
                                   // found expression  
                                   this->error->throw_semantic(E_INVALID_RETURN, node->attr_line, lexeme, "expression");
                              } else {
                                 if (func != NULL && func->return_struct != str) {
                                    this->error->throw_semantic(E_INVALID_RETURN, node->attr_line, lexeme, func->lexeme);
                                 }  
                              }
                        } else {
                              this->error->throw_semantic(E_INVALID_RETURN, node->attr_line, lexeme, "expression");
                        }
                  }           
            }
      }
} 
void SemanticAnalyser::visit(node_signal* node){} 
void SemanticAnalyser::visit(node_stmt_list* node){
      if (node->attr_stmt){
            node->attr_stmt->accept(this);
            this->last_stmt = node->attr_stmt;    
      } 
      if (node->attr_next) node->attr_next->accept(this);
} 

void SemanticAnalyser::visit(node_type* node){
      if (node->attr_struct) {
            Symbol_struct* str = this->get_struct(node->attr_id->get_lexeme());
            if (str == NULL) {
                  this->error->throw_semantic(E_UNDECLARED_STRUCT,node->attr_line, node->attr_id->get_lexeme());
            }
      }
} 

void SemanticAnalyser::visit(node_var_list* node){
      if (node->attr_name) node->attr_name->accept(this);
      if (node->attr_next) node->attr_next->accept(this);
} 

void SemanticAnalyser::visit(node_while* node){
      if (node->attr_expr) node->attr_expr->accept(this);
      if (node->attr_stmt) node->attr_stmt->accept(this);
}
void SemanticAnalyser::visit(node_sizeof* node){} 
void SemanticAnalyser::visit(node_case* node){} 
void SemanticAnalyser::visit(node_case_list* node){} 
void SemanticAnalyser::visit(node_switch* node){} 
void SemanticAnalyser::visit(node_do* node){} 
void SemanticAnalyser::visit(node_struct* node){
      this->begin();
      
      Symbol_struct* str = this->get_struct(node->attr_id->get_lexeme());
      if (str != NULL) {
            this->error->throw_semantic(E_REDECLARING_STRUCT, node->attr_line, node->attr_id->get_lexeme());
      } else {
           str = this->declaring(node);
      }
      if (node->attr_fields) node->attr_fields->accept(this);
      node_var_list* var_list = node->attr_fields;
      
      for (int i = 0; i < str->num_fields; i++, var_list = var_list->attr_next) {
            node_name_decl* name = var_list->attr_name;
            Symbol_var* var = this->get_var(name->attr_id->get_lexeme()); //new Symbol_var(name->attr_id->get_lexeme());
            var->type_struct = (name->attr_type->attr_struct) ? this->get_struct(name->attr_type->attr_id->get_lexeme()) : NULL;
            var->type_primitive = name->attr_type->attr_primitive;
            
            str->array_fields[i] = var;
      }
      node->attr_str = str;
      this->end();
      if (node->attr_next) node->attr_next->accept(this);      
} 

void SemanticAnalyser::visit(node_enum* node){} 
void SemanticAnalyser::visit(node_value* node){} 
void SemanticAnalyser::visit(node_address* node){} 
void SemanticAnalyser::visit(node_for* node){
      if (node->attr_init) node->attr_init->accept(this);
      if (node->attr_expr) node->attr_expr->accept(this);
      if (node->attr_steps) node->attr_steps->accept(this); 
      if (node->attr_stmt) node->attr_stmt->accept(this); 
}
void SemanticAnalyser::visit(node_length* node){
      if (node->attr_next) node->attr_next->accept(this);
}
void SemanticAnalyser::visit( node_program* node){

      if(node->attr_struct) node->attr_struct->accept(this);
      if(node->attr_var_list) node->attr_var_list->accept(this);
      if(node->attr_func) node->attr_func->accept(this);
} 