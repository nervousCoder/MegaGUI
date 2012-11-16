#include <QApplication>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QGroupBox>
#include <QMessageBox>
#include "wizard.h"
#include "ftdidevice.h"

MegaEthernet Wizard::device;

Wizard::Wizard(QWidget *parent) : QWizard(parent)
{
    resize(600, 500);
    setWizardStyle(QWizard::AeroStyle);
    setWindowTitle(tr("MegaGUI"));
    addPage(new IntroPage(this));
    addPage(new DevicesPage(this));
    addPage(new ProgressPage(this));
    addPage(new ActionPage(this));
    addPage(new ReadFilesPage(this));
    addPage(new WriteFilesPage(this));
    addPage(new ConfigurationPage(this));
}

Wizard::~Wizard()
{
}

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(QApplication::translate("Wizard", "MegaEthernet konfigurator.", 0, QApplication::UnicodeUTF8));
    setSubTitle(QApplication::translate("Wizard", "by Damian Kmiecik", 0, QApplication::UnicodeUTF8));
    textBrowser = new QTextBrowser(this);
    textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
    textBrowser->setFrameShape(QFrame::NoFrame);
    textBrowser->setFrameShadow(QFrame::Plain);
    textBrowser->setLineWidth(0);
    textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textBrowser);
    setLayout(layout);
    textBrowser->setHtml(QApplication::translate(
        "Wizard",
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:7.8pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><img src=\":/images/MegaEthernet.png\" /></p>\n"
        "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p>\n"
        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Narz\304\231dzie pozwoli Ci na zmian\304\231 ustawie\305\204 po\305\202\304\205czenia Ethernet, wy\305\202\304\205czenia/w\305\202\304\205czenia niekt\303\263rych funkcjinalno\305\233ci "
                            "i zaprogramowania pami\304\231ci Flash.</span></p>\n"
        "<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Program jest udost\304\231pniony na licencji OEM i nie mo\305\274e by\304\207 u\305\274ywany na sprz\304\231cie innym ni\305\274 MegaEthernet. Autor zastrzega sobie do niego wszelkie prawa.</span></p></body></html>", 0, QApplication::UnicodeUTF8
    ));
}

void IntroPage::initializePage()
{
}

DevicesPage::DevicesPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(QApplication::translate("Wizard", "Wybierz urz\304\205dzenie z listy.", 0, QApplication::UnicodeUTF8));
    setSubTitle(QApplication::translate("Wizard", "Obs\305\202ugiwane s\304\205 tylko uk\305\202ady MegaEthernet.", 0, QApplication::UnicodeUTF8));
    tableDevices = new QTableWidget(this);
    tableDevices->setColumnCount(2);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    tableDevices->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    tableDevices->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    tableDevices->setSelectionMode(QAbstractItemView::SingleSelection);
    tableDevices->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableDevices->horizontalHeader()->setDefaultSectionSize(200);
    tableDevices->horizontalHeader()->setMinimumSectionSize(100);
    tableDevices->horizontalHeader()->setStretchLastSection(true);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableDevices);
    setLayout(layout);
    QTableWidgetItem *___qtablewidgetitem = tableDevices->horizontalHeaderItem(0);
    ___qtablewidgetitem->setText(QApplication::translate("Wizard", "Kod seryjny", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem1 = tableDevices->horizontalHeaderItem(1);
    ___qtablewidgetitem1->setText(QApplication::translate("Wizard", "Opis", 0, QApplication::UnicodeUTF8));

    connect(tableDevices, SIGNAL(itemSelectionChanged()), this, SIGNAL(completeChanged()));
    connect(tableDevices, SIGNAL(itemSelectionChanged()), this, SLOT(serialcodeUpdate()));
    setField("serialcode", serialcode);
}

ProgressPage::ProgressPage(QWidget *parent) : QWizardPage(parent)
{
    timer = new QTimer;
    timer->setInterval(1000);
    timer->stop();

    setTitle(QApplication::translate("Wizard", "Po\305\202\304\205czono z urz\304\205dzeniem!", 0, QApplication::UnicodeUTF8));
    setSubTitle(QApplication::translate("Wizard", "Odczytywane s\304\205 dane konfiguracyjne...", 0, QApplication::UnicodeUTF8));
    QVBoxLayout *layout = new QVBoxLayout;
    labelConfDev = new QLabel(this);
    labelConfDev->setEnabled(false);
    layout->addWidget(labelConfDev);
    labelConfApps = new QLabel(this);
    labelConfApps->setEnabled(false);
    layout->addWidget(labelConfApps);
    labelConfFlash = new QLabel(this);
    labelConfFlash->setEnabled(false);
    layout->addWidget(labelConfFlash);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 282, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer);
    setLayout(layout);
    labelConfDev->setText(QApplication::translate("Wizard", "Konfiguracja sieci", 0, QApplication::UnicodeUTF8));
    labelConfApps->setText(QApplication::translate("Wizard", "Konfiguracja aplikacji", 0, QApplication::UnicodeUTF8));
    labelConfFlash->setText(QApplication::translate("Wizard", "Pami\304\231\304\207 flash", 0, QApplication::UnicodeUTF8));

    connect(timer, SIGNAL(timeout()), this, SLOT(timerDo()));
}

