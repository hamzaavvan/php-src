#include "php.h"
#include "php_globals.h"

#include "SAPI.h"

#if CGI_BINARY

#include <stdio.h>
#include "php.h"
#ifdef MSVC5
#include "win32/time.h"
#include "win32/signal.h"
#include <process.h>
#else
#include "build-defs.h"
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#if HAVE_SETLOCALE
#include <locale.h>
#endif
#include "zend.h"
#include "php_ini.h"
#include "php_globals.h"
#include "main.h"
#include "fopen-wrappers.h"
#include "ext/standard/php3_standard.h"
#include "snprintf.h"
#if WIN32|WINNT
#include <io.h>
#include <fcntl.h>
#include "win32/syslog.h"
#include "win32/php_registry.h"
#else
#include <syslog.h>
#endif

#include "zend_compile.h"
#include "zend_execute.h"
#include "zend_highlight.h"
#include "zend_indent.h"


#if USE_SAPI
#include "serverapi/sapi.h"
void *gLock;
#ifndef THREAD_SAFE
struct sapi_request_info *sapi_rqst;
#endif
#endif

#if MSVC5 || !defined(HAVE_GETOPT)
#include "getopt.h"
#endif

PHPAPI extern char *php3_ini_path;

#define PHP_MODE_STANDARD	1
#define PHP_MODE_HIGHLIGHT	2
#define PHP_MODE_INDENT		3

PHPAPI extern char *optarg;
PHPAPI extern int optind;


static int zend_cgibin_ub_write(const char *str, uint str_length)
{
	return fwrite(str, 1, str_length, stdout);
}


static void sapi_cgi_send_header(sapi_header_struct *sapi_header, void *server_context)
{
	if (sapi_header) {
		PHPWRITE_H(sapi_header->header, sapi_header->header_len);
	}
	PHPWRITE_H("\r\n", 2);
}


static sapi_module_struct sapi_module = {
	"PHP Language",					/* name */
									
	php_module_startup,				/* startup */
	php_module_shutdown_wrapper,	/* shutdown */

	zend_cgibin_ub_write,			/* unbuffered write */

	php3_error,						/* error handler */

	NULL,							/* header handler */
	NULL,							/* send headers handler */
	sapi_cgi_send_header,			/* send header handler */
};


static void php_cgi_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php";
	}

	php3_printf("Usage: %s [-q] [-h]"
				" [-s]"
				" [-v] [-i] [-f <file>] | "
				"{<file> [args...]}\n"
				"  -q       Quiet-mode.  Suppress HTTP Header output.\n"
				"  -s       Display colour syntax highlighted source.\n"
				"  -f<file> Parse <file>.  Implies `-q'\n"
				"  -v       Version number\n"
				"  -c<path> Look for php3.ini file in this directory\n"
#if SUPPORT_INTERACTIVE
				"  -a		Run interactively\n"
#endif
				"  -e		Generate extended information for debugger/profiler\n"
				"  -i       PHP information\n"
				"  -h       This help\n", prog);
}


static void init_request_info(SLS_D)
{
	char *request_method = getenv("REQUEST_METHOD");

	SG(request_info).query_string = getenv("QUERY_STRING");
	SG(request_info).request_uri = getenv("PATH_INFO");
	if (request_method && !strcmp(request_method, "HEAD")) {
		SG(request_info).headers_only = 1;
	} else {
		SG(request_info).headers_only = 0;
	}
}


