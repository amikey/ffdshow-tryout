/*
 * Copyright (c) 2002-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include <shlwapi.h>
#include "TsubtitlesFile.h"
#include "TsubreaderMplayer.h"
#include "TsubreaderVobsub.h"
#include "TsubreaderPGS.h"
#include "TsubtitlesSettings.h"
#include "IffdshowBase.h"
#include "distance.h"
#include "Tstream.h"

const char_t* TsubtitlesFile::exts[]= {
    _l("utf"),
    _l("idx"),
    _l("sub"),
    _l("srt"),
    _l("smi"),
    _l("rt") ,
    _l("txt"),
    _l("ass"),
    _l("ssa"),
    _l("aqt"),
    _l("mpl"),
    _l("usf"),
    _l("sup"),
    NULL
};
const char_t* TsubtitlesFile::mask=_l("Subtitles (*.utf;*.sub;*.srt;*.smi;*.rt;*.txt;*.ssa;*.ass;*.aqt;*.mpl;*.idx;*.sup)\0*.utf;*.sub;*.srt;*.smi;*.rt;*.txt;*.ssa;*.ass;*.aqt;*.mpl;*.usf;*.idx;*.sup\0All files (*.*)\0*.*\0");

bool TsubtitlesFile::extMatch(const char_t *flnm)
{
    ffstring ext;
    extractfileext(flnm,ext);
    ext.ConvertToLowerCase();
    for (int i=0; exts[i]; i++)
        if (ext == exts[i]) {
            return true;
        }
    return false;
}

static void fixRelativePaths(strings &dirs,const char_t *basepath)
{
    for (strings::iterator d=dirs.begin(); d!=dirs.end(); d++)
        if (PathIsRelative(d->c_str())) {
            char_t absdir[MAX_PATH];
            _makepath_s(absdir, countof(absdir), NULL, basepath, d->c_str(), NULL);
            char_t absdirC[MAX_PATH];
            PathCanonicalize(absdirC,absdir);
            *d=absdirC;
        }
}

void TsubtitlesFile::findSubtitlesFile(const char_t *aviFlnm,const char_t *sdir,const char_t *sext,char_t *subFlnm,size_t buflen,int heuristic,IcheckSubtitle *checkSubtitle)
{
    if (!subFlnm) {
        return;
    }
    if (heuristic) {
        char_t aviDsk[MAX_PATH],aviDir[MAX_PATH],aviName[MAX_PATH];
        _splitpath_s(aviFlnm, aviDsk, countof(aviDsk), aviDir, countof(aviDir), aviName, countof(aviName), NULL, 0);
        char_t aviDskDirName[MAX_PATH];
        _makepath_s(aviDskDirName, countof(aviDskDirName), aviDsk, aviDir, aviName, NULL);
        strings subfiles;
        findPossibleSubtitles(aviFlnm,sdir,subfiles);
        int mindist=INT_MAX;
        EditDistance dist;
        strings::const_iterator s=subfiles.end();
        typedef std::vector< std::pair<int,ffstring> > TsubsDists;
        TsubsDists subdists;
        for (strings::const_iterator sf=subfiles.begin(); sf!=subfiles.end(); sf++) {
            char_t sfDsk[MAX_PATH],sfDir[MAX_PATH],sfName[MAX_PATH];
            _splitpath_s(sf->c_str(), sfDsk, countof(sfDsk), sfDir, countof(sfDir), sfName, countof(sfName), NULL, 0);
            char_t sfDskDirName[MAX_PATH];
            _makepath_s(sfDskDirName, countof(sfDskDirName), sfDsk, sfDir, sfName, NULL);
            int newdist=dist.CalEditDistance(aviDskDirName,sfDskDirName,2*MAX_PATH);
            if (!checkSubtitle) {
                if (newdist<mindist) {
                    s=sf;
                    mindist=newdist;
                    if (mindist==0) {
                        break;
                    }
                }
            } else {
                subdists.push_back(std::make_pair(newdist,*sf));
            }
        }
        if (!checkSubtitle) {
            if (s!=subfiles.end()) {
                ff_strncpy(subFlnm,s->c_str(),buflen);
                return;
            }
        } else {
            std::stable_sort(subdists.begin(),subdists.end());
            for (TsubsDists::const_iterator sd=subdists.begin(); sd!=subdists.end(); sd++)
                if (checkSubtitle->checkSubtitle(sd->second.c_str())) {
                    ff_strncpy(subFlnm,sd->second.c_str(),buflen);
                    return;
                }
        }
    } else {
        char_t dsk[MAX_PATH],dir[MAX_PATH],name[MAX_PATH],ext[MAX_PATH];
        _splitpath_s(aviFlnm, dsk, countof(dsk), dir, countof(dir), name, countof(name), ext, countof(ext));
        char_t fname[MAX_PATH];
        _makepath_s(fname, countof(fname), NULL, NULL, name, ext); //fname - movie file name with extension (without path)
        if (name[0]) {
            char_t path[MAX_PATH];
            _makepath_s(path, countof(path), dsk, dir, NULL, NULL);    //path - directory where movie is stored
            strings dirs;
            strtok(sdir,_l(";"),dirs);
            fixRelativePaths(dirs,path);
            //exact match (only extension differs)
            for (strings::const_iterator d=dirs.begin(); d!=dirs.end(); d++) {
                if (d->empty()) {
                    continue;
                }
                char_t subFlnm0[MAX_PATH];
                //sub_extsfind(d->c_str(),name,subFlnm0);
                strings etensions;
                strtok(sext,_l(";"),etensions);

                for (strings::const_iterator e=etensions.begin(); e!=etensions.end(); e++) {
                    _makepath_s(subFlnm0, countof(subFlnm0), NULL, d->c_str(), name, e->c_str());
                    if (fileexists(subFlnm0) && (!checkSubtitle || checkSubtitle->checkSubtitle(subFlnm0))) {
                        ff_strncpy(subFlnm,subFlnm0,buflen);
                        return;
                    }
                }
            }
            //movie.avi -> movie en.txt, movie.en.sub,....
            for (strings::const_iterator d=dirs.begin(); d!=dirs.end(); d++) {
                _makepath_s(fname,MAX_PATH,NULL,d->c_str(),name,NULL);
                strncat_s(fname, countof(fname), _l("*"), _TRUNCATE); //fname - c:\movies\movie*.*
                strings files;
                findFiles(fname,files);
                for (strings::const_iterator f=files.begin(); f!=files.end(); f++) {
                    ffstring ext;
                    extractfileext(f->c_str(),ext);
                    ext.ConvertToLowerCase();
                    strings etensions;
                    strtok(sext,_l(";"),etensions);
                    for (strings::const_iterator e=etensions.begin(); e!=etensions.end(); e++)
                        if (ext == *e && (!checkSubtitle || checkSubtitle->checkSubtitle(f->c_str()))) {
                            ff_strncpy(subFlnm,f->c_str(),buflen);
                            subFlnm[buflen-1]='\0';
                            return;
                        }
                }
            }
        }
    }
    subFlnm[0]='\0';
}

void TsubtitlesFile::findPossibleSubtitles(const char_t *dir,strings &files)
{
    char_t autosubmask[MAX_PATH];
    _makepath_s(autosubmask,MAX_PATH,NULL,dir,/*name*/_l("*"),_l("*"));
    strings files0;
    findFiles(autosubmask,files0);
    for (strings::iterator f=files0.begin(); f!=files0.end(); f++)
        if (extMatch(f->c_str())) {
            files.push_back(*f);
        }
    std::sort(files.begin(),files.end(),ffstring_iless());
}
void TsubtitlesFile::findPossibleSubtitles(const char_t *aviFlnm,const char_t *sdir,strings &files,subtitleFilesSearchMode searchMode)
{
    if (!aviFlnm || !sdir) {
        return;
    }
    char_t avidir[MAX_PATH];
    if (aviFlnm[0]) {
        char_t dsk[MAX_PATH],dir[MAX_PATH],name[MAX_PATH];
        _splitpath_s(aviFlnm,dsk,MAX_PATH,dir,MAX_PATH,name,MAX_PATH,NULL,0);
        _makepath_s(avidir, countof(avidir), dsk, dir, NULL, NULL);
    } else {
        strcpy(avidir,_l("."));
    }
    strings dirs;
    strtok(sdir,_l(";"),dirs);
    fixRelativePaths(dirs,avidir);
    for (strings::const_iterator sd=dirs.begin(); sd!=dirs.end(); sd++) {
        findPossibleSubtitles(sd->c_str(),files);
    }

    // If search mode is "search video file match then heuristic", then test if at least 1 subtitle filename matches with the video filename
    bool videoFileMatch = false;
    ffstring videoFlnmNoExt;
    extractfilenameWOext(aviFlnm,videoFlnmNoExt);
    videoFlnmNoExt.ConvertToLowerCase();

    if (searchMode == SUBFILES_VIDEO_FILE_HEURISTIC) {
        for (strings::iterator f=files.begin(); f!=files.end(); f++) {
            ffstring flnmNoExt;
            extractfilenameWOext((*f).c_str(),flnmNoExt);
            flnmNoExt.ConvertToLowerCase();
            if (strncmp(videoFlnmNoExt.c_str(), flnmNoExt.c_str(), videoFlnmNoExt.length()) == 0) {
                /* Common portion of video and subtitle filenames. Check if lengths are equal (identical names)
                   or if this is a dot "." right after like in videofile.<suffix>.ext */
                if (videoFlnmNoExt.length() == flnmNoExt.length() || flnmNoExt[videoFlnmNoExt.length()] == _l('.')) {
                    videoFileMatch = true;
                    break;
                }
            }
        }
    }

    if (searchMode != SUBFILES_ALL) {
        EditDistance dist;
        for (strings::iterator f=files.begin(); f!=files.end();) {
            ffstring flnmNoExt;
            extractfilenameWOext((*f).c_str(),flnmNoExt);
            flnmNoExt.ConvertToLowerCase();
            // If search mode is based on video file name removes the non matching entries
            if (searchMode == SUBFILES_VIDEO_FILE_MATCH
                    || (searchMode == SUBFILES_VIDEO_FILE_HEURISTIC && videoFileMatch)) {
                if (strncmp(videoFlnmNoExt.c_str(), flnmNoExt.c_str(), videoFlnmNoExt.length()) != 0
                        || (videoFlnmNoExt.length() != flnmNoExt.length() && flnmNoExt[videoFlnmNoExt.length()] != _l('.'))) {
                    f = files.erase(f);
                } else {
                    f++;
                }
                continue;
            }

            // Heuristic search
            int distance=dist.CalEditDistance(videoFlnmNoExt.c_str(),flnmNoExt.c_str(),2*MAX_PATH);
            //DPRINTF(_l("Subtitle files : %s vs %s match result = %d"), flnmNoExt.c_str(), videoFlnmNoExt.c_str(), distance);
            if (distance > SUBFILES_HEURISTIC_LIMIT) {
                f = files.erase(f);
            } else {
                f++;
            }
        }
    }
}

