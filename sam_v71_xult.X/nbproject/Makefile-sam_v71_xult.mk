#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-sam_v71_xult.mk)" "nbproject/Makefile-local-sam_v71_xult.mk"
include nbproject/Makefile-local-sam_v71_xult.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=sam_v71_xult
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/config/sam_v71_xult/bsp/bsp.c ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1.c ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1_device.c ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c ../src/config/sam_v71_xult/peripheral/efc/plib_efc.c ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c ../src/config/sam_v71_xult/stdio/xc32_monitor.c ../src/config/sam_v71_xult/system/cache/sys_cache.c ../src/config/sam_v71_xult/system/int/src/sys_int.c ../src/config/sam_v71_xult/usb/src/usb_device.c ../src/config/sam_v71_xult/usb/src/usb_device_cdc.c ../src/config/sam_v71_xult/usb/src/usb_device_cdc_acm.c ../src/config/sam_v71_xult/initialization.c ../src/config/sam_v71_xult/interrupts.c ../src/config/sam_v71_xult/exceptions.c ../src/config/sam_v71_xult/startup_xc32.c ../src/config/sam_v71_xult/libc_syscalls.c ../src/config/sam_v71_xult/usb_device_init_data.c ../src/config/sam_v71_xult/tasks.c ../src/main.c ../src/app.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1379310441/bsp.o ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o ${OBJECTDIR}/_ext/1157990217/plib_clk.o ${OBJECTDIR}/_ext/1157988489/plib_efc.o ${OBJECTDIR}/_ext/1537621011/plib_nvic.o ${OBJECTDIR}/_ext/1157977813/plib_pio.o ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o ${OBJECTDIR}/_ext/1871840330/sys_cache.o ${OBJECTDIR}/_ext/663512716/sys_int.o ${OBJECTDIR}/_ext/388987185/usb_device.o ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o ${OBJECTDIR}/_ext/1320227943/initialization.o ${OBJECTDIR}/_ext/1320227943/interrupts.o ${OBJECTDIR}/_ext/1320227943/exceptions.o ${OBJECTDIR}/_ext/1320227943/startup_xc32.o ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o ${OBJECTDIR}/_ext/1320227943/tasks.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/app.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1379310441/bsp.o.d ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o.d ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o.d ${OBJECTDIR}/_ext/1157990217/plib_clk.o.d ${OBJECTDIR}/_ext/1157988489/plib_efc.o.d ${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d ${OBJECTDIR}/_ext/1157977813/plib_pio.o.d ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d ${OBJECTDIR}/_ext/1871840330/sys_cache.o.d ${OBJECTDIR}/_ext/663512716/sys_int.o.d ${OBJECTDIR}/_ext/388987185/usb_device.o.d ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o.d ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o.d ${OBJECTDIR}/_ext/1320227943/initialization.o.d ${OBJECTDIR}/_ext/1320227943/interrupts.o.d ${OBJECTDIR}/_ext/1320227943/exceptions.o.d ${OBJECTDIR}/_ext/1320227943/startup_xc32.o.d ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o.d ${OBJECTDIR}/_ext/1320227943/tasks.o.d ${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1360937237/app.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1379310441/bsp.o ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o ${OBJECTDIR}/_ext/1157990217/plib_clk.o ${OBJECTDIR}/_ext/1157988489/plib_efc.o ${OBJECTDIR}/_ext/1537621011/plib_nvic.o ${OBJECTDIR}/_ext/1157977813/plib_pio.o ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o ${OBJECTDIR}/_ext/1871840330/sys_cache.o ${OBJECTDIR}/_ext/663512716/sys_int.o ${OBJECTDIR}/_ext/388987185/usb_device.o ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o ${OBJECTDIR}/_ext/1320227943/initialization.o ${OBJECTDIR}/_ext/1320227943/interrupts.o ${OBJECTDIR}/_ext/1320227943/exceptions.o ${OBJECTDIR}/_ext/1320227943/startup_xc32.o ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o ${OBJECTDIR}/_ext/1320227943/tasks.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/app.o

# Source Files
SOURCEFILES=../src/config/sam_v71_xult/bsp/bsp.c ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1.c ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1_device.c ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c ../src/config/sam_v71_xult/peripheral/efc/plib_efc.c ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c ../src/config/sam_v71_xult/stdio/xc32_monitor.c ../src/config/sam_v71_xult/system/cache/sys_cache.c ../src/config/sam_v71_xult/system/int/src/sys_int.c ../src/config/sam_v71_xult/usb/src/usb_device.c ../src/config/sam_v71_xult/usb/src/usb_device_cdc.c ../src/config/sam_v71_xult/usb/src/usb_device_cdc_acm.c ../src/config/sam_v71_xult/initialization.c ../src/config/sam_v71_xult/interrupts.c ../src/config/sam_v71_xult/exceptions.c ../src/config/sam_v71_xult/startup_xc32.c ../src/config/sam_v71_xult/libc_syscalls.c ../src/config/sam_v71_xult/usb_device_init_data.c ../src/config/sam_v71_xult/tasks.c ../src/main.c ../src/app.c



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
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-sam_v71_xult.mk ${DISTDIR}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=ATSAMV71Q21B
MP_LINKER_FILE_OPTION=,--script="..\src\config\sam_v71_xult\ATSAMV71Q21B.ld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1379310441/bsp.o: ../src/config/sam_v71_xult/bsp/bsp.c  .generated_files/flags/sam_v71_xult/89c2a70abf37290d04c55791cfcbcf62edd0e349 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1379310441" 
	@${RM} ${OBJECTDIR}/_ext/1379310441/bsp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1379310441/bsp.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1379310441/bsp.o.d" -o ${OBJECTDIR}/_ext/1379310441/bsp.o ../src/config/sam_v71_xult/bsp/bsp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o: ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1.c  .generated_files/flags/sam_v71_xult/5082b29cf7804dcc0673bc296ab400ac881b3f2c .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1153718613" 
	@${RM} ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o.d" -o ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o: ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1_device.c  .generated_files/flags/sam_v71_xult/32e74d98b988e7cd27f98451322ab63d013205a5 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1153718613" 
	@${RM} ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o.d" -o ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1_device.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1157990217/plib_clk.o: ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c  .generated_files/flags/sam_v71_xult/cd89c34ff679ee7015b645941f8aa094407e33d0 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1157990217" 
	@${RM} ${OBJECTDIR}/_ext/1157990217/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157990217/plib_clk.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1157990217/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1157990217/plib_clk.o ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1157988489/plib_efc.o: ../src/config/sam_v71_xult/peripheral/efc/plib_efc.c  .generated_files/flags/sam_v71_xult/50a8770d9d05195d749751aaca27cd2139d60ed .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1157988489" 
	@${RM} ${OBJECTDIR}/_ext/1157988489/plib_efc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157988489/plib_efc.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1157988489/plib_efc.o.d" -o ${OBJECTDIR}/_ext/1157988489/plib_efc.o ../src/config/sam_v71_xult/peripheral/efc/plib_efc.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1537621011/plib_nvic.o: ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c  .generated_files/flags/sam_v71_xult/2a1e6424e6fbf6421a06a2e1cd5f8052226bf1ab .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1537621011" 
	@${RM} ${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1537621011/plib_nvic.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1537621011/plib_nvic.o ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1157977813/plib_pio.o: ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c  .generated_files/flags/sam_v71_xult/34544397f74dc98333a883a5ac043296cd561b6d .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1157977813" 
	@${RM} ${OBJECTDIR}/_ext/1157977813/plib_pio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157977813/plib_pio.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1157977813/plib_pio.o.d" -o ${OBJECTDIR}/_ext/1157977813/plib_pio.o ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1643282145/xc32_monitor.o: ../src/config/sam_v71_xult/stdio/xc32_monitor.c  .generated_files/flags/sam_v71_xult/488726991896b1563ce3a01cde5e88b879f7cbf8 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1643282145" 
	@${RM} ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o ../src/config/sam_v71_xult/stdio/xc32_monitor.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1871840330/sys_cache.o: ../src/config/sam_v71_xult/system/cache/sys_cache.c  .generated_files/flags/sam_v71_xult/c5b923cb8d4403e63b6e34e8dbd6c06a9a974763 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1871840330" 
	@${RM} ${OBJECTDIR}/_ext/1871840330/sys_cache.o.d 
	@${RM} ${OBJECTDIR}/_ext/1871840330/sys_cache.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1871840330/sys_cache.o.d" -o ${OBJECTDIR}/_ext/1871840330/sys_cache.o ../src/config/sam_v71_xult/system/cache/sys_cache.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/663512716/sys_int.o: ../src/config/sam_v71_xult/system/int/src/sys_int.c  .generated_files/flags/sam_v71_xult/e546b3ec1532cb852a89191701716ef817a65ac .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/663512716" 
	@${RM} ${OBJECTDIR}/_ext/663512716/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/663512716/sys_int.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/663512716/sys_int.o.d" -o ${OBJECTDIR}/_ext/663512716/sys_int.o ../src/config/sam_v71_xult/system/int/src/sys_int.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/388987185/usb_device.o: ../src/config/sam_v71_xult/usb/src/usb_device.c  .generated_files/flags/sam_v71_xult/9d5532502b83872f698d60d015ec5e6443c72124 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/388987185" 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/388987185/usb_device.o.d" -o ${OBJECTDIR}/_ext/388987185/usb_device.o ../src/config/sam_v71_xult/usb/src/usb_device.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/388987185/usb_device_cdc.o: ../src/config/sam_v71_xult/usb/src/usb_device_cdc.c  .generated_files/flags/sam_v71_xult/2f52d0af2487ad669b48854d471b24e493188491 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/388987185" 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o.d 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/388987185/usb_device_cdc.o.d" -o ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o ../src/config/sam_v71_xult/usb/src/usb_device_cdc.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o: ../src/config/sam_v71_xult/usb/src/usb_device_cdc_acm.c  .generated_files/flags/sam_v71_xult/dbc700ebd869c53c47febcb077a29a886c24dfba .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/388987185" 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o.d 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o.d" -o ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o ../src/config/sam_v71_xult/usb/src/usb_device_cdc_acm.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/initialization.o: ../src/config/sam_v71_xult/initialization.c  .generated_files/flags/sam_v71_xult/9636210fe23711b79d24477fec4b8d992814c8ba .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/initialization.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/initialization.o.d" -o ${OBJECTDIR}/_ext/1320227943/initialization.o ../src/config/sam_v71_xult/initialization.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/interrupts.o: ../src/config/sam_v71_xult/interrupts.c  .generated_files/flags/sam_v71_xult/4ef6aa0f9fb8e88b36a9eb0b867e753002e9608d .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/interrupts.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/interrupts.o.d" -o ${OBJECTDIR}/_ext/1320227943/interrupts.o ../src/config/sam_v71_xult/interrupts.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/exceptions.o: ../src/config/sam_v71_xult/exceptions.c  .generated_files/flags/sam_v71_xult/7d63f400a0979aa6227a5d5c643260234fadd02d .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/exceptions.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/exceptions.o.d" -o ${OBJECTDIR}/_ext/1320227943/exceptions.o ../src/config/sam_v71_xult/exceptions.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/startup_xc32.o: ../src/config/sam_v71_xult/startup_xc32.c  .generated_files/flags/sam_v71_xult/ade34075e87ccb5a98b4e7178740bc60c06fdcd2 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/startup_xc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/startup_xc32.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/startup_xc32.o.d" -o ${OBJECTDIR}/_ext/1320227943/startup_xc32.o ../src/config/sam_v71_xult/startup_xc32.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/libc_syscalls.o: ../src/config/sam_v71_xult/libc_syscalls.c  .generated_files/flags/sam_v71_xult/df0cadca09fa4bb565037e5b6676ed6db1c1fbd4 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o ../src/config/sam_v71_xult/libc_syscalls.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o: ../src/config/sam_v71_xult/usb_device_init_data.c  .generated_files/flags/sam_v71_xult/8af9b66cf54545c6b52eadee1e7c5e3a40cdbcd0 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o.d" -o ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o ../src/config/sam_v71_xult/usb_device_init_data.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/tasks.o: ../src/config/sam_v71_xult/tasks.c  .generated_files/flags/sam_v71_xult/af9e61ad097f1e4627d6676c9e40e19e09078be9 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/tasks.o.d" -o ${OBJECTDIR}/_ext/1320227943/tasks.o ../src/config/sam_v71_xult/tasks.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/sam_v71_xult/bfdc0c618986c0d671c07a4c17eb801b13502435 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  .generated_files/flags/sam_v71_xult/ae3ab336d10aea0f24daaf532c51825b6da319c4 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
else
${OBJECTDIR}/_ext/1379310441/bsp.o: ../src/config/sam_v71_xult/bsp/bsp.c  .generated_files/flags/sam_v71_xult/177f4e0f2b2534f351c0e177386fdcecd32827a0 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1379310441" 
	@${RM} ${OBJECTDIR}/_ext/1379310441/bsp.o.d 
	@${RM} ${OBJECTDIR}/_ext/1379310441/bsp.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1379310441/bsp.o.d" -o ${OBJECTDIR}/_ext/1379310441/bsp.o ../src/config/sam_v71_xult/bsp/bsp.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o: ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1.c  .generated_files/flags/sam_v71_xult/1f70a885c3cd0ef4de017e7938e5cb0d49c9c010 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1153718613" 
	@${RM} ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o.d" -o ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1.o ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o: ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1_device.c  .generated_files/flags/sam_v71_xult/61ac46853c75dc325f84b8b9d17a26da56482b61 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1153718613" 
	@${RM} ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o.d" -o ${OBJECTDIR}/_ext/1153718613/drv_usbhsv1_device.o ../src/config/sam_v71_xult/driver/usb/usbhsv1/src/drv_usbhsv1_device.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1157990217/plib_clk.o: ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c  .generated_files/flags/sam_v71_xult/1fcc7ed40c5960d3d34042712c7d0a7a0da118f .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1157990217" 
	@${RM} ${OBJECTDIR}/_ext/1157990217/plib_clk.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157990217/plib_clk.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1157990217/plib_clk.o.d" -o ${OBJECTDIR}/_ext/1157990217/plib_clk.o ../src/config/sam_v71_xult/peripheral/clk/plib_clk.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1157988489/plib_efc.o: ../src/config/sam_v71_xult/peripheral/efc/plib_efc.c  .generated_files/flags/sam_v71_xult/72b8a2d453f65a2f44551d5f9a6e68a6e361d539 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1157988489" 
	@${RM} ${OBJECTDIR}/_ext/1157988489/plib_efc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157988489/plib_efc.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1157988489/plib_efc.o.d" -o ${OBJECTDIR}/_ext/1157988489/plib_efc.o ../src/config/sam_v71_xult/peripheral/efc/plib_efc.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1537621011/plib_nvic.o: ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c  .generated_files/flags/sam_v71_xult/e74e36d1e3aa692c629561e77d3ec05eb812e183 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1537621011" 
	@${RM} ${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d 
	@${RM} ${OBJECTDIR}/_ext/1537621011/plib_nvic.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1537621011/plib_nvic.o.d" -o ${OBJECTDIR}/_ext/1537621011/plib_nvic.o ../src/config/sam_v71_xult/peripheral/nvic/plib_nvic.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1157977813/plib_pio.o: ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c  .generated_files/flags/sam_v71_xult/a938ce77dbbdf6e84f8835feecb145daff223272 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1157977813" 
	@${RM} ${OBJECTDIR}/_ext/1157977813/plib_pio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1157977813/plib_pio.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1157977813/plib_pio.o.d" -o ${OBJECTDIR}/_ext/1157977813/plib_pio.o ../src/config/sam_v71_xult/peripheral/pio/plib_pio.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1643282145/xc32_monitor.o: ../src/config/sam_v71_xult/stdio/xc32_monitor.c  .generated_files/flags/sam_v71_xult/be4ccde5d0f387af9cbdc15d01cf3ea4c6b65d8d .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1643282145" 
	@${RM} ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d 
	@${RM} ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1643282145/xc32_monitor.o.d" -o ${OBJECTDIR}/_ext/1643282145/xc32_monitor.o ../src/config/sam_v71_xult/stdio/xc32_monitor.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1871840330/sys_cache.o: ../src/config/sam_v71_xult/system/cache/sys_cache.c  .generated_files/flags/sam_v71_xult/8faaaea13542975739204a21dfaf1c7f29909c9 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1871840330" 
	@${RM} ${OBJECTDIR}/_ext/1871840330/sys_cache.o.d 
	@${RM} ${OBJECTDIR}/_ext/1871840330/sys_cache.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1871840330/sys_cache.o.d" -o ${OBJECTDIR}/_ext/1871840330/sys_cache.o ../src/config/sam_v71_xult/system/cache/sys_cache.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/663512716/sys_int.o: ../src/config/sam_v71_xult/system/int/src/sys_int.c  .generated_files/flags/sam_v71_xult/aa2227c6850c44e313f59af0e1300c69b472d247 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/663512716" 
	@${RM} ${OBJECTDIR}/_ext/663512716/sys_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/663512716/sys_int.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/663512716/sys_int.o.d" -o ${OBJECTDIR}/_ext/663512716/sys_int.o ../src/config/sam_v71_xult/system/int/src/sys_int.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/388987185/usb_device.o: ../src/config/sam_v71_xult/usb/src/usb_device.c  .generated_files/flags/sam_v71_xult/3e2d4c8a43fe0139f2d115f32c6c8cbca94deef9 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/388987185" 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device.o.d 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/388987185/usb_device.o.d" -o ${OBJECTDIR}/_ext/388987185/usb_device.o ../src/config/sam_v71_xult/usb/src/usb_device.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/388987185/usb_device_cdc.o: ../src/config/sam_v71_xult/usb/src/usb_device_cdc.c  .generated_files/flags/sam_v71_xult/4e457a01c510a4a15c2a41f0864f9fff3503868d .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/388987185" 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o.d 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/388987185/usb_device_cdc.o.d" -o ${OBJECTDIR}/_ext/388987185/usb_device_cdc.o ../src/config/sam_v71_xult/usb/src/usb_device_cdc.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o: ../src/config/sam_v71_xult/usb/src/usb_device_cdc_acm.c  .generated_files/flags/sam_v71_xult/974051900bd9a2cb420762e7dc3372f4c14836e7 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/388987185" 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o.d 
	@${RM} ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o.d" -o ${OBJECTDIR}/_ext/388987185/usb_device_cdc_acm.o ../src/config/sam_v71_xult/usb/src/usb_device_cdc_acm.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/initialization.o: ../src/config/sam_v71_xult/initialization.c  .generated_files/flags/sam_v71_xult/ecfd7af066629b119d0bde188d5bdc003b98d427 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/initialization.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/initialization.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/initialization.o.d" -o ${OBJECTDIR}/_ext/1320227943/initialization.o ../src/config/sam_v71_xult/initialization.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/interrupts.o: ../src/config/sam_v71_xult/interrupts.c  .generated_files/flags/sam_v71_xult/baa7eca210e8cbadd2ec5ff6a5977b0a8dfcf94c .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/interrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/interrupts.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/interrupts.o.d" -o ${OBJECTDIR}/_ext/1320227943/interrupts.o ../src/config/sam_v71_xult/interrupts.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/exceptions.o: ../src/config/sam_v71_xult/exceptions.c  .generated_files/flags/sam_v71_xult/2e72b0151b38a79be1e48b22e99ca0ef7c7fc223 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/exceptions.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/exceptions.o.d" -o ${OBJECTDIR}/_ext/1320227943/exceptions.o ../src/config/sam_v71_xult/exceptions.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/startup_xc32.o: ../src/config/sam_v71_xult/startup_xc32.c  .generated_files/flags/sam_v71_xult/5f1919560e211eaf5ad1bb0938edd32d762d70b4 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/startup_xc32.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/startup_xc32.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/startup_xc32.o.d" -o ${OBJECTDIR}/_ext/1320227943/startup_xc32.o ../src/config/sam_v71_xult/startup_xc32.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/libc_syscalls.o: ../src/config/sam_v71_xult/libc_syscalls.c  .generated_files/flags/sam_v71_xult/e3db1c1489f0652f9eb5ade65b7de0bd4ede280d .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/libc_syscalls.o.d" -o ${OBJECTDIR}/_ext/1320227943/libc_syscalls.o ../src/config/sam_v71_xult/libc_syscalls.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o: ../src/config/sam_v71_xult/usb_device_init_data.c  .generated_files/flags/sam_v71_xult/9c3f29a3ad000a56cd4f9cec7a53cd09ac5b132d .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o.d" -o ${OBJECTDIR}/_ext/1320227943/usb_device_init_data.o ../src/config/sam_v71_xult/usb_device_init_data.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1320227943/tasks.o: ../src/config/sam_v71_xult/tasks.c  .generated_files/flags/sam_v71_xult/5eeffb88a5917ef50912095918cff6890119fe6b .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1320227943" 
	@${RM} ${OBJECTDIR}/_ext/1320227943/tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/1320227943/tasks.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1320227943/tasks.o.d" -o ${OBJECTDIR}/_ext/1320227943/tasks.o ../src/config/sam_v71_xult/tasks.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  .generated_files/flags/sam_v71_xult/7c8cabe2be44886a13a7e0592b19987f9d971330 .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  .generated_files/flags/sam_v71_xult/f93158bc399e92eb3e543eecc1393361c05860dd .generated_files/flags/sam_v71_xult/da39a3ee5e6b4b0d3255bfef95601890afd80709
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -fdata-sections -O1 -fno-common -I"../src" -I"../src/config/sam_v71_xult" -I"../src/packs/ATSAMV71Q21B_DFP" -I"../src/packs/CMSIS/" -I"../src/packs/CMSIS/CMSIS/Core/Include" -MP -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wno-deprecated-declarations -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wmissing-noreturn -mdfp="${DFP_DIR}/samv71b" ${PACK_COMMON_OPTIONS} 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../src/config/sam_v71_xult/ATSAMV71Q21B.ld
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g   -mprocessor=$(MP_PROCESSOR_OPTION)  -mno-device-startup-code -o ${DISTDIR}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=_min_heap_size=512,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}/samv71b"
	
else
${DISTDIR}/sam_v71_xult.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../src/config/sam_v71_xult/ATSAMV71Q21B.ld
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -mno-device-startup-code -o ${DISTDIR}/sam_v71_xult.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_sam_v71_xult=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=512,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}/samv71b"
	${MP_CC_DIR}\\xc32-bin2hex ${DISTDIR}/sam_v71_xult.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}
