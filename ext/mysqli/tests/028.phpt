--TEST--
function test: mysqli_character_set_name
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	$cset = substr(mysqli_character_set_name($link),0,6);

	var_dump($cset);

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
%unicode|string%(%d) "%s"
done!