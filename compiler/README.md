Cloud blocks is a environment that have like goal to help students of computer science. It is a website with RIA concepts that uses a compiler of the C language.

Version 0.11.10 - Libra
We remove any features from original C language. Below, see list with this features:

    - Scanner don't recognize any tokens, in flex generator, this tokens are 
    represented by the lines: 
    * reserved words
        "volatile"		{ count(); return(VOLATILE); }
        "register"		{ count(); return(T_REGISTER); }
    * operators
        "&="			{ count(); return(AND_ASSIGN);}
        "..."			{ count(); return(ELLIPSIS); }
        ">>="			{ count(); return(RIGHT_ASSIGN); }
        "<<="			{ count(); return(LEFT_ASSIGN); }
        "^="			{ count(); return(XOR_ASSIGN); }
        "|="			{ count(); return(OR_ASSIGN); }
        ">>"			{ count(); return(RIGHT_OP); }
        "<<"			{ count(); return(LEFT_OP); }
        "goto"			{ count(); return(GOTO); }
        "extern"		{ count(); return(EXTERN); }

