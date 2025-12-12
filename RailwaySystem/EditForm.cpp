#include "EditForm.h"
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace RailwaySystem;
using namespace msclr::interop;

// Конструктор для добавления нового тарифа
EditForm::EditForm(void) {
    InitializeComponent();
    checkBoxDiscount->Checked = false;
    EnableDiscountControls(false);
    this->Text = "Добавить тариф";
}

// Конструктор для редактирования существующего тарифа
EditForm::EditForm(String^ destination, double price, double discount) {
    InitializeComponent();

    txtDestination->Text = destination;
    txtPrice->Text = price.ToString("F2");

    if (discount > 0) {
        checkBoxDiscount->Checked = true;
        txtDiscount->Text = discount.ToString("F1");
        EnableDiscountControls(true);
    }
    else {
        checkBoxDiscount->Checked = false;
        EnableDiscountControls(false);
    }

    this->Text = "Редактировать тариф";
}

// Деструктор
EditForm::~EditForm() {
    if (components) {
        delete components;
    }
}

// Финализатор
EditForm::!EditForm() {
    // В данном случае нет неуправляемых ресурсов для очистки
}

// Инициализация компонентов формы
void EditForm::InitializeComponent(void) {
    this->components = gcnew System::ComponentModel::Container();

    // Создание Label для "Направление"
    this->label1 = gcnew Label();
    this->label1->Text = "Направление (город):";
    this->label1->Location = Point(20, 20);
    this->label1->Size = Drawing::Size(150, 20);

    // Создание TextBox для направления
    this->txtDestination = gcnew TextBox();
    this->txtDestination->Location = Point(180, 20);
    this->txtDestination->Size = Drawing::Size(200, 20);
    this->txtDestination->MaxLength = 50;

    // Создание Label для "Цена"
    this->label2 = gcnew Label();
    this->label2->Text = "Базовая цена (руб.):";
    this->label2->Location = Point(20, 60);
    this->label2->Size = Drawing::Size(150, 20);

    // Создание TextBox для цены
    this->txtPrice = gcnew TextBox();
    this->txtPrice->Location = Point(180, 60);
    this->txtPrice->Size = Drawing::Size(100, 20);
    this->txtPrice->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &EditForm::txtPrice_KeyPress);

    // Создание CheckBox для скидки
    this->checkBoxDiscount = gcnew CheckBox();
    this->checkBoxDiscount->Text = "Тариф со скидкой";
    this->checkBoxDiscount->Location = Point(20, 100);
    this->checkBoxDiscount->Size = Drawing::Size(150, 20);
    this->checkBoxDiscount->CheckedChanged += gcnew System::EventHandler(this, &EditForm::checkBoxDiscount_CheckedChanged);

    // Создание Label для "Скидка"
    this->label3 = gcnew Label();
    this->label3->Text = "Скидка (%):";
    this->label3->Location = Point(180, 100);
    this->label3->Size = Drawing::Size(80, 20);

    // Создание TextBox для скидки
    this->txtDiscount = gcnew TextBox();
    this->txtDiscount->Location = Point(260, 100);
    this->txtDiscount->Size = Drawing::Size(80, 20);
    this->txtDiscount->Enabled = false;
    this->txtDiscount->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &EditForm::txtDiscount_KeyPress);

    // Создание кнопки "Сохранить"
    this->btnSave = gcnew Button();
    this->btnSave->Text = "Сохранить";
    this->btnSave->Location = Point(100, 150);
    this->btnSave->Size = Drawing::Size(100, 30);
    this->btnSave->Click += gcnew System::EventHandler(this, &EditForm::btnSave_Click);

    // Создание кнопки "Отмена"
    this->btnCancel = gcnew Button();
    this->btnCancel->Text = "Отмена";
    this->btnCancel->Location = Point(220, 150);
    this->btnCancel->Size = Drawing::Size(100, 30);
    this->btnCancel->DialogResult = Windows::Forms::DialogResult::Cancel;
    this->btnCancel->Click += gcnew System::EventHandler(this, &EditForm::btnCancel_Click);

    // Подписка на событие FormClosing
    this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &EditForm::EditForm_FormClosing);

    // Настройка формы
    this->Text = "Редактирование тарифа";
    this->Size = Drawing::Size(400, 230);
    this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->StartPosition = FormStartPosition::CenterParent;
    this->MaximizeBox = false;
    this->MinimizeBox = false;
    this->AcceptButton = btnSave;
    this->CancelButton = btnCancel;

    // Добавление элементов на форму
    this->Controls->Add(this->label1);
    this->Controls->Add(this->txtDestination);
    this->Controls->Add(this->label2);
    this->Controls->Add(this->txtPrice);
    this->Controls->Add(this->checkBoxDiscount);
    this->Controls->Add(this->label3);
    this->Controls->Add(this->txtDiscount);
    this->Controls->Add(this->btnSave);
    this->Controls->Add(this->btnCancel);
}

// Включение/выключение элементов управления скидкой
void EditForm::EnableDiscountControls(bool enabled) {
    label3->Enabled = enabled;
    txtDiscount->Enabled = enabled;
}

