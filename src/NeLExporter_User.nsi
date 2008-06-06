; Name: NeL Exporter
; Authors: Jason Oppel
; Version: v0.0.0.1
; Install script for the NeL Exporter
; Copyright (c) 2007 Ironic Entertainment
; All Rights Reserved

;if then logic - jmoppel
!include "Sections.nsh"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "NeL Exporter"
!define PRODUCT_VERSION "0.0.0.1"
!define PRODUCT_PUBLISHER "Ironic Entertainment"
!define PRODUCT_WEB_SITE "http://www.ironicentertainment.com"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "resources\werewolf_logo_single_w_32x32.ico"
;!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_UNICON "resources\werewolf_logo_single_w_32x32.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "resources\GPLv2-NonDistrib.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\ReadMe.html"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Var MayaSupVer

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "NeLExporter_Setup_v${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}" "Install_Dir"
ShowInstDetails show
ShowUnInstDetails show

;maya install path detection routines
!macro Maya85InstPath
  ;DetailPrint "Hello ${What} ${When}"
  ReadRegStr $R0 HKLM \
  "SOFTWARE\Autodesk\Maya\8.5\Setup\InstallPath\" MAYA_INSTALL_LOCATION
  ;IfErrors lbl_nomaya85
  ;lbl_nomaya85:
!macroend

!macro Maya2008InstPath
  ReadRegStr $R1 HKLM \
  "SOFTWARE\Autodesk\Maya\2008\Setup\InstallPath\" MAYA_INSTALL_LOCATION
  ;IfErrors lbl_nomaya2008
  ;lbl_nomaya2008:
!macroend

!macro AppenduserSetup MayaUsrPath nelMenuTF
  ;!insertmacro AppendFile "${MayaUsrPath}userSetup.mel" "// Werewolf userSetup.mel - Copyright (C) 2007 Ironic Entertainment"
  !insertmacro AppendFile "${MayaUsrPath}userSetup.mel" " nelMenu ${nelMenuTF};"
  ;!insertmacro AppendFile "${MayaUsrPath}userSetup.mel" "// End Werewolf userSetUp.mel"
!macroend

!macro AppendFile File2App Text2App
  FileOpen $4 "${File2App}" a
  FileSeek $4 0 END
  ;FileWrite $4 "$\r$\n" ; we write a new line
  FileWrite $4 "${Text2App}"
  FileWrite $4 "$\r$\n" ; we write an extra line
  FileClose $4 ; and close the file
!macroend

!macro WriteFile File2Write Text2Write
  FileOpen $4 "${File2Write}" w
  FileWrite $4 "${Text2Write}"
  FileClose $4 ; and close the file
!macroend

!macro BackupFile FILE_DIR FILE BACKUP_TO
 IfFileExists "${BACKUP_TO}\*.*" +2
  CreateDirectory "${BACKUP_TO}"
 IfFileExists "${FILE_DIR}\${FILE}" 0 +2
 IfFileExists "${BACKUP_TO}\${FILE}" +2 0 ;don't replace an existing backup
  Rename "${FILE_DIR}\${FILE}" "${BACKUP_TO}\${FILE}"
!macroend

!macro MayaConf MayaVer
  CreateDirectory "$DOCUMENTS\maya\${MayaVer}\modules"
  !insertmacro WriteFile "$DOCUMENTS\maya\${MayaVer}\Modules\NeL" "+ NeL 6.0 $INSTDIR"
  ${If} ${FileExists} "$DOCUMENTS\maya\projects\default\mel\userSetup.mel"
    !insertmacro AppenduserSetup "$DOCUMENTS\maya\projects\default\mel\" "false"
  ${ElseIf} ${FileExists} "$DOCUMENTS\maya\${MayaVer}\scripts\userSetup.mel"
    !insertmacro AppenduserSetup "$DOCUMENTS\maya\${MayaVer}\scripts\" "false"
  ${ElseIf} ${FileExists} "$DOCUMENTS\maya\scripts\userSetup.mel"
    !insertmacro AppenduserSetup "$DOCUMENTS\maya\scripts\" "false"
  ${Else}
    !insertmacro AppenduserSetup "$DOCUMENTS\maya\${MayaVer}\scripts\" "false"
  ${EndIf}
!macroend

