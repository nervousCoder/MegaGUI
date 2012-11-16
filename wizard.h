#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include <QWizardPage>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QRadioButton>
#include <QPushButton>
#include <QTextBrowser>
#include <QSpacerItem>
#include <QLineEdit>
#include <QCheckBox>
#include <QTimer>
#include "megaethernet.h"

/** @class Wizard
  */
class Wizard : public QWizard
{
    Q_OBJECT

public:

    enum {
        introPage           = 0,
        devicesPage         = 1,
        progressPage        = 2,
        actionPage          = 3,
        readFilesPage       = 4,
        writeFilesPage      = 5,
        configurationPage   = 6
    };

    explicit Wizard(QWidget *parent = 0);
    ~Wizard();
    static MegaEthernet device;
};

/** @class IntroPage
  */
class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

protected:
    void initializePage(void);

private:
    QTextBrowser *textBrowser;
};

/** @class DevicesPage
  */
class DevicesPage : public QWizardPage
{
    Q_OBJECT

public:
    DevicesPage(QWidget *parent = 0);

protected:
    void initializePage();
    bool isComplete() const;
    int nextId() const;

protected slots:
    void serialcodeUpdate();

private:
    QTableWidget *tableDevices;
    QString serialcode;
};

/** @class ProgressPage
  */
class ProgressPage : public QWizardPage
{
    Q_OBJECT

public:
    ProgressPage(QWidget *parent = 0);

protected:
    void initializePage();
    int nextId() const;

protected slots:
    void timerDo();

private:
    QLabel *labelConfDev;
    QLabel *labelConfApps;
    QLabel *labelConfFlash;
    QTimer *timer;
};

/** @class ActionPage
  */
class ActionPage : public QWizardPage
{
    Q_OBJECT

public:
    ActionPage(QWidget *parent = 0);

protected:
    void initializePage(void);
    bool isComplete() const;
    int nextId() const;

private:
    QLabel *labelDHCP;
    QLabel *labelIP;
    QLabel *labelMask;
    QLabel *labelGate;
    QLabel *labelMAC;
    QLabel *labelGg;
    QLabel *labelHTTPD;
    QLabel *labelTELNETD;
    QLabel *labelNTP;
    QLabel *labelFlashFiles;
    QLabel *labelFlashSize;
    QRadioButton *radioConf;
    QRadioButton *radioFlashRead;
    QRadioButton *radioFlashWrite;
};

/** @class ReadFilesPage
  */
class ReadFilesPage : public QWizardPage
{
    Q_OBJECT

public:
    ReadFilesPage(QWidget *parent = 0);

protected:
    void initializePage(void);
    int nextId() const;

private:
    QTableWidget *tableFlashRead;
};

/** @class WriteFilesPage
  */
class WriteFilesPage : public QWizardPage
{
    Q_OBJECT

public:
    WriteFilesPage(QWidget *parent = 0);

protected:
    void initializePage(void);
    int nextId() const;

private:
    QTableWidget *tableFlashWrite;
    QPushButton *pushRemove;
    QPushButton *pushAdd;
    QProgressBar *progressFlashSize;
};

/** @class ConfigurationPage
  */
class ConfigurationPage : public QWizardPage
{
    Q_OBJECT

public:
    ConfigurationPage(QWidget *parent = 0);

protected:
    void initializePage(void);
    bool validatePage();
    int nextId() const;

private:
    QCheckBox *checkDHCP;
    QLineEdit *editIP;
    QLineEdit *editMask;
    QLineEdit *editGate;
    QLineEdit *editMAC;
    QLineEdit *editGgNumber;
    QLineEdit *editGgIp;
    QLineEdit *editGgPass;
    QLineEdit *editDns;
    QLineEdit *editNtp;
    QLineEdit *editHttpdLogin;
    QLineEdit *editHttpdPass;
    QLineEdit *editTelnetdPass;
};

#endif // WIZARD_H
