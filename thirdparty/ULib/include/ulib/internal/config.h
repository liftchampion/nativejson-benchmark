/* include/ulib/internal/config.h.  Generated from config.h.in by configure.  */
/* include/ulib/internal/config.h.in.  Generated from configure.ac by autoheader.  */

/* C Flags */
#define CFLAGS_VAR "-g -O2  -Werror-implicit-function-declaration -Wstrict-prototypes -Wc++-compat -Wmissing-prototypes -Wnested-externs -Wdeclaration-after-statement -Wold-style-definition -Wsizeof-pointer-memaccess"

/* Define to 1 if the `closedir' function returns void instead of `int'. */
/* #undef CLOSEDIR_VOID */

/* Preprocessor Flags */
#define CPPFLAGS_VAR " -mcrc32 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -pipe   -Wstrict-aliasing=2 -Wall -Wextra -Wsign-compare -Wpointer-arith -Wwrite-strings -Wmissing-declarations -Wpacked -Wswitch-enum -Wmissing-format-attribute -Winit-self -Wformat -Wenum-compare -Wlogical-not-parentheses -Wsizeof-array-argument -Wbool-compare -Wno-unused-result -Wshadow -Wsuggest-attribute=pure -Wsuggest-attribute=noreturn -Wlogical-op -Wduplicated-cond -Wtautological-compare -Wswitch-bool -Wshift-negative-value -Wshift-overflow -Wshift-overflow=2 -Wnull-dereference -Wnonnull -fno-stack-protector -ffast-math -ftree-vectorize -fno-crossjumping -fno-gcse -fpartial-inlining -Ofast -flto -ffat-lto-objects -fuse-linker-plugin -march=native -mtune=native -Winline -Wno-unused-parameter -Wno-unused-variable   "

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define if we are cross compiling */
/* #undef CROSS_COMPILE */

/* C++ Flags */
#define CXXFLAGS_VAR " -I/usr/local/include -fvisibility=hidden -fvisibility-inlines-hidden -fno-exceptions -fno-rtti  -fno-check-new -fno-enforce-eh-specs -Wno-deprecated -Wdelete-non-virtual-dtor -Wodr -Wterminate -Wlto-type-mismatch -Wsubobject-linkage -Wplacement-new -Wvirtual-inheritance -Wnamespaces"

/* Compiler */
#define CXX_VAR "/opt/rh/devtoolset-9/root/usr/bin/g++"

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* debugging model */
/* #undef DEBUG */

/* Define if you have d_namlen in struct dirent */
/* #undef DIRENT_HAVE_D_NAMLEN */

/* Define if you have d_reclen in struct dirent */
#define DIRENT_HAVE_D_RECLEN 1

/* Define if you have d_type in struct dirent */
#define DIRENT_HAVE_D_TYPE 1

/* Define if disable ipv6 support */
#define DISABLE_IPV6 1

/* Define if disable ZIP support */
#define DISABLE_ZIP 1

/* Define if enable ipv6 support */
/* #undef ENABLE_IPV6 */

/* enable Large File Support features */
#define ENABLE_LFS 1

/* enable memory pool features */
#define ENABLE_MEMPOOL 1

/* Define if enable thread support */
/* #undef ENABLE_THREAD */

/* Define if enable ZIP support */
/* #undef ENABLE_ZIP */

/* Define to 1 if the fallocate() function is supported */
#define FALLOCATE_IS_SUPPORTED 1

/* GNU C/C++ Compiler version */
#define GCC_VERSION "9"

/* GNU C/C++ Compiler version extented */
#define GCC_VERSION_EXT "gcc (GCC) 5.3.0"

/* has accept4 */
#define HAVE_ACCEPT4 1

/* Define to 1 if you have the `alarm' function. */
#define HAVE_ALARM 1

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
#define HAVE_ALLOCA_H 1

/* Define if we are on 64 bit architecture */
#define HAVE_ARCH64 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <asm/mman.h> header file. */
#define HAVE_ASM_MMAN_H 1

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* has atexit */
#define HAVE_ATEXIT 1

/* C++ atomic header */
#define HAVE_ATOMIC_H 1

