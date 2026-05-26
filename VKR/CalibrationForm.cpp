#include "CalibrationForm.h"
#pragma comment(lib, "advapi32.lib")
using namespace System::Globalization;

// ==================== Конструктор / Деструктор ====================
CalibrationForm::CalibrationForm(void) {
    InitializeComponent();
    hPort = INVALID_HANDLE_VALUE;
    baudRate = CBR_9600;
    byteSize = 8;
    stopBits = ONESTOPBIT;
    parity = NOPARITY;
    portLock = gcnew Object();
    portNames = gcnew List<String^>();
    foundIds = gcnew List<int>();
    backupRegisters = gcnew Dictionary<int, float>();
    lastSlaveId = -1;
    stopPolling = false;
    calibrating = false;
    scanning = false;
    liveTimer = gcnew System::Windows::Forms::Timer();
    liveTimer->Interval = 5000;   // 5 секунд
    liveTimer->Tick += gcnew EventHandler(this, &CalibrationForm::liveTimer_Tick);
    this->Load += gcnew EventHandler(this, &CalibrationForm::OnLoad);
}

CalibrationForm::~CalibrationForm() {
    if (liveTimer != nullptr) { liveTimer->Stop(); delete liveTimer; }
    if (hPort != INVALID_HANDLE_VALUE) CloseHandle(hPort);
}

// ==================== Интерфейс ====================
void CalibrationForm::InitializeComponent() {
    this->Text = L"Калибратор плотномера (много датчиков)";
    this->ClientSize = System::Drawing::Size(480, 360);   // уменьшено примерно на 40%
    this->Font = gcnew System::Drawing::Font(L"Arial", 9);

    // Порт
    comboBoxPorts = gcnew ComboBox();
    comboBoxPorts->Location = Point(12, 12);
    comboBoxPorts->Size = System::Drawing::Size(72, 22);
    this->Controls->Add(comboBoxPorts);

    buttonOpenPort = gcnew Button();
    buttonOpenPort->Text = L"Открыть";
    buttonOpenPort->Location = Point(90, 10);
    buttonOpenPort->Size = System::Drawing::Size(48, 24);
    buttonOpenPort->Click += gcnew EventHandler(this, &CalibrationForm::buttonOpenPort_Click);
    this->Controls->Add(buttonOpenPort);

    buttonClosePort = gcnew Button();
    buttonClosePort->Text = L"Закрыть";
    buttonClosePort->Location = Point(144, 10);
    buttonClosePort->Size = System::Drawing::Size(48, 24);
    buttonClosePort->Enabled = false;
    buttonClosePort->Click += gcnew EventHandler(this, &CalibrationForm::buttonClosePort_Click);
    this->Controls->Add(buttonClosePort);

    buttonScanIds = gcnew Button();
    buttonScanIds->Text = L"Сканировать ID";
    buttonScanIds->Location = Point(198, 10);
    buttonScanIds->Size = System::Drawing::Size(84, 24);
    buttonScanIds->Enabled = false;
    buttonScanIds->Click += gcnew EventHandler(this, &CalibrationForm::buttonScanIds_Click);
    this->Controls->Add(buttonScanIds);

    // Прогресс-бар
    progressBarScan = gcnew ProgressBar();
    progressBarScan->Location = Point(12, 42);
    progressBarScan->Size = System::Drawing::Size(456, 10);
    progressBarScan->Minimum = 0;
    progressBarScan->Maximum = 50;
    progressBarScan->Value = 0;
    progressBarScan->Visible = false;
    this->Controls->Add(progressBarScan);

    // Эталонный датчик
    Label^ lblRef = gcnew Label(); lblRef->Text = L"Эталонный датчик:"; lblRef->Location = Point(12, 60); lblRef->Size = System::Drawing::Size(120, 16); this->Controls->Add(lblRef);
    comboBoxRefDevice = gcnew ComboBox();
    comboBoxRefDevice->DropDownStyle = ComboBoxStyle::DropDownList;
    comboBoxRefDevice->Location = Point(140, 57);
    comboBoxRefDevice->Size = System::Drawing::Size(80, 22);
    comboBoxRefDevice->Enabled = false;
    comboBoxRefDevice->SelectedIndexChanged += gcnew EventHandler(this, &CalibrationForm::comboBoxRefDevice_SelectedIndexChanged);
    this->Controls->Add(comboBoxRefDevice);

    // Плотность эталона
    labelRefDensity = gcnew Label(); labelRefDensity->Text = L"Плотность эталона: — кг/м³"; labelRefDensity->Location = Point(12, 84); labelRefDensity->Size = System::Drawing::Size(460, 16); this->Controls->Add(labelRefDensity);

    // Ручной ввод целевой плотности
    Label^ lblManualTarget = gcnew Label(); lblManualTarget->Text = L"Целевая плотность (если нет эталона):"; lblManualTarget->Location = Point(12, 106); lblManualTarget->Size = System::Drawing::Size(230, 16); this->Controls->Add(lblManualTarget);
    textBoxTargetDensity = gcnew TextBox();
    textBoxTargetDensity->Location = Point(248, 103);
    textBoxTargetDensity->Size = System::Drawing::Size(60, 20);
    textBoxTargetDensity->Text = L"1.2";
    this->Controls->Add(textBoxTargetDensity);

    // Кнопки
    buttonCalibrateAll = gcnew Button(); buttonCalibrateAll->Text = L"Калибровать все"; buttonCalibrateAll->Location = Point(12, 132); buttonCalibrateAll->Size = System::Drawing::Size(100, 24); buttonCalibrateAll->Click += gcnew EventHandler(this, &CalibrationForm::buttonCalibrateAll_Click); this->Controls->Add(buttonCalibrateAll);
    buttonRollback = gcnew Button(); buttonRollback->Text = L"Откат"; buttonRollback->Location = Point(118, 132); buttonRollback->Size = System::Drawing::Size(60, 24); buttonRollback->Enabled = false; buttonRollback->Click += gcnew EventHandler(this, &CalibrationForm::buttonRollback_Click); this->Controls->Add(buttonRollback);

    labelStatus = gcnew Label(); labelStatus->Text = L"Готов"; labelStatus->Location = Point(12, 162); labelStatus->Size = System::Drawing::Size(456, 24); this->Controls->Add(labelStatus);

    // Таблица датчиков
    dataGridViewDevices = gcnew DataGridView();
    dataGridViewDevices->Location = Point(12, 192);
    dataGridViewDevices->Size = System::Drawing::Size(456, 156);   // масштабируемая таблица
    dataGridViewDevices->Anchor = System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right;
    dataGridViewDevices->AllowUserToAddRows = false;
    dataGridViewDevices->AllowUserToDeleteRows = false;
    dataGridViewDevices->ReadOnly = true;
    dataGridViewDevices->ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode::AutoSize;
    dataGridViewDevices->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
    dataGridViewDevices->Columns->Add(L"ID", L"ID");
    dataGridViewDevices->Columns->Add(L"Temperature", L"Температура (°C)");
    dataGridViewDevices->Columns->Add(L"Density", L"Плотность (кг/м³)");
    dataGridViewDevices->Columns->Add(L"Frequency", L"Частота (Гц)");
    this->Controls->Add(dataGridViewDevices);
}

