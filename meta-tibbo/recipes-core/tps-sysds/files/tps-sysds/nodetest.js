var ml = require( "@tibbo-tps/SysDs");
var c = require('constants');

var X = new ml.SysDs();
var r;

r = X.open_S();
if ( r < 0) {
  console.log( 'y0: '+r+'('+c.r+')');
  console.log( 'y1: '+X.get_errM());
  return( 1);  }

r = X.call_StartStop( "StartUnit", "zopa");
if ( r < 0) {
  console.log( 'y0: '+r+'('+c.r+')');
  console.log( 'y1: '+X.get_errM());
  return( 1);  }

 // read string reply
 r = X.Rs( path);
 console.log( 'Queued service job as ' + path);