/* there are the built-in functions to detect run-time CPU type and ISA */
#define HAVE_BUILTIN_CPU_INIT 1

/* Define to 1 if you have the <byteswap.h> header file. */
#define HAVE_BYTESWAP_H 1

/* Define to 1 if your system has a working `chown' function. */
#define HAVE_CHOWN 1

/* has clock_gettime */
#define HAVE_CLOCK_GETTIME 1

/* enable cpp preprocessor features */
#define HAVE_CPP 1

/* Define to 1 if you have the <cpuid.h> header file. */
#define HAVE_CPUID_H 1

/* Define if exist type cpu_set_t */
#define HAVE_CPU_SET_T 1

/* define if the compiler supports basic C++11 syntax */
#define HAVE_CXX11 1

/* define if the compiler supports basic C++14 syntax */
#define HAVE_CXX14 1

/* has daemon */
#define HAVE_DAEMON 1

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* has dup3 */
#define HAVE_DUP3 1

/* Define to 1 if you have the <endian.h> header file. */
#define HAVE_ENDIAN_H 1

/* has epoll_create1 */
#define HAVE_EPOLL_CREATE1 1

/* has epoll_ctl_batch */
/* #undef HAVE_EPOLL_CTL_BATCH */

/* has epoll_wait */
#define HAVE_EPOLL_WAIT 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* has fallocate */
#define HAVE_FALLOCATE 1

/* has fallocate64 */
#define HAVE_FALLOCATE64 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* has fnmatch */
#define HAVE_FNMATCH 1

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* cannot test in autoconf safely */
#define HAVE_GCC_ATOMICS 1

/* has getaddrinfo */
#define HAVE_GETADDRINFO 1

/* ipv6 host lookup */
#define HAVE_GETHOSTBYNAME2 1

/* has getnameinfo */
#define HAVE_GETNAMEINFO 1

/* has getopt_long */
#define HAVE_GETOPT_LONG 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* has getpriority */
#define HAVE_GETPRIORITY 1

/* has gmtime_r */
#define HAVE_GMTIME_R 1

/* enable gcc visibility features */
#define HAVE_GNUC_VISIBILTY 1

/* Define to 1 if you have the <ifaddrs.h> header file. */
#define HAVE_IFADDRS_H 1

/* has inet_ntop */
#define HAVE_INET_NTOP 1

/* ipv6 support */
#define HAVE_INET_PTON 1

/* Define to 1 if you have the `inotify_init1' function. */
#define HAVE_INOTIFY_INIT1 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* struct iphdr needed for IPv6 support */
#define HAVE_IPHDR 1

/* has kqueue */
/* #undef HAVE_KQUEUE */

/* has kqueue1 */
/* #undef HAVE_KQUEUE1 */

/* Define to 1 if you have the `bson-1.0' library (-lbson-1.0). */
/* #undef HAVE_LIBBSON_1_0 */

/* Define to 1 if you have the `GeoIP' library (-lGeoIP). */
/* #undef HAVE_LIBGEOIP */

/* Define to 1 if you have the <libintl.h> header file. */
#define HAVE_LIBINTL_H 1

/* Define to 1 if you have the `mongoc-1.0' library (-lmongoc-1.0). */
/* #undef HAVE_LIBMONGOC_1_0 */

/* Define to 1 if you have the
   <libnetfilter_conntrack/libnetfilter_conntrack.h> header file. */
/* #undef HAVE_LIBNETFILTER_CONNTRACK_LIBNETFILTER_CONNTRACK_H */

/* Define to 1 if you have the `numa' library (-lnuma). */
/* #undef HAVE_LIBNUMA */

/* Define to 1 if you have the `tcc' library (-ltcc). */
/* #undef HAVE_LIBTCC */

/* Define to 1 if you have the `tdb' library (-ltdb). */
/* #undef HAVE_LIBTDB */

/* Define to 1 if you have the `z' library (-lz). */
/* #undef HAVE_LIBZ */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <linux/in6.h> header file. */
#define HAVE_LINUX_IN6_H 1

/* Define to 1 if you have the <linux/netfilter_ipv4/ipt_ACCOUNT.h> header
   file. */
