<!DOCTYPE html>
<html>
<head>
<title>TPS</title>
</head>
<body>
<b>Build:</b>
<?php
 $r = exec( 'uname -a');
 echo( $r);
 $sys = array();
 $xa = file( '/etc/os-release');
 foreach ( $xa as $k=>$v) {
   $xa = explode( '=' , $v);
   $sys[ $xa[ 0]] = $xa[ 1];
 }
?>
<br>
<b>Name:</b> <?php echo( $sys[ 'PRETTY_NAME']); ?>
<br>
<b>Device Tree:</b>
<?php
 $x = file_get_contents( '/proc/device-tree/model');
 echo( $x);
?>
<br>
<b>Current Date/Time:</b>
<?php
 echo( @date( 'Y-m-d H:i:s e (O)'));
?>
<br><br>
<a href='/dt.php'>Edit Date/Time</a><br>
<a href='/pass_sys.php'>System password</a><br>
<a href='/pass_www.php'>WebAdmin password</a><br>
<?php
 $Da = glob( 'mod-*', GLOB_ONLYDIR);
 foreach ( $Da as $k=>$v) {
   echo( "<a href='/${v}/'>" . $v . "</a><br>\n");
 }
?>
</body>
</html>
