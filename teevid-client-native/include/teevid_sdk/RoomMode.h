#ifndef TEEVID_SDK_ROOMMODE_H
#define TEEVID_SDK_ROOMMODE_H

namespace teevid_sdk
{
typedef enum {
    kInteractive = 0,
    kLecture = 1,
    kWaitingRoom = 2,
    kWaitingForLecturer = 3
} RoomMode;
}
#endif //TEEVID_SDK_ROOMMODE_H
