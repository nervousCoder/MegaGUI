#ifndef MEGAETHERNET_H
#define MEGAETHERNET_H

#include <QtNetwork/QHostAddress>
#include "ftdidevice.h"

#define HTONS32(x)	((x & 0xFF000000)>>24)+((x & 0x00FF0000)>>8)+((x & 0x0000FF00)<<8)+((x & 0x000000FF)<<24)

class MegaEthernet
{   
public:

    #pragma pack(1)
    struct FILE_TABLE {
        uint8_t filename[14];
        uint16_t size;
        uint32_t pointer;
    };
    struct DEVICE_SETTINGS {
        uint8_t mac[6];
        uint32_t ip;
        uint32_t mask;
        uint32_t gate;
        uint32_t dns;
        uint32_t ntp;
        uint32_t gg;
        uint32_t gg_ip;
        char gg_pass[10];
        char httpd_pass[25];
        char telnetd_pass[10];
        uint8_t dhcp_enabled;
    };
    #pragma pack()

    MegaEthernet();
    void setDevice(int dev);
    void setDevice(QString serial);
    int connect(void);
    int disconnect(void);
    int open(void);
    void close(void);
    int readConfiguration();
    int writeConfiguration();
    int readFlashFiletable();
    QString getHttpdLogin();
    QString getHttpdPass();
    void setHttpdLoginPass(const QString &login, const QString &pass);

    uint32_t getFlashSize() {return flashSize;}
    int getFilesCount() {return flashFiles;}
    QString getHardwareType() {return hardwareType;}
    QString getMegaEthernetId() {return megaId;}
    bool isGGEnabled() {return ggEnabled;}
    bool isHTTPDEnabled() {return httpdEnabled;}
    bool isTELNETDEnabled() {return telnetdEnabled;}
    bool isNTPEnabled() {return ntpEnabled;}
    bool isDHCPEnabled() {return dhcpEnabled;}
    QHostAddress getIpAddr() {return ip;}
    QHostAddress getMaskAddr() {return mask;}
    QHostAddress getGateAddr() {return gate;}
    uint8_t* getMacAddr() {return &mac[0];}
    QHostAddress getGgIPAddr() {return ggIp;}
    QHostAddress getDnsAddr() {return dns;}
    QHostAddress getNtpAddr() {return ntp;}
    void getMacAddr(uint8_t *buf) {memcpy(buf, mac, 6);}
    uint32_t getGgNumber() {return gg;}
    QString getGgPass() {return ggPass;}

    void setDHCPEnabled(bool s) {dhcpEnabled = s;}
    void setIpAddr(QHostAddress s) {ip = s;}
    void setMaskAddr(QHostAddress s) {mask = s;}
    void setGateAddr(QHostAddress s) {gate = s;}
    void setGgIPAddr(QHostAddress s) {ggIp = s;}
    void setDnsAddr(QHostAddress s) {dns = s;}
    void setNtpAddr(QHostAddress s) {ntp = s;}
    void setGgNumber(uint32_t s) {gg = s;}
    void setGgPass(const QString &s) {ggPass = s;}
    void setTelnetdPass(const QString &s) {telnetdPass = s;}
    void setMacAddr(uint8_t *buf) {memcpy(mac, buf, 6);}

    QList<struct FILE_TABLE> files;

protected:
    int resetDevice(void);
    uint32_t flashSize;
    int flashFiles;
    QString hardwareType;
    QString megaId;
    bool ggEnabled;
    bool httpdEnabled;
    bool telnetdEnabled;
    bool ntpEnabled;
    bool dhcpEnabled;
    QHostAddress ip;
    QHostAddress mask;
    QHostAddress gate;
    uint8_t mac[6];
    QHostAddress ggIp;
    QHostAddress dns;
    QHostAddress ntp;
    uint32_t gg;
    QString ggPass;
    QString httpdPass;
    QString telnetdPass;

private:
    FtdiDevice *ftdi;
    int device;
};

#endif // MEGAETHERNET_H
