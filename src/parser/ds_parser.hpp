/* A Bison parser, made by GNU Bison 3.7.3.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_DAS_YY_DS_PARSER_HPP_INCLUDED
# define YY_DAS_YY_DS_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef DAS_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define DAS_YYDEBUG 1
#  else
#   define DAS_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define DAS_YYDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined DAS_YYDEBUG */
#if DAS_YYDEBUG
extern int das_yydebug;
#endif
/* "%code requires" blocks.  */
#line 2 "ds_parser.ypp"

    #include "daScript/misc/platform.h"
    #include "daScript/ast/ast.h"
    #include "daScript/ast/ast_generate.h"
    #include "daScript/ast/ast_expressions.h"

    enum {
        CorM_COPY   = 0,
        CorM_MOVE   = (1<<0),
        CorM_CLONE  = (1<<1)
    };

    namespace das {
        extern ProgramPtr               g_Program;
        extern FileAccessPtr            g_Access;
        extern vector<FileInfo *>       g_FileAccessStack;
        extern das_map<string,string>   das_module_alias;

        extern ReaderMacro *            g_ReaderMacro;
        extern ExprReader *             g_ReaderExpr;
    }
    using namespace das;

    struct VariableDeclaration {
        VariableDeclaration ( vector<pair<string,LineInfo>> * n, TypeDecl * t, Expression * i )
            : pNameList(n), pTypeDecl(t), pInit(i) {}
        virtual ~VariableDeclaration () {
            if ( pNameList ) delete pNameList;
            delete pTypeDecl;
            if ( pInit ) delete pInit;
            if ( annotation ) delete annotation;
        }
        vector<pair<string,LineInfo>>   *pNameList;
        TypeDecl                *pTypeDecl;
        Expression              *pInit;
        bool                    init_via_move = false;
        bool                    init_via_clone = false;
        bool                    override = false;
        AnnotationArgumentList  *annotation = nullptr;
    };

    ExprLooksLikeCall * parseFunctionArguments ( ExprLooksLikeCall * pCall, Expression * arguments );
    vector<ExpressionPtr> sequenceToList ( Expression * arguments );
    void deleteVariableDeclarationList ( vector<VariableDeclaration *> * list );
    void varDeclToTypeDecl ( TypeDecl * pType, vector<VariableDeclaration*> * list, bool needNames = true );
    void runFunctionAnnotations ( Function * func, AnnotationList * annL, const LineInfo & at );

    LineInfo tokAt ( const struct YYLTYPE & li );
    LineInfo tokRangeAt ( const struct YYLTYPE & li, const struct YYLTYPE & lie );

    Annotation * findAnnotation ( const string & name, const LineInfo & at );

    extern bool das_need_oxford_comma;
    extern bool das_force_oxford_comma;
    extern bool das_supress_errors;
    extern int das_arrow_depth;

    extern Structure * g_thisStructure;

#line 117 "ds_parser.hpp"

