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

#include <stdbool.h>
#include "sl_simple_timer.h"

// -----------------------------------------------------------------------------
// Private macros

#define RTOS_ERROR_SPACE              0x8000
#define RTOS_ERROR_TO_STATUS(e)       (sl_status_t)((e).Code | RTOS_ERROR_SPACE)
#define RTOS_ERROR_CHECK(e)           \
  do {                                \
    if ((e).Code != RTOS_ERR_NONE) {  \
      return RTOS_ERROR_TO_STATUS(e); \
    }                                 \
  } while (0)

// -----------------------------------------------------------------------------
// Private function declarations

/***************************************************************************//**
 * Check if OS timer has already been created.
 *
 * @return true if timer has already been created, false otherwise.
 ******************************************************************************/
static bool timer_is_created(OS_TMR *timer);

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_simple_timer_start(sl_simple_timer_t *timer,
                                  uint32_t timeout_ms,
                                  sl_simple_timer_callback_t callback,
                                  void *callback_data,
                                  bool is_periodic)
{
  sl_status_t sc;
  RTOS_ERR err;
  OS_RATE_HZ tick_rate;
  OS_TICK delay, d_error, period;
  OS_OPT opt;

  // Check input parameters.
  if ((timeout_ms == 0) && is_periodic) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Calculate timer period.
  tick_rate = OSTimeTickRateHzGet(&err);
  RTOS_ERROR_CHECK(err);
  delay = (timeout_ms * tick_rate) / (OSTmrUpdateCnt * 1000);
  d_error = (timeout_ms * tick_rate) % (OSTmrUpdateCnt * 1000);
  if (d_error != 0) {
    // The timer resolution is too small for the requested timeout.
    return SL_STATUS_INVALID_PARAMETER;
  }
  period = is_periodic ? delay : 0;
  opt = is_periodic ? OS_OPT_TMR_PERIODIC : OS_OPT_TMR_ONE_SHOT;

  // Make sure that timer is stopped, also check for NULL.
  sc = sl_simple_timer_stop(timer);
  if (SL_STATUS_OK != sc) {
    return sc;
  }

  // Create OS timer.
  OSTmrCreate(timer,
              "simple timer",
              delay,
              period,
              opt,
              (OS_TMR_CALLBACK_PTR)callback,
              callback_data,
              &err);
  RTOS_ERROR_CHECK(err);

  // Start OS timer.
  OSTmrStart(timer, &err);
  RTOS_ERROR_CHECK(err);

  return SL_STATUS_OK;
}

sl_status_t sl_simple_timer_stop(sl_simple_timer_t *timer)
{
  if (timer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (timer_is_created(timer)) {
    RTOS_ERR err;
    // Stop and delete OS timer.
    OSTmrDel(timer, &err);
    RTOS_ERROR_CHECK(err);
  }

  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
// Private function definitions

static bool timer_is_created(OS_TMR *timer)
{
  bool ret = false;

  #if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
  if (timer->Type != OS_OBJ_TYPE_TMR) {
    return false;
  }
  #endif

  switch (timer->State) {
    case OS_TMR_STATE_STOPPED:
    case OS_TMR_STATE_RUNNING:
    case OS_TMR_STATE_COMPLETED:
      ret = true;
      break;

    case OS_TMR_STATE_UNUSED:
    default:
      ret = false;
      break;
  }
  return ret;
}
