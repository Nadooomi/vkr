#include "VKR.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Xml;
using namespace System::Drawing;
using namespace System::Collections::Generic;
using namespace System::Threading;


// Таблицы CRC16
static unsigned char auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
};

static unsigned char auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};

// ============================================================================
// Конструктор и деструктор
// ============================================================================

ModbusMasterForm::ModbusMasterForm(void)
{
    InitializeComponent();
    InitializeCustomComponent();
    InitializeRegistersGrid();
    InitializeParametersGrid();
    InitializeLiveValuesPanel();
    allRegistersData = gcnew Dictionary<int, Tuple<float, int32_t>^>();
    allRegistersRaw = gcnew Dictionary<int, System::UInt32>();
    paramNames = gcnew List<String^>();
    paramAddresses = gcnew List<int>();
    paramUnits = gcnew List<String^>();
    changedRegistersValues = gcnew Dictionary<int, uint32_t>();
    changedRegistersFloats = gcnew Dictionary<int, float>();

    hSerial = INVALID_HANDLE_VALUE;
    baudRate = CBR_9600;
    byteSize = 8;
    stopBits = ONESTOPBIT;
    parity = NOPARITY;
    isAutoUpdating = false;
    isRegistersPanelExpanded = false;
    isUpdatingTables = false;
    isPollingInProgress = false;
    isRegisterCellEditing = false;
    resumeAutoUpdateAfterEdit = false;
    pendingUiRefreshAfterEdit = false;
    pollState = PollState::Idle;
    pollStartAddress = 0;
    pollCurrentBatchSize = 0;
    pollBatchStartTick = 0;
    pollTotalBytesRead = 0;
    pollResponseBuffer = gcnew array<System::Byte>(1024);
    portLock = gcnew System::Object();

    InitializeParameters();
    InitializeTimer();

    originalFormHeight = this->Height;
    originalRegistersGroupHeight = groupBoxRegisters->Height;
    isRegistersPanelExpanded = true;
}

ModbusMasterForm::~ModbusMasterForm()
{
    if (components) delete components;
    if (hSerial != INVALID_HANDLE_VALUE) ClosePort();

    if (updateTimer != nullptr)
    {
        updateTimer->Stop();
        delete updateTimer;
    }

    if (pollTimer != nullptr)
    {
        pollTimer->Stop();
        delete pollTimer;
    }
}

// ============================================================================
// Инициализация компонентов формы
// ============================================================================

