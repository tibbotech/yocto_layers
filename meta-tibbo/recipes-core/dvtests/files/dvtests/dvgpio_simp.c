#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dv/CPin.h"

typedef struct {
 char pin_s[ 4];
 int pin_i;
} pin_t;

pin_t pins[] = {
 // south
 { "S19", 	4 },
 // west
 { "W9", 	22 },
 { "W10",	23 },
 { "W11",	24 },
 { "W12",	25 },
 { "W13",	26 },
 { "W14",	27 },
 { "W15",	28 },
 { "W16",	29 },
 // north
 { "N4",	66 },
 { "N5",	67 },
 { "N6",	68 },
 { "N7",	69 },
 { "N8",	70 },
 { "N9",	71 },
// { "N10", 72 },
// { "N11", 73 },
 { "N12",	74 },
 { "N13",	75 },
 { "N14",	76 },
 { "N15",	77 },
 { "N16",	78 },
 { "N17",	79 },
// { "N18", 100 },
// { "N19", 100 },
 // east
 { "E6", 	90 },
 { "E9",	93 },
 { "E10",	94 },
 { "E11",	95 },
};

void leg_m( CPin *_p) {
 int i = 0;
 for ( i = 0; i != -1; i++) {
    _p->W( ( i % 2));
if ( i % 100000 == 0) printf( "done 100 000\n");
 }
 return;  }

void leg_n( CPin *_p0, CPin *_p1) {
 int i = 0;
 for ( i = 0; i != -1; i++) {
    _p0->W( ( i % 2));
    _p1->W( ( i % 2));
if ( i % 100000 == 0) printf( "done 100 000\n");
 }
 return;  }

int main( int argc, char *argv[]) {
 if ( argc < 2) {
   printf( "%s <leg> [action] [val]\n", argv[ 0]);
   printf( "action:\tlist,state,read,write,in,out\n");
   printf( "\tlist - show free pins with nicknames\n");
   printf( "\texample: '%s list' - give list of free legs\n", argv[ 0]);
   printf( "\texample: '%s W15 state' - give state of West#15 (PA28) leg\n", argv[ 0]);
   printf( "\texample: '%s 28 state' - give state of West#15 (PA28) leg\n", argv[ 0]);
   printf( "\texample: '%s 28 read' - read West#15 (PA28) leg\n", argv[ 0]);
   printf( "\texample: '%s 28 readw' - read West#15 (PA28) leg in while(1)\n", argv[ 0]);
   printf( "\texample: '%s 28 out' - set state to OUT for West#15 (PA28) leg\n", argv[ 0]);
   printf( "\texample: '%s 28 write 1' - write 1 to West#15 (PA28) leg\n", argv[ 0]);
   printf( "\texample: '%s 28 write 0' - write 0 to West#15 (PA28) leg\n", argv[ 0]);
   printf( "\texample: '%s 28 in' - set state to IN for West#15 (PA28) leg\n", argv[ 0]);
   printf( "\texample: '%s 28 m' - masturbate with West#15 (PA28) leg\n", argv[ 0]);
   printf( "\texample: '%s 28 n 29' - masturbate with West#15 (PA28) leg\n", argv[ 0]);
   return( 1);  }
 const char *leg_s = argv[ 1];
 const char *act_s = ( argc > 2 ? argv[ 2] : "list");
 const char *val_s = ( argc > 3 ? argv[ 3] : "1");
 int i = 0;
 if ( strcmp( act_s, "list") == 0) {
   for ( i = 0; i < sizeof( pins)/sizeof( pin_t); i++) printf( "pin:%s\t(#%d)\n", pins[ i].pin_s, pins[ i].pin_i);
   exit( 0);
 }
 int leg_idi = atoi( leg_s);
 printf( "Leg %s translated to #%d\n", leg_s, leg_idi);
 CPin L;
 CPin L1;
 if ( L.init( leg_idi) < 0) exit( 1);
 // work
 if ( strcmp( act_s, "state") == 0) {
   printf( "state:%s\n", ( L.dir_get() == PIN_DIR_I ? "IN" : "OUT"));
 }
 if ( strcmp( act_s, "read") == 0) {
   printf( "read:%d\n", L.R());
   sleep( 5);
   printf( "read:%d\n", L.R());
 }
 if ( strcmp( act_s, "readw") == 0) {
   while ( 1) printf( "read:%d\n", L.R());
 }
 if ( strcmp( act_s, "write") == 0) {
   printf( "set OUT ok:%d\n", L.dir_set( PIN_DIR_O));
   printf( "write done:%d\n", L.W( atoi( val_s)));
 }
 if ( strcmp( act_s, "in") == 0) {
   printf( "set IN ok:%d\n", L.dir_set( PIN_DIR_I));
 }
 if ( strcmp( act_s, "out") == 0) {
   printf( "set OUT ok:%d\n", L.dir_set( PIN_DIR_O));
 }
 if ( strcmp( act_s, "m") == 0) {
   printf( "m start for %d\n", L.cur());
   leg_m( &L);
 }
 if ( strcmp( act_s, "n") == 0) {
   printf( "n start\n");
   if ( L1.init( atoi( argv[ 3])) < 0) exit( 1);
   printf( "n start for %d and %d\n", L.cur(), L1.cur());
   leg_n( &L, &L1);
 }
 return( 0);  }
