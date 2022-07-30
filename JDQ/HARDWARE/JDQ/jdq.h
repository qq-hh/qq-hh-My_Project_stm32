#ifndef __JDQ_H
#define __JDQ_H	 
#include "sys.h"

#define JDQ1 PBout(9)
#define JDQ2 PBout(12)
#define JDQ3 PBout(14)
#define JDQ4 PBout(13)


void relay_init(void);
void relay_on(void);
void relay_off(void);

#endif


