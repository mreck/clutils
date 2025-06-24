#ifndef CLUTIS_ERR_H_
#define CLUTIS_ERR_H_

#define ERR_CAPACITY_TOO_SMALL -1
#define ERR_OUT_OF_BOUNDS      -2
#define ERR_NOT_FOUND          -3

char * err_get_msg(int err);
void   die(int err);
void   die_if(int err);

#endif // CLUTIS_ERR_H_