// ==================== Загрузка формы ====================
void CalibrationForm::OnLoad(Object^ sender, EventArgs^ e) {
    RefreshPorts();
}

// ==================== Работа с портом ====================
void CalibrationForm::RefreshPorts() {
    comboBoxPorts->Items->Clear();
    portNames->Clear();

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD index = 0;
        char valueName[256]; DWORD valueNameSize;
        BYTE data[256]; DWORD dataSize; DWORD type;
        while (true) {
            valueNameSize = sizeof(valueName);
            dataSize = sizeof(data);
            if (RegEnumValueA(hKey, index, valueName, &valueNameSize, NULL, &type, data, &dataSize) != ERROR_SUCCESS)
                break;
            if (type == REG_SZ) {
                std::string portName = "\\\\.\\" + std::string((char*)data);
                portNames->Add(gcnew String(portName.c_str()));
                comboBoxPorts->Items->Add(gcnew String(((char*)data)));
            }
            index++;
        }
        RegCloseKey(hKey);
    }
    if (comboBoxPorts->Items->Count > 0)
        comboBoxPorts->SelectedIndex = 0;
}

void CalibrationForm::OpenPort() {
    if (comboBoxPorts->SelectedIndex < 0) {
        MessageBox::Show(L"Выберите порт!");
        return;
    }
    String^ portFull = portNames[comboBoxPorts->SelectedIndex];
    std::string portStd = msclr::interop::marshal_as<std::string>(portFull);
    hPort = CreateFileA(portStd.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPort == INVALID_HANDLE_VALUE) {
        MessageBox::Show(L"Не удалось открыть порт");
        return;
    }
    if (!ConfigurePort()) {
        CloseHandle(hPort); hPort = INVALID_HANDLE_VALUE;
        MessageBox::Show(L"Ошибка настройки порта");
        return;
    }
    PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR);
    buttonOpenPort->Enabled = false;
    buttonClosePort->Enabled = true;
    buttonScanIds->Enabled = true;
    comboBoxPorts->Enabled = false;
    Sleep(200);
    labelStatus->Text = L"Порт открыт. Поиск устройств...";
    ScanForDevices();
}

