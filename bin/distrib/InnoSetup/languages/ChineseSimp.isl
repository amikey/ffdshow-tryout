;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).

[LangOptions]
; The following three entries are very important. Be sure to read and
; understand the '[LangOptions] section' topic in the help file.
LanguageName=<7B80><4F53><4E2D><6587>
LanguageID=$0804
LanguageCodePage=936
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
DialogFontName=����
DialogFontSize=9
WelcomeFontName=����
WelcomeFontSize=12
TitleFontName=����
TitleFontSize=29
CopyrightFontName=����
CopyrightFontSize=9

[Messages]

; *** Application titles
SetupAppTitle=��װ
SetupWindowTitle=��װ - %1
UninstallAppTitle=ж��
UninstallAppFullTitle=ж�� %1

; *** Misc. common
InformationTitle=��Ϣ
ConfirmTitle=ȷ��
ErrorTitle=����

; *** SetupLdr messages
SetupLdrStartupMessage=������װ %1����ȷ��Ҫ������
LdrCannotCreateTemp=�޷�������ʱ�ļ�����װ��ֹ
LdrCannotExecTemp=�޷�����ʱ�ļ��ļ�������һ���ļ�����װ��ֹ

; *** Startup error messages
LastErrorMessage=%1��%n%n���� %2��%3
SetupFileMissing=��װ�ļ������޷��ҵ��ļ���%1�����������������������»�ȡ�ð�װ���������ĸ�����
SetupFileCorrupt=��װ�ļ����𻵡������»�ȡ�ð�װ���������ĸ�����
SetupFileCorruptOrWrongVer=��װ�ļ����𻵣������뵱ǰ�İ�װ����汾�����ݡ��������������������»�ȡ�ð�װ���������ĸ�����
NotOnThisPlatform=�������������� %1 �ϡ�
OnlyOnThisPlatform=��������������� %1 �ϡ�
OnlyOnTheseArchitectures=������ֻ�ܰ�װ��Ϊ���´������ܹ���Ƶ� Windows �汾�ϣ�%n%n%1
MissingWOW64APIs=�����е� Windows �汾��֧�ְ�װ����ִ�� 64 λ��װ����Ĺ��ܡ�Ҫ����������⣬�밲װ Service Pack %1��
WinVersionTooLowError=��������Ҫ %1 �汾 %2 ����ߵİ汾��
WinVersionTooHighError=�������ܰ�װ�� %1 �汾 %2 ����ߵİ汾�ϡ�
AdminPrivilegesRequired=��������ϵͳ����Ա����ݵ�¼ϵͳ���ܰ�װ������
PowerUserPrivilegesRequired=��������ϵͳ����Ա����ݵ�¼ϵͳ�������������� Power Users ��ĳ�Ա�����ܰ�װ������
SetupAppRunningError=��װ�����⵽��%1���������С�%n%n��ر������н��̣�Ȼ�󵥻���ȷ�����Լ��������ߵ�����ȡ�������˳���
UninstallAppRunningError=ж�س����⵽��%1���������С�%n%n��ر������н��̣�Ȼ�󵥻���ȷ�����Լ��������ߵ�����ȡ�������˳���

; *** Misc. errors
ErrorCreatingDir=��װ�����޷������ļ��С�%1��
ErrorTooManyFilesInDir=�����ļ��С�%1���а�����̫����ļ����޷������д������ļ�

; *** Setup common messages
ExitSetupTitle=�˳���װ
ExitSetupMessage=��װ��δ��ɡ�����������˳���Ӧ�ó���û�б���װ��%n%n������������ʱ���������������װ��������ɰ�װ��%n%n��ȷ��Ҫ�˳���װ������
AboutSetupMenuItem=���ڰ�װ����(&A)...
AboutSetupTitle=���ڰ�װ����
AboutSetupMessage=%1 �汾 %2%n%3%n%n%1 ��ַ��%n%4
AboutSetupNote=
TranslatorNote=

