/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#ifndef _ZEND_ERRORS_H
#define _ZEND_ERRORS_H

#define E_CORE_ERROR 	(1<<0L)
#define E_CORE_WARNING	(1<<1L)
#define E_COMPILE_ERROR	(1<<2L)
#define E_COMPILE_WARNING	(1<<3L)
#define E_PARSE			(1<<4L)
#define E_ERROR			(1<<5L)
#define E_WARNING		(1<<6L)
#define E_NOTICE		(1<<7L)

#define E_ALL (E_ERROR | E_WARNING | E_PARSE | E_NOTICE | E_CORE_ERROR | E_CORE_WARNING | E_COMPILE_ERROR | E_COMPILE_WARNING)
#define E_CORE (E_CORE_ERROR | E_CORE_WARNING)


#endif /* _ZEND_ERRORS_H */
