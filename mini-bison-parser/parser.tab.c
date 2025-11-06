/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>

class ASTNode {
public:
    std::string type;
    std::string value;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    ASTNode(const std::string& t, const std::string& v = "") : type(t), value(v) {}
    
    void addChild(std::unique_ptr<ASTNode> child) {
        children.push_back(std::move(child));
    }
    
    void print(int indent = 0) const {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << type;
        if (!value.empty()) std::cout << " [" << value << "]";
        std::cout << std::endl;
        
        for (const auto& child : children) {
            child->print(indent + 1);
        }
    }
};

class SymbolTable {
private:
    std::vector<std::string> symbols;
    
public:
    bool isDeclared(const std::string& identifier) {
        for (const auto& var : symbols) {
            if (var == identifier) return true;
        }
        return false;
    }
    
    void declareVariable(const std::string& identifier) {
        if (!isDeclared(identifier)) {
            symbols.push_back(identifier);
        }
    }
    
    void addBuiltins() {
        symbols.insert(symbols.end(), {
            "cout", "cin", "endl", "main", "printf", "scanf", 
            "malloc", "free", "true", "false", "NULL", "nullptr",
            "add", "factorial", "printMessage", "calculate"
        });
    }
};

extern int yylex();
extern void yyerror(const char* s);
extern FILE* yyin;

static std::unique_ptr<ASTNode> root;
static SymbolTable symbolTable;