ActionPage::ActionPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(QApplication::translate("Wizard", "Wybierz akcje do wykonania.", 0, QApplication::UnicodeUTF8));
    setSubTitle(QApplication::translate("Wizard", "Poni\305\274ej prezentowane s\304\205 aktualne dane odczytane z urz\304\205dzenia.", 0, QApplication::UnicodeUTF8));

    QVBoxLayout *layout1 = new QVBoxLayout;
    QHBoxLayout *layout2 = new QHBoxLayout;

    QGroupBox *groupBox1 = new QGroupBox(this);
    QFormLayout *formLayout1 = new QFormLayout(groupBox1);

    QLabel *label1 = new QLabel(groupBox1);
    formLayout1->setWidget(0, QFormLayout::LabelRole, label1);
    labelDHCP = new QLabel(groupBox1);
    formLayout1->setWidget(0, QFormLayout::FieldRole, labelDHCP);

    QLabel *label2 = new QLabel(groupBox1);
    formLayout1->setWidget(1, QFormLayout::LabelRole, label2);
    labelIP = new QLabel(groupBox1);
    formLayout1->setWidget(1, QFormLayout::FieldRole, labelIP);

    QLabel *label3 = new QLabel(groupBox1);
    formLayout1->setWidget(2, QFormLayout::LabelRole, label3);
    labelMask = new QLabel(groupBox1);
    formLayout1->setWidget(2, QFormLayout::FieldRole, labelMask);

    QLabel *label4 = new QLabel(groupBox1);
    formLayout1->setWidget(3, QFormLayout::LabelRole, label4);
    labelGate = new QLabel(groupBox1);
    formLayout1->setWidget(3, QFormLayout::FieldRole, labelGate);

    QLabel *label5 = new QLabel(groupBox1);
    formLayout1->setWidget(4, QFormLayout::LabelRole, label5);
    labelMAC = new QLabel(groupBox1);
    formLayout1->setWidget(4, QFormLayout::FieldRole, labelMAC);

    layout2->addWidget(groupBox1);

    QGroupBox *groupBox2 = new QGroupBox(this);
    QFormLayout *formLayout2 = new QFormLayout(groupBox2);

    QLabel *label6 = new QLabel(groupBox2);
    formLayout2->setWidget(0, QFormLayout::LabelRole, label6);
    labelGg = new QLabel(groupBox2);
    formLayout2->setWidget(0, QFormLayout::FieldRole, labelGg);

    QLabel *label7 = new QLabel(groupBox2);
    formLayout2->setWidget(1, QFormLayout::LabelRole, label7);
    labelHTTPD = new QLabel(groupBox2);
    formLayout2->setWidget(1, QFormLayout::FieldRole, labelHTTPD);

    QLabel *label8 = new QLabel(groupBox2);
    formLayout2->setWidget(2, QFormLayout::LabelRole, label8);
    labelTELNETD = new QLabel(groupBox2);
    formLayout2->setWidget(2, QFormLayout::FieldRole, labelTELNETD);

    QLabel *label9 = new QLabel(groupBox2);
    formLayout2->setWidget(3, QFormLayout::LabelRole, label9);
    labelNTP = new QLabel(groupBox2);
    formLayout2->setWidget(3, QFormLayout::FieldRole, labelNTP);

    layout2->addWidget(groupBox2);

    QGroupBox *groupBox3 = new QGroupBox(this);
    QFormLayout *formLayout3 = new QFormLayout(groupBox3);

    QLabel *label10 = new QLabel(groupBox3);
    formLayout3->setWidget(0, QFormLayout::LabelRole, label10);
    labelFlashFiles = new QLabel(groupBox3);
    formLayout3->setWidget(0, QFormLayout::FieldRole, labelFlashFiles);

    QLabel *label11 = new QLabel(groupBox3);
    formLayout3->setWidget(1, QFormLayout::LabelRole, label11);
    labelFlashSize = new QLabel(groupBox3);
    formLayout3->setWidget(1, QFormLayout::FieldRole, labelFlashSize);

    layout2->addWidget(groupBox3);
    layout1->addLayout(layout2);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout1->addItem(verticalSpacer);

    QVBoxLayout *layout3 = new QVBoxLayout;
    radioConf = new QRadioButton(this);
    layout3->addWidget(radioConf);
    radioFlashRead = new QRadioButton(this);
    layout3->addWidget(radioFlashRead);
    radioFlashWrite = new QRadioButton(this);
    layout3->addWidget(radioFlashWrite);
    layout1->addLayout(layout3);

    setLayout(layout1);

    groupBox1->setTitle(QApplication::translate("Wizard", "Konfiguracja sieci", 0, QApplication::UnicodeUTF8));
    label1->setText(QApplication::translate("Wizard", "DHCP:", 0, QApplication::UnicodeUTF8));
    labelDHCP->setText(QApplication::translate("Wizard", "w\305\202\304\205czone", 0, QApplication::UnicodeUTF8));
    label2->setText(QApplication::translate("Wizard", "IP:", 0, QApplication::UnicodeUTF8));
    labelIP->setText(QApplication::translate("Wizard", "z DHCP", 0, QApplication::UnicodeUTF8));
    label3->setText(QApplication::translate("Wizard", "Maska:", 0, QApplication::UnicodeUTF8));
    labelMask->setText(QApplication::translate("Wizard", "z DHCP", 0, QApplication::UnicodeUTF8));
    label4->setText(QApplication::translate("Wizard", "Brama:", 0, QApplication::UnicodeUTF8));
    labelGate->setText(QApplication::translate("Wizard", "z DHCP", 0, QApplication::UnicodeUTF8));
    label5->setText(QApplication::translate("Wizard", "MAC:", 0, QApplication::UnicodeUTF8));
    labelMAC->setText(QApplication::translate("Wizard", "00:00:00:00:00:00", 0, QApplication::UnicodeUTF8));
    groupBox2->setTitle(QApplication::translate("Wizard", "Konfiguracja aplikacji", 0, QApplication::UnicodeUTF8));
    label6->setText(QApplication::translate("Wizard", "Gadu-gadu:", 0, QApplication::UnicodeUTF8));
    labelGg->setText(QApplication::translate("Wizard", "w\305\202\304\205czone", 0, QApplication::UnicodeUTF8));
    label7->setText(QApplication::translate("Wizard", "HTTPD:", 0, QApplication::UnicodeUTF8));
    labelHTTPD->setText(QApplication::translate("Wizard", "w\305\202\304\205czone", 0, QApplication::UnicodeUTF8));
    label8->setText(QApplication::translate("Wizard", "TELNETD:", 0, QApplication::UnicodeUTF8));
    labelTELNETD->setText(QApplication::translate("Wizard", "w\305\202\304\205czone", 0, QApplication::UnicodeUTF8));
    label9->setText(QApplication::translate("Wizard", "NTP:", 0, QApplication::UnicodeUTF8));
    labelNTP->setText(QApplication::translate("Wizard", "w\305\202\304\205czone", 0, QApplication::UnicodeUTF8));
    groupBox3->setTitle(QApplication::translate("Wizard", "Pami\304\231\304\207 flash", 0, QApplication::UnicodeUTF8));
    label10->setText(QApplication::translate("Wizard", "Ilo\305\233\304\207 plik\303\263w:", 0, QApplication::UnicodeUTF8));
    labelFlashFiles->setText(QApplication::translate("Wizard", "0", 0, QApplication::UnicodeUTF8));
    label11->setText(QApplication::translate("Wizard", "Rozmiar pami\304\231ci:", 0, QApplication::UnicodeUTF8));
    labelFlashSize->setText(QApplication::translate("Wizard", "0", 0, QApplication::UnicodeUTF8));
    radioConf->setText(QApplication::translate("Wizard", "Edycja konfiguracji", 0, QApplication::UnicodeUTF8));
    radioFlashRead->setText(QApplication::translate("Wizard", "Odczyt plik\303\263w z Flash", 0, QApplication::UnicodeUTF8));
    radioFlashWrite->setText(QApplication::translate("Wizard", "Zapis plik\303\263w do Flash", 0, QApplication::UnicodeUTF8));
    radioConf->setChecked(true);

    connect(radioConf, SIGNAL(toggled(bool)), this, SIGNAL(completeChanged()));
    connect(radioFlashRead, SIGNAL(toggled(bool)), this, SIGNAL(completeChanged()));
    connect(radioFlashWrite, SIGNAL(toggled(bool)), this, SIGNAL(completeChanged()));
}

