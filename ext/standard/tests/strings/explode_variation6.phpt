--TEST--
Test explode() function : usage variations - misc tests
--FILE--
<?php

/* Prototype  : array explode  ( string $delimiter  , string $string  [, int $limit  ] )
 * Description: Split a string by string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing explode() function: misc tests ***\n";

$str = b"one\x00two\x00three\x00four";

echo "\n-- positive limit with null separator --\n";
$e = test_explode(b"\x00", $str, 2);

echo "\n-- negative limit (since PHP 5.1) with null separator --\n";
$e = test_explode(b"\x00", $str, -2);

echo "\n-- unknown string --\n";
$e = test_explode(b"fred", $str,1);

echo "\n-- limit = 0 --\n";
$e = test_explode(b"\x00", $str, 0);

echo "\n-- limit = -1 --\n";
$e = test_explode(b"\x00", $str, -1);

echo "\n-- large limit = -100 --\n";
$e = test_explode(b"\x00", $str, 100);

function test_explode($delim, $string, $limit)
{
	$e = explode($delim, $string, $limit);
	foreach ( $e as $v) 
	{
		var_dump(bin2hex($v));
	}	
}
?>
===DONE===
--EXPECT--
*** Testing explode() function: misc tests ***

-- positive limit with null separator --
unicode(6) "6f6e65"
unicode(28) "74776f00746872656500666f7572"

-- negative limit (since PHP 5.1) with null separator --
unicode(6) "6f6e65"
unicode(6) "74776f"

-- unknown string --
unicode(36) "6f6e650074776f00746872656500666f7572"

-- limit = 0 --
unicode(36) "6f6e650074776f00746872656500666f7572"

-- limit = -1 --
unicode(6) "6f6e65"
unicode(6) "74776f"
unicode(10) "7468726565"
unicode(8) "666f7572"

-- large limit = -100 --
unicode(6) "6f6e65"
unicode(6) "74776f"
unicode(10) "7468726565"
unicode(8) "666f7572"
===DONE===