#line 139 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_STRING_LITERAL = 4,             /* STRING_LITERAL  */
  YYSYMBOL_CHAR_LITERAL = 5,               /* CHAR_LITERAL  */
  YYSYMBOL_INTEGER = 6,                    /* INTEGER  */
  YYSYMBOL_FLOAT = 7,                      /* FLOAT  */
  YYSYMBOL_INT = 8,                        /* INT  */
  YYSYMBOL_FLOAT_TYPE = 9,                 /* FLOAT_TYPE  */
  YYSYMBOL_DOUBLE = 10,                    /* DOUBLE  */
  YYSYMBOL_CHAR = 11,                      /* CHAR  */
  YYSYMBOL_BOOL = 12,                      /* BOOL  */
  YYSYMBOL_VOID = 13,                      /* VOID  */
  YYSYMBOL_STRING = 14,                    /* STRING  */
  YYSYMBOL_IF = 15,                        /* IF  */
  YYSYMBOL_ELSE = 16,                      /* ELSE  */
  YYSYMBOL_WHILE = 17,                     /* WHILE  */
  YYSYMBOL_FOR = 18,                       /* FOR  */
  YYSYMBOL_RETURN = 19,                    /* RETURN  */
  YYSYMBOL_BREAK = 20,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 21,                  /* CONTINUE  */
  YYSYMBOL_COUT = 22,                      /* COUT  */
  YYSYMBOL_CIN = 23,                       /* CIN  */
  YYSYMBOL_ENDL = 24,                      /* ENDL  */
  YYSYMBOL_PLUS = 25,                      /* PLUS  */
  YYSYMBOL_MINUS = 26,                     /* MINUS  */
  YYSYMBOL_MULTIPLY = 27,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 28,                    /* DIVIDE  */
  YYSYMBOL_MODULO = 29,                    /* MODULO  */
  YYSYMBOL_ASSIGN = 30,                    /* ASSIGN  */
  YYSYMBOL_EQ = 31,                        /* EQ  */
  YYSYMBOL_NE = 32,                        /* NE  */
  YYSYMBOL_LT = 33,                        /* LT  */
  YYSYMBOL_LE = 34,                        /* LE  */
  YYSYMBOL_GT = 35,                        /* GT  */
  YYSYMBOL_GE = 36,                        /* GE  */
  YYSYMBOL_AND = 37,                       /* AND  */
  YYSYMBOL_OR = 38,                        /* OR  */
  YYSYMBOL_NOT = 39,                       /* NOT  */
  YYSYMBOL_INCREMENT = 40,                 /* INCREMENT  */
  YYSYMBOL_DECREMENT = 41,                 /* DECREMENT  */
  YYSYMBOL_LPAREN = 42,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 43,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 44,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 45,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 46,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 47,                  /* RBRACKET  */
  YYSYMBOL_SEMICOLON = 48,                 /* SEMICOLON  */
  YYSYMBOL_COMMA = 49,                     /* COMMA  */
  YYSYMBOL_COLON = 50,                     /* COLON  */
  YYSYMBOL_OUTPUT = 51,                    /* OUTPUT  */
  YYSYMBOL_INPUT = 52,                     /* INPUT  */
  YYSYMBOL_TRUE = 53,                      /* TRUE  */
  YYSYMBOL_FALSE = 54,                     /* FALSE  */
  YYSYMBOL_LOWER_THAN_ELSE = 55,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_YYACCEPT = 56,                  /* $accept  */
  YYSYMBOL_program = 57,                   /* program  */
  YYSYMBOL_translation_unit = 58,          /* translation_unit  */
  YYSYMBOL_external_declaration = 59,      /* external_declaration  */
  YYSYMBOL_function_definition = 60,       /* function_definition  */
  YYSYMBOL_parameter_list = 61,            /* parameter_list  */
  YYSYMBOL_parameter_declaration = 62,     /* parameter_declaration  */
  YYSYMBOL_declaration = 63,               /* declaration  */
  YYSYMBOL_init_declarator_list = 64,      /* init_declarator_list  */
  YYSYMBOL_init_declarator = 65,           /* init_declarator  */
  YYSYMBOL_type = 66,                      /* type  */
  YYSYMBOL_compound_statement = 67,        /* compound_statement  */
  YYSYMBOL_statement_list = 68,            /* statement_list  */
  YYSYMBOL_statement = 69,                 /* statement  */
  YYSYMBOL_selection_statement = 70,       /* selection_statement  */
  YYSYMBOL_iteration_statement = 71,       /* iteration_statement  */
  YYSYMBOL_jump_statement = 72,            /* jump_statement  */
  YYSYMBOL_expression = 73,                /* expression  */
  YYSYMBOL_assignment_expression = 74,     /* assignment_expression  */
  YYSYMBOL_logical_or_expression = 75,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 76,    /* logical_and_expression  */
  YYSYMBOL_equality_expression = 77,       /* equality_expression  */
  YYSYMBOL_relational_expression = 78,     /* relational_expression  */
  YYSYMBOL_additive_expression = 79,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 80, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 81,          /* unary_expression  */
  YYSYMBOL_postfix_expression = 82,        /* postfix_expression  */
  YYSYMBOL_primary_expression = 83,        /* primary_expression  */
  YYSYMBOL_argument_expression_list = 84   /* argument_expression_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   297

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  56
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  29
/* YYNRULES -- Number of rules.  */
#define YYNRULES  85
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  151

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   310


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   122,   122,   129,   133,   143,   147,   154,   164,   176,
     182,   192,   203,   213,   219,   229,   236,   247,   248,   249,
     250,   251,   252,   253,   257,   263,   270,   276,   286,   290,
     294,   298,   302,   306,   310,   316,   323,   330,   337,   348,
     355,   367,   373,   377,   381,   388,   395,   399,   413,   414,
     424,   425,   435,   436,   443,   453,   454,   461,   468,   475,
     485,   486,   493,   503,   504,   511,   518,   528,   529,   535,
     541,   550,   551,   557,   563,   571,   581,   591,   596,   601,
     607,   613,   617,   621,   630,   636
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER",
  "STRING_LITERAL", "CHAR_LITERAL", "INTEGER", "FLOAT", "INT",
  "FLOAT_TYPE", "DOUBLE", "CHAR", "BOOL", "VOID", "STRING", "IF", "ELSE",
  "WHILE", "FOR", "RETURN", "BREAK", "CONTINUE", "COUT", "CIN", "ENDL",
  "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "MODULO", "ASSIGN", "EQ", "NE",
  "LT", "LE", "GT", "GE", "AND", "OR", "NOT", "INCREMENT", "DECREMENT",
  "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACKET", "RBRACKET",
  "SEMICOLON", "COMMA", "COLON", "OUTPUT", "INPUT", "TRUE", "FALSE",
  "LOWER_THAN_ELSE", "$accept", "program", "translation_unit",
  "external_declaration", "function_definition", "parameter_list",
  "parameter_declaration", "declaration", "init_declarator_list",
  "init_declarator", "type", "compound_statement", "statement_list",
  "statement", "selection_statement", "iteration_statement",
  "jump_statement", "expression", "assignment_expression",
  "logical_or_expression", "logical_and_expression", "equality_expression",
  "relational_expression", "additive_expression",
  "multiplicative_expression", "unary_expression", "postfix_expression",
  "primary_expression", "argument_expression_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-50)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     146,   -50,   -50,   -50,   -50,   -50,   -50,   -50,    46,   146,
     -50,   -50,   -50,    54,   -50,   -50,   -26,   -29,   -50,    49,
     254,   -50,    68,   -24,   -50,   -50,   -50,   -50,   207,   207,
     207,    49,   -50,   -50,   -50,   -50,    24,    39,    11,    59,
      34,    87,   -50,    25,   -50,    38,   -36,   -50,    81,    55,
     -50,    49,    44,    62,   -50,   -50,   -50,    53,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   -50,   -50,    19,   -50,    38,   146,   -50,   -50,   -50,
     -50,   -34,   -50,    39,    11,    59,    59,    34,    34,    34,
      34,    87,    87,   -50,   -50,   -50,    63,    65,    77,   200,
      73,    75,    74,   -50,   -50,   -50,    68,   -50,   124,   -50,
     -50,   -50,   -50,    76,   -50,   -50,   -50,    49,    49,    49,
      49,   -50,    92,   -50,   -50,    49,   -50,   -50,   -50,   -50,
      67,   104,   100,   -50,   -37,   176,   176,    49,   -50,    49,
     145,   -50,   114,   116,   176,    49,   -50,   -50,   122,   176,
     -50
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    17,    18,    19,    20,    21,    22,    23,     0,     2,
       3,     5,     6,     0,     1,     4,    15,     0,    13,     0,
       0,    12,     0,    76,    79,    80,    77,    78,     0,     0,
       0,     0,    81,    82,    16,    45,    46,    48,    50,    52,
      55,    60,    63,    67,    71,     0,     0,     9,     0,    15,
      14,     0,     0,    76,    68,    69,    70,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    72,    73,     0,     8,     0,     0,    11,    47,    75,
      84,     0,    83,    49,    51,    53,    54,    56,    57,    58,
      59,    61,    62,    64,    65,    66,     0,     0,     0,     0,
       0,     0,     0,    25,    36,    33,     0,    29,     0,    26,
      30,    31,    32,     0,     7,    10,    74,     0,     0,     0,
       0,    42,     0,    43,    44,     0,    24,    27,    28,    85,
       0,     0,     0,    41,     0,     0,     0,     0,    34,     0,
      37,    39,     0,     0,     0,     0,    35,    38,     0,     0,
      40
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -50,   -50,   -50,   158,   -50,   -50,    94,     8,   -50,   149,
       1,   -40,   -50,   -27,   -50,   -50,   -50,   -19,   -49,   -50,
     134,   140,    18,   111,    23,    83,   -50,   -50,   -50
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     8,     9,    10,    11,    46,    47,   105,    17,    18,
     106,   107,   108,   109,   110,   111,   112,   113,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    81
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      34,    13,    78,    80,    19,    74,    51,    75,    12,   116,
      13,   138,    57,    76,   139,   117,    20,    12,    52,    21,
      22,    48,    23,    24,    25,    26,    27,     1,     2,     3,
       4,     5,     6,     7,    96,   114,    97,    98,    99,   100,
     101,   102,    60,    61,    28,    29,    14,    23,    24,    25,
      26,    27,    23,    24,    25,    26,    27,    16,    30,    66,
      67,    31,    58,    73,   103,    71,    72,   104,   129,    28,
      29,    49,    32,    33,    28,    29,    59,    48,    85,    86,
     122,   127,    73,    30,    77,    19,    31,    79,    30,    91,
      92,    31,    62,    63,    64,    65,    82,    32,    33,   130,
     131,   132,    32,    33,    52,   118,   134,   119,   140,   141,
     135,    54,    55,    56,    68,    69,    70,   147,   142,   120,
     143,   123,   150,   124,   128,   125,   148,    23,    24,    25,
      26,    27,     1,     2,     3,     4,     5,     6,     7,    96,
     133,    97,    98,    99,   100,   101,   102,   136,   137,    28,
      29,    93,    94,    95,     1,     2,     3,     4,     5,     6,
       7,   144,   145,    30,   146,   149,    31,    15,    73,   126,
     115,    50,   104,    87,    88,    89,    90,    32,    33,    23,
      24,    25,    26,    27,     1,     2,     3,     4,     5,     6,
       7,    96,    83,    97,    98,    99,   100,   101,   102,    84,
       0,    28,    29,    23,    24,    25,    26,    27,     0,     0,
      53,    24,    25,    26,    27,    30,     0,     0,    31,     0,
      73,     0,     0,     0,   104,    28,    29,     0,     0,    32,
      33,     0,    28,    29,     0,     0,     0,     0,     0,    30,
       0,     0,    31,     0,     0,     0,    30,     0,   121,    31,
       0,     0,     0,    32,    33,     0,     0,     0,     0,     0,
      32,    33,     1,     2,     3,     4,     5,     6,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    45
};