bool CalibrationForm::ConfigurePort() {
    DCB dcb = { 0 }; dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(hPort, &dcb)) return false;
    dcb.BaudRate = baudRate; dcb.ByteSize = byteSize;
    dcb.StopBits = stopBits; dcb.Parity = parity;
    dcb.fOutxCtsFlow = FALSE; dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    if (!SetCommState(hPort, &dcb)) return false;

    COMMTIMEOUTS to = { 0 };
    to.ReadIntervalTimeout = 50;
    to.ReadTotalTimeoutConstant = 500;
    to.ReadTotalTimeoutMultiplier = 1;
    to.WriteTotalTimeoutConstant = 100;
    to.WriteTotalTimeoutMultiplier = 1;
    return SetCommTimeouts(hPort, &to) != 0;
}

void CalibrationForm::ClosePort() {
    stopPolling = true;
    if (hPort != INVALID_HANDLE_VALUE) {
        CloseHandle(hPort);
        hPort = INVALID_HANDLE_VALUE;
    }
    buttonOpenPort->Enabled = true;
    buttonClosePort->Enabled = false;
    buttonScanIds->Enabled = false;
    comboBoxRefDevice->Enabled = false;
    comboBoxPorts->Enabled = true;
    liveTimer->Stop();
    foundIds->Clear();
    comboBoxRefDevice->Items->Clear();
    dataGridViewDevices->Rows->Clear();
    labelRefDensity->Text = L"Плотность эталона: — кг/м³";
    labelStatus->Text = L"Порт закрыт";
    progressBarScan->Visible = false;
    stopPolling = false;
}

// ==================== Modbus-функции ====================
uint16_t CalibrationForm::CRC16(array<Byte>^ data, int length) {
    unsigned char uchCRCHi = 0xFF;
    unsigned char uchCRCLo = 0xFF;
    for (int i = 0; i < length; i++) {
        int index = uchCRCLo ^ data[i];
        uchCRCLo = uchCRCHi ^ crcHi[index];
        uchCRCHi = crcLo[index];
    }
    return (uchCRCHi << 8) | uchCRCLo;
}

bool CalibrationForm::ReadRegisters(int slaveId, int startAddress, int numRegs, array<Byte>^% result) {
    COMMTIMEOUTS oldTimeouts;
    GetCommTimeouts(hPort, &oldTimeouts);
    COMMTIMEOUTS readTimeouts;
    readTimeouts.ReadIntervalTimeout = 200;
    readTimeouts.ReadTotalTimeoutConstant = 500;
    readTimeouts.ReadTotalTimeoutMultiplier = 1;
    readTimeouts.WriteTotalTimeoutConstant = 100;
    readTimeouts.WriteTotalTimeoutMultiplier = 1;
    SetCommTimeouts(hPort, &readTimeouts);

    array<Byte>^ request = gcnew array<Byte>(8);
    request[0] = (Byte)slaveId; request[1] = 0x03;
    request[2] = (Byte)((startAddress >> 8) & 0xFF); request[3] = (Byte)(startAddress & 0xFF);
    request[4] = (Byte)((numRegs >> 8) & 0xFF); request[5] = (Byte)(numRegs & 0xFF);
    uint16_t crc = CRC16(request, 6);
    request[6] = (Byte)(crc & 0xFF); request[7] = (Byte)((crc >> 8) & 0xFF);

    Monitor::Enter(portLock);
    try {
        PurgeComm(hPort, PURGE_RXCLEAR);
        DWORD written;
        pin_ptr<Byte> pReq = &request[0];
        if (!WriteFile(hPort, pReq, 8, &written, NULL) || written != 8) {
            SetCommTimeouts(hPort, &oldTimeouts);
            return false;
        }

        Sleep(10);

        array<Byte>^ response = gcnew array<Byte>(256);
        DWORD totalRead = 0;
        DWORD start = GetTickCount();

        while ((GetTickCount() - start) < 600) {
            pin_ptr<Byte> pResp = &response[0];
            unsigned char* pBuf = pResp + totalRead;
            DWORD toRead = response->Length - totalRead;
            DWORD br;
            if (ReadFile(hPort, pBuf, toRead, &br, NULL) && br > 0) {
                totalRead += br;
                if (totalRead >= 3 && response[0] == (Byte)slaveId) {
                    if (response[1] == 0x83) {
                        if (totalRead >= 5) {
                            SetCommTimeouts(hPort, &oldTimeouts);
                            return false;
                        }
                    }
                    else if (response[1] == 0x03 && totalRead >= 3) {
                        int byteCount = response[2];
                        int expectedLen = byteCount + 5;
                        if (totalRead >= expectedLen) {
                            uint16_t calcCrc = CRC16(response, expectedLen - 2);
                            uint16_t respCrc = ((uint16_t)response[expectedLen - 1] << 8) | response[expectedLen - 2];
                            SetCommTimeouts(hPort, &oldTimeouts);
                            if (calcCrc == respCrc) {
                                result = gcnew array<Byte>(expectedLen);
                                Array::Copy(response, result, expectedLen);
                                return true;
                            }
                            else {
                                PurgeComm(hPort, PURGE_RXCLEAR);
                                return false;
                            }
                        }
                    }
                }
            }
            else {
                Sleep(5);
            }
        }
        PurgeComm(hPort, PURGE_RXCLEAR);
        SetCommTimeouts(hPort, &oldTimeouts);
    }
    finally {
        SetCommTimeouts(hPort, &oldTimeouts);
        PurgeComm(hPort, PURGE_RXCLEAR);
        Monitor::Exit(portLock);
    }
    return false;
}

