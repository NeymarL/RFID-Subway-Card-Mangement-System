#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->fillPortsParameters(ui->baudRateBox);//波特率填充
    this->serialPortThread = new SerialPortThread(this);
    this->serialPort = new QSerialPort(this);
    this->intValidator = new QIntValidator(0, 4000000, this);
    this->m1356dll = new M1356Dll();
    ui->btn_connect->setEnabled(true);
    ui->btn_refresh->setEnabled(true);
    ui->btn_enter->setEnabled(false);
    this->on_btn_refresh_clicked();
    this->addWidgets();
    // handle connect
    connect(serialPortThread, SIGNAL(sendMsg(char*, int)), this, SLOT(onSendMessage(char*, int)));
    connect(serialPortThread, SIGNAL(wirteMsgError(QString)), this, SLOT(onOperationError(QString)));
    connect(serialPortThread, SIGNAL(receivedMsg(QByteArray)), this, SLOT(on_serialMsgreceived(QByteArray)));
}

MainWindow::~MainWindow()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    delete intValidator;
    delete m1356dll;
    delete serialPort;
    delete ui;
}

/**
 * @brief MainWindow::addWidgets
 * 添加子页面
 */
void MainWindow::addWidgets()
{
    MainPage* main_page = new MainPage(this, this->serialPortThread);
    ui->stackedWidget->addWidget(main_page); // 2
    connect(this, SIGNAL(sendCardId(QString)), main_page, SLOT(on_tagIdReceived(QString)));
    connect(serialPortThread, SIGNAL(wirteMsgError(QString)), main_page, SLOT(onOperationError(QString)));
    connect(this, SIGNAL(findFailed()), main_page, SLOT(onOperationError()));
}

//Baudrate parameter init
void MainWindow::fillPortsParameters(QComboBox *box)
{
    box->clear();
    box->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    box->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    box->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    box->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    box->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
}

/**
 * @brief MainWindow::on_btn_connect_clicked
 * 连接串口
 */
void MainWindow::on_btn_connect_clicked()
{
    // 获取串口设备名
    QString name = ui->serialNameBox->currentText();
    // 获取波特率
    QString baud = ui->baudRateBox->currentText().trimmed();
    if (baud.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), "波特率输入错误！");
        return ;
    }
    // 用Qt自带的SerialPort类设置串口设备名
    serialPort->setPortName(name);
    // 设置波特率
    serialPort->setBaudRate(baud.toInt(), QSerialPort::AllDirections);
    // 连接串口
    if (serialPort->open(QIODevice::ReadWrite)) {
        // 连接成功时
        ui->btn_connect->setEnabled(false);
        ui->btn_refresh->setEnabled(false);
        ui->btn_enter->setEnabled(true);
        // 启动传输数据线程开始工作
        serialPortThread->setSerialPort(serialPort);
        serialPortThread->setRetryTimes(RETRY_45);
        serialPortThread->start();
        // 构造第一个数据帧测试连接
        uint16 frameLen;
        uint8 data[1];
        uint8 *p;
        data[0] = RC632_WORK_MODEL_1443A;
        // 调用库函数构造数据帧，命令字为RC632_CMD_CONFIG_ISOTYPE
        p = m1356dll->RC632_SendCmdReq(RC632_CMD_CONFIG_ISOTYPE, data, 1);
        frameLen = BUILD_UINT16(p[0], p[1]);
        // 使用串口数据传输线程发送数据
        serialPortThread->writeData((char *)(p + 2 ), frameLen, true);
        ui->statusBar->showMessage(tr("Connected to %1 : %2 success").arg(name, baud));
        // 等待返回数据
        if (!serialPort->waitForReadyRead(1000)) {
            // 若没收到返回数据，则提示连接异常
            ui->statusBar->showMessage(tr("Not Ready for Read!"));
            ui->btn_connect->setEnabled(true);
            ui->btn_refresh->setEnabled(true);
            ui->btn_enter->setEnabled(false);
            // 关闭串口
            if (serialPort->isOpen()) {
                serialPort->close();
            }
            QMessageBox::warning(this, tr("提示"), tr("连接异常，请重新连接").arg(name), QMessageBox::Yes);
        }
    } else {
        // 连接失败，显示提示信息
        ui->btn_connect->setEnabled(true);
        ui->btn_refresh->setEnabled(true);
        ui->btn_enter->setEnabled(false);
        QMessageBox::warning(this, tr("提示"), tr("初始化%1失败！请检查串口是否已经被占用？").arg(name), QMessageBox::Yes);
    }
}
/**
 * @brief MainWindow::on_btn_enter_clicked
 * 断开连接
 */
