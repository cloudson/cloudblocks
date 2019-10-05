%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <typeinfo>
    #include "scanner.h"
    #include "ast.h"
    #define YYERROR_VERBOSE 1
    #define YYDEBUG 1 
    
    class node_name_decl;
    
    node_program   *ast; 
    node_var_list  *var_list;
    node_func_list *func_list;
    extern char* yytext;
    extern int yyparse();
    extern int column;
    void yyerror(char *s) { 
        fflush(stdout);
	    printf("\n%*s\n%*s\n", column, "^", column, s);
    }
    
    void insertVar(node_name_decl* var){
        if(var_list == NULL){
            var_list = new node_var_list(var,NULL,0);
            return;
        }
        
        node_var_list* aux = var_list;
        while(aux->attr_var_list != NULL){
            aux = aux->attr_var_list;
        }
        aux->attr_var_list = new node_var_list(var,NULL,0);
    }
      
%}

%union {
	char* str;
	int   token;
    void* Nprogram; // há um problema com node_program
    node_expr* NExp;
    node_op_multiplication* Nmult;
    node_expr* Nadd;
    node* Nstmt;    
    node_name_decl* Nnamedecl;
    node_type* Ntype;
    
} 


%token <token> T_EOF 
%token <token>T_BREAK 
%token <token>T_CASE 
%token <token> T_CHAR 
%token <token> T_CONTINUE 
%token <token> T_DEFAULT 
%token <token> T_DOUBLE 
%token <token> T_ELSE 
%token <token> T_FLOAT 
%token <token> T_FOR 
%token <token> T_IF 
%token <token> T_INT 
%token <token> T_RETURN  
%token <token> T_SIZEOF 
%token <token> T_SWITCH 
%token <token> T_STRUCT 
%token <token> T_ADD_ASSIGN 
%token <token> T_ADD 
%token <token> T_SUB_ASSIGN
%token <token> T_SUB  
%token <token> T_MULT_ASSIGN 
%token <token> T_MULT 
%token <token> T_DIV 
%token <token> T_DIV_ASSIGN 
%token <token> T_MOD 
%token <token> T_MOD_ASSIGN 
%token <token> T_INC 
%token <token> T_DEC 
%token <token> T_PTR 
%token <token> T_AND 
%token <token> T_OR 
%token <token> T_BEGIN 
%token <token> T_END 
%token <token> T_COMMA 
%token <token> T_SEMICOLON 
%token <token> T_QUEST 
%token <token> T_LESS 
%token <token> T_LESS_EQ
%token <token> T_GREATER
%token <token> T_GREATER_EQ
%token <token> T_EQUAL 
%token <token> T_NOT_EQUAL
%token <token> T_DOT
%token <token> T_BRACKET_L
%token <token> T_BRACKET_R
%token <token> T_PARENT_L
%token <token> T_PARENT_R
%token <token> T_WHILE
%token <token> T_VOID
%token <token> T_LITERAL
%token <token> T_CONSTANT 
%token <token> T_ID
%token <token> T_ENUM
%token <token> T_SHORT
%token <token> T_SIGNED
%token <token> T_UNSIGNED
%token <token> T_STRING_LITERAL
%token <token> T_LONG
%token <token> T_STATIC
%token <token> T_TYPEDEF
%token <token> T_UNION 
%token <token> T_DO
%token <token> T_ADDRESS
%token <token> T_DOT_DOUBLE
%token <token> T_NEG
%token <token> T_ASSIGN
%token <token> T_PIPE
%token <token> T_XOR
%token <token> T_LEFT
%token <token> T_RIGHT