bool CalibrationForm::WriteMultipleRegisters(int slaveId, int startAddress, int numRegs, array<Byte>^ data) {
    int byteCount = numRegs * 2;
    array<Byte>^ request = gcnew array<Byte>(9 + byteCount);
    request[0] = (Byte)slaveId; request[1] = 0x10;
    request[2] = (Byte)((startAddress >> 8) & 0xFF); request[3] = (Byte)(startAddress & 0xFF);
    request[4] = (Byte)((numRegs >> 8) & 0xFF); request[5] = (Byte)(numRegs & 0xFF);
    request[6] = (Byte)byteCount;
    Array::Copy(data, 0, request, 7, byteCount);
    uint16_t crc = CRC16(request, 7 + byteCount);
    request[7 + byteCount] = (Byte)(crc & 0xFF);
    request[8 + byteCount] = (Byte)((crc >> 8) & 0xFF);

    Monitor::Enter(portLock);
    try {
        PurgeComm(hPort, PURGE_RXCLEAR);
        pin_ptr<Byte> pReq = &request[0];
        DWORD written;
        if (!WriteFile(hPort, pReq, request->Length, &written, NULL)) return false;
        array<Byte>^ resp = gcnew array<Byte>(8);
        DWORD totalRead = 0; DWORD start = GetTickCount();
        while (GetTickCount() - start < 250) {
            pin_ptr<Byte> pResp = &resp[0];
            unsigned char* pBuf = pResp + totalRead;
            DWORD toRead = resp->Length - totalRead;
            DWORD br;
            if (ReadFile(hPort, pBuf, toRead, &br, NULL) && br > 0) {
                totalRead += br;
                if (totalRead >= 8) {
                    PurgeComm(hPort, PURGE_RXCLEAR);
                    return (resp[0] == slaveId && resp[1] == 0x10);
                }
            }
            Sleep(1);
        }
        PurgeComm(hPort, PURGE_RXCLEAR);
    }
    finally { Monitor::Exit(portLock); }
    return false;
}

bool CalibrationForm::WriteSingleRegister(int slaveId, int regAddress, uint16_t value) {
    array<Byte>^ request = gcnew array<Byte>(8);
    request[0] = (Byte)slaveId; request[1] = 0x06;
    request[2] = (Byte)((regAddress >> 8) & 0xFF); request[3] = (Byte)(regAddress & 0xFF);
    request[4] = (Byte)((value >> 8) & 0xFF); request[5] = (Byte)(value & 0xFF);
    uint16_t crc = CRC16(request, 6);
    request[6] = (Byte)(crc & 0xFF); request[7] = (Byte)((crc >> 8) & 0xFF);

    Monitor::Enter(portLock);
    try {
        PurgeComm(hPort, PURGE_RXCLEAR);
        pin_ptr<Byte> pReq = &request[0];
        DWORD written;
        if (!WriteFile(hPort, pReq, 8, &written, NULL)) return false;
        array<Byte>^ resp = gcnew array<Byte>(8);
        DWORD totalRead = 0; DWORD start = GetTickCount();
        while (GetTickCount() - start < 200) {
            pin_ptr<Byte> pResp = &resp[0];
            unsigned char* pBuf = pResp + totalRead;
            DWORD toRead = resp->Length - totalRead;
            DWORD br;
            if (ReadFile(hPort, pBuf, toRead, &br, NULL) && br > 0) {
                totalRead += br;
                if (totalRead >= 8) {
                    PurgeComm(hPort, PURGE_RXCLEAR);
                    return (resp[0] == slaveId && resp[1] == 0x06);
                }
            }
            Sleep(1);
        }
        PurgeComm(hPort, PURGE_RXCLEAR);
    }
    finally { Monitor::Exit(portLock); }
    return false;
}

