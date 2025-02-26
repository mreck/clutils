#ifndef CLUTILS_MACROS_H_
#define CLUTILS_MACROS_H_

#ifndef CLU_TEMP_DIR
#define CLU_TEMP_DIR "/tmp"
#endif

#define CLU_ERR_TOO_MANY_ARGS    -1
#define CLU_ERR_BUFFER_TOO_SMALL -2

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(*(a)))

#endif // CLUTILS_MACROS_H_
