
#ifndef _OS400CONVERTCHAR_H
#define _OS400CONVERTCHAR_H

#include "Char16.h"
#include "String.h"

PEGASUS_NAMESPACE_BEGIN

extern char AtoEchars[256];
extern char EtoAchars[256];

void AtoE (char* ptr);

void AtoE (char* ptr, int size);

void AtoE (Char16 *ptr);

////////////////////////////////////////////
// Convert ASCII to EBCDIC.
// Note: the first size chars at ptr are changed.
////////////////////////////////////////////
void AtoE (Char16* ptr, int size);

//void AtoE (String &name);

void EtoA (char* ptr);

void EtoA (char* ptr, int size);

void EtoA (Char16 *ptr);

//void EtoA (String &name);

PEGASUS_NAMESPACE_END

#endif /* _OS400CONVERTCHAR_H */

