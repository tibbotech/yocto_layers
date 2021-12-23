<!DOCTYPE html>
<html>
<head>
<title>Date/Time</title>
</head>
<body>
<?php
 if ( $_GET[ 'zone'] != '') {
   $out = '';
   $c = 'timedatectl set-timezone ' . $_GET[ 'zone'] . ' 2>&1';
   $r = exec( $c, $out, $ret);
   if ( $ret != 0) echo( "ERROR");
   foreach( $out as $k=>$v) echo( $v . '<br>');
 }
?>
<h1><a href='/'>/</a> Date and time</h1>
<?php
 $cur_tz = 'UTC';

 $out = '';
 $t_info = array();
 $r = exec( 'timedatectl status 2>&1', $out, $ret);
 foreach( $out as $k=>$v) {
   $ta = explode( ':', $v);
   $t_info[ trim( $ta[ 0])] = trim( $ta[ 1]);
   echo( $v . '<br>');
 }
 $ta = explode( ' ', $t_info[ 'Time zone']);
 $cur_tz = $ta[ 0];
?>
<br>
<form action=''>
Set Zone:
<select name='zone'>
<?php
 $out = '';
 $r = exec( 'timedatectl list-timezones 2>&1', $out, $ret);
 foreach( $out as $k=>$v) echo( "<option " . ( $cur_tz == $v ? 'selected=selected ' : '') . "value='${v}'>${v}</option>\n");
?>
</select>
<input type='submit'>
</form>
</body>
</html>