bool CalibrationForm::WriteFloat(int slaveId, int regAddress, float value) {
    array<Byte>^ floatBytes = BitConverter::GetBytes(value);
    UInt32 raw = BitConverter::ToUInt32(floatBytes, 0);
    array<Byte>^ data = gcnew array<Byte>{
        (Byte)(raw & 0xFF), (Byte)((raw >> 8) & 0xFF),
            (Byte)((raw >> 16) & 0xFF), (Byte)((raw >> 24) & 0xFF)
    };
    return WriteMultipleRegisters(slaveId, regAddress, 2, data);
}

float CalibrationForm::ExtractFloat(array<Byte>^ response) {
    UInt32 raw = ((UInt32)response[3]) |
        ((UInt32)response[4] << 8) |
        ((UInt32)response[5] << 16) |
        ((UInt32)response[6] << 24);
    return BitConverter::ToSingle(BitConverter::GetBytes(raw), 0);
}

int CalibrationForm::ExtractInt32(array<Byte>^ response) {
    UInt32 raw = ((UInt32)response[3]) |
        ((UInt32)response[4] << 8) |
        ((UInt32)response[5] << 16) |
        ((UInt32)response[6] << 24);
    return static_cast<int>(raw);
}

float CalibrationForm::ReadDensity(int slaveId) {
    array<Byte>^ resp;
    if (ReadRegisters(slaveId, REG_DENSITY, 2, resp) && resp->Length >= 9)
        return ExtractFloat(resp);
    return -1.0f;
}

float CalibrationForm::ReadTemperature(int slaveId) {
    array<Byte>^ resp;
    if (ReadRegisters(slaveId, REG_TEMPERATURE, 2, resp) && resp->Length >= 9)
        return ExtractFloat(resp);
    return -300.0f;
}

float CalibrationForm::ReadFrequency(int slaveId) {
    array<Byte>^ resp;
    if (ReadRegisters(slaveId, REG_FREQ_A, 2, resp) && resp->Length >= 9)
        return ExtractFloat(resp);
    return -1.0f;
}

// ==================== Математика ====================
float CalibrationForm::Polinom3(array<float>^ coeff, float x) {
    float res = coeff[0], xn = x;
    for (int i = 1; i < coeff->Length; i++) { res += coeff[i] * xn; xn *= x; }
    return res;
}

float CalibrationForm::GetReferenceDensity(String^ medium, float temperature) {
    float rho20, beta;
    if (medium == L"Воздух") { rho20 = 1.205f; beta = 0.00367f; }
    else if (medium == L"Дистиллированная вода") { rho20 = 998.2f; beta = 0.0002f; }
    else if (medium == L"Нефрас") { rho20 = 720.0f; beta = 0.0010f; }
    else if (medium == L"Дизельное топливо") { rho20 = 830.0f; beta = 0.0008f; }
    else if (medium == L"Индустриальное масло И20") { rho20 = 890.0f; beta = 0.0007f; }
    else return 0.0f;
    float deltaT = temperature - 20.0f;
    return rho20 / (1.0f + beta * deltaT);
}

// ==================== Обновление плотности эталона ====================
void CalibrationForm::UpdateRefDensity() {
    if (hPort == INVALID_HANDLE_VALUE || comboBoxRefDevice->SelectedIndex <= 0) {
        labelRefDensity->Text = L"Плотность эталона: — кг/м³";
        return;
    }
    int refId = foundIds[comboBoxRefDevice->SelectedIndex - 1];
    float dens = ReadDensity(refId);
    if (dens >= 0)
        labelRefDensity->Text = String::Format(L"Плотность эталона: {0:F3} кг/м³", dens);
    else
        labelRefDensity->Text = L"Плотность эталона: ошибка";
}