void ModbusMasterForm::InitializeComponent()
{
    this->comboBoxPorts = (gcnew System::Windows::Forms::ComboBox());
    this->buttonRefresh = (gcnew System::Windows::Forms::Button());
    this->buttonConnect = (gcnew System::Windows::Forms::Button());
    this->buttonDisconnect = (gcnew System::Windows::Forms::Button());
    this->buttonScanDevices = (gcnew System::Windows::Forms::Button());
    this->textBoxLog = (gcnew System::Windows::Forms::TextBox());
    this->labelStatus = (gcnew System::Windows::Forms::Label());
    this->groupBoxPort = (gcnew System::Windows::Forms::GroupBox());
    this->labelSlaveID = (gcnew System::Windows::Forms::Label());
    this->numericSlaveID = (gcnew System::Windows::Forms::NumericUpDown());
    this->labelAutoUpdateStatus = (gcnew System::Windows::Forms::Label());
    this->checkBoxShowAll = (gcnew System::Windows::Forms::CheckBox());
    this->buttonStopAutoUpdate = (gcnew System::Windows::Forms::Button());
    this->buttonStartAutoUpdate = (gcnew System::Windows::Forms::Button());
    this->buttonRefreshRegisters = (gcnew System::Windows::Forms::Button());
    this->textBoxScanResult = (gcnew System::Windows::Forms::TextBox());
    this->labelFoundDevices = (gcnew System::Windows::Forms::Label());
    this->labelModelCaption = (gcnew System::Windows::Forms::Label());
    this->labelModelValue = (gcnew System::Windows::Forms::Label());
    this->labelSerialCaption = (gcnew System::Windows::Forms::Label());
    this->labelSerialValue = (gcnew System::Windows::Forms::Label());
    this->labelTypeCaption = (gcnew System::Windows::Forms::Label());
    this->labelTypeValue = (gcnew System::Windows::Forms::Label());
    this->labelSpeedCaption = (gcnew System::Windows::Forms::Label());
    this->labelSpeedValue = (gcnew System::Windows::Forms::Label());
    this->buttonClearLog = (gcnew System::Windows::Forms::Button());
    this->dataGridViewRegisters = (gcnew System::Windows::Forms::DataGridView());
    this->dataGridViewParameters = (gcnew System::Windows::Forms::DataGridView());
    this->buttonExport = (gcnew System::Windows::Forms::Button());
    this->buttonSendChanges = (gcnew System::Windows::Forms::Button());
    this->groupBoxRegisters = (gcnew System::Windows::Forms::GroupBox());
    this->groupBoxParameters = (gcnew System::Windows::Forms::GroupBox());
    this->saveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
    this->groupBoxPort->SuspendLayout();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericSlaveID))->BeginInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewRegisters))->BeginInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewParameters))->BeginInit();
    this->groupBoxRegisters->SuspendLayout();
    this->groupBoxParameters->SuspendLayout();
    this->SuspendLayout();
    // 
    // comboBoxPorts
    // 
    this->comboBoxPorts->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
    this->comboBoxPorts->FormattingEnabled = true;
    this->comboBoxPorts->Location = System::Drawing::Point(15, 30);
    this->comboBoxPorts->Name = L"comboBoxPorts";
    this->comboBoxPorts->Size = System::Drawing::Size(150, 28);
    this->comboBoxPorts->TabIndex = 0;
    // 
    // buttonRefresh
    // 
    this->buttonRefresh->Location = System::Drawing::Point(175, 30);
    this->buttonRefresh->Name = L"buttonRefresh";
    this->buttonRefresh->Size = System::Drawing::Size(100, 30);
    this->buttonRefresh->TabIndex = 1;
    this->buttonRefresh->Text = L"Обновить порты";
    this->buttonRefresh->UseVisualStyleBackColor = true;
    this->buttonRefresh->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonRefresh_Click);
    // 
    // buttonConnect
    // 
    this->buttonConnect->Location = System::Drawing::Point(285, 30);
    this->buttonConnect->Name = L"buttonConnect";
    this->buttonConnect->Size = System::Drawing::Size(100, 30);
    this->buttonConnect->TabIndex = 2;
    this->buttonConnect->Text = L"Подключить";
    this->buttonConnect->UseVisualStyleBackColor = true;
    this->buttonConnect->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonConnect_Click);
    // 
    // buttonDisconnect
    // 
    this->buttonDisconnect->Enabled = false;
    this->buttonDisconnect->Location = System::Drawing::Point(395, 30);
    this->buttonDisconnect->Name = L"buttonDisconnect";
    this->buttonDisconnect->Size = System::Drawing::Size(100, 30);
    this->buttonDisconnect->TabIndex = 3;
    this->buttonDisconnect->Text = L"Отключить";
    this->buttonDisconnect->UseVisualStyleBackColor = true;
    this->buttonDisconnect->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonDisconnect_Click);
    // 
    // buttonScanDevices
    // 
    this->buttonScanDevices->Location = System::Drawing::Point(523, 30);
    this->buttonScanDevices->Name = L"buttonScanDevices";
    this->buttonScanDevices->Size = System::Drawing::Size(150, 30);
    this->buttonScanDevices->TabIndex = 4;
    this->buttonScanDevices->Text = L"Поиск устройств";
    this->buttonScanDevices->UseVisualStyleBackColor = true;
    this->buttonScanDevices->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonScanDevices_Click);
    // 
    // textBoxLog
    // 
    this->textBoxLog->Location = System::Drawing::Point(340, 533);
    this->textBoxLog->Multiline = true;
    this->textBoxLog->Name = L"textBoxLog";
    this->textBoxLog->ReadOnly = true;
    this->textBoxLog->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
    this->textBoxLog->Size = System::Drawing::Size(285, 140);
    this->textBoxLog->TabIndex = 5;
    // 
    // labelStatus
    // 
    this->labelStatus->AutoSize = true;
    this->labelStatus->Location = System::Drawing::Point(12, 695);
    this->labelStatus->Name = L"labelStatus";
    this->labelStatus->Size = System::Drawing::Size(148, 20);
    this->labelStatus->TabIndex = 6;
    this->labelStatus->Text = L"Статус: Отключен";
    // 
    // groupBoxPort
    // 
    this->groupBoxPort->Controls->Add(this->labelSlaveID);
    this->groupBoxPort->Controls->Add(this->numericSlaveID);
    this->groupBoxPort->Controls->Add(this->labelAutoUpdateStatus);
    this->groupBoxPort->Controls->Add(this->checkBoxShowAll);
    this->groupBoxPort->Controls->Add(this->buttonStopAutoUpdate);
    this->groupBoxPort->Controls->Add(this->comboBoxPorts);
    this->groupBoxPort->Controls->Add(this->buttonStartAutoUpdate);
    this->groupBoxPort->Controls->Add(this->buttonScanDevices);
    this->groupBoxPort->Controls->Add(this->labelModelCaption);
    this->groupBoxPort->Controls->Add(this->labelSpeedValue);
    this->groupBoxPort->Controls->Add(this->buttonRefreshRegisters);
    this->groupBoxPort->Controls->Add(this->labelSpeedCaption);
    this->groupBoxPort->Controls->Add(this->buttonRefresh);
    this->groupBoxPort->Controls->Add(this->labelTypeValue);
    this->groupBoxPort->Controls->Add(this->buttonDisconnect);
    this->groupBoxPort->Controls->Add(this->labelTypeCaption);
    this->groupBoxPort->Controls->Add(this->buttonConnect);
    this->groupBoxPort->Controls->Add(this->labelSerialValue);
    this->groupBoxPort->Controls->Add(this->textBoxScanResult);
    this->groupBoxPort->Controls->Add(this->labelSerialCaption);
    this->groupBoxPort->Controls->Add(this->labelFoundDevices);
    this->groupBoxPort->Controls->Add(this->labelModelValue);
    this->groupBoxPort->Location = System::Drawing::Point(12, 12);
    this->groupBoxPort->Name = L"groupBoxPort";
    this->groupBoxPort->Size = System::Drawing::Size(1209, 131);
    this->groupBoxPort->TabIndex = 7;
    this->groupBoxPort->TabStop = false;
    this->groupBoxPort->Text = L"Управление портом (RS-485, 9600 бод)";
    // 
    // labelSlaveID
    // 
    this->labelSlaveID->AutoSize = true;
    this->labelSlaveID->Location = System::Drawing::Point(885, 33);
    this->labelSlaveID->Name = L"labelSlaveID";
    this->labelSlaveID->Size = System::Drawing::Size(120, 20);
    this->labelSlaveID->TabIndex = 12;
    this->labelSlaveID->Text = L"ID устройства:";
    this->labelSlaveID->Click += gcnew System::EventHandler(this, &ModbusMasterForm::labelSlaveID_Click);
    // 
    // numericSlaveID
    // 
    this->numericSlaveID->Location = System::Drawing::Point(909, 91);
    this->numericSlaveID->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 247, 0, 0, 0 });
    this->numericSlaveID->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
    this->numericSlaveID->Name = L"numericSlaveID";
    this->numericSlaveID->Size = System::Drawing::Size(60, 26);
    this->numericSlaveID->TabIndex = 11;
    this->numericSlaveID->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
    // 
    // labelAutoUpdateStatus
    // 
    this->labelAutoUpdateStatus->AutoSize = true;
    this->labelAutoUpdateStatus->Location = System::Drawing::Point(11, 97);
    this->labelAutoUpdateStatus->Name = L"labelAutoUpdateStatus";
    this->labelAutoUpdateStatus->Size = System::Drawing::Size(232, 20);
    this->labelAutoUpdateStatus->TabIndex = 5;
    this->labelAutoUpdateStatus->Text = L"Автообновление: выключено";
    // 
    // checkBoxShowAll
    // 
    this->checkBoxShowAll->AutoSize = true;
    this->checkBoxShowAll->Location = System::Drawing::Point(15, 65);
    this->checkBoxShowAll->Name = L"checkBoxShowAll";
    this->checkBoxShowAll->Size = System::Drawing::Size(240, 24);
    this->checkBoxShowAll->TabIndex = 5;
    this->checkBoxShowAll->Text = L"Показывать занятые порты";
    this->checkBoxShowAll->UseVisualStyleBackColor = true;
    // 
    // buttonStopAutoUpdate
    // 
    this->buttonStopAutoUpdate->Enabled = false;
    this->buttonStopAutoUpdate->Location = System::Drawing::Point(493, 92);
    this->buttonStopAutoUpdate->Name = L"buttonStopAutoUpdate";
    this->buttonStopAutoUpdate->Size = System::Drawing::Size(120, 35);
    this->buttonStopAutoUpdate->TabIndex = 4;
    this->buttonStopAutoUpdate->Text = L"Автообновление ВЫКЛ";
    this->buttonStopAutoUpdate->UseVisualStyleBackColor = true;
    this->buttonStopAutoUpdate->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonStopAutoUpdate_Click);
    // 
    // buttonStartAutoUpdate
    // 
    this->buttonStartAutoUpdate->Location = System::Drawing::Point(367, 92);
    this->buttonStartAutoUpdate->Name = L"buttonStartAutoUpdate";
    this->buttonStartAutoUpdate->Size = System::Drawing::Size(120, 35);
    this->buttonStartAutoUpdate->TabIndex = 3;
    this->buttonStartAutoUpdate->Text = L"Автообновление ВКЛ";
    this->buttonStartAutoUpdate->UseVisualStyleBackColor = true;
    this->buttonStartAutoUpdate->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonStartAutoUpdate_Click);
    // 
    // buttonRefreshRegisters
    // 
    this->buttonRefreshRegisters->Location = System::Drawing::Point(261, 92);
    this->buttonRefreshRegisters->Name = L"buttonRefreshRegisters";
    this->buttonRefreshRegisters->Size = System::Drawing::Size(100, 35);
    this->buttonRefreshRegisters->TabIndex = 2;
    this->buttonRefreshRegisters->Text = L"Обновить";
    this->buttonRefreshRegisters->UseVisualStyleBackColor = true;
    this->buttonRefreshRegisters->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonRefreshRegisters_Click);
    // 
    // textBoxScanResult
    // 
    this->textBoxScanResult->Location = System::Drawing::Point(676, 69);
    this->textBoxScanResult->Multiline = true;
    this->textBoxScanResult->Name = L"textBoxScanResult";
    this->textBoxScanResult->Size = System::Drawing::Size(191, 60);
    this->textBoxScanResult->TabIndex = 9;
    this->textBoxScanResult->Visible = false;
    // 
    // labelFoundDevices
    // 
    this->labelFoundDevices->AutoSize = true;
    this->labelFoundDevices->Location = System::Drawing::Point(676, 30);
    this->labelFoundDevices->Name = L"labelFoundDevices";
    this->labelFoundDevices->Size = System::Drawing::Size(191, 20);
    this->labelFoundDevices->TabIndex = 8;
    this->labelFoundDevices->Text = L"Найденные устройства:";
    this->labelFoundDevices->Visible = false;
    // 
    // labelModelCaption
    // 
    this->labelModelCaption->AutoSize = true;
    this->labelModelCaption->Location = System::Drawing::Point(1011, 30);
    this->labelModelCaption->Name = L"labelModelCaption";
    this->labelModelCaption->Size = System::Drawing::Size(74, 20);
    this->labelModelCaption->TabIndex = 13;
    this->labelModelCaption->Text = L"Модель:";
    // 
    // labelModelValue
    // 
    this->labelModelValue->AutoSize = true;
    this->labelModelValue->Location = System::Drawing::Point(1141, 30);
    this->labelModelValue->Name = L"labelModelValue";
    this->labelModelValue->Size = System::Drawing::Size(14, 20);
    this->labelModelValue->TabIndex = 14;
    this->labelModelValue->Text = L"-";
    // 
    // labelSerialCaption
    // 
    this->labelSerialCaption->AutoSize = true;
    this->labelSerialCaption->Location = System::Drawing::Point(1011, 55);
    this->labelSerialCaption->Name = L"labelSerialCaption";
    this->labelSerialCaption->Size = System::Drawing::Size(140, 20);
    this->labelSerialCaption->TabIndex = 15;
    this->labelSerialCaption->Text = L"Серийный номер:";
    // 
    // labelSerialValue
    // 
    this->labelSerialValue->AutoSize = true;
    this->labelSerialValue->Location = System::Drawing::Point(1156, 55);
    this->labelSerialValue->Name = L"labelSerialValue";
    this->labelSerialValue->Size = System::Drawing::Size(14, 20);
    this->labelSerialValue->TabIndex = 16;
    this->labelSerialValue->Text = L"-";
    // 
    // labelTypeCaption
    // 
    this->labelTypeCaption->AutoSize = true;
    this->labelTypeCaption->Location = System::Drawing::Point(1011, 80);
    this->labelTypeCaption->Name = L"labelTypeCaption";
    this->labelTypeCaption->Size = System::Drawing::Size(40, 20);
    this->labelTypeCaption->TabIndex = 17;
    this->labelTypeCaption->Text = L"Тип:";
    // 
    // labelTypeValue
    // 
    this->labelTypeValue->AutoSize = true;
    this->labelTypeValue->Location = System::Drawing::Point(1141, 80);
    this->labelTypeValue->Name = L"labelTypeValue";
    this->labelTypeValue->Size = System::Drawing::Size(14, 20);
    this->labelTypeValue->TabIndex = 18;
    this->labelTypeValue->Text = L"-";
    // 
    // labelSpeedCaption
    // 
    this->labelSpeedCaption->AutoSize = true;
    this->labelSpeedCaption->Location = System::Drawing::Point(1011, 105);
    this->labelSpeedCaption->Name = L"labelSpeedCaption";
    this->labelSpeedCaption->Size = System::Drawing::Size(133, 20);
    this->labelSpeedCaption->TabIndex = 19;
    this->labelSpeedCaption->Text = L"Скорость UART:";
    // 
    // labelSpeedValue
    // 
    this->labelSpeedValue->AutoSize = true;
    this->labelSpeedValue->Location = System::Drawing::Point(1141, 105);
    this->labelSpeedValue->Name = L"labelSpeedValue";
    this->labelSpeedValue->Size = System::Drawing::Size(14, 20);
    this->labelSpeedValue->TabIndex = 20;
    this->labelSpeedValue->Text = L"-";
    // 
    // buttonClearLog
    // 
    this->buttonClearLog->Location = System::Drawing::Point(940, 695);
    this->buttonClearLog->Name = L"buttonClearLog";
    this->buttonClearLog->Size = System::Drawing::Size(72, 30);
    this->buttonClearLog->TabIndex = 10;
    this->buttonClearLog->Text = L"Очистить лог";
    this->buttonClearLog->UseVisualStyleBackColor = true;
    this->buttonClearLog->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonClearLog_Click);
    // 
    // dataGridViewRegisters
    // 
    this->dataGridViewRegisters->AllowUserToAddRows = false;
    this->dataGridViewRegisters->AllowUserToDeleteRows = false;
    this->dataGridViewRegisters->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
    this->dataGridViewRegisters->Dock = System::Windows::Forms::DockStyle::Fill;
    this->dataGridViewRegisters->Location = System::Drawing::Point(3, 22);
    this->dataGridViewRegisters->Name = L"dataGridViewRegisters";
    this->dataGridViewRegisters->RowHeadersWidth = 62;
    this->dataGridViewRegisters->Size = System::Drawing::Size(556, 491);
    this->dataGridViewRegisters->TabIndex = 0;
    this->dataGridViewRegisters->CellBeginEdit += gcnew System::Windows::Forms::DataGridViewCellCancelEventHandler(this, &ModbusMasterForm::dataGridViewRegisters_CellBeginEdit);
    this->dataGridViewRegisters->CellEndEdit += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &ModbusMasterForm::dataGridViewRegisters_CellEndEdit);
    this->dataGridViewRegisters->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &ModbusMasterForm::dataGridViewRegisters_CellValueChanged);
    // 
    // dataGridViewParameters
    // 
    this->dataGridViewParameters->AllowUserToAddRows = false;
    this->dataGridViewParameters->AllowUserToDeleteRows = false;
    this->dataGridViewParameters->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
    this->dataGridViewParameters->Dock = System::Windows::Forms::DockStyle::Fill;
    this->dataGridViewParameters->Location = System::Drawing::Point(3, 22);
    this->dataGridViewParameters->Name = L"dataGridViewParameters";
    this->dataGridViewParameters->ReadOnly = true;
    this->dataGridViewParameters->RowHeadersWidth = 62;
    this->dataGridViewParameters->Size = System::Drawing::Size(663, 341);
    this->dataGridViewParameters->TabIndex = 1;
    this->dataGridViewParameters->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &ModbusMasterForm::dataGridViewParameters_CellDoubleClick);
    // 
    // buttonExport
    // 
    this->buttonExport->Location = System::Drawing::Point(224, 638);
    this->buttonExport->Name = L"buttonExport";
    this->buttonExport->Size = System::Drawing::Size(100, 35);
    this->buttonExport->TabIndex = 1;
    this->buttonExport->Text = L"Экспорт в CSV";
    this->buttonExport->UseVisualStyleBackColor = true;
    this->buttonExport->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonExport_Click);
    // 
    // buttonSendChanges
    // 
    this->buttonSendChanges->Enabled = false;
    this->buttonSendChanges->Location = System::Drawing::Point(98, 638);
    this->buttonSendChanges->Name = L"buttonSendChanges";
    this->buttonSendChanges->Size = System::Drawing::Size(120, 35);
    this->buttonSendChanges->TabIndex = 6;
    this->buttonSendChanges->Text = L"Отправить изменения";
    this->buttonSendChanges->UseVisualStyleBackColor = true;
    this->buttonSendChanges->Click += gcnew System::EventHandler(this, &ModbusMasterForm::buttonSendChanges_Click);
    // 
    // groupBoxRegisters
    // 
    this->groupBoxRegisters->Controls->Add(this->dataGridViewRegisters);
    this->groupBoxRegisters->Location = System::Drawing::Point(692, 161);
    this->groupBoxRegisters->Name = L"groupBoxRegisters";
    this->groupBoxRegisters->Size = System::Drawing::Size(562, 516);
    this->groupBoxRegisters->TabIndex = 6;
    this->groupBoxRegisters->TabStop = false;
    this->groupBoxRegisters->Text = L"Все регистры (32-битные значения)";
    // 
    // groupBoxParameters
    // 
    this->groupBoxParameters->Controls->Add(this->dataGridViewParameters);
    this->groupBoxParameters->Location = System::Drawing::Point(16, 161);
    this->groupBoxParameters->Name = L"groupBoxParameters";
    this->groupBoxParameters->Size = System::Drawing::Size(669, 366);
    this->groupBoxParameters->TabIndex = 7;
    this->groupBoxParameters->TabStop = false;
    this->groupBoxParameters->Text = L"Контролируемые параметры";
    // 
    // ModbusMasterForm
    // 
    this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
    this->ClientSize = System::Drawing::Size(1266, 740);
    this->Controls->Add(this->groupBoxParameters);
    this->Controls->Add(this->groupBoxRegisters);
    this->Controls->Add(this->buttonSendChanges);
    this->Controls->Add(this->buttonExport);
    this->Controls->Add(this->buttonClearLog);
    this->Controls->Add(this->groupBoxPort);
    this->Controls->Add(this->labelStatus);
    this->Controls->Add(this->textBoxLog);
    this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(204)));
    this->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
    this->Name = L"ModbusMasterForm";
    this->Text = L"Modbus Master для RS-485 устройства (Монитор регистров)";
    this->Load += gcnew System::EventHandler(this, &ModbusMasterForm::Form1_Load);
    this->groupBoxPort->ResumeLayout(false);
    this->groupBoxPort->PerformLayout();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericSlaveID))->EndInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewRegisters))->EndInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridViewParameters))->EndInit();
    this->groupBoxRegisters->ResumeLayout(false);
    this->groupBoxParameters->ResumeLayout(false);
    this->ResumeLayout(false);
    this->PerformLayout();

}

