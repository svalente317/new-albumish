#if !defined(__FTPLIB_H)
#define __FTPLIB_H

#include <limits.h>
#include <inttypes.h>

/* FtpAccess() type codes */
#define FTPLIB_LIST_DIRECTORY 1
#define FTPLIB_FILE_READ 2
#define FTPLIB_FILE_WRITE 3

/* FtpAccess() mode codes */
#define FTPLIB_ASCII 'A'
#define FTPLIB_IMAGE 'I'
#define FTPLIB_TEXT FTPLIB_ASCII
#define FTPLIB_BINARY FTPLIB_IMAGE

/* connection modes */
#define FTPLIB_PASSIVE 1
#define FTPLIB_PORT 2

/* connection option names */
#define FTPLIB_CONNMODE 1
#define FTPLIB_CALLBACK 2
#define FTPLIB_IDLETIME 3
#define FTPLIB_CALLBACKARG 4
#define FTPLIB_CALLBACKBYTES 5

typedef struct NetBuf netbuf;
typedef int (*FtpCallback)(netbuf *nControl, uint64_t xfered, void *arg);

typedef struct FtpCallbackOptions {
    FtpCallback cbFunc;        /* function to call */
    void *cbArg;               /* argument to pass to function */
    unsigned int bytesXferred; /* callback if this number of bytes transferred */
    unsigned int idleTime;     /* callback if this many milliseconds have elapsed */
} FtpCallbackOptions;

extern int ftplib_debug;
char *FtpLastResponse(netbuf *nControl);
int FtpConnect(const char *host, int port, netbuf **nControl);
int FtpOptions(int opt, long val, netbuf *nControl);
int FtpSetCallback(const FtpCallbackOptions *opt, netbuf *nControl);
int FtpClearCallback(netbuf *nControl);
int FtpLogin(const char *user, const char *pass, netbuf *nControl);
int FtpAccess(const char *path, int typ, int mode, netbuf *nControl, netbuf **);
int FtpRead(void *buf, int max, netbuf *nData);
int FtpWrite(const void *buf, int len, netbuf *nData);
int FtpClose(netbuf *nData);
int FtpSite(const char *cmd, netbuf *nControl);
int FtpSysType(char *buf, int max, netbuf *nControl);
int FtpMkdir(const char *path, netbuf *nControl);
int FtpChdir(const char *path, netbuf *nControl);
int FtpCDUp(netbuf *nControl);
int FtpRmdir(const char *path, netbuf *nControl);
int FtpPwd(char *path, int max, netbuf *nControl);
int FtpListDirectory(const char *output, const char *path, netbuf *nControl);
int FtpSize(const char *path, unsigned int *size, char mode, netbuf *nControl);
int FtpModDate(const char *path, char *dt, int max, netbuf *nControl);
int FtpGet(const char *output, const char *path, char mode, netbuf *nControl);
int FtpPut(const char *input, const char *path, char mode, netbuf *nControl);
int FtpRename(const char *src, const char *dst, netbuf *nControl);
int FtpDelete(const char *fnm, netbuf *nControl);
void FtpQuit(netbuf *nControl);

#endif /* __FTPLIB_H */