/* #undef HAVE_LINUX_NETFILTER_IPV4_IPT_ACCOUNT_H */

/* has lrintl */
#define HAVE_LRINTL 1

/* have lstat */
#define HAVE_LSTAT 1

/* Define if you have Mac OSX sendfile() semantics */
/* #undef HAVE_MACOSX_SENDFILE */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <mcheck.h> header file. */
#define HAVE_MCHECK_H 1

/* enable mcpp preprocessor features */
/* #undef HAVE_MCPP */

/* Define if you have si_addr in siginfo_t */
#define HAVE_MEMBER_SI_ADDR 1

/* has memmem */
#define HAVE_MEMMEM 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* has memrchr */
#define HAVE_MEMRCHR 1

/* has mkdtemp */
#define HAVE_MKDTEMP 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* has mremap */
#define HAVE_MREMAP 1

/* Define if compiler recognizes nanosleep function */
#define HAVE_NANOSLEEP 1

/* Define to 1 if you have the <ndir.h> header file. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet6/in6.h> header file. */
/* #undef HAVE_NETINET6_IN6_H */

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <netpacket/packet.h> header file. */
#define HAVE_NETPACKET_PACKET_H 1

/* Define to 1 if you have the <net/if_arp.h> header file. */
#define HAVE_NET_IF_ARP_H 1

/* Define to 1 if the target don't allows unaligned reads and write */
/* #undef HAVE_NO_UNALIGNED_ACCESSES */

/* define if the compiler implements old style iostreams */
/* #undef HAVE_OLD_IOSTREAM */

/* has pipe2 */
#define HAVE_PIPE2 1

/* Define to 1 if you have the <poll.h> header file. */
#define HAVE_POLL_H 1

/* posix memory alignment */
#define HAVE_POSIX_MEMALIGN 1

/* has posix_spawn */
#define HAVE_POSIX_SPAWN 1

/* has pread */
#define HAVE_PREAD 1

/* pthread inherit scheduling attribute */
#define HAVE_PTHREAD_ATTR_SETINHRITSCHED 1

/* pthread cancel */
#define HAVE_PTHREAD_CANCEL 1

/* pthread clocking */
#define HAVE_PTHREAD_CONDATTR_SETCLOCK 1

/* pthread delay */
/* #undef HAVE_PTHREAD_DELAY */

/* pthread delay np */
/* #undef HAVE_PTHREAD_DELAY_NP */

/* pthread concurrency */
#define HAVE_PTHREAD_SETCONCURRENCY 1

/* pthread scheduling */
#define HAVE_PTHREAD_SETSCHEDPRIO 1

/* pthread suspend */
/* #undef HAVE_PTHREAD_SUSPEND */

/* pthread yield */
#define HAVE_PTHREAD_YIELD 1

/* Define to 1 if the system has the type `ptrdiff_t'. */
#define HAVE_PTRDIFF_T 1

/* If available, contains the Python version number currently in use. */
/* #undef HAVE_PYTHON */

/* Define to 1 if you have the <rump/rump.h> header file. */
/* #undef HAVE_RUMP_RUMP_H */

/* has sched_getaffinity */
#define HAVE_SCHED_GETAFFINITY 1

/* has sched_getcpu */
#define HAVE_SCHED_GETCPU 1

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define to 1 if you have the <semaphore.h> header file. */
#define HAVE_SEMAPHORE_H 1

/* has sem_getvalue */
#define HAVE_SEM_GETVALUE 1

/* has sem_init */
#define HAVE_SEM_INIT 1

/* has sem_timedwait */
#define HAVE_SEM_TIMEDWAIT 1

/* has sendfile */
#define HAVE_SENDFILE 1

/* has sendfile64 */
#define HAVE_SENDFILE64 1

/* alternate dyn loader */
/* #undef HAVE_SHL_LOAD */

/* has shm_open */
#define HAVE_SHM_OPEN 1

/* Define if exist type siginfo_t */
#define HAVE_SIGINFO_T 1

/* have socketpair in libsocket */
#define HAVE_SOCKETPAIR 1

/* Define if we have time stamp support in openssl */
/* #undef HAVE_SSL_TS */

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* string collation */
#define HAVE_STRCOLL 1

