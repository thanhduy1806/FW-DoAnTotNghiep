/* Copyright (c) 2013-2018 GomSpace A/S. All rights reserved. */

#include <rgosh_serialize.h>

#include <M1_SysApp/nanopb/pb.h>
#include <M1_SysApp/nanopb/pb_encode.h>
#include <M1_SysApp/nanopb/pb_decode.h>

#include "M1_SysApp/xlog/xlog.h"

xerr_t gs_rgosh_serialize_request(GoshRequest *req, uint8_t *buf, uint32_t *buf_size)
{
    if ((buf == NULL) || (buf_size == NULL)) {
        return XERR_INVAL;
    }

    size_t encoded_size = 0;
    if(!pb_get_encoded_size(&encoded_size, GoshRequest_fields, req)) {
        return XERR_INVAL;
    }

    if (encoded_size > *buf_size) {
        return XERR_OVERFLOW;
    }

    pb_ostream_t pb_ostream = pb_ostream_from_buffer(buf, encoded_size);
    if (!pb_encode(&pb_ostream, GoshRequest_fields, req)) {
        return XERR_INVAL;
    }

    *buf_size = encoded_size;
    return XERR_OK;
}


xerr_t gs_rgosh_serialize_response(GoshResponse *resp, uint8_t *buf, uint32_t *buf_size)
{
    if ((buf == NULL) || (buf_size == NULL)) {
        return XERR_INVAL;
    }
;
    size_t encoded_size = 0;
    if(!pb_get_encoded_size(&encoded_size, GoshResponse_fields, resp)) {
        return XERR_INVAL;
    }

    if (encoded_size > *buf_size) {
        return XERR_OVERFLOW;
    }

    pb_ostream_t pb_ostream = pb_ostream_from_buffer(buf, encoded_size);
    if (!pb_encode(&pb_ostream, GoshResponse_fields, resp)) {
        return XERR_INVAL;
    }

    *buf_size = encoded_size;
    return XERR_OK;
}

xerr_t gs_rgosh_deserialize_request(uint8_t *buf, uint32_t buf_size, GoshRequest *req)
{
    if (req == NULL) {
        return XERR_INVAL;
    }

    pb_istream_t pb_istream = pb_istream_from_buffer(buf, buf_size);
    if (!pb_decode(&pb_istream, GoshRequest_fields, req)) {
        return XERR_INVAL;
    }

    return XERR_OK;
}

xerr_t gs_rgosh_deserialize_response(uint8_t *buf, uint32_t buf_size, GoshResponse *resp)
{
    if (resp == NULL) {
        return XERR_INVAL;
    }

    pb_istream_t pb_istream = pb_istream_from_buffer(buf, buf_size);
    if (!pb_decode(&pb_istream, GoshResponse_fields, resp)) {
        return XERR_INVAL;
    }

    return XERR_OK;
}