%type <token> type_specifier 
%type <token> unary_operator 
%type <token> struct_or_union_specifier
%type <token> struct_or_union
%type <token> primary_expression
%type <Nnamedecl> direct_declarator
%type <Nnamedecl> declarator
%type <Nnamedecl> init_declarator
%type <Nnamedecl> init_declarator_list
%type <Ntype> declaration_specifiers // trocar para Ntype
%type <NExp> conditional_expression
%type <NExp> assignment_expression
%type <NExp> equality_expression
%type <NExp> initializer
%type <Nmult> multiplicative_expression
%type <Nadd> additive_expression
%type <NExp> shift_expression
%type <NExp> relational_expression
%type <NExp> logical_or_expression
%type <NExp> and_expression
%type <NExp> exclusive_or_expression
%type <NExp> inclusive_or_expression
%type <NExp> logical_and_expression
%type <token> assignment_operator



%start translation_unit
%%

translation_unit
	: external_declaration {
        if(!ast){
           ast = new node_program(var_list,func_list,0);
        }
    }
	| translation_unit external_declaration 
	;

primary_expression
	: T_ID { $$ = 0 }
	| T_CONSTANT {$$ = 0}
	| T_STRING_LITERAL { $$ = 0 }
	| T_PARENT_L expression T_PARENT_R { $$ = 0 }
	;

postfix_expression
	: primary_expression
	| postfix_expression T_BRACKET_L expression T_BRACKET_R
	| postfix_expression T_PARENT_L T_PARENT_R
	| postfix_expression T_PARENT_L argument_expression_list T_PARENT_R
	| postfix_expression T_DOT T_ID
	| postfix_expression T_PTR T_ID
	| postfix_expression T_INC
	| postfix_expression T_DEC
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list T_COMMA assignment_expression
	;

unary_expression
	: postfix_expression
//	| T_INC unary_expression {}
//	| T_DEC unary_expression
	| unary_operator cast_expression
//	| T_SIZEOF unary_expression
//	| T_SIZEOF T_PARENT_L type_name T_PARENT_R
	;

unary_operator
	: T_ADDRESS { $$ = T_ADDRESS}
	| T_MULT    { $$ = T_MULT}
	| T_ADD     { $$ = T_ADD}
	| T_SUB     { $$ = T_SUB}
	| T_NEG     { $$ = T_NEG}
	;

cast_expression
	: unary_expression 
	| T_PARENT_L type_name T_PARENT_R cast_expression
	;

multiplicative_expression
	: cast_expression { $$ = new node_op_multiplication(0,NULL,NULL,0); } 
	| multiplicative_expression T_MULT cast_expression { $$ = new node_op_multiplication(T_MULT,NULL,NULL,0) }
	| multiplicative_expression T_DIV cast_expression { $$ = new node_op_multiplication(T_DIV,NULL,NULL,0) }
	| multiplicative_expression T_MOD cast_expression { $$ = new node_op_multiplication(T_MOD,NULL,NULL,0) }
	;

additive_expression
	: multiplicative_expression { $$ = $1}
	| additive_expression T_ADD multiplicative_expression { $$ = new node_op_addition(T_ADD,NULL,NULL,0) }
	| additive_expression T_SUB multiplicative_expression { $$ = new node_op_addition(T_SUB,NULL,NULL,0) }
	;

shift_expression
	: additive_expression { $$  = NULL } 
	| shift_expression T_LEFT additive_expression { $$  = NULL } 
	| shift_expression T_RIGHT additive_expression { $$  = NULL } 
	;

relational_expression
	: shift_expression { $$  = NULL } 
	| relational_expression T_GREATER shift_expression { $$  = NULL } 
	| relational_expression T_LESS shift_expression { $$  = NULL } 
	| relational_expression T_LESS_EQ shift_expression { $$  = NULL } 
	| relational_expression T_GREATER_EQ shift_expression { $$  = NULL } 
	;

equality_expression
	: relational_expression { $$  = NULL } 
	| equality_expression T_EQUAL relational_expression { $$  = NULL }
	| equality_expression T_NOT_EQUAL relational_expression { $$  = NULL }
	;

and_expression
	: equality_expression { $$ = $1}
	| and_expression T_ADDRESS equality_expression { $$ = new node_op_boolean(T_AND,NULL,NULL,0) }
	;