/* Define to 1 if you have the <streambuf.h> header file. */
/* #undef HAVE_STREAMBUF_H */

/* has strerror */
#define HAVE_STRERROR 1

/* string icmp */
/* #undef HAVE_STRICMP */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* string istr */
/* #undef HAVE_STRISTR */

/* has strndup */
#define HAVE_STRNDUP 1

/* has strptime */
#define HAVE_STRPTIME 1

/* has strsignal */
#define HAVE_STRSIGNAL 1

/* Define to 1 if you have the <strstream.h> header file. */
/* #undef HAVE_STRSTREAM_H */

/* has strtof */
#define HAVE_STRTOF 1

/* has strtold */
#define HAVE_STRTOLD 1

/* has strtoull */
#define HAVE_STRTOULL 1

/* Define to 1 if `st_blksize' is a member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_BLKSIZE 1

/* Define to 1 if `st_blocks' is a member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_BLOCKS 1

/* Define to 1 if `st_rdev' is a member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_RDEV 1

/* Define to 1 if your `struct stat' has `st_blocks'. Deprecated, use
   `HAVE_STRUCT_STAT_ST_BLOCKS' instead. */
#define HAVE_ST_BLOCKS 1

/* system config */
#define HAVE_SYSCONF 1

/* Define to 1 if you have the <sysexits.h> header file. */
#define HAVE_SYSEXITS_H 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/dir.h> header file. */
#define HAVE_SYS_DIR_H 1

/* Define to 1 if you have the <sys/endian.h> header file. */
/* #undef HAVE_SYS_ENDIAN_H */

/* Define to 1 if you have the <sys/inotify.h> header file. */
#define HAVE_SYS_INOTIFY_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/ndir.h> header file. */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/sched.h> header file. */
/* #undef HAVE_SYS_SCHED_H */

/* Define to 1 if you have the <sys/sendfile.h> header file. */
#define HAVE_SYS_SENDFILE_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/syscall.h> header file. */
#define HAVE_SYS_SYSCALL_H 1

/* Define to 1 if you have the <sys/timeb.h> header file. */
#define HAVE_SYS_TIMEB_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* has timegm */
#define HAVE_TIMEGM 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define to 1 if `fork' works. */
#define HAVE_WORKING_FORK 1

/* Define if socket option SO_RCVTIMEO is implemented */
#define HAVE_WORKING_SOCKET_OPTION_SO_RCVTIMEO 1

/* Define to 1 if `vfork' works. */
#define HAVE_WORKING_VFORK 1

/* Define to 1 if the system has the type `_Bool'. */
/* #undef HAVE__BOOL */

/* Define to 1 if you have the
   </usr/src/linux/include/generated/uapi/linux/version.h> header file. */
/* #undef HAVE__USR_SRC_LINUX_INCLUDE_GENERATED_UAPI_LINUX_VERSION_H */

/* Linker Flags */
#define LDFLAGS_VAR " -L/usr/local/lib -Wl,-O1 -Wl,--as-needed -Wl,-z,now,-O1,--hash-style=gnu,--sort-common -Wl,--as-needed"

/* Linker */
#define LD_VAR "/opt/rh/devtoolset-9/root/usr/libexec/gcc/x86_64-redhat-linux/9/ld -m elf_x86_64"

/* LD Linker version */
#define LD_VERSION "GNU ld version 2.32-16.el7"

/* Standard C lib version */
#define LIBC_VERSION "glibc 2.17"

/* Libraries */
#define LIBS_VAR " /opt/rh/devtoolset-9/root/usr/lib/gcc/x86_64-redhat-linux/9/libstdc++.a /opt/rh/devtoolset-9/root/usr/lib/gcc/x86_64-redhat-linux/9/libgcc.a  -lpthread -lrt -ldl -lc"

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
#define LSTAT_FOLLOWS_SLASHED_SYMLINK 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to 1 if `major', `minor', and `makedev' are declared in <mkdev.h>.
   */
/* #undef MAJOR_IN_MKDEV */

/* Define to 1 if `major', `minor', and `makedev' are declared in
   <sysmacros.h>. */
