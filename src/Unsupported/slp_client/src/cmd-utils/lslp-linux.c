/*****************************************************************************
 *  Description:   
 *
 *  Originated: December 20, 2001
 *	Original Author: Mike Day md@soft-hackle.net
 *                                mdd@us.ibm.com
 *
 *  $Header: /cvs/MSB/pegasus/src/Unsupported/slp_client/src/cmd-utils/Attic/lslp-linux.c,v 1.1.1.1 2003/05/21 15:14:08 mday Exp $ 	                                                            
 *               					                    
 *  Copyright (c) 2001 - 2003  IBM                                          
 *  Copyright (c) 2000 - 2003 Michael Day                                    
 *                                                                           
 *  Permission is hereby granted, free of charge, to any person obtaining a  
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the     
 *  Software is furnished to do so, subject to the following conditions:       
 * 
 *  The above copyright notice and this permission notice shall be included in 
 *  all copies or substantial portions of the Software.
 * 
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/




#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <assert.h>
#include "lslp-linux.h"

void  num_to_ascii(uint32 val, int8 *buf, int32 radix, BOOL is_neg)
{
  int8 *p;
  int8 *firstdig;
  int8 temp;
  uint32 digval;

  assert(buf != NULL);
  p = buf;
  /* safeguard against a div by zero fault ! */
  if(val == 0){
    *p++ = '0';
    *p = 0x00;
    return;
  }

  /* default to a decimal radix */
  if (radix <= 0)
    radix = 10;

  /* negate two's complement, print a minus sign */
  if (is_neg == TRUE){
    *p++ = '-';
    val = (uint32)(-(int32)val);
  }

  firstdig = p;

  do {
    digval = (uint32) (val % radix);
    val /= radix;
    if(digval > 9)
      *p++ = (int8)(digval - 10 + 'a');
    else
      *p++ = (int8)(digval + '0');
  } while (val > 0);

  /* by getting the mod value before the div value, the digits are */
  /* reversed in the buffer */
  /* terminate the string */
  *p-- = '\0';
  do {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp;
    --p;
    ++firstdig;
  }while(firstdig < p);
  return;  
}


void  hug_num_to_ascii(uint64 val, int8 *buf, int32 radix, BOOL is_neg)
{
  int8 *p;
  int8 *firstdig;
  int8 temp;
  uint64 digval;

  assert(buf != NULL);
  p = buf;
  /* safeguard against a div by zero fault ! */
  if(val == 0){
    *p++ = '0';
    *p = 0x00;
    return;
  }

  /* default to a decimal radix */
  if (radix <= 0)
    radix = 10;

  /* negate two's complement, print a minus sign */
  if (is_neg == TRUE){
    *p++ = '-';
    val = (uint64)(-(int64)val);
  }

  firstdig = p;

  do {
    digval = (uint64)(val % radix);
    val /= radix;
    if(digval > 9)
      *p++ = (int8)(digval - 10 + 'a');
    else
      *p++ = (int8)(digval + '0');
  } while (val > 0);

  /* by getting the mod value before the div value, the digits are */
  /* reversed in the buffer */
  /* terminate the string */
  *p-- = '\0';
  do {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp;
    --p;
    ++firstdig;
  }while(firstdig < p);
  return;  
}


/* signal handler */
void _lslp_term(int sig) 
{
  static int dieNow;
  
  if(dieNow)
    raise(sig);  /* recurse into oblivion */
  dieNow = 1; 
  /* kill threads */
  signal(sig, SIG_DFL);
  raise(sig);
}
 

