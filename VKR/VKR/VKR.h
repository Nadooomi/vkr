#pragma once

// Неуправляемые заголовки Windows
#include <windows.h>
#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fcntl.h>
#include <limits>
#include <iostream>

// Заголовок для маршалинга в C++/CLR
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Collections::Generic;

/// <summary>
/// Класс главной формы
/// </summary>
public ref class ModbusMasterForm : public System::Windows::Forms::Form
{
public:
    ModbusMasterForm(void);

protected:
    ~ModbusMasterForm();

private:
    // Элементы управления для порта
    System::Windows::Forms::ComboBox^ comboBoxPorts;
    System::Windows::Forms::Button^ buttonRefresh;
    System::Windows::Forms::Button^ buttonConnect;
    System::Windows::Forms::Button^ buttonDisconnect;
    System::Windows::Forms::Button^ buttonScanDevices;
    System::Windows::Forms::TextBox^ textBoxLog;
    System::Windows::Forms::Label^ labelStatus;
    System::Windows::Forms::GroupBox^ groupBoxPort;
    System::Windows::Forms::CheckBox^ checkBoxShowAll;
    System::Windows::Forms::Button^ buttonClearLog;
    System::Windows::Forms::NumericUpDown^ numericSlaveID;
    System::Windows::Forms::Label^ labelSlaveID;
    System::Windows::Forms::Label^ labelFoundDevices;
    System::Windows::Forms::TextBox^ textBoxScanResult;
    System::Windows::Forms::Label^ labelModelCaption;
    System::Windows::Forms::Label^ labelModelValue;
    System::Windows::Forms::Label^ labelSerialCaption;
    System::Windows::Forms::Label^ labelSerialValue;
    System::Windows::Forms::Label^ labelTypeCaption;
    System::Windows::Forms::Label^ labelTypeValue;
    System::Windows::Forms::Label^ labelSpeedCaption;
    System::Windows::Forms::Label^ labelSpeedValue;

    // Элементы управления для регистров
    System::Windows::Forms::DataGridView^ dataGridViewRegisters;
    System::Windows::Forms::DataGridView^ dataGridViewParameters;
    System::Windows::Forms::Button^ buttonExport;
    System::Windows::Forms::Button^ buttonRefreshRegisters;
    System::Windows::Forms::Button^ buttonStartAutoUpdate;
    System::Windows::Forms::Button^ buttonStopAutoUpdate;
    System::Windows::Forms::Button^ buttonSendChanges;
    System::Windows::Forms::Label^ labelAutoUpdateStatus;
    System::Windows::Forms::GroupBox^ groupBoxRegisters;
    System::Windows::Forms::GroupBox^ groupBoxParameters;
    System::Windows::Forms::SaveFileDialog^ saveFileDialog;

    // Приватные поля
    HANDLE hSerial;
    List<String^>^ availablePorts;

    // Фиксированные параметры порта для RS-485
    DWORD baudRate;
    BYTE byteSize;
    BYTE stopBits;
    BYTE parity;

    // Константы
    const int MIN_SLAVE_ID = 1;
    const int MAX_SLAVE_ID = 20;
    const int FIRST_REGISTER_ADDRESS = 0;      // Начальный адрес для сканирования
    const int TOTAL_32BIT_VALUES = 500;        // Количество 32-битных значений

    // Хранилище данных регистров (КЛЮЧ = физический адрес 16-битного регистра)
    Dictionary<int, Tuple<float, int32_t>^>^ allRegistersData;
    Dictionary<int, System::UInt32>^ allRegistersRaw;

    // Данные для контролируемых параметров (фиксированные)
    List<String^>^ paramNames;
    List<int>^ paramAddresses;
    List<String^>^ paramUnits;

    // Измененные регистры
    Dictionary<int, uint32_t>^ changedRegistersValues;
    Dictionary<int, float>^ changedRegistersFloats;

    // Таймер для автообновления
 // Таймер для автообновления
    System::Windows::Forms::Timer^ updateTimer;
    System::Windows::Forms::Timer^ pollTimer;

    enum class PollState
    {
        Idle,
        SendBatch,
        WaitBatch
    };

    PollState pollState;
    int pollStartAddress;
    int pollCurrentBatchSize;
    DWORD pollBatchStartTick;
    DWORD pollTotalBytesRead;
    array<System::Byte>^ pollResponseBuffer;

    bool isAutoUpdating;
    bool isRegistersPanelExpanded;
    DateTime lastUpdateTime;
    int originalFormHeight;
    int originalRegistersGroupHeight;

    // Флаги синхронизации
    bool isUpdatingTables;
    bool isPollingInProgress;
    bool isRegisterCellEditing;
    bool resumeAutoUpdateAfterEdit;
    bool pendingUiRefreshAfterEdit;
    bool pendingTablesRefresh;
    bool isInternalCellUpdate;
    System::Object^ portLock;
    System::Windows::Forms::GroupBox^ groupBoxLiveValues;
    System::Windows::Forms::Label^ labelLiveDensityCaption;
    System::Windows::Forms::Label^ labelLiveTemperatureCaption;
    System::Windows::Forms::Label^ labelLiveUoutCaption;

    System::Windows::Forms::TextBox^ textBoxLiveDensity;
    System::Windows::Forms::TextBox^ textBoxLiveTemperature;
    System::Windows::Forms::TextBox^ textBoxLiveUout;
    System::ComponentModel::Container^ components;

    void InitializeComponent();
    void InitializeCustomComponent();
    void InitializeParameters();
    void InitializeTimer();
    void InitializeRegistersGrid();
    void InitializeParametersGrid();
    void StartPollingCycle();
    void SendPollBatch();
    void ReadPollBatch();
    void FinishPollingCycle();
    void OnPollTimerTick(Object^ sender, EventArgs^ e);
    void RefreshPorts();
    void ConnectToPort();
    bool ConfigurePort();
    void DisconnectFromPort();
    void ClosePort();
    uint16_t CalculateCRC16(const uint8_t* data, size_t length);
    bool CheckCRC(const uint8_t* response, size_t length);
    void ScanForDevices();
    void RefreshRegistersData();
    void ScanAllRegisters();
    void WriteRegisters();
    void UpdateRegistersData();
    void UpdateFullRegistersTable();
    void UpdateParametersTable();
    void CollapseRegistersPanel();
    void ExpandRegistersPanel();
    void LogMessage(System::String^ message);
    void LogMessage(const std::wstring& message);
    System::String^ StringToWString(const std::string& str);
    System::String^ BytesToHex(const uint8_t* data, int length);
    void OnTimerTick(Object^ sender, EventArgs^ e);
    void AutoUpdateData();
    void MarkRegisterAsChanged(int registerAddress, uint32_t newValue, float newFloatValue);
    bool IsRegisterChanged(int registerAddress);
    void ClearChangedRegisters();
    // Вспомогательные методы
    float ConvertToFloat(uint32_t value);
    int32_t ConvertToInt32(uint32_t value);
    uint32_t FloatToRaw(float value);
    bool ReadLogicalReg32(int logicalReg, System::UInt32% rawValue, System::Int32% intValue, float% floatValue);
    void UpdateDeviceInfoLabels();
    void ClearDeviceInfoLabels();
    float ConvertToFloatSwappedWords(uint32_t value);
    bool ReadLogicalReg32WithFunction(int logicalReg, System::Byte functionCode,
        System::UInt32% rawValue, System::Int32% intValue, float% floatValue);
    void DebugTemperatureProbe();

    // Обработчики событий
    void InitializeLiveValuesPanel();
    void UpdateLiveValuesPanel();
    void Form1_Load(System::Object^ sender, System::EventArgs^ e);
    void buttonRefresh_Click(System::Object^ sender, System::EventArgs^ e);
    void buttonConnect_Click(System::Object^ sender, System::EventArgs^ e);
    void buttonDisconnect_Click(System::Object^ sender, System::EventArgs^ e);
    void buttonScanDevices_Click(System::Object^ sender, System::EventArgs^ e);
    void buttonClearLog_Click(System::Object^ sender, System::EventArgs^ e);
    void buttonExport_Click(System::Object^ sender, EventArgs^ e);
    void buttonRefreshRegisters_Click(System::Object^ sender, EventArgs^ e);
    void buttonStartAutoUpdate_Click(System::Object^ sender, EventArgs^ e);
    void buttonStopAutoUpdate_Click(System::Object^ sender, EventArgs^ e);
    void buttonSendChanges_Click(System::Object^ sender, EventArgs^ e);
    void dataGridViewRegisters_CellValueChanged(Object^ sender, DataGridViewCellEventArgs^ e);
    void dataGridViewParameters_CellDoubleClick(Object^ sender, DataGridViewCellEventArgs^ e);
    void dataGridViewRegisters_CellBeginEdit(System::Object^ sender, System::Windows::Forms::DataGridViewCellCancelEventArgs^ e);
    void dataGridViewRegisters_CellEndEdit(System::Object^ sender, System::Windows::Forms::DataGridViewCellEventArgs^ e);
    void PauseAutoUpdateForEditing();
    void ResumeAutoUpdateAfterEditing();
private: System::Void labelSlaveID_Click(System::Object^ sender, System::EventArgs^ e) {
}
};