void ModbusMasterForm::InitializeCustomComponent()
{
    numericSlaveID->Minimum = MIN_SLAVE_ID;
    numericSlaveID->Maximum = MAX_SLAVE_ID;
    numericSlaveID->Value = 1;
}

void ModbusMasterForm::InitializeRegistersGrid()
{
    dataGridViewRegisters->Columns->Clear();

    dataGridViewRegisters->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dataGridViewRegisters->ScrollBars = ScrollBars::Both;

    dataGridViewRegisters->Columns->Add(L"Address", L"Адрес");
    dataGridViewRegisters->Columns->Add(L"ValueHex", L"Значение (hex)");
    dataGridViewRegisters->Columns->Add(L"ValueInt32", L"Значение (int32)");
    dataGridViewRegisters->Columns->Add(L"ValueFloat", L"Значение (float)");

    dataGridViewRegisters->Columns[0]->Width = 100;
    dataGridViewRegisters->Columns[1]->Width = 150;
    dataGridViewRegisters->Columns[2]->Width = 120;
    dataGridViewRegisters->Columns[3]->Width = 120;
}

void ModbusMasterForm::InitializeParametersGrid()
{
    dataGridViewParameters->Columns->Clear();

    dataGridViewParameters->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::None;
    dataGridViewParameters->ScrollBars = ScrollBars::Both;
    dataGridViewParameters->ReadOnly = true;

    dataGridViewParameters->Columns->Add(L"DateTime", L"Дата/Время");
    dataGridViewParameters->Columns->Add(L"Parameter", L"Параметр");
    dataGridViewParameters->Columns->Add(L"RegisterAddr", L"Адрес регистра");
    dataGridViewParameters->Columns->Add(L"Value", L"Значение");
    dataGridViewParameters->Columns->Add(L"Unit", L"Ед. изм.");

    dataGridViewParameters->Columns[0]->Width = 140;
    dataGridViewParameters->Columns[1]->Width = 140;
    dataGridViewParameters->Columns[2]->Width = 120;
    dataGridViewParameters->Columns[3]->Width = 110;
    dataGridViewParameters->Columns[4]->Width = 90;
}

void ModbusMasterForm::InitializeParameters()
{
    paramNames->Clear();
    paramAddresses->Clear();
    paramUnits->Clear();

    // Физические адреса 16-битных регистров (согласно документации)
    paramNames->Add(L"Плотность");
    paramAddresses->Add(257);
    paramUnits->Add(L"кг/м³");

    paramNames->Add(L"Температура");
    paramAddresses->Add(259);
    paramUnits->Add(L"°C");

    paramNames->Add(L"Uout");
    paramAddresses->Add(264);
    paramUnits->Add(L"В");

    paramNames->Add(L"Период А");
    paramAddresses->Add(286);
    paramUnits->Add(L"мкс");

    paramNames->Add(L"Ku");
    paramAddresses->Add(285);
    paramUnits->Add(L"");

    paramNames->Add(L"Плотность NC");
    paramAddresses->Add(288);
    paramUnits->Add(L"кг/м³");

    paramNames->Add(L"Добротность");
    paramAddresses->Add(265);
    paramUnits->Add(L"");
}

void ModbusMasterForm::InitializeTimer()
{
    updateTimer = gcnew System::Windows::Forms::Timer();
    updateTimer->Interval = 5000; // стартово 5 секунд
    updateTimer->Tick += gcnew EventHandler(this, &ModbusMasterForm::OnTimerTick);

    pollTimer = gcnew System::Windows::Forms::Timer();
    pollTimer->Interval = 5; // короткий шаг, чтобы UI успевал жить
    pollTimer->Tick += gcnew EventHandler(this, &ModbusMasterForm::OnPollTimerTick);

    isAutoUpdating = false;
    pollState = PollState::Idle;
}
void ModbusMasterForm::InitializeLiveValuesPanel()
{
    groupBoxLiveValues = gcnew GroupBox();
    labelLiveDensityCaption = gcnew Label();
    labelLiveTemperatureCaption = gcnew Label();
    labelLiveUoutCaption = gcnew Label();

    textBoxLiveDensity = gcnew TextBox();
    textBoxLiveTemperature = gcnew TextBox();
    textBoxLiveUout = gcnew TextBox();

    groupBoxLiveValues->Location = System::Drawing::Point(16, 533);
    groupBoxLiveValues->Name = L"groupBoxLiveValues";
    groupBoxLiveValues->Size = System::Drawing::Size(318, 95);
    groupBoxLiveValues->TabStop = false;
    groupBoxLiveValues->Text = L"";

    labelLiveDensityCaption->AutoSize = true;
    labelLiveDensityCaption->Location = System::Drawing::Point(10, 25);
    labelLiveDensityCaption->Text = L"Плотность:";

    textBoxLiveDensity->Location = System::Drawing::Point(120, 22);
    textBoxLiveDensity->Size = System::Drawing::Size(185, 26);
    textBoxLiveDensity->ReadOnly = true;
    textBoxLiveDensity->TabStop = false;

    labelLiveTemperatureCaption->AutoSize = true;
    labelLiveTemperatureCaption->Location = System::Drawing::Point(10, 55);
    labelLiveTemperatureCaption->Text = L"Температура:";

    textBoxLiveTemperature->Location = System::Drawing::Point(120, 52);
    textBoxLiveTemperature->Size = System::Drawing::Size(185, 26);
    textBoxLiveTemperature->ReadOnly = true;
    textBoxLiveTemperature->TabStop = false;

    labelLiveUoutCaption->AutoSize = true;
    labelLiveUoutCaption->Location = System::Drawing::Point(10, 85);
    labelLiveUoutCaption->Text = L"Uout:";

    textBoxLiveUout->Location = System::Drawing::Point(120, 82);
    textBoxLiveUout->Size = System::Drawing::Size(185, 26);
    textBoxLiveUout->ReadOnly = true;
    textBoxLiveUout->TabStop = false;

    // уменьшим высоту группы, чтобы третья строка влезла
    groupBoxLiveValues->Size = System::Drawing::Size(318, 115);

    groupBoxLiveValues->Controls->Add(labelLiveDensityCaption);
    groupBoxLiveValues->Controls->Add(textBoxLiveDensity);
    groupBoxLiveValues->Controls->Add(labelLiveTemperatureCaption);
    groupBoxLiveValues->Controls->Add(textBoxLiveTemperature);
    groupBoxLiveValues->Controls->Add(labelLiveUoutCaption);
    groupBoxLiveValues->Controls->Add(textBoxLiveUout);

    this->Controls->Add(groupBoxLiveValues);

    textBoxLiveDensity->Text = L"-";
    textBoxLiveTemperature->Text = L"-";
    textBoxLiveUout->Text = L"-";
}

void ModbusMasterForm::StartPollingCycle()
{
    if (hSerial == INVALID_HANDLE_VALUE) return;
    if (isPollingInProgress) return;

    allRegistersData->Clear();
    allRegistersRaw->Clear();
    pollStartAddress = 0;
    pollCurrentBatchSize = 0;
    pollBatchStartTick = 0;
    pollTotalBytesRead = 0;
    Array::Clear(pollResponseBuffer, 0, pollResponseBuffer->Length);

    pollState = PollState::SendBatch;
    isPollingInProgress = true;

    pollTimer->Start();
}

void ModbusMasterForm::SendPollBatch()
{
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        FinishPollingCycle();
        return;
    }

    const int BATCH_SIZE = 20;
    const int MAX_REGISTER_ADDRESS = TOTAL_32BIT_VALUES;

    if (pollStartAddress >= MAX_REGISTER_ADDRESS)
    {
        FinishPollingCycle();
        return;
    }

    // Не даём батчу пересекать границу 256
    int boundary = ((pollStartAddress / 256) + 1) * 256;
    int maxUntilBoundary = boundary - pollStartAddress;

    pollCurrentBatchSize = min(BATCH_SIZE, MAX_REGISTER_ADDRESS - pollStartAddress);
    pollCurrentBatchSize = min(pollCurrentBatchSize, maxUntilBoundary);

    uint8_t slaveID = static_cast<uint8_t>(numericSlaveID->Value);

    std::vector<uint8_t> request;
    request.push_back(slaveID);
    request.push_back(0x03);
    request.push_back((pollStartAddress >> 8) & 0xFF);
    request.push_back(pollStartAddress & 0xFF);
    request.push_back((pollCurrentBatchSize >> 8) & 0xFF);
    request.push_back(pollCurrentBatchSize & 0xFF);

    uint16_t crc = CalculateCRC16(request.data(), request.size());
    request.push_back(crc & 0xFF);
    request.push_back((crc >> 8) & 0xFF);

    PurgeComm(hSerial, PURGE_RXCLEAR);

    DWORD bytesWritten = 0;
    if (!WriteFile(hSerial, request.data(), (DWORD)request.size(), &bytesWritten, NULL))
    {
        pollStartAddress += pollCurrentBatchSize;
        pollState = PollState::SendBatch;
        return;
    }

    pollBatchStartTick = GetTickCount();
    pollTotalBytesRead = 0;
    Array::Clear(pollResponseBuffer, 0, pollResponseBuffer->Length);

    pollState = PollState::WaitBatch;
}

