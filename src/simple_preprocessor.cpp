/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"

#define assert(EXP) if (!(exp)) { *(void *)0 = 0; }

static char*
read_entire_file_and_null_terminate(char *file_name)
{
    char *entire_file = 0;
    FILE *file = fopen(file_name, "r");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        entire_file = (char *)malloc(file_size + 1);
        fread(entire_file, file_size, 1, file);
        entire_file[file_size] = 0;
    }
    else
    {
        fprintf(stderr, "ERROR: Couldn't open file %s", file_name);
    }

    return entire_file;
}

enum Token_Type
{
    eToken_Unknown,

    eToken_Colon,
    eToken_Semicolon,
    eToken_Asterisk,
    eToken_Open_Paren,
    eToken_Close_Paren,
    eToken_Open_Bracket,
    eToken_Close_Bracket,
    eToken_Open_Brace,
    eToken_Close_Brace,

    eToken_String,
    eToken_Identifier,

    eToken_End_Of_Stream,
};

struct Token
{
    Token_Type type;
    size_t text_length;
    char *text;
};

struct Tokenizer
{
    char *at;
};

inline bool
is_end_of_line(char c)
{
    bool result = ((c == '\n') ||
                   (c == 'r'));
    return result;
}

inline bool
is_whitespace(char c)
{
    bool result = (is_end_of_line(c) ||
                   (c == ' ')  ||
                   (c == '\t') ||
                   (c == '\v') ||
                   (c == '\f'));
    return result;
}

inline bool
is_alphabet(char c)
{
    bool result = (((c >= 'a') && (c <= 'z')) ||
                   ((c >= 'A') && (c <= 'Z')));
    return result;
}

inline bool
is_number(char c)
{
    bool result = ((c >= '0') && (c <= '9'));
    return result;
}

static void
eat_all_whitespaces(Tokenizer *tokenizer)
{
    for (;;)
    {
        if (is_whitespace(tokenizer->at[0]))
        {
            ++tokenizer->at;
        }
        else if ((tokenizer->at[0] == '/') && 
                 (tokenizer->at[1] == '/'))
        {
            tokenizer->at += 2;
            while ((tokenizer->at[0]) &&
                   !is_end_of_line(tokenizer->at[0]))
            {
                ++tokenizer->at;
            }
        }
        else if ((tokenizer->at[0] == '/') &&
                 (tokenizer->at[1] == '*'))
        {
            tokenizer->at += 2;
            while (tokenizer->at[0] &&
                   !((tokenizer->at[0] == '*') &&
                     (tokenizer->at[1] == '/')))
            {
                ++tokenizer->at;
            }

            if (tokenizer->at[0] == '*')
            {
                tokenizer->at += 2;
            }
        }
        else
        {
            break;
        }
    }
}

static Token
get_token(Tokenizer *tokenizer)
{
    eat_all_whitespaces(tokenizer);

    Token token = {};
    token.text_length = 1;
    token.text = tokenizer->at;

    char c = tokenizer->at[0];
    ++tokenizer->at;

    switch(c)
    {
        case '\0': { token.type = eToken_End_Of_Stream; } break;

        case '(': { token.type = eToken_Open_Paren; } break;
        case ')': { token.type = eToken_Close_Paren; } break;
        case '[': { token.type = eToken_Open_Bracket; } break;
        case ']': { token.type = eToken_Close_Bracket; } break;
        case '{': { token.type = eToken_Open_Brace; } break;
        case '}': { token.type = eToken_Close_Brace; } break;
        case ':': { token.type = eToken_Colon; } break;
        case ';': { token.type = eToken_Semicolon; } break;
        case '*': { token.type = eToken_Asterisk; } break;

        case '"': 
        { 
            token.type = eToken_String;
            token.text = tokenizer->at;

            while (tokenizer->at[0] &&
                   tokenizer->at[0] != '"')
            {
                if (tokenizer->at[0] == '\\' &&
                    tokenizer->at[1])
                {
                    ++tokenizer->at;
                }
                ++tokenizer->at;
            }

            token.text_length = tokenizer->at - token.text;
            if (tokenizer->at[0] == '"')
            {
                ++tokenizer->at;
            }
        } break;

        default:
        {
            if (is_alphabet(c))
            {
                token.type = eToken_Identifier;

                while (is_alphabet(tokenizer->at[0]) ||
                       is_number(tokenizer->at[0]) ||
                       (tokenizer->at[0] == '_'))
                {
                    ++tokenizer->at;
                }

                token.text_length = tokenizer->at - token.text;
            }
#if 0
            else if (is_number(c))
            {
                parse_number();
            }
#endif
            else
            {
                token.type = eToken_Unknown;
            }
        } break;
    }

    return token;
}

