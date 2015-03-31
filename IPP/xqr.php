#!/usr/bin/php
<?php
#XQR:xcoufa09

/*
 * =====================================================================
 *        Version:  1.0
 *        Created:  03/17/2015 06:37:50 PM
 *         Author:  Tomáš Coufal
 *        Company:  FIT VUT
 * =====================================================================
 */

// parametry
$arg = arg_parse($argv);
// vstup
if (isset($arg['input']))
  $data = load_file($arg['input']);
else
  $data = file_get_contents("php://stdin");

$data = new SimpleXmlElement($data) or extended_die("XML is not valid", 4);
// query
if (!isset($arg['query']))
  exit(1);
$query = query_parse($arg['query']);
if (empty($query))
  $result = array();
else
  $result = proceed_query($data, $query);
// result as Xml
$xml = (string) NULL;
if (!empty($result)) {
  foreach ($result as $item) {
    $xml = $xml . $item->asXml();
  }
}
// output
if (isset($arg['root']))
  $xml = "<{$arg['root']}>\n" . "$xml" . "\n</{$arg['root']}>";
if (!isset($arg['noheader']))
  $xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" . "$xml";
if (isset($arg["output"]))
  dump_output($arg['output'], $xml);
else
  file_put_contents("php://stdout", $xml);

