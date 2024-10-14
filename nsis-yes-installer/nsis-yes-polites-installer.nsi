# ---
#
# This script creates the installer for the _YES_POLITES Eclipse environment.  
# Documentation for the _YES Eclipse environment is available at:
# https://nachc-cad.github.io/fhir-to-omop/pages/navbar/how-tos/developer-how-tos/install-eclipse/InstallEclipse.html
#
# For this script to work you will need to install the EnVar plug in.  
# https://nsis.sourceforge.io/EnVar_plug-in 
#
# Documentation for the EnVar API is here:
# https://github.com/GsNSIS/EnVar
#
# Instructions for plugin install
# From: https://nsis.sourceforge.io/How_can_I_install_a_plugin 
#
#   After downloading and unpacking a plugin one needs to accept the license agreement (residing in the "license.txt" file if applicable). 
#   Then the ".dll" files must be put into the "NSIS\Plugins[\platform]" subfolder and the ".nsh" file into the "NSIS\Include" subfolder.
#   NSIS v2 only supports ANSI plug-ins and they are stored in "NSIS\Plugins". 
#   NSIS v3 supports multiple targets and plugins are stored in subfolders under "NSIS\Plugins\".
#
# WriteEnvStr.nsh was downloaded from the following url (put the .nsh file in <NSIS>/Include folder):
# https://github.com/rasa/nsislib/blob/master/WriteEnvStr.nsh
# 
# ---


# includes
!include WriteEnvStr.nsh
!include PrependEnv.nsh

#
# defines
#

# env variables
!define JAVA_HOME "C:\_YES_POLITES\tools\java\jdk-11.0.11"
!define JAVA_VERSION "C:\_YES_POLITES\tools\java\jdk-11.0.11\bin"

# path additions/modifications
!define JAVA_PATH "%JAVA_VERSION%"
!define GIT_PATH "C:\_YES_POLITES\tools\git\Git-2.27.0\bin;"
!define MVN_PATH "C:\_YES_POLITES\tools\mvn\apache-maven-3.6.3\bin;"
!define BAT_PATH "C:\_YES_POLITES\tools\bat;"
!define R_PATH "C:\_YES_POLITES\tools\r\R\R-4.2.3\bin;"
!define SQL_SERVER_DLL_PATH "C:\_YES_POLITES\databases\sqlserver\sqljdbc_12.6.2.0_enu\sqljdbc_12.6\enu\auth\x64;"

# definitions
<<<<<<< HEAD
Outfile "YesPolitesInstaller-1.2.048.exe"
=======
Outfile "YesPolitesInstaller-1.2.046.exe"
>>>>>>> 3628d2008e72c22e1702ed4f0e5e0898c5837076
InstallDir "C:\_YES_POLITES"

Page Directory
Page InstFiles

ShowInstDetails show


#
# Main section
#