/* Token kinds.  */
#ifndef DAS_YYTOKENTYPE
# define DAS_YYTOKENTYPE
  enum das_yytokentype
  {
    DAS_YYEMPTY = -2,
    DAS_YYEOF = 0,                 /* "end of file"  */
    DAS_YYerror = 256,             /* error  */
    DAS_YYUNDEF = 257,             /* "invalid token"  */
    LEXER_ERROR = 258,             /* "lexer error"  */
    DAS_STRUCT = 259,              /* "struct"  */
    DAS_CLASS = 260,               /* "class"  */
    DAS_LET = 261,                 /* "let"  */
    DAS_DEF = 262,                 /* "def"  */
    DAS_WHILE = 263,               /* "while"  */
    DAS_IF = 264,                  /* "if"  */
    DAS_STATIC_IF = 265,           /* "static_if"  */
    DAS_ELSE = 266,                /* "else"  */
    DAS_FOR = 267,                 /* "for"  */
    DAS_CATCH = 268,               /* "recover"  */
    DAS_TRUE = 269,                /* "true"  */
    DAS_FALSE = 270,               /* "false"  */
    DAS_NEWT = 271,                /* "new"  */
    DAS_TYPEINFO = 272,            /* "typeinfo"  */
    DAS_TYPE = 273,                /* "type"  */
    DAS_IN = 274,                  /* "in"  */
    DAS_IS = 275,                  /* "is"  */
    DAS_AS = 276,                  /* "as"  */
    DAS_ELIF = 277,                /* "elif"  */
    DAS_STATIC_ELIF = 278,         /* "static_elif"  */
    DAS_ARRAY = 279,               /* "array"  */
    DAS_RETURN = 280,              /* "return"  */
    DAS_NULL = 281,                /* "null"  */
    DAS_BREAK = 282,               /* "break"  */
    DAS_TRY = 283,                 /* "try"  */
    DAS_OPTIONS = 284,             /* "options"  */
    DAS_TABLE = 285,               /* "table"  */
    DAS_EXPECT = 286,              /* "expect"  */
    DAS_CONST = 287,               /* "const"  */
    DAS_REQUIRE = 288,             /* "require"  */
    DAS_OPERATOR = 289,            /* "operator"  */
    DAS_ENUM = 290,                /* "enum"  */
    DAS_FINALLY = 291,             /* "finally"  */
    DAS_DELETE = 292,              /* "delete"  */
    DAS_DEREF = 293,               /* "deref"  */
    DAS_TYPEDEF = 294,             /* "typedef"  */
    DAS_WITH = 295,                /* "with"  */
    DAS_CAST = 296,                /* "cast"  */
    DAS_OVERRIDE = 297,            /* "override"  */
    DAS_ABSTRACT = 298,            /* "abstract"  */
    DAS_UPCAST = 299,              /* "upcast"  */
    DAS_ITERATOR = 300,            /* "iterator"  */
    DAS_VAR = 301,                 /* "var"  */
    DAS_ADDR = 302,                /* "addr"  */
    DAS_CONTINUE = 303,            /* "continue"  */
    DAS_WHERE = 304,               /* "where"  */
    DAS_PASS = 305,                /* "pass"  */
    DAS_REINTERPRET = 306,         /* "reinterpret"  */
    DAS_MODULE = 307,              /* "module"  */
    DAS_PUBLIC = 308,              /* "public"  */
    DAS_LABEL = 309,               /* "label"  */
    DAS_GOTO = 310,                /* "goto"  */
    DAS_IMPLICIT = 311,            /* "implicit"  */
    DAS_EXPLICIT = 312,            /* "explicit"  */
    DAS_SHARED = 313,              /* "shared"  */
    DAS_SMART_PTR = 314,           /* "smart_ptr"  */
    DAS_UNSAFE = 315,              /* "unsafe"  */
    DAS_TBOOL = 316,               /* "bool"  */
    DAS_TVOID = 317,               /* "void"  */
    DAS_TSTRING = 318,             /* "string"  */
    DAS_TAUTO = 319,               /* "auto"  */
    DAS_TINT = 320,                /* "int"  */
    DAS_TINT2 = 321,               /* "int2"  */
    DAS_TINT3 = 322,               /* "int3"  */
    DAS_TINT4 = 323,               /* "int4"  */
    DAS_TUINT = 324,               /* "uint"  */
    DAS_TBITFIELD = 325,           /* "bitfield"  */
    DAS_TUINT2 = 326,              /* "uint2"  */
    DAS_TUINT3 = 327,              /* "uint3"  */
    DAS_TUINT4 = 328,              /* "uint4"  */
    DAS_TFLOAT = 329,              /* "float"  */
    DAS_TFLOAT2 = 330,             /* "float2"  */
    DAS_TFLOAT3 = 331,             /* "float3"  */
    DAS_TFLOAT4 = 332,             /* "float4"  */
    DAS_TRANGE = 333,              /* "range"  */
    DAS_TURANGE = 334,             /* "urange"  */
    DAS_TBLOCK = 335,              /* "block"  */
    DAS_TINT64 = 336,              /* "int64"  */
    DAS_TUINT64 = 337,             /* "uint64"  */
    DAS_TDOUBLE = 338,             /* "double"  */
    DAS_TFUNCTION = 339,           /* "function"  */
    DAS_TLAMBDA = 340,             /* "lambda"  */
    DAS_TINT8 = 341,               /* "int8"  */
    DAS_TUINT8 = 342,              /* "uint8"  */
    DAS_TINT16 = 343,              /* "int16"  */
    DAS_TUINT16 = 344,             /* "uint16"  */
    DAS_TTUPLE = 345,              /* "tuple"  */
    DAS_TVARIANT = 346,            /* "variant"  */
    DAS_GENERATOR = 347,           /* "generator"  */
    DAS_YIELD = 348,               /* "yield"  */
    ADDEQU = 349,                  /* "+="  */
    SUBEQU = 350,                  /* "-="  */
    DIVEQU = 351,                  /* "/="  */
    MULEQU = 352,                  /* "*="  */
    MODEQU = 353,                  /* "%="  */
    ANDEQU = 354,                  /* "&="  */
    OREQU = 355,                   /* "|="  */
    XOREQU = 356,                  /* "^="  */
    SHL = 357,                     /* "<<"  */
    SHR = 358,                     /* ">>"  */
    ADDADD = 359,                  /* "++"  */
    SUBSUB = 360,                  /* "--"  */
    LEEQU = 361,                   /* "<="  */
    SHLEQU = 362,                  /* "<<="  */
    SHREQU = 363,                  /* ">>="  */
    GREQU = 364,                   /* ">="  */
    EQUEQU = 365,                  /* "=="  */
    NOTEQU = 366,                  /* "!="  */
    RARROW = 367,                  /* "->"  */
    LARROW = 368,                  /* "<-"  */
    QQ = 369,                      /* "??"  */
    QDOT = 370,                    /* "?."  */
    QBRA = 371,                    /* "?["  */
    LPIPE = 372,                   /* "<|"  */
    LBPIPE = 373,                  /* " <|"  */
    LAPIPE = 374,                  /* "@ <|"  */
    LFPIPE = 375,                  /* "@@ <|"  */
    RPIPE = 376,                   /* "|>"  */
    CLONEEQU = 377,                /* ":="  */
    ROTL = 378,                    /* "<<<"  */
    ROTR = 379,                    /* ">>>"  */
    ROTLEQU = 380,                 /* "<<<="  */
    ROTREQU = 381,                 /* ">>>="  */
    MAPTO = 382,                   /* "=>"  */
    COLCOL = 383,                  /* "::"  */
    ANDAND = 384,                  /* "&&"  */
    OROR = 385,                    /* "||"  */
    XORXOR = 386,                  /* "^^"  */
    ANDANDEQU = 387,               /* "&&="  */
    OROREQU = 388,                 /* "||="  */
    XORXOREQU = 389,               /* "^^="  */
    BRABRAB = 390,                 /* "[["  */
    BRACBRB = 391,                 /* "[{"  */
    CBRCBRB = 392,                 /* "{{"  */
    INTEGER = 393,                 /* "integer constant"  */
    LONG_INTEGER = 394,            /* "long integer constant"  */
    UNSIGNED_INTEGER = 395,        /* "unsigned integer constant"  */
    UNSIGNED_LONG_INTEGER = 396,   /* "unsigned long integer constant"  */
    FLOAT = 397,                   /* "floating point constant"  */
    DOUBLE = 398,                  /* "double constant"  */
    NAME = 399,                    /* "name"  */
    BEGIN_STRING = 400,            /* "start of the string"  */
    STRING_CHARACTER = 401,        /* STRING_CHARACTER  */
    END_STRING = 402,              /* "end of the string"  */
    BEGIN_STRING_EXPR = 403,       /* "{"  */
    END_STRING_EXPR = 404,         /* "}"  */
    END_OF_READ = 405,             /* "end of failed eader macro"  */
    UNARY_MINUS = 406,             /* UNARY_MINUS  */
    UNARY_PLUS = 407,              /* UNARY_PLUS  */
    PRE_INC = 408,                 /* PRE_INC  */
    PRE_DEC = 409,                 /* PRE_DEC  */
    POST_INC = 410,                /* POST_INC  */
    POST_DEC = 411,                /* POST_DEC  */
    DEREF = 412                    /* DEREF  */
  };
  typedef enum das_yytokentype das_yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined DAS_YYSTYPE && ! defined DAS_YYSTYPE_IS_DECLARED
