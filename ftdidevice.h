#ifndef FTDIDEVICE_H
#define FTDIDEVICE_H

#ifndef __linux__
    #include "windows.h"
    #define FTD2XX_EXPORTS
#endif
#include "ftd2xx.h"

#include <QStringList>

class FtdiDevice
{
public:

    enum baudrate_e {
        FTDI_BAUD_300       = 300,
        FTDI_BAUD_600       = 600,
        FTDI_BAUD_1200      = 1200,
        FTDI_BAUD_2400      = 2400,
        FTDI_BAUD_4800      = 4800,
        FTDI_BAUD_9600      = 9600,
        FTDI_BAUD_14400     = 14400,
        FTDI_BAUD_19200     = 19200,
        FTDI_BAUD_38400     = 38400,
        FTDI_BAUD_57600     = 57600,
        FTDI_BAUD_115200    = 115200,
        FTDI_BAUD_230400    = 230400,
        FTDI_BAUD_460800    = 460800,
        FTDI_BAUD_921600    = 921600
    };

    FtdiDevice(int device, enum baudrate_e baud);
    FtdiDevice(int device);
    FtdiDevice();
    ~FtdiDevice();
    enum baudrate_e getBaudrate(void);
    int close(void);
    int setBaudrate(enum baudrate_e);
    int writeByte(char b);
    int readByte(int timeout);
    int flush(void);
    bool isOpen(void);
    int setRts(void);
    int clrRts(void);
    int read(unsigned char *buf, uint32_t size, unsigned int time);
    int setTimeout(unsigned int time);

    static int getDevicesAmount(void)
    {
        FT_STATUS status;
        DWORD numDevs;
        status = FT_CreateDeviceInfoList(&numDevs);
        if (status == FT_OK)
            return numDevs;
        else
            return -1;
    }

    static QStringList getDescriptionsList(void)
    {
        FT_STATUS status;
        QStringList list;
        FT_DEVICE_LIST_INFO_NODE *devInfo;
        DWORD numDevs = FtdiDevice::getDevicesAmount();
        if (numDevs <= 0)
            return list;
        devInfo = new FT_DEVICE_LIST_INFO_NODE[numDevs];
        status = FT_GetDeviceInfoList(devInfo, &numDevs);
        if (status == FT_OK)
        {
            for (unsigned int i = 0; i < numDevs; i++)
            {
                list.append(QString::fromAscii(devInfo[i].Description));
            }
            delete [] devInfo;
            return list;
        } else {
            delete [] devInfo;
            return list;
        }
    }

    static QStringList getSerialnumberList(void)
    {
        FT_STATUS status;
        QStringList list;
        FT_DEVICE_LIST_INFO_NODE *devInfo;
        DWORD numDevs = FtdiDevice::getDevicesAmount();
        if (numDevs <= 0)
            return list;
        devInfo = new FT_DEVICE_LIST_INFO_NODE[numDevs];
        status = FT_GetDeviceInfoList(devInfo, &numDevs);
        if (status == FT_OK)
        {
            for (unsigned int i = 0; i < numDevs; i++)
            {
                list.append(QString::fromAscii(devInfo[i].SerialNumber));
            }
            delete [] devInfo;
            return list;
        } else {
            delete [] devInfo;
            return list;
        }
    }

private:
    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;
    int baudrate;
    bool open;
};

#endif // FTDIDEVICE_H
