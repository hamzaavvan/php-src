/* Generated by re2c 0.11.0 on Mon Feb  5 14:06:48 2007 */
#line 1 "ext/phar/phar_path_check.re"
/*
  +----------------------------------------------------------------------+
  | phar php single-file executable PHP extension                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2006 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Marcus Boerger <helly@php.net>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "phar_internal.h"

phar_path_check_result phar_path_check(char **s, int *len, const char **error)
{
	const unsigned char *p = (const unsigned char*)*s;
	const unsigned char *m;

	if (*len == 1 && *p == '.') {
		*error = "current directory reference";
		return pcr_err_curr_dir;
	} else if (*len == 2 && p[0] == '.' && p[1] == '.') {
		*error = "upper directory reference";
		return pcr_err_up_dir;
	}

#define YYCTYPE         unsigned char
#define YYCURSOR        p
#define YYLIMIT         p+*len
#define YYMARKER        m
#define YYFILL(n)

loop:
{

#line 48 "ext/phar/phar_path_check.c"
	{
		YYCTYPE yych;

		if((YYLIMIT - YYCURSOR) < 4) YYFILL(4);
		yych = *YYCURSOR;
		if(yych <= '.') {
			if(yych <= 0x0A) {
				if(yych <= 0x00) goto yy13;
				if(yych <= 0x09) goto yy10;
				goto yy12;
			} else {
				if(yych <= 0x19) goto yy10;
				if(yych == '*') goto yy6;
				goto yy15;
			}
		} else {
			if(yych <= '?') {
				if(yych <= '/') goto yy2;
				if(yych <= '>') goto yy15;
				goto yy8;
			} else {
				if(yych == '\\') goto yy4;
				if(yych <= 0x7F) goto yy15;
				goto yy10;
			}
		}
yy2:
		yych = *(YYMARKER = ++YYCURSOR);
		if(yych <= 0x00) goto yy16;
		if(yych <= '-') goto yy3;
		if(yych <= '.') goto yy18;
		if(yych <= '/') goto yy20;
yy3:
#line 97 "ext/phar/phar_path_check.re"
		{
			goto loop;
		}
#line 86 "ext/phar/phar_path_check.c"
yy4:
		++YYCURSOR;
#line 60 "ext/phar/phar_path_check.re"
		{
			*error = "back-slash";
			return pcr_err_back_slash;
		}
#line 94 "ext/phar/phar_path_check.c"
yy6:
		++YYCURSOR;
#line 68 "ext/phar/phar_path_check.re"
		{
			*error = "star";
			return pcr_err_star;
		}
#line 102 "ext/phar/phar_path_check.c"
yy8:
		++YYCURSOR;
#line 72 "ext/phar/phar_path_check.re"
		{
			if (**s == '/') {
				(*s)++;
			}
			*len = (p - (const unsigned char*)*s) -1;
			*error = NULL;
			return pcr_use_query;
		}
#line 114 "ext/phar/phar_path_check.c"
yy10:
		++YYCURSOR;
yy11:
#line 80 "ext/phar/phar_path_check.re"
		{
			*error ="illegal character";
			return pcr_err_illegal_char;
		}
#line 123 "ext/phar/phar_path_check.c"
yy12:
		yych = *++YYCURSOR;
		goto yy11;
yy13:
		++YYCURSOR;
#line 84 "ext/phar/phar_path_check.re"
		{
			if (**s == '/') {
				(*s)++;
				(*len)--;
			}
			if ((p - (const unsigned char*)*s) - 1 != *len)
			{
				*error ="illegal character";
				return pcr_err_illegal_char;
			}
			*error = NULL;
			return pcr_is_ok;
		}
#line 143 "ext/phar/phar_path_check.c"
yy15:
		yych = *++YYCURSOR;
		goto yy3;
yy16:
		++YYCURSOR;
#line 64 "ext/phar/phar_path_check.re"
		{
			*error = "empty directory";
			return pcr_err_empty_entry;
		}
#line 154 "ext/phar/phar_path_check.c"
yy18:
		yych = *++YYCURSOR;
		if(yych <= 0x00) goto yy23;
		if(yych <= '-') goto yy19;
		if(yych <= '.') goto yy22;
		if(yych <= '/') goto yy23;
yy19:
		YYCURSOR = YYMARKER;
		goto yy3;
yy20:
		++YYCURSOR;
#line 48 "ext/phar/phar_path_check.re"
		{
			*error = "double slash";
			return pcr_err_double_slash;
		}
#line 171 "ext/phar/phar_path_check.c"
yy22:
		yych = *++YYCURSOR;
		if(yych <= 0x00) goto yy25;
		if(yych == '/') goto yy25;
		goto yy19;
yy23:
		++YYCURSOR;
#line 56 "ext/phar/phar_path_check.re"
		{
			*error = "current directory reference";
			return pcr_err_curr_dir;
		}
#line 184 "ext/phar/phar_path_check.c"
yy25:
		++YYCURSOR;
#line 52 "ext/phar/phar_path_check.re"
		{
			*error = "upper directory reference";
			return pcr_err_up_dir;
		}
#line 192 "ext/phar/phar_path_check.c"
	}
}
#line 100 "ext/phar/phar_path_check.re"

}