Section

	IfFileExists "$InstDir\*.*" file_found file_not_found
	file_not_found:
	
		#
		# copy files
		#
			
		DetailPrint ""
		DetailPrint "Copying files to $InstDir..."
		DetailPrint ""
		SetOutPath "$InstDir"
		File /a /r "D:\_YES_POLITES_SRC\_YES_POLITES\"

		#
		# create environment variables
		#
	
		# JAVA_HOME
	    DetailPrint ""
	    DetailPrint "Updating JAVA_HOME Environment Variable (this takes a few seconds)..."
		Push JAVA_HOME
		Push '${JAVA_HOME}'
		Call WriteEnvStr
		
		# JAVA_VERSION
	    DetailPrint ""
	    DetailPrint "Updating JAVA_VERSION Environment Variable (this takes a few seconds)..."
		Push JAVA_VERSION
		Push '${JAVA_VERSION}'
		Call WriteEnvStr
	
		#
		# modifications to path env variable
		#
	
		# set env to current user
		DetailPrint ""
		DetailPrint "Settin env to Current User..."
		EnVar::SetHKCU
	
		#
		# r
		#
	
		# remove ${R_PATH} from path
		DetailPrint ""
		DetailPrint "Removing existing instance of R_PATH from Path"
		EnVar::DeleteValue "Path" "${R_PATH}"
		Pop $0
		DetailPrint "EnVar::Check returned=|$0| (should be 0)"  

		# prepend our ${R_PATH} to the path env variable
		DetailPrint ""
		DetailPrint "Prepending ${R_PATH}"
		Push ${HKEY_CURRENT_USER}
		Push "Environment"
		Push "Path"
		Push ";"
		Push "${R_PATH}"
		Call RegPrependString
		Pop $0
		DetailPrint "RegPrependString:Error=$0 (Should be 0)"
		
		#
		# bat
		#
	
		# remove ${BAT_PATH} from path
		DetailPrint ""
		DetailPrint "Removing existing instance of BAT_PATH from Path"
		EnVar::DeleteValue "Path" "${BAT_PATH}"
		Pop $0
		DetailPrint "EnVar::Check returned=|$0| (should be 0)"  

		# prepend our ${BAT_PATH} to the path env variable
		DetailPrint ""
		DetailPrint "Prepending ${BAT_PATH}"
		Push ${HKEY_CURRENT_USER}
		Push "Environment"
		Push "Path"
		Push ";"
		Push "${BAT_PATH}"
		Call RegPrependString
		Pop $0
		DetailPrint "RegPrependString:Error=$0 (Should be 0)"
		
		#
		# mvn
		#
	
		# remove ${MVN_PATH} from path
		DetailPrint ""
		DetailPrint "Removing existing instance of MVN_PATH from Path"
		EnVar::DeleteValue "Path" "${MVN_PATH}"
		Pop $0
		DetailPrint "EnVar::Check returned=|$0| (should be 0)"  

		# prepend our ${MVN_PATH} to the path env variable
		DetailPrint ""
		DetailPrint "Prepending ${MVN_PATH}"
		Push ${HKEY_CURRENT_USER}
		Push "Environment"
		Push "Path"
		Push ";"
		Push "${MVN_PATH}"
		Call RegPrependString
		Pop $0
		DetailPrint "RegPrependString:Error=$0 (Should be 0)"
	
		#
		# git
		#
	
		# remove ${GIT_PATH} from path
		DetailPrint ""
		DetailPrint "Removing existing instance of GIT_PATH from Path"
		EnVar::DeleteValue "Path" "${GIT_PATH}"
		Pop $0
		DetailPrint "EnVar::Check returned=|$0| (should be 0)"  

		# prepend our ${GIT_PATH} to the path env variable
		DetailPrint ""
		DetailPrint "Prepending ${GIT_PATH}"
		Push ${HKEY_CURRENT_USER}
		Push "Environment"
		Push "Path"
		Push ";"
		Push "${GIT_PATH}"
		Call RegPrependString
		Pop $0
		DetailPrint "RegPrependString:Error=$0 (Should be 0)"
	
		#
		# java
		#
	
		# remove ${JAVA_PATH} from path
		DetailPrint ""
		DetailPrint "Removing existing instance of JAVA_PATH from Path"
		EnVar::DeleteValue "Path" "${JAVA_PATH}"
		Pop $0
		DetailPrint "EnVar::Check returned=|$0| (should be 0)"  

		# prepend our ${JAVA_PATH} to the path env variable
		DetailPrint ""
		DetailPrint "Prepending ${JAVA_PATH}"
		Push ${HKEY_CURRENT_USER}
		Push "Environment"
		Push "Path"
		Push ";"
		Push "${JAVA_PATH}"
		Call RegPrependString
		Pop $0
		DetailPrint "RegPrependString:Error=$0 (Should be 0)"

		#
		# SQL_SERVER_DLL_PATH
		#
	
		# remove ${SQL_SERVER_DLL_PATH} from path
		DetailPrint ""
		DetailPrint "Removing existing instance of SQL_SERVER_DLL_PATH from Path"
		EnVar::DeleteValue "Path" "${SQL_SERVER_DLL_PATH}"
		Pop $0
		DetailPrint "EnVar::Check returned=|$0| (should be 0)"  

		# prepend our ${SQL_SERVER_DLL_PATH} to the path env variable
		DetailPrint ""
		DetailPrint "Prepending ${SQL_SERVER_DLL_PATH}"
		Push ${HKEY_CURRENT_USER}
		Push "Environment"
		Push "Path"
		Push ";"
		Push "${SQL_SERVER_DLL_PATH}"
		Call RegPrependString
		Pop $0
		DetailPrint "RegPrependString:Error=$0 (Should be 0)"

		#
		# configure git to use the installed certificate
		#

		DetailPrint ""		
		DetailPrint "Configuring git certificate..."
		Exec '$InstDir\tools\git\Git-2.27.0\bin\git.exe config --system http.sslcainfo "C:\_YES_POLITES\tools\git\Git-2.27.0\mingw64\ssl\certs\ca-bundle.crt"'
		Pop $0
		Pop $1
		DetailPrint "RegPrependString:Error=$0 (Should be 0)"
		DetailPrint "$1"
	
		#
		# run jv 11 (this is required for Polites)
		#

		DetailPrint ""		
		DetailPrint "Setting default JVM..."
		Exec '$InstDir\tools\bat\jv.bat 11'
		Pop $0
		Pop $1
		DetailPrint "RegPrependString:Error=$0 (Should be 0)"
		DetailPrint "Done setting default JVM."
		DetailPrint "$1"

		#
		# create a logical link if the default was not selected
		#

		DetailPrint ""		
		DetailPrint "Creating logical link..."
		${If} $InstDir != "C:\_YES_POLITES"
			DetailPrint "Creating logical link as C:\_YES_POLITES."
			nsExec::ExecToStack 'cmd.exe /C mklink /D C:\_YES_POLITES $InstDir'
			Pop $0
			Pop $1
			DetailPrint "RegPrependString:Error=$0 (Should be 0)"
			DetailPrint "$1"
		${Else}
		    DetailPrint "Installed to default location (logical link not needed)."		
		${EndIf}

	goto end_of_script
	file_found:
		DetailPrint ""
		DetailPrint "---------------------------------------"
		DetailPrint "INSTALL DIRECTORY EXISTS: "
		DetailPrint "$InstDir"
		DetailPrint "PLEASE DELETE EXISTING TARGET DIRECTORY BEFORE INSTALLING"
		DetailPrint "INSTALLATION WAS NOT EXECUTED"
		DetailPrint "---------------------------------------"

	end_of_script:

    #
    # done
    #
    
    DetailPrint ""
    DetailPrint "Done."
    DetailPrint ""

SectionEnd