ReadFilesPage::ReadFilesPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(QApplication::translate("Wizard", "Odczyt plik\303\263w z pami\304\231ci Flash.", 0, QApplication::UnicodeUTF8));
    setSubTitle(QApplication::translate("Wizard", "Wybierz pliki do odczytania.", 0, QApplication::UnicodeUTF8));
    tableFlashRead = new QTableWidget(this);
    tableFlashRead->setColumnCount(2);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    tableFlashRead->setHorizontalHeaderItem(0, __qtablewidgetitem2);
    QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
    tableFlashRead->setHorizontalHeaderItem(1, __qtablewidgetitem3);
    tableFlashRead->setDragDropMode(QAbstractItemView::NoDragDrop);
    tableFlashRead->setSelectionMode(QAbstractItemView::MultiSelection);
    tableFlashRead->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableFlashRead->horizontalHeader()->setStretchLastSection(true);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableFlashRead);
    setLayout(layout);
    QTableWidgetItem *___qtablewidgetitem2 = tableFlashRead->horizontalHeaderItem(0);
    ___qtablewidgetitem2->setText(QApplication::translate("Wizard", "Rozmiar", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem3 = tableFlashRead->horizontalHeaderItem(1);
    ___qtablewidgetitem3->setText(QApplication::translate("Wizard", "Nazwa", 0, QApplication::UnicodeUTF8));
}

WriteFilesPage::WriteFilesPage(QWidget *parent) : QWizardPage(parent)
{
    setTitle(QApplication::translate("Wizard", "Zapis plik\303\263w do pami\304\231ci Flash.", 0, QApplication::UnicodeUTF8));
    setSubTitle(QApplication::translate("Wizard", "Dodaj pliki do zapisu.", 0, QApplication::UnicodeUTF8));

    QVBoxLayout *layout2 = new QVBoxLayout;
    tableFlashWrite = new QTableWidget(this);
    tableFlashWrite->setColumnCount(2);
    QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
    tableFlashWrite->setHorizontalHeaderItem(0, __qtablewidgetitem4);
    QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
    tableFlashWrite->setHorizontalHeaderItem(1, __qtablewidgetitem5);
    tableFlashWrite->setObjectName(QString::fromUtf8("tableFlashWrite"));
    tableFlashWrite->setDragDropMode(QAbstractItemView::DropOnly);
    tableFlashWrite->setSelectionMode(QAbstractItemView::MultiSelection);
    tableFlashWrite->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableFlashWrite->horizontalHeader()->setStretchLastSection(true);

    layout2->addWidget(tableFlashWrite);

    QHBoxLayout *layout3 = new QHBoxLayout;
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout3->addItem(horizontalSpacer);
    pushRemove = new QPushButton(this);
    layout3->addWidget(pushRemove);
    pushAdd = new QPushButton(this);
    layout3->addWidget(pushAdd);
    layout2->addLayout(layout3);

    QHBoxLayout *layout4 = new QHBoxLayout();
    QLabel *label1 = new QLabel(this);
    layout4->addWidget(label1);
    progressFlashSize = new QProgressBar(this);
    progressFlashSize->setValue(0);
    layout4->addWidget(progressFlashSize);

    layout2->addLayout(layout4);
    QHBoxLayout *layout1 = new QHBoxLayout;
    layout1->addLayout(layout2);
    setLayout(layout1);

    QTableWidgetItem *___qtablewidgetitem4 = tableFlashWrite->horizontalHeaderItem(0);
    ___qtablewidgetitem4->setText(QApplication::translate("Wizard", "Rozmiar", 0, QApplication::UnicodeUTF8));
    QTableWidgetItem *___qtablewidgetitem5 = tableFlashWrite->horizontalHeaderItem(1);
    ___qtablewidgetitem5->setText(QApplication::translate("Wizard", "Nazwa", 0, QApplication::UnicodeUTF8));
    pushRemove->setText(QApplication::translate("Wizard", "Usu\305\204", 0, QApplication::UnicodeUTF8));
    pushAdd->setText(QApplication::translate("Wizard", "Dodaj", 0, QApplication::UnicodeUTF8));
    label1->setText(QApplication::translate("Wizard", "Pami\304\231\304\207:", 0, QApplication::UnicodeUTF8));
    progressFlashSize->setFormat(QApplication::translate("Wizard", "%vB z %mB", 0, QApplication::UnicodeUTF8));
}

void DevicesPage::initializePage(void)
{
    tableDevices->clearContents();
    int amount = FtdiDevice::getDevicesAmount();
    // Serach all devices if any
    if (!amount)
    {
//        QMessageBox::critical(
//            this,
//            QApplication::translate("Wizard", "MegaGUI", 0, QApplication::UnicodeUTF8),
//            QApplication::translate("Wizard", "Nie znaleziono urz\304\205dzeĹ„!", 0, QApplication::UnicodeUTF8)
//        );
        return;
    }
    QStringList serialNumbers = FtdiDevice::getSerialnumberList();
    QStringList descriptions = FtdiDevice::getDescriptionsList();
    // Write to the table
    tableDevices->setRowCount(amount);
    for (int i = 0; i < amount; i++)
    {
        QTableWidgetItem *qtablewidgetitem = new QTableWidgetItem;
        qtablewidgetitem->setText(QString().setNum(i+1));
        tableDevices->setVerticalHeaderItem(i, qtablewidgetitem);

        qtablewidgetitem = new QTableWidgetItem();
        qtablewidgetitem->setText(serialNumbers.at(i));
        tableDevices->setItem(i, 0, qtablewidgetitem);

        qtablewidgetitem = new QTableWidgetItem();
        qtablewidgetitem->setText(descriptions.at(i));
        tableDevices->setItem(i, 1, qtablewidgetitem);
    }
}

void ProgressPage::initializePage()
{
    labelConfDev->setEnabled(false);
    labelConfApps->setEnabled(false);
    labelConfFlash->setEnabled(false);
    timer->start();
}

void ActionPage::initializePage()
{
    labelFlashFiles->setText(QString().setNum(
        Wizard::device.getFilesCount()
    ));
    labelFlashSize->setText(QString().setNum(
        Wizard::device.getFlashSize()/1024
    ).append("kB"));
    QString on = QApplication::translate("Wizard", "włączony", 0, QApplication::UnicodeUTF8);
    QString off = QApplication::translate("Wizard", "wyłączony", 0, QApplication::UnicodeUTF8);
    labelGg->setText(
        Wizard::device.isGGEnabled()?on:off
    );
    labelHTTPD->setText(
        Wizard::device.isHTTPDEnabled()?on:off
    );
    labelTELNETD->setText(
        Wizard::device.isTELNETDEnabled()?on:off
    );
    labelNTP->setText(
        Wizard::device.isNTPEnabled()?on:off
    );
    labelDHCP->setText(
        Wizard::device.isDHCPEnabled()?on:off
    );
    labelIP->setText(
        Wizard::device.getIpAddr().toString()
    );
    labelMask->setText(
        Wizard::device.getMaskAddr().toString()
    );
    labelGate->setText(
        Wizard::device.getGateAddr().toString()
    );
    // MAC
    uint8_t buf[6];
    Wizard::device.getMacAddr(buf);
    QString a;
    a.sprintf(
        "%02X:%02X:%02X:%02X:%02X:%02X",
        buf[0], buf[1], buf[2],
        buf[3], buf[4], buf[5]
    );
    labelMAC->setText(a);
}

void ReadFilesPage::initializePage()
{
    tableFlashRead->clearContents();
    if (!Wizard::device.files.count())
        return;
    tableFlashRead->setRowCount(Wizard::device.files.count());
    for (int i = 0; i < Wizard::device.files.count(); i++)
    {
        QTableWidgetItem *qtablewidgetitem = new QTableWidgetItem;
        qtablewidgetitem->setText(QString().setNum(i+1));
        tableFlashRead->setVerticalHeaderItem(i, qtablewidgetitem);

        const struct MegaEthernet::FILE_TABLE *tableItem  = &Wizard::device.files.at(i);

        qtablewidgetitem = new QTableWidgetItem();
        qtablewidgetitem->setText(QString().setNum(tableItem->size).append(tr(" B")));
        tableFlashRead->setItem(i, 0, qtablewidgetitem);

        qtablewidgetitem = new QTableWidgetItem();
        qtablewidgetitem->setText(QString().fromAscii((const char*)tableItem->filename));
        tableFlashRead->setItem(i, 1, qtablewidgetitem);
    }
}

void WriteFilesPage::initializePage()
{
}

bool DevicesPage::isComplete() const
{
    return tableDevices->selectedItems().count()?true:false;
}

void DevicesPage::serialcodeUpdate()
{
    if (tableDevices->selectedItems().count())
    {
        int row = tableDevices->selectedItems().at(0)->row();
        serialcode = tableDevices->item(row, 0)->text();
    }
}

int DevicesPage::nextId() const
{
    if (isComplete())
    {
        Wizard::device.setDevice(serialcode);
        if (Wizard::device.open() == 0)
        {
            if (Wizard::device.connect() == 0)
            {
                return Wizard::progressPage;
            }
        }
        Wizard::device.close();
        QMessageBox::critical(
            wizard(),
            QApplication::translate("Wizard", "MegaGUI", 0, QApplication::UnicodeUTF8),
            QApplication::translate("Wizard", "Urządzenie nie odpowiada.", 0, QApplication::UnicodeUTF8)
        );
        return Wizard::devicesPage;
    } else
        return Wizard::devicesPage;
}

void ProgressPage::timerDo()
{
    static int a = 0;
    if (a == 0)
    {
        if (Wizard::device.readConfiguration() < 0)
        {
            QMessageBox::critical(
                wizard(),
                QApplication::translate("Wizard", "MegaGUI", 0, QApplication::UnicodeUTF8),
                QApplication::translate("Wizard", "Nie można odczytać danych konfiguracyjnych!", 0, QApplication::UnicodeUTF8)
            );
            a = 0;
            timer->stop();
        } else {
            labelConfDev->setEnabled(true);

            a++;
        }
    } else if (a == 1) {
        labelConfApps->setEnabled(true);
        a++;
    } else if (a == 2) {
        if (Wizard::device.readFlashFiletable() < 0)
        {
            QMessageBox::critical(
                wizard(),
                QApplication::translate("Wizard", "MegaGUI", 0, QApplication::UnicodeUTF8),
                QApplication::translate("Wizard", "Nie można odczytać pamięci flash!", 0, QApplication::UnicodeUTF8)
            );
            a = 0;
            timer->stop();
        } else {
            labelConfFlash->setEnabled(true);
            a = 0;
            timer->stop();
        }
    }
}

int ProgressPage::nextId() const
{
    return Wizard::actionPage;
}

int ActionPage::nextId() const
{
    if (radioFlashRead->isChecked())
        return Wizard::readFilesPage;
    if (radioConf->isChecked())
        return Wizard::configurationPage;
    if (radioFlashWrite->isChecked())
        return Wizard::writeFilesPage;
    return -1;
}

bool ActionPage::isComplete() const
{
    if (radioConf->isChecked())
        return true;
    if (radioFlashRead->isChecked())
        return true;
    if (radioFlashWrite->isChecked())
        return true;
    return false;
}

ConfigurationPage::ConfigurationPage(QWidget *parent)
{
    setTitle(QApplication::translate("Wizard", "Edycja ustawień MegaEthernet.", 0, QApplication::UnicodeUTF8));
    setSubTitle(QApplication::translate("Wizard", "Ustawienia zostną zapisane do pamięci EEPROM.", 0, QApplication::UnicodeUTF8));

    QVBoxLayout *vlayout = new QVBoxLayout;
    QHBoxLayout *hlayout1 = new QHBoxLayout;

    QGroupBox *groupBox1 = new QGroupBox(this);
    QFormLayout *formLayout1 = new QFormLayout(groupBox1);

    QLabel *label1 = new QLabel(groupBox1);
    formLayout1->setWidget(0, QFormLayout::LabelRole, label1);
    checkDHCP = new QCheckBox(groupBox1);
    formLayout1->setWidget(0, QFormLayout::FieldRole, checkDHCP);

    QLabel *label2 = new QLabel(groupBox1);
    formLayout1->setWidget(1, QFormLayout::LabelRole, label2);
    editIP = new QLineEdit(groupBox1);
    formLayout1->setWidget(1, QFormLayout::FieldRole, editIP);

    QLabel *label3 = new QLabel(groupBox1);
    formLayout1->setWidget(2, QFormLayout::LabelRole, label3);
    editMask = new QLineEdit(groupBox1);
    formLayout1->setWidget(2, QFormLayout::FieldRole, editMask);

    QLabel *label4 = new QLabel(groupBox1);
    formLayout1->setWidget(3, QFormLayout::LabelRole, label4);
    editGate = new QLineEdit(groupBox1);
    formLayout1->setWidget(3, QFormLayout::FieldRole, editGate);

    QLabel *label5 = new QLabel(groupBox1);
    formLayout1->setWidget(4, QFormLayout::LabelRole, label5);
    editDns = new QLineEdit(groupBox1);
    formLayout1->setWidget(4, QFormLayout::FieldRole, editDns);

    QLabel *label6 = new QLabel(groupBox1);
    formLayout1->setWidget(5, QFormLayout::LabelRole, label6);
    editMAC = new QLineEdit(groupBox1);
    formLayout1->setWidget(5, QFormLayout::FieldRole, editMAC);

    hlayout1->addWidget(groupBox1);
    QGroupBox *groupBox2 = new QGroupBox(this);
    QFormLayout *formLayout2 = new QFormLayout(groupBox2);

    QLabel *label7 = new QLabel(groupBox2);
    formLayout2->setWidget(0, QFormLayout::LabelRole, label7);
    editGgIp = new QLineEdit(groupBox2);
    formLayout2->setWidget(0, QFormLayout::FieldRole, editGgIp);

    QLabel *label8 = new QLabel(groupBox2);
    formLayout2->setWidget(1, QFormLayout::LabelRole, label8);
    editGgNumber = new QLineEdit(groupBox2);
    formLayout2->setWidget(1, QFormLayout::FieldRole, editGgNumber);

    QLabel *label9 = new QLabel(groupBox2);
    formLayout2->setWidget(2, QFormLayout::LabelRole, label9);
    editGgPass = new QLineEdit(groupBox2);
    formLayout2->setWidget(2, QFormLayout::FieldRole, editGgPass);

    hlayout1->addWidget(groupBox2);
    vlayout->addLayout(hlayout1);
    QHBoxLayout *hlayout2 = new QHBoxLayout;

    QGroupBox *groupBox3 = new QGroupBox(this);
    QFormLayout *formLayout3 = new QFormLayout(groupBox3);

    QLabel *label10 = new QLabel(groupBox3);
    formLayout3->setWidget(0, QFormLayout::LabelRole, label10);
    editHttpdLogin = new QLineEdit(groupBox3);
    formLayout3->setWidget(0, QFormLayout::FieldRole, editHttpdLogin);

    QLabel *label11 = new QLabel(groupBox3);
    formLayout3->setWidget(1, QFormLayout::LabelRole, label11);
    editHttpdPass = new QLineEdit(groupBox3);
    formLayout3->setWidget(1, QFormLayout::FieldRole, editHttpdPass);

    hlayout2->addWidget(groupBox3);
    QGroupBox *groupBox4 = new QGroupBox(this);
    QFormLayout *formLayout4 = new QFormLayout(groupBox4);

    QLabel *label12 = new QLabel(groupBox4);
    formLayout4->setWidget(0, QFormLayout::LabelRole, label12);
    editNtp = new QLineEdit(groupBox4);
    formLayout4->setWidget(0, QFormLayout::FieldRole, editNtp);

    hlayout2->addWidget(groupBox4);
    vlayout->addLayout(hlayout2);
    setLayout(vlayout);

    groupBox1->setTitle(QApplication::translate("Wizard", "Konfiguracja sieci", 0, QApplication::UnicodeUTF8));
    groupBox2->setTitle(QApplication::translate("Wizard", "Gadu-gadu", 0, QApplication::UnicodeUTF8));
    groupBox3->setTitle(QApplication::translate("Wizard", "HTTPD", 0, QApplication::UnicodeUTF8));
    groupBox4->setTitle(QApplication::translate("Wizard", "NTP", 0, QApplication::UnicodeUTF8));
    label1->setText(QApplication::translate("Wizard", "DHCP:", 0, QApplication::UnicodeUTF8));
    label2->setText(QApplication::translate("Wizard", "IP:", 0, QApplication::UnicodeUTF8));
    label3->setText(QApplication::translate("Wizard", "Maska:", 0, QApplication::UnicodeUTF8));
    label4->setText(QApplication::translate("Wizard", "Brama:", 0, QApplication::UnicodeUTF8));
    label5->setText(QApplication::translate("Wizard", "DNS:", 0, QApplication::UnicodeUTF8));
    label6->setText(QApplication::translate("Wizard", "MAC:", 0, QApplication::UnicodeUTF8));
    label7->setText(QApplication::translate("Wizard", "IP serwera:", 0, QApplication::UnicodeUTF8));
    label8->setText(QApplication::translate("Wizard", "Numer:", 0, QApplication::UnicodeUTF8));
    label9->setText(QApplication::translate("Wizard", "Hasło:", 0, QApplication::UnicodeUTF8));
    label10->setText(QApplication::translate("Wizard", "Login:", 0, QApplication::UnicodeUTF8));
    label11->setText(QApplication::translate("Wizard", "Hasło:", 0, QApplication::UnicodeUTF8));
    label12->setText(QApplication::translate("Wizard", "IP serwera:", 0, QApplication::UnicodeUTF8));

    QString Octet = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    editIP->setValidator(new QRegExpValidator(
        QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"),
        this
    ));
    editMask->setValidator(new QRegExpValidator(
        QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"),
        this
    ));
    editGate->setValidator(new QRegExpValidator(
        QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"),
        this
    ));
    editDns->setValidator(new QRegExpValidator(
        QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"),
        this
    ));
    editGgIp->setValidator(new QRegExpValidator(
        QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"),
        this
    ));
    editNtp->setValidator(new QRegExpValidator(
        QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"),
        this
    ));
    editMAC->setInputMask("HH:HH:HH:HH:HH:HH;_");
}

void ConfigurationPage::initializePage()
{
    checkDHCP->setChecked(Wizard::device.isDHCPEnabled());
    editIP->setText(Wizard::device.getIpAddr().toString());
    editMask->setText(Wizard::device.getMaskAddr().toString());
    editGate->setText(Wizard::device.getGateAddr().toString());
    editDns->setText(Wizard::device.getDnsAddr().toString());
    editNtp->setText(Wizard::device.getNtpAddr().toString());
    editGgIp->setText(Wizard::device.getGgIPAddr().toString());
    editGgNumber->setText(QString().setNum(Wizard::device.getGgNumber()));
    editGgPass->setText(Wizard::device.getGgPass());
    editHttpdLogin->setText(Wizard::device.getHttpdLogin());
    editHttpdPass->setText(Wizard::device.getHttpdPass());
    // MAC
    uint8_t buf[6];
    Wizard::device.getMacAddr(buf);
    QString a;
    a.sprintf(
        "%02X:%02X:%02X:%02X:%02X:%02X",
        buf[0], buf[1], buf[2],
        buf[3], buf[4], buf[5]
    );
    editMAC->setText(a);
}

int ConfigurationPage::nextId() const
{
    return -1;
}

int WriteFilesPage::nextId() const
{
    return -1;
}

int ReadFilesPage::nextId() const
{
    return -1;
}

bool ConfigurationPage::validatePage()
{
    Wizard::device.setDHCPEnabled(checkDHCP->isChecked());
    Wizard::device.setDnsAddr(QHostAddress(editDns->text()));
    Wizard::device.setIpAddr(QHostAddress(editIP->text()));
    Wizard::device.setMaskAddr(QHostAddress(editMask->text()));
    Wizard::device.setGateAddr(QHostAddress(editGate->text()));
    Wizard::device.setNtpAddr(QHostAddress(editNtp->text()));
    Wizard::device.setGgNumber(editGgNumber->text().toLong());
    Wizard::device.setGgPass(editGgPass->text());
    Wizard::device.setGgIPAddr(QHostAddress(editGgIp->text()));
    Wizard::device.setHttpdLoginPass(editHttpdLogin->text(), editHttpdPass->text());
//    Wizard::device.setTelnetdPass(editTelnetdPass->text());
    // MAC
    QStringList l = editMAC->text().split(":");
    if (l.count() == 6)
    {
        uint8_t buf[6];
        buf[0] = l.at(0).toShort(NULL, 16);
        buf[1] = l.at(1).toShort(NULL, 16);
        buf[2] = l.at(2).toShort(NULL, 16);
        buf[3] = l.at(3).toShort(NULL, 16);
        buf[4] = l.at(4).toShort(NULL, 16);
        buf[5] = l.at(5).toShort(NULL, 16);
        Wizard::device.setMacAddr(&buf[0]);
    }
    if (Wizard::device.writeConfiguration() < 0)
    {
        QMessageBox::critical(
            wizard(),
            QApplication::translate("Wizard", "MegaGUI", 0, QApplication::UnicodeUTF8),
            QApplication::translate("Wizard", "Błąd zapisywania danych konfiguracyjnych!", 0, QApplication::UnicodeUTF8)
        );
        return false;
    }
    return true;
}