void ModbusMasterForm::ReadPollBatch()
{
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        FinishPollingCycle();
        return;
    }

    pin_ptr<System::Byte> pResponse = &pollResponseBuffer[0];

    DWORD bytesRead = 0;
    ReadFile(hSerial,reinterpret_cast<void*>(pResponse + pollTotalBytesRead),pollResponseBuffer->Length - pollTotalBytesRead,&bytesRead,NULL);

    if (bytesRead > 0)
    {
        pollTotalBytesRead += bytesRead;

        uint8_t slaveID = static_cast<uint8_t>(numericSlaveID->Value);

        if (pollTotalBytesRead >= 3 &&
            pResponse[0] == slaveID &&
            pResponse[1] == 0x03)
        {
            int expectedLength = pResponse[2] + 5; // slave + func + byteCount + data + CRC

            if (pollTotalBytesRead >= static_cast<DWORD>(expectedLength))
            {
                if (CheckCRC(reinterpret_cast<uint8_t*>(pResponse), expectedLength))
                {
                    uint8_t dataLength = pResponse[2];
                    int expectedDataLength = pollCurrentBatchSize * 4;

                    if (dataLength != expectedDataLength)
                    {
                        LogMessage(String::Format(
                            L"SHORT BATCH: start={0}, requestedRegs={1}, expectedBytes={2}, gotBytes={3}",
                            pollStartAddress,
                            pollCurrentBatchSize,
                            expectedDataLength,
                            dataLength));

                        pollStartAddress += pollCurrentBatchSize;
                        pollState = PollState::SendBatch;
                        return;
                    }

                    int registerCount = dataLength / 4;

                    for (int i = 0; i < registerCount; i++)
                    {
                        int logicalReg = pollStartAddress + i;

                        System::UInt32 raw =
                            ((System::UInt32)pResponse[3 + i * 4]) |
                            ((System::UInt32)pResponse[4 + i * 4] << 8) |
                            ((System::UInt32)pResponse[5 + i * 4] << 16) |
                            ((System::UInt32)pResponse[6 + i * 4] << 24);

                        float floatValue = ConvertToFloat(static_cast<uint32_t>(raw));
                        int32_t intValue = static_cast<int32_t>(raw);

                        allRegistersRaw[logicalReg] = raw;
                        allRegistersData[logicalReg] = Tuple::Create(floatValue, intValue);
                    }
                }

                pollStartAddress += pollCurrentBatchSize;
                pollState = PollState::SendBatch;
                return;
            }
        }
    }

    if ((GetTickCount() - pollBatchStartTick) >= 500)
    {
        pollStartAddress += pollCurrentBatchSize;
        pollState = PollState::SendBatch;
    }
}

void ModbusMasterForm::FinishPollingCycle()
{
    pollTimer->Stop();
    pollState = PollState::Idle;
    isPollingInProgress = false;

    // Эти 3 поля обновляем всегда, даже во время редактирования
    UpdateLiveValuesPanel();

    // Таблицы не трогаем, если пользователь редактирует
    if (isRegisterCellEditing)
    {
        pendingTablesRefresh = true;
    }
    else
    {
        UpdateRegistersData();
        pendingTablesRefresh = false;
    }

    lastUpdateTime = DateTime::Now;

    LogMessage(String::Format(
        L"Цикл опроса завершён: {0:dd.MM.yyyy HH:mm:ss}",
        lastUpdateTime));

    if (isAutoUpdating)
    {
        labelAutoUpdateStatus->Text =
            String::Format(L"Автообновление: активно (последнее: {0:HH:mm:ss})", lastUpdateTime);
    }
    else
    {
        labelAutoUpdateStatus->Text =
            String::Format(L"Обновлено: {0:HH:mm:ss}", lastUpdateTime);
    }
}

void ModbusMasterForm::OnPollTimerTick(Object^ sender, EventArgs^ e)
{
    switch (pollState)
    {
    case PollState::Idle:
        pollTimer->Stop();
        break;

    case PollState::SendBatch:
        SendPollBatch();
        break;

    case PollState::WaitBatch:
        ReadPollBatch();
        break;
    }
}

// ============================================================================
// Вспомогательные методы для преобразования
// ============================================================================

float ModbusMasterForm::ConvertToFloat(uint32_t value)
{
    float result;
    memcpy(&result, &value, sizeof(float));
    return result;
}
uint32_t ModbusMasterForm::FloatToRaw(float value)
{
    uint32_t raw;
    memcpy(&raw, &value, sizeof(float));
    return raw;
}
int32_t ModbusMasterForm::ConvertToInt32(uint32_t value)
{
    return static_cast<int32_t>(value);
}
float ModbusMasterForm::ConvertToFloatSwappedWords(uint32_t value)
{
    uint32_t swapped = ((value & 0x0000FFFF) << 16) | ((value & 0xFFFF0000) >> 16);
    float result;
    memcpy(&result, &swapped, sizeof(float));
    return result;
}

bool ModbusMasterForm::ReadLogicalReg32(int logicalReg, System::UInt32% rawValue, System::Int32% intValue, float% floatValue)
{
    return ReadLogicalReg32WithFunction(logicalReg, 0x03, rawValue, intValue, floatValue);
}
bool ModbusMasterForm::ReadLogicalReg32WithFunction(
    int logicalReg,
    System::Byte functionCode,
    System::UInt32% rawValue,
    System::Int32% intValue,
    float% floatValue)
{
    rawValue = 0;
    intValue = 0;
    floatValue = 0.0f;

    if (hSerial == INVALID_HANDLE_VALUE)
        return false;

    int startAddress = logicalReg;
    uint8_t slaveID = static_cast<uint8_t>(numericSlaveID->Value);

    std::vector<uint8_t> request;
    request.push_back(slaveID);
    request.push_back(functionCode);
    request.push_back((startAddress >> 8) & 0xFF);
    request.push_back(startAddress & 0xFF);
    request.push_back(0x00);
    request.push_back(0x01); // один логический 32-битный регистр

    uint16_t crc = CalculateCRC16(request.data(), request.size());
    request.push_back(crc & 0xFF);
    request.push_back((crc >> 8) & 0xFF);

    Monitor::Enter(portLock);
    try
    {
        PurgeComm(hSerial, PURGE_RXCLEAR);

        DWORD bytesWritten = 0;
        if (!WriteFile(hSerial, request.data(), (DWORD)request.size(), &bytesWritten, NULL) ||
            bytesWritten != request.size())
        {
            return false;
        }

        uint8_t response[64] = { 0 };
        DWORD totalBytesRead = 0;
        DWORD bytesRead = 0;
        DWORD startTime = GetTickCount();

        while ((GetTickCount() - startTime) < 600)
        {
            if (ReadFile(hSerial,
                response + totalBytesRead,
                sizeof(response) - totalBytesRead,
                &bytesRead,
                NULL))
            {
                if (bytesRead > 0)
                {
                    totalBytesRead += bytesRead;

                    if (totalBytesRead >= 5 && response[0] == slaveID && response[1] == (functionCode | 0x80))
                    {
                        return false;
                    }

                    if (totalBytesRead >= 3 && response[0] == slaveID && response[1] == functionCode)
                    {
                        int expectedLength = response[2] + 5;
                        if (totalBytesRead >= (DWORD)expectedLength)
                        {
                            if (!CheckCRC(response, expectedLength))
                                return false;

                            if (response[2] != 0x04)
                                return false;

                            System::UInt32 raw =
                                ((System::UInt32)response[3]) |
                                ((System::UInt32)response[4] << 8) |
                                ((System::UInt32)response[5] << 16) |
                                ((System::UInt32)response[6] << 24);

                            rawValue = raw;
                            intValue = static_cast<System::Int32>(raw);
                            floatValue = ConvertToFloat(static_cast<uint32_t>(raw));
                            return true;
                        }
                    }
                }
            }

            Sleep(5);
        }

        return false;
    }
    finally
    {
        Monitor::Exit(portLock);
    }
}
void ModbusMasterForm::ClearDeviceInfoLabels()
{
    labelModelValue->Text = L"-";
    labelSerialValue->Text = L"-";
    labelTypeValue->Text = L"-";
    labelSpeedValue->Text = L"-";
}

void ModbusMasterForm::UpdateDeviceInfoLabels()
{
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        ClearDeviceInfoLabels();
        return;
    }

    System::UInt32 raw = 0;
    System::Int32 asInt = 0;
    float asFloat = 0.0f;

    if (ReadLogicalReg32(200, raw, asInt, asFloat))
        labelModelValue->Text = asInt.ToString();
    else
        labelModelValue->Text = L"-";

    if (ReadLogicalReg32(203, raw, asInt, asFloat))
        labelSerialValue->Text = asInt.ToString();
    else
        labelSerialValue->Text = L"-";

    if (ReadLogicalReg32(204, raw, asInt, asFloat))
        labelTypeValue->Text = asInt.ToString();
    else
        labelTypeValue->Text = L"-";

    if (ReadLogicalReg32(205, raw, asInt, asFloat))
        labelSpeedValue->Text = asInt.ToString();
    else
        labelSpeedValue->Text = L"-";
}
void ModbusMasterForm::DebugTemperatureProbe()
{
    array<int>^ regs = gcnew array<int> { 258, 259, 260, 261 };
    array<System::Byte>^ funcs = gcnew array<System::Byte> { 0x03, 0x04 };

    for each (System::Byte fc in funcs)
    {
        for each (int reg in regs)
        {
            System::UInt32 raw = 0;
            System::Int32 asInt = 0;
            float asFloat = 0.0f;

            if (ReadLogicalReg32WithFunction(reg, fc, raw, asInt, asFloat))
            {
                float swapped = ConvertToFloatSwappedWords(raw);

                LogMessage(String::Format(
                    L"TEMP TEST fc=0x{0:X2} Reg{1}: raw=0x{2:X8}, int={3}, float={4:F6}, swapped={5:F6}",
                    fc, reg, raw, asInt, asFloat, swapped));
            }
            else
            {
                LogMessage(String::Format(
                    L"TEMP TEST fc=0x{0:X2} Reg{1}: нет данных/ошибка",
                    fc, reg));
            }
        }
    }
}
// ============================================================================
// Работа с портом
// ============================================================================

void ModbusMasterForm::Form1_Load(System::Object^ sender, System::EventArgs^ e)
{
    RefreshPorts();
    LogMessage(L"Программа запущена. Режим RS-485, 9600 бод.");
}