void MainWindow::on_btn_enter_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->statusBar->showMessage("");
}
/**
 * @brief MainWindow::on_btn_refresh_clicked
 * 刷新按钮点击事件
 */
void MainWindow::on_btn_refresh_clicked()
{
    QStringList list = getSerialName();
    ui->serialNameBox->clear();
    ui->serialNameBox->addItems(list);
}

/**
 * @brief MainWindow::checkCustomBaudRatePolicy
 * @param idx combox被选中的索引值
 * 设置自定义波特率
 */
void MainWindow::checkCustomBaudRatePolicy(int idx)
{
    QComboBox *box = dynamic_cast<QComboBox*>(QObject::sender());
    bool isCustomBaudRate = !box->itemData(idx).isValid();
    box->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        box->clearEditText();
        box->setValidator(intValidator);
    }
}

/**
 * @brief MainWindow::handleError
 * @param error SerialPortError枚举类,详细请看SerialPortError的定义
 * 处理错误信息
 */
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serialPort->errorString());
    }
}

void MainWindow::on_error(QString error)
{
    QString msg = "Error: ";
    this->ui->statusBar->showMessage(msg + error);
}

/**
 * @brief MainWindow::on_serialMsgreceived
 * @param bytes 接收到的数据字节数组
 * 串口接收消息后的处理槽函数
 */
void MainWindow::on_serialMsgreceived(QByteArray bytes)
{
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if (frame.status.left(2) == "00")
    {
        if (frame.cmd.remove(" ") == "0102") //寻卡
        {
            uint16 frameLen;
            quint8 buffer[1];
            uint8 *p;
            memset(buffer, 0, 1);
            buffer[0] = 0x04;
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_ANTICOLL, buffer, 1);
            frameLen = BUILD_UINT16(p[0], p[1]);
            serialPortThread->writeData((char *)(p + 2 ), frameLen, true);
        }
        else if (frame.cmd.remove(" ") == "0202") //寻卡结果
        {
            uint16 frameLen;
            quint8 buffer[4];
            uint8 *p;
            memset(buffer, 0, 4);
            QSTRING_TO_HEX(frame.vdata.remove(" "), buffer, 4); // 卡号
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_SELECT, buffer, 4);
            frameLen = BUILD_UINT16(p[0], p[1]);
            serialPortThread->writeData((char *)(p + 2 ), frameLen, true);
            tagID = frame.vdata.remove(" ");
        }
        else if (frame.cmd.remove(" ") == "0302")
        {
            emit sendCardId(tagID);
        }
    }
    else
    {
        if (frame.cmd.remove(" ") == "0102") //寻卡
        {
            ui->statusBar->showMessage("寻卡失败，请调整卡与读卡器的距离后再试！");
        }
        else if (frame.cmd.remove(" ") == "0202") //寻卡结果
        {
            ui->statusBar->showMessage("A卡防冲撞失败，请调整卡与读卡器的距离后再试！");
        }
        emit findFailed();
    }
    // ui->statusBar->showMessage(tr("Receievd Message"));
}

/**
 * 显示发送消息
 * @param data     [description]
 * @param frameLen [description]
 */
void MainWindow::onSendMessage(char *data, int frameLen)
{
    QString space;
    space = CharStringtoHexString(tr(" "), data, frameLen);
    ui->statusBar->showMessage(space);
    Q_UNUSED(data);
    Q_UNUSED(frameLen);
}

/**
 * @brief MainWindow::onOperationError
 * @param msg 发送的命令
 * 当串口写入数据无响应时调用该方法
 */
void MainWindow::onOperationError(QString msg)
{
    //QMessageBox::warning(this, tr("温馨提示"), msg, QMessageBox::Yes);
    ui->statusBar->showMessage(msg);
}