;!macro RestoreFile BUP_DIR FILE RESTORE_TO
; IfFileExists "${BUP_DIR}\${FILE}" 0 +2
;  Rename "${BUP_DIR}\${FILE}" "${RESTORE_TO}\${FILE}"
;!macroend

Section "Common Components" COMMON
  SectionIn RO
  WriteRegStr HKLM "Software\Ironic Entertainment\NeL Exporter" "Install_Dir" "$INSTDIR"
  SetOverwrite ifnewer
  SetOutPath "$INSTDIR\scripts"
  File "maya\exporter\Binary\NeL\scripts\AENeLShader.mel"
  SetOutPath "$INSTDIR\scripts"
  File "maya\exporter\Binary\NeL\scripts\nelAddBaseAttrs.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAddForceAnimationExport.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAddTransformAttrs.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAssignBillboardColorRange.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAssignEdgeVisibility.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAssignFillTiles.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAssignInterfaceMeshes.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAssignMRMAttrs.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAssignRestoreLeafSlotAttributes.mel"
  File "maya\exporter\Binary\NeL\scripts\nelAssignRestoreSlotAttribtues.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2Cluster.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2ClusterBase.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2CollisionMesh.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2Instance.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2MRM.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2NelShader.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2NelWaterShader.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2Portal.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2Veg.mel"
  File "maya\exporter\Binary\NeL\scripts\nelConvert2Zone.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateBBCluster.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateBBSlot.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateBillboard.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateClusterBase.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateCollisionFromFaces.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateCollisionMeshes.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateCreature.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateLeafCluster.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateLeafSlot.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateLitIGs.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateLod.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateNelShader.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateNelWaterShader.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreatePatchPainter.mel"
  File "maya\exporter\Binary\NeL\scripts\nelCreateSlotBase.mel"
  File "maya\exporter\Binary\NeL\scripts\nelExportOptions.mel"
  File "maya\exporter\Binary\NeL\scripts\nelExportTexture.mel"
  File "maya\exporter\Binary\NeL\scripts\nelGUICollisionPaint.mel"
  File "maya\exporter\Binary\NeL\scripts\nelGUICreateLitIGs.mel"
  File "maya\exporter\Binary\NeL\scripts\nelGUICreatureEditor.mel"
  File "maya\exporter\Binary\NeL\scripts\nelGUILod.mel"
  File "maya\exporter\Binary\NeL\scripts\nelGUIMaya2NelUnits.mel"
  File "maya\exporter\Binary\NeL\scripts\nelGUITilePaint.mel"
  File "maya\exporter\Binary\NeL\scripts\nelListCreatures.mel"
  File "maya\exporter\Binary\NeL\scripts\nelMenu.mel"
  File "maya\exporter\Binary\NeL\scripts\nelPerformAssignEdgeVisibility.mel"
  File "maya\exporter\Binary\NeL\scripts\nelPerformAssignMRMAttrs.mel"
  File "maya\exporter\Binary\NeL\scripts\nelPreviewBB.mel"
  ;File "maya\exporter\Binary\NeL\scripts\userSetup.mel"
  SetOutPath "$INSTDIR"
  File "resources\${PRODUCT_NAME}.url"
  File "resources\Readme.html"
  File "resources\main.css"
; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateShortCut "Read Me.lnk" "$INSTDIR\Readme.html"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Base (Maya 8.5)" MAYA85
  SetOutPath "$INSTDIR\plug-ins"
  SetOverwrite ifnewer
  File "maya\exporter\Binary\NeL\plug-ins\NeLExporter_v8.5.mll"
  SetOutPath "$R0scripts\AETemplates"
  SetOverwrite on
  !insertmacro BackupFile "$R0scripts\AETemplates" "AEphongTemplate.mel" "$R0scripts\AETemplates\backup"
  File "maya\exporter\Binary\AETemplates\AEphongTemplate.mel"
  !insertmacro MayaConf "8.5"
; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Base (Maya 2008)" MAYA2008
  SetOutPath "$INSTDIR\plug-ins"
  SetOverwrite ifnewer
  File "maya\exporter\Binary\NeL\plug-ins\NeLExporter_v2008.mll"
  SetOutPath "$R1scripts\AETemplates"
  SetOverwrite on
  !insertmacro BackupFile "$R1scripts\AETemplates" "AEphongTemplate.mel" "$R1scripts\AETemplates\backup"
  File "maya\exporter\Binary\AETemplates\AEphongTemplate.mel"
  !insertmacro MayaConf "2008"
; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Function un.AdvReplaceInFile
  Exch $0 ;file to replace in
  Exch
  Exch $1 ;number to replace after
  Exch
  Exch 2
  Exch $2 ;replace and onwards
  Exch 2
  Exch 3
  Exch $3 ;replace with
  Exch 3
  Exch 4
  Exch $4 ;to replace
  Exch 4
  Push $5 ;minus count
  Push $6 ;universal
  Push $7 ;end string
  Push $8 ;left string
  Push $9 ;right string
  Push $R0 ;file1
  Push $R1 ;file2
  Push $R2;read
  Push $R3 ;universal
  Push $R4 ;count (onwards)
  Push $R5 ;count (after)
  Push $R6 ;temp file name

  GetTempFileName $R6
  FileOpen $R1 $0 r ;file to search in
  FileOpen $R0 $R6 w ;temp file
   StrLen $R3 $4
   StrCpy $R4 -1
   StrCpy $R5 -1

  loop_read:
   ClearErrors
   FileRead $R1 $R2 ;read line
   IfErrors exit

     StrCpy $5 0
     StrCpy $7 $R2

  loop_filter:
     IntOp $5 $5 - 1
     StrCpy $6 $7 $R3 $5 ;search
     StrCmp $6 "" file_write2
     StrCmp $6 $4 0 loop_filter

  StrCpy $8 $7 $5 ;left part
  IntOp $6 $5 + $R3
  IntCmp $6 0 is0 not0
  is0:
  StrCpy $9 ""
  Goto done
  not0:
  StrCpy $9 $7 "" $6 ;right part
  done:
  StrCpy $7 $8$3$9 ;re-join

  IntOp $R4 $R4 + 1
  StrCmp $2 all file_write1
  StrCmp $R4 $2 0 file_write2
  IntOp $R4 $R4 - 1

  IntOp $R5 $R5 + 1
  StrCmp $1 all file_write1
  StrCmp $R5 $1 0 file_write1
  IntOp $R5 $R5 - 1
  Goto file_write2

  file_write1:
   FileWrite $R0 $7 ;write modified line
  Goto loop_read

  file_write2:
   FileWrite $R0 $R2 ;write unmodified line
  Goto loop_read

  exit:
    FileClose $R0
    FileClose $R1

  SetDetailsPrint none
  Delete $0
  Rename $R6 $0
  Delete $R6
  SetDetailsPrint both

  Pop $R6
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Pop $R0
  Pop $9
  Pop $8
  Pop $7
  Pop $6
  Pop $5
  Pop $0
  Pop $1
  Pop $2
  Pop $3
  Pop $4
FunctionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${COMMON} "This is required to use the exporter."
  !insertmacro MUI_DESCRIPTION_TEXT ${MAYA85} "Required for all Maya 8.5 users."
  !insertmacro MUI_DESCRIPTION_TEXT ${MAYA2008} "Required for all Maya 2008 users."
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;run code before installer starts
Function .onInstSuccess
  Delete "$TEMP\vcredis1.cab"
  Delete "$TEMP\vcredist.msi"
FunctionEnd


Function .onInit
  StrCpy $MayaSupVer "8.5/2008"
  ClearErrors
  ReadRegStr $R2 HKLM "Software\Ironic Entertainment\NeL Exporter" "Install_Dir"
  IfErrors +5 0 ;don't need to uninstall what's not there
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1 "Detected another install of ${PRODUCT_NAME}.  First we need to remove the current installation of ${PRODUCT_NAME}.  Ok to uninstall current installtion of ${PRODUCT_NAME}?" IDYES +2
  quit
  Exec '"$R2\uninst.exe" /S'
  MessageBox MB_OK "Uninstall of ${PRODUCT_NAME} complete!"
  IfErrors lbl_nomaya85
  DetailPrint "Maya Located at {$R0}"
  SetOutPath "$TEMP"
  SetOverwrite ifnewer
  File "resources\vcredist.msi"
  File "resources\vcredis1.cab"
  Exec 'msiexec /i "$TEMP\vcredist.msi" /qn'
  lbl_nomaya85:
  
  !insertmacro Maya85InstPath
  ${If} $R0 == ""
    !insertmacro UnselectSection ${MAYA85}
  ${EndIf}

  !insertmacro Maya2008InstPath
  ${If} $R1 == ""
    !insertmacro UnselectSection ${MAYA2008}
  ${EndIf}
  
  ${If} $R0 == ""
  ${AndIf} $R1 == ""
    MessageBox MB_OK "No supported version of Maya found!  Please install Maya $MayaSupVer and re-run this installer."
    quit
  ${EndIf}
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  IfSilent +2
    MessageBox MB_ICONINFORMATION|MB_OK "Ironic NeL Exporter was successfully removed from your computer."
