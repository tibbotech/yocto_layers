#include <endian.h>
#include <stdio.h>

int main() {
 printf("Byte Order: ");
 if (BYTE_ORDER == LITTLE_ENDIAN) printf("little");
 else {
   if (BYTE_ORDER == BIG_ENDIAN) printf("big");
   else printf("unknown");
 }
 printf(" endian.\n");
 return 0;
}