exclusive_or_expression
	: and_expression { $$ = $1 }
	| exclusive_or_expression T_XOR and_expression { $$ = new node_op_boolean(T_XOR,NULL,NULL,0) }
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = $1}
	| inclusive_or_expression T_PIPE exclusive_or_expression { $$ = new node_op_boolean(T_OR,NULL,NULL,0) }
	;

logical_and_expression
	: inclusive_or_expression { $$ = $1}
	| logical_and_expression T_AND inclusive_or_expression { $$ = new node_op_boolean(T_AND,NULL,NULL,0) }
	;

logical_or_expression
	: logical_and_expression {$$ = $1}
	| logical_or_expression T_OR logical_and_expression { $$ = new node_op_boolean(T_OR,NULL,NULL,0) }
	;

conditional_expression
	: logical_or_expression {$$ = $1}
	| logical_or_expression T_QUEST expression T_DOT_DOUBLE conditional_expression { $$ = NULL } 
	;

assignment_expression
	: conditional_expression {$$ = $1 } 
	| unary_expression assignment_operator assignment_expression {
        //$$ = new node_assign(NULL,$3,0);
        $$ = $3;
     }
	;

assignment_operator
	: T_ASSIGN {$$ =  T_ASSIGN }
	| T_MULT_ASSIGN {$$ =  T_MULT_ASSIGN }
	| T_DIV_ASSIGN {$$ =  T_DIV_ASSIGN }
	| T_MOD_ASSIGN {$$ =  T_MOD_ASSIGN }
	| T_ADD_ASSIGN {$$ =  T_ADD_ASSIGN }
	| T_SUB_ASSIGN {$$ =  T_SUB_ASSIGN }
	;

expression
	: assignment_expression
	| expression T_COMMA assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers T_SEMICOLON
	| declaration_specifiers init_declarator_list T_SEMICOLON { 
        node_name_decl* aux = $2;
        node_type* type = $1 ; //new node_type($1,0);
        insertVar( new node_name_decl(type,aux->attr_id,aux->attr_assign,0) ); 
    }
	;

declaration_specifiers
	: storage_class_specifier {$$ = 0}
	| storage_class_specifier declaration_specifiers {$$ = 0}
	| type_specifier  {$$ = new node_type($1,0);}
	| type_specifier  declaration_specifiers {$$ = new node_type(1,0);}
	;

init_declarator_list
	: init_declarator {$$ = $1}
	| init_declarator_list T_COMMA init_declarator {$$ = $1}
	;

init_declarator
	: declarator {$$ = $1}
	| declarator T_ASSIGN initializer {
        node_name_decl* aux = $1;
        if(typeid($2) == typeid(node_assign*)){
            aux->attr_assign = $2;
        }else{
            aux->attr_assign = new node_assign(NULL,$2,0);
        }
        $$ = aux;
    }
	;

storage_class_specifier
	: T_TYPEDEF
	| T_STATIC
	;

type_specifier
	: T_VOID {$$ = T_VOID} 
	| T_CHAR {$$ = T_CHAR}
	| T_SHORT {$$ = T_SHORT}
	| T_INT {$$ = T_INT}
	| T_LONG {$$ = T_LONG}
	| T_FLOAT {$$ = T_FLOAT}
	| T_DOUBLE {$$ = T_DOUBLE}
	| T_SIGNED {$$ = T_SIGNED}
	| T_UNSIGNED {$$ = T_UNSIGNED}
	| struct_or_union_specifier {$$ = 0}
	| enum_specifier {$$ = 0}
	;

struct_or_union_specifier
	: struct_or_union T_ID T_BEGIN struct_declaration_list T_END { $$ = 0 }
	| struct_or_union T_BEGIN struct_declaration_list T_END { $$ = 0 }
	| struct_or_union T_ID { $$ = 0 }
	;

struct_or_union
	: T_STRUCT {$$ = T_STRUCT}
    | T_UNION  {$$ = T_UNION}
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list T_SEMICOLON
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list T_COMMA struct_declarator
	;