; *** Buttons
ButtonBack=< ��һ��(&B)
ButtonNext=��һ��(&N) >
ButtonInstall=��װ(&I)
ButtonOK=ȷ��
ButtonCancel=ȡ��
ButtonYes=��(&Y)
ButtonYesToAll=ȫ��(&A)
ButtonNo=��(&N)
ButtonNoToAll=ȫ��(&O)
ButtonFinish=���(&F)
ButtonBrowse=���(&B)...
ButtonWizardBrowse=���(&R)...
ButtonNewFolder=�������ļ���(&M)

; *** "Select Language" dialog messages
SelectLanguageTitle=ѡ��װ����
SelectLanguageLabel=��ѡ���ڰ�װ������ʹ�õ����ԣ�

; *** Common wizard text
ClickNext=��������һ����������װ��������ȡ�����˳���װ��
BeveledLabel=
BrowseDialogTitle=����ļ���
BrowseDialogLabel=����������б���ѡ��һ���ļ��У�Ȼ�󵥻���ȷ������
NewFolderName=�½��ļ���

; *** "Welcome" wizard page
WelcomeLabel1=��ӭʹ�á�[name]����װ��
WelcomeLabel2=���������ļ�����ϰ�װ��[name/ver]����%n%n�������ڼ�����װ֮ǰ�ر�����������Ӧ�ó���

; *** "Password" wizard page
WizardPassword=����
PasswordLabel1=����װ�����뱣����
PasswordLabel3=���������룬��������һ�����Լ�����װ����ע�������Ǵ�Сд���еġ�
PasswordEditLabel=����(&P)��
IncorrectPassword=����������벻��ȷ��������һ�Ρ�

; *** "License Agreement" wizard page
WizardLicense=���Э��
LicenseLabel=�ڼ�����װ֮ǰ�����Ķ��������Ҫ��Ϣ��
LicenseLabel3=���Ķ���������Э�顣������ͬ�����Э���ϵ�����ſ��Լ�����װ��
LicenseAccepted=��ͬ�����Э���ϵ�����(&A)
LicenseNotAccepted=�Ҳ�ͬ�����Э���ϵ�����(&D)

; *** "Information" wizard pages
WizardInfoBefore=��Ϣ
InfoBeforeLabel=�ڼ�����װ֮ǰ�����Ķ��������Ҫ��Ϣ��
InfoBeforeClickLabel=����׼���ü������а�װʱ���뵥������һ������
WizardInfoAfter=��Ϣ
InfoAfterLabel=�ڼ�����װ֮ǰ�����Ķ��������Ҫ��Ϣ��
InfoAfterClickLabel=����׼���ü������а�װʱ���뵥������һ������

; *** "User Information" wizard page
WizardUserInfo=�û���Ϣ
UserInfoDesc=������������Ϣ��
UserInfoName=�û�����(&U)��
UserInfoOrg=��֯(&O)��
UserInfoSerial=��Ʒ���к�(&S)��
UserInfoNameRequired=�����������û�������

; *** "Select Destination Location" wizard page
WizardSelectDir=ѡ��Ŀ���ļ���
SelectDirDesc=�����㽫 [name] ��װ�ںδ���
SelectDirLabel3=��װ���򽫰� [name] ��װ��������ļ��С�
SelectDirBrowseLabel=Ҫ������װ���뵥������һ�������������Ҫѡ��һ����ͬ���ļ��У��뵥�����������
DiskSpaceMBLabel=������Ҫ [mb] MB �Ŀ�����̿ռ䡣
ToUNCPathname=��װ�����ܽ�������װ��һ�� UNC ·���С���������������ϰ�װ������Ҫ����ӳ��������������
InvalidPath=���������������������������·�������磺%n%nC:\APP%n%n������������ʽ�� UNC ·����%n%n\\server\share
InvalidDrive=��������������� UNC �������ڻ��޷����ʡ���ѡ���������������� UNC ����
DiskSpaceWarningTitle=���̿ռ䲻��
DiskSpaceWarning=��װ������Ҫ���� %1 KB ������̿ռ��Խ��а�װ��������ѡ���������ֻʣ�� %2 KB �Ŀռ䡣%n%n��Ҫǿ�м�����װ��
DirNameTooLong=�ļ������ƻ�·������̫����
InvalidDirName=��Ч���ļ������ơ�
BadDirName32=�ļ��������в��ܰ������������г����ַ���%n%n%1
DirExistsTitle=�ļ����Ѵ���
DirExists=�ļ��У�%n%n%1%n%n�Ѵ��ڡ�����ȻҪ��װ������ļ�������
DirDoesntExistTitle=�ļ��в�����
DirDoesntExist=�ļ��У�%n%n%1%n%n�����ڡ���Ҫ��������ļ�����

; *** "Select Components" wizard page
WizardSelectComponents=ѡ�����
SelectComponentsDesc=�����㰲װ��Щ�����
SelectComponentsLabel2=��ѡ����׼����װ��������������׼����װ�����������׼��������װʱ���뵥������һ������
FullInstallation=������װ
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=���Ͱ�װ
CustomInstallation=�Զ��尲װ
NoUninstallWarningTitle=����Ѵ���
NoUninstallWarning=��װ�����鵽��������Ѿ���װ�����ļ�����ϣ�%n%n%1%n%n��ѡ����Щ���������ж�����ǡ�%n%n��Ҫ������
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=��ǰѡ��������Ҫ���� [mb] MB ������̿ռ䡣

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=ѡ�񸽼Ӱ�װ����
SelectTasksDesc=����Ҫ��װ����ִ����Щ���Ӱ�װ����
SelectTasksLabel2=��ѡ�����ڰ�װ [name] ʱ��Ҫִ�еĸ��Ӱ�װ����Ȼ�󵥻�����һ������

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=ѡ��ʼ�˵��ļ���
SelectStartMenuFolderDesc=��װ����Ӧ�ںδ����ÿ�ݷ�ʽ��
SelectStartMenuFolderLabel3=��װ����������Ŀ�ʼ�˵��ļ����з��ÿ�ݷ�ʽ��
SelectStartMenuFolderBrowseLabel=Ҫ������װ���뵥������һ������Ҫѡ�������ļ��У��뵥�����������
MustEnterGroupName=�����������ļ��е����ơ�
GroupNameTooLong=�ļ������ƻ�·��̫����
InvalidGroupName=��Ч���ļ������ơ�
BadGroupName=�ļ��������в��ܰ������������г����ַ���%n%n%1
NoProgramGroupCheck2=��Ҫ������ʼ�˵��ļ���(&D)

; *** "Ready to Install" wizard page
WizardReady=׼����ʼ��װ
ReadyLabel1=��װ��������׼����ʼ�����ļ�����ϰ�װ [name]��
ReadyLabel2a=�뵥������װ���Լ������а�װ��������������㸴������޸�ĳЩ���ã��뵥������һ������
ReadyLabel2b=�뵥������װ���Լ������а�װ
ReadyMemoUserInfo=�û���Ϣ��
ReadyMemoDir=Ŀ���ļ��У�
ReadyMemoType=��װ���ͣ�
ReadyMemoComponents=ѡ��������
ReadyMemoGroup=��ʼ�˵��ļ��У�
ReadyMemoTasks=���Ӱ�װ����

; *** "Preparing to Install" wizard page
WizardPreparing=��װ׼��
PreparingDesc=��װ��������Ϊ�� [name] ��װ�����ļ�����Ͻ���׼����
PreviousInstallNotCompleted=��һ�γ���İ�װ/ɾ��������δ��ɡ�����Ҫ������������������ٴ����б���װ��������� [name] �İ�װ��
CannotContinue=��װ�����޷��������뵥����ȡ�������˳���

; *** "Installing" wizard page
WizardInstalling=���ڰ�װ
InstallingLabel=��װ�������ڽ� [name] ��װ�����ļ�����У����Ժ�

; *** "Setup Completed" wizard page
FinishedHeadingLabel=��� [name] ��װ��
FinishedLabelNoIcons=��װ�����Ѿ��� [name] ��װ�����ļ�����ϡ�
FinishedLabel=��װ�����Ѿ��� [name] ��װ�����ļ�����ϡ�������ͨ���Ѿ���װ�Ŀ�ݷ�ʽͼ��������Ӧ�ó���
ClickFinish=�뵥������ɡ��˳���װ����
FinishedRestartLabel=Ҫ��� [name] �İ�װ����װ������������������ļ��������׼�����ھ�����������
FinishedRestartMessage=Ҫ��� [name] �İ�װ����װ������������������ļ������%n%n��׼�����ھ�����������
ShowReadmeCheck=�ǵģ���׼���鿴�����ļ�
YesRadio=�ǣ����ھ��������������(&Y)
NoRadio=����׼���Ժ��������������(&N)
; used for example as 'Run MyProg.exe'
RunEntryExec=���С�%1��
; used for example as 'View Readme.txt'
RunEntryShellExec=�鿴��%1��

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=��װ������Ҫ��һ����װ����
SelectDiskLabel2=����밲װ���� %1 ��������ȷ������%n%n����ð�װ�����ϵ��ļ������������г���·�������ã���������ȷ��·���򵥻����������
PathLabel=·��(&P)��
FileNotInDir2=�޷��ڡ�%2���л�ȡ�ļ���%1�����������ȷ�İ�װ���ʻ�ѡ�������ļ��С�
SelectDirectoryLabel=��ָ����һ����װ���ʵ�λ�á�

; *** Installation phase messages
SetupAborted=��װ������δ��ɡ�%n%n���������ֵ����Ⲣ�ٴ����а�װ����
EntryAbortRetryIgnore=���������ԡ��ٳ���һ�Σ����������ԡ�ǿ�Ƽ��������ߵ�������ֹ��ȡ����װ��

; *** Installation status messages
StatusCreateDirs=���ڴ����ļ��С���
StatusExtractFiles=���ڽ�ѹ���ļ�����
StatusCreateIcons=���ڴ�����ݷ�ʽ����
StatusCreateIniEntries=���ڴ��� INI ��Ŀ����
StatusCreateRegistryEntries=���ڴ���ע�����Ŀ����
StatusRegisterFiles=����ע���ļ�����
StatusSavingUninstall=���ڱ���ж����Ϣ����
StatusRunProgram=������ɰ�װ����
StatusRollback=���ڻ�ԭ���ΰ�װ�����ĸĶ�����

; *** Misc. errors
ErrorInternal2=�ڲ�����%1
ErrorFunctionFailedNoCode=%1ʧ��
ErrorFunctionFailed=%1ʧ�ܣ�������룺%2
ErrorFunctionFailedWithMessage=%1ʧ�ܣ�������룺%2��%n%3
ErrorExecutingProgram=�޷������ļ���%n%1

; *** Registry errors
ErrorRegOpenKey=��ע�������ʱ��������%n%1\%2
ErrorRegCreateKey=����ע�������ʱ��������%n%1\%2
ErrorRegWriteKey=д��ע����ֵʱ��������%n%1\%2

; *** INI errors
ErrorIniEntry=���ļ���%1���д��� INI ��Ŀʱ��������

; *** File copying errors
FileAbortRetryIgnore=���������ԡ��ٳ���һ�Σ����������ԡ��������ļ�(���Ƽ�)�����ߵ�������ֹ��ȡ����װ��
FileAbortRetryIgnore2=���������ԡ��ٳ���һ�Σ����������ԡ�ǿ�Ƽ���(���Ƽ�)�����ߵ�������ֹ��ȡ����װ��
SourceIsCorrupted=ԭʼ�ļ���
SourceDoesntExist=ԭʼ�ļ���%1��������
ExistingFileReadOnly=�����ļ���ֻ���ġ�%n%n���������ԡ���ɾ��ֻ�����Բ��ٳ���һ�Σ����������ԡ��������ļ������ߵ�������ֹ��ȡ����װ��
ErrorReadingExistingDest=���Զ�ȡ�����ļ�ʱ��������
FileExists=���ļ��Ѵ��ڡ�%n%n����Ҫ��װ���򸲸�����
ExistingFileNewer=�����ļ��ȱ���װ�����԰�װ�İ汾���¡��������������е��ļ���%n%n��Ҫ���������ļ���
ErrorChangingAttr=�����޸������ļ�����ʱ��������
ErrorCreatingTemp=������Ŀ���ļ����д����ļ�ʱ��������
ErrorReadingSource=���Զ�ȡԭʼ�ļ�ʱ��������
ErrorCopying=���Ը����ļ�ʱ��������
ErrorReplacingExistingFile=�����滻�����ļ�ʱ��������
ErrorRestartReplace=��������ʧ�ܣ�
ErrorRenamingTemp=������Ŀ���ļ����и����ļ�ʱ��������
ErrorRegisterServer=�޷�ע�� DLL/OCX��%1
ErrorRegSvr32Failed=RegSvr32 ����ʧ�ܣ�������Ϊ��%1
ErrorRegisterTypeLib=�޷�ע�����Ϳ⣺%1

; *** Post-installation errors
ErrorOpeningReadme=���Դ������ļ�ʱ��������
ErrorRestartingComputer=��װ�����޷�������������������ֹ�������

; *** Uninstaller messages
UninstallNotFound=�ļ���%1�������ڡ��޷�ִ��ж�ز�����
UninstallOpenError=�ļ���%1���޷��򿪡��޷�ִ��ж�ز�����
UninstallUnsupportedVer=��ǰ�汾��ж�س����޷�ʶ��ж����־�ļ���%1���ĸ�ʽ���޷�ִ��ж�ز�����
UninstallUnknownEntry=ж����־�ļ�����δ֪��Ŀ(%1)
ConfirmUninstall=��ȷ��Ҫ��ȫж�� %1 ���������������
UninstallOnlyOnWin64=�ð�װֻ���� 64 λ Windows ϵͳ��ж�ء�
OnlyAdminCanUninstall=�ð�װֻ���ɾ���ϵͳ����ԱȨ�޵��û�ж�ء�
UninstallStatusLabel=���ڴ����ļ������ж�� %1�����Ժ�
UninstalledAll= %1 �Ѿ��ɹ������ļ������ж�ء�
UninstalledMost= %1 ж����ɡ�%n%nһЩ����û�б�ɾ�����������ֹ�ɾ�����ǡ�
UninstalledAndNeedsRestart=Ҫ��� %1 ��ж�أ���Ҫ�����������ļ������%n%n��׼�����ھ�����������
UninstallDataCorrupted=�ļ���%1���𻵡��޷�ִ��ж�ز�����

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=ɾ�������ļ���
ConfirmDeleteSharedFile2=ϵͳ��ʾ���¹����ļ������ٱ��κγ���ʹ�á�������ж����Щ�����ļ���%n%n�������������Ȼ��Ҫʹ������ļ�������ɾ���ˣ���Щ���򽫿����޷����С��������ȷ������ѡ�񡰷񡱡�������ϵͳ�б�����Щ�ļ���������κ��˺���
SharedFileNameLabel=�ļ�����
SharedFileLocationLabel=λ�ã�
WizardUninstalling=ж��״̬
StatusUninstalling=����ж�� %1����

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 �汾 %2
AdditionalIcons=����ͼ�꣺
CreateDesktopIcon=��������ͼ��(&D)
CreateQuickLaunchIcon=������������ͼ��(&Q)
ProgramOnTheWeb=�����ϵġ�%1��
UninstallProgram=ж�ء�%1��
LaunchProgram=���С�%1��
AssocFileExtension=����չ��Ϊ��%2�����ļ���������%1��(&A)
AssocingFileExtension=���ڽ���չ��Ϊ��%2�����ļ���������%1������
