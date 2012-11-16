#include "megaethernet.h"
#include "ftdidevice.h"

#define CPU             0xFA
#define FLASH		0xFB
#define CONF		0xFC
#define EXIT		0xFD
#define ESCAPE		0xF4
#define FLASH_WRITE	0xFE
#define FLASH_READ	0xF2
#define EEPROM_WRITE	0xC0
#define EEPROM_READ     0xC1

#define FILE_TABLE_ADDR	0x01
#define TABLE_SIZE_ADDR	0x00

#define GG_CONF_BIT         (1<<7)
#define HTTPD_CONF_BIT      (1<<6)
#define TELNETD_CONF_BIT    (1<<5)
#define NTP_CONF_BIT        (1<<4)

MegaEthernet::MegaEthernet()
{

}

void MegaEthernet::setDevice(int dev)
{
    device = dev;
}

int MegaEthernet::connect(void)
{
    if (ftdi->isOpen() == false)
        return -1;
    resetDevice();
    ftdi->flush();
    int t = 0;
    while (ftdi->readByte(100) != 0xEA)
    {
        if (t++ == 20)
        {
            return -1;
        }
    }
    ftdi->flush();
    ftdi->writeByte(0xEB);
    ftdi->writeByte(0x99);
    if (ftdi->readByte(1000) != 0x9A)
        return -1;
    return 0;
}

int MegaEthernet::disconnect(void)
{
    if (!ftdi->isOpen())
       return -1;
    resetDevice();
    return 0;
}

int MegaEthernet::open(void)
{
    ftdi = new FtdiDevice(device, FtdiDevice::FTDI_BAUD_230400);
    if (ftdi->isOpen())
    {
        return 0;
    } else {
        delete ftdi;
        return -1;
    }
}

void MegaEthernet::close(void)
{
    if (ftdi != NULL)
       delete ftdi;
}

int MegaEthernet::resetDevice(void)
{
    if (ftdi->isOpen() == false)
        return -1;
    ftdi->setRts();
    ftdi->readByte(10);
    ftdi->clrRts();
    return 0;
}

void MegaEthernet::setDevice(QString serial)
{
    QStringList list = FtdiDevice::getSerialnumberList();
    for (int i = 0; i < list.count(); i++)
    {
        if (list.at(i) == serial)
        {
            device = i;
            break;
        }
    }
}

int MegaEthernet::readConfiguration()
{
    if (ftdi->isOpen() == false)
        return -1;
    // Read hardware type
    ftdi->writeByte(CPU);
    int cpu = ftdi->readByte(100);
    if (cpu < 0)
        return -1;
    switch (cpu)
    {
    case 32:
        hardwareType = "ATmega32";
        break;
    case 16:
        hardwareType = "ATmega16";
        break;
    default:
        return -1;
    }
    // Flash size
    ftdi->writeByte(FLASH);
    int flashSize_ = ftdi->readByte(100);
    if (flashSize_ < 0)
        return -1;
    flashSize = flashSize_*1024*1024/8;
    // Conf
    ftdi->writeByte(CONF);
    int confVal = ftdi->readByte(100);
    if (confVal < 0)
        return -1;
    ggEnabled = (confVal & GG_CONF_BIT)?true:false;
    httpdEnabled = (confVal & HTTPD_CONF_BIT)?true:false;
    telnetdEnabled = (confVal & TELNETD_CONF_BIT)?true:false;
    ntpEnabled = (confVal & NTP_CONF_BIT)?true:false;
    // EEPROM config table
    uint8_t buf[sizeof(struct DEVICE_SETTINGS)];
    uint16_t size = sizeof(struct DEVICE_SETTINGS);
    ftdi->flush();
    ftdi->writeByte(EEPROM_READ);
    ftdi->writeByte((size>>8)&0xFF);
    ftdi->writeByte(size&0xFF);
    if (ftdi->read(buf, size, 5000) < 0)
        return -1;
    struct DEVICE_SETTINGS *sett = (struct DEVICE_SETTINGS*)&buf[0];
    ip.setAddress(HTONS32(sett->ip));
    mask.setAddress(HTONS32(sett->mask));
    gate.setAddress(HTONS32(sett->gate));
    ntp.setAddress(HTONS32(sett->ntp));
    dns.setAddress(HTONS32(sett->dns));
    ggIp.setAddress(HTONS32(sett->gg_ip));
    memcpy(mac, sett->mac, 6);
    dhcpEnabled = sett->dhcp_enabled?true:false;
    gg = sett->gg;
    ggPass = (char*)&sett->gg_pass[0];
    httpdPass = (char*)&sett->httpd_pass[0];
    telnetdPass = (char*)&sett->telnetd_pass[0];
    return 0;
}

int MegaEthernet::readFlashFiletable()
{
    files.clear();
    if (ftdi->isOpen() == false)
        return -1;
    // Read files count
    uint32_t size = 1;
/** WTF? **/
    ftdi->flush();
    ftdi->readByte(1000);
/** ?FTW **/
    ftdi->flush();
    ftdi->writeByte(FLASH_READ);
    ftdi->writeByte((size>>24)&0xFF);
    ftdi->writeByte((size>>16)&0xFF);
    ftdi->writeByte((size>>8)&0xFF);
    ftdi->writeByte(size&0xFF);
    int filesAmount = ftdi->readByte(1000);
    if (filesAmount < 0)
        return -1;
    if (filesAmount != 255)
        flashFiles = filesAmount;
    else
        flashFiles = 0;
    // Read filenames
    size = flashFiles*sizeof(struct FILE_TABLE)+1;
    ftdi->flush();
    ftdi->writeByte(FLASH_READ);
    ftdi->writeByte((size>>24)&0xFF);
    ftdi->writeByte((size>>16)&0xFF);
    ftdi->writeByte((size>>8)&0xFF);
    ftdi->writeByte(size&0xFF);
    uint8_t *buf = new uint8_t[size];
    if (ftdi->read(buf, size, 5000) < 0)
    {
        delete [] buf;
        return -1;
    }
    struct FILE_TABLE *fileTable = (struct FILE_TABLE*)&buf[FILE_TABLE_ADDR];
    for (int i = 0; i < flashFiles; i++)
    {
        files.append(fileTable[i]);
    }
    delete [] buf;
    return 0;
}

int MegaEthernet::writeConfiguration()
{
    struct DEVICE_SETTINGS sett;
    sett.dhcp_enabled = dhcpEnabled;
    sett.dns = HTONS32(dns.toIPv4Address());
    sett.gate = HTONS32(gate.toIPv4Address());
    sett.gg_ip = HTONS32(ggIp.toIPv4Address());
    sett.ip = HTONS32(ip.toIPv4Address());
    sett.mask = HTONS32(mask.toIPv4Address());
    sett.ntp = HTONS32(ntp.toIPv4Address());
    sett.gg = gg;
    strncpy(sett.gg_pass, ggPass.toAscii(), 10);
    strncpy(sett.telnetd_pass, telnetdPass.toAscii(), 10);
    strncpy(sett.httpd_pass, httpdPass.toAscii(), 25);
    memcpy(sett.mac, mac, 6);
    // WRITE!
    uint8_t *buffer = (uint8_t*)&sett;
    ftdi->flush();
    ftdi->writeByte(EEPROM_WRITE);
    uint16_t table_size = sizeof(struct DEVICE_SETTINGS);
    // Send conntent
    for (uint16_t i = 0; i < table_size; i++)
    {
        ftdi->writeByte(buffer[i]);
        if (!((i+1) % 100) && ((i+1) >= 100))
        {
            if (ftdi->readByte(5000) != ESCAPE)
            {
                return -1;
            }
        }
        if (i+1 == table_size)
        {
            for (uint16_t a = 0; a < 100-(i%100); a++)
            {
                ftdi->writeByte(0x00);
            }
            if (ftdi->readByte(5000) == ESCAPE)
            {
                return 0;
            }
        }
    }
    return -1;
}

void MegaEthernet::setHttpdLoginPass(const QString &login, const QString &pass)
{
    if (login.isEmpty() || pass.isEmpty())
       return;
    QString a;
    a.append(login);
    a.append(":");
    a.append(pass);
    QByteArray w = QByteArray(a.toAscii()).toBase64();
    httpdPass = w;
}

QString MegaEthernet::getHttpdLogin()
{
    QString z = QByteArray().fromBase64(httpdPass.toAscii());
    QStringList q = z.split(":");
    if (q.count() != 2)
        return QString();
    return q.at(0);
}

QString MegaEthernet::getHttpdPass()
{
    QString z = QByteArray().fromBase64(httpdPass.toAscii());
    QStringList q = z.split(":");
    if (q.count() != 2)
        return QString();
    return q.at(1);
}
