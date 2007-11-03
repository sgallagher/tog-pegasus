ROMFS=$(shell pwd)/romfs

all: proj
	( cd proj; make TOOL=diab ROMFS_DIR=$(ROMFS) RAM_LOW_ADRS=07200000 RAM_HIGH_ADRS=07600000 )

proj:
	mkdir -p proj
	( cd proj; vxprj create iq8134x diab vxsim.wpj )
	cp usrAppInit.c proj
	( cd proj; vxprj component add INCLUDE_POSIX_PTHREADS )
	( cd proj; vxprj component add INCLUDE_POSIX_SEM )
	( cd proj; vxprj component add INCLUDE_SHELL )
	( cd proj; vxprj component add INCLUDE_ROMFS )
	( cd proj; vxprj component add INCLUDE_SHELL_INTERP_CMD )
	( cd proj; vxprj component add INCLUDE_DISK_UTIL_SHELL_CMD )
	( cd proj; vxprj component add INCLUDE_CPLUS )
	( cd proj; vxprj component add INCLUDE_NFS )
	( cd proj; vxprj component add INCLUDE_NFS_CLIENT_ALL )
	( cd proj; vxprj component add INCLUDE_STARTUP_SCRIPT )
	( cd proj; vxprj component add INCLUDE_CPLUS_IOSTREAMS )
	( cd proj; vxprj component add INCLUDE_GETSERVBYNAME )
	( cd proj; vxprj component add INCLUDE_GETADDRINFO )
	( cd proj; vxprj component add INCLUDE_GETNAMEINFO )
	( cd proj; vxprj component add INCLUDE_GETNAMEINFO_SYSCTL )
	( cd proj; vxprj component add INCLUDE_GETNETBYNAME )
	( cd proj; vxprj component add INCLUDE_POSIX_CLOCKS )
	( cd proj; vxprj component add INCLUDE_POSIX_TIMERS )
	( cd proj; vxprj component add INCLUDE_PROTECT_TASK_STACK )
	( cd proj; vxprj parameter set JOB_TASK_STACK_SIZE 0x80000 )
	( cd proj; vxprj parameter set ROOT_STACK_SIZE 64000 )
	( cd proj; vxprj parameter set ISR_STACK_SIZE 50000 )
	( cd proj; vxprj parameter set TASK_USER_EXC_STACK_SIZE 32000 )
	( cd proj; vxprj parameter set NET_TASK_STACKSIZE 64000 )

pre:
	cp usrAppInit.c proj

clean:
	rm -rf proj

share:
	cp proj/vxWorks /share/