// ==================== Сканирование ID ====================
void CalibrationForm::ScanForDevices() {
    if (scanning) return;
    scanning = true;
    try {
        if (hPort == INVALID_HANDLE_VALUE) return;

        COMMTIMEOUTS oldTimeouts;
        GetCommTimeouts(hPort, &oldTimeouts);
        COMMTIMEOUTS scanTimeouts;
        scanTimeouts.ReadIntervalTimeout = 50;
        scanTimeouts.ReadTotalTimeoutConstant = 100;
        scanTimeouts.ReadTotalTimeoutMultiplier = 0;
        scanTimeouts.WriteTotalTimeoutConstant = 100;
        scanTimeouts.WriteTotalTimeoutMultiplier = 0;
        SetCommTimeouts(hPort, &scanTimeouts);

        foundIds->Clear();
        comboBoxRefDevice->Items->Clear();
        dataGridViewDevices->Rows->Clear();
        comboBoxRefDevice->Enabled = false;
        labelStatus->Text = L"Сканирование...";
        progressBarScan->Visible = true;
        progressBarScan->Value = 0;
        Application::DoEvents();

        for (int slaveId = 1; slaveId <= 50; slaveId++) {
            array<Byte>^ request = gcnew array<Byte>(8);
            request[0] = (Byte)slaveId;
            request[1] = 0x03;
            request[2] = 0x00; request[3] = 0x00;
            request[4] = 0x00; request[5] = 0x02;
            uint16_t crc = CRC16(request, 6);
            request[6] = (Byte)(crc & 0xFF);
            request[7] = (Byte)((crc >> 8) & 0xFF);

            Monitor::Enter(portLock);
            try {
                PurgeComm(hPort, PURGE_RXCLEAR);
                DWORD written;
                pin_ptr<Byte> pReq = &request[0];
                if (!WriteFile(hPort, pReq, 8, &written, NULL) || written != 8)
                    continue;

                Sleep(10);

                array<Byte>^ response = gcnew array<Byte>(256);
                DWORD totalRead = 0;
                DWORD startTick = GetTickCount();
                bool frameOK = false;

                while ((GetTickCount() - startTick) < 200) {
                    pin_ptr<Byte> pResp = &response[0];
                    unsigned char* pBuf = pResp + totalRead;
                    DWORD toRead = response->Length - totalRead;
                    DWORD br = 0;
                    if (ReadFile(hPort, pBuf, toRead, &br, NULL) && br > 0) {
                        totalRead += br;
                        if (totalRead >= 5 && response[0] == (Byte)slaveId) {
                            uint16_t calcCrc = CRC16(response, totalRead - 2);
                            uint16_t respCrc = ((uint16_t)response[totalRead - 1] << 8) |
                                response[totalRead - 2];
                            if (calcCrc == respCrc) {
                                frameOK = true;
                                break;
                            }
                        }
                    }
                    else {
                        Sleep(10);
                    }
                }

                if (frameOK) {
                    foundIds->Add(slaveId);
                    comboBoxRefDevice->Items->Add(slaveId.ToString());
                    labelStatus->Text = String::Format(L"Найден ID {0}", slaveId);
                }
            }
            finally { Monitor::Exit(portLock); }

            Sleep(50);
            progressBarScan->Value = slaveId;
            Application::DoEvents();
        }

        SetCommTimeouts(hPort, &oldTimeouts);
        PurgeComm(hPort, PURGE_RXCLEAR);
        progressBarScan->Visible = false;

        if (foundIds->Count > 0) {
            comboBoxRefDevice->Items->Insert(0, L"(не выбран)");
            comboBoxRefDevice->SelectedIndex = 0;
            comboBoxRefDevice->Enabled = true;
            labelStatus->Text = String::Format(L"Найдено устройств: {0}", foundIds->Count);

            dataGridViewDevices->Rows->Clear();
            for each (int id in foundIds) {
                dataGridViewDevices->Rows->Add(id.ToString(), L"—", L"—", L"—");
            }

            liveTimer->Start();
            UpdateDeviceTable();
        }
        else {
            labelStatus->Text = L"Устройства не найдены";
        }
    }
    finally {
        scanning = false;
    }
}

// ==================== Резервное копирование и откат ====================
void CalibrationForm::BackupCalibrationData(int slaveId) {
    array<Byte>^ resp;
    backupRegisters->Clear();
    lastSlaveId = slaveId;

    if (ReadRegisters(slaveId, REG_FREQ0, 2, resp) && resp->Length >= 9)
        backupRegisters[REG_FREQ0] = ExtractFloat(resp);
    if (ReadRegisters(slaveId, REG_K, 2, resp) && resp->Length >= 9)
        backupRegisters[REG_K] = ExtractFloat(resp);
    if (ReadRegisters(slaveId, REG_FREQ_AIR, 2, resp) && resp->Length >= 9)
        backupRegisters[REG_FREQ_AIR] = ExtractFloat(resp);
    if (ReadRegisters(slaveId, REG_FREQ_WATER, 2, resp) && resp->Length >= 9)
        backupRegisters[REG_FREQ_WATER] = ExtractFloat(resp);
    if (ReadRegisters(slaveId, REG_KQ0T_BASE, 2, resp) && resp->Length >= 9)
        backupRegisters[REG_KQ0T_BASE] = ExtractFloat(resp);
    if (ReadRegisters(slaveId, REG_KQDT_BASE, 2, resp) && resp->Length >= 9)
        backupRegisters[REG_KQDT_BASE] = ExtractFloat(resp);
    if (ReadRegisters(slaveId, REG_DEMP_DENS, 2, resp) && resp->Length >= 9)
        backupRegisters[REG_DEMP_DENS] = ExtractFloat(resp);

    buttonRollback->Enabled = (backupRegisters->Count > 0);
}

