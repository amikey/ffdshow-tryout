#ifndef _TGLOBALSETTINGS_H_
#define _TGLOBALSETTINGS_H_

#include "TOSDsettings.h"
#include "ffcodecs.h"
#include "Toptions.h"

#define MAX_COMPATIBILITYLIST_LENGTH 5000

#define WHITELIST_EXE_FILENAME \
    _l("3wPlayer.exe;ACDSee10.exe;ACDSee11.exe;ACDSee5.exe;ACDSee6.exe;ACDSee7.exe;ACDSee8.exe;ACDSee8Pro.exe;ACDSee9.exe;ACDSeePro2.exe;ACDSeePro25.exe;acdseepro3.exe;Acer Crystal Eye webcam.exe;aegisub.exe;afreecaplayer.exe;")\
    _l("afreecastudio.exe;AfterFX.exe;aim6.exe;aircamwin.exe;ALLPlayer.exe;allradio.exe;AlltoaviV4.exe;ALShow.exe;ALSong.exe;AltDVB.exe;amcap.exe;amf_slv.exe;amvtransform.exe;Apollo DivX to DVD Creator.exe;Apollo3GPVideoConverter.exe;")\
    _l("Ares.exe;AsfTools.exe;ass_help3r.exe;ASUSDVD.exe;Audition.exe;AutoGK.exe;autorun.exe;avant.exe;AVerTV.exe;Avi2Dvd.exe;avi2mpg.exe;avicodec.exe;avipreview.exe;aviutl.exe;avs2avi.exe;Badak.exe;")\
    _l("BearShare.exe;BePipe.exe;bestplayer.exe;bestplayer1.0.exe;bestpl~1.exe;BitComet.exe;BlazeDVD.exe;BoonPlayer.exe;bplay.exe;bsplay.exe;bsplayer.exe;BTVD3DShell.exe;Camfrog Video Chat.exe;CamRecorder.exe;CamtasiaStudio.exe;")\
    _l("carom.exe;CEC_MAIN.exe;christv.exe;chrome.exe;cinemaplayer.exe;CinergyDVR.exe;CodecInstaller.exe;ConvertXtoDvd.exe;coolpro2.exe;CorePlayer.exe;Crystal.exe;crystalfree.exe;CrystalPro.exe;cscript.exe;CTCMS.exe;")\
    _l("CTCMSU.exe;CTWave.exe;CTWave32.exe;cut_assistant.exe;dashboard.exe;demo32.exe;divx plus player.exe;DivX Player.exe;DivxToDVD.exe;dllhost.exe;dpgenc.exe;Dr.DivX.exe;drdivx.exe;drdivx2.exe;DreamMaker.exe;DSBrws.exe;")\
    _l("DScaler.exe;dv.exe;dvbdream.exe;dvbviewer.exe;DVD Shrink 3.2.exe;DVDAuthor.exe;dvdfab.exe;DVDMaker.exe;DVDMF.exe;dvdplay.dll;dvdplay.exe;dvdSanta.exe;DXEffectTester.exe;DXEnum.exe;dxvachecker.exe;Easy RealMedia Tools.exe;")\
    _l("ehExtHost.exe;ehshell.exe;Encode360.exe;explorer.exe;fenglei.exe;ffmpeg.exe;filtermanager.exe;firefox.exe;Flash.exe;FLVPlayer4Free.exe;FMRadio.exe;Fortius.exe;FreeStyle.exe;FSViewer.exe;Funshion.exe;FusionHDTV.exe;")\
    _l("gameex.exe;GDivX Player.exe;gdsmux.exe;GoldWave.exe;gom.exe;GomEnc.exe;GoogleDesktop.exe;GoogleDesktopCrawl.exe;graphedit.exe;graphedt.exe;GraphStudio.exe;GraphStudio64.exe;GraphStudioNext.exe;GraphStudioNext64.exe;gspot.exe;HBP.exe;HDVSplit.exe;hmplayer.exe;honestechTV.exe;")\
    _l("HPWUCli.exe;i_view32.exe;ICQ.exe;ICQLite.exe;iexplore.exe;IHT.exe;IncMail.exe;InfoTool.exe;infotv.exe;InstallChecker.exe;Internet TV.exe;iPlayer.exe;ipod_video_converter.exe;IPODConverter.exe;")\
    _l("JetAudio.exe;jwBrowser.exe;kiteplayer.exe;kmplayer.exe;KwMusic.exe;LA.exe;LifeCam.exe;LifeFrame.exe;Lilith.exe;makeAVIS.exe;MatroskaDiag.exe;Maxthon.exe;MDirect.exe;Media Center 12.exe;Media Jukebox.exe;Media Player Classic.exe;")\
    _l("MediaLife.exe;mediamonkeyvhelper.exe;MediaPortal.exe;MEDIAREVOLUTION.EXE;MediaServer.exe;megui.exe;mencoder.exe;Metacafe.exe;MMPlayer.exe;MovieMaker.exe;moviemk.exe;moviethumb.exe;MP4Converter.exe;Mp4Player.exe;mpc-hc.exe;mpc-hc64.exe;mpcstar.exe;MpegVideoWizard.exe;")\
    _l("mplayer2.exe;mplayerc.exe;mplayerc64.exe;msnmsgr.exe;msoobe.exe;MultimediaPlayer.exe;Munite.exe;MusicManager.exe;Muzikbrowzer.exe;Mv2PlayerPlus.exe;My Movies.exe;myplayer.exe;nero.exe;NeroHome.exe;NeroVision.exe;")\
    _l("NicoPlayer.exe;NMSTranscoder.exe;nvplayer.exe;Omgjbox.exe;OnlineTV.exe;Opera.exe;OrbStreamerClient.exe;PaintDotNet.exe;paltalk.exe;pcwmp.exe;PhotoScreensaver.scr;Photoshop.exe;Picasa2.exe;picasa3.exe;")\
    _l("playwnd.exe;plugin-container.exe;potplayermini.exe;potplayermini64.exe;PowerDirector.exe;powerdvd.exe;POWERPNT.EXE;PPLive.exe;ppmate.exe;PPStream.exe;PQDVD_PSP.exe;Procoder2.exe;Producer.exe;progdvb.exe;ProgDvbNet.exe;PVCR.exe;Qonoha.exe;QQ.exe;QQLive.exe;")\
    _l("QQMusic.exe;QQPlayerSvr.exe;QvodPlayer.exe;QzoneMusic.exe;RadLight.exe;realplay.exe;ReClockHelper.dll;Recode.exe;RecordingManager.exe;ripbot264.exe;rlkernel.exe;RoxMediaDB10.exe;RoxMediaDB9.exe;rundll32.exe;")\
    _l("Safari.exe;SelfMV.exe;Shareaza.exe;sherlock2.exe;ShowTime.exe;sidebar.exe;SinkuHadouken.exe;Sleipnir.exe;smartmovie.exe;soffice.bin;songbird.exe;SopCast.exe;SplitCam.exe;START.EXE;staxrip.exe;stereoplayer.exe;stillcap.exe;Studio.exe;subedit.exe;")\
    _l("SubtitleEdit.exe;SubtitleWorkshop.exe;SubtitleWorkshop4.exe;SWFConverter.exe;telewizja.exe;TheaterTek DVD.exe;thumbgen.exe;time_adjuster.exe;timecodec.exe;tmc.exe;TMPGEnc.exe;TMPGEnc4XP.exe;TOTALCMD.EXE;tsdoctor.exe;TSPlayer.exe;Tvants.exe;")\
    _l("tvc.exe;TVersity.exe;TVPlayer.exe;TVUPlayer.exe;UCC.exe;Ultra EDIT.exe;UUSeePlayer.exe;VCD_PLAY.EXE;VeohClient.exe;VFAPIFrameServer.exe;VideoConvert.exe;videoconverter.exe;videoenc.exe;VideoManager.exe;")\
    _l("VideoSnapshot.exe;VideoSplitter.exe;VIDEOS~1.SCR;VideoWave9.exe;ViPlay.exe;ViPlay3.exe;ViPlay4.exe;virtualdub.exe;virtualdubmod.exe;vplayer.exe;WaveChk.exe;WCreator.exe;WebMediaPlayer.exe;WFTV.exe;")\
    _l("winamp.exe;WinAVI 9.0.exe;WinAVI MP4 Converter.exe;WinAVI.exe;WindowsPhotoGallery.exe;windvd.exe;WinDvr.exe;WinMPGVideoConvert.exe;WINWORD.EXE;WLXPhotoGallery.exe;wmenc.exe;wmplayer.exe;wmprph.exe;")\
    _l("wscript.exe;x264.exe;XNVIEW.EXE;Xvid4PSP.exe;zplayer.exe;Zune.exe;")

