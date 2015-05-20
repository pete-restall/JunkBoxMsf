#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/JunkBoxMsf.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/JunkBoxMsf.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/2108379793/Config.o ${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o ${OBJECTDIR}/_ext/1300160130/MsfDecoder.o ${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o ${OBJECTDIR}/_ext/1300160130/Receiver.o ${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o ${OBJECTDIR}/_ext/1739485253/Lcd.o ${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o ${OBJECTDIR}/_ext/2057908330/Screen.o ${OBJECTDIR}/_ext/1472/Main.o ${OBJECTDIR}/_ext/1055310863/LowPowerMode.o ${OBJECTDIR}/_ext/1055310863/Clock.o ${OBJECTDIR}/_ext/1055310863/Debug.o ${OBJECTDIR}/_ext/1055310863/Button.o ${OBJECTDIR}/_ext/1055310863/Menu.o ${OBJECTDIR}/_ext/1055310863/NumberFormatting.o ${OBJECTDIR}/_ext/1472/Mcu.o ${OBJECTDIR}/_ext/1472/McuConfigBits.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/2108379793/Config.o.d ${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o.d ${OBJECTDIR}/_ext/1300160130/MsfDecoder.o.d ${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o.d ${OBJECTDIR}/_ext/1300160130/Receiver.o.d ${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o.d ${OBJECTDIR}/_ext/1739485253/Lcd.o.d ${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o.d ${OBJECTDIR}/_ext/2057908330/Screen.o.d ${OBJECTDIR}/_ext/1472/Main.o.d ${OBJECTDIR}/_ext/1055310863/LowPowerMode.o.d ${OBJECTDIR}/_ext/1055310863/Clock.o.d ${OBJECTDIR}/_ext/1055310863/Debug.o.d ${OBJECTDIR}/_ext/1055310863/Button.o.d ${OBJECTDIR}/_ext/1055310863/Menu.o.d ${OBJECTDIR}/_ext/1055310863/NumberFormatting.o.d ${OBJECTDIR}/_ext/1472/Mcu.o.d ${OBJECTDIR}/_ext/1472/McuConfigBits.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/2108379793/Config.o ${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o ${OBJECTDIR}/_ext/1300160130/MsfDecoder.o ${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o ${OBJECTDIR}/_ext/1300160130/Receiver.o ${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o ${OBJECTDIR}/_ext/1739485253/Lcd.o ${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o ${OBJECTDIR}/_ext/2057908330/Screen.o ${OBJECTDIR}/_ext/1472/Main.o ${OBJECTDIR}/_ext/1055310863/LowPowerMode.o ${OBJECTDIR}/_ext/1055310863/Clock.o ${OBJECTDIR}/_ext/1055310863/Debug.o ${OBJECTDIR}/_ext/1055310863/Button.o ${OBJECTDIR}/_ext/1055310863/Menu.o ${OBJECTDIR}/_ext/1055310863/NumberFormatting.o ${OBJECTDIR}/_ext/1472/Mcu.o ${OBJECTDIR}/_ext/1472/McuConfigBits.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/JunkBoxMsf.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=30F2010
MP_LINKER_FILE_OPTION=,--script=p30F2010.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/2108379793/Config.o: ../Config/Config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2108379793 
	@${RM} ${OBJECTDIR}/_ext/2108379793/Config.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Config/Config.c  -o ${OBJECTDIR}/_ext/2108379793/Config.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2108379793/Config.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/2108379793/Config.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1300160130/MsfDecoder.o: ../Receiver/MsfDecoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1300160130 
	@${RM} ${OBJECTDIR}/_ext/1300160130/MsfDecoder.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/MsfDecoder.c  -o ${OBJECTDIR}/_ext/1300160130/MsfDecoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1300160130/MsfDecoder.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1300160130/MsfDecoder.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o: ../Receiver/GoertzelDetector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1300160130 
	@${RM} ${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/GoertzelDetector.c  -o ${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1300160130/Receiver.o: ../Receiver/Receiver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1300160130 
	@${RM} ${OBJECTDIR}/_ext/1300160130/Receiver.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/Receiver.c  -o ${OBJECTDIR}/_ext/1300160130/Receiver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1300160130/Receiver.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1300160130/Receiver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1739485253/Lcd.o: ../Screen/Lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1739485253 
	@${RM} ${OBJECTDIR}/_ext/1739485253/Lcd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Screen/Lcd.c  -o ${OBJECTDIR}/_ext/1739485253/Lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1739485253/Lcd.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1739485253/Lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2057908330/Screen.o: ../Receiver/../Screen/Screen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2057908330 
	@${RM} ${OBJECTDIR}/_ext/2057908330/Screen.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Screen/Screen.c  -o ${OBJECTDIR}/_ext/2057908330/Screen.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2057908330/Screen.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/2057908330/Screen.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Main.o: ../Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Main.c  -o ${OBJECTDIR}/_ext/1472/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/Main.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/LowPowerMode.o: ../Receiver/../LowPowerMode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/LowPowerMode.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../LowPowerMode.c  -o ${OBJECTDIR}/_ext/1055310863/LowPowerMode.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/LowPowerMode.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/LowPowerMode.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/Clock.o: ../Receiver/../Clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/Clock.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Clock.c  -o ${OBJECTDIR}/_ext/1055310863/Clock.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/Clock.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/Clock.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/Debug.o: ../Receiver/../Debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/Debug.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Debug.c  -o ${OBJECTDIR}/_ext/1055310863/Debug.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/Debug.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/Debug.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/Button.o: ../Receiver/../Button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/Button.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Button.c  -o ${OBJECTDIR}/_ext/1055310863/Button.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/Button.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/Button.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/Menu.o: ../Receiver/../Menu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/Menu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Menu.c  -o ${OBJECTDIR}/_ext/1055310863/Menu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/Menu.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/Menu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/NumberFormatting.o: ../Receiver/../NumberFormatting.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/NumberFormatting.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../NumberFormatting.c  -o ${OBJECTDIR}/_ext/1055310863/NumberFormatting.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/NumberFormatting.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/NumberFormatting.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Mcu.o: ../Mcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Mcu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Mcu.c  -o ${OBJECTDIR}/_ext/1472/Mcu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/Mcu.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Mcu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/McuConfigBits.o: ../McuConfigBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/McuConfigBits.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../McuConfigBits.c  -o ${OBJECTDIR}/_ext/1472/McuConfigBits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/McuConfigBits.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/McuConfigBits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/2108379793/Config.o: ../Config/Config.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2108379793 
	@${RM} ${OBJECTDIR}/_ext/2108379793/Config.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Config/Config.c  -o ${OBJECTDIR}/_ext/2108379793/Config.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2108379793/Config.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/2108379793/Config.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1300160130/MsfDecoder.o: ../Receiver/MsfDecoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1300160130 
	@${RM} ${OBJECTDIR}/_ext/1300160130/MsfDecoder.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/MsfDecoder.c  -o ${OBJECTDIR}/_ext/1300160130/MsfDecoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1300160130/MsfDecoder.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1300160130/MsfDecoder.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o: ../Receiver/GoertzelDetector.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1300160130 
	@${RM} ${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/GoertzelDetector.c  -o ${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1300160130/GoertzelDetector.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1300160130/Receiver.o: ../Receiver/Receiver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1300160130 
	@${RM} ${OBJECTDIR}/_ext/1300160130/Receiver.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/Receiver.c  -o ${OBJECTDIR}/_ext/1300160130/Receiver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1300160130/Receiver.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1300160130/Receiver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1739485253/Lcd.o: ../Screen/Lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1739485253 
	@${RM} ${OBJECTDIR}/_ext/1739485253/Lcd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Screen/Lcd.c  -o ${OBJECTDIR}/_ext/1739485253/Lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1739485253/Lcd.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1739485253/Lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2057908330/Screen.o: ../Receiver/../Screen/Screen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2057908330 
	@${RM} ${OBJECTDIR}/_ext/2057908330/Screen.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Screen/Screen.c  -o ${OBJECTDIR}/_ext/2057908330/Screen.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2057908330/Screen.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/2057908330/Screen.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Main.o: ../Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Main.c  -o ${OBJECTDIR}/_ext/1472/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/Main.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/LowPowerMode.o: ../Receiver/../LowPowerMode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/LowPowerMode.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../LowPowerMode.c  -o ${OBJECTDIR}/_ext/1055310863/LowPowerMode.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/LowPowerMode.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/LowPowerMode.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/Clock.o: ../Receiver/../Clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/Clock.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Clock.c  -o ${OBJECTDIR}/_ext/1055310863/Clock.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/Clock.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/Clock.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/Debug.o: ../Receiver/../Debug.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/Debug.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Debug.c  -o ${OBJECTDIR}/_ext/1055310863/Debug.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/Debug.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/Debug.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/Button.o: ../Receiver/../Button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/Button.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Button.c  -o ${OBJECTDIR}/_ext/1055310863/Button.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/Button.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/Button.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/Menu.o: ../Receiver/../Menu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/Menu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../Menu.c  -o ${OBJECTDIR}/_ext/1055310863/Menu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/Menu.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/Menu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1055310863/NumberFormatting.o: ../Receiver/../NumberFormatting.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1055310863 
	@${RM} ${OBJECTDIR}/_ext/1055310863/NumberFormatting.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Receiver/../NumberFormatting.c  -o ${OBJECTDIR}/_ext/1055310863/NumberFormatting.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1055310863/NumberFormatting.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1055310863/NumberFormatting.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/Mcu.o: ../Mcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Mcu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../Mcu.c  -o ${OBJECTDIR}/_ext/1472/Mcu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/Mcu.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Mcu.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1472/McuConfigBits.o: ../McuConfigBits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/McuConfigBits.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../McuConfigBits.c  -o ${OBJECTDIR}/_ext/1472/McuConfigBits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1472/McuConfigBits.o.d"        -g -omf=elf -Os -msmart-io=1 -Werror -Wall -msfr-warn=on -save-temps
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/McuConfigBits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o: ../Config/EepromReadWrite.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2108379793 
	@${RM} ${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../Config/EepromReadWrite.s  -o ${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Wa,-MD,"${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,--fatal-warnings$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o: ../Receiver/GoertzelDetectorIsr.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1300160130 
	@${RM} ${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../Receiver/GoertzelDetectorIsr.s  -o ${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Wa,-MD,"${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,--fatal-warnings$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o: ../Screen/LcdReadWrite.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1739485253 
	@${RM} ${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../Screen/LcdReadWrite.s  -o ${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Wa,-MD,"${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,--fatal-warnings$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o: ../Config/EepromReadWrite.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/2108379793 
	@${RM} ${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../Config/EepromReadWrite.s  -o ${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -Wa,-MD,"${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,--fatal-warnings$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/2108379793/EepromReadWrite.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o: ../Receiver/GoertzelDetectorIsr.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1300160130 
	@${RM} ${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../Receiver/GoertzelDetectorIsr.s  -o ${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -Wa,-MD,"${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,--fatal-warnings$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1300160130/GoertzelDetectorIsr.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o: ../Screen/LcdReadWrite.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1739485253 
	@${RM} ${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../Screen/LcdReadWrite.s  -o ${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -Wa,-MD,"${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,--fatal-warnings$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1739485253/LcdReadWrite.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/JunkBoxMsf.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/JunkBoxMsf.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--no-data-init,--pack-data,--no-handles,--no-isr,--gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="/home/pete/Projects/JunkBoxMsf/Firmware/JunkBoxMsf.X/JunkBoxMsf.map",--report-mem,--cref$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/JunkBoxMsf.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/JunkBoxMsf.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--no-data-init,--pack-data,--no-handles,--no-isr,--gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="/home/pete/Projects/JunkBoxMsf/Firmware/JunkBoxMsf.X/JunkBoxMsf.map",--report-mem,--cref$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}/xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/JunkBoxMsf.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
