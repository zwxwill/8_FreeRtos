#ifndef __SNTP_H__
#define __SNTP_H__

#ifdef __cplusplus
extern "C" {
#endif

//Custom configuration
#include "srv_timer.h"
#define SNTP_SERVER_ADDRESS "82.197.164.46" /* swisstime.ethz.ch */
#define SNTP_UPDATE_DELAY   (60000*60) /* unit ms SNTP update every 1 hour */
#define SNTP_SET_SYSTEM_TIME(sec) Srv_SNTPAdjustTime(sec)
//#define SNTP_GET_SYSTEM_TIME(sec, us) \
//    do{uint64_t time = rtcGetTimeUnixUsec(&RTCD1);\
//       (sec) = time / 1000000;\
//       (us) = time % 1000000;}while(0)

void sntp_init(void);
void sntp_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __SNTP_H__ */