FunctionEnd

Function un.onInit
  IfSilent +3
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove Ironic NeL Exporter and all of its components?" IDYES +2
  Abort
FunctionEnd

Function un.SanitizeUserSetup
  ;make sure that we comment out all nelmenu commands in usersetup.mel
  ${If} ${FileExists} "$DOCUMENTS\maya\projects\default\mel\userSetup.mel"
    Push " nelMenu false;" #text to be replaced
    Push "" #replace with
    Push all #replace all occurrences
    Push all #replace all occurrences
    Push "$DOCUMENTS\maya\projects\default\mel\userSetup.mel" #file to replace in
    Call un.AdvReplaceInFile
  ${ElseIf} ${FileExists} "$DOCUMENTS\maya\2008\scripts\userSetup.mel"
    Push " nelMenu false;" #text to be replaced
    Push "" #replace with
    Push all #replace all occurrences
    Push all #replace all occurrences
    Push "$DOCUMENTS\maya\2008\scripts\userSetup.mel" #file to replace in
    Call un.AdvReplaceInFile
  ${ElseIf} ${FileExists} "$DOCUMENTS\maya\scripts\userSetup.mel"
    Push " nelMenu false;" #text to be replaced
    Push "" #replace with
    Push all #replace all occurrences
    Push all #replace all occurrences
    Push "$DOCUMENTS\maya\scripts\userSetup.mel" #file to replace in
    Call un.AdvReplaceInFile
  ${ElseIf} ${FileExists} "$DOCUMENTS\maya\8.5\scripts\userSetup.mel"
    Push " nelMenu false;" #text to be replaced
    Push "" #replace with
    Push all #replace all occurrences
    Push all #replace all occurrences
    Push "$DOCUMENTS\maya\2008\scripts\userSetup.mel" #file to replace in
    Call un.AdvReplaceInFile
  ${EndIf}