void CalibrationForm::RollbackCalibration() {
    if (calibrating) return;
    calibrating = true;
    try {
        if (hPort == INVALID_HANDLE_VALUE || lastSlaveId < 0) {
            MessageBox::Show(L"Нет датчика для отката");
            return;
        }
        if (backupRegisters->Count == 0) {
            MessageBox::Show(L"Нет сохранённых данных для отката");
            return;
        }

        liveTimer->Stop();
        for each (auto kvp in backupRegisters) {
            if (!WriteFloat(lastSlaveId, kvp.Key, kvp.Value))
                throw gcnew Exception(String::Format("Не удалось записать регистр {0}", kvp.Key));
            Sleep(20);
        }

        labelStatus->Text = L"Откат выполнен успешно";
        MessageBox::Show(L"Параметры восстановлены", L"Откат");

        backupRegisters->Clear();
        buttonRollback->Enabled = false;
    }
    catch (Exception^ ex) {
        labelStatus->Text = L"Ошибка отката: " + ex->Message;
        MessageBox::Show(ex->Message, L"Ошибка");
    }
    finally {
        liveTimer->Start();
        calibrating = false;
    }
}

// ==================== Калибровка ====================
float CalibrationForm::GetTargetDensity() {
    if (comboBoxRefDevice->SelectedIndex > 0) {
        int idx = comboBoxRefDevice->SelectedIndex - 1;
        if (idx < 0 || idx >= foundIds->Count) {
            labelStatus->Text = L"Некорректный индекс эталона";
            return -1.0f;
        }
        int refId = foundIds[idx];
        float refDens = ReadDensity(refId);
        if (refDens >= 0) {
            labelStatus->Text = String::Format(L"Цель: {0:F3} кг/м³ (с эталона ID {1})", refDens, refId);
            return refDens;
        }
        else {
            labelStatus->Text = L"Не удалось прочитать плотность эталона, используется ручной ввод";
        }
    }
    float manualTarget;
    if (Single::TryParse(textBoxTargetDensity->Text,
        System::Globalization::NumberStyles::Any,
        System::Globalization::CultureInfo::InvariantCulture,
        manualTarget) && manualTarget > 0) {
        labelStatus->Text = String::Format(L"Цель: {0:F3} кг/м³ (вручную)", manualTarget);
        return manualTarget;
    }
    throw gcnew Exception("Не задана целевая плотность (введите число или выберите эталон)");
}

void CalibrationForm::CalibrateSingle(int slaveId) {
    BackupCalibrationData(slaveId);

    float targetDensity = GetTargetDensity();
    float density0 = ReadDensity(slaveId);
    if (density0 < 0) throw gcnew Exception("Ошибка чтения плотности");

    if (Math::Abs(density0 - targetDensity) < 0.005f) {
        return;
    }

    array<Byte>^ resp;
    float freqAir = 0.0f, freqWater = 0.0f;
    if (!ReadRegisters(slaveId, REG_FREQ_AIR, 2, resp) || resp->Length < 9)
        throw gcnew Exception("Не удалось прочитать FreqAir");
    freqAir = ExtractFloat(resp);

    if (!ReadRegisters(slaveId, REG_FREQ_WATER, 2, resp) || resp->Length < 9)
        throw gcnew Exception("Не удалось прочитать FreqWater");
    freqWater = ExtractFloat(resp);

    bool useAirForTest = (density0 < 10.0f);
    int testReg = useAirForTest ? REG_FREQ_AIR : REG_FREQ_WATER;
    float testFreq0 = useAirForTest ? freqAir : freqWater;
    float testStep = useAirForTest ? 0.2f : 0.5f;

    float testFreq1 = testFreq0 + testStep;
    if (!WriteFloat(slaveId, testReg, testFreq1))
        throw gcnew Exception("Ошибка записи тестового значения");
    Sleep(500);

    float density1 = ReadDensity(slaveId);
    if (density1 < 0) {
        WriteFloat(slaveId, testReg, testFreq0);
        throw gcnew Exception("Ошибка чтения плотности после шага");
    }

    float derivative = (density1 - density0) / testStep;
    WriteFloat(slaveId, testReg, testFreq0);

    if (Math::Abs(derivative) < 0.001f) {
        throw gcnew Exception("Не удалось оценить чувствительность (производная ≈ 0)");
    }

    float deltaF = (targetDensity - density0) / derivative;
    float newFreqAir = freqAir + deltaF;
    float newFreqWater = freqWater + deltaF;

    if (newFreqAir < freqAir * 0.5f || newFreqAir > freqAir * 1.5f ||
        newFreqWater < freqWater * 0.5f || newFreqWater > freqWater * 1.5f) {
        String^ msg = String::Format(L"Новые частоты: FreqAir={0:F2} (было {1:F2}), FreqWater={2:F2} (было {3:F2}).\nПрименить?",
            newFreqAir, freqAir, newFreqWater, freqWater);
        if (MessageBox::Show(msg, L"Предупреждение", MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::No)
            return;
    }

    if (!WriteFloat(slaveId, REG_FREQ_AIR, newFreqAir))
        throw gcnew Exception("Не удалось записать новый FreqAir");
    if (!WriteFloat(slaveId, REG_FREQ_WATER, newFreqWater))
        throw gcnew Exception("Не удалось записать новый FreqWater");

    Sleep(500);
    float finalDensity = ReadDensity(slaveId);
}

void CalibrationForm::CalibrateAll() {
    if (calibrating) return;
    calibrating = true;
    try {
        if (hPort == INVALID_HANDLE_VALUE || foundIds->Count == 0) return;

        float targetDensity;
        try {
            targetDensity = GetTargetDensity();
        }
        catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
            return;
        }

        int refId = -1;
        if (comboBoxRefDevice->SelectedIndex > 0)
            refId = foundIds[comboBoxRefDevice->SelectedIndex - 1];

        liveTimer->Stop();
        int total = 0, success = 0;
        String^ log = L"";

        for each (int slaveId in foundIds) {
            if (slaveId == refId) continue;

            total++;
            labelStatus->Text = String::Format(L"Калибровка датчика ID {0} ({1}/{2})...", slaveId, total, foundIds->Count - (refId >= 0 ? 1 : 0));
            Application::DoEvents();

            try {
                CalibrateSingle(slaveId);
                success++;
                log += String::Format(L"ID {0}: успех\r\n", slaveId);
            }
            catch (Exception^ ex) {
                log += String::Format(L"ID {0}: ошибка ({1})\r\n", slaveId, ex->Message);
            }

            Sleep(200);
        }

        labelStatus->Text = String::Format(L"Готово. Успешно: {0}/{1}", success, total);
        MessageBox::Show(log, L"Результаты калибровки всех датчиков");
        liveTimer->Start();
        UpdateDeviceTable();
    }
    finally {
        calibrating = false;
    }
}

