<?php
$page = isset($_GET['page']) ? $_GET['page'] : '';
$options = '-E UTF-8 -ansi -modern';
$development = false;

$select_page = preg_match('/^\\w+$/', $page) ? ":select $page" : '';
$stderr = $development ? '2>&1' : '';

header('X-Powered-By: Lisp');
header('Content-Type: text/html; charset=UTF-8');
system("clisp $options Web.lisp $select_page $stderr", $exit_status);

if ($exit_status <> 0) {
    include 'index.html';
}
?>