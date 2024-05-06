/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <expr.hpp>
#include <sdb.hpp>
#include <regex.h>
#include <stdbool.h>
#include <fake_memory.h>
#include <string.h>
#include <cassert>
#include <unordered_map>
#include <stdexcept>

// #define EXPR_DBG

namespace expr
{

  enum
  {
    TK_NOTYPE = 256,
    /* TODO: Add more token types */
    TK_LEFT_PAREN,
    TK_RIGHT_PAREN,
    TK_DECIMAL,
    TK_HEX,
    TK_ADD,
    TK_SUB,
    TK_MUL,
    TK_DIV,
    TK_REGISTER,
    TK_NUMBER, // only for parsing
    TK_EQUAL,
    TK_UNEQUAL,
  };

  static struct rule
  {
    const char *regex;
    int token_type;
  } rules[] = {

      /* TODO: Add more rules.
       * Pay attention to the precedence level of different rules.
       */
      {" +", TK_NOTYPE},                                                                                             // spaces
      {"\\+", TK_ADD},                                                                                               // plus
      {"-", TK_SUB},                                                                                                 // sub
      {"\\*", TK_MUL},                                                                                               // mul
      {"/", TK_DIV},                                                                                                 // div
      {"==", TK_EQUAL},                                                                                              // equal
      {"!=", TK_UNEQUAL},                                                                                            // unequal
      {"\\(", TK_LEFT_PAREN},                                                                                        // left parenthesis
      {"\\)", TK_RIGHT_PAREN},                                                                                       // right parenthesis
      {"0x([0-9]|a|A|b|B|c|C|d|D|e|E|f|F)+", TK_HEX},                                                                // hex
      {"\\$(x([0-9]|(1[0-5]))|(0)|(ra)|(sp)|(gp)|(tp)|(t[0-6])|(fp)|(s([0-9]|(1[0-1])))|(a[0-7])|pc)", TK_REGISTER}, // register
      {"[0-9]+", TK_DECIMAL},                                                                                        // decimal

  };

#define NR_REGEX sizeof(rules) / sizeof(rules[0])

  // c priority
  // https://en.cppreference.com/w/c/language/operator_precedence
  static unordered_map<int, int> OP_PRIORITY = {
      {TK_MUL, 3},
      {TK_DIV, 3},
      {TK_ADD, 4},
      {TK_SUB, 4},
      {TK_EQUAL, 7},
      {TK_UNEQUAL, 7},
  };

  static regex_t re[NR_REGEX] = {};