/* #undef MAJOR_IN_SYSMACROS */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "stefano.casazza@gmail.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "ULib"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ULib 1.4.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ULib"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://stefanocasazza.github.io/ULib/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.4.2"

/* Platform */
#define PLATFORM_VAR "centos 7.8.2003 (AMD64)"

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* The number of bytes in type int */
#define SIZEOF_INT 4

/* The number of bytes in type long */
#define SIZEOF_LONG 8

/* The number of bytes in type long long */
#define SIZEOF_LONG_LONG 8

/* The size of `off_t', as computed by sizeof. */
#define SIZEOF_OFF_T 8

/* The number of bytes in type short */
#define SIZEOF_SHORT 2

/* The number of bytes in type size_t */
#define SIZEOF_SIZE_T 8

/* The number of bytes in type time_t */
#define SIZEOF_TIME_T 8

/* The number of bytes in type unsigned char */
#define SIZEOF_UNSIGNED_CHAR 1

/* The number of bytes in type void * */
#define SIZEOF_VOID_P 8

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
/* #undef STAT_MACROS_BROKEN */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Stardard GNU C++ Library version */
#define STDGPP_VERSION "unknown"

/* sys/inotify.h exists and works correctly */
#define SYS_INOTIFY_H_EXISTS_AND_WORKS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define if you have tm_gmtoff in struct tm */
#define TM_HAVE_TM_GMTOFF 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* ULib version */
#define ULIB_VERSION "1.4.2"

/* Define if enable c-ares support */
/* #undef USE_C_ARES */

/* Define if we have support for crc32 intrinsics */
#define USE_HARDWARE_CRC32 1

/* Define if enable libcurl support */
/* #undef USE_LIBCURL */

/* Define if enable libdbi support */
/* #undef USE_LIBDBI */

/* Define if enable libevent support */
/* #undef USE_LIBEVENT */

/* Define if enable libexpat support */
/* #undef USE_LIBEXPAT */

/* Define if enable libldap support */
/* #undef USE_LIBLDAP */

/* Define if enable libmagic support */
/* #undef USE_LIBMAGIC */

/* Define if enable libpcre support */
/* #undef USE_LIBPCRE */

/* Define if enable libssh support */
/* #undef USE_LIBSSH */

/* Define if enable libssl support */
/* #undef USE_LIBSSL */

/* Define if enable libtdb support */
/* #undef USE_LIBTDB */

/* Define if enable libuuid support */
/* #undef USE_LIBUUID */

/* Define if enable libv8 support */
/* #undef USE_LIBV8 */

/* Define if enable libxml2 support */
/* #undef USE_LIBXML2 */

/* Define if enable libz support */
/* #undef USE_LIBZ */

/* Define if enable libzopfli support */
/* #undef USE_LIBZOPFLI */

/* enable load balance support between physical server via udp brodcast */
/* #undef USE_LOAD_BALANCE */

/* Define if enable mongodb support */
/* #undef USE_MONGODB */

/* Define if enable MySQL database support */
/* #undef USE_MYSQL */

/* Define if enable libpagespeed support */
/* #undef USE_PAGE_SPEED */

/* Define if use parser generator bison/yacc */
/* #undef USE_PARSER */

/* Define if enable PostgreSQL database support */
/* #undef USE_PGSQL */

/* Define if enable php support */
/* #undef USE_PHP */

/* Define if enable python support */
/* #undef USE_PYTHON */

/* Define if enable ruby support */
/* #undef USE_RUBY */

/* Define if enable SQLite database support */
/* #undef USE_SQLITE */

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* enable ACL filtering support */
/* #undef U_ACL_SUPPORT */

/* enable alias URI support */
#define U_ALIAS 1

/* use apex_memmove written by Trevor Herselman in 2014 */
/* #undef U_APEX_ENABLE */

/* disable cache request support */
#define U_CACHE_REQUEST_DISABLE 1

/* enable server classic model support */
/* #undef U_CLASSIC_SUPPORT */

/* enable client response partial write support */
/* #undef U_CLIENT_RESPONSE_PARTIAL_WRITE_SUPPORT */

/* enable GDB stack dump support */
/* #undef U_GDB_STACK_DUMP_ENABLE */

