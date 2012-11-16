#include "ftdidevice.h"

FtdiDevice::FtdiDevice()
{
    ftStatus = FT_Open(0, &ftHandle);
    if (ftStatus == FT_OK)
        open = true;
    else
        open = false;
}

FtdiDevice::FtdiDevice(int device)
{
    ftStatus = FT_Open(device, &ftHandle);
    if (ftStatus == FT_OK)
        open = true;
    else
        open = false;
}

FtdiDevice::FtdiDevice(int device, enum baudrate_e baud)
{
    ftStatus = FT_Open(device, &ftHandle);
    if (ftStatus == FT_OK)
        open = true;
    else
        open = false;
    if (setBaudrate(baud) != 0)
        open = false;
}

int FtdiDevice::close(void)
{
    ftStatus = FT_Close(ftHandle);
    open = false;
    if (ftStatus != FT_OK)
        return -1;
    else
        return 0;
}

int FtdiDevice::setBaudrate(enum baudrate_e baud)
{
    ftStatus = FT_SetDataCharacteristics(
                ftHandle,
                FT_BITS_8,
                FT_STOP_BITS_1,
                FT_PARITY_NONE
    );
    if (ftStatus != FT_OK)
        return -1;
    ftStatus = FT_SetBaudRate(ftHandle, baud);
    if (ftStatus != FT_OK)
        return -1;
    return 0;
}

bool FtdiDevice::isOpen(void)
{
    if (open)
        return true;
    else
        return false;
}

int FtdiDevice::setRts(void)
{
    ftStatus = FT_SetRts(ftHandle);
    if (ftStatus == FT_OK)
        return 0;
    else
        return -1;
}

int FtdiDevice::clrRts(void)
{
    ftStatus = FT_ClrRts(ftHandle);
    if (ftStatus == FT_OK)
        return 0;
    else
        return -1;
}

int FtdiDevice::writeByte(char b)
{
    DWORD BytesWritten;
    unsigned char buf[2] = {0,0};
    buf[0] = b;
    ftStatus = FT_Write(ftHandle, buf, 1, &BytesWritten);
    if ((ftStatus == FT_OK) && (BytesWritten == 1))
        return 0;
    else
        return -1;
}

int FtdiDevice::readByte(int timeout)
{
    DWORD BytesReceived;
    unsigned char buf[2];
    FT_SetTimeouts(ftHandle, timeout, 0);
    ftStatus = FT_Read(ftHandle, buf, 1, &BytesReceived);
    if ((ftStatus == FT_OK) && (BytesReceived >= 1))
        return buf[0];
    else
        return -1;
}

int FtdiDevice::flush(void)
{
    DWORD RxBytes;
    DWORD BytesReceived;
    char *tmp_buffer;
    FT_GetQueueStatus(ftHandle, &RxBytes);
    if (RxBytes > 0)
    {
       tmp_buffer = new char[RxBytes];
       FT_SetTimeouts(ftHandle, 0, 0);
       ftStatus = FT_Read(ftHandle, tmp_buffer, RxBytes, &BytesReceived);
       delete [] tmp_buffer;
       if ((ftStatus != FT_OK) || (BytesReceived != RxBytes))
          return -1;
    }
    return 0;
}

FtdiDevice::~FtdiDevice()
{
    FT_Close(ftHandle);
}

int FtdiDevice::read(unsigned char *buf, uint32_t size, unsigned int time)
{
    DWORD BytesReturned;
    FT_SetTimeouts(ftHandle, time, 0);
    uint32_t op;
    for (op = 0; (op+50) < size; op+=50)
    {
        ftStatus = FT_Read(ftHandle, &buf[op], 50, &BytesReturned);
        if (ftStatus != FT_OK)
            return -1;
        if (BytesReturned != 50)
            return -1;
    }
    FT_Read(ftHandle, &buf[op], size-op, &BytesReturned);
    return 0;
}

int FtdiDevice::setTimeout(unsigned int time)
{
    ftStatus = FT_SetTimeouts(ftHandle, time, 0);
    if (ftStatus != FT_OK)
        return -1;
    return 0;
}
