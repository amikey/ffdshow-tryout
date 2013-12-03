; Requires Inno Setup Unicode: http://www.jrsoftware.org/isdl.php
; (Make sure you choose to install Inno Setup Preprocessor)
#if VER < EncodeVer(5,5,4)
  #error Update your Inno Setup version (5.5.4 or newer)
#endif
#ifndef UNICODE
  #error Use the Unicode Inno Setup
#endif


#define dummy Exec("update_version.bat","","",1,SW_HIDE)

#define VERSION_MAJOR 1
#define VERSION_MINOR 3
#include "..\..\src\svn_version.h"


; Build specific options
#define localize                  = True

#define include_xvidcore          = False
#define include_makeavis          = True
#define include_plugin_avisynth   = True
#define include_plugin_virtualdub = True
#define include_plugin_dscaler    = True
#define include_quicksync         = True

#define include_info_before       = False
#define include_gnu_license       = False
#define include_setup_icon        = False

#define sse_required              = False
#define sse2_required             = False

; Output settings
#define filename_suffix           = ''
#define outputdir                 = '.'

; Location of binaries
#define bindir                    = '..\'

; Custom builder preferences (uncomment one to enable, or define it through a command line parameter)
; example: ISCC.exe ffdshow_installer.iss /dPREF_XYZ
;#define PREF_CLSID
;#define PREF_CLSID_X64
;#define PREF_YAMAGATA
;#define PREF_XXL
;#define PREF_XXL_X64
;#define PREF_ALBAIN
;#define PREF_ALBAIN_x64
;#define PREF_XHMIKOSR_ICL
;#define PREF_EGUR
;#define PREF_EGUR_x64

#ifdef PREF_CLSID
  #define filename_suffix        = '_clsid'
  #define bindir                 = '..\x86'
  #define outputdir              = '..\..\..\'
#endif
#ifdef PREF_CLSID_X64
  #define is64bit                = True
  #define filename_suffix        = '_clsid_x64'
  #define bindir                 = '..\x64'
  #define outputdir              = '..\..\..\'
#endif
#ifdef PREF_YAMAGATA
  #define filename_suffix        = '_Q'
#endif
#ifdef PREF_XXL
  #define localize               = False
  #define include_info_before    = True
;  #define include_setup_icon     = True
  #define filename_suffix        = '_xxl'
#endif
#ifdef PREF_XXL_X64
  #define is64bit                = True
  #define include_info_before    = True
;  #define include_setup_icon     = True
  #define filename_suffix        = '_xxl_x64'
#endif
#ifdef PREF_ALBAIN
  #define sse_required           = True
  #define filename_suffix        = '_dbt_'
#endif
#ifdef PREF_ALBAIN_X64
  #define is64bit                = True
  #define filename_suffix        = '_dbt_x64'
#endif
#ifdef PREF_XHMIKOSR_ICL
  #define sse2_required          = True
  #define filename_suffix        = '_xhmikosr_icl12'
#endif
#ifdef PREF_EGUR
  #define sse2_required          = True
  #define filename_suffix        = '_egur'
#endif
#ifdef PREF_EGUR_x64
  #define is64bit                = True
  #define sse2_required          = True
  #define filename_suffix        = '_egur_x64'
#endif

; Compiler settings
#ifndef is64bit
#define is64bit = False
#endif

#if is64bit
  #define ff_sys = '{sys}'
  #define include_plugin_dscaler = False
  #define include_makeavis = False
#else
  #define ff_sys = '{syswow64}'
#endif


