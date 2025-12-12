#pragma once

#include "RailwayStation.h"
#include "DatabaseManager.h"

namespace RailwaySystem {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class MainForm : public Form {
    private:
        RailwayStation* station;
        DatabaseManager^ databaseManager;
        System::Windows::Forms::DataGridView^ dataGridView1;
        System::Windows::Forms::Button^ btnAdd;
        System::Windows::Forms::Button^ btnEdit;
        System::Windows::Forms::Button^ btnDelete;
        System::Windows::Forms::Button^ btnLoad;
        System::Windows::Forms::Button^ btnSave;
        System::Windows::Forms::Button^ btnSortDest;
        System::Windows::Forms::Button^ btnSortPrice;
        System::Windows::Forms::Button^ btnStats;
        System::Windows::Forms::Button^ btnFindMin;
        System::Windows::Forms::Button^ btnImportDB;
        System::Windows::Forms::Button^ btnExportDB;
        System::Windows::Forms::Button^ btnRefreshDB;
        System::Windows::Forms::OpenFileDialog^ openFileDialog1;
        System::Windows::Forms::SaveFileDialog^ saveFileDialog1;
        System::Windows::Forms::Label^ label1;
        System::Windows::Forms::Label^ labelStatus;
        System::ComponentModel::Container^ components;

        void InitializeComponent(void);
        void UpdateDataGrid();
        void LoadFromDatabase();
        std::string SystemStringToStdString(String^ str);
        String^ StdStringToSystemString(const std::string& str);
        void ShowDatabaseStatus();

    public:
        MainForm(void);
        ~MainForm();

    protected:
        !MainForm();

    private:
        System::Void btnAdd_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnEdit_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnDelete_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnLoad_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnSave_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnSortDest_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnSortPrice_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnStats_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnFindMin_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnImportDB_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnExportDB_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnRefreshDB_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void MainForm_Load(System::Object^ sender, System::EventArgs^ e);
    };
}