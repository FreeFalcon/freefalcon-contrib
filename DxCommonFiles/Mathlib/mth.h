#pragma warning(disable : 4035)	// Retro 29Apr2004 - suppress 'No return value' warning
static inline int FloatToInt32(float x)
{
  __asm
  {
    fld dword ptr [x];
    fistp dword ptr [x];
    mov eax,dword ptr [x];
  }
}
#pragma warning(default : 4035)	// Retro 29Apr2004