union DAS_YYSTYPE
{
#line 94 "ds_parser.ypp"

    char                            ch;
    bool                            b;
    int32_t                         i;
    uint32_t                        ui;
    int64_t                         i64;
    uint64_t                        ui64;
    double                          d;
    double                          fd;
    string *                        s;
    vector<string> *                pNameList;
    vector<pair<string,LineInfo>> * pNameWithPosList;
    VariableDeclaration *           pVarDecl;
    vector<VariableDeclaration*> *  pVarDeclList;
    TypeDecl *                      pTypeDecl;
    Expression *                    pExpression;
    Type                            type;
    AnnotationArgument *            aa;
    AnnotationArgumentList *        aaList;
    AnnotationDeclaration *         fa;
    AnnotationList *                faList;
    MakeStruct *                    pMakeStruct;
    Enumeration *                   pEnum;
    Structure *                     pStructure;
    Function *                      pFuncDecl;
    CaptureEntry *                  pCapt;
    vector<CaptureEntry> *          pCaptList;

#line 320 "ds_parser.hpp"

};
typedef union DAS_YYSTYPE DAS_YYSTYPE;
# define DAS_YYSTYPE_IS_TRIVIAL 1
# define DAS_YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined DAS_YYLTYPE && ! defined DAS_YYLTYPE_IS_DECLARED
typedef struct DAS_YYLTYPE DAS_YYLTYPE;
struct DAS_YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define DAS_YYLTYPE_IS_DECLARED 1
# define DAS_YYLTYPE_IS_TRIVIAL 1
#endif


extern DAS_YYSTYPE das_yylval;
extern DAS_YYLTYPE das_yylloc;
int das_yyparse (void);

#endif /* !YY_DAS_YY_DS_PARSER_HPP_INCLUDED  */
