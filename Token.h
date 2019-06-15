#pragma once

// TOKENS--------------->
enum {
  tok_fn = 2,

  tok_ret = 100,
  tok_void = 101,
  tok_int = 102,

  tok_num_int = 200,
  tok_num_double = 201,

  tok_semi = 300,
  tok_equal = 301,
  tok_lp = 302, // (
  tok_rp = 303, // )
  tok_lb = 304, // {
  tok_rb = 305, // }
  tok_arrow = 306,

  // operator
  tok_plus = 400,
  tok_minus = 401,
  tok_star = 402,
  tok_slash = 403,

  tok_identifier = 500,

  tok_eof = -1
};