// ==================== Таблица датчиков ====================
void CalibrationForm::UpdateDeviceTable() {
    if (stopPolling || hPort == INVALID_HANDLE_VALUE || foundIds->Count == 0) return;

    if (dataGridViewDevices->Rows->Count != foundIds->Count) {
        dataGridViewDevices->Rows->Clear();
        for each (int id in foundIds) {
            dataGridViewDevices->Rows->Add(id.ToString(), L"—", L"—", L"—");
        }
    }

    labelStatus->Text = L"Обновление таблицы...";
    Application::DoEvents();

    for (int i = 0; i < foundIds->Count && !stopPolling; i++) {
        int slaveId = foundIds[i];
        float temp = ReadTemperature(slaveId);
        float dens = ReadDensity(slaveId);
        float freq = ReadFrequency(slaveId);

        if (stopPolling) break;

        DataGridViewRow^ row = dataGridViewDevices->Rows[i];
        row->Cells[1]->Value = (temp > -100.0f) ? String::Format(L"{0:F2}", temp) : L"Ошибка";
        row->Cells[2]->Value = (dens >= 0) ? String::Format(L"{0:F3}", dens) : L"Ошибка";
        row->Cells[3]->Value = (freq >= 0) ? String::Format(L"{0:F1}", freq) : L"Ошибка";

        if (i % 5 == 0) Application::DoEvents();
    }

    labelStatus->Text = String::Format(L"Обновлено устройств: {0}", foundIds->Count);
}

// ==================== Обработчики кнопок ====================
void CalibrationForm::buttonOpenPort_Click(Object^ sender, EventArgs^ e) { OpenPort(); }

void CalibrationForm::buttonScanIds_Click(Object^ sender, EventArgs^ e) {
    if (hPort == INVALID_HANDLE_VALUE) return;
    liveTimer->Stop();
    ScanForDevices();
}

void CalibrationForm::buttonCalibrateAll_Click(Object^ sender, EventArgs^ e) { CalibrateAll(); }

void CalibrationForm::buttonClosePort_Click(Object^ sender, EventArgs^ e) { ClosePort(); }

void CalibrationForm::comboBoxRefDevice_SelectedIndexChanged(Object^ sender, EventArgs^ e) { UpdateRefDensity(); }

void CalibrationForm::buttonRollback_Click(Object^ sender, EventArgs^ e) { RollbackCalibration(); }

void CalibrationForm::liveTimer_Tick(Object^ sender, EventArgs^ e) {
    try {
        UpdateDeviceTable();
        UpdateRefDensity();
    }
    catch (Exception^ ex) {
        labelStatus->Text = L"Ошибка обновления: " + ex->Message;
    }
}

// ==================== Точка входа ====================
[STAThreadAttribute]
int main(array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew CalibrationForm());
    return 0;
}