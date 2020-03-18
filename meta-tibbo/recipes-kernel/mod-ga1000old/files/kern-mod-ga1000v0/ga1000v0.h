#ifndef TPSGA1000V0_H
#define TPSGA1000V0_H

#define MNAME "tpsga1000v0"
#define M_LIC "GPL v2"
#define M_AUT "Dvorkin Dmitry dvorkin@tibbo.com"
#define M_NAM "Tibbo TPS GA1000v0 module"
#define M_ORG "Tibbo Tech."
#define M_CPR "(C) 2015-2016"


#include "ga1000v0_syshdrs.h"
//#include "ga1000v0_defs.h"

// gpio3.20
//#define TPSGA1000V0_GPIO_RESET 116
#define TPSGA1000V0_GPIO_RESET 13
// ??? extra ? gpio3.19
//#define TPSGA1000V0_GPIO_STRAP 115
// maybe chip select gpio0.06
//#define TPSGA1000V0_GPIO_STRAP 6
#define TPSGA1000V0_GPIO_STRAP 14

#endif // TPSGA1000V0_H
