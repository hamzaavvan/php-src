--TEST--
Bug #32828 (Throwing exception in output_callback function with ob_start and ob_end_clean leads to segfault)
--FILE--
<?php

function output_handler($buffer)
{
	throw new Exception;
}

ob_start('output_handler');

ob_end_clean();
?>
--EXPECTF--	
Fatal error: Uncaught exception 'Exception' in %s:%d
Stack trace:
#0 %s(%d): output_handler()
#1 %s(%d): output_handler('', %d)
#2 %s(%d): ob_end_clean()
#3 {main}
  thrown in %s on line %d