struct_declarator
	: declarator
	| T_DOT_DOUBLE constant_expression
	| declarator T_DOT_DOUBLE constant_expression
	;

enum_specifier
	: T_ENUM T_BEGIN enumerator_list T_END 
	| T_ENUM T_ID T_BEGIN enumerator_list T_END 
	| T_ENUM T_ID
	;

enumerator_list
	: enumerator 
	| enumerator_list T_COMMA enumerator
	;

enumerator
	: T_ID 
	| T_ID T_ASSIGN constant_expression
	;

declarator
	: pointer direct_declarator {$$ = $2}
	| direct_declarator {$$ = $1}
	;

direct_declarator
	: T_ID { $$ = new node_name_decl(0,new Symbol_id(yytext),0)}
	| T_PARENT_L declarator T_PARENT_R { $$ = new node_name_decl(0,NULL,0)}
	| direct_declarator T_BRACKET_L constant_expression T_BRACKET_R { $$ = new node_name_decl(0,NULL,0)}
	| direct_declarator T_BRACKET_L T_BRACKET_R { $$ = new node_name_decl(0,NULL,0)}
	| direct_declarator T_PARENT_L parameter_type_list T_PARENT_R { $$ = new node_name_decl(0,NULL,0)}
	| direct_declarator T_PARENT_L identifier_list T_PARENT_R { $$ = new node_name_decl(0,NULL,0)}
	| direct_declarator T_PARENT_L T_PARENT_R { $$ = new node_name_decl(0,NULL,0)}
	;

pointer
	: T_MULT
	| T_MULT pointer
	;

parameter_type_list
	: parameter_list
	;

parameter_list
	: parameter_declaration
	| parameter_list T_COMMA parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: T_ID
	| identifier_list T_COMMA T_ID
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: T_PARENT_L abstract_declarator T_PARENT_R
	| T_BRACKET_L T_BRACKET_R
	| T_BRACKET_L constant_expression T_BRACKET_R
	| direct_abstract_declarator T_BRACKET_L T_BRACKET_R
	| direct_abstract_declarator T_BRACKET_L constant_expression T_BRACKET_R
	| T_PARENT_L T_PARENT_R
	| T_PARENT_L parameter_type_list T_PARENT_R
	| direct_abstract_declarator T_PARENT_L T_PARENT_R
	| direct_abstract_declarator T_PARENT_L parameter_type_list T_PARENT_R
	;

initializer
	: assignment_expression {$$ = $1}
	| T_BEGIN initializer_list T_END { $$ = NULL }
	| T_BEGIN initializer_list T_COMMA T_END { $$ = NULL }
	;

initializer_list
	: initializer
	| initializer_list T_COMMA initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: T_ID T_DOT_DOUBLE statement
	| T_CASE constant_expression T_DOT_DOUBLE statement
	| T_DEFAULT T_DOT_DOUBLE statement
	;

compound_statement
	: T_BEGIN T_END
	| T_BEGIN statement_list T_END
	| T_BEGIN declaration_list T_END
	| T_BEGIN declaration_list statement_list T_END
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: T_SEMICOLON
	| expression T_SEMICOLON
	;

selection_statement
	: T_IF T_PARENT_L expression T_PARENT_R statement
	| T_IF T_PARENT_L expression T_PARENT_R statement T_ELSE statement
	| T_SWITCH T_PARENT_L expression T_PARENT_R statement
	;

iteration_statement
	: T_WHILE T_PARENT_L expression T_PARENT_R statement
	| T_DO statement T_WHILE T_PARENT_L expression T_PARENT_R T_SEMICOLON
	| T_FOR T_PARENT_L expression_statement expression_statement T_PARENT_R statement
	| T_FOR T_PARENT_L expression_statement expression_statement expression T_PARENT_R statement
	;

jump_statement
	: T_CONTINUE T_SEMICOLON
	| T_BREAK T_SEMICOLON
	| T_RETURN T_SEMICOLON
	| T_RETURN expression T_SEMICOLON
	;



external_declaration
	: function_definition 
	| declaration  
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;

%%
