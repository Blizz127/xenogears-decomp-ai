/*
 * Throwaway world-loop capture bridge.  PsyCross exposes the screenshot
 * helper from C++ with its ABI-mangled name; keep the experiment's call site
 * in the C port sources without adding a vendored-source dependency.
 */
#include <stddef.h>

extern void PsyX_TakeScreenshotPath(const char* path)
    __asm__("_Z23PsyX_TakeScreenshotPathPKc");

void PsyX_TakeScreenshotPath_C(const char* path)
{
    PsyX_TakeScreenshotPath(path);
}