inline bool
token_equals(Token token, char *match)
{
    char *at = match;
    for (int idx = 0;
         idx < token.text_length;
         ++idx, ++at)
    {
        if ((*at == 0) ||
            (token.text[idx] != *at))
        {
            return false;
        }
    }

    bool result = (*at == 0);
    return result;
}

static bool
require_token(Tokenizer *tokenizer, Token_Type desired_type)
{
    Token token = get_token(tokenizer);
    bool result = (token.type == desired_type);
    return result;
}

static void
parse_introspection_params(Tokenizer *tokenizer)
{
    for (;;)
    {
        Token token = get_token(tokenizer);
        if ((token.type == eToken_Close_Paren) || 
            (token.type == eToken_End_Of_Stream))
        {
            break;
        }
    }
}

static void
parse_member(Tokenizer *tokenizer, Token struct_type_token, Token member_type_token)
{
    bool parsing = true;
    bool is_pointer = false;
    while (parsing)
    {
        Token token = get_token(tokenizer);
        switch (token.type)
        {
            case eToken_Asterisk:
            {
                is_pointer = true;
            } break;
            
            case eToken_Identifier:
            {
                printf("    {eMeta_Type_%.*s, \"%.*s\", (u32)&(((%.*s *)0)->%.*s)},\n",
                       member_type_token.text_length, member_type_token.text,
                       token.text_length, token.text,
                       struct_type_token.text_length, struct_type_token.text,
                       token.text_length, token.text);
            } break;

            case eToken_Semicolon:
            case eToken_End_Of_Stream:
            {
                parsing = false;
            } break;
        }
    }
}

struct Meta_Struct
{
    char *name;
    Meta_Struct *next;
};
static Meta_Struct *first_meta_struct;

static void
parse_struct(Tokenizer *tokenizer)
{
    Token name_token = get_token(tokenizer);
    if (require_token(tokenizer, eToken_Open_Brace))
    {
        printf("Member_Definition Members_Of_%.*s[] = \n", name_token.text_length, name_token.text);
        printf("{\n");
        for (;;)
        {
            Token member_token = get_token(tokenizer);
            if (member_token.type == eToken_Close_Brace)
            {
                break;
            }
            else
            {
                parse_member(tokenizer, name_token, member_token);
            }
        }
        printf("};\n");

        Meta_Struct *meta = (Meta_Struct *)malloc(sizeof(Meta_Struct));
        meta->name = (char *)malloc(name_token.text_length + 1);
        memcpy(meta->name, name_token.text, name_token.text_length);
        meta->name[name_token.text_length] = 0;
        meta->next = first_meta_struct;
        first_meta_struct = meta;
    }
}

static void
parse_introspectable(Tokenizer *tokenizer)
{
    if (require_token(tokenizer, eToken_Open_Paren))
    {
        parse_introspection_params(tokenizer);

        Token type_token = get_token(tokenizer);
        if (token_equals(type_token, "struct"))
        {
            parse_struct(tokenizer);
        }
        else
        {
        }
    }
    else
    {
    }
}

int
main(int arg_count, char **args)
{
    char *file_names[] = {
        "../src/game.h",
        "../src/math.h",
    };

    for (int file_idx = 0;
         file_idx < (sizeof(file_names) / sizeof(file_names[0]));
         ++file_idx)
    {
        char *entire_file = read_entire_file_and_null_terminate(file_names[file_idx]);

        Tokenizer tokenizer = {};
        tokenizer.at = entire_file;

        bool parsing = true;
        while (parsing)
        {
            Token token = get_token(&tokenizer);
            switch (token.type)
            {
                case eToken_End_Of_Stream:
                {
                    parsing = false;
                } break;

                case eToken_Unknown:
                {
                } break;

                case eToken_Identifier:
                {
                    if (token_equals(token, "INTROSPECT"))
                    {
                        parse_introspectable(&tokenizer);
                    }
                } break;

                default:
                {
                } break;
            }
        }
    }

    printf("#define META_HANDLE_TYPE_DUMP(member_ptr, next_indent_level) \\\n");
    for (Meta_Struct *meta = first_meta_struct;
         meta;
         meta = meta->next)
    {
        printf("    case eMeta_Type_%s: { debug_text_line(member->name); debug_dump_struct(array_count(Members_Of_%s), Members_Of_%s, member_ptr, (next_indent_level)); } break; %s\n",
               meta->name, meta->name, meta->name,
               meta->next ? "\\" : "");
    }
   



    return 0;
}