void ModbusMasterForm::RefreshPorts()
{
    comboBoxPorts->Items->Clear();

    if (availablePorts == nullptr)
        availablePorts = gcnew List<String^>();
    else
        availablePorts->Clear();

    bool showAll = checkBoxShowAll->Checked;

    LogMessage(L"Поиск COM-портов..." + (showAll ? L" (включая занятые)" : L" (только доступные)"));

    std::set<std::string> portSet;

    const char* registryPaths[] = {
        "HARDWARE\\DEVICEMAP\\SERIALCOMM",
        "SYSTEM\\CurrentControlSet\\Services\\Serial\\Parameters\\SerialPorts"
    };

    for (const char* registryPath : registryPaths)
    {
        HKEY hKey;
        LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, registryPath, 0, KEY_READ, &hKey);

        if (result == ERROR_SUCCESS)
        {
            DWORD index = 0;
            char valueName[256];
            DWORD valueNameSize;
            BYTE data[256];
            DWORD dataSize;
            DWORD type;

            while (true)
            {
                valueNameSize = sizeof(valueName);
                dataSize = sizeof(data);

                result = RegEnumValueA(hKey, index, valueName, &valueNameSize,
                    NULL, &type, data, &dataSize);

                if (result == ERROR_NO_MORE_ITEMS)
                    break;

                if (result == ERROR_SUCCESS && type == REG_SZ)
                {
                    std::string portName = "\\\\.\\" + std::string((char*)data);

                    if (showAll)
                    {
                        portSet.insert(portName);
                        LogMessage(L"Найден порт: " + StringToWString(portName));
                    }
                    else
                    {
                        HANDLE hTest = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

                        if (hTest != INVALID_HANDLE_VALUE)
                        {
                            CloseHandle(hTest);
                            portSet.insert(portName);
                            LogMessage(L"Найден доступный порт: " + StringToWString(portName));
                        }
                    }
                }
                index++;
            }
            RegCloseKey(hKey);
        }
    }

    for (const auto& port : portSet)
    {
        std::string displayName = port.substr(4); 
        String^ managedDisplayName = gcnew String(displayName.c_str());
        String^ managedFullName = gcnew String(port.c_str()); // 

        availablePorts->Add(managedFullName);       // для CreateFile
        comboBoxPorts->Items->Add(managedDisplayName); // для интерфейса
    }

    if (availablePorts->Count == 0)
    {
        LogMessage(L"Не найдено ни одного COM-порта.");
    }
    else
    {
        LogMessage(L"Найдено портов: " + availablePorts->Count);
        comboBoxPorts->SelectedIndex = 0;
    }
}

