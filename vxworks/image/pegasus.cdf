Parameter PEGASUS_VFS_NAME {
    NAME Name of pegasus virtual file system (single file).
    TYPE string
    DEFAULT "/tmp/pegasus.vfs"
}

Parameter PEGASUS_DEV_NAME {
    NAME Name of pegasus device
    TYPE string
    DEFAULT "/pegasus"
}

Component INCLUDE_PEGASUS {
    NAME Pegasus
    SYNOPSIS The Pegasus Component
    INIT_RTN pegasusInit();
    CONFIGLETTES pegasusConfig.c    
    HDR_FILES pegasusConfig.h
    _INIT_ORDER usrIosExtraInit
    REQUIRES INCLUDE_POSIX_PTHREAD_SCHEDULER INCLUDE_HRFS INCLUDE_HRFS_FORMAT INCLUDE_DOSFS
    INCLUDE_WHEN INCLUDE_IO_SYSTEM
    CFG_PARAMS = PEGASUS_VFS_NAME PEGASUS_DEV_NAME
}