[Setup]
#if is64bit
ArchitecturesAllowed            = x64
ArchitecturesInstallIn64BitMode = x64
AppId                           = ffdshow64
DefaultGroupName                = ffdshow x64
AppVerName                      = ffdshow x64 v{#= VERSION_MAJOR}.{#= VERSION_MINOR}.{#= SVN_REVISION} [{#= BUILD_YEAR}-{#= BUILD_MONTH}-{#= BUILD_DAY}]
#else
AppId                           = ffdshow
DefaultGroupName                = ffdshow
AppVerName                      = ffdshow v{#= VERSION_MAJOR}.{#= VERSION_MINOR}.{#= SVN_REVISION} [{#= BUILD_YEAR}-{#= BUILD_MONTH}-{#= BUILD_DAY}]
#endif
AllowCancelDuringInstall        = no
AllowNoIcons                    = yes
AllowUNCPath                    = no
AppName                         = ffdshow
AppVersion                      = {#= VERSION_MAJOR}.{#= VERSION_MINOR}.{#= SVN_REVISION}.0
Compression                     = lzma/ultra
InternalCompressLevel           = ultra
SolidCompression                = True
DefaultDirName                  = {code:GetDefaultInstallDir|}
DirExistsWarning                = no
DisableDirPage                  = auto
DisableProgramGroupPage         = auto
MinVersion                      = 5.01SP2
OutputBaseFilename              = ffdshow_rev{#= SVN_REVISION}_{#= BUILD_YEAR}{#= BUILD_MONTH}{#= BUILD_DAY}{#= filename_suffix}
OutputDir                       = {#= outputdir}
PrivilegesRequired              = admin
ShowComponentSizes              = no
#if include_setup_icon
SetupIconFile                   = ..\..\src\FFd.ico
#endif
#if localize
ShowLanguageDialog              = yes
#else
ShowLanguageDialog              = no
#endif
ShowTasksTreeLines              = yes
UninstallDisplayIcon            = {app}\ffdshow.ax,9
UsePreviousTasks                = yes
VersionInfoCompany              = ffdshow
VersionInfoCopyright            = GNU
VersionInfoVersion              = {#= VERSION_MAJOR}.{#= VERSION_MINOR}.{#= SVN_REVISION}.0
WizardImageFile                 = MicrosoftModern01.bmp
WizardSmallImageFile            = SetupModernSmall26.bmp

[Languages]
  #if !include_gnu_license & !include_info_before
Name: "en";      MessagesFile: "compiler:Default.isl"
  #elif include_gnu_license & !include_info_before
Name: "en";      MessagesFile: "compiler:Default.isl"; LicenseFile: "license\gnu_license.txt";
  #elif !include_gnu_license & include_info_before
Name: "en";      MessagesFile: "compiler:Default.isl"; InfoBeforeFile: "infobefore.rtf";
  #else
Name: "en";      MessagesFile: "compiler:Default.isl"; LicenseFile: "license\gnu_license.txt"; InfoBeforeFile: "infobefore.rtf";
  #endif
#if localize
Name: "ca";      MessagesFile: "compiler:Languages\Catalan.isl"
  #if !include_gnu_license
Name: "cs";      MessagesFile: "compiler:Languages\Czech.isl"
  #else
Name: "cs";      MessagesFile: "compiler:Languages\Czech.isl"; LicenseFile: "license\copying.cs.txt"
  #endif
Name: "da";      MessagesFile: "compiler:Languages\Danish.isl"
  #if !include_gnu_license & !include_info_before
Name: "de";      MessagesFile: "compiler:Languages\German.isl"
  #elif include_gnu_license & !include_info_before
Name: "de";      MessagesFile: "compiler:Languages\German.isl"; LicenseFile: "license\copying.de.txt";
  #elif !include_gnu_license & include_info_before
Name: "de";      MessagesFile: "compiler:Languages\German.isl";                                        InfoBeforeFile: "infobefore.de.rtf";
  #else
Name: "de";      MessagesFile: "compiler:Languages\German.isl"; LicenseFile: "license\copying.de.txt"; InfoBeforeFile: "infobefore.de.rtf";
  #endif
Name: "es";      MessagesFile: "compiler:Languages\Spanish.isl"
Name: "eu";      MessagesFile:          "languages\Basque.isl"
Name: "fi";      MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr";      MessagesFile: "compiler:Languages\French.isl"
Name: "hu";      MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "it";      MessagesFile: "compiler:Languages\Italian.isl"
  #if !include_gnu_license
Name: "ja";      MessagesFile: "compiler:Languages\Japanese.isl"
  #else
Name: "ja";      MessagesFile: "compiler:Languages\Japanese.isl"; LicenseFile: "license\copying.ja.txt"
  #endif
Name: "nl";      MessagesFile: "compiler:Languages\Dutch.isl"
Name: "no";      MessagesFile: "compiler:Languages\Norwegian.isl"
  #if !include_gnu_license & !include_info_before
Name: "pl";      MessagesFile: "compiler:Languages\Polish.isl"
  #elif include_gnu_license & !include_info_before
Name: "pl";      MessagesFile: "compiler:Languages\Polish.isl"; LicenseFile: "license\copying.pl.txt";
  #elif !include_gnu_license & include_info_before
Name: "pl";      MessagesFile: "compiler:Languages\Polish.isl";                                        InfoBeforeFile: "infobefore.pl.rtf";
  #else
Name: "pl";      MessagesFile: "compiler:Languages\Polish.isl"; LicenseFile: "license\copying.pl.txt"; InfoBeforeFile: "infobefore.pl.rtf";
  #endif
Name: "pt_BR";   MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "pt_PT";   MessagesFile: "compiler:Languages\Portuguese.isl"
  #if !include_gnu_license
Name: "ru";      MessagesFile: "compiler:Languages\Russian.isl"
  #else
Name: "ru";      MessagesFile: "compiler:Languages\Russian.isl"; LicenseFile: "license\copying.ru.txt"
  #endif
  #if !include_gnu_license
Name: "sk";      MessagesFile:          "languages\Slovak.isl"
  #else
Name: "sk";      MessagesFile: "compiler:Languages\Slovak.isl"; LicenseFile: "license\copying.sk.txt"
  #endif
Name: "sl";      MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "uk";      MessagesFile: "compiler:Languages\Ukrainian.isl"
Name: "zh_Hant"; MessagesFile:          "languages\ChineseTrad.isl"
Name: "zh_Hans"; MessagesFile:          "languages\ChineseSimp.isl"
#endif

[Types]
Name: "Normal"; Description: "Normal"; Flags: iscustom

[Components]
Name: "directshow";           Description: DirectShow:;
Name: "directshow\video";     Description: "{cm:comp_video}";     Types: Normal;
Name: "directshow\audio";     Description: "{cm:comp_audio}";     Types: Normal;
Name: "directshow\dxva";      Description: "{cm:comp_dxva}";
Name: "directshow\videoproc"; Description: "{cm:comp_videoproc}"; Types: Normal;
Name: "directshow\audioproc"; Description: "{cm:comp_audioproc}"; Types: Normal;
#if is64bit
Name: "vfw";                  Description: "{cm:comp_vfwInterface}";         Types: Normal; OnlyBelowVersion: 6.2
Name: "vfw";                  Description: "{cm:comp_vfwInterface_win8x64}";                MinVersion: 6.2
#else
Name: "vfw";                  Description: "{cm:comp_vfwInterface}";         Types: Normal;
#endif
#if include_plugin_avisynth | include_plugin_virtualdub | include_plugin_dscaler
Name: "plugins";              Description: "{cm:comp_appPlugins}";
  #if include_plugin_avisynth
Name: "plugins\avisynth";     Description: "AviSynth";
  #endif
  #if include_plugin_virtualdub
Name: "plugins\virtualdub";   Description: "VirtualDub";
  #endif
  #if include_plugin_dscaler
Name: "plugins\dscaler";      Description: "DScaler";
  #endif
#endif
#if include_makeavis
Name: "makeavis";             Description: "{cm:comp_makeAvis}";
#endif

[Tasks]
Name: "resetsettings";           Description: "{cm:tsk_resetSettings}";                                                                 Check: NOT IsUpdate; GroupDescription: "{cm:tsk_settings}"
Name: "video";                   Description: "{cm:tsk_videoFormatsSelect}";  Flags: unchecked;           Components: directshow\video; Check: NOT IsUpdate; GroupDescription: "{cm:tsk_videoFormats}"
Name: "video\h264";              Description: "H.264 / AVC";
Name: "video\h264\libavcodec";   Description: "libavcodec";                   Flags: unchecked exclusive
#if include_quicksync
Name: "video\h264\quicksync";    Description: "Intel QuickSync";              Flags: unchecked exclusive; Check: IsQSCapableIntelCPU; MinVersion: 6.0;
#endif
Name: "video\divx";              Description: "DivX";
Name: "video\xvid";              Description: "Xvid";
Name: "video\mpeg4";             Description: "{cm:tsk_genericMpeg4}";
Name: "video\flv";               Description: "FLV1, FLV4";
Name: "video\vp6";               Description: "VP6";
Name: "video\h263";              Description: "H.263(+)";
Name: "video\mpeg1";             Description: "MPEG-1";                       Flags: unchecked dontinheritcheck
Name: "video\mpeg1\libmpeg2";    Description: "libmpeg2";                     Flags: unchecked exclusive
Name: "video\mpeg1\libavcodec";  Description: "libavcodec";                   Flags: unchecked exclusive
Name: "video\mpeg2";             Description: "MPEG-2";                       Flags: unchecked
Name: "video\mpeg2\libmpeg2";    Description: "libmpeg2";                     Flags: unchecked exclusive
Name: "video\mpeg2\libavcodec";  Description: "libavcodec";                   Flags: unchecked exclusive
#if include_quicksync
Name: "video\mpeg2\quicksync";   Description: "Intel QuickSync";              Flags: unchecked exclusive; Check: IsQSCapableIntelCPU; MinVersion: 6.0;
#endif
Name: "video\huffyuv";           Description: "Huffyuv";
Name: "video\qt";                Description: "SVQ1, SVQ3, RPZA, QT RLE";
Name: "video\vc1";               Description: "VC-1";                         Flags: unchecked dontinheritcheck
Name: "video\vc1\wmv9";          Description: "wmv9";                         Flags: unchecked exclusive
Name: "video\vc1\libavcodec";    Description: "libavcodec";                   Flags: unchecked exclusive
#if include_quicksync
Name: "video\vc1\quicksync";     Description: "Intel QuickSync";              Flags: unchecked exclusive; Check: IsQSCapableIntelCPU; MinVersion: 6.0;
#endif
Name: "video\wmv1";              Description: "WMV1";                         Flags: unchecked dontinheritcheck
Name: "video\wmv2";              Description: "WMV2";                         Flags: unchecked dontinheritcheck
Name: "video\wmv3";              Description: "WMV3";                         Flags: unchecked dontinheritcheck
Name: "video\wmv3\wmv9";         Description: "wmv9";                         Flags: unchecked exclusive
Name: "video\wmv3\libavcodec";   Description: "libavcodec";                   Flags: unchecked exclusive
#if include_quicksync
Name: "video\wmv3\quicksync";    Description: "Intel QuickSync";              Flags: unchecked exclusive
#endif
Name: "video\wvp2";              Description: "WMVP, WVP2";                   Flags: unchecked dontinheritcheck
Name: "video\mss2";              Description: "MSS1, MSS2";                   Flags: unchecked dontinheritcheck
Name: "video\dvsd";              Description: "DV";                           Flags: unchecked dontinheritcheck
Name: "video\techsmith";         Description: "Techsmith";                    Flags: unchecked
Name: "video\fraps";             Description: "Fraps";                        Flags: unchecked
Name: "video\msvidc";            Description: "MS Video 1";                   Flags: unchecked
Name: "video\msrle";             Description: "MS RLE";                       Flags: unchecked
Name: "video\cinepak";           Description: "Cinepak";                      Flags: unchecked
Name: "video\mjpeg";             Description: "MJPEG";
Name: "video\truemotion";        Description: "TrueMotion";
Name: "video\camstudio";         Description: "Camstudio";
Name: "video\indeo";             Description: "Indeo 2/3";
Name: "video\other1";            Description: "Theora, VP3, VP5";
Name: "video\other2";            Description: "CorePNG, H.261, MSZH, ZLIB";
Name: "video\other3";            Description: "8BPS, ASV1/2, CYUV, LOCO, QPEG, VCR1, WNV1"; Flags: unchecked
Name: "video\other4";            Description: "AASC, Ultimotion, VIXL, ZMBV"; Flags: unchecked
Name: "video\rawv";              Description: "{cm:tsk_rawVideo}";            Flags: unchecked dontinheritcheck
Name: "audio";                   Description: "{cm:tsk_audioFormatsSelect}";  Flags: unchecked; Components: directshow\audio; Check: NOT IsUpdate; GroupDescription: "{cm:tsk_audioFormats}"
Name: "audio\mp2";               Description: "MP1, MP2";
Name: "audio\mp3";               Description: "MP3";
Name: "audio\aac";               Description: "AAC";
Name: "audio\ac3";               Description: "AC3";
Name: "audio\dts";               Description: "DTS";
Name: "audio\dts\libdts";        Description: "libdts";                       Flags:           exclusive
Name: "audio\dts\libavcodec";    Description: "libavcodec";                   Flags: unchecked exclusive
Name: "audio\eac3";              Description: "E-AC3 (Dolby Digital Plus)";
Name: "audio\truehd";            Description: "Dolby TrueHD";
Name: "audio\vorbis";            Description: "Vorbis";
Name: "audio\flac";              Description: "FLAC";
Name: "audio\mlp";               Description: "MLP";
Name: "audio\lpcm";              Description: "LPCM";
Name: "audio\tta";               Description: "True Audio";
Name: "audio\amr";               Description: "AMR";
Name: "audio\qt";                Description: "QDM2, MACE";
Name: "audio\msadpcm";           Description: "MS ADPCM";                     Flags: unchecked
Name: "audio\imaadpcm";          Description: "IMA ADPCM";                    Flags: unchecked
Name: "audio\msgsm";             Description: "MS GSM";                       Flags: unchecked
Name: "audio\law";               Description: "Alaw, u-Law";                  Flags: unchecked
Name: "audio\truespeech";        Description: "Truespeech";                   Flags: unchecked
Name: "audio\rawa";              Description: "{cm:tsk_rawAudio}";            Flags: unchecked dontinheritcheck
Name: "filter";                  Description: "{cm:tsk_filtersSelect}";       Flags: unchecked;                  Components: directshow\video directshow\audio; Check: NOT IsUpdate; GroupDescription: "{cm:tsk_filters}"
Name: "filter\passthroughac3";   Description: "{cm:tsk_passthroughac3}";      Flags: unchecked;                  Components: directshow\audio
Name: "filter\passthroughdts";   Description: "{cm:tsk_passthroughdts}";      Flags: unchecked;                  Components: directshow\audio
Name: "filter\normalize";        Description: "{cm:tsk_volumeNorm}";          Flags: unchecked;                  Components: directshow\audio
Name: "filter\subtitles";        Description: "{cm:tsk_subtitles}";           Flags: unchecked;                  Components: directshow\video
#ifndef PREF_YAMAGATA
Name: "skiph264inloop";          Description: "{cm:tsk_skipInloop}";          Flags: unchecked;                  Components: directshow\video;                  Check: NOT IsUpdate; GroupDescription: "{cm:tsk_tweaks}"
#endif
Name: "whitelist";               Description: "{cm:tsk_whitelist}";           Flags: unchecked;                  Components: directshow\video directshow\audio; Check: NOT IsUpdate; GroupDescription: "{cm:tsk_compatibilityManager}"
Name: "whitelist\prompt";        Description: "{cm:tsk_whitelistPrompt}";     Flags: unchecked

[Icons]
#if is64bit
Name: {group}\{cm:shrt_audioConfig} x64;     Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configureAudio";     WorkingDir: {app};       IconFilename: {app}\ffdshow.ax; IconIndex: 4;  Components: directshow\audio
Name: {group}\{cm:shrt_videoConfig} x64;     Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configure";          WorkingDir: {app};       IconFilename: {app}\ffdshow.ax; IconIndex: 3;  Components: directshow\video
Name: {group}\{cm:shrt_videoDXVAConfig} x64; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configureDXVA";      WorkingDir: {app};       IconFilename: {app}\ffdshow.ax; IconIndex: 10; Components: directshow\dxva
Name: {group}\{cm:shrt_vfwConfig} x64;       Filename: {#= ff_sys}\rundll32.exe; Parameters: """{#= ff_sys}\ff_vfw.dll"",configureVFW"; WorkingDir: {#= ff_sys}; IconFilename: {app}\ffdshow.ax; IconIndex: 5;  Components: vfw; OnlyBelowVersion: 6.2;
#else
Name: {group}\{cm:shrt_audioConfig};     Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configureAudio";     WorkingDir: {app};       IconFilename: {app}\ffdshow.ax; IconIndex: 4;  Components: directshow\audio
Name: {group}\{cm:shrt_videoConfig};     Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configure";          WorkingDir: {app};       IconFilename: {app}\ffdshow.ax; IconIndex: 3;  Components: directshow\video
Name: {group}\{cm:shrt_videoDXVAConfig}; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configureDXVA";      WorkingDir: {app};       IconFilename: {app}\ffdshow.ax; IconIndex: 10; Components: directshow\dxva
Name: {group}\{cm:shrt_vfwConfig};       Filename: {#= ff_sys}\rundll32.exe; Parameters: """{#= ff_sys}\ff_vfw.dll"",configureVFW"; WorkingDir: {#= ff_sys}; IconFilename: {app}\ffdshow.ax; IconIndex: 5;  Components: vfw
#endif
#if include_makeavis
Name: {group}\makeAVIS;            Filename: {app}\makeAVIS.exe; Components: makeavis
#endif
Name: {group}\{cm:shrt_uninstall}; Filename: {uninstallexe}
Name: {group}\{cm:shrt_homepage};  Filename: http://ffdshow-tryout.sourceforge.net/; IconFilename: {app}\ffdshow.ax; IconIndex: 9;

[Files]
; For speaker config and QuickSync detection
Source: "ffSpkCfg.dll"; Flags: dontcopy

Source: "{#= bindir}\ffmpeg.dll";                 DestDir: "{app}";                                                     Flags: ignoreversion
Source: "{#= bindir}\ff_liba52.dll";              DestDir: "{app}";                       Components: directshow\audio; Flags: ignoreversion
Source: "{#= bindir}\ff_libdts.dll";              DestDir: "{app}";                       Components: directshow\audio; Flags: ignoreversion
Source: "{#= bindir}\ff_libfaad2.dll";            DestDir: "{app}";                       Components: directshow\audio; Flags: ignoreversion
Source: "{#= bindir}\ff_libmad.dll";              DestDir: "{app}";                       Components: directshow\audio; Flags: ignoreversion
Source: "{#= bindir}\ff_unrar.dll";               DestDir: "{app}";                       Components: directshow\video directshow\videoproc; Flags: ignoreversion
Source: "{#= bindir}\ff_samplerate.dll";          DestDir: "{app}";                       Components: directshow\audio directshow\audioproc; Flags: ignoreversion

#if include_xvidcore
Source: "{#= bindir}\xvidcore.dll";               DestDir: "{app}";                       Components: directshow\video vfw; Flags: ignoreversion
#endif
Source: "{#= bindir}\ff_kernelDeint.dll";         DestDir: "{app}";                       Components: directshow\video directshow\videoproc; Flags: ignoreversion; Check: Is_SSE_Supported;
Source: "{#= bindir}\TomsMoComp_ff.dll";          DestDir: "{app}";                       Components: directshow\video directshow\videoproc; Flags: ignoreversion
Source: "{#= bindir}\libmpeg2_ff.dll";            DestDir: "{app}";                       Components: directshow\video;                      Flags: ignoreversion restartreplace uninsrestartdelete

Source: "{#= bindir}\ffdshow.ax";                 DestDir: "{app}";                                                     Flags: ignoreversion restartreplace uninsrestartdelete regserver noregerror
Source: "..\manifest\ffdshow.ax.manifest";        DestDir: "{app}";                                                     Flags: ignoreversion restartreplace uninsrestartdelete; OnlyBelowVersion: 6.0;

Source: "{#= bindir}\ff_wmv9.dll";                DestDir: "{app}";                       Components: directshow\video vfw; Flags: ignoreversion
#if include_quicksync
Source: "{#= bindir}\IntelQuickSyncDecoder.dll";  DestDir: "{app}";                       Components: directshow\video vfw; Flags: ignoreversion; Check: IsQSCapableIntelCPU; MinVersion: 6.0;
#endif

Source: "{#= bindir}\ff_vfw.dll";                 DestDir: "{sys}";                       Components: vfw;                Flags: ignoreversion restartreplace uninsrestartdelete
Source: "..\manifest\ff_vfw.dll.manifest";        DestDir: "{sys}";                       Components: vfw;                Flags: ignoreversion restartreplace uninsrestartdelete; OnlyBelowVersion: 6.0;

#if include_plugin_avisynth
Source: "..\ffavisynth.avsi";                     DestDir: "{code:GetAviSynthPluginDir}"; Components: plugins\avisynth;   Flags: ignoreversion restartreplace uninsrestartdelete
Source: "{#= bindir}\ffavisynth.dll";             DestDir: "{code:GetAviSynthPluginDir}"; Components: plugins\avisynth;   Flags: ignoreversion restartreplace uninsrestartdelete
#endif
#if include_plugin_virtualdub
Source: "{#= bindir}\ffvdub.vdf";                 DestDir: "{code:GetVdubPluginDir}";     Components: plugins\virtualdub; Flags: ignoreversion restartreplace uninsrestartdelete
#endif
#if include_plugin_dscaler
Source: "{#= bindir}\FLT_ffdshow.dll";            DestDir: "{code:GetDScalerDir|}";       Components: plugins\dscaler;    Flags: ignoreversion restartreplace uninsrestartdelete
#endif

#if include_makeavis
Source: "{#= bindir}\makeAVIS.exe";               DestDir: "{app}";                       Components: makeavis;           Flags: ignoreversion restartreplace uninsrestartdelete
Source: "..\manifest\makeAVIS.exe.manifest";      DestDir: "{app}";                       Components: makeavis;           Flags: ignoreversion restartreplace uninsrestartdelete; OnlyBelowVersion: 6.0;
Source: "{#= bindir}\ff_acm.acm";                 DestDir: "{sys}";                       Components: makeavis;           Flags: ignoreversion restartreplace uninsrestartdelete
#endif

Source: "..\languages\*.*";                       DestDir: "{app}\languages";                                             Flags: ignoreversion
Source: "..\openIE.js";                           DestDir: "{app}";                                                       Flags: ignoreversion
Source: "license\gnu_license.txt";                DestDir: "{app}";                                                       Flags: ignoreversion
Source: "license\Boost_Software_License_1.0.txt"; DestDir: "{app}";                                                       Flags: ignoreversion

[InstallDelete]
; Private assemblies
Type: files; Name: "{app}\msvcr80.dll";
Type: files; Name: "{app}\microsoft.vc80.crt.manifest";
Type: files; Name: "{group}\*.*";
; Files not included in this build
#if !include_xvidcore
Type: files; Name: "{app}\xvidcore.dll";
#endif
; Outdated files
Type: files; Name: "{app}\languages\ffdshow.1029.cz";
Type: files; Name: "{app}\languages\ffdshow.1041.jp";
Type: files; Name: "{app}\languages\ffdshow.1053.se";
Type: files; Name: "{app}\libavcodec.dll";
Type: files; Name: "{app}\libmplayer.dll";
Type: files; Name: "{app}\ff_tremor.dll";
Type: files; Name: "{app}\ff_x264.dll";
Type: files; Name: "{app}\ffmpegmt.dll";
#if is64bit
Type: files; Name: "{sys}\ff_vfw.dll"; Components: NOT vfw; MinVersion: 6.2
Type: files; Name: "{sys}\ff_acm.acm";                      MinVersion: 6.2
#endif

[Registry]
#if is64bit
  #define ff_reg_base = 'Software\GNU\ffdshow64'
#else
  #define ff_reg_base = 'Software\GNU\ffdshow'
#endif

; Cleanup of settings
Root: HKCU; Subkey: "Software\GNU";               Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "{#= ff_reg_base}";           Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";     Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_audio_raw"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";      Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_enc";       Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_raw";       Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw";       Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "Software\GNU";               Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "{#= ff_reg_base}";           Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "{#= ff_reg_base}_audio";     Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "{#= ff_reg_base}_vfw";       Flags: dontcreatekey uninsdeletekey

; Reset settings
Root: HKCU; Subkey: "{#= ff_reg_base}";           Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";     Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_audio_raw"; Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";      Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_enc";       Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_raw";       Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw";       Flags: deletekey; Tasks: resetsettings
Root: HKLM; Subkey: "{#= ff_reg_base}";           Flags: deletekey; Tasks: resetsettings
Root: HKLM; Subkey: "{#= ff_reg_base}_audio";     Flags: deletekey; Tasks: resetsettings
Root: HKLM; Subkey: "{#= ff_reg_base}_vfw";       Flags: deletekey; Tasks: resetsettings

; Path
Root: HKLM; Subkey: "{#= ff_reg_base}"; ValueType: string; ValueName: "pth";           ValueData: "{app}";
Root: HKLM; Subkey: "{#= ff_reg_base}";                    ValueName: "pthPriority";                                                                                               Flags: deletevalue
#if include_plugin_avisynth
Root: HKLM; SubKey: "{#= ff_reg_base}"; ValueType: string; ValueName: "pthAvisynth";   ValueData: "{code:GetAviSynthPluginDir}";                   Components: plugins\avisynth;   Flags: uninsclearvalue
#endif
#if include_plugin_virtualdub
Root: HKLM; SubKey: "{#= ff_reg_base}"; ValueType: string; ValueName: "pthVirtualDub"; ValueData: "{code:GetVdubPluginDir}";                       Components: plugins\virtualdub; Flags: uninsclearvalue
#endif
#if include_plugin_dscaler
Root: HKLM; SubKey: "{#= ff_reg_base}"; ValueType: string; ValueName: "dscalerPth";    ValueData: "{code:GetDScalerDir|}";                         Components: plugins\dscaler;    Flags: uninsclearvalue
#endif

; Version info
Root: HKLM; Subkey: "{#= ff_reg_base}"; ValueType: dword;  ValueName: "revision";      ValueData: "{#= SVN_REVISION}";
Root: HKLM; Subkey: "{#= ff_reg_base}"; ValueType: dword;  ValueName: "builddate";     ValueData: "{#= BUILD_YEAR}{#= BUILD_MONTH}{#= BUILD_DAY}";

; Language
#if localize
Root: HKLM; Subkey: "{#= ff_reg_base}"; ValueType: string; ValueName: "lang";          ValueData: "{cm:langid}";
#endif

; Register VfW interface
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\drivers.desc";      ValueType: string; ValueName: "ff_vfw.dll";   ValueData: "ffdshow video encoder"; Components: vfw;      Flags: uninsdeletevalue
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32";         ValueType: string; ValueName: "VIDC.FFDS";    ValueData: "ff_vfw.dll";            Components: vfw;      Flags: uninsdeletevalue
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";                                                                                    Components: vfw;      Flags: uninsdeletekey
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";  ValueType: string; ValueName: "Description";  ValueData: "ffdshow video encoder"; Components: vfw;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";  ValueType: string; ValueName: "Driver";       ValueData: "ff_vfw.dll";            Components: vfw;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";  ValueType: string; ValueName: "FriendlyName"; ValueData: "ffdshow video encoder"; Components: vfw;

#if include_makeavis
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\drivers.desc";      ValueType: string; ValueName: "ff_acm.acm";   ValueData: "ffdshow ACM codec";     Components: makeavis; Flags: uninsdeletevalue
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32";         ValueType: string; ValueName: "msacm.avis";   ValueData: "ff_acm.acm";            Components: makeavis; Flags: uninsdeletevalue
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis";                                                                                   Components: makeavis; Flags: uninsdeletekey
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis"; ValueType: string; ValueName: "Description";  ValueData: "ffdshow ACM codec";     Components: makeavis;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis"; ValueType: string; ValueName: "Driver";       ValueData: "ff_acm.acm";            Components: makeavis;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis"; ValueType: string; ValueName: "FriendlyName"; ValueData: "ffdshow ACM codec";     Components: makeavis;
#endif

; Remove old incompatible codecs on Windows 8 x64
#if is64bit
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\drivers.desc";     ValueName: "ff_vfw.dll"; Flags: deletevalue; Components: NOT vfw; MinVersion: 6.2
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32";        ValueName: "VIDC.FFDS";  Flags: deletevalue; Components: NOT vfw; MinVersion: 6.2
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";                          Flags: deletekey;   Components: NOT vfw; MinVersion: 6.2

Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\drivers.desc";     ValueName: "ff_acm.acm"; Flags: deletevalue;
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32";        ValueName: "msacm.avis"; Flags: deletevalue;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis";                         Flags: deletekey;
#endif

; Recommended settings
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "postprocH264mode";     ValueData: "0";                  Components: directshow\video;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "resizeMethod";         ValueData: "9";                  Components: directshow\video;     Flags: createvalueifdoesntexist

#ifndef PREF_YAMAGATA
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "fastH264";             ValueData: "2";                  Components: directshow\video; Tasks:     skiph264inloop
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "fastH264";             ValueData: "0";                  Components: directshow\video; Tasks: NOT skiph264inloop;        Check: NOT IsUpdate
#endif

Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "subTextpin";           ValueData: "1";                  Components: directshow\video;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "subTextpinSSA";        ValueData: "1";                  Components: directshow\video;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "subIsExpand";          ValueData: "0";                  Components: directshow\video;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "isSubtitles";          ValueData: "1";                  Components: directshow\video; Tasks:     filter\subtitles
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "isSubtitles";          ValueData: "0";                  Components: directshow\video; Tasks: NOT filter\subtitles;      Check: NOT IsUpdate

Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "mixerNormalizeMatrix"; ValueData: "0";                  Components: directshow\audio;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "isvolume";             ValueData: "1";                  Components: directshow\audio; Tasks:     filter\normalize
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "volNormalize";         ValueData: "1";                  Components: directshow\audio; Tasks:     filter\normalize
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "volNormalize";         ValueData: "0";                  Components: directshow\audio; Tasks: NOT filter\normalize;      Check: NOT IsUpdate

Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "ismixer";              ValueData: "{code:GetIsMixer}";  Components: directshow\audio
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "mixerOut";             ValueData: "{code:GetMixerOut}"; Components: directshow\audio

; Blacklist
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isBlacklist";          ValueData: "1";                  Components: directshow\video;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isBlacklist";          ValueData: "1";                  Components: directshow\audio;     Flags: createvalueifdoesntexist

; Compatibility list
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isWhitelist";          ValueData: "1";                  Components: directshow\video; Tasks:     whitelist;
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isWhitelist";          ValueData: "0";                  Components: directshow\video; Tasks: NOT whitelist;             Check: NOT IsUpdate
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isWhitelist";          ValueData: "1";                  Components: directshow\audio; Tasks:     whitelist
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isWhitelist";          ValueData: "0";                  Components: directshow\audio; Tasks: NOT whitelist;             Check: NOT IsUpdate

Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isCompMgr";            ValueData: "1";                  Components: directshow\video; Tasks: whitelist AND     whitelist\prompt
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isCompMgr";            ValueData: "0";                  Components: directshow\video; Tasks: whitelist AND NOT whitelist\prompt
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isCompMgr";            ValueData: "1";                  Components: directshow\audio; Tasks: whitelist AND     whitelist\prompt
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isCompMgr";            ValueData: "0";                  Components: directshow\audio; Tasks: whitelist AND NOT whitelist\prompt

; VFW settings
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw\default";   ValueType: dword;  ValueName: "needOutcspsFix";       ValueData: "0";                  Components: vfw;
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw\default";   ValueType: dword;  ValueName: "outNV12";              ValueData: "0";                  Components: vfw;
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw\default";   ValueType: dword;  ValueName: "outP010";              ValueData: "0";                  Components: vfw;
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw\default";   ValueType: dword;  ValueName: "outP016";              ValueData: "0";                  Components: vfw;
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw\default";   ValueType: dword;  ValueName: "outP210";              ValueData: "0";                  Components: vfw;
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw\default";   ValueType: dword;  ValueName: "outP216";              ValueData: "0";                  Components: vfw;
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw\default";   ValueType: dword;  ValueName: "outY416";              ValueData: "0";                  Components: vfw;

; filter registration flags
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noVideoDecoder"; ValueData: "0";                  Components:     directshow\video
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noVideoDecoder"; ValueData: "1";                  Components: NOT directshow\video
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noAudioDecoder"; ValueData: "0";                  Components:     directshow\audio
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noAudioDecoder"; ValueData: "1";                  Components: NOT directshow\audio
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noDxvaDecoder";  ValueData: "0";                  Components:     directshow\dxva
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noDxvaDecoder";  ValueData: "1";                  Components: NOT directshow\dxva
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noVideoProc";    ValueData: "0";                  Components:     directshow\videoproc
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noVideoProc";    ValueData: "1";                  Components: NOT directshow\videoproc
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noSubFilter";    ValueData: "0";                  Components:     directshow\videoproc
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noSubFilter";    ValueData: "1";                  Components: NOT directshow\videoproc
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noAudioProc";    ValueData: "0";                  Components:     directshow\audioproc
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noAudioProc";    ValueData: "1";                  Components: NOT directshow\audioproc
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noVFW";          ValueData: "0";                  Components:     vfw
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noVFW";          ValueData: "1";                  Components: NOT vfw

; Remove filters if unchecked
Root: HKCR; Subkey: CLSID\{{083863F1-70DE-11D0-BD40-00A0C911CE86}\Instance\{{04FE9017-F873-410E-871E-AB91661A4EF7}; Flags: deletekey; Components: NOT directshow\video
Root: HKCR; Subkey: CLSID\{{083863F1-70DE-11D0-BD40-00A0C911CE86}\Instance\{{0F40E1E5-4F79-4988-B1A9-CC98794E6B55}; Flags: deletekey; Components: NOT directshow\audio
Root: HKCR; Subkey: CLSID\{{083863F1-70DE-11D0-BD40-00A0C911CE86}\Instance\{{0B0EFF97-C750-462C-9488-B10E7D87F1A6}; Flags: deletekey; Components: NOT directshow\dxva
Root: HKCR; Subkey: CLSID\{{083863F1-70DE-11D0-BD40-00A0C911CE86}\Instance\{{0B390488-D80F-4A68-8408-48DC199F0E97}; Flags: deletekey; Components: NOT directshow\videoproc
Root: HKCR; Subkey: CLSID\{{083863F1-70DE-11D0-BD40-00A0C911CE86}\Instance\{{B86F6BEE-E7C0-4D03-8D52-5B4430CF6C88}; Flags: deletekey; Components: NOT directshow\audioproc

Root: HKCR; Subkey: CLSID\{{04FE9017-F873-410E-871E-AB91661A4EF7}; Flags: deletekey; Components: NOT directshow\video
Root: HKCR; Subkey: CLSID\{{9A98ADCC-C6A4-449E-A8B1-0363673D9F8A}; Flags: deletekey; Components: NOT directshow\video
Root: HKCR; Subkey: CLSID\{{0F40E1E5-4F79-4988-B1A9-CC98794E6B55}; Flags: deletekey; Components: NOT directshow\audio
Root: HKCR; Subkey: CLSID\{{007FC171-01AA-4B3A-B2DB-062DEE815A1E}; Flags: deletekey; Components: NOT directshow\audio
Root: HKCR; Subkey: CLSID\{{0B0EFF97-C750-462C-9488-B10E7D87F1A6}; Flags: deletekey; Components: NOT directshow\dxva
Root: HKCR; Subkey: CLSID\{{545A00C2-FCCC-40B3-9310-2C36AE64B0DD}; Flags: deletekey; Components: NOT directshow\dxva
Root: HKCR; Subkey: CLSID\{{0B390488-D80F-4A68-8408-48DC199F0E97}; Flags: deletekey; Components: NOT directshow\videoproc
Root: HKCR; Subkey: CLSID\{{DBF9000E-F08C-4858-B769-C914A0FBB1D7}; Flags: deletekey; Components: NOT directshow\videoproc
Root: HKCR; Subkey: CLSID\{{B86F6BEE-E7C0-4D03-8D52-5B4430CF6C88}; Flags: deletekey; Components: NOT directshow\audioproc
Root: HKCR; Subkey: CLSID\{{3E3ECA90-4D6A-4344-98C3-1BB95BF24038}; Flags: deletekey; Components: NOT directshow\audioproc

; DXVA Compatibility list
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";          ValueType: dword;  ValueName: "isCompMgr";            ValueData: "0";                  Components: directshow\dxva; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";          ValueType: dword;  ValueName: "isBlacklist";          ValueData: "0";                  Components: directshow\dxva; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";          ValueType: dword;  ValueName: "isWhitelist";          ValueData: "1";                  Components: directshow\dxva; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";          ValueType: string;  ValueName: "Whitelist";           ValueData: "bsplayer.exe;coreplayer.exe;ehshell.exe;gom.exe;graphedit.exe;graphedt.exe;graphstudio.exe;graphstudio64.exe;graphstudionext.exe;graphstudionext64.exe;kmplayer.exe;mpc-hc.exe;mpc-hc64.exe;mplayerc.exe;wmplayer.exe;zplayer.exe;"; Components: directshow\dxva; Flags: createvalueifdoesntexist

; Registry keys for the audio/video formats:
#include "reg_formats.iss"

Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "passthroughAC3";       ValueData: "1";                  Components: directshow\audio; Tasks:     filter\passthroughac3
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "passthroughAC3";       ValueData: "0";                  Components: directshow\audio; Tasks: NOT filter\passthroughac3; Check: NOT IsUpdate
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "passthroughDTS";       ValueData: "1";                  Components: directshow\audio; Tasks:     filter\passthroughdts
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "passthroughDTS";       ValueData: "0";                  Components: directshow\audio; Tasks: NOT filter\passthroughdts; Check: NOT IsUpdate

[Run]
Description: "{cm:run_audioConfig}";     Filename: "{#= ff_sys}\rundll32.exe"; Parameters: """{app}\ffdshow.ax"",configureAudio";     WorkingDir: "{app}";       Components: directshow\audio; Flags: postinstall nowait unchecked
Description: "{cm:run_videoConfig}";     Filename: "{#= ff_sys}\rundll32.exe"; Parameters: """{app}\ffdshow.ax"",configure";          WorkingDir: "{app}";       Components: directshow\video; Flags: postinstall nowait unchecked
Description: "{cm:run_videoDXVAConfig}"; Filename: "{#= ff_sys}\rundll32.exe"; Parameters: """{app}\ffdshow.ax"",configureDXVA";      WorkingDir: "{app}";       Components: directshow\dxva;  Flags: postinstall nowait unchecked
Description: "{cm:run_vfwConfig}";       Filename: "{#= ff_sys}\rundll32.exe"; Parameters: """{#= ff_sys}\ff_vfw.dll"",configureVFW"; WorkingDir: "{#= ff_sys}"; Components: vfw;              Flags: postinstall nowait unchecked

; All custom strings in the installer:
#include "custom_messages.iss"

[Code]
// Global vars
var
  reg_mixerOut: Cardinal;
  reg_ismixer: Cardinal;
  SpeakerPage: TInputOptionWizardPage;
  is8DisableMixer: Boolean;

#if include_plugin_avisynth
var
  avisynthplugindir: String;

function GetAviSynthPluginDir(dummy: String): String;
begin
  if Length(avisynthplugindir) = 0 then begin
    if NOT RegQueryStringValue(HKLM, 'SOFTWARE\AviSynth', 'plugindir2_5', avisynthplugindir) OR NOT DirExists(avisynthplugindir) then begin
      if NOT RegQueryStringValue(HKLM, 'SOFTWARE\AviSynth', 'plugindir', avisynthplugindir) OR NOT DirExists(avisynthplugindir) then begin
        avisynthplugindir := ExpandConstant('{app}');
      end
    end
  end;

  Result := avisynthplugindir;
end;
#endif

#if include_plugin_dscaler
var
  dscalerdir: String;

function GetDScalerDir(dummy: String): String;
var
  proglist: Array of String;
  i: Integer;
  temp : String;
begin
  if Length(dscalerdir) = 0 then begin
    dscalerdir := ExpandConstant('{app}');
    if RegGetSubkeyNames(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall', proglist) then begin
      for i:=0 to (GetArrayLength(proglist) - 1) do begin
        if Pos('dscaler', Lowercase(proglist[i])) > 0 then begin
          if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\' + proglist[i], 'Inno Setup: App Path', temp) AND DirExists(temp) then begin
            dscalerdir := temp;
            Break;
          end
        end
      end
    end
  end;
  Result := dscalerdir;
end;
#endif

#if include_plugin_virtualdub
var
  VdubDirPage: TInputDirWizardPage;

function GetVdubPluginDir(dummy: String): String;
begin
  Result := VdubDirPage.Values[0];
end;
#endif

function GetDefaultInstallDir(dummy: String): String;
begin
  if NOT RegQueryStringValue(HKLM, '{#= ff_reg_base}', 'pth', Result) OR (Length(Result) = 0) OR NOT DirExists(Result) then begin
    Result := ExpandConstant('{pf}\ffdshow');
  end
end;

function IsUpdate(): Boolean;
var
  sPrevPath: String;
begin
  sPrevPath := WizardForm.PrevAppDir;
  Result := (sPrevPath <> '');
end;

#if !is64bit
procedure RemoveBuildUsingNSIS();
var
  regval: String;
  resultCode: Integer;
begin
  if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow', 'UninstallString', regval) then begin
    MsgBox(CustomMessage('msg_uninstallFirst'), mbInformation, mb_ok);
    if NOT Exec('>', regval, '', SW_SHOW, ewWaitUntilTerminated, resultCode) then begin
      MsgBox(SysErrorMessage(resultCode), mbError, MB_OK);
    end
  end
end;
#endif


// CPU detection functions
#include "innosetup_cpu_detection.iss"


function InitializeSetup(): Boolean;
begin
  Result := True;

  // CPU capabilities
  DetectCPU;

  if GetCPULevel < 6 then begin
    Result := False;
    MsgBox(CustomMessage('unsupported_cpu'), mbError, MB_OK);
  end;
  #if sse2_required
  if Result AND NOT Is_SSE2_Supported() then begin
    Result := False;
    MsgBox(CustomMessage('simd_msg_sse2'), mbError, MB_OK);
  end;
  #elif sse_required
  if Result AND NOT Is_SSE_Supported() then begin
    Result := False;
    MsgBox(CustomMessage('simd_msg_sse'), mbError, MB_OK);
  end;
  #endif

  #if !is64bit
  if Result then begin
    RemoveBuildUsingNSIS;
  end;
  #endif
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    if IsTaskSelected('resetsettings') OR NOT RegValueExists(HKCU, '{#= ff_reg_base}\default', 'threadsnum') then begin
      RegWriteDwordValue(HKCU, '{#= ff_reg_base}\default', 'threadsnum', GetNumberOfCores);
    end
  end
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
  if CurPageID = wpSelectComponents then begin
    if NOT WizardSilent AND (WizardSelectedComponents(False) = '') then begin
      msgbox(ExpandConstant('{cm:comp_msg_selectOneComp}'), mbError, mb_ok);
      Result := False;
    end;
  end;
end;

function InitializeUninstall(): Boolean;
begin
  Result := True;

  #if !is64bit
  // Also uninstall ancient versions when uninstalling.
  RemoveBuildUsingNSIS;
  #endif
end;

// #define DSSPEAKER_DIRECTOUT         0x00000000
// #define DSSPEAKER_HEADPHONE         0x00000001
// #define DSSPEAKER_MONO              0x00000002
// #define DSSPEAKER_QUAD              0x00000003
// #define DSSPEAKER_STEREO            0x00000004
// #define DSSPEAKER_SURROUND          0x00000005
// #define DSSPEAKER_5POINT1           0x00000006
// #define DSSPEAKER_7POINT1           0x00000007

function ffSpkCfg(): Integer;
external 'getSpeakerConfig@files:ffSpkCfg.dll stdcall delayload';

function getSpeakerConfig(): Integer;
begin
  try
    Result := ffSpkCfg();
  except
    Result := 4; // DSSPEAKER_STEREO
  end;
end;

function GetMixerOut(dummy: String): String;
begin
  if      SpeakerPage.Values[0] = True then
    Result := '0'
  else if SpeakerPage.Values[1] = True then
    Result := '17'
  else if SpeakerPage.Values[2] = True then
    Result := '1'
  else if SpeakerPage.Values[3] = True then
    Result := '2'
  else if SpeakerPage.Values[4] = True then
    Result := '12'
  else if SpeakerPage.Values[5] = True then
    Result := '6'
  else if SpeakerPage.Values[6] = True then
    Result := '13'
  else if SpeakerPage.Values[7] = True then
    Result := '24'
  else if SpeakerPage.Values[8] = True then
    Result := IntToStr(reg_mixerOut);
  RegWriteDWordValue(HKLM, '{#= ff_reg_base}_audio', 'isSpkCfg', 1);
end;

function GetIsMixer(dummy: String): String;
begin
  Result := '1';
  if (is8DisableMixer = True) and (SpeakerPage.Values[8] = True) then
    Result := '0';
end;

function ffRegReadDWordHKCU(regKeyName: String; regValName: String; defaultValue: Cardinal): Cardinal;
begin
  if NOT RegQueryDwordValue(HKCU, regKeyName, regValName, Result) then
    Result := defaultValue;
end;

procedure InitializeWizard;
var
  systemSpeakerConfig: Integer;
  reg_isSpkCfg: Cardinal;
  isMajorType: Boolean;
begin
  { Create custom pages }

  // Speaker setup

  is8DisableMixer := False;
  SpeakerPage := CreateInputOptionPage(wpSelectTasks,
    CustomMessage('spk_Label1'),
    CustomMessage('spk_Label2'),
    CustomMessage('spk_Label3'),
    True, False);
  SpeakerPage.Add('1.0 (' + CustomMessage('spk_mono')      + ')');                                          // 0
  SpeakerPage.Add('2.0 (' + CustomMessage('spk_headPhone') + ')');                                          // 17
  SpeakerPage.Add('2.0 (' + CustomMessage('spk_stereo')    + ')');                                          // 1
  SpeakerPage.Add('3.0 (' + CustomMessage('spk_front_3ch') + ')');                                          // 2
  SpeakerPage.Add('4.1 (' + CustomMessage('spk_quadro')    + ' + ' + CustomMessage('spk_subwoofer') + ')'); // 12
  SpeakerPage.Add('5.0 (' + CustomMessage('spk_5ch')       + ')');                                          // 6
  SpeakerPage.Add('5.1 (' + CustomMessage('spk_5ch')       + ' + ' + CustomMessage('spk_subwoofer') + ')'); // 13
  SpeakerPage.Add('7.1 (' + CustomMessage('spk_7ch')       + ' + ' + CustomMessage('spk_subwoofer') + ')'); // 24
  if  RegQueryDWordValue(HKCU, '{#= ff_reg_base}_audio\default', 'mixerOut', reg_mixerOut)
  and RegQueryDWordValue(HKCU, '{#= ff_reg_base}_audio\default', 'ismixer' , reg_ismixer)
  and RegQueryDWordValue(HKLM, '{#= ff_reg_base}_audio'        , 'isSpkCfg', reg_isSpkCfg) then
  begin
    if reg_ismixer = 1 then begin
      isMajorType := True;
      if      reg_mixerOut = 0 then
        SpeakerPage.Values[0] := True
      else if reg_mixerOut = 17 then
        SpeakerPage.Values[1] := True
      else if reg_mixerOut = 1 then
        SpeakerPage.Values[2] := True
      else if reg_mixerOut = 2 then
        SpeakerPage.Values[3] := True
      else if reg_mixerOut = 12 then
        SpeakerPage.Values[4] := True
      else if reg_mixerOut = 6 then
        SpeakerPage.Values[5] := True
      else if reg_mixerOut = 13 then
        SpeakerPage.Values[6] := True
      else if reg_mixerOut = 24 then
        SpeakerPage.Values[7] := True
      else begin
        if reg_mixerOut = 3 then
          SpeakerPage.Add('2+1 ('   + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_rear_1ch')  + ')')
        else if reg_mixerOut = 4 then
          SpeakerPage.Add('3+1 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_rear_1ch')  + ')')
        else if reg_mixerOut = 5 then
          SpeakerPage.Add('4.0 ('   + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_rear_2ch')  + ')')
        else if reg_mixerOut = 7 then
          SpeakerPage.Add('1.1 ('   + CustomMessage('spk_mono')      + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 8 then
          SpeakerPage.Add('2.1 ('   + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 9 then
          SpeakerPage.Add('3.1 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 10 then
          SpeakerPage.Add('2+1.1 (' + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_rear_1ch')  + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 11 then
          SpeakerPage.Add('3+1.1 (' + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_rear_1ch')  + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 14 then
          SpeakerPage.Add(            CustomMessage('spk_dolby1'))
        else if reg_mixerOut = 19 then
          SpeakerPage.Add(            CustomMessage('spk_dolby1')    + ' + ' + CustomMessage('spk_subwoofer'))
        else if reg_mixerOut = 15 then
          SpeakerPage.Add(            CustomMessage('spk_dolby2'))
        else if reg_mixerOut = 20 then
          SpeakerPage.Add(            CustomMessage('spk_dolby2')    + ' + ' + CustomMessage('spk_subwoofer'))
        else if reg_mixerOut = 16 then
          SpeakerPage.Add(            CustomMessage('spk_sameAsInput'))
        else if reg_mixerOut = 18 then
          SpeakerPage.Add(            CustomMessage('spk_hrtf'))
        else if reg_mixerOut = 21 then
          SpeakerPage.Add('6.0 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_side_2ch')  + ' + ' + CustomMessage('spk_rear_1ch')  + ')')
        else if reg_mixerOut = 22 then
          SpeakerPage.Add('6.1 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_side_2ch')  + ' + ' + CustomMessage('spk_rear_1ch')  + ' + ' + CustomMessage('spk_subwoofer') + ')')
        else if reg_mixerOut = 23 then
          SpeakerPage.Add('7.0 ('   + CustomMessage('spk_front_3ch') + ' + ' + CustomMessage('spk_side_2ch')  + ' + ' + CustomMessage('spk_rear_2ch')  + ')')
        else if reg_mixerOut > 24 then
          SpeakerPage.Add(           CustomMessage('spk_unknownSpk'));
        SpeakerPage.Values[8] := True;
        isMajorType := False;
      end;
      if isMajorType then begin
       SpeakerPage.Add(CustomMessage('spk_disableMixer'));
       is8DisableMixer := True;
      end;
    end else begin
      SpeakerPage.Add(CustomMessage('spk_disableMixer'));
      is8DisableMixer := True;
      SpeakerPage.Values[8] := True;
    end;
  end else begin
    reg_ismixer := 1;
    reg_mixerOut := 1;
    is8DisableMixer := True;
    systemSpeakerConfig := getSpeakerConfig(); // read the setting of control panel(requires directX 8)
    SpeakerPage.Values[2] := True;             // default 2.0 (Stereo)

    if      systemSpeakerConfig = 2 then // DSSPEAKER_MONO
      SpeakerPage.Values[0] := True
    else if systemSpeakerConfig = 1 then // DSSPEAKER_HEADPHONE
      SpeakerPage.Values[1] := True
    else if systemSpeakerConfig = 3 then // DSSPEAKER_QUAD
      begin
       SpeakerPage.Add('4.0 (' + CustomMessage('spk_front_2ch') + ' + ' + CustomMessage('spk_rear_2ch') + ')');
       SpeakerPage.Values[8] := True;
       isMajorType := False;
       is8DisableMixer := False;
       reg_mixerOut := 5;
      end
    else if systemSpeakerConfig = 5 then // DSSPEAKER_SURROUND
      SpeakerPage.Values[5] := True
    else if systemSpeakerConfig = 6 then // DSSPEAKER_5POINT1
      SpeakerPage.Values[6] := True
    else if systemSpeakerConfig = 7 then // DSSPEAKER_7POINT1
      SpeakerPage.Values[6] := True
    else if systemSpeakerConfig = 8 then // 7.1ch hometheater
      SpeakerPage.Values[7] := True;
    if is8DisableMixer then
     SpeakerPage.Add(CustomMessage('spk_disableMixer'));
  end;

#if include_plugin_virtualdub
  // VirtualDub plugin install directory setting
  VdubDirPage := CreateInputDirPage(SpeakerPage.ID,
    FmtMessage(CustomMessage('plg_Label1'), ['VirtualDub']),
    FmtMessage(CustomMessage('plg_Label2'), ['VirtualDub']),
    FmtMessage(CustomMessage('plg_Label3'), ['VirtualDub']),
    False, '');
  VdubDirPage.Add('');
#endif
end;

function ShouldSkipPage(PageID: Integer): Boolean;
var
  regval: String;
begin
  Result := False;
  if PageID = wpLicense then begin
    Result := IsUpdate;
  end;

  #if include_plugin_virtualdub
  if PageID = VdubDirPage.ID then begin
    if IsComponentSelected('plugins\virtualdub') then begin
      if VdubDirPage.Values[0] = '' then begin
        if RegQueryStringValue(HKLM, '{#= ff_reg_base}', 'pthVirtualDub', regval)
        and not (regval = ExpandConstant('{app}')) and not (regval = '') then
          VdubDirPage.Values[0] := regval
        else if FileOrDirExists(ExpandConstant('{pf}\VirtualDub\plugins')) then
            VdubDirPage.Values[0] := ExpandConstant('{pf}\VirtualDub\plugins')
        else if FileOrDirExists(ExpandConstant('{sd}\VirtualDub\plugins')) then
            VdubDirPage.Values[0] := ExpandConstant('{sd}\VirtualDub\plugins')
        else
          VdubDirPage.Values[0] := ExpandConstant('{app}');
      end;
    end
    else begin
      Result := True;
    end;
  end;
  #endif
end;
