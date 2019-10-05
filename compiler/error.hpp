#ifndef CLOUD_ERROR
#define CLOUD_ERROR 1
#include <vector>
#include <stdlib.h>

#define E_REDECLARING_VAR 1
#define E_INVALID_ASSIGN  2
#define E_UNDECLARED_VAR  3
#define E_REDECLARING_FUNC 4
#define E_INVALID_RETURN 5
#define E_REDECLARING_STRUCT 6
#define E_UNDECLARED_STRUCT 7 
#define E_UNDECLARED_FUNC 8 
#define E_EXPECTED_RETURN 9 
#define E_IF_EXPRESSION 10
#define E_INVALID_CALL 11
#define E_UNDEFINED_VECTOR 12
#define E_INVALID_DIMENSION 13
#define E_INVALID_FORMAT_SCANF 14
#define E_INVALID_ACCESS 15
#define E_ATTRIBUTTE_INVALID 16
#define E_UNDECLARED_ATTR 17
	
class ErrorManager
{
	private:
		bool syntax_ok;
		bool semantic_ok;
	public:
		bool is_syntax_ok();
		bool is_semantic_ok();
		void set_syntax_ok(bool val);
		void set_semantic_ok(bool val);
		void throw_syntax(int token_expected, int token_found, int line);
		void throw_syntax(int* tokens, int token_found, int line);
		void throw_semantic(int errorno, int line, const char* info1 = NULL, const char* info2 = NULL, const char* info3 = NULL); 
		static ErrorManager& getInstance(); // singleton! 
		const char* get_token_name(int token);
		std::vector<const char*> errors;
		ErrorManager(){}
		~ErrorManager(){}
		
};

#endif