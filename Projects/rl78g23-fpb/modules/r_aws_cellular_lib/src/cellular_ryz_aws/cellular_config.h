#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#else
#include "r_aws_cellular_config.h"
#endif

#ifndef CELLULAR_CONFIG_H_
#define CELLULAR_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif

#define CELLULAR_MCC_MAX_SIZE                               (3)
#define CELLULAR_MNC_MAX_SIZE                               (3)
#define CELLULAR_ICCID_MAX_SIZE                             (20)
#define CELLULAR_IMSI_MAX_SIZE                              (15)
#define CELLULAR_FW_VERSION_MAX_SIZE                        (32)
#define CELLULAR_HW_VERSION_MAX_SIZE                        (12)
#define CELLULAR_SERIAL_NUM_MAX_SIZE                        (12)
#define CELLULAR_IMEI_MAX_SIZE                              (15)
#define CELLULAR_NETWORK_NAME_MAX_SIZE                      (32)
#define CELLULAR_APN_MAX_SIZE                               (64)
#define CELLULAR_PDN_USERNAME_MAX_SIZE                      (32)
#define CELLULAR_PDN_PASSWORD_MAX_SIZE                      (32)
#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#define CELLULAR_IP_ADDRESS_MAX_SIZE                        (128)
#define CELLULAR_AT_CMD_MAX_SIZE                            (500)
#else
#define CELLULAR_IP_ADDRESS_MAX_SIZE                        (60)
#define CELLULAR_AT_CMD_MAX_SIZE                            (2000)
#endif
#define CELLULAR_NUM_SOCKET_MAX                             (6)
#define CELLULAR_MANUFACTURE_ID_MAX_SIZE                    (20)
#define CELLULAR_MODEL_ID_MAX_SIZE                          (10)
#define CELLULAR_EDRX_LIST_MAX_SIZE                         (4)
#define CELLULAR_PDN_CONTEXT_ID_MIN                         (1)
#define CELLULAR_PDN_CONTEXT_ID_MAX                         (6)
#define CELLULAR_MAX_RAT_PRIORITY_COUNT                     (3)
#define CELLULAR_MAX_SEND_DATA_LEN                          (1500)
#define CELLULAR_MAX_RECV_DATA_LEN                          (1500)
#define CELLULAR_SUPPORT_GETHOSTBYNAME                      (1)
#define CELLULAR_COMM_IF_SEND_TIMEOUT_MS                    (1000)
#define CELLULAR_COMM_IF_RECV_TIMEOUT_MS                    (50)
#define CELLULAR_CONFIG_STATIC_ALLOCATION_CONTEXT           (0)
#define CELLULAR_CONFIG_STATIC_COMM_CONTEXT_ALLOCATION      (0)
#define CELLULAR_CONFIG_DEFAULT_RAT                         (8)
#define CELLULAR_CONFIG_STATIC_SOCKET_CONTEXT_ALLOCATION    (0)
#define CELLULAR_MODEM_NO_GSM_NETWORK
#define CELLULAR_PHONENUM_MAX_SIZE                          (15)

#if defined(__CCRL__) || defined(__ICCRL78__) || defined(__RL)
#define CELLULAR_COMMON_AT_COMMAND_TIMEOUT_MS               (60000)
#define CELLULAR_AT_COMMAND_RAW_TIMEOUT_MS                  (60000)
#else
#define CELLULAR_COMMON_AT_COMMAND_TIMEOUT_MS               AWS_CELLULAR_CFG_AT_COMMAND_TIMEOUT
#define CELLULAR_AT_COMMAND_RAW_TIMEOUT_MS                  AWS_CELLULAR_CFG_AT_COMMAND_TIMEOUT
#endif

#endif /* CELLULAR_CONFIG_H_ */