int main(int argc, char *argv[])
{
	int cgi = 0, c, i, len;
	zend_file_handle file_handle;
	char *s;
/* temporary locals */
	char *_cgi_filename=NULL;
	int _cgi_started=0;
	int behavior=PHP_MODE_STANDARD;
#if SUPPORT_INTERACTIVE
	int interactive=0;
#endif
/* end of temporary locals */
#ifdef ZTS
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	php_core_globals *core_globals;
	sapi_globals_struct *sapi_globals;
#endif


#ifndef ZTS
	if (setjmp(EG(bailout))!=0) {
		return -1;
	}
#endif

#ifdef ZTS
	tsrm_startup(1,1,0);
#endif

	sapi_startup(&sapi_module);

#if WIN32|WINNT
	_fmode = _O_BINARY;			/*sets default for file streams to binary */
	setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
	setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif


	/* Make sure we detect we are a cgi - a bit redundancy here,
	   but the default case is that we have to check only the first one. */
	if (getenv("SERVER_SOFTWARE")
		|| getenv("SERVER_NAME")
		|| getenv("GATEWAY_INTERFACE")
		|| getenv("REQUEST_METHOD")) {
		cgi = 1;
		if (argc > 1)
			request_info.php_argv0 = strdup(argv[1]);
		else request_info.php_argv0 = NULL;
#if FORCE_CGI_REDIRECT
		if (!getenv("REDIRECT_STATUS")) {
			PUTS("<b>Security Alert!</b>  PHP CGI cannot be accessed directly.\n\
\n\
<P>This PHP CGI binary was compiled with force-cgi-redirect enabled.  This\n\
means that a page will only be served up if the REDIRECT_STATUS CGI variable is\n\
set.  This variable is set, for example, by Apache's Action directive redirect.\n\
<P>You may disable this restriction by recompiling the PHP binary with the\n\
--disable-force-cgi-redirect switch.  If you do this and you have your PHP CGI\n\
binary accessible somewhere in your web tree, people will be able to circumvent\n\
.htaccess security by loading files through the PHP parser.  A good way around\n\
this is to define doc_root in your php3.ini file to something other than your\n\
top-level DOCUMENT_ROOT.  This way you can separate the part of your web space\n\n\
which uses PHP from the normal part using .htaccess security.  If you do not have\n\
any .htaccess restrictions anywhere on your site you can leave doc_root undefined.\n\
\n");

			/* remove that detailed explanation some time */

			return FAILURE;
		}
#endif							/* FORCE_CGI_REDIRECT */
	}

	if (php_module_startup(&sapi_module)==FAILURE) {
		return FAILURE;
	}
#ifdef ZTS
	compiler_globals = ts_resource(compiler_globals_id);
	executor_globals = ts_resource(executor_globals_id);
	core_globals = ts_resource(core_globals_id);
	sapi_globals = ts_resource(sapi_globals_id);
#endif

	CG(extended_info) = 0;

	if (!cgi) {					/* never execute the arguments if you are a CGI */
		request_info.php_argv0 = NULL;
		while ((c = getopt(argc, argv, "c:qvisnaeh?vf:")) != -1) {
			switch (c) {
				case 'f':
					if (!_cgi_started){ 
						if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
							php_module_shutdown();
							return FAILURE;
						}
					}
					_cgi_started=1;
					_cgi_filename = estrdup(optarg);
					/* break missing intentionally */
				case 'q':
					php3_noheader();
					break;
				case 'v':
					if (!_cgi_started) {
						if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
							php_module_shutdown();
							return FAILURE;
						}
					}
					php3_printf("%s\n", PHP_VERSION);
					exit(1);
					break;
				case 'i':
					if (!_cgi_started) {
						if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
							php_module_shutdown();
							return FAILURE;
						}
					}
					_cgi_started=1;
					php3_TreatHeaders();
					_php3_info();
					exit(1);
					break;
				case 's':
					behavior=PHP_MODE_HIGHLIGHT;
					break;
				case 'n':
					behavior=PHP_MODE_INDENT;
					break;
				case 'c':
					php3_ini_path = strdup(optarg);		/* intentional leak */
					break;
				case 'a':
#if SUPPORT_INTERACTIVE
					printf("Interactive mode enabled\n\n");
					interactive=1;
#else
					printf("Interactive mode not supported!\n\n");
#endif
					break;
				case 'e':
					CG(extended_info) = 1;
					break;
				case 'h':
				case '?':
					php3_noheader();
					zend_output_startup();
					php_cgi_usage(argv[0]);
					exit(1);
					break;
				default:
					break;
			}
		}
	}							/* not cgi */

#if SUPPORT_INTERACTIVE
	EG(interactive) = interactive;
#endif

	if (!_cgi_started) {
		if (php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC)==FAILURE) {
			php_module_shutdown();
			return FAILURE;
		}
	}
	file_handle.filename = "-";
	file_handle.type = ZEND_HANDLE_FP;
	file_handle.handle.fp = stdin;
	if (_cgi_filename) {
		request_info.filename = _cgi_filename;
	}

	php3_TreatHeaders();

	init_request_info(SLS_C);

	if (!cgi) {
		if (!SG(request_info).query_string) {
			for (i = optind, len = 0; i < argc; i++)
				len += strlen(argv[i]) + 1;

			s = malloc(len + 1);	/* leak - but only for command line version, so ok */
			*s = '\0';			/* we are pretending it came from the environment  */
			for (i = optind, len = 0; i < argc; i++) {
				strcat(s, argv[i]);
				if (i < (argc - 1))
					strcat(s, "+");
			}
			SG(request_info).query_string = s;
		}
		if (!request_info.filename && argc > optind)
			request_info.filename = argv[optind];
	}
	/* If for some reason the CGI interface is not setting the
	   PATH_TRANSLATED correctly, request_info.filename is NULL.
	   We still call php3_fopen_for_parser, because if you set doc_root
	   or user_dir configuration directives, PATH_INFO is used to construct
	   the filename as a side effect of php3_fopen_for_parser.
	 */
	if (cgi || request_info.filename) {
		file_handle.filename = request_info.filename;
		file_handle.handle.fp = php3_fopen_for_parser();
	}

	if (cgi && !file_handle.handle.fp) {
		PUTS("No input file specified.\n");
#if 0	/* this is here for debuging under windows */
		if (argc) {
			i = 0;
			php3_printf("\nargc %d\n",argc); 
			while (i <= argc) {
				php3_printf("%s\n",argv[i]); 
				i++;
			}
		}
#endif
		php_request_shutdown((void *) 0);
		php_module_shutdown();
		return FAILURE;
	} else if (file_handle.handle.fp && file_handle.handle.fp!=stdin) {
		/* #!php support */
		c = fgetc(file_handle.handle.fp);
		if (c == '#') {
			while (c != 10 && c != 13) {
				c = fgetc(file_handle.handle.fp);	/* skip to end of line */
			}
			CG(zend_lineno)++;
		} else {
			rewind(file_handle.handle.fp);
		}
	}

	switch (behavior) {
		case PHP_MODE_STANDARD:
			php_execute_script(&file_handle CLS_CC ELS_CC PLS_CC);
			break;
		case PHP_MODE_HIGHLIGHT: {
				zend_syntax_highlighter_ini syntax_highlighter_ini;

				if (open_file_for_scanning(&file_handle CLS_CC)==SUCCESS) {
					php_get_highlight_struct(&syntax_highlighter_ini);
					zend_highlight(&syntax_highlighter_ini);
					fclose(file_handle.handle.fp);
				}
				return 0;
			}
			break;
		case PHP_MODE_INDENT:
			open_file_for_scanning(&file_handle CLS_CC);
			zend_indent();
			fclose(file_handle.handle.fp);
			return 0;
			break;
	}

	php3_header();			/* Make sure headers have been sent */
	php_request_shutdown((void *) 0);
	php_module_shutdown();
	return SUCCESS;
}


#endif
