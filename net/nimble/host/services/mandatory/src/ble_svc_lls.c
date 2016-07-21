/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <string.h>
#include "host/ble_hs.h"
#include "services/mandatory/ble_svc_lls.h"

static uint8_t ble_svc_lls_alert_level;

static int
ble_svc_lls_access(uint16_t conn_handle, uint16_t attr_handle,
                   struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def ble_svc_lls_defs[] = {
    {
        /*** Service: Link Lost Service (LLS). */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid128 = BLE_UUID16(BLE_SVC_LLS_UUID16),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /*** Characteristic: Alert Level. */
            .uuid128 = BLE_UUID16(BLE_SVC_LLS_CHR_UUID16_ALERT_LEVEL),
            .access_cb = ble_svc_lls_access,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics in this service. */
        } },
    },

    {
        0, /* No more services. */
    },
};


static int
ble_svc_lls_access(uint16_t conn_handle, uint16_t attr_handle,
                   struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    assert(ctxt->chr == &ble_svc_lls_defs[0].characteristics[0]);
    switch(ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        ctxt->att->read.data = &ble_svc_lls_alert_level;
        ctxt->att->read.len = sizeof ble_svc_lls_alert_level;
        break;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        if (ctxt->att->write.len != sizeof ble_svc_lls_alert_level) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        memcpy(ble_svc_lls_alert_level, ctxt->att->write.data, 
            sizeof ble_svc_lls_alert_level);
        break;
    default:
        assert(0);
        break;
    }

    return 0;
}

uint8_t
ble_svc_lls_alert_level(void)
{
    return ble_svc_lls_alert_level;
}

int
ble_svc_gap_device_name_set(uint8_t alert_level)
{
    if (alert_level > BLE_SVC_LLS_ALERT_LEVEL_MAX) {
        return BLE_HS_EINVAL;
    }
    
    memcpy(ble_sve_lls_alert_level, alert_level, 
            sizeof alert_level);

    return 0;
}

int
ble_svc_lls_register(void)
{
    int rc;

    rc = ble_gatts_register_svcs(ble_svc_lls_defs, NULL, NULL);
    return rc;
}

int
ble_svc_lls_init(struct ble_hs_cfg *cfg)
{
    int rc;

    rc = ble_gatts_count_cfg(ble_svc_lls_defs, cfg);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