#define BLACKLIST_EXE_FILENAME \
    _l("age3.exe;AvatarMP.exe;BeachSoccer.exe;Bully.exe;civilizationv.exe;civilizationv_dx11.exe;cm2008.exe;cm2010.exe;cmr4.exe;cmr5.exe;conflictdeniedops.exe;conquer.exe;crazy_taxi_pc.exe;daoriginslauncher.exe;dark_horizon.exe;DaybreakDX.exe;DisciplesIII.exe;divinity2.exe;dmc3se.exe;dragonica.exe;dragonage2launcher.exe;drakensang.exe;drome racers.exe;em3.exe;em4.exe;empires_dmw.exe;engine.exe;evochronmercenary.exe;fable.exe;Edition.exe;FalconAF.exe;fallout3.exe;falloutnv.4gb;falloutnv.exe;ff7.exe;fordorr.exe;fr2.exe;fr3.exe;freedom.exe;fsr.exe;game.bin;game.exe;googletalkplugin.exe;Grandia2.exe;gta-vc.exe;gta3.exe;gta_sa.exe;")\
    _l("gta-underground.exe;gta t�rk.exe;guildII.exe;helpctr.exe;hl.exe;hl2.exe;iclone.exe;juiced.exe;juiced2_hin.exe;king.exe;launch.exe;launcher.exe;ldd.exe;legoracers.exe;lf2.exe;Main.exe;MassEffectLauncher.exe;MassEffect2Launcher.exe;maxpayne.exe;mixcraft.exe;mixcraft3.exe;mixcraft4.exe;mixcra~1.exe;morrowind.exe;msninst.exe;nations.exe;ninjablade.exe;NokiaMServer.exe;nokiaovisuite.exe;oblivion.exe;outlook.exe;patriots.exe;pes2008.exe;pes2009.exe;pes2010.exe;pes4.exe;pes5.exe;pes6.exe;powerdvd8.exe;powerdvd9.exe;powerdvd10.exe;powerdvd11.exe;power2go.exe;premiere.exe;pwo.exe;questviewer.exe;rct.exe;rct3.exe;rct3plus.exe;residentevil3.exe;")\
    _l("rf.exe;rf_online.bin;RomeGame.exe;rometw.exe;sacred.exe;sega rally.exe;sh3.exe;sh4.exe;shattered_horizon.exe;shippingpc-skygame.exe;silverlight.configuration.exe;sims.exe;simtractor.exe;skin.exe;smartlogon.exe;starsword.exe;StreetFighterIV.exe;streetlegal_redline.exe;SWRepublicCommando.exe;tw2008.exe;twoworlds.exe;vmcExtenderSetup.exe;wa.exe;war3.exe;webcamdell.exe;webcamdell2.exe;wlstartup.exe;worms 4 mayhem.exe;worms3d.exe;wormsmayhem.exe;wwp.exe;x3ap.exe;x3tc.exe;Yahoo.Messenger.YmApp.exe;yahoom~1.exe;YahooMessenger.exe;YahooMusicEngine.exe;yahoow~1.exe;YahooWidgetEngine.exe;YahooWidgets.exe;ymini.exe;zoo.exe;")

struct TregOp;
struct Tconfig;
struct TglobalSettingsBase :public Toptions {
private:
    void addToCompatiblityList(char_t *list, const char_t *exe, const char_t *delimit);
protected:
    const Tconfig *config;
    const char_t *reg_child;
    virtual void reg_op_codec(TregOp &t,TregOp *t2) {}
    void _reg_op_codec(short id,TregOp &tHKCU,TregOp *tHKLM,const char_t *name,int &val,int def);
    strings blacklistList,whitelistList;
    bool firstBlacklist,firstWhitelist;
public:
    TglobalSettingsBase(const Tconfig *Iconfig,int Imode,const char_t *Ireg_child,TintStrColl *Icoll);
    virtual ~TglobalSettingsBase() {}
    bool exportReg(bool all,const char_t *regflnm,bool unicode);
    int filtermode;
    int multipleInstances;
    int isBlacklist,isWhitelist;
    char_t blacklist[MAX_COMPATIBILITYLIST_LENGTH],whitelist[MAX_COMPATIBILITYLIST_LENGTH];
    virtual bool inBlacklist(const char_t *exe);
    virtual bool inWhitelist(const char_t *exe,IffdshowBase *Ideci);
    int addToROT;
    int allowedCPUflags;
    int compOnLoadMode;
    const char_t* getRegChild(void) {
        return reg_child;
    };

    virtual void load(void);
    virtual void save(void);
    int trayIcon,trayIconType,trayIconExt,trayIconChanged;
    int isCompMgr,isCompMgrChanged;
    int allowDPRINTF,allowDPRINTFchanged;

    int outputdebug;
    int outputdebugfile;
    char_t debugfile[MAX_PATH];
    int errorbox;

    char_t dscalerPth[MAX_PATH];
};

struct TglobalSettingsDec :public TglobalSettingsBase {
private:
    TglobalSettingsDec& operator =(const TglobalSettingsDec&);
protected:
    virtual void reg_op(TregOp &t);
    void fixMissing(int &codecId,int movie1,int movie2,int movie3);
    void fixMissing(int &codecId,int movie1,int movie2);
    void fixMissing(int &codecId,int movie);
    virtual int getDefault(int id);
    static void cleanupCodecsList(std::vector<CodecID> &ids,Tstrptrs &codecs);
    TglobalSettingsDec(const Tconfig *Iconfig,int Imode,const char_t *Ireg_child,TintStrColl *Icoll);
public:
    char_t defaultPreset[MAX_PATH];
    int autoPreset,autoPresetFileFirst;

    int streamsMenu, streamsSubFilesMode;

    virtual void load(void);
    virtual void save(void);

    virtual CodecID getCodecId(DWORD fourCC,FOURCC *AVIfourCC) const =0;
    virtual const char_t** getFOURCClist(void) const=0;
    virtual void getCodecsList(Tstrptrs &codecs) const=0;
};

struct TglobalSettingsDecVideo :public TglobalSettingsDec {
private:
    int forceInCSP;
    int needCodecFix;
    void fixNewCodecs(void);
    static const char_t *fourccs[];
    static const CodecID c_mpeg4[IDFF_MOVIE_MAX+1],c_mpeg1[IDFF_MOVIE_MAX+1],c_mpeg2[IDFF_MOVIE_MAX+1],c_wvc1[IDFF_MOVIE_MAX+1],c_wmv3[IDFF_MOVIE_MAX+1],c_wmv2[IDFF_MOVIE_MAX+1],c_wmv1[IDFF_MOVIE_MAX+1],c_h264[IDFF_MOVIE_MAX+1];
protected:
    virtual void reg_op(TregOp &t);
    virtual void reg_op_codec(TregOp &t,TregOp *t2);
    virtual int getDefault(int id);
public:
    int buildHistogram;
    int xvid,div3,mp4v,dx50,fvfw,mp43,mp42,mp41,h263,h264,vp5,vp6,vp6f,vp8,wmv1,wmv2,wvc1,wmv3,mss2,wvp2,cavs,rawv,mpg1,mpg2,mpegAVI,em2v,avrn,mjpg,dvsd,cdvc,hfyu,cyuv,theo,asv1,svq1,svq3,cram,rt21,iv32,cvid,rv10,ffv1,vp3,vcr1,rle,avis,mszh,zlib,flv1,_8bps,png1,qtrle,duck,tscc,qpeg,h261,loco,wnv1,cscd,zmbv,ulti,vixl,aasc,fps1,qtrpza,rv30,rv40,iv50,i263;
    int supdvddec;
    int fastMpeg2,fastH264;
    int alternateUncompressed;