// Проверка ввода
bool EditForm::ValidateInput() {
    // Проверка направления
    if (String::IsNullOrWhiteSpace(txtDestination->Text)) {
        MessageBox::Show("Введите направление (город)!",
            "Ошибка ввода",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
        txtDestination->Focus();
        return false;
    }

    // Проверка названия города (только буквы, пробелы, дефисы)
    String^ cityName = txtDestination->Text->Trim();
    bool hasLetter = false;
    bool isValid = true;

    for (int i = 0; i < cityName->Length; i++) {
        wchar_t ch = cityName[i];

        if (Char::IsLetter(ch)) {
            hasLetter = true;
        }
        else if (!Char::IsWhiteSpace(ch) && ch != '-' && ch != '.') {
            isValid = false;
            break;
        }
    }

    if (!isValid) {
        MessageBox::Show("Название города может содержать только буквы, пробелы и дефисы!",
            "Ошибка ввода",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
        txtDestination->Focus();
        txtDestination->SelectAll();
        return false;
    }

    if (!hasLetter) {
        MessageBox::Show("Название города должно содержать хотя бы одну букву!",
            "Ошибка ввода",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
        txtDestination->Focus();
        txtDestination->SelectAll();
        return false;
    }

    // Проверка цены
    double price;
    if (!Double::TryParse(txtPrice->Text, price) || price <= 0 || price > 50000) {
        MessageBox::Show("Цена должна быть числом от 0.01 до 50000!",
            "Ошибка ввода",
            MessageBoxButtons::OK,
            MessageBoxIcon::Error);
        txtPrice->Focus();
        txtPrice->SelectAll();
        return false;
    }

    // Проверка скидки, если включена
    if (checkBoxDiscount->Checked) {
        double discount;
        if (!Double::TryParse(txtDiscount->Text, discount) || discount < 1 || discount > 99) {
            MessageBox::Show("Скидка должна быть числом от 1 до 99%!",
                "Ошибка ввода",
                MessageBoxButtons::OK,
                MessageBoxIcon::Error);
            txtDiscount->Focus();
            txtDiscount->SelectAll();
            return false;
        }
    }

    return true;
}

// Обработчик изменения состояния CheckBox скидки
System::Void EditForm::checkBoxDiscount_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
    EnableDiscountControls(checkBoxDiscount->Checked);
    if (checkBoxDiscount->Checked) {
        if (String::IsNullOrWhiteSpace(txtDiscount->Text)) {
            txtDiscount->Text = "10"; // Значение по умолчанию
        }
        txtDiscount->SelectAll();
        txtDiscount->Focus();
    }
    else {
        txtDiscount->Text = "";
    }
}

// Обработчик нажатия кнопки "Сохранить" - ГЛАВНОЕ ИСПРАВЛЕНИЕ
System::Void EditForm::btnSave_Click(System::Object^ sender, System::EventArgs^ e) {
    if (ValidateInput()) {
        this->DialogResult = Windows::Forms::DialogResult::OK;
        this->Close();
    }
    // Если валидация не прошла, НЕ закрываем форму
    // Пользователь остается в форме для исправления ошибок
}

// Обработчик нажатия кнопки "Отмена"
System::Void EditForm::btnCancel_Click(System::Object^ sender, System::EventArgs^ e) {
    this->DialogResult = Windows::Forms::DialogResult::Cancel;
    this->Close();
}

// Обработчик ввода для текстового поля цены (только цифры и точка)
System::Void EditForm::txtPrice_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {
    // Разрешаем: цифры, точка, запятая, Backspace
    if (!Char::IsDigit(e->KeyChar) &&
        e->KeyChar != '.' &&
        e->KeyChar != ',' &&
        e->KeyChar != (wchar_t)Keys::Back) {
        e->Handled = true;
    }

    // Заменяем запятую на точку
    if (e->KeyChar == ',') {
        e->KeyChar = '.';
    }

    // Проверяем, что точка только одна
    TextBox^ textBox = (TextBox^)sender;
    String^ text = textBox->Text;
    if ((e->KeyChar == '.' || e->KeyChar == ',') && text->Contains(".")) {
        e->Handled = true;
    }
}

// Обработчик ввода для текстового поля скидки (только цифры и точка)
System::Void EditForm::txtDiscount_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e) {
    // Разрешаем: цифры, точка, Backspace
    if (!Char::IsDigit(e->KeyChar) &&
        e->KeyChar != '.' &&
        e->KeyChar != (wchar_t)Keys::Back) {
        e->Handled = true;
    }

    // Проверяем, что точка только одна
    TextBox^ textBox = (TextBox^)sender;
    String^ text = textBox->Text;
    if (e->KeyChar == '.' && text->Contains(".")) {
        e->Handled = true;
    }
}

// Обработчик закрытия формы - дополнительная проверка
System::Void EditForm::EditForm_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
    // Если пользователь пытается закрыть форму через крестик или Alt+F4
    if (this->DialogResult == Windows::Forms::DialogResult::None) {
        this->DialogResult = Windows::Forms::DialogResult::Cancel;
    }
}