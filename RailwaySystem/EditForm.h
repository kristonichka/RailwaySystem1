#pragma once

namespace RailwaySystem {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class EditForm : public Form {
    private:
        System::Windows::Forms::TextBox^ txtDestination;
        System::Windows::Forms::TextBox^ txtPrice;
        System::Windows::Forms::TextBox^ txtDiscount;
        System::Windows::Forms::Label^ label1;
        System::Windows::Forms::Label^ label2;
        System::Windows::Forms::Label^ label3;
        System::Windows::Forms::Button^ btnSave;
        System::Windows::Forms::Button^ btnCancel;
        System::Windows::Forms::CheckBox^ checkBoxDiscount;
        System::ComponentModel::Container^ components;

        void InitializeComponent(void);
        bool ValidateInput();
        void EnableDiscountControls(bool enabled);

    public:
        // Конструктор для добавления нового тарифа
        EditForm(void);

        // Конструктор для редактирования существующего тарифа (перегрузка)
        EditForm(String^ destination, double price, double discount);

        ~EditForm();

    protected:
        !EditForm();

    public:
        property String^ Destination {
            String^ get() { return txtDestination->Text; }
        }

        property double Price {
            double get() {
                double result = 0.0;
                Double::TryParse(txtPrice->Text, result);
                return result;
            }
        }

        property double Discount {
            double get() {
                double result = 0.0;
                Double::TryParse(txtDiscount->Text, result);
                return result;
            }
        }

        property bool HasDiscount {
            bool get() { return checkBoxDiscount->Checked; }
        }

    private:
        System::Void checkBoxDiscount_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
        System::Void btnSave_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnCancel_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void txtPrice_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e);
        System::Void txtDiscount_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e);
        System::Void EditForm_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);
    };
}