    virtual void load(void);
    virtual void save(void);

    struct TsubtitlesSettings : Toptions {
    protected:
        virtual void getDefaultStr(int id,char_t *buf,size_t buflen);
    public:
        TsubtitlesSettings(TintStrColl *Icoll);
        char_t searchDir[2*MAX_PATH];
        char_t searchExt[2*MAX_PATH];
        int searchHeuristic;
        int watch;
        int embeddedPriority;
        int textpin;
    } sub;

    TglobalSettingsDecVideo(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll);
    virtual CodecID getCodecId(DWORD fourCC,FOURCC *AVIfourCC) const;
    virtual const char_t** getFOURCClist(void) const;
    virtual void getCodecsList(Tstrptrs &codecs) const;
};

struct TglobalSettingsDecAudio :public TglobalSettingsDec {
private:
    static const char_t *wave_formats[];
    static const CodecID c_mp123[IDFF_MOVIE_MAX+1],c_ac3[IDFF_MOVIE_MAX+1],c_dts[IDFF_MOVIE_MAX+1],c_aac[IDFF_MOVIE_MAX+1],c_vorbis[IDFF_MOVIE_MAX+1];
protected:
    virtual void reg_op_codec(TregOp &t,TregOp *t2);
    virtual int getDefault(int id);
    virtual void getDefaultStr(int id,char_t *buf,size_t buflen);
public:
    TglobalSettingsDecAudio(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll,const char_t *Ireg_child=FFDSHOWDECAUDIO);
    int wma1,wma2,mp2,mp3,ac3,aac,amr,rawa,avis,iadpcm,msadpcm,flac,lpcm,dts,vorbis,law,gsm,tta,otherAdpcm,qdm2,truespeech,mace,ra,imc,atrac3,nellymoser,wavpack,eac3,mlp,truehd,cook,qtpcm;
    int dtsinwav;
    int ac3drc,dtsdrc;
    int SPDIFCompatibility;
    int showCurrentVolume;
    int showCurrentFFT;
    int firIsUserDisplayMaxFreq,firUserDisplayMaxFreq;
    int isAudioSwitcher;
    int alwaysextensible;
    int allowOutStream;
    char_t winamp2dir[MAX_PATH];
    virtual void load(void);
    virtual void save(void);
    virtual CodecID getCodecId(DWORD fourCC,FOURCC *AVIfourCC) const;
    virtual const char_t** getFOURCClist(void) const;
    virtual void getCodecsList(Tstrptrs &codecs) const;
};
struct TglobalSettingsDecAudioRaw :public TglobalSettingsDecAudio {
    TglobalSettingsDecAudioRaw(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll):TglobalSettingsDecAudio(Iconfig,Imode,Icoll,FFDSHOWDECAUDIORAW) {}
};

struct TglobalSettingsEnc :public TglobalSettingsBase {
protected:
    virtual int getDefault(int id);
public:
    TglobalSettingsEnc(const Tconfig *Iconfig,int Imode,TintStrColl *Icoll);
    int psnr;
    int isDyInterlaced,dyInterlaced;
};

#endif