  /* Rules are used for many times.
   * Therefore we compile them only once before any usage.
   */
  void init_regex()
  {
    int i;
    char error_msg[128];
    int ret;

    for (i = 0; i < int(NR_REGEX); i++)
    {
      ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
      if (ret != 0)
      {
        regerror(ret, &re[i], error_msg, 128);
        // panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
        printf("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
      }
    }
  }

  typedef struct token
  {
    int type;
    char str[32];
  } Token;

  static Token tokens[32] __attribute__((used)) = {};
  static int nr_token __attribute__((used)) = 0;

  static void make_token(const char *e)
  {
    int position = 0;
    int i;
    regmatch_t pmatch;

    nr_token = 0;

    while (e[position] != '\0')
    {
      /* Try all rules one by one. */
      for (i = 0; i < int(NR_REGEX); i++)
      {
        if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
        {
          const char *substr_start = e + position;
          int substr_len = pmatch.rm_eo;

// Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
//     i, rules[i].regex, position, substr_len, substr_len, substr_start);
#ifdef EXPR_DBG
          printf("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
                 i, rules[i].regex, position, substr_len, substr_len, substr_start);
#endif
          position += substr_len;

          /* TODO: Now a new token is recognized with rules[i]. Add codes
           * to record the token in the array `tokens'. For certain types
           * of tokens, some extra actions should be performed.
           */

          // check the expr is not too long
          if (nr_token >= 32)
          {
            throw std::runtime_error("the expression is too long");
          }
          // check the too long token
          if (substr_len > 32)
          {
            throw std::runtime_error("some token is too long");
          }

          switch (rules[i].token_type)
          {
          case TK_NOTYPE:
            // ignor the space
            break;

          case TK_HEX:
          { // convert hex to decimal number
            char substr_hex[32];
            strncpy(substr_hex, substr_start + 2, substr_len - 2);
            substr_hex[substr_len - 2] = '\0';

            word_t dec_value = strtoul(substr_hex, NULL, 16);

            sprintf(tokens[nr_token].str, "%u", dec_value);
            tokens[nr_token].type = TK_NUMBER;

            break;
          }

          case TK_DECIMAL:
          {
            // just copy the decimal number
            tokens[nr_token].type = TK_NUMBER;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';

            break;
          }

          case TK_REGISTER:
          {
            // convert register to number
            char substr_register[32];
            strncpy(substr_register, substr_start + 1, substr_len - 1);
            substr_register[substr_len - 1] = '\0';

            bool ok = false;
            // word_t reg_val = isa_reg_str2val(substr_register, &ok);
            word_t reg_val = sdb::reg_str2val(substr_register, ok);
            assert(ok);

            tokens[nr_token].type = TK_NUMBER;
            sprintf(tokens[nr_token].str, "%u", reg_val);

            break;
          }

          default:
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
          }

          // increment the number of tokens
          nr_token++;

          break;
        }
      }
      if (i == NR_REGEX)
      {
        throw std::runtime_error(std::format("no match at position {}\n{}\n{:{}s}^\n", position, e, position, ""));
      }
    }
  }

  static Token *find_dominant_op(Token *begin, Token *end)
  {
    // find the first open paren
    Token *res = NULL;

    int unclosed_paren = 0;

    for (Token *token = begin; token != end; token++)
    {
      if (token->type == TK_LEFT_PAREN)
      {
        unclosed_paren++;
        continue;
      }
      if (token->type == TK_RIGHT_PAREN)
      {
        unclosed_paren--;
        continue;
      }

      if (unclosed_paren == 0)
      {
        if (res == NULL || OP_PRIORITY[token->type] > OP_PRIORITY[res->type])
        {
          res = token;
        }
      }
    }

    return res;
  }

  static bool is_parentheses_match(Token *begin, Token *end)
  {
    // test if the parentheses are matched
    int cnt = 0;
    for (Token *p = begin; p != end; p++)
    {
      if (p->type == TK_LEFT_PAREN)
      {
        cnt++;
      }
      else if (p->type == TK_RIGHT_PAREN)
      {
        cnt--;
      }
      if (cnt < 0)
      {
        return 0;
      }
    }

    return cnt == 0;
  }

  // eval express by token
  // [begin, end)
  static word_t eval(Token *begin, Token *end)
  {
    assert(begin <= end);

    // if the expression is empty
    if (begin == end)
    {
      throw std::runtime_error("unexpected expression");
    }

    // remove the outer parentheses
    if (begin->type == TK_LEFT_PAREN && is_parentheses_match(begin + 1, end - 1))
    {
      begin++;
      end--;
    }

    // test if the parentheses are matched
    if (!is_parentheses_match(begin, end))
    {
      throw std::runtime_error("parentheses are not matched");
    }

    word_t res = 0;

    // if it is a single number
    if (begin + 1 == end && begin->type == TK_NUMBER)
    {
      res = atoi(begin->str);
    }
    else if (begin + 2 == end && begin->type != TK_NUMBER)
    {
      // if it is a unary operator
      // now we only support the unary operator '-' and '*'
      if (begin->type == TK_SUB && (begin + 1)->type == TK_NUMBER)
      {
        res = -atoi((begin + 1)->str);
      }

      if (begin->type == TK_MUL && (begin + 1)->type == TK_NUMBER)
      {
        // word_t addr = atoi((begin + 1)->str);
        // check if the address is valid
        // if (!in_pmem(addr))
        // {
        //   sprintf(error_msg, "invalid address: 0x%x", addr);
        //   return false;
        // }

        // res = vaddr_read(addr, 4);
        word_t addr = atoi((begin + 1)->str);
        word_t value;
        sdb::cpu->memory.read(addr, (uint8_t *)&value, 4);

        res = value;
      }
    }
    else
    {
      // it must be a binary operator
      // find the dominant operator
      Token *dominant_op = find_dominant_op(begin, end);

      if (dominant_op == NULL)
      {
        // if there is no operator, failed
        throw std::runtime_error("unexpected expression");
      }

      // eval the left and right part
      word_t left_val = eval(begin, dominant_op);
      word_t right_val = eval(dominant_op + 1, end);

      // eval the result
      switch (dominant_op->type)
      {
      case TK_ADD:
        res = left_val + right_val;
        break;
      case TK_SUB:
        res = left_val - right_val;
        break;
      case TK_MUL:
        res = left_val * right_val;
        break;
      case TK_DIV:
        res = left_val / right_val;
        break;
      case TK_EQUAL:
        res = (left_val == right_val);
        break;
      case TK_UNEQUAL:
        res = (left_val != right_val);
        break;
      default:
        assert(0);
      }
    }

    return res;
  }

  word_t expr(std::string expr)
  {
    make_token(expr.c_str());

    // print the tokens
    for (int i = 0; i < nr_token; i++)
    {
#ifdef EXPR_DBG
      printf("tokens[%d]: type = %d, str = %s\n", i, tokens[i].type, tokens[i].str);
#endif
    }

    // /* TODO: Insert codes to evaluate the expression. */
    return eval(tokens, tokens + nr_token);
  }
}
