#include <iostream>
#include "tokens.h"
#include "scanner.h"
#include "parser.hpp"
#include "ast.h"
#include "visitor.h"
#include "semantic.hpp"
#include "helper.hpp"
#include "converter.hpp"

using namespace std;
extern int yylex();
//extern int yyparse();

int main(int argc,char * argv[])
{

    const char* src = Helper::str_cpy(argv[1]);
    FILE * fp = fopen(src,"r");
    yyrestart(fp);
    int token =0; 
    
    Parser* parser = new Parser();

    if (CLOUD_DEBUG) fprintf(stdout, "=================== Syntax ====================\n");
    node_program* program = parser->get_AST();   

    if (!parser->error.is_syntax_ok()) {
        return 1;
    }
    if (CLOUD_DEBUG) {
        Visitor_print* printer = new Visitor_print();
        printer->visit(program);    
    }
    
    
    if (CLOUD_DEBUG) fprintf(stdout, "=================== Semantic ====================\n");
    SemanticAnalyser* semantic = new SemanticAnalyser(&(parser->error));
    semantic->analyse(program);

    if (!parser->error.is_semantic_ok()){
        return 2;
    }

    Converter* converter = new Converter();
    converter->convert(program);

    return 0;
}