/* enable HTML pagination support */
/* #undef U_HTML_PAGINATION_SUPPORT */

/* disable HTTP/2 support */
#define U_HTTP2_DISABLE 1

/* enable HTTP inotify support */
/* #undef U_HTTP_INOTIFY_SUPPORT */

/* enable HTTP Strict Transport Security support */
/* #undef U_HTTP_STRICT_TRANSPORT_SECURITY */

/* install directory for plugins */
#define U_LIBEXECDIR "/usr/local/libexec"

/* disable client and server log support */
/* #undef U_LOG_DISABLE */

/* disable homogeneous pipeline request support */
#define U_PIPELINE_HOMOGENEOUS_DISABLE 1

/* install directory */
#define U_PREFIXDIR "/usr/local"

/* enable RFC1918 filtering support */
/* #undef U_RFC1918_SUPPORT */

/* enable server captive portal mode */
/* #undef U_SERVER_CAPTIVE_PORTAL */

/* enable server check time between request for parallelization */
/* #undef U_SERVER_CHECK_TIME_BETWEEN_REQUEST */

/* enable server thread approach support */
/* #undef U_SERVER_THREAD_APPROACH_SUPPORT */

/* STATIC_HANDLER_ECHO */
/* #undef U_STATIC_HANDLER_ECHO */

/* STATIC_HANDLER_FCGI */
/* #undef U_STATIC_HANDLER_FCGI */

/* STATIC_HANDLER_GEOIP */
/* #undef U_STATIC_HANDLER_GEOIP */

/* STATIC_HANDLER_HTTP */
/* #undef U_STATIC_HANDLER_HTTP */

/* STATIC_HANDLER_NOCAT */
/* #undef U_STATIC_HANDLER_NOCAT */

/* STATIC_HANDLER_PROXY */
/* #undef U_STATIC_HANDLER_PROXY */

/* STATIC_HANDLER_RPC */
/* #undef U_STATIC_HANDLER_RPC */

/* STATIC_HANDLER_SCGI */
/* #undef U_STATIC_HANDLER_SCGI */

/* STATIC_HANDLER_SHIB */
/* #undef U_STATIC_HANDLER_SHIB */

/* STATIC_HANDLER_SOAP */
/* #undef U_STATIC_HANDLER_SOAP */

/* STATIC_HANDLER_SOCKET */
/* #undef U_STATIC_HANDLER_SOCKET */

/* STATIC_HANDLER_SSI */
/* #undef U_STATIC_HANDLER_SSI */

/* STATIC_HANDLER_STREAM */
/* #undef U_STATIC_HANDLER_STREAM */

/* STATIC_HANDLER_TSA */
/* #undef U_STATIC_HANDLER_TSA */

/* we want only static linking */
#define U_STATIC_ONLY 1

/* STATIC_ORM_DRIVER_MYSQL */
/* #undef U_STATIC_ORM_DRIVER_MYSQL */

/* STATIC_ORM_DRIVER_PGSQL */
/* #undef U_STATIC_ORM_DRIVER_PGSQL */

/* STATIC_ORM_DRIVER_SQLITE */
/* #undef U_STATIC_ORM_DRIVER_SQLITE */

/* STATIC_SERVLET */
/* #undef U_STATIC_SERVLET */

/* STATIC_SERVLET_WI_AUTH */
/* #undef U_STATIC_SERVLET_WI_AUTH */

/* full c++ support and linking model */
#define U_STDCPP_ENABLE 1

/* install directory for configuration data */
#define U_SYSCONFDIR "/usr/local/etc"

/* Define if we have libtdb with this function */
/* #undef U_TDB_TRAVERSE_READ */

/* enable server bandwidth throttling support */
/* #undef U_THROTTLING_SUPPORT */

/* enable welcome message support */
/* #undef U_WELCOME_SUPPORT */

/* Define if we have zlib with this function */
/* #undef U_ZLIB_DEFLATE_WORKSPACESIZE */

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
/* #undef YYTEXT_POINTER */

/* Bison version */
#define _BISON_VERSION "None"

/* libcURL - multiprotocol file transfer library version */
#define _CURL_VERSION "unknown"

/* version of c-ares library for asynchronous DNS requests */
#define _C_ARES_VERSION ""

