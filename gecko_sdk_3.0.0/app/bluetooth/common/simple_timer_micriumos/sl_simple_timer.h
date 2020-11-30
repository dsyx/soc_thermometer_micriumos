/***************************************************************************//**
 * @file
 * @brief Simple timer service based on Micrium RTOS
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef SL_SIMPLE_TIMER_H
#define SL_SIMPLE_TIMER_H

#include "os.h"

/// Timer structure
typedef OS_TMR sl_simple_timer_t;

/***************************************************************************//**
 * Expected prototype of the user's callback function which is called when a
 * timer expires.
 *
 * @param timer Pointer to the timer handle.
 * @param data An extra parameter for the user application.
 ******************************************************************************/
typedef void (*sl_simple_timer_callback_t)(sl_simple_timer_t *timer, void *data);

/***************************************************************************//**
 * Start timer or restart if it is running already.
 *
 * @param[in] timer Pointer to the timer.
 * @param[in] timeout_ms Timer timeout, in milliseconds.
 * @param[in] callback Callback function that is called when timeout expires.
 * @param[in] callback_data Pointer to user data that will be passed to callback.
 * @param[in] is_periodic Reload timer when it expires if true.
 *
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t sl_simple_timer_start(sl_simple_timer_t *timer,
                                  uint32_t timeout_ms,
                                  sl_simple_timer_callback_t callback,
                                  void *callback_data,
                                  bool is_periodic);

/***************************************************************************//**
 * Stop running timer.
 *
 * @param[in] timer Pointer to the timer.
 *
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t sl_simple_timer_stop(sl_simple_timer_t *timer);

#endif // SL_SIMPLE_TIMER_H
