/*
 * can_mcan.h
 */

#ifndef CSP_DRIVERS_CAN_MCAN_H
#define CSP_DRIVERS_CAN_MCAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <csp/csp.h>
#include <csp/interfaces/csp_if_can.h>

/**
 * @brief Open MCAN interface and add to CSP
 *
 * This is the MCU equivalent of csp_can_socketcan_open_and_add_interface().
 * It configures MCAN1 Message RAM, registers ISR callbacks,
 * creates an RX processing task, and adds the interface to CSP.
 *
 * @param[in]  ifname        CSP interface name, NULL for default "CAN"
 * @param[in]  promisc       true = receive all frames, false = filter by CSP address
 * @param[out] return_iface  pointer to the created CSP interface (can be NULL)
 * @return CSP_ERR_NONE on success, error code on failure
 *
 * @note Call this AFTER csp_init() and BEFORE csp_route_start_task()
 * @note MCAN1 must be initialized by MCC-generated MCAN1_Initialize() before calling this
 */
int csp_can_mcan_open_and_add_interface(const char *ifname, bool promisc, csp_iface_t **return_iface);

/**
 * @brief Stop RX task and clean up (for testing)
 *
 * @param[in] iface  interface to stop
 * @return CSP_ERR_NONE on success
 */
int csp_can_mcan_stop(csp_iface_t *iface);

#ifdef __cplusplus
}
#endif

#endif /* CSP_DRIVERS_CAN_MCAN_H */