<?php

// Hello! I'm the router php, i will get the request ajax and call the compiler that 
// will convert a program C to Javascript! It's awesome! 
error_reporting(E_ALL);

define('CLOUD_PARSER_ERROR', 1);
define('CLOUD_SEMANTIC_ERROR', 2);


$source = $_POST['source'];


$output = '';
$return = 0;

$ip = $_SERVER['REMOTE_ADDR'];
$filename = sha1($ip);


$fp = fopen(__DIR__.'/sources/'.$filename.'.cloud', 'w+');
fwrite($fp, $source);
fclose($fp);

exec(__DIR__.'/compiler/cloudblocks '. __DIR__.'/sources/'.$filename.'.cloud', $output, $return);

$to_browser = array();

switch ($return) {
	case CLOUD_SEMANTIC_ERROR:
	case CLOUD_PARSER_ERROR:
		$to_browser = array(
			'errors' =>	$output, 
			'errorno' => $return,
		);
	break;

	default: 
		// return code, only! =) 
		$to_browser = array(
			'errorno' => 0,
			'source' => implode("\n",$output),
		);
	break; 
}

header("Access-Control-Allow-Origin: *");
header('Content-Type: Application/json');

echo json_encode($to_browser);  exit(0);