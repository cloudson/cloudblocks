#ifndef CLOUD_SYMBOLS_TABLES
#define CLOUD_SYMBOLS_TABLES 1

class scope;
class AcessoLocal;
class ir_expr;
class Symbol_var;
class node_call;
class ErrorManager;
class SemanticAnalyser;
class node_length;

#define PRIME_TABLE_LENGTH 37
#define ARRAY_LEXEMES_LENGTH 10
#define ARRAY_LEXEMES_MORE_SPACE 10

/*
*  Classe abstrata que representa o símbolo
*  e seus filhos concretos.
*  O simbolo é o elemento mais importante da tabela de simbolos,
* no caso da tabela de palavras reservada, o simbolo guarda o numero do token
*/


class Symbol {
    public:
        Symbol(char* lexeme);
        Symbol();
        ~Symbol();
        int position; // posição no arranjao
        char* lexeme;
};

class Symbol_id: public Symbol{
     public:
         Symbol_id(const char* lexeme);
         const char* get_lexeme();
};
class Symbol_struct: public Symbol{
    public:
        Symbol_struct(const char* lexeme);
        Symbol_var* get_attr(const char* lexeme);
        int num_fields;
        Symbol_var** array_fields;
        const char* lexeme;
};

class Symbol_var: public Symbol{
    public:
        Symbol_var(const char* lexeme);
        Symbol_struct* type_struct;
        int type_primitive;
        bool is_vector;
        int  dimension;
        node_length* length;
        const char* lexeme;
};

class Symbol_func: public Symbol{
    public:
        Symbol_func(const char* name);
        int is_correct_call(node_call* call, SemanticAnalyser* semantic);
        const char* get_signature(ErrorManager* error);
        const char* lexeme;
        Symbol_struct* return_struct;
        int return_primitive;
        int array;
        Symbol_var** params;
        int num_params;
        int access;
        Symbol_func* overload; // usado para criar uma lista de metodos sobrecarregados

};

typedef struct struct_node* entry;
struct struct_node {
    entry  next;
    Symbol * info;
};

// class Symbols_table{
//     public:
//         Symbols_table();
//         ~Symbols_table();
//         void destroy_symbols(); // deve ser chamado antes do destrutor;
//         char* get_lexeme(Symbol* symbol);
//         int insert(char * lexeme, Symbol* symbol);
//         Symbol* search(char * lexeme);
//         void print_symbol(Symbol* symbol);
//         void SET_array_lexemes(char* lexeme);
//         virtual void show()=0;  // método que força a tabela de símbolos a ser abstrata
//         Symbol** getAtributos();
//     protected:
//          entry * table ; // array que representa a tabela em si
//          int current_position_array_lexemes; // posicao atual do arranjao de lexemas
//          char * array_lexemes;  //Arranjão de lexemas da tabela
//          int length_array_lexemes; // tamanho atual do arranjao de lexemas
//     private:
//         int hash(char * lexeme);
// };

// class Symbols_table_rw: public Symbols_table{
//     public:
//         Symbols_table_rw();
//         void show();
// };

// class Symbols_table_id: public Symbols_table{ public: void show(); };

// class Symbols_table_literal: public Symbols_table{ public: void show(); };
#endif // SYMBOLS_TABLES_H