static const yytype_int16 yycheck[] =
{
      19,     0,    51,    52,    30,    45,    30,    43,     0,    43,
       9,    48,    31,    49,    51,    49,    42,     9,    42,    48,
      49,    20,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    75,    17,    18,    19,    20,
      21,    22,    31,    32,    25,    26,     0,     3,     4,     5,
       6,     7,     3,     4,     5,     6,     7,     3,    39,    25,
      26,    42,    38,    44,    45,    40,    41,    48,   117,    25,
      26,     3,    53,    54,    25,    26,    37,    76,    60,    61,
      99,   108,    44,    39,     3,    30,    42,    43,    39,    66,
      67,    42,    33,    34,    35,    36,    43,    53,    54,   118,
     119,   120,    53,    54,    42,    42,   125,    42,   135,   136,
      43,    28,    29,    30,    27,    28,    29,   144,   137,    42,
     139,    48,   149,    48,    48,    51,   145,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      48,    17,    18,    19,    20,    21,    22,    43,    48,    25,
      26,    68,    69,    70,     8,     9,    10,    11,    12,    13,
      14,    16,    48,    39,    48,    43,    42,     9,    44,    45,
      76,    22,    48,    62,    63,    64,    65,    53,    54,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    58,    17,    18,    19,    20,    21,    22,    59,
      -1,    25,    26,     3,     4,     5,     6,     7,    -1,    -1,
       3,     4,     5,     6,     7,    39,    -1,    -1,    42,    -1,
      44,    -1,    -1,    -1,    48,    25,    26,    -1,    -1,    53,
      54,    -1,    25,    26,    -1,    -1,    -1,    -1,    -1,    39,
      -1,    -1,    42,    -1,    -1,    -1,    39,    -1,    48,    42,
      -1,    -1,    -1,    53,    54,    -1,    -1,    -1,    -1,    -1,
      53,    54,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     8,     9,    10,    11,    12,    13,    14,    57,    58,
      59,    60,    63,    66,     0,    59,     3,    64,    65,    30,
      42,    48,    49,     3,     4,     5,     6,     7,    25,    26,
      39,    42,    53,    54,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    43,    61,    62,    66,     3,
      65,    30,    42,     3,    81,    81,    81,    73,    38,    37,
      31,    32,    33,    34,    35,    36,    25,    26,    27,    28,
      29,    40,    41,    44,    67,    43,    49,     3,    74,    43,
      74,    84,    43,    76,    77,    78,    78,    79,    79,    79,
      79,    80,    80,    81,    81,    81,    15,    17,    18,    19,
      20,    21,    22,    45,    48,    63,    66,    67,    68,    69,
      70,    71,    72,    73,    67,    62,    43,    49,    42,    42,
      42,    48,    73,    48,    48,    51,    45,    69,    48,    74,
      73,    73,    73,    48,    73,    43,    43,    48,    48,    51,
      69,    69,    73,    73,    16,    48,    48,    69,    73,    43,
      69
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    56,    57,    58,    58,    59,    59,    60,    60,    61,
      61,    62,    63,    64,    64,    65,    65,    66,    66,    66,
      66,    66,    66,    66,    67,    67,    68,    68,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    70,    70,    71,
      71,    72,    72,    72,    72,    73,    74,    74,    75,    75,
      76,    76,    77,    77,    77,    78,    78,    78,    78,    78,
      79,    79,    79,    80,    80,    80,    80,    81,    81,    81,
      81,    82,    82,    82,    82,    82,    83,    83,    83,    83,
      83,    83,    83,    83,    84,    84
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     6,     5,     1,
       3,     2,     3,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     3,     2,     1,     2,     2,     1,
       1,     1,     1,     1,     4,     6,     1,     5,     7,     5,
       9,     3,     2,     2,     2,     1,     1,     3,     1,     3,
       1,     3,     1,     3,     3,     1,     3,     3,     3,     3,
       1,     3,     3,     1,     3,     3,     3,     1,     2,     2,
       2,     1,     2,     2,     4,     3,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: translation_unit  */
#line 123 "parser.y"
    {
        root = std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val)));
    }
