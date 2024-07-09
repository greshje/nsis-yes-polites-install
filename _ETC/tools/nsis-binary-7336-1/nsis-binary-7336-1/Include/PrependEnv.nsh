!include LogicLib.nsh
!include WinCore.nsh

Function RegPrependString
System::Store S
Pop $R0 ; append
Pop $R1 ; separator
Pop $R2 ; reg value
Pop $R3 ; reg path
Pop $R4 ; reg hkey
System::Call 'ADVAPI32::RegCreateKey(i$R4,tR3,*p.r1)i.r0'
${If} $0 = 0
    System::Call 'ADVAPI32::RegQueryValueEx(pr1,tR2,p0,*i.r2,p0,*i0r3)i.r0'
    ${If} $0 <> 0
        StrCpy $2 ${REG_SZ}
        StrCpy $3 0
    ${EndIf}
    StrLen $4 $R0
    StrCpy $8 $4
    StrLen $5 $R1
    IntOp $4 $4 + $5
    System::Call '*(&t$4s,&t$5,&t$3,&t1"")p.r9' $R0
    ${If} $0 = 0
    ${AndIf} $3 <> 0
      System::Call 'KERNEL32::lstrcat(t)(pr9,tR1)'
      IntOp $8 $8 + $5
    ${EndIf}
     IntOp $3 $3 + 1 ; For \0
    !if ${NSIS_CHAR_SIZE} > 1
      IntOp $3 $3 * ${NSIS_CHAR_SIZE}
      IntOp $8 $8 * ${NSIS_CHAR_SIZE}
    !endif
    IntPtrOp $8 $9 + $8
    System::Call 'ADVAPI32::RegQueryValueEx(pr1,tR2,p0,p0,pr8,*ir3)i.r0'
    ${If} $0 = 0
    ${OrIf} $0 = ${ERROR_FILE_NOT_FOUND}
        System::Call 'KERNEL32::lstrlen(t)(pr9)i.r0'
        IntOp $0 $0 + 1
        !if ${NSIS_CHAR_SIZE} > 1
            IntOp $0 $0 * ${NSIS_CHAR_SIZE}
        !endif
        System::Call 'ADVAPI32::RegSetValueEx(pr1,tR2,i0,ir2,pr9,ir0)i.r0'
    ${EndIf}
    System::Free $9
    System::Call 'ADVAPI32::RegCloseKey(pr1)'
${EndIf}
Push $0
System::Store L
FunctionEnd

Section

# Push ${HKEY_CURRENT_USER}
# Push "Environment"
# Push "Path"
# Push ";"
# Push "c:\whatever"
# Call RegPrependString
# Pop $0
# DetailPrint RegPrependString:Error=$0

SectionEnd 