TsubtitlesFile::TsubtitlesFile(IffdshowBase *Ideci):Tsubtitles(Ideci)
{
    hwatch=NULL;
    f=NULL;
    subFlnm[0]='\0';
}
TsubtitlesFile::~TsubtitlesFile()
{
    if (f!=NULL) {
        fclose(f);
        f=NULL;
    }
    if (hwatch) {
        FindCloseChangeNotification(hwatch);
    }
}

void TsubtitlesFile::done(void)
{
    Tsubtitles::done();
    if (hwatch) {
        FindCloseChangeNotification(hwatch);
    }
    hwatch=NULL;
}

bool TsubtitlesFile::init(const TsubtitlesSettings *cfg,const char_t *IsubFlnm,double Ifps,bool watch,int checkOnly)
{
    if (!cfg->is || !cfg->isSubFiles) {
        DPRINTF(_l("TsubtitlesFile::init subtitles files disabled is(%d), isSubFiles(%d)"),cfg->is, cfg->isSubFiles);
        return false;
    }

    if (!IsubFlnm || (checkOnly==2 || stricmp(subFlnm,IsubFlnm)!=0)) {
        ff_strncpy(subFlnm, IsubFlnm ? IsubFlnm : _l(""), countof(subFlnm));
    }
    if (checkOnly<2) {
        fps=Ifps;
        done();
    }
    if (subFlnm[0]=='\0') {
        return false;
    }
    if (f!= NULL) {
        fclose(f);
        f=NULL;
    }
    f=fopen(subFlnm,_l("rb"));
    if (!f) {
        return false;
    }
    fs = TstreamFile(f,TfontSettings::GDI_charset_to_code_page(deci->getParam2(IDFF_fontCharset)));
    sub_format=Tsubreader::sub_autodetect(fs,ffcfg);
    if (sub_format!=Tsubreader::SUB_INVALID && checkOnly!=2) {
        fs.rewind();
        if ((sub_format&Tsubreader::SUB_FORMATMASK)==Tsubreader::SUB_VOBSUB) {
            if (cfg->vobsub) {
                subs=new TsubreaderVobsub(fs,subFlnm,deci);
                int deflang=subs->findlang(cfg->deflang);
                if (deflang==0) {
                    deflang=subs->findlang(cfg->deflang2);
                }
                subs->setLang(deflang);
            }
        } else if ((sub_format&Tsubreader::SUB_FORMATMASK)==Tsubreader::SUB_PGS) {
            if (!cfg->pgs) {
                DPRINTF(_l("TsubtitlesFile::init Blu-Ray subtitles detected but disabled by user"));
                return false;
            }
            DPRINTF(_l("TsubtitlesFile::init Blu-Ray subtitles detected"));
            subs=new TsubreaderPGS(deci, fs,fps,cfg,ffcfg);
        } else {
            if ((sub_format&Tsubreader::SUB_FORMATMASK)==Tsubreader::SUB_SSA && !deci->getParam2(IDFF_subSSA)) {
                DPRINTF(_l("TsubtitlesFile::init SSA subtitles detected but disabled by user"));
                return false;
            }
            if (((sub_format&Tsubreader::SUB_FORMATMASK)==Tsubreader::SUB_SUBVIEWER
                    || (sub_format&Tsubreader::SUB_FORMATMASK)==Tsubreader::SUB_SUBVIEWER2)&& !cfg->isSubText) {
                DPRINTF(_l("TsubtitlesFile::init text subtitles detected but disabled by user"));
                return false;
            }
            DPRINTF(_l("TsubtitlesFile::init text subtitles detected"));
            subs=new TsubreaderMplayer(fs,sub_format,fps,cfg,ffcfg,false);
        }
    }
    // Keep the file descriptor opened if SUB_KEEP_FILE_OPENED flag is present
    if ((sub_format&Tsubreader::SUB_KEEP_FILE_OPENED) == 0) {
        fclose(f);
        f=NULL;
    }
    if (checkOnly==2) {
        return sub_format!=Tsubreader::SUB_INVALID;
    }

    // We can have empty subtitles if the file is read progressively
    if (!subs || (subs->empty() && ((sub_format&Tsubreader::SUB_KEEP_FILE_OPENED) == 0))) {
        if (subs) {
            delete subs;
        }
        subs=NULL;
        subFlnm[0]='\0';
        return false;
    }
    if (!checkOnly) {
        //subs->adjust_subs_time(6.0);
        Tsubtitles::init();
        ffstring subPath;
        extractfilepath(subFlnm,subPath);
        if (watch) {
            hwatch=FindFirstChangeNotification(subPath.c_str(),FALSE,FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_SIZE);
            lastwritetime=fileLastWriteTime(subFlnm);
        }
    }
    return true;
}

void TsubtitlesFile::checkChange(const TsubtitlesSettings *cfg,bool *forceChange)
{
    if (hwatch) {
        DWORD res=WaitForSingleObject(hwatch,0);
        if (res==WAIT_OBJECT_0 && lastwritetime!=fileLastWriteTime(subFlnm)) {
            init(cfg,subFlnm,fps,true,0);
            if (forceChange) {
                *forceChange=true;
            }
        }
    }
}

Tsubtitle* TsubtitlesFile::getSubtitle(const TsubtitlesSettings *cfg,REFERENCE_TIME rtStart, REFERENCE_TIME rtStop,bool *forceChange)
{
    if (subs) {
        subs->getSubtitle(cfg, rtStart, rtStop, forceChange);
    }
    return Tsubtitles::getSubtitle(cfg, rtStart, rtStop, forceChange);
}