#line 1351 "parser.tab.c"
    break;

  case 3: /* translation_unit: external_declaration  */
#line 130 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1359 "parser.tab.c"
    break;

  case 4: /* translation_unit: translation_unit external_declaration  */
#line 134 "parser.y"
    {
        ASTNode* left = static_cast<ASTNode*>((yyvsp[-1].node_val));
        ASTNode* right = static_cast<ASTNode*>((yyvsp[0].node_val));
        left->addChild(std::unique_ptr<ASTNode>(right));
        (yyval.node_val) = (yyvsp[-1].node_val);
    }
#line 1370 "parser.tab.c"
    break;

  case 5: /* external_declaration: function_definition  */
#line 144 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1378 "parser.tab.c"
    break;

  case 6: /* external_declaration: declaration  */
#line 148 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1386 "parser.tab.c"
    break;

  case 7: /* function_definition: type IDENTIFIER LPAREN parameter_list RPAREN compound_statement  */
#line 155 "parser.y"
    {
        auto node = new ASTNode("Function", (yyvsp[-4].string_val));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-5].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        symbolTable.declareVariable((yyvsp[-4].string_val));
        free((yyvsp[-4].string_val));
        (yyval.node_val) = node;
    }
#line 1400 "parser.tab.c"
    break;

  case 8: /* function_definition: type IDENTIFIER LPAREN RPAREN compound_statement  */
#line 165 "parser.y"
    {
        auto node = new ASTNode("Function", (yyvsp[-3].string_val));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-4].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        symbolTable.declareVariable((yyvsp[-3].string_val));
        free((yyvsp[-3].string_val));
        (yyval.node_val) = node;
    }
#line 1413 "parser.tab.c"
    break;

  case 9: /* parameter_list: parameter_declaration  */
