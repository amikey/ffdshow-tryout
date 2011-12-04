; Requires Inno Setup Unicode: http://www.jrsoftware.org/isdl.php
; (Make sure you choose to install Inno Setup Preprocessor)
#if VER < 0x05040200
  #error Update your Inno Setup version
#endif
#ifndef UNICODE
  #error Use the Unicode Inno Setup
#endif


#define dummy Exec("update_version.bat","","",1,SW_HIDE)

#define ISPP_IS_BUGGY
#include "..\..\src\version.h"


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
;#define PREF_CLSID_ICL
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
#ifdef PREF_CLSID_ICL
  #define sse_required           = True
  #define filename_suffix        = '_clsid_icl10'
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
MinVersion                      = 0,5.01SP2
OutputBaseFilename              = ffdshow_rev{#= SVN_REVISION}_{#= BUILD_YEAR}{#= BUILD_MONTH}{#= BUILD_DAY}{#= filename_suffix}
OutputDir                       = {#= outputdir}
PrivilegesRequired              = admin
#if include_setup_icon
SetupIconFile                   = ..\..\src\FFd.ico
#endif
#if localize
ShowLanguageDialog              = yes
#else
ShowLanguageDialog              = no
#endif
ShowTasksTreeLines              = yes
ShowUndisplayableLanguages      = no
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
  #if localize
Name: "ca";      MessagesFile: "compiler:Languages\Catalan.isl"
Name: "cs";      MessagesFile: "compiler:Languages\Czech.isl"
Name: "da";      MessagesFile: "compiler:Languages\Danish.isl"
Name: "de";      MessagesFile: "compiler:Languages\German.isl"
Name: "es";      MessagesFile: "compiler:Languages\Spanish.isl"
Name: "eu";      MessagesFile: "compiler:Languages\Basque.isl"
Name: "fi";      MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr";      MessagesFile: "compiler:Languages\French.isl"
Name: "hu";      MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "it";      MessagesFile: "compiler:Languages\Italian.isl"
Name: "ja";      MessagesFile: "compiler:Languages\Japanese.isl"
Name: "nl";      MessagesFile: "compiler:Languages\Dutch.isl"
Name: "no";      MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "pl";      MessagesFile: "compiler:Languages\Polish.isl"
Name: "pt_BR";   MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "pt_PT";   MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ru";      MessagesFile: "compiler:Languages\Russian.isl"
Name: "sk";      MessagesFile: "compiler:Languages\Slovak.isl"
Name: "sl";      MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "zh_Hant"; MessagesFile:          "languages\ChineseTrad.isl"
Name: "zh_Hans"; MessagesFile:          "languages\ChineseSimp.isl"
  #endif
#elif include_gnu_license & !include_info_before
Name: "en";      MessagesFile: "compiler:Default.isl";               LicenseFile: "license\gnu_license.txt"
  #if localize
Name: "ca";      MessagesFile: "compiler:Languages\Catalan.isl"
Name: "cs";      MessagesFile: "compiler:Languages\Czech.isl";       LicenseFile: "license\copying.cs.txt"
Name: "da";      MessagesFile: "compiler:Languages\Danish.isl"
Name: "de";      MessagesFile: "compiler:Languages\German.isl";      LicenseFile: "license\copying.de.txt"
Name: "es";      MessagesFile: "compiler:Languages\Spanish.isl"
Name: "eu";      MessagesFile: "compiler:Languages\Basque.isl"
Name: "fi";      MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr";      MessagesFile: "compiler:Languages\French.isl"
Name: "hu";      MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "it";      MessagesFile: "compiler:Languages\Italian.isl"
Name: "ja";      MessagesFile: "compiler:Languages\Japanese.isl";    LicenseFile: "license\copying.ja.txt"
Name: "nl";      MessagesFile: "compiler:Languages\Dutch.isl"
Name: "no";      MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "pl";      MessagesFile: "compiler:Languages\Polish.isl";      LicenseFile: "license\copying.pl.txt"
Name: "pt_BR";   MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "pt_PT";   MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ru";      MessagesFile: "compiler:Languages\Russian.isl";     LicenseFile: "license\copying.ru.txt"
Name: "sk";      MessagesFile: "compiler:Languages\Slovak.isl";      LicenseFile: "license\copying.sk.txt"
Name: "sl";      MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "zh_Hant"; MessagesFile:          "languages\ChineseTrad.isl"
Name: "zh_Hans"; MessagesFile:          "languages\ChineseSimp.isl"
  #endif
#elif !include_gnu_license & include_info_before
Name: "en";      MessagesFile: "compiler:Default.isl";               InfoBeforeFile: "infobefore.rtf"
  #if localize
Name: "ca";      MessagesFile: "compiler:Languages\Catalan.isl"
Name: "cs";      MessagesFile: "compiler:Languages\Czech.isl"
Name: "da";      MessagesFile: "compiler:Languages\Danish.isl"
Name: "de";      MessagesFile: "compiler:Languages\German.isl";      InfoBeforeFile: "infobefore\infobefore.de.rtf"
Name: "es";      MessagesFile: "compiler:Languages\Spanish.isl"
Name: "eu";      MessagesFile: "compiler:Languages\Basque.isl"
Name: "fi";      MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr";      MessagesFile: "compiler:Languages\French.isl"
Name: "hu";      MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "it";      MessagesFile: "compiler:Languages\Italian.isl"
Name: "ja";      MessagesFile: "compiler:Languages\Japanese.isl"
Name: "nl";      MessagesFile: "compiler:Languages\Dutch.isl"
Name: "no";      MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "pl";      MessagesFile: "compiler:Languages\Polish.isl";      InfoBeforeFile: "infobefore\infobefore.pl.rtf"
Name: "pt_BR";   MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "pt_PT";   MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ru";      MessagesFile: "compiler:Languages\Russian.isl"
Name: "sk";      MessagesFile: "compiler:Languages\Slovak.isl"
Name: "sl";      MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "zh_Hant"; MessagesFile:          "languages\ChineseTrad.isl"
Name: "zh_Hans"; MessagesFile:          "languages\ChineseSimp.isl"
  #endif
#else
Name: "en";      MessagesFile: "compiler:Default.isl";               LicenseFile: "license\gnu_license.txt";  InfoBeforeFile: "infobefore\infobefore.rtf"
  #if localize
Name: "ca";      MessagesFile: "compiler:Languages\Catalan.isl"
Name: "cs";      MessagesFile: "compiler:Languages\Czech.isl";       LicenseFile: "license\copying.cs.txt"
Name: "da";      MessagesFile: "compiler:Languages\Danish.isl"
Name: "de";      MessagesFile: "compiler:Languages\German.isl";      LicenseFile: "license\copying.de.txt";   InfoBeforeFile: "infobefore\infobefore.de.rtf"
Name: "es";      MessagesFile: "compiler:Languages\Spanish.isl"
Name: "eu";      MessagesFile: "compiler:Languages\Basque.isl"
Name: "fi";      MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr";      MessagesFile: "compiler:Languages\French.isl"
Name: "hu";      MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "it";      MessagesFile: "compiler:Languages\Italian.isl"
Name: "ja";      MessagesFile: "compiler:Languages\Japanese.isl";    LicenseFile: "license\copying.ja.txt"
Name: "nl";      MessagesFile: "compiler:Languages\Dutch.isl"
Name: "no";      MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "pl";      MessagesFile: "compiler:Languages\Polish.isl";      LicenseFile: "license\copying.pl.txt";   InfoBeforeFile: "infobefore\infobefore.pl.rtf"
Name: "pt_BR";   MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "pt_PT";   MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ru";      MessagesFile: "compiler:Languages\Russian.isl";     LicenseFile: "license\copying.ru.txt"
Name: "sk";      MessagesFile: "compiler:Languages\Slovak.isl";      LicenseFile: "license\copying.sk.txt"
Name: "sl";      MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "zh_Hant"; MessagesFile:          "languages\ChineseTrad.isl"
Name: "zh_Hans"; MessagesFile:          "languages\ChineseSimp.isl"
  #endif
#endif

[Types]
Name: "Normal"; Description: "Normal"; Flags: iscustom

[Components]
Name: "ffdshow";                    Description: "{cm:comp_ffdshowds}";    Types: Normal; Flags: fixed
Name: "ffdshow\dxva";               Description: "{cm:comp_dxvaDecoder}"
Name: "ffdshow\vfw";                Description: "{cm:comp_vfwInterface}"; Types: Normal
#if include_makeavis
Name: "ffdshow\makeavis";           Description: "{cm:comp_makeAvis}";         Flags: dontinheritcheck
#endif
#if include_plugin_avisynth | include_plugin_virtualdub | include_plugin_dscaler
Name: "ffdshow\plugins";            Description: "{cm:comp_appPlugins}";       Flags: dontinheritcheck
  #if include_plugin_avisynth
Name: "ffdshow\plugins\avisynth";   Description: "AviSynth"
  #endif
  #if include_plugin_virtualdub
Name: "ffdshow\plugins\virtualdub"; Description: "VirtualDub"
  #endif
  #if include_plugin_dscaler
Name: "ffdshow\plugins\dscaler";    Description: "DScaler"
  #endif
#endif

[Tasks]
Name: "resetsettings";           Description: "{cm:tsk_resetSettings}";           Components: ffdshow;                                    Check: NOT IsUpdate; GroupDescription: "{cm:tsk_settings}"
Name: "video";                   Description: "{cm:tsk_videoFormatsSelect}";      Components: ffdshow; Flags: unchecked;                  Check: NOT IsUpdate; GroupDescription: "{cm:tsk_videoFormats}"
Name: "video\h264";              Description: "H.264 / AVC";                      Components: ffdshow
Name: "video\h264\libavcodec";   Description: "libavcodec";                       Components: ffdshow; Flags: unchecked exclusive
#if include_quicksync
Name: "video\h264\quicksync";    Description: "Intel QuickSync";                  Components: ffdshow; Flags: unchecked exclusive
#endif
Name: "video\divx";              Description: "DivX";                             Components: ffdshow
Name: "video\xvid";              Description: "Xvid";                             Components: ffdshow
Name: "video\mpeg4";             Description: "{cm:tsk_genericMpeg4}";            Components: ffdshow
Name: "video\flv";               Description: "FLV1, FLV4";                       Components: ffdshow
Name: "video\vp6";               Description: "VP6";                              Components: ffdshow
Name: "video\h263";              Description: "H.263(+)";                         Components: ffdshow
Name: "video\mpeg1";             Description: "MPEG-1";                           Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "video\mpeg1\libmpeg2";    Description: "libmpeg2";                         Components: ffdshow; Flags: unchecked exclusive
Name: "video\mpeg1\libavcodec";  Description: "libavcodec";                       Components: ffdshow; Flags: unchecked exclusive
Name: "video\mpeg2";             Description: "MPEG-2";                           Components: ffdshow; Flags: unchecked
Name: "video\mpeg2\libmpeg2";    Description: "libmpeg2";                         Components: ffdshow; Flags: unchecked exclusive
Name: "video\mpeg2\libavcodec";  Description: "libavcodec";                       Components: ffdshow; Flags: unchecked exclusive
#if include_quicksync
Name: "video\mpeg2\quicksync";   Description: "Intel QuickSync";                  Components: ffdshow; Flags: unchecked exclusive
#endif
Name: "video\huffyuv";           Description: "Huffyuv";                          Components: ffdshow
Name: "video\qt";                Description: "SVQ1, SVQ3, RPZA, QT RLE";         Components: ffdshow
Name: "video\vc1";               Description: "VC-1";                             Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "video\vc1\wmv9";          Description: "wmv9";                             Components: ffdshow; Flags: unchecked exclusive
Name: "video\vc1\libavcodec";    Description: "libavcodec";                       Components: ffdshow; Flags: unchecked exclusive
#if include_quicksync
Name: "video\vc1\quicksync";     Description: "Intel QuickSync";                  Components: ffdshow; Flags: unchecked exclusive
#endif
Name: "video\wmv1";              Description: "WMV1";                             Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "video\wmv2";              Description: "WMV2";                             Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "video\wmv3";              Description: "WMV3";                             Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "video\wmv3\wmv9";         Description: "wmv9";                             Components: ffdshow; Flags: unchecked exclusive
Name: "video\wmv3\libavcodec";   Description: "libavcodec";                       Components: ffdshow; Flags: unchecked exclusive
#if include_quicksync
Name: "video\wmv3\quicksync";    Description: "Intel QuickSync";                  Components: ffdshow; Flags: unchecked exclusive
#endif
Name: "video\wvp2";              Description: "WMVP, WVP2";                       Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "video\mss2";              Description: "MSS1, MSS2";                       Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "video\dvsd";              Description: "DV";                               Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "video\techsmith";         Description: "Techsmith";                        Components: ffdshow; Flags: unchecked
Name: "video\fraps";             Description: "Fraps";                            Components: ffdshow; Flags: unchecked
Name: "video\msvidc";            Description: "MS Video 1";                       Components: ffdshow; Flags: unchecked
Name: "video\msrle";             Description: "MS RLE";                           Components: ffdshow; Flags: unchecked
Name: "video\cinepak";           Description: "Cinepak";                          Components: ffdshow; Flags: unchecked
Name: "video\mjpeg";             Description: "MJPEG";                            Components: ffdshow
Name: "video\truemotion";        Description: "TrueMotion";                       Components: ffdshow
Name: "video\camstudio";         Description: "Camstudio";                        Components: ffdshow
Name: "video\indeo";             Description: "Indeo 2/3";                        Components: ffdshow
Name: "video\other1";            Description: "Theora, VP3, VP5";                 Components: ffdshow
Name: "video\other2";            Description: "CorePNG, H.261, MSZH, ZLIB";       Components: ffdshow
Name: "video\other3";            Description: "8BPS, ASV1/2, CYUV, LOCO, QPEG, VCR1, WNV1"; Components: ffdshow; Flags: unchecked
Name: "video\other4";            Description: "AASC, Ultimotion, VIXL, ZMBV";     Components: ffdshow; Flags: unchecked
Name: "video\rawv";              Description: "{cm:tsk_rawVideo}";                Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "audio";                   Description: "{cm:tsk_audioFormatsSelect}";      Components: ffdshow; Flags: unchecked;                  Check: NOT IsUpdate; GroupDescription: "{cm:tsk_audioFormats}"
Name: "audio\mp2";               Description: "MP1, MP2";                         Components: ffdshow;
Name: "audio\mp3";               Description: "MP3";                              Components: ffdshow
Name: "audio\aac";               Description: "AAC";                              Components: ffdshow
Name: "audio\ac3";               Description: "AC3";                              Components: ffdshow
Name: "audio\ac3\liba52";        Description: "liba52";                           Components: ffdshow; Flags:           exclusive
Name: "audio\ac3\libavcodec";    Description: "libavcodec";                       Components: ffdshow; Flags: unchecked exclusive
Name: "audio\dts";               Description: "DTS";                              Components: ffdshow
Name: "audio\dts\libdts";        Description: "libdts";                           Components: ffdshow; Flags:           exclusive
Name: "audio\dts\libavcodec";    Description: "libavcodec";                       Components: ffdshow; Flags: unchecked exclusive
Name: "audio\eac3";              Description: "E-AC3 (Dolby Digital Plus)";       Components: ffdshow
Name: "audio\truehd";            Description: "Dolby TrueHD";                     Components: ffdshow
Name: "audio\vorbis";            Description: "Vorbis";                           Components: ffdshow
Name: "audio\flac";              Description: "FLAC";                             Components: ffdshow
Name: "audio\mlp";               Description: "MLP";                              Components: ffdshow
Name: "audio\lpcm";              Description: "LPCM";                             Components: ffdshow
Name: "audio\tta";               Description: "True Audio";                       Components: ffdshow
Name: "audio\amr";               Description: "AMR";                              Components: ffdshow
Name: "audio\qt";                Description: "QDM2, MACE";                       Components: ffdshow
Name: "audio\msadpcm";           Description: "MS ADPCM";                         Components: ffdshow; Flags: unchecked
Name: "audio\imaadpcm";          Description: "IMA ADPCM";                        Components: ffdshow; Flags: unchecked
Name: "audio\msgsm";             Description: "MS GSM";                           Components: ffdshow; Flags: unchecked
Name: "audio\law";               Description: "Alaw, u-Law";                      Components: ffdshow; Flags: unchecked
Name: "audio\truespeech";        Description: "Truespeech";                       Components: ffdshow; Flags: unchecked
Name: "audio\rawa";              Description: "{cm:tsk_rawAudio}";                Components: ffdshow; Flags: unchecked dontinheritcheck
Name: "filter";                  Description: "{cm:tsk_filtersSelect}";           Components: ffdshow; Flags: unchecked;                  Check: NOT IsUpdate; GroupDescription: "{cm:tsk_filters}"
Name: "filter\passthroughac3";   Description: "{cm:tsk_passthroughac3}";          Components: ffdshow; Flags: unchecked
Name: "filter\passthroughdts";   Description: "{cm:tsk_passthroughdts}";          Components: ffdshow; Flags: unchecked
Name: "filter\normalize";        Description: "{cm:tsk_volumeNorm}";              Components: ffdshow; Flags: unchecked
Name: "filter\subtitles";        Description: "{cm:tsk_subtitles}";               Components: ffdshow; Flags: unchecked
#ifndef PREF_YAMAGATA
Name: "skiph264inloop";          Description: "{cm:tsk_skipInloop}";              Components: ffdshow; Flags: unchecked;                  Check: NOT IsUpdate; GroupDescription: "{cm:tsk_tweaks}"
#endif
Name: "whitelist";               Description: "{cm:tsk_whitelist}";               Components: ffdshow; Flags: unchecked;                  Check: NOT IsUpdate; GroupDescription: "{cm:tsk_compatibilityManager}"
Name: "whitelist\prompt";        Description: "{cm:tsk_whitelistPrompt}";         Components: ffdshow; Flags: unchecked

[Icons]
#if is64bit
Name: {group}\{cm:shrt_audioConfig} x64; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configureAudio"; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 4; Components: ffdshow
Name: {group}\{cm:shrt_videoConfig} x64; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configure"; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 3; Components: ffdshow
Name: {group}\{cm:shrt_videoDXVAConfig} x64; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configureDXVA"; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 10; Components: ffdshow\dxva
Name: {group}\{cm:shrt_vfwConfig} x64; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{#= ff_sys}\ff_vfw.dll"",configureVFW"; WorkingDir: {#= ff_sys}; IconFilename: {app}\ffdshow.ax; IconIndex: 5; Components: ffdshow\vfw
#else
Name: {group}\{cm:shrt_audioConfig}; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configureAudio"; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 4; Components: ffdshow
Name: {group}\{cm:shrt_videoConfig}; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configure"; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 3; Components: ffdshow
Name: {group}\{cm:shrt_videoDXVAConfig}; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{app}\ffdshow.ax"",configureDXVA"; WorkingDir: {app}; IconFilename: {app}\ffdshow.ax; IconIndex: 10; Components: ffdshow\dxva
Name: {group}\{cm:shrt_vfwConfig}; Filename: {#= ff_sys}\rundll32.exe; Parameters: """{#= ff_sys}\ff_vfw.dll"",configureVFW"; WorkingDir: {#= ff_sys}; IconFilename: {app}\ffdshow.ax; IconIndex: 5; Components: ffdshow\vfw
#endif
#if include_makeavis
Name: {group}\makeAVIS; Filename: {app}\makeAVIS.exe; Components: ffdshow\makeavis
#endif
Name: {group}\{cm:shrt_uninstall}; Filename: {uninstallexe}
Name: {group}\{cm:shrt_homepage}; Filename: http://ffdshow-tryout.sourceforge.net/; IconFilename: {app}\ffdshow.ax; IconIndex: 9; Components: ffdshow

[Files]
; For speaker config
Source: "ffSpkCfg.dll"; Flags: dontcopy

Source: "{#= bindir}\ffmpeg.dll";                 DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "{#= bindir}\ff_liba52.dll";              DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "{#= bindir}\ff_libdts.dll";              DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "{#= bindir}\ff_libfaad2.dll";            DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "{#= bindir}\ff_libmad.dll";              DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "{#= bindir}\ff_unrar.dll";               DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "{#= bindir}\ff_samplerate.dll";          DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion

#if include_xvidcore
Source: "{#= bindir}\xvidcore.dll";               DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
#endif
Source: "{#= bindir}\ff_kernelDeint.dll";         DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion; Check: Is_SSE_Supported;
Source: "{#= bindir}\TomsMoComp_ff.dll";          DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "{#= bindir}\libmpeg2_ff.dll";            DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion restartreplace uninsrestartdelete

#ifdef PREF_CLSID_ICL
Source: "{#= bindir}\ffdshow_icl.ax";             DestDir: "{app}"; DestName: "ffdshow.ax"; Components: ffdshow;                    Flags: ignoreversion restartreplace uninsrestartdelete regserver noregerror
#else
Source: "{#= bindir}\ffdshow.ax";                 DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion restartreplace uninsrestartdelete regserver noregerror
#endif
Source: "..\manifest\ffdshow.ax.manifest";        DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion restartreplace uninsrestartdelete; OnlyBelowVersion: 0,6;

Source: "{#= bindir}\ff_wmv9.dll";                DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
#if include_quicksync
Source: "{#= bindir}\IntelQuickSyncDecoder.dll";  DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
#endif

Source: "{#= bindir}\ff_vfw.dll";                 DestDir: "{sys}";                         Components: ffdshow\vfw;                Flags: ignoreversion restartreplace uninsrestartdelete
Source: "..\manifest\ff_vfw.dll.manifest";        DestDir: "{sys}";                         Components: ffdshow\vfw;                Flags: ignoreversion restartreplace uninsrestartdelete; OnlyBelowVersion: 0,6;

#if include_plugin_avisynth
Source: "..\ffavisynth.avsi";                     DestDir: "{code:GetAviSynthPluginDir}";   Components: ffdshow\plugins\avisynth;   Flags: ignoreversion restartreplace uninsrestartdelete
Source: "{#= bindir}\ffavisynth.dll";             DestDir: "{code:GetAviSynthPluginDir}";   Components: ffdshow\plugins\avisynth;   Flags: ignoreversion restartreplace uninsrestartdelete
#endif
#if include_plugin_virtualdub
Source: "{#= bindir}\ffvdub.vdf";                 DestDir: "{code:GetVdubPluginDir}";       Components: ffdshow\plugins\virtualdub; Flags: ignoreversion restartreplace uninsrestartdelete
#endif
#if include_plugin_dscaler
Source: "{#= bindir}\FLT_ffdshow.dll";            DestDir: "{code:GetDScalerDir|}";         Components: ffdshow\plugins\dscaler;    Flags: ignoreversion restartreplace uninsrestartdelete
#endif

#if include_makeavis
Source: "{#= bindir}\makeAVIS.exe";               DestDir: "{app}";                         Components: ffdshow\makeavis;           Flags: ignoreversion restartreplace uninsrestartdelete
Source: "..\manifest\makeAVIS.exe.manifest";      DestDir: "{app}";                         Components: ffdshow\makeavis;           Flags: ignoreversion restartreplace uninsrestartdelete; OnlyBelowVersion: 0,6;
Source: "{#= bindir}\ff_acm.acm";                 DestDir: "{sys}";                         Components: ffdshow\makeavis;           Flags: ignoreversion restartreplace uninsrestartdelete
#endif

Source: "..\languages\*.*";                       DestDir: "{app}\languages";               Components: ffdshow;                    Flags: ignoreversion
Source: "..\custom matrices\*.*";                 DestDir: "{app}\custom matrices";         Components: ffdshow\vfw;                Flags: ignoreversion
Source: "..\openIE.js";                           DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "license\gnu_license.txt";                DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion
Source: "license\Boost_Software_License_1.0.txt"; DestDir: "{app}";                         Components: ffdshow;                    Flags: ignoreversion

[InstallDelete]
; Private assemblies
Type: files; Name: "{app}\msvcr80.dll";                            Components: ffdshow
Type: files; Name: "{app}\microsoft.vc80.crt.manifest";            Components: ffdshow
Type: files; Name: "{group}\*.*";                                  Components: ffdshow
; Files not included in this build
#if !include_xvidcore
Type: files; Name: "{app}\xvidcore.dll";                           Components: ffdshow
#endif
; Outdated files
Type: files; Name: "{app}\languages\ffdshow.1029.cz";              Components: ffdshow
Type: files; Name: "{app}\languages\ffdshow.1041.jp";              Components: ffdshow
Type: files; Name: "{app}\languages\ffdshow.1053.se";              Components: ffdshow
Type: files; Name: "{app}\libavcodec.dll";                         Components: ffdshow
Type: files; Name: "{app}\libmplayer.dll";                         Components: ffdshow
Type: files; Name: "{app}\ff_tremor.dll";                          Components: ffdshow
Type: files; Name: "{app}\ff_x264.dll";                            Components: ffdshow
Type: files; Name: "{app}\ffmpegmt.dll";                           Components: ffdshow

[Registry]
#if is64bit
  #define ff_reg_base = 'Software\GNU\ffdshow64'
#else
  #define ff_reg_base = 'Software\GNU\ffdshow'
#endif

; Cleanup of settings
Root: HKCU; Subkey: "Software\GNU";               Components: ffdshow;     Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "{#= ff_reg_base}";           Components: ffdshow;     Flags: uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";     Components: ffdshow;     Flags: uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_audio_raw"; Components: ffdshow;     Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";      Components: ffdshow\dxva; Flags: uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_enc";       Components: ffdshow;     Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_raw";       Components: ffdshow;     Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw";       Components: ffdshow\vfw; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\GNU";               Components: ffdshow;     Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "{#= ff_reg_base}";           Components: ffdshow;     Flags: uninsdeletekey
Root: HKLM; Subkey: "{#= ff_reg_base}_audio";     Components: ffdshow;     Flags: uninsdeletekey
Root: HKLM; Subkey: "{#= ff_reg_base}_vfw";       Components: ffdshow\vfw; Flags: uninsdeletekey

; Reset settings
Root: HKCU; Subkey: "{#= ff_reg_base}";           Components: ffdshow;     Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";     Components: ffdshow;     Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_audio_raw"; Components: ffdshow;     Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";      Components: ffdshow\dxva; Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_enc";       Components: ffdshow;     Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_raw";       Components: ffdshow;     Flags: deletekey; Tasks: resetsettings
Root: HKCU; Subkey: "{#= ff_reg_base}_vfw";       Components: ffdshow\vfw; Flags: deletekey; Tasks: resetsettings
Root: HKLM; Subkey: "{#= ff_reg_base}";           Components: ffdshow;     Flags: deletekey; Tasks: resetsettings
Root: HKLM; Subkey: "{#= ff_reg_base}_audio";     Components: ffdshow;     Flags: deletekey; Tasks: resetsettings
Root: HKLM; Subkey: "{#= ff_reg_base}_vfw";       Components: ffdshow\vfw; Flags: deletekey; Tasks: resetsettings

; Path
Root: HKLM; Subkey: "{#= ff_reg_base}"; ValueType: string; ValueName: "pth";           ValueData: "{app}";                                         Components: ffdshow
Root: HKLM; Subkey: "{#= ff_reg_base}";                    ValueName: "pthPriority";                                                               Components: ffdshow;                    Flags: deletevalue
#if include_plugin_avisynth
Root: HKLM; SubKey: "{#= ff_reg_base}"; ValueType: string; ValueName: "pthAvisynth";   ValueData: "{code:GetAviSynthPluginDir}";                   Components: ffdshow\plugins\avisynth;   Flags: uninsclearvalue
#endif
#if include_plugin_virtualdub
Root: HKLM; SubKey: "{#= ff_reg_base}"; ValueType: string; ValueName: "pthVirtualDub"; ValueData: "{code:GetVdubPluginDir}";                       Components: ffdshow\plugins\virtualdub; Flags: uninsclearvalue
#endif
#if include_plugin_dscaler
Root: HKLM; SubKey: "{#= ff_reg_base}"; ValueType: string; ValueName: "dscalerPth";    ValueData: "{code:GetDScalerDir|}";                         Components: ffdshow\plugins\dscaler;    Flags: uninsclearvalue
#endif

; Version info
Root: HKLM; Subkey: "{#= ff_reg_base}"; ValueType: dword;  ValueName: "revision";      ValueData: "{#= SVN_REVISION}";                             Components: ffdshow
Root: HKLM; Subkey: "{#= ff_reg_base}"; ValueType: dword;  ValueName: "builddate";     ValueData: "{#= BUILD_YEAR}{#= BUILD_MONTH}{#= BUILD_DAY}"; Components: ffdshow

; Language
#if localize
Root: HKLM; Subkey: "{#= ff_reg_base}"; ValueType: string; ValueName: "lang";          ValueData: "{cm:langid}";                                   Components: ffdshow
#endif

; Register VfW interface
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\drivers.desc";      ValueType: string; ValueName: "ff_vfw.dll";   ValueData: "ffdshow video encoder"; Components: ffdshow\vfw;      Flags: uninsdeletevalue
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32";         ValueType: string; ValueName: "VIDC.FFDS";    ValueData: "ff_vfw.dll";            Components: ffdshow\vfw;      Flags: uninsdeletevalue
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";                                                                                    Components: ffdshow\vfw;      Flags: uninsdeletekey
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";  ValueType: string; ValueName: "Description";  ValueData: "ffdshow video encoder"; Components: ffdshow\vfw;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";  ValueType: string; ValueName: "Driver";       ValueData: "ff_vfw.dll";            Components: ffdshow\vfw;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\icm\VIDC.FFDS";  ValueType: string; ValueName: "FriendlyName"; ValueData: "ffdshow video encoder"; Components: ffdshow\vfw;

#if include_makeavis
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\drivers.desc";      ValueType: string; ValueName: "ff_acm.acm";   ValueData: "ffdshow ACM codec";     Components: ffdshow\makeavis; Flags: uninsdeletevalue
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32";         ValueType: string; ValueName: "msacm.avis";   ValueData: "ff_acm.acm";            Components: ffdshow\makeavis; Flags: uninsdeletevalue
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis";                                                                                   Components: ffdshow\makeavis; Flags: uninsdeletekey
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis"; ValueType: string; ValueName: "Description";  ValueData: "ffdshow ACM codec";     Components: ffdshow\makeavis;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis"; ValueType: string; ValueName: "Driver";       ValueData: "ff_acm.acm";            Components: ffdshow\makeavis;
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\MediaResources\acm\msacm.avis"; ValueType: string; ValueName: "FriendlyName"; ValueData: "ffdshow ACM codec";     Components: ffdshow\makeavis;
#endif

; Recommended settings
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "postprocH264mode";     ValueData: "0";                  Components: ffdshow;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "resizeMethod";         ValueData: "9";                  Components: ffdshow;     Flags: createvalueifdoesntexist

#ifndef PREF_YAMAGATA
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "fastH264";             ValueData: "2";                  Components: ffdshow; Tasks:     skiph264inloop
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "fastH264";             ValueData: "0";                  Components: ffdshow; Tasks: NOT skiph264inloop;        Check: NOT IsUpdate
#endif

Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "subTextpin";           ValueData: "1";                  Components: ffdshow;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "subTextpinSSA";        ValueData: "1";                  Components: ffdshow;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "subIsExpand";          ValueData: "0";                  Components: ffdshow;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "isSubtitles";          ValueData: "1";                  Components: ffdshow; Tasks:     filter\subtitles
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "isSubtitles";          ValueData: "0";                  Components: ffdshow; Tasks: NOT filter\subtitles;      Check: NOT IsUpdate

Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "mixerNormalizeMatrix"; ValueData: "0";                  Components: ffdshow;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "isvolume";             ValueData: "1";                  Components: ffdshow; Tasks:     filter\normalize
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "volNormalize";         ValueData: "1";                  Components: ffdshow; Tasks:     filter\normalize
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "volNormalize";         ValueData: "0";                  Components: ffdshow; Tasks: NOT filter\normalize;      Check: NOT IsUpdate

Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "ismixer";              ValueData: "{code:GetIsMixer}";  Components: ffdshow
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "mixerOut";             ValueData: "{code:GetMixerOut}"; Components: ffdshow

; Blacklist
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isBlacklist";          ValueData: "1";                  Components: ffdshow;     Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isBlacklist";          ValueData: "1";                  Components: ffdshow;     Flags: createvalueifdoesntexist

; Compatibility list
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isWhitelist";          ValueData: "1";                  Components: ffdshow; Tasks:     whitelist;
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isWhitelist";          ValueData: "0";                  Components: ffdshow; Tasks: NOT whitelist;             Check: NOT IsUpdate
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isWhitelist";          ValueData: "1";                  Components: ffdshow; Tasks:     whitelist
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isWhitelist";          ValueData: "0";                  Components: ffdshow; Tasks: NOT whitelist;             Check: NOT IsUpdate

Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isCompMgr";            ValueData: "1";                  Components: ffdshow; Tasks: whitelist AND     whitelist\prompt
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "isCompMgr";            ValueData: "0";                  Components: ffdshow; Tasks: whitelist AND NOT whitelist\prompt
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isCompMgr";            ValueData: "1";                  Components: ffdshow; Tasks: whitelist AND     whitelist\prompt
Root: HKCU; Subkey: "{#= ff_reg_base}_audio";         ValueType: dword;  ValueName: "isCompMgr";            ValueData: "0";                  Components: ffdshow; Tasks: whitelist AND NOT whitelist\prompt

; DXVA registration flag
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noDxvaDecoder";        ValueData: "0";                  Components: ffdshow\dxva
Root: HKLM; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "noDxvaDecoder";        ValueData: "1";                  Components: NOT ffdshow\dxva

; DXVA Compatibility list
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";          ValueType: dword;  ValueName: "isCompMgr";            ValueData: "0";                  Components: ffdshow\dxva; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";          ValueType: dword;  ValueName: "isBlacklist";          ValueData: "0";                  Components: ffdshow\dxva; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";          ValueType: dword;  ValueName: "isWhitelist";          ValueData: "1";                  Components: ffdshow\dxva; Flags: createvalueifdoesntexist
Root: HKCU; Subkey: "{#= ff_reg_base}_dxva";          ValueType: string;  ValueName: "Whitelist";           ValueData: "bsplayer.exe;coreplayer.exe;ehshell.exe;gom.exe;graphedit.exe;graphedt.exe;graphstudio.exe;graphstudio64.exe;kmplayer.exe;mpc-hc.exe;mpc-hc64.exe;mplayerc.exe;wmplayer.exe;zplayer.exe;";                  Components: ffdshow\dxva; Flags: createvalueifdoesntexist

; Fix invalid codec settings
Root: HKCU; Subkey: "{#= ff_reg_base}";               ValueType: dword;  ValueName: "needCodecFix";         ValueData: "1";                  Components: ffdshow;                                   Check: IsUpdate

; Registry keys for the audio/video formats:
#include "reg_formats.iss"

; Audio pass-through upgrade path:
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "passthroughAC3";       ValueData: "1";                  Components: ffdshow; Tasks:     filter\passthroughac3
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "passthroughAC3";       ValueData: "0";                  Components: ffdshow; Tasks: NOT filter\passthroughac3; Check: NOT IsUpdate
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "passthroughDTS";       ValueData: "1";                  Components: ffdshow; Tasks:     filter\passthroughdts
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "passthroughDTS";       ValueData: "0";                  Components: ffdshow; Tasks: NOT filter\passthroughdts; Check: NOT IsUpdate
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\";                           ValueName: "ac3SPDIF";                                              Components: ffdshow;     Flags: deletevalue

; DRC upgrade path:
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "decoderDRC";           ValueData: "1";                  Components: ffdshow;                                   Check:     GetDecoderDRC
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "decoderDRC";           ValueData: "0";                  Components: ffdshow;                                   Check: NOT GetDecoderDRC
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\";                           ValueName: "ac3DRC";                                                Components: ffdshow;     Flags: deletevalue
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\";                           ValueName: "dtsDRC";                                                Components: ffdshow;     Flags: deletevalue

; OSD upgrade path:
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "isOSD";                ValueData: "1";                  Components: ffdshow;                                   Check:     GetDefaultPresetIsOSD
Root: HKCU; Subkey: "{#= ff_reg_base}\default";       ValueType: dword;  ValueName: "isOSD";                ValueData: "0";                  Components: ffdshow;                                   Check: NOT GetDefaultPresetIsOSD
Root: HKCU; Subkey: "{#= ff_reg_base}\";                                 ValueName: "isOSD";                                                 Components: ffdshow;     Flags: deletevalue
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "isOSD";                ValueData: "1";                  Components: ffdshow;                                   Check:     GetDefaultAudioPresetIsOSD
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\default"; ValueType: dword;  ValueName: "isOSD";                ValueData: "0";                  Components: ffdshow;                                   Check: NOT GetDefaultAudioPresetIsOSD
Root: HKCU; Subkey: "{#= ff_reg_base}_audio\";                           ValueName: "isOSD";                                                 Components: ffdshow;     Flags: deletevalue

[Run]
Description: "{cm:run_audioConfig}";     Filename: "{#= ff_sys}\rundll32.exe"; Parameters: """{app}\ffdshow.ax"",configureAudio";     WorkingDir: "{app}";       Components: ffdshow;     Flags: postinstall nowait unchecked
Description: "{cm:run_videoConfig}";     Filename: "{#= ff_sys}\rundll32.exe"; Parameters: """{app}\ffdshow.ax"",configure";          WorkingDir: "{app}";       Components: ffdshow;     Flags: postinstall nowait unchecked
Description: "{cm:run_videoDXVAConfig}"; Filename: "{#= ff_sys}\rundll32.exe"; Parameters: """{app}\ffdshow.ax"",configureDXVA";      WorkingDir: "{app}";       Components: ffdshow\dxva; Flags: postinstall nowait unchecked
Description: "{cm:run_vfwConfig}";       Filename: "{#= ff_sys}\rundll32.exe"; Parameters: """{#= ff_sys}\ff_vfw.dll"",configureVFW"; WorkingDir: "{#= ff_sys}"; Components: ffdshow\vfw; Flags: postinstall nowait unchecked

; All custom strings in the installer:
#include "custom_messages.iss"

[Code]
// Global vars
var
  reg_mixerOut: Cardinal;
  reg_ismixer: Cardinal;
  SpeakerPage: TInputOptionWizardPage;
  is8DisableMixer: Boolean;


function CheckTaskAudio(name: String; value: Integer; showbydefault: Boolean): Boolean;
var
  regval: Cardinal;
begin
  Result := False;
  if RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio', name, regval) then begin
    Result := (regval = value);
  end
  else begin
    if RegQueryDwordValue(HKLM, '{#= ff_reg_base}_audio', name, regval) then begin
      Result := (regval = value);
    end
    else begin
      Result := showbydefault;
    end
  end
end;

function CheckTaskAudioInpreset(name: String; value: Integer; showbydefault: Boolean): Boolean;
var
  regval: Cardinal;
begin
  Result := False;
  if RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio\default', name, regval) then
    Result := (regval = value)
  else
    Result := showbydefault;
end;

function GetTaskVolNormalize(): Boolean;
begin
  Result := False;
  if CheckTaskAudioInpreset('isvolume', 1, False) then
    if CheckTaskAudioInpreset('volNormalize', 1, False) then
     Result := True;
end;

function GetTaskPassthroughAC3(): Boolean;
var
  regval: Cardinal;
  presetList: TArrayOfString;
  index: Integer;
begin
  Result := False;
  if CheckTaskAudioInpreset('passthroughAC3', 1, False) then
    begin
      Result := True;
    end;
  if CheckTaskAudio('ac3', 16, False) then
    begin
      Result := True;
    end;
  if RegGetSubkeyNames(HKCU, '{#= ff_reg_base}_audio\', presetList) then
    begin
      for index := 0 to GetArrayLength(presetList)-1 do
        begin
          if (presetList[index] <> 'default') then
            begin
              if RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio\' + presetList[index], 'outsfs', regval) then
                begin
                  // the second condition equals (regval & 16 > 0)
                  if (Result) OR (CheckTaskAudio('ac3SPDIF', 1, False) AND (((regval / 16) MOD 2) = 1)) then
                    begin
                      RegWriteDWordValue(HKCU, '{#= ff_reg_base}_audio\' + presetList[index], 'passthroughAC3', 1)
                    end
                end
            end
        end
    end;
  if RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio\default', 'outsfs', regval) then
  begin
    // the second condition equals (regval & 16 > 0)
    if CheckTaskAudio('ac3SPDIF', 1, False) AND (((regval / 16) MOD 2) = 1) then
      begin
        Result := True;
      end
  end
end;

function GetTaskPassthroughDTS(): Boolean;
var
  regval: Cardinal;
  presetList: TArrayOfString;
  index: Integer;
begin
  Result := False;
  if CheckTaskAudioInpreset('passthroughDTS', 1, False) then
    begin
      Result := True;
    end;
  if CheckTaskAudio('dts', 16, False) then
    begin
      Result := True;
    end;
  if RegGetSubkeyNames(HKCU, '{#= ff_reg_base}_audio\', presetList) then
    begin
      for index := 0 to GetArrayLength(presetList)-1 do
        begin
          if (presetList[index] <> 'default') then
            begin
              if RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio\' + presetList[index], 'outsfs', regval) then
                begin
                  // the second condition equals (regval & 16 > 0)
                  if (Result) OR (CheckTaskAudio('ac3SPDIF', 1, False) AND (((regval / 16) MOD 2) = 1)) then
                    begin
                      RegWriteDWordValue(HKCU, '{#= ff_reg_base}_audio\' + presetList[index], 'passthroughDTS', 1)
                    end
                end
            end
        end
    end;
  if RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio\default', 'outsfs', regval) then
  begin
    // the second condition equals (regval & 16 > 0)
    if CheckTaskAudio('ac3SPDIF', 1, False) AND (((regval / 16) MOD 2) = 1) then
      begin
        Result := True;
      end
  end
end;

function GetDecoderDRC(): Boolean;
var
  presetList: TArrayOfString;
  index: Integer;
begin
  Result := False;
  if CheckTaskAudio('ac3DRC', 1, False) then
    begin
      Result := True;
    end;
  if CheckTaskAudio('dtsDRC', 1, False) then
    begin
      Result := True;
    end;
if (Result) then
  begin
    if RegGetSubkeyNames(HKCU, '{#= ff_reg_base}_audio\', presetList) then
      begin
        for index := 0 to GetArrayLength(presetList)-1 do
          begin
            if (presetList[index] <> 'default') then
              begin
                RegWriteDWordValue(HKCU, '{#= ff_reg_base}_audio\' + presetList[index], 'decoderDRC', 1)
              end
          end
      end
  end;
  if CheckTaskAudioInpreset('decoderDRC', 1, False) then
    begin
      Result := True;
    end;
end;

function GetDefaultPresetIsOSD(): Boolean;
var
  regval: Cardinal;
  presetList: TArrayOfString;
  index: Integer;
begin
  Result := False;
  if (RegQueryDwordValue(HKCU, '{#= ff_reg_base}', 'isOSD', regval)) then
    begin
      Result := True;
    end;
  if (Result) then
    begin
      if RegGetSubkeyNames(HKCU, '{#= ff_reg_base}\', presetList) then
        begin
          for index := 0 to GetArrayLength(presetList)-1 do
            begin
              if (RegQueryDwordValue(HKCU, '{#= ff_reg_base}\' + presetList[index], 'orderGrab', regval)) then
                begin
                  RegWriteDWordValue(HKCU, '{#= ff_reg_base}\' + presetList[index], 'orderOSD', regval + 1)
                end;
            end
        end
    end;
  Result := False;
  if (RegQueryDwordValue(HKCU, '{#= ff_reg_base}\default', 'isOSD', regval)) then
    begin
      if (regval = 1) then
        begin
          Result := True;
        end;
    end;
end;

function GetDefaultAudioPresetIsOSD(): Boolean;
var
  regval: Cardinal;
  presetList: TArrayOfString;
  index: Integer;
begin
  Result := False;
  if (RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio', 'isOSD', regval)) then
    begin
      Result := True;
    end;
  if (Result) then
    begin
      if RegGetSubkeyNames(HKCU, '{#= ff_reg_base}_audio\', presetList) then
        begin
          for index := 0 to GetArrayLength(presetList)-1 do
            begin
              if (RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio\' + presetList[index], 'orderMixer', regval)) then
                begin
                  RegWriteDWordValue(HKCU, '{#= ff_reg_base}_audio\' + presetList[index], 'orderOSD', regval + 1)
                end;
            end
        end
    end;
  Result := False;
  if (RegQueryDwordValue(HKCU, '{#= ff_reg_base}_audio\default', 'isOSD', regval)) then
    begin
      if (regval = 1) then
        begin
          Result := True;
        end;
    end;
end;

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
  if PageID = wpLicense then begin
    Result := IsUpdate;
  end;

  #if include_plugin_virtualdub
  if PageID = VdubDirPage.ID then begin
    if IsComponentSelected('ffdshow\plugins\virtualdub') then begin
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
