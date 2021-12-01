#ifndef CAMERA_PROTOCOL_H_
#define CAMERA_PROTOCOL_H_

#include <stdint.h>
#include "esp_log.h"

#include "dbg_protocol.h"

#define CAM_MESSAGE_LENGTH 512

namespace lavazza
{
    namespace camera
    {
		void receivePhoto(dbgMsg_stc& msg);
    }
}

#endif // CAMERA_PROTOCOL_H_