FunctionEnd


Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  ;sanitize userSetup.mel
  Call un.SanitizeUserSetup
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\maya\Werewolf\Werewolf"
  Delete "$DOCUMENTS\maya\2008\modules\NeL"
  Delete "$INSTDIR\plug-ins\NeLExporter_v2008.mll"
  Delete "$DOCUMENTS\maya\8.5\modules\NeL"
  Delete "$INSTDIR\plug-ins\NeLExporter_v8.5.mll"
  Delete "$INSTDIR\Readme.html"
  Delete "$INSTDIR\main.css"
  !insertmacro Maya2008InstPath
  Delete "$R1scripts\AETemplates\AEphongTemplate.mel"
  Rename "$R1scripts\AETemplates\backup\AEphongTemplate.mel" "$R1scripts\AETemplates\AEphongTemplate.mel"
  !insertmacro Maya85InstPath
  Delete "$R0scripts\AETemplates\AEphongTemplate.mel"
  Rename "$R0scripts\AETemplates\backup\AEphongTemplate.mel" "$R1scripts\AETemplates\AEphongTemplate.mel"
  Delete "$INSTDIR\scripts\nelPreviewBB.mel"
  Delete "$INSTDIR\scripts\nelPerformAssignMRMAttrs.mel"
  Delete "$INSTDIR\scripts\nelPerformAssignEdgeVisibility.mel"
  Delete "$INSTDIR\scripts\nelMenu.mel"
  Delete "$INSTDIR\scripts\nelListCreatures.mel"
  Delete "$INSTDIR\scripts\nelGUITilePaint.mel"
  Delete "$INSTDIR\scripts\nelGUIMaya2NelUnits.mel"
  Delete "$INSTDIR\scripts\nelGUILod.mel"
  Delete "$INSTDIR\scripts\nelGUICreatureEditor.mel"
  Delete "$INSTDIR\scripts\nelGUICreateLitIGs.mel"
  Delete "$INSTDIR\scripts\nelGUICollisionPaint.mel"
  Delete "$INSTDIR\scripts\nelExportTexture.mel"
  Delete "$INSTDIR\scripts\nelExportOptions.mel"
  Delete "$INSTDIR\scripts\nelCreateSlotBase.mel"
  Delete "$INSTDIR\scripts\nelCreatePatchPainter.mel"
  Delete "$INSTDIR\scripts\nelCreateNelWaterShader.mel"
  Delete "$INSTDIR\scripts\nelCreateNelShader.mel"
  Delete "$INSTDIR\scripts\nelCreateLod.mel"
  Delete "$INSTDIR\scripts\nelCreateLitIGs.mel"
  Delete "$INSTDIR\scripts\nelCreateLeafSlot.mel"
  Delete "$INSTDIR\scripts\nelCreateLeafCluster.mel"
  Delete "$INSTDIR\scripts\nelCreateCreature.mel"
  Delete "$INSTDIR\scripts\nelCreateCollisionMeshes.mel"
  Delete "$INSTDIR\scripts\nelCreateCollisionFromFaces.mel"
  Delete "$INSTDIR\scripts\nelCreateClusterBase.mel"
  Delete "$INSTDIR\scripts\nelCreateBillboard.mel"
  Delete "$INSTDIR\scripts\nelCreateBBSlot.mel"
  Delete "$INSTDIR\scripts\nelCreateBBCluster.mel"
  Delete "$INSTDIR\scripts\nelConvert2Zone.mel"
  Delete "$INSTDIR\scripts\nelConvert2Veg.mel"
  Delete "$INSTDIR\scripts\nelConvert2Portal.mel"
  Delete "$INSTDIR\scripts\nelConvert2NelWaterShader.mel"
  Delete "$INSTDIR\scripts\nelConvert2NelShader.mel"
  Delete "$INSTDIR\scripts\nelConvert2MRM.mel"
  Delete "$INSTDIR\scripts\nelConvert2Instance.mel"
  Delete "$INSTDIR\scripts\nelConvert2CollisionMesh.mel"
  Delete "$INSTDIR\scripts\nelConvert2ClusterBase.mel"
  Delete "$INSTDIR\scripts\nelConvert2Cluster.mel"
  Delete "$INSTDIR\scripts\nelAssignRestoreSlotAttribtues.mel"
  Delete "$INSTDIR\scripts\nelAssignRestoreLeafSlotAttributes.mel"
  Delete "$INSTDIR\scripts\nelAssignMRMAttrs.mel"
  Delete "$INSTDIR\scripts\nelAssignInterfaceMeshes.mel"
  Delete "$INSTDIR\scripts\nelAssignFillTiles.mel"
  Delete "$INSTDIR\scripts\nelAssignEdgeVisibility.mel"
  Delete "$INSTDIR\scripts\nelAssignBillboardColorRange.mel"
  Delete "$INSTDIR\scripts\nelAddTransformAttrs.mel"
  Delete "$INSTDIR\scripts\nelAddForceAnimationExport.mel"
  Delete "$INSTDIR\scripts\nelAddBaseAttrs.mel"
  Delete "$INSTDIR\scripts\AENeLShader.mel"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  Delete "$INSTDIR\Read Me.lnk"
  RMDir "$STARTMENU\${PRODUCT_PUBLISHER}"
  RMDir "$PROGRAMFILES\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
  RMDir "$INSTDIR\scripts"
  RMDir "$INSTDIR\plug-ins"
  RMDir "$INSTDIR"
  RMDir "$PROGRAMFILES\${PRODUCT_PUBLISHER}"
  RMDir "$R0\scripts\AETemplates\backup"
  RMDir "$R1\scripts\AETemplates\backup"
  RMDir "$INSTDIR\backup"
  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  Delete "$SMPROGRAMS\${PRODUCT_PUBLISHER}\*.*"
  RMDir "$SMPROGRAMS\${PRODUCT_PUBLISHER}"
  
  RMDir "$DOCUMENTS\maya\8.5\Modules"
  RMDir "$DOCUMENTS\maya\2008\Modules"
  
  DeleteRegKey HKLM "Software\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
  DeleteRegKey HKLM "Software\${PRODUCT_PUBLISHER}"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetAutoClose true
SectionEnd