/* libdbi - Database Independent Abstraction Layer version */
#define _DBI_VERSION "unknown"

/* Expat version */
#define _EXPAT_VERSION "unknown"

/* Flex version */
#define _FLEX_VERSION "None"

/* Ldap version */
#define _LDAP_VERSION "unknown"

/* libevent - event notification library version */
#define _LIBEVENT_VERSION "unknown"

/* libSSH version */
#define _LIBSSH_VERSION "unknown"

/* libtdb - samba Trivial DB library version */
#define _LIBTDB_VERSION "unknown"

/* libuuid - version library to create a new UUID value */
#define _LIBUUID_VERSION "unknown"

/* libxml2 - version 2 of the library to manipulate XML files */
#define _LIBXML2_VERSION "unknown"

/* libzopfli - google compression library version */
#define _LIBZOPFLI_VERSION "unknown"

/* libz - general purpose compression library version */
#define _LIBZ_VERSION "unknow"

/* libmagic - magic number recognition library version */
#define _MAGIC_VERSION "unknown"

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* libmysql - MySQL Database System library version */
#define _MYSQL_VERSION "unknown"

/* Operating System version */
#define _OS_VERSION "Linux gleb 5.9.1-1.el7.elrepo.x86_64 #1 SMP Fri Oct 16 10:55:30 EDT 2020 x86_64 x86_64 x86_64 GNU/Linux"

/* PageSpeed - version of Google page speed SDK */
#define _PAGE_SPEED_VERSION "unknown"

/* PCRE - Perl compatible regular expressions version */
#define _PCRE_VERSION "unknown"

/* version of PostgreSQL database support */
#define _PGSQL_VERSION "unknown"

/* version of PHP language runtime engine embed SAPIs */
#define _PHP_VERSION "unknown"

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* version of PYTHON language runtime engine embed */
#define _PYTHON_VERSION "unknown"

/* version of RUBY language runtime engine embed */
#define _RUBY_VERSION "unknown"

/* libsqlite - SQLITE Database System library version */
#define _SQLITE_VERSION "unknown"

/* SSL version */
#define _SSL_VERSION "unknown"

/* Define for Solaris 2.5.1 so the uint32_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT32_T */

/* Define for Solaris 2.5.1 so the uint64_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT64_T */

/* Define for Solaris 2.5.1 so the uint8_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT8_T */

/* V8 - version of Google V8 JavaScript Engine */
#define _V8_VERSION "unknown"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to the type of a signed integer type of width exactly 16 bits if
   such a type exists and the standard includes do not define it. */
/* #undef int16_t */

/* Define to the type of a signed integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
/* #undef int32_t */

/* Define to the type of a signed integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
/* #undef int64_t */

/* Define to the type of a signed integer type of width exactly 8 bits if such
   a type exists and the standard includes do not define it. */
/* #undef int8_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef ptrdiff_t */

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#define restrict __restrict
/* Work around a bug in Sun C++: it does not support _Restrict or
   __restrict__, even though the corresponding Sun C compiler ends up with
   "#define restrict _Restrict" or "#define restrict __restrict__" in the
   previous line.  Perhaps some future version of Sun C++ will work with
   restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define to the type of an unsigned integer type of width exactly 16 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint16_t */

/* Define to the type of an unsigned integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint32_t */

/* Define to the type of an unsigned integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint64_t */

/* Define to the type of an unsigned integer type of width exactly 8 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint8_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */

/* Define to empty if the keyword `volatile' does not work. Warning: valid
   code using `volatile' can become incorrect without. Disable with care. */
/* #undef volatile */


#include <ulib/internal/platform.h>

#define CONFIGURE_DEFINES " DISABLE_IPV6 DISABLE_ZIP ENABLE_LFS ENABLE_MEMPOOL USE_HARDWARE_CRC32"

#define CONFIGURE_CALL "  $ ./configure --disable-shared CC=/opt/rh/devtoolset-9/root/usr/bin/gcc CXX=/opt/rh/devtoolset-9/root/usr/bin/g++ CPP=/opt/rh/devtoolset-9/root/usr/bin/cpp"

