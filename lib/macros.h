#ifndef CLUTILS_MACROS_H_
#define CLUTILS_MACROS_H_

#ifndef CLU_TEMP_DIR
#define CLU_TEMP_DIR "/tmp"
#endif

#define CLU_ERR_TOO_MANY_ARGS    -1
#define CLU_ERR_BUFFER_TOO_SMALL -2
#define CLU_ERR_FILE_OP_FAILED   -3

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(*(a)))

#define DEFER() goto DEFER
#define DEFER_WITH(r) do { result = (r); DEFER(); } while(0)

#endif // CLUTILS_MACROS_H_