// =====================================================================
// Parse arguments the C-like way, ugly as hell
function arg_parse($arguments)
{
  $argarray = array();
  if (count($arguments) == 2) {
    if ($arguments[1] == "--help") {
      print_help();
    }
  }
  while (count($arguments) > 1) {
    $argument = array_pop($arguments);
    if ($argument == "-n") {
      if (!array_key_exists("noheader", $argarray))
        $argarray['noheader'] = True;
      else
        extended_die("Parameter -n is used more then once.", 1);
    }
    elseif (substr($argument,0,8) == "--input=") {
      if (!array_key_exists("input", $argarray) AND strlen($argument) > 8)
        $argarray['input'] = substr($argument, 8);
      else
        extended_die("Parameter --input is not valid.", 1);
    }
    elseif (substr($argument,0,9) == "--output=") {
      if (!array_key_exists("output", $argarray) AND strlen($argument) > 9)
        $argarray['output'] = substr($argument, 9);
      else
        extended_die("Parameter --output is not valid.", 1);
    }
    elseif (substr($argument,0,7) == "--root=") {
      if (!array_key_exists("root", $argarray) AND strlen($argument) > 7)
        $argarray['root'] = substr($argument, 7);
      else
        extended_die("Parameter --root is not valid.", 1);
    }
    elseif (substr($argument,0,5) == "--qf=") {
      if (!array_key_exists("query", $argarray) AND strlen($argument) > 5)
        $argarray['query'] = load_file(substr($argument, 5));
      else
        extended_die("Parameter --qf is not valid.", 1);
    }
    elseif (substr($argument,0,8) == "--query=") {
      if (!array_key_exists("query", $argarray) AND strlen($argument) > 8)
        $argarray['query'] = substr($argument, 8);
      else
        extended_die("Parameter --query is not valid.", 1);
    }
    else {
      extended_die("Unknown parameter, try --help.", 1);
    }
  }
  return $argarray;
}
// Parse query and split it into array/map
function query_parse($query) {
  // first check
  preg_match("/(SELECT.*(LIMIT)?.*FROM.*(WHERE)?.*(ORDER BY)?.*)/", $query, $querySyntax);
  if (empty($querySyntax))
    extended_die("Syntax error when parsing query", 80);
  // SELECT
  preg_match("/(?<=SELECT\s)(?:\s*).+?(?=\s+LIMIT|\s+FROM)/", $query, $attribute['SELECT']);
  if (isset($attribute['SELECT'][0])) {
    preg_match("/\w*/", $attribute['SELECT'][0], $tmp);
    if ($attribute['SELECT'][0] != $tmp[0])
      extended_die("query: SELECT is not valid", 80);
  }
  // LIMIT
  preg_match("/(?<=LIMIT\s)(?:\s*).+(?=\s+FROM)/", $query, $attribute['LIMIT']);
  if (isset($attribute['LIMIT'][0])) {
    preg_match("/\d*/", $attribute['LIMIT'][0], $tmp);
    if ($attribute['LIMIT'][0] != $tmp[0])
      extended_die("query: LIMIT is not valid '{$attribute['LIMIT'][0]}' vs. '{$tmp[0]}'", 80);
  }
  // FROM
  preg_match("/(?<=FROM\s)(?:\s*).*?(?=\s|$)/", $query, $attribute['FROM']);
  if (isset($attribute['FROM'][0])) {
    preg_match("/\w*(?:\.)?\w+/", $attribute['FROM'][0], $tmp);
    if ($attribute['FROM'][0] != $tmp[0])
      extended_die("query: FROM is not valid", 80);
    $attribute['FROM'] = preg_split("/[.]/", $attribute['FROM'][0]);
  }
  // WHERE
  preg_match("/(?<=WHERE\s)(?:\s*).*(?=\sORDER BY|$)/", $query, $attribute['WHERE']);
  if (isset($attribute['WHERE'][0])) {
    preg_match("/^(?:\s*)((?:NOT\s+)*)(\w+)?(?:\.(\w+)?)?\s+(CONTAINS|=|>|<)\s+(\\\"(?:.*)\\\"|-?\d+\b|\\\"\\\")$/", $attribute['WHERE'][0], $attribute['WHERE']);
    array_shift($attribute['WHERE']);
    if ((substr_count($attribute['WHERE'][0], "NOT") % 2) == 0)
      $attribute['WHERE'][0] = "";
    else
      $attribute['WHERE'][0] = "NOT";
    if  ((count($attribute['WHERE']) != 5)
    or ((isset($attribute['WHERE'][1][0])) and (is_numeric($attribute['WHERE'][1][0])))
    or (($attribute['WHERE'][3] == "CONTAINS") and (is_numeric($attribute['WHERE'][4])))) {
        extended_die("WHERE condition is not valid", 80);
    }
  }
  // ORDER BY
  preg_match("/(?<=ORDER BY\s)\w+(?:\.\w+)|(?:ASC|DESC)$/", $query, $attribute['ORDER BY']);
  if (isset($attribute['ORDER BY'][0]))
    $attribute['ORDER BY'] = preg_split("/[.]/", $attribute['ORDER BY'][0]);
  return $attribute;
}
// Apply query on an input using XPath
function proceed_query($data, $query) {
  // FROM
  if (!isset($query['FROM'][0]))
    return "";
  if ($query['FROM'][0] == "" or $query['FROM'][0] == "ROOT")
    $xpath = "//*";
  else
    $xpath = "//{$query['FROM'][0]}";
  if (isset($query['FROM'][1]) and $query['FROM'][1] != "")
    $xpath .= "[@{$query['FROM'][1]}]";
  $data = $data->xpath($xpath);
  $merlin = array_filter($data);
  if (empty($merlin))
    return "";
  $data = $data[0];
  // SELECT
  if (!isset($query['SELECT'][0]))
    return "";
  $data = $data->xpath(".//{$query['SELECT'][0]}");
  // WHERE
  if (isset($query['WHERE'][0])) {
    // body
    if ($query['WHERE'][3] == "CONTAINS") {
      if (!isset($query['WHERE'][2]) or $query['WHERE'][2] == "")
        $query['WHERE'][2] = ".";
      else
        $query['WHERE'][2] = "@{$query['WHERE'][2]}";
      $xpath = "contains({$query['WHERE'][2]}, {$query['WHERE'][4]})";
    } else {
      if ($query['WHERE'][1] == "")
        $xpath = "*";
      else
        $xpath = "{$query['WHERE'][1]}";
      $closure = "";
      if ($query['WHERE'][2] != "") {
        $xpath .= "[@{$query['WHERE'][2]}";
        $closure = "]";
      }
      // [3] == '=' | '>' | '<' only -> safe
      $xpath .= " {$query['WHERE'][3]} {$query['WHERE'][4]}{$closure}";
    }
    // additoinal wrappers
    if ($query['WHERE'][0] == "NOT")
      $xpath = "[not({$xpath})]";
    else
      $xpath = "[{$xpath}]";
    if ($query['SELECT'][0] != "")
      $xpath = "//{$query['SELECT'][0]}{$xpath}";
    else
      $xpath = "//*{$xpath}";
    foreach ($data as $item) {
      $data = $item->xpath($xpath);
    }
  }
  // LIMIT
  if (isset($query['LIMIT'][0]))
    $data = array_slice($data, 0, $query['LIMIT'][0]);
  return $data;
}
// Dump output to given file
function dump_output($filename, $data) {
  $file = fopen($filename, "w") or extended_die("Unable to open file '{$filename} for writing'", 3);
  fwrite($file, $data);
  fclose($file);
  return;
}
// Load content of a file
function load_file($filename) {
  $file = fopen($filename, "r") or extended_die("File '{$filename}'is not readable or does not exist", 2);
  $data = fread($file, filesize($filename));
  fclose($file);
  return $data;
}
// Print error messages
function extended_die($msg, $exitcode) {
  file_put_contents('php://stderr', $msg . "\r\n");
  die($exitcode);
}
function print_help() {
  print("
XML query in PHP

parameters:
  --help                 - Show help
  --input=filename.ext   - Input file with xml
  --output=filename.ext  - Output file with xml
  --query='query'        - Query under xml - can not be used with -qf attribute
  --qf=filename.ext      - Filename with query under xml
  -n                     - Xml will be generated without XML header
  --root=element         - Name of root element

Author: Tomas Coufal <xcoufa09@stud.fit.vutbr.cz>
");
  die(0);
}
?>
