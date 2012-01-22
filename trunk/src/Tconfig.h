#ifndef _TCONFIG_H_
#define _TCONFIG_H_

enum {
    FF_CPU_MMX     =0x00000001,
    FF_CPU_MMXEXT  =0x00000002,
    FF_CPU_SSE     =0x00000004,
    FF_CPU_SSE2    =0x00000008,
    FF_CPU_SSE3    =0x00000010,
    FF_CPU_SSSE3   =0x00000020,
    FF_CPU_3DNOW   =0x00000040,
    FF_CPU_3DNOWEXT=0x00000080,
    FF_CPU_TSC     =0x00000100,
    FF_CPU_SSE41   =0x00000200,
    FF_CPU_SSE42   =0x00000400,
    FF_CPU_SSE4A   =0x00000800,
    FF_CPU_SSE5    =0x00001000,
};

#ifndef FF_CPU_ONLY

#include "Toptions.h"
#include "ffdshow_constants.h"

class ThtmlColors;
class Tdll;
struct Tconfig :public Toptions {
private:
    mutable char_t exeflnm[MAX_PATH];
    mutable unsigned int shellversion;
    int available_cpu_flags0,cpu_flags0;
    mutable ThtmlColors *htmlcolors;
    mutable bool is_WMEncEng, done_WMEncEng;
    mutable Tdll *kernel;
public:
    Tconfig(TintStrColl *Icoll);
    Tconfig(HINSTANCE hInst,int allowedCpuGFlags);
    ~Tconfig();
    void init1(HINSTANCE hi),init2(void),load(void),save(void);
    char_t pth[MAX_PATH],epth[MAX_PATH];
    bool savePth;
    const char_t *getExeflnm(void) const;
    static int available_cpu_flags,cpu_flags,lavc_cpu_flags; //suppose that CPU capabilities won't change per instance
    static void initCPU(int allowedCpuFlags);
    static DWORD getCPUcount(void);
    static int get_trayIconFullColorOS(void);
    static int cache_line;
    bool check(const char_t *dllname) const;
    int isDecoder[IDFF_MOVIE_MAX+1];
    unsigned int getShellVersion(void) const;
    ThtmlColors *getHtmlColors(void) const;
    bool is_WMEncEng_loaded(void) const;
};

#endif

#endif