#line 177 "parser.y"
    {
        auto node = new ASTNode("ParameterList");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1423 "parser.tab.c"
    break;

  case 10: /* parameter_list: parameter_list COMMA parameter_declaration  */
#line 183 "parser.y"
    {
        ASTNode* list = static_cast<ASTNode*>((yyvsp[-2].node_val));
        ASTNode* param = static_cast<ASTNode*>((yyvsp[0].node_val));
        list->addChild(std::unique_ptr<ASTNode>(param));
        (yyval.node_val) = (yyvsp[-2].node_val);
    }
#line 1434 "parser.tab.c"
    break;

  case 11: /* parameter_declaration: type IDENTIFIER  */
#line 193 "parser.y"
    {
        auto node = new ASTNode("Parameter", (yyvsp[0].string_val));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        symbolTable.declareVariable((yyvsp[0].string_val));
        free((yyvsp[0].string_val));
        (yyval.node_val) = node;
    }
#line 1446 "parser.tab.c"
    break;

  case 12: /* declaration: type init_declarator_list SEMICOLON  */
#line 204 "parser.y"
    {
        auto node = new ASTNode("Declaration");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        (yyval.node_val) = node;
    }
#line 1457 "parser.tab.c"
    break;

  case 13: /* init_declarator_list: init_declarator  */
#line 214 "parser.y"
    {
        auto node = new ASTNode("InitList");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1467 "parser.tab.c"
    break;

  case 14: /* init_declarator_list: init_declarator_list COMMA init_declarator  */
#line 220 "parser.y"
    {
        ASTNode* list = static_cast<ASTNode*>((yyvsp[-2].node_val));
        ASTNode* init = static_cast<ASTNode*>((yyvsp[0].node_val));
        list->addChild(std::unique_ptr<ASTNode>(init));
        (yyval.node_val) = (yyvsp[-2].node_val);
    }
#line 1478 "parser.tab.c"
    break;

  case 15: /* init_declarator: IDENTIFIER  */
#line 230 "parser.y"
    {
        auto node = new ASTNode("Variable", (yyvsp[0].string_val));
        symbolTable.declareVariable((yyvsp[0].string_val));
        free((yyvsp[0].string_val));
        (yyval.node_val) = node;
    }
#line 1489 "parser.tab.c"
    break;

  case 16: /* init_declarator: IDENTIFIER ASSIGN expression  */
#line 237 "parser.y"
    {
        auto node = new ASTNode("InitializedVar", (yyvsp[-2].string_val));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        symbolTable.declareVariable((yyvsp[-2].string_val));
        free((yyvsp[-2].string_val));
        (yyval.node_val) = node;
    }
#line 1501 "parser.tab.c"
    break;

  case 17: /* type: INT  */
#line 247 "parser.y"
        { (yyval.node_val) = new ASTNode("Type", "int"); }
#line 1507 "parser.tab.c"
    break;

  case 18: /* type: FLOAT_TYPE  */
#line 248 "parser.y"
                 { (yyval.node_val) = new ASTNode("Type", "float"); }
#line 1513 "parser.tab.c"
    break;

  case 19: /* type: DOUBLE  */
#line 249 "parser.y"
             { (yyval.node_val) = new ASTNode("Type", "double"); }
#line 1519 "parser.tab.c"
    break;

  case 20: /* type: CHAR  */
#line 250 "parser.y"
           { (yyval.node_val) = new ASTNode("Type", "char"); }
#line 1525 "parser.tab.c"
    break;

  case 21: /* type: BOOL  */
#line 251 "parser.y"
           { (yyval.node_val) = new ASTNode("Type", "bool"); }
#line 1531 "parser.tab.c"
    break;

  case 22: /* type: VOID  */
#line 252 "parser.y"
           { (yyval.node_val) = new ASTNode("Type", "void"); }
#line 1537 "parser.tab.c"
    break;

  case 23: /* type: STRING  */
#line 253 "parser.y"
             { (yyval.node_val) = new ASTNode("Type", "string"); }
#line 1543 "parser.tab.c"
    break;

  case 24: /* compound_statement: LBRACE statement_list RBRACE  */
#line 258 "parser.y"
    {
        auto node = new ASTNode("Block");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        (yyval.node_val) = node;
    }
#line 1553 "parser.tab.c"
    break;

  case 25: /* compound_statement: LBRACE RBRACE  */
#line 264 "parser.y"
    {
        (yyval.node_val) = new ASTNode("EmptyBlock");
    }
#line 1561 "parser.tab.c"
    break;

  case 26: /* statement_list: statement  */
#line 271 "parser.y"
    {
        auto node = new ASTNode("StatementList");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1571 "parser.tab.c"
    break;

  case 27: /* statement_list: statement_list statement  */
#line 277 "parser.y"
    {
        ASTNode* list = static_cast<ASTNode*>((yyvsp[-1].node_val));
        ASTNode* stmt = static_cast<ASTNode*>((yyvsp[0].node_val));
        list->addChild(std::unique_ptr<ASTNode>(stmt));
        (yyval.node_val) = (yyvsp[-1].node_val);
    }
#line 1582 "parser.tab.c"
    break;

  case 28: /* statement: expression SEMICOLON  */
#line 287 "parser.y"
    {
        (yyval.node_val) = (yyvsp[-1].node_val);
    }
#line 1590 "parser.tab.c"
    break;

  case 29: /* statement: compound_statement  */
#line 291 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1598 "parser.tab.c"
    break;

  case 30: /* statement: selection_statement  */
#line 295 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1606 "parser.tab.c"
    break;

  case 31: /* statement: iteration_statement  */
#line 299 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1614 "parser.tab.c"
    break;

  case 32: /* statement: jump_statement  */
#line 303 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1622 "parser.tab.c"
    break;

  case 33: /* statement: declaration  */
#line 307 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1630 "parser.tab.c"
    break;

  case 34: /* statement: COUT OUTPUT expression SEMICOLON  */
#line 311 "parser.y"
    {
        auto node = new ASTNode("Output");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        (yyval.node_val) = node;
    }
#line 1640 "parser.tab.c"
    break;

  case 35: /* statement: COUT OUTPUT expression OUTPUT expression SEMICOLON  */
#line 317 "parser.y"
    {
        auto node = new ASTNode("OutputChain");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-3].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        (yyval.node_val) = node;
    }
#line 1651 "parser.tab.c"
    break;

  case 36: /* statement: SEMICOLON  */
#line 324 "parser.y"
    {
        (yyval.node_val) = new ASTNode("EmptyStatement");
    }
#line 1659 "parser.tab.c"
    break;

  case 37: /* selection_statement: IF LPAREN expression RPAREN statement  */
#line 331 "parser.y"
    {
        auto node = new ASTNode("IfStatement");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1670 "parser.tab.c"
    break;

  case 38: /* selection_statement: IF LPAREN expression RPAREN statement ELSE statement  */
#line 338 "parser.y"
    {
        auto node = new ASTNode("IfElseStatement");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-4].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1682 "parser.tab.c"
    break;

  case 39: /* iteration_statement: WHILE LPAREN expression RPAREN statement  */
#line 349 "parser.y"
    {
        auto node = new ASTNode("WhileLoop");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1693 "parser.tab.c"
    break;

  case 40: /* iteration_statement: FOR LPAREN expression SEMICOLON expression SEMICOLON expression RPAREN statement  */
#line 356 "parser.y"
    {
        auto node = new ASTNode("ForLoop");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-6].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-4].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1706 "parser.tab.c"
    break;

  case 41: /* jump_statement: RETURN expression SEMICOLON  */
#line 368 "parser.y"
    {
        auto node = new ASTNode("Return");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        (yyval.node_val) = node;
    }
#line 1716 "parser.tab.c"
    break;

  case 42: /* jump_statement: RETURN SEMICOLON  */
#line 374 "parser.y"
    {
        (yyval.node_val) = new ASTNode("ReturnVoid");
    }
#line 1724 "parser.tab.c"
    break;

  case 43: /* jump_statement: BREAK SEMICOLON  */
#line 378 "parser.y"
    {
        (yyval.node_val) = new ASTNode("Break");
    }
#line 1732 "parser.tab.c"
    break;

  case 44: /* jump_statement: CONTINUE SEMICOLON  */
#line 382 "parser.y"
    {
        (yyval.node_val) = new ASTNode("Continue");
    }
#line 1740 "parser.tab.c"
    break;

  case 45: /* expression: assignment_expression  */
#line 389 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1748 "parser.tab.c"
    break;

  case 46: /* assignment_expression: logical_or_expression  */
#line 396 "parser.y"
    {
        (yyval.node_val) = (yyvsp[0].node_val);
    }
#line 1756 "parser.tab.c"
    break;

  case 47: /* assignment_expression: IDENTIFIER ASSIGN assignment_expression  */
#line 400 "parser.y"
    {
        if (!symbolTable.isDeclared((yyvsp[-2].string_val))) {
            yyerror("Undeclared variable");
            YYERROR;
        }
        auto node = new ASTNode("Assignment", (yyvsp[-2].string_val));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        free((yyvsp[-2].string_val));
        (yyval.node_val) = node;
    }
#line 1771 "parser.tab.c"
    break;

  case 48: /* logical_or_expression: logical_and_expression  */
#line 413 "parser.y"
                           { (yyval.node_val) = (yyvsp[0].node_val); }
#line 1777 "parser.tab.c"
    break;

  case 49: /* logical_or_expression: logical_or_expression OR logical_and_expression  */
#line 415 "parser.y"
    {
        auto node = new ASTNode("LogicalOR");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1788 "parser.tab.c"
    break;

  case 50: /* logical_and_expression: equality_expression  */
#line 424 "parser.y"
                        { (yyval.node_val) = (yyvsp[0].node_val); }
#line 1794 "parser.tab.c"
    break;

  case 51: /* logical_and_expression: logical_and_expression AND equality_expression  */
#line 426 "parser.y"
    {
        auto node = new ASTNode("LogicalAND");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1805 "parser.tab.c"
    break;

  case 52: /* equality_expression: relational_expression  */
#line 435 "parser.y"
                          { (yyval.node_val) = (yyvsp[0].node_val); }
#line 1811 "parser.tab.c"
    break;

  case 53: /* equality_expression: equality_expression EQ relational_expression  */
#line 437 "parser.y"
    {
        auto node = new ASTNode("Equal");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1822 "parser.tab.c"
    break;

  case 54: /* equality_expression: equality_expression NE relational_expression  */
#line 444 "parser.y"
    {
        auto node = new ASTNode("NotEqual");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1833 "parser.tab.c"
    break;

  case 55: /* relational_expression: additive_expression  */
#line 453 "parser.y"
                        { (yyval.node_val) = (yyvsp[0].node_val); }
#line 1839 "parser.tab.c"
    break;

  case 56: /* relational_expression: relational_expression LT additive_expression  */
#line 455 "parser.y"
    {
        auto node = new ASTNode("LessThan");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1850 "parser.tab.c"
    break;

  case 57: /* relational_expression: relational_expression LE additive_expression  */
#line 462 "parser.y"
    {
        auto node = new ASTNode("LessEqual");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1861 "parser.tab.c"
    break;

  case 58: /* relational_expression: relational_expression GT additive_expression  */
#line 469 "parser.y"
    {
        auto node = new ASTNode("GreaterThan");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1872 "parser.tab.c"
    break;

  case 59: /* relational_expression: relational_expression GE additive_expression  */
#line 476 "parser.y"
    {
        auto node = new ASTNode("GreaterEqual");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1883 "parser.tab.c"
    break;

  case 60: /* additive_expression: multiplicative_expression  */
#line 485 "parser.y"
                              { (yyval.node_val) = (yyvsp[0].node_val); }
#line 1889 "parser.tab.c"
    break;

  case 61: /* additive_expression: additive_expression PLUS multiplicative_expression  */
#line 487 "parser.y"
    {
        auto node = new ASTNode("Add");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1900 "parser.tab.c"
    break;

  case 62: /* additive_expression: additive_expression MINUS multiplicative_expression  */
#line 494 "parser.y"
    {
        auto node = new ASTNode("Subtract");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1911 "parser.tab.c"
    break;

  case 63: /* multiplicative_expression: unary_expression  */
#line 503 "parser.y"
                     { (yyval.node_val) = (yyvsp[0].node_val); }
#line 1917 "parser.tab.c"
    break;

  case 64: /* multiplicative_expression: multiplicative_expression MULTIPLY unary_expression  */
#line 505 "parser.y"
    {
        auto node = new ASTNode("Multiply");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1928 "parser.tab.c"
    break;

  case 65: /* multiplicative_expression: multiplicative_expression DIVIDE unary_expression  */
#line 512 "parser.y"
    {
        auto node = new ASTNode("Divide");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1939 "parser.tab.c"
    break;

  case 66: /* multiplicative_expression: multiplicative_expression MODULO unary_expression  */
#line 519 "parser.y"
    {
        auto node = new ASTNode("Modulo");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-2].node_val))));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1950 "parser.tab.c"
    break;

  case 67: /* unary_expression: postfix_expression  */
#line 528 "parser.y"
                       { (yyval.node_val) = (yyvsp[0].node_val); }
#line 1956 "parser.tab.c"
    break;

  case 68: /* unary_expression: PLUS unary_expression  */
#line 530 "parser.y"
    {
        auto node = new ASTNode("UnaryPlus");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1966 "parser.tab.c"
    break;

  case 69: /* unary_expression: MINUS unary_expression  */
#line 536 "parser.y"
    {
        auto node = new ASTNode("UnaryMinus");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1976 "parser.tab.c"
    break;

  case 70: /* unary_expression: NOT unary_expression  */
#line 542 "parser.y"
    {
        auto node = new ASTNode("LogicalNOT");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 1986 "parser.tab.c"
    break;

  case 71: /* postfix_expression: primary_expression  */
#line 550 "parser.y"
                       { (yyval.node_val) = (yyvsp[0].node_val); }
#line 1992 "parser.tab.c"
    break;

  case 72: /* postfix_expression: postfix_expression INCREMENT  */
#line 552 "parser.y"
    {
        auto node = new ASTNode("PostfixIncrement");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        (yyval.node_val) = node;
    }
#line 2002 "parser.tab.c"
    break;

  case 73: /* postfix_expression: postfix_expression DECREMENT  */
#line 558 "parser.y"
    {
        auto node = new ASTNode("PostfixDecrement");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        (yyval.node_val) = node;
    }
#line 2012 "parser.tab.c"
    break;

  case 74: /* postfix_expression: IDENTIFIER LPAREN argument_expression_list RPAREN  */
#line 564 "parser.y"
    {
        auto node = new ASTNode("FunctionCall", (yyvsp[-3].string_val));
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        symbolTable.declareVariable((yyvsp[-3].string_val));
        free((yyvsp[-3].string_val));
        (yyval.node_val) = node;
    }
#line 2024 "parser.tab.c"
    break;

  case 75: /* postfix_expression: IDENTIFIER LPAREN RPAREN  */
#line 572 "parser.y"
    {
        auto node = new ASTNode("FunctionCall", (yyvsp[-2].string_val));
        symbolTable.declareVariable((yyvsp[-2].string_val));
        free((yyvsp[-2].string_val));
        (yyval.node_val) = node;
    }
#line 2035 "parser.tab.c"
    break;

  case 76: /* primary_expression: IDENTIFIER  */
#line 582 "parser.y"
    {
        if (!symbolTable.isDeclared((yyvsp[0].string_val))) {
            yyerror("Undeclared variable");
            YYERROR;
        }
        auto node = new ASTNode("Variable", (yyvsp[0].string_val));
        free((yyvsp[0].string_val));
        (yyval.node_val) = node;
    }
#line 2049 "parser.tab.c"
    break;

  case 77: /* primary_expression: INTEGER  */
#line 592 "parser.y"
    {
        auto node = new ASTNode("Integer", std::to_string((yyvsp[0].int_val)));
        (yyval.node_val) = node;
    }
#line 2058 "parser.tab.c"
    break;

  case 78: /* primary_expression: FLOAT  */
#line 597 "parser.y"
    {
        auto node = new ASTNode("Float", std::to_string((yyvsp[0].double_val)));
        (yyval.node_val) = node;
    }
#line 2067 "parser.tab.c"
    break;

  case 79: /* primary_expression: STRING_LITERAL  */
#line 602 "parser.y"
    {
        auto node = new ASTNode("String", (yyvsp[0].string_val));
        free((yyvsp[0].string_val));
        (yyval.node_val) = node;
    }
#line 2077 "parser.tab.c"
    break;

  case 80: /* primary_expression: CHAR_LITERAL  */
#line 608 "parser.y"
    {
        auto node = new ASTNode("Char", (yyvsp[0].string_val));
        free((yyvsp[0].string_val));
        (yyval.node_val) = node;
    }
#line 2087 "parser.tab.c"
    break;

  case 81: /* primary_expression: TRUE  */
#line 614 "parser.y"
    {
        (yyval.node_val) = new ASTNode("Boolean", "true");
    }
#line 2095 "parser.tab.c"
    break;

  case 82: /* primary_expression: FALSE  */
#line 618 "parser.y"
    {
        (yyval.node_val) = new ASTNode("Boolean", "false");
    }
#line 2103 "parser.tab.c"
    break;

  case 83: /* primary_expression: LPAREN expression RPAREN  */
#line 622 "parser.y"
    {
        auto node = new ASTNode("Parenthesized");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[-1].node_val))));
        (yyval.node_val) = node;
    }
#line 2113 "parser.tab.c"
    break;

  case 84: /* argument_expression_list: assignment_expression  */
#line 631 "parser.y"
    {
        auto node = new ASTNode("ArgumentList");
        node->addChild(std::unique_ptr<ASTNode>(static_cast<ASTNode*>((yyvsp[0].node_val))));
        (yyval.node_val) = node;
    }
#line 2123 "parser.tab.c"
    break;

  case 85: /* argument_expression_list: argument_expression_list COMMA assignment_expression  */
#line 637 "parser.y"
    {
        ASTNode* list = static_cast<ASTNode*>((yyvsp[-2].node_val));
        ASTNode* arg = static_cast<ASTNode*>((yyvsp[0].node_val));
        list->addChild(std::unique_ptr<ASTNode>(arg));
        (yyval.node_val) = (yyvsp[-2].node_val);
    }
#line 2134 "parser.tab.c"
    break;


#line 2138 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 645 "parser.y"


void yyerror(const char* s) {
    fprintf(stderr, "Parser Error: %s\n", s);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Cannot open file: %s\n", argv[1]);
            return 1;
        }
    }
    
    std::cout << "C++ Parser with AST Generation" << std::endl;
    symbolTable.addBuiltins();
    
    int result = yyparse();
    
    if (yyin != stdin) {
        fclose(yyin);
    }
    
    if (result == 0 && root) {
        std::cout << "\n=== ABSTRACT SYNTAX TREE ===" << std::endl;
        root->print();
        std::cout << "============================\n" << std::endl;
    }
    
    return result;
}