void ModbusMasterForm::ConnectToPort()
{
    if (comboBoxPorts->SelectedIndex < 0 || availablePorts == nullptr)
    {
        MessageBox::Show(L"Выберите порт для подключения!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    String^ selectedPort = availablePorts[comboBoxPorts->SelectedIndex];
    std::string selectedPortStd = msclr::interop::marshal_as<std::string>(selectedPort);

    LogMessage(L"Открытие порта " + selectedPort + L"...");

    hSerial = CreateFileA(selectedPortStd.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        LogMessage(L"Ошибка! Не удалось открыть порт. Код ошибки: " + error);
        MessageBox::Show(L"Не удалось открыть порт!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return;
    }

    if (ConfigurePort())
    {
        ClosePort();
        LogMessage(L"Не удалось настроить порт.");
        return;
    }

    LogMessage(L"Порт успешно открыт и настроен!");

    buttonConnect->Enabled = false;
    buttonDisconnect->Enabled = true;
    comboBoxPorts->Enabled = false;
    buttonRefresh->Enabled = false;
    buttonScanDevices->Enabled = true;
    buttonRefreshRegisters->Enabled = true;
    buttonStartAutoUpdate->Enabled = true;

    labelStatus->Text = L"Статус: Подключен к " + comboBoxPorts->Text;
    UpdateDeviceInfoLabels();
}

bool ModbusMasterForm::ConfigurePort()
{
    LogMessage(L"Настройка порта...");

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        LogMessage(L"Ошибка получения текущих параметров порта.");
        return true;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = byteSize;
    dcbSerialParams.StopBits = stopBits;
    dcbSerialParams.Parity = parity;

    dcbSerialParams.fOutxCtsFlow = FALSE;
    dcbSerialParams.fOutxDsrFlow = FALSE;
    dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
    dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        LogMessage(L"Ошибка установки параметров порта.");
        return true;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 10;
    timeouts.ReadTotalTimeoutConstant = 120;
    timeouts.ReadTotalTimeoutMultiplier = 2;
    timeouts.WriteTotalTimeoutConstant = 100;
    timeouts.WriteTotalTimeoutMultiplier = 1;

    if (!SetCommTimeouts(hSerial, &timeouts))
    {
        LogMessage(L"Ошибка установки таймаутов.");
        return true;
    }

    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    LogMessage(L"Порт сконфигурирован: 9600 бод, 8 бит данных, 1 стоп-бит, без контроля четности.");
    return false;
}

void ModbusMasterForm::DisconnectFromPort()
{


    if (pollTimer != nullptr)
    {
        pollTimer->Stop();
    }

    pollState = PollState::Idle;
    isPollingInProgress = false;

    ClosePort();

    buttonConnect->Enabled = true;
    buttonDisconnect->Enabled = false;
    comboBoxPorts->Enabled = true;
    buttonRefresh->Enabled = true;
    buttonScanDevices->Enabled = false;
    buttonRefreshRegisters->Enabled = false;
    buttonStartAutoUpdate->Enabled = false;
    buttonStopAutoUpdate->Enabled = false;

    if (isAutoUpdating)
    {
        isAutoUpdating = false;
        updateTimer->Stop();
        buttonStartAutoUpdate->Enabled = false;
        buttonStopAutoUpdate->Enabled = false;
        labelAutoUpdateStatus->Text = L"Автообновление: выключено";
    }

    labelStatus->Text = L"Статус: Отключен";
    ClearDeviceInfoLabels();
    LogMessage(L"Отключен от порта.");
}

void ModbusMasterForm::ClosePort()
{
    if (hSerial != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}

// ============================================================================
// Modbus протокол
// ============================================================================

uint16_t ModbusMasterForm::CalculateCRC16(const uint8_t* data, size_t length)
{
    unsigned char crcHi = 0xFF;
    unsigned char crcLo = 0xFF;
    unsigned int index;

    while (length--)
    {
        index = crcLo ^ *data++;
        crcLo = crcHi ^ auchCRCHi[index];
        crcHi = auchCRCLo[index];
    }

    return (crcHi << 8) | crcLo;
}

bool ModbusMasterForm::CheckCRC(const uint8_t* response, size_t length)
{
    if (length < 2) return false;
    uint16_t calculatedCRC = CalculateCRC16(response, length - 2);
    uint16_t responseCRC = (response[length - 1] << 8) | response[length - 2];
    return (calculatedCRC == responseCRC);
}

// ============================================================================
// Сканирование устройств
// ============================================================================

void ModbusMasterForm::ScanForDevices()
{
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        MessageBox::Show(L"Порт не подключен!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    LogMessage(L"Начинаю поиск устройств Modbus...");
    textBoxScanResult->Visible = true;
    labelFoundDevices->Visible = true;

    buttonScanDevices->Enabled = false;

    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    std::vector<int> foundDevices;
    std::wstringstream result;

    COMMTIMEOUTS originalTimeouts;
    GetCommTimeouts(hSerial, &originalTimeouts);

    COMMTIMEOUTS scanTimeouts;
    scanTimeouts.ReadIntervalTimeout = 50;
    scanTimeouts.ReadTotalTimeoutConstant = 100;
    scanTimeouts.ReadTotalTimeoutMultiplier = 0;
    scanTimeouts.WriteTotalTimeoutConstant = 100;
    scanTimeouts.WriteTotalTimeoutMultiplier = 0;

    SetCommTimeouts(hSerial, &scanTimeouts);

    std::vector<uint8_t> request;
    request.resize(8);

    for (int slaveID = MIN_SLAVE_ID; slaveID <= MAX_SLAVE_ID; slaveID++)
    {
        request[0] = slaveID;
        request[1] = 0x03;
        request[2] = 0x00;
        request[3] = 0x00;
        request[4] = 0x00;
        request[5] = 0x02;

        uint16_t crc = CalculateCRC16(request.data(), 6);
        request[6] = crc & 0xFF;
        request[7] = (crc >> 8) & 0xFF;

        PurgeComm(hSerial, PURGE_RXCLEAR);

        DWORD bytesWritten;
        if (!WriteFile(hSerial, request.data(), (DWORD)request.size(), &bytesWritten, NULL))
        {
            continue;
        }

        uint8_t response[256];
        DWORD totalBytesRead = 0;
        DWORD bytesRead = 0;
        DWORD startTime = GetTickCount();

        while (GetTickCount() - startTime < 200)
        {
            if (ReadFile(hSerial, response + totalBytesRead,
                sizeof(response) - totalBytesRead, &bytesRead, NULL))
            {
                if (bytesRead > 0)
                {
                    totalBytesRead += bytesRead;

                    if (totalBytesRead >= 5 && response[0] == slaveID)
                    {
                        if (CheckCRC(response, totalBytesRead))
                        {
                            foundDevices.push_back(slaveID);
                            LogMessage(L"Найдено устройство с ID: " + slaveID);

                            result << L"ID: " << slaveID << L"\r\n";
                            break;
                        }
                    }
                }
            }
            Sleep(10);
        }

        if (slaveID % 5 == 0)
        {
            labelStatus->Text = L"Сканирование... " + (slaveID * 100 / MAX_SLAVE_ID) + L"%";
            Application::DoEvents();
        }

        Sleep(50);
    }

    SetCommTimeouts(hSerial, &originalTimeouts);

    if (foundDevices.empty())
    {
        result << L"Устройства не найдены";
        LogMessage(L"Устройства не найдены");
    }
    else
    {
        LogMessage(L"Поиск завершен. Найдено устройств: " + foundDevices.size());
    }

    textBoxScanResult->Text = gcnew String(result.str().c_str());
    labelStatus->Text = L"Статус: Подключен к " + comboBoxPorts->Text;
    buttonScanDevices->Enabled = true;
}

// ============================================================================
// Чтение регистров (ОСНОВНОЙ МЕТОД - САМЫЙ ВАЖНЫЙ)
// ============================================================================

void ModbusMasterForm::RefreshRegistersData()
{
    if (hSerial == INVALID_HANDLE_VALUE)
        return;

    allRegistersData->Clear();
    allRegistersRaw->Clear();

    ScanAllRegisters();
    UpdateRegistersData();
}

void ModbusMasterForm::ScanAllRegisters()
{
    if (hSerial == INVALID_HANDLE_VALUE) return;

    Monitor::Enter(portLock);

    try
    {
        const int BATCH_SIZE = 20;
        const int MAX_REGISTER_ADDRESS = TOTAL_32BIT_VALUES;
        const DWORD RESPONSE_TIMEOUT_MS = 250;

        uint8_t slaveID = static_cast<uint8_t>(numericSlaveID->Value);

        int startAddress = 0;

        while (startAddress < MAX_REGISTER_ADDRESS)
        {
            // Не даём батчу перейти через границу 256
            int boundary = ((startAddress / 256) + 1) * 256;
            int maxUntilBoundary = boundary - startAddress;

            int currentBatchSize = min(BATCH_SIZE, MAX_REGISTER_ADDRESS - startAddress);
            currentBatchSize = min(currentBatchSize, maxUntilBoundary);

            std::vector<uint8_t> request;
            request.reserve(8);
            request.push_back(slaveID);
            request.push_back(0x03);
            request.push_back((startAddress >> 8) & 0xFF);
            request.push_back(startAddress & 0xFF);
            request.push_back((currentBatchSize >> 8) & 0xFF);
            request.push_back(currentBatchSize & 0xFF);

            uint16_t crc = CalculateCRC16(request.data(), request.size());
            request.push_back(crc & 0xFF);
            request.push_back((crc >> 8) & 0xFF);

            PurgeComm(hSerial, PURGE_RXCLEAR);

            DWORD bytesWritten = 0;
            if (!WriteFile(hSerial, request.data(), (DWORD)request.size(), &bytesWritten, NULL) ||
                bytesWritten != request.size())
            {
                startAddress += currentBatchSize;
                continue;
            }

            uint8_t response[256] = { 0 };
            DWORD totalBytesRead = 0;
            DWORD bytesRead = 0;
            DWORD startTime = GetTickCount();
            bool frameComplete = false;

            while ((GetTickCount() - startTime) < RESPONSE_TIMEOUT_MS)
            {
                if (ReadFile(hSerial,
                    response + totalBytesRead,
                    sizeof(response) - totalBytesRead,
                    &bytesRead,
                    NULL))
                {
                    if (bytesRead > 0)
                    {
                        totalBytesRead += bytesRead;

                        if (totalBytesRead >= 3 &&
                            response[0] == slaveID &&
                            response[1] == 0x03)
                        {
                            int expectedLength = response[2] + 5;

                            if (totalBytesRead >= static_cast<DWORD>(expectedLength))
                            {
                                totalBytesRead = expectedLength;
                                frameComplete = true;
                                break;
                            }
                        }
                    }
                }

                Sleep(1);
            }

            if (!frameComplete)
            {
                startAddress += currentBatchSize;
                continue;
            }

            if (!CheckCRC(response, totalBytesRead))
            {
                startAddress += currentBatchSize;
                continue;
            }

            uint8_t dataLength = response[2];
            int expectedDataLength = currentBatchSize * 4;

            if (dataLength != expectedDataLength)
            {
                LogMessage(String::Format(
                    L"SHORT SCAN BATCH: start={0}, requestedRegs={1}, expectedBytes={2}, gotBytes={3}",
                    startAddress,
                    currentBatchSize,
                    expectedDataLength,
                    dataLength));

                startAddress += currentBatchSize;
                continue;
            }

            int registerCount = dataLength / 4;

            for (int i = 0; i < registerCount; i++)
            {
                int logicalReg = startAddress + i;

                System::UInt32 raw =
                    ((System::UInt32)response[3 + i * 4]) |
                    ((System::UInt32)response[4 + i * 4] << 8) |
                    ((System::UInt32)response[5 + i * 4] << 16) |
                    ((System::UInt32)response[6 + i * 4] << 24);

                float floatValue = ConvertToFloat(static_cast<uint32_t>(raw));
                int32_t intValue = static_cast<int32_t>(raw);

                allRegistersRaw[logicalReg] = raw;
                allRegistersData[logicalReg] = Tuple::Create(floatValue, intValue);
            }

            startAddress += currentBatchSize;
        }
    }
    finally
    {
        Monitor::Exit(portLock);
    }
}

// ============================================================================
// Обновление таблиц
// ============================================================================

void ModbusMasterForm::UpdateRegistersData()
{
    UpdateFullRegistersTable();
    UpdateParametersTable();
}

void ModbusMasterForm::UpdateFullRegistersTable()
{
    if (isUpdatingTables) return;
    isUpdatingTables = true;

    dataGridViewRegisters->SuspendLayout();

    try
    {
        // -----------------------------
        // Сохраняем вертикальную позицию
        // -----------------------------
        int topVisibleReg = -1;
        if (dataGridViewRegisters->FirstDisplayedScrollingRowIndex >= 0 &&
            dataGridViewRegisters->Rows->Count > 0)
        {
            int topRow = dataGridViewRegisters->FirstDisplayedScrollingRowIndex;
            Object^ topCell = dataGridViewRegisters->Rows[topRow]->Cells[0]->Value;

            if (topCell != nullptr)
            {
                String^ addrStr = topCell->ToString();
                if (addrStr->StartsWith(L"Reg"))
                {
                    topVisibleReg = Int32::Parse(addrStr->Substring(3));
                }
            }
        }

        // -----------------------------
        // Сохраняем выбранную строку
        // -----------------------------
        int selectedReg = -1;
        if (dataGridViewRegisters->CurrentRow != nullptr &&
            dataGridViewRegisters->CurrentRow->Cells[0]->Value != nullptr)
        {
            String^ addrStr = dataGridViewRegisters->CurrentRow->Cells[0]->Value->ToString();
            if (addrStr->StartsWith(L"Reg"))
            {
                selectedReg = Int32::Parse(addrStr->Substring(3));
            }
        }

        // -----------------------------
        // Сохраняем горизонтальную позицию
        // -----------------------------
        int firstDisplayedColumn = -1;
        int horizontalOffset = 0;

        if (dataGridViewRegisters->Columns->Count > 0)
        {
            try
            {
                firstDisplayedColumn = dataGridViewRegisters->FirstDisplayedScrollingColumnIndex;
            }
            catch (...) {}

            try
            {
                horizontalOffset = dataGridViewRegisters->HorizontalScrollingOffset;
            }
            catch (...) {}
        }

        // -----------------------------
        // Обновляем только строки
        // -----------------------------
        dataGridViewRegisters->Rows->Clear();

        List<int>^ addresses = gcnew List<int>(allRegistersData->Keys);
        addresses->Sort();

        for each (int address in addresses)
        {
            float floatValue = allRegistersData[address]->Item1;
            int32_t intValue = allRegistersData[address]->Item2;
            System::UInt32 rawValue = allRegistersRaw->ContainsKey(address) ? allRegistersRaw[address] : 0;

            if (IsRegisterChanged(address))
            {
                rawValue = changedRegistersValues[address];
                floatValue = changedRegistersFloats[address];
                intValue = static_cast<int32_t>(rawValue);
            }

            String^ hexStr = String::Format(L"0x{0:X8}", rawValue);
            String^ addrStr = String::Format(L"Reg{0}", address);

            int rowIndex = dataGridViewRegisters->Rows->Add(
                addrStr,
                hexStr,
                intValue.ToString(),
                floatValue.ToString(L"F6"));

            if (IsRegisterChanged(address))
            {
                dataGridViewRegisters->Rows[rowIndex]->DefaultCellStyle->BackColor = Color::LightYellow;
            }
        }

        // -----------------------------
        // Восстанавливаем верхнюю строку
        // -----------------------------
        if (topVisibleReg >= 0)
        {
            for (int i = 0; i < dataGridViewRegisters->Rows->Count; i++)
            {
                Object^ cellValue = dataGridViewRegisters->Rows[i]->Cells[0]->Value;
                if (cellValue != nullptr &&
                    cellValue->ToString() == String::Format(L"Reg{0}", topVisibleReg))
                {
                    try
                    {
                        dataGridViewRegisters->FirstDisplayedScrollingRowIndex = i;
                    }
                    catch (...) {}
                    break;
                }
            }
        }

        // -----------------------------
        // Восстанавливаем выбранную строку
        // -----------------------------
        if (selectedReg >= 0)
        {
            for (int i = 0; i < dataGridViewRegisters->Rows->Count; i++)
            {
                Object^ cellValue = dataGridViewRegisters->Rows[i]->Cells[0]->Value;
                if (cellValue != nullptr &&
                    cellValue->ToString() == String::Format(L"Reg{0}", selectedReg))
                {
                    dataGridViewRegisters->Rows[i]->Selected = true;

                    if (dataGridViewRegisters->Rows[i]->Cells->Count > 0)
                    {
                        dataGridViewRegisters->CurrentCell = dataGridViewRegisters->Rows[i]->Cells[0];
                    }
                    break;
                }
            }
        }

        // -----------------------------
        // Восстанавливаем горизонтальную позицию
        // -----------------------------
        if (firstDisplayedColumn >= 0 &&
            firstDisplayedColumn < dataGridViewRegisters->Columns->Count)
        {
            try
            {
                dataGridViewRegisters->FirstDisplayedScrollingColumnIndex = firstDisplayedColumn;
            }
            catch (...) {}
        }

        try
        {
            dataGridViewRegisters->HorizontalScrollingOffset = horizontalOffset;
        }
        catch (...) {}
    }
    finally
    {
        dataGridViewRegisters->ResumeLayout();
        isUpdatingTables = false;
    }
}

void ModbusMasterForm::UpdateParametersTable()
{
    if (isUpdatingTables) return;
    isUpdatingTables = true;

    dataGridViewParameters->SuspendLayout();

    try
    {
        int topVisibleRow = -1;
        if (dataGridViewParameters->FirstDisplayedScrollingRowIndex >= 0 &&
            dataGridViewParameters->Rows->Count > 0)
        {
            topVisibleRow = dataGridViewParameters->FirstDisplayedScrollingRowIndex;
        }

        int selectedRow = -1;
        if (dataGridViewParameters->CurrentRow != nullptr)
        {
            selectedRow = dataGridViewParameters->CurrentRow->Index;
        }

        int firstDisplayedColumn = -1;
        int horizontalOffset = 0;

        if (dataGridViewParameters->Columns->Count > 0)
        {
            try
            {
                firstDisplayedColumn = dataGridViewParameters->FirstDisplayedScrollingColumnIndex;
            }
            catch (...) {}

            try
            {
                horizontalOffset = dataGridViewParameters->HorizontalScrollingOffset;
            }
            catch (...) {}
        }

        dataGridViewParameters->Rows->Clear();

        String^ currentTime = DateTime::Now.ToString(L"dd.MM.yyyy HH:mm:ss");

        for (int i = 0; i < paramNames->Count; i++)
        {
            float value = 0.0f;
            int physicalAddress = paramAddresses[i];

            if (IsRegisterChanged(physicalAddress))
            {
                value = changedRegistersFloats[physicalAddress];
            }
            else if (allRegistersData != nullptr && allRegistersData->ContainsKey(physicalAddress))
            {
                value = allRegistersData[physicalAddress]->Item1;
            }

            String^ addrStr = String::Format(L"Reg{0}", physicalAddress);

            String^ valueStr;
            if (paramUnits[i] == L"°C" || paramUnits[i] == L"кг/м³" || paramUnits[i] == L"В")
                valueStr = value.ToString(L"F3");
            else if (paramUnits[i] == L"мкс")
                valueStr = value.ToString(L"F0");
            else
                valueStr = value.ToString(L"F6");

            int rowIndex = dataGridViewParameters->Rows->Add(
                currentTime,
                paramNames[i],
                addrStr,
                valueStr,
                paramUnits[i]);

            if (IsRegisterChanged(physicalAddress))
            {
                dataGridViewParameters->Rows[rowIndex]->DefaultCellStyle->BackColor = Color::LightYellow;
            }
        }

        if (topVisibleRow >= 0 && topVisibleRow < dataGridViewParameters->Rows->Count)
        {
            try
            {
                dataGridViewParameters->FirstDisplayedScrollingRowIndex = topVisibleRow;
            }
            catch (...) {}
        }

        if (selectedRow >= 0 && selectedRow < dataGridViewParameters->Rows->Count)
        {
            try
            {
                dataGridViewParameters->Rows[selectedRow]->Selected = true;

                if (dataGridViewParameters->Rows[selectedRow]->Cells->Count > 0)
                {
                    dataGridViewParameters->CurrentCell =
                        dataGridViewParameters->Rows[selectedRow]->Cells[0];
                }
            }
            catch (...) {}
        }

        if (firstDisplayedColumn >= 0 &&
            firstDisplayedColumn < dataGridViewParameters->Columns->Count)
        {
            try
            {
                dataGridViewParameters->FirstDisplayedScrollingColumnIndex = firstDisplayedColumn;
            }
            catch (...) {}
        }

        try
        {
            dataGridViewParameters->HorizontalScrollingOffset = horizontalOffset;
        }
        catch (...) {}
    }
    finally
    {
        dataGridViewParameters->ResumeLayout();
        isUpdatingTables = false;
    }
}

void ModbusMasterForm::CollapseRegistersPanel()
{
    groupBoxRegisters->Height = 475;
    dataGridViewRegisters->Visible = true;
    groupBoxRegisters->Text = L"Все регистры (32-битные значения)";
    isRegistersPanelExpanded = true;
    groupBoxParameters->Height = 170;
}

void ModbusMasterForm::ExpandRegistersPanel()
{
    groupBoxRegisters->Height = 475;
    dataGridViewRegisters->Visible = true;
    groupBoxRegisters->Text = L"Все регистры (32-битные значения)";
    isRegistersPanelExpanded = true;
    groupBoxParameters->Height = 170;
}

void ModbusMasterForm::UpdateLiveValuesPanel()
{
    if (allRegistersData != nullptr && allRegistersData->ContainsKey(257))
        textBoxLiveDensity->Text = allRegistersData[257]->Item1.ToString(L"F3") + L" кг/м³";
    else
        textBoxLiveDensity->Text = L"-";

    if (allRegistersData != nullptr && allRegistersData->ContainsKey(259))
        textBoxLiveTemperature->Text = allRegistersData[259]->Item1.ToString(L"F3") + L" °C";
    else
        textBoxLiveTemperature->Text = L"-";

    if (allRegistersData != nullptr && allRegistersData->ContainsKey(264))
        textBoxLiveUout->Text = allRegistersData[264]->Item1.ToString(L"F3") + L" В";
    else
        textBoxLiveUout->Text = L"-";
}

// ============================================================================
// Работа с изменениями регистров
// ============================================================================

void ModbusMasterForm::MarkRegisterAsChanged(int registerAddress, uint32_t newValue, float newFloatValue)
{
    changedRegistersValues[registerAddress] = newValue;
    changedRegistersFloats[registerAddress] = newFloatValue;
    buttonSendChanges->Enabled = true;
}

bool ModbusMasterForm::IsRegisterChanged(int registerAddress)
{
    return changedRegistersValues->ContainsKey(registerAddress);
}

void ModbusMasterForm::ClearChangedRegisters()
{
    changedRegistersValues->Clear();
    changedRegistersFloats->Clear();
    buttonSendChanges->Enabled = false;
}
void ModbusMasterForm::PauseAutoUpdateForEditing()
{
    if (isRegisterCellEditing)
        return;

    isRegisterCellEditing = true;

    if (isAutoUpdating)
    {
        resumeAutoUpdateAfterEdit = true;
        isAutoUpdating = false;
        updateTimer->Stop();

        labelAutoUpdateStatus->Text = L"Автообновление: пауза (редактирование)";
        buttonStartAutoUpdate->Enabled = true;
        buttonStopAutoUpdate->Enabled = false;
    }
    else
    {
        resumeAutoUpdateAfterEdit = false;
    }
}

void ModbusMasterForm::ResumeAutoUpdateAfterEditing()
{
    isRegisterCellEditing = false;

    if (pendingUiRefreshAfterEdit)
    {
        pendingUiRefreshAfterEdit = false;
        UpdateRegistersData();
    }

    if (resumeAutoUpdateAfterEdit && hSerial != INVALID_HANDLE_VALUE)
    {
        isAutoUpdating = true;
        updateTimer->Start();

        buttonStartAutoUpdate->Enabled = false;
        buttonStopAutoUpdate->Enabled = true;

        if (lastUpdateTime != DateTime())
        {
            labelAutoUpdateStatus->Text =
                String::Format(L"Автообновление: активно (последнее: {0:HH:mm:ss})", lastUpdateTime);
        }
        else
        {
            labelAutoUpdateStatus->Text = L"Автообновление: активно";
        }
    }

    resumeAutoUpdateAfterEdit = false;
}
void ModbusMasterForm::dataGridViewRegisters_CellBeginEdit(
    System::Object^ sender,
    System::Windows::Forms::DataGridViewCellCancelEventArgs^ e)
{
    if (e->RowIndex < 0)
        return;

    // Разрешаем паузу только при редактировании значений int32/float
    if (e->ColumnIndex == 2 || e->ColumnIndex == 3)
    {
        PauseAutoUpdateForEditing();
    }
}

void ModbusMasterForm::dataGridViewRegisters_CellEndEdit(
    System::Object^ sender,
    System::Windows::Forms::DataGridViewCellEventArgs^ e)
{
    if (e->RowIndex < 0)
        return;

    if (e->ColumnIndex == 2 || e->ColumnIndex == 3)
    {
        ResumeAutoUpdateAfterEditing();
    }
}
// ============================================================================
// Запись регистров
// ============================================================================

void ModbusMasterForm::WriteRegisters()
{
    if (isPollingInProgress)
    {
        LogMessage(L"Запись отменена: идёт опрос устройства.");
        return;
    }

    bool wasAutoUpdating = isAutoUpdating;

    if (isAutoUpdating)
    {
        isAutoUpdating = false;
        updateTimer->Stop();
    }

    if (pollTimer != nullptr)
        pollTimer->Stop();

    pollState = PollState::Idle;
    isPollingInProgress = false;

    if (hSerial == INVALID_HANDLE_VALUE) return;

    Monitor::Enter(portLock);

    try
    {
        LogMessage(String::Format(L"Начинаю запись {0} измененных регистров...",
            changedRegistersValues->Count));

        int successCount = 0;
        int failCount = 0;
        uint8_t slaveID = static_cast<uint8_t>(numericSlaveID->Value);

        for each (auto kvp in changedRegistersValues)
        {
            int registerAddress = kvp.Key;
            uint32_t newValue = kvp.Value;
            float newFloatValue = changedRegistersFloats[registerAddress];

            uint16_t startAddress = static_cast<uint16_t>(registerAddress);
            uint16_t registerCount = 2;
            std::vector<uint8_t> request;
            request.push_back(slaveID);
            request.push_back(0x10);  // Write Multiple Registers
            request.push_back((startAddress >> 8) & 0xFF);
            request.push_back(startAddress & 0xFF);
            request.push_back((registerCount >> 8) & 0xFF);
            request.push_back(registerCount & 0xFF);
            request.push_back(4);


            request.push_back(newValue & 0xFF);
            request.push_back((newValue >> 8) & 0xFF);
            request.push_back((newValue >> 16) & 0xFF);
            request.push_back((newValue >> 24) & 0xFF);

            uint16_t crc = CalculateCRC16(request.data(), request.size());
            request.push_back(crc & 0xFF);
            request.push_back((crc >> 8) & 0xFF);
            PurgeComm(hSerial, PURGE_RXCLEAR);
            DWORD bytesWritten;
            LogMessage(L"TX(0x10): " + BytesToHex(request.data(), (int)request.size()));
            if (!WriteFile(hSerial, request.data(), (DWORD)request.size(), &bytesWritten, NULL))
            {
                LogMessage(String::Format(L"Ошибка отправки запроса для регистра {0}", registerAddress));
                failCount++;
                continue;
            }

            Sleep(50);

            uint8_t response[256];
            DWORD totalBytesRead = 0;
            DWORD bytesRead = 0;
            DWORD startTime = GetTickCount();

            while ((GetTickCount() - startTime) < 300)
            {
                if (ReadFile(hSerial, response + totalBytesRead,
                    sizeof(response) - totalBytesRead, &bytesRead, NULL) && bytesRead > 0)
                {
                    totalBytesRead += bytesRead;
                    if (totalBytesRead >= 8 && response[1] == 0x10) break;
                }
                Sleep(5);
            }

            if (totalBytesRead >= 5 &&
                response[0] == slaveID &&
                response[1] == (0x10 | 0x80))
            {
                if (CheckCRC(response, 5))
                {
                    System::Byte exCode = response[2];
                    LogMessage(String::Format(
                        L"Ошибка записи регистра {0}: exception 0x{1:X2}",
                        registerAddress, exCode));
                }
                else
                {
                    LogMessage(String::Format(
                        L"Ошибка записи регистра {0}: битый exception response",
                        registerAddress));
                }

                failCount++;
            }
            else if (totalBytesRead >= 8 &&
                response[0] == slaveID &&
                response[1] == 0x10 &&
                CheckCRC(response, 8))
            {
                uint16_t respAddr =
                    ((uint16_t)response[2] << 8) | response[3];

                uint16_t respCount =
                    ((uint16_t)response[4] << 8) | response[5];

                if (respAddr == startAddress && respCount == registerCount)
                {
                    LogMessage(String::Format(
                        L"Регистр {0} успешно записан (значение: {1})",
                        registerAddress, newFloatValue));
                    successCount++;
                }
                else
                {
                    LogMessage(String::Format(
                        L"Ошибка записи регистра {0}: ответ не совпал по адресу/количеству",
                        registerAddress));
                    failCount++;
                }
            }
            else
            {
                LogMessage(String::Format(
                    L"Ошибка записи регистра {0}: нет корректного ответа",
                    registerAddress));
                failCount++;
            }
            System::UInt32 verifyRaw = 0;
            System::Int32 verifyInt = 0;
            float verifyFloat = 0.0f;

            if (ReadLogicalReg32(registerAddress, verifyRaw, verifyInt, verifyFloat))
            {
                if (verifyRaw == newValue)
                {
                    LogMessage(String::Format(
                        L"Проверка записи Reg{0}: OK, raw=0x{1:X8}",
                        registerAddress, verifyRaw));
                }
                else
                {
                    LogMessage(String::Format(
                        L"Проверка записи Reg{0}: MISMATCH, записали 0x{1:X8}, прочитали 0x{2:X8}",
                        registerAddress, newValue, verifyRaw));
                }
            }
            else
            {
                LogMessage(String::Format(
                    L"Проверка записи Reg{0}: не удалось перечитать",
                    registerAddress));
            }
            Sleep(20);
            if (totalBytesRead > 0)
            {
                LogMessage(L"RX(0x10): " + BytesToHex(response, totalBytesRead));
            }
        }

        LogMessage(String::Format(L"Запись завершена. Успешно: {0}, Ошибок: {1}",
            successCount, failCount));

    }
    finally
    {
        Monitor::Exit(portLock);
    }
    if (wasAutoUpdating)
    {
        isAutoUpdating = true;
        updateTimer->Start();
    }
}

String^ ModbusMasterForm::BytesToHex(const uint8_t* data, int length)
{
    System::Text::StringBuilder^ sb = gcnew System::Text::StringBuilder();

    for (int i = 0; i < length; i++)
    {
        sb->AppendFormat(L"{0:X2}", data[i]);
        if (i < length - 1)
            sb->Append(L" ");
    }

    return sb->ToString();
}

// ============================================================================
// Обработчики событий кнопок
// ============================================================================

void ModbusMasterForm::buttonRefresh_Click(System::Object^ sender, System::EventArgs^ e)
{
    RefreshPorts();
}

void ModbusMasterForm::buttonConnect_Click(System::Object^ sender, System::EventArgs^ e)
{
    ConnectToPort();
}

void ModbusMasterForm::buttonDisconnect_Click(System::Object^ sender, System::EventArgs^ e)
{
    DisconnectFromPort();
}

void ModbusMasterForm::buttonScanDevices_Click(System::Object^ sender, System::EventArgs^ e)
{
    ScanForDevices();
}

void ModbusMasterForm::buttonClearLog_Click(System::Object^ sender, System::EventArgs^ e)
{
    textBoxLog->Clear();
}

void ModbusMasterForm::buttonRefreshRegisters_Click(System::Object^ sender, EventArgs^ e)
{
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        MessageBox::Show(L"Порт не подключен!", L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    if (isPollingInProgress)
    {
        LogMessage(L"Опрос уже выполняется, подождите...");
        return;
    }

    StartPollingCycle();
}
void ModbusMasterForm::buttonStartAutoUpdate_Click(Object^ sender, EventArgs^ e)
{
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        MessageBox::Show(L"Порт не подключен!", L"Ошибка",
            MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    isAutoUpdating = true;
    updateTimer->Start();

    buttonStartAutoUpdate->Enabled = false;
    buttonStopAutoUpdate->Enabled = true;

    labelAutoUpdateStatus->Text = L"Автообновление: активно";

    if (!isPollingInProgress)
    {
        StartPollingCycle();
    }
}

void ModbusMasterForm::buttonStopAutoUpdate_Click(Object^ sender, EventArgs^ e)
{
    isAutoUpdating = false;
    updateTimer->Stop();

    if (pollTimer != nullptr)
    {
        pollTimer->Stop();
    }

    pollState = PollState::Idle;
    isPollingInProgress = false;

    buttonStartAutoUpdate->Enabled = true;
    buttonStopAutoUpdate->Enabled = false;
    labelAutoUpdateStatus->Text = L"Автообновление: выключено";
}

void ModbusMasterForm::buttonSendChanges_Click(System::Object^ sender, EventArgs^ e)
{
    if (changedRegistersValues->Count == 0)
    {
        MessageBox::Show(L"Нет изменений для отправки!", L"Информация",
            MessageBoxButtons::OK, MessageBoxIcon::Information);
        return;
    }

    if (hSerial != INVALID_HANDLE_VALUE)
    {
        WriteRegisters();
        ClearChangedRegisters();
        RefreshRegistersData();
    }
    else
    {
        MessageBox::Show(L"Порт не подключен!", L"Ошибка",
            MessageBoxButtons::OK, MessageBoxIcon::Warning);
    }
}

void ModbusMasterForm::buttonExport_Click(Object^ sender, EventArgs^ e)
{
    if (dataGridViewRegisters->Rows->Count == 0)
    {
        MessageBox::Show(L"Нет данных для экспорта!", L"Предупреждение", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    saveFileDialog->Filter = L"CSV files (*.csv)|*.csv|All files (*.*)|*.*";
    saveFileDialog->FilterIndex = 1;
    saveFileDialog->RestoreDirectory = true;
    saveFileDialog->FileName = L"registers_export.csv";

    if (saveFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
    {
        try
        {
            System::IO::StreamWriter^ sw = gcnew System::IO::StreamWriter(saveFileDialog->FileName);

            sw->WriteLine(L"=== ВСЕ РЕГИСТРЫ ===");
            for (int i = 0; i < dataGridViewRegisters->Columns->Count; i++)
            {
                sw->Write(dataGridViewRegisters->Columns[i]->HeaderText);
                if (i < dataGridViewRegisters->Columns->Count - 1) sw->Write(L";");
            }
            sw->WriteLine();

            for (int i = 0; i < dataGridViewRegisters->Rows->Count; i++)
            {
                for (int j = 0; j < dataGridViewRegisters->Columns->Count; j++)
                {
                    String^ cellValue = dataGridViewRegisters->Rows[i]->Cells[j]->Value != nullptr ?
                        dataGridViewRegisters->Rows[i]->Cells[j]->Value->ToString() : L"";
                    cellValue = cellValue->Replace(L"\"", L"\"\"");
                    if (cellValue->Contains(L";") || cellValue->Contains(L"\""))
                        sw->Write(L"\"" + cellValue + L"\"");
                    else
                        sw->Write(cellValue);
                    if (j < dataGridViewRegisters->Columns->Count - 1) sw->Write(L";");
                }
                sw->WriteLine();
            }

            sw->WriteLine();
            sw->WriteLine(L"=== КОНТРОЛИРУЕМЫЕ ПАРАМЕТРЫ ===");
            for (int i = 0; i < dataGridViewParameters->Columns->Count; i++)
            {
                sw->Write(dataGridViewParameters->Columns[i]->HeaderText);
                if (i < dataGridViewParameters->Columns->Count - 1) sw->Write(L";");
            }
            sw->WriteLine();

            for (int i = 0; i < dataGridViewParameters->Rows->Count; i++)
            {
                for (int j = 0; j < dataGridViewParameters->Columns->Count; j++)
                {
                    String^ cellValue = dataGridViewParameters->Rows[i]->Cells[j]->Value != nullptr ?
                        dataGridViewParameters->Rows[i]->Cells[j]->Value->ToString() : L"";
                    cellValue = cellValue->Replace(L"\"", L"\"\"");
                    if (cellValue->Contains(L";") || cellValue->Contains(L"\""))
                        sw->Write(L"\"" + cellValue + L"\"");
                    else
                        sw->Write(cellValue);
                    if (j < dataGridViewParameters->Columns->Count - 1) sw->Write(L";");
                }
                sw->WriteLine();
            }

            sw->Close();
            MessageBox::Show(L"Данные успешно экспортированы в " + saveFileDialog->FileName, L"Экспорт завершен",
                MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        catch (Exception^ ex)
        {
            MessageBox::Show(L"Ошибка при экспорте: " + ex->Message, L"Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

// ============================================================================
// Обработчики таблиц
// ============================================================================

void ModbusMasterForm::dataGridViewRegisters_CellValueChanged(Object^ sender, DataGridViewCellEventArgs^ e)
{
    if (e->RowIndex >= 0 && e->ColumnIndex >= 2 && e->ColumnIndex <= 3)
    {
        // Получаем физический адрес из первой колонки
        String^ addrStr = dataGridViewRegisters->Rows[e->RowIndex]->Cells[0]->Value->ToString();
        if (addrStr->StartsWith(L"Reg"))
        {
            int physicalAddress = Int32::Parse(addrStr->Substring(3));

            String^ newValueStr = dataGridViewRegisters->Rows[e->RowIndex]->Cells[e->ColumnIndex]->Value->ToString();

            try
            {
                int32_t newIntValue;
                float newFloatValue;
                uint32_t newUintValue;

                if (e->ColumnIndex == 2)  // int32 колонка
                {
                    newIntValue = Convert::ToInt32(newValueStr);
                    newUintValue = static_cast<uint32_t>(newIntValue);
                    newFloatValue = ConvertToFloat(newUintValue);
                    dataGridViewRegisters->Rows[e->RowIndex]->Cells[3]->Value = newFloatValue.ToString(L"F6");
                }
                else  // float колонка
                {
                    newFloatValue = Convert::ToSingle(newValueStr);
newUintValue = FloatToRaw(newFloatValue);
newIntValue = static_cast<int32_t>(newUintValue);
                    dataGridViewRegisters->Rows[e->RowIndex]->Cells[2]->Value = newIntValue.ToString();
                }

                dataGridViewRegisters->Rows[e->RowIndex]->Cells[1]->Value = String::Format(L"0x{0:X8}", newUintValue);

                MarkRegisterAsChanged(physicalAddress, newUintValue, newFloatValue);
                dataGridViewRegisters->Rows[e->RowIndex]->DefaultCellStyle->BackColor = Color::LightYellow;
                UpdateParametersTable();
            }
            catch (Exception^ ex)
            {
                MessageBox::Show(L"Ошибка ввода значения: " + ex->Message, L"Ошибка",
                    MessageBoxButtons::OK, MessageBoxIcon::Warning);
                UpdateFullRegistersTable();
            }
        }
    }
}

void ModbusMasterForm::dataGridViewParameters_CellDoubleClick(Object^ sender, DataGridViewCellEventArgs^ e)
{
    if (e->RowIndex >= 0 && e->RowIndex < paramNames->Count)
    {
        int physicalAddress = paramAddresses[e->RowIndex];

        for (int i = 0; i < dataGridViewRegisters->Rows->Count; i++)
        {
            DataGridViewRow^ row = dataGridViewRegisters->Rows[i];
            if (row->Cells[0]->Value != nullptr)
            {
                String^ addrStr = row->Cells[0]->Value->ToString();
                if (addrStr == String::Format(L"Reg{0}", physicalAddress))
                {
                    dataGridViewRegisters->FirstDisplayedScrollingRowIndex = i;
                    dataGridViewRegisters->Rows[i]->Selected = true;
                    dataGridViewRegisters->CurrentCell = dataGridViewRegisters->Rows[i]->Cells[0];
                    break;
                }
            }
        }
    }
}

// ============================================================================
// Таймер автообновления
// ============================================================================

void ModbusMasterForm::OnTimerTick(Object^ sender, EventArgs^ e)
{
    AutoUpdateData();
}

void ModbusMasterForm::AutoUpdateData()
{
    if (!isAutoUpdating) return;
    if (hSerial == INVALID_HANDLE_VALUE) return;
    if (isPollingInProgress) return;
    if (isRegisterCellEditing) return;

    StartPollingCycle();
}

// ============================================================================
// Логирование
// ============================================================================

void ModbusMasterForm::LogMessage(System::String^ message)
{
    textBoxLog->AppendText(DateTime::Now.ToString(L"HH:mm:ss") + L" - " + message + L"\r\n");
    textBoxLog->SelectionStart = textBoxLog->TextLength;
    textBoxLog->ScrollToCaret();
}

void ModbusMasterForm::LogMessage(const std::wstring& message)
{
    LogMessage(gcnew String(message.c_str()));
}

System::String^ ModbusMasterForm::StringToWString(const std::string& str)
{
    return gcnew String(str.c_str());
}

// ============================================================================
// Точка входа
// ============================================================================

[STAThreadAttribute]
int main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    try
    {
        Application::Run(gcnew ModbusMasterForm());
    }
    catch (Exception^ ex)
    {
        MessageBox::Show("Ошибка: " + ex->Message, "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }

    return 0;
}