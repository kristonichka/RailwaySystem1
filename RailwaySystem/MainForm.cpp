#include "MainForm.h"
#include "EditForm.h"
#include <string>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace RailwaySystem;

// Вспомогательные методы преобразования строк БЕЗ marshal_as
std::string MainForm::SystemStringToStdString(String^ str) {
    if (str == nullptr) return "";

    std::string result;
    for each (wchar_t wch in str) {
        result.push_back(static_cast<char>(wch));
    }
    return result;
}

String^ MainForm::StdStringToSystemString(const std::string& str) {
    return gcnew String(str.c_str());
}

// Конструктор
MainForm::MainForm(void) {
    InitializeComponent();
    station = new RailwayStation();
    databaseManager = gcnew DatabaseManager();

    // Пробуем подключиться к базе данных
    if (databaseManager->Connect()) {
        LoadFromDatabase();
    }
    else {
        // Если не удалось подключиться, работаем в локальном режиме
        try {
            station->addRegularTariff("Саратов", 2500.0);
            station->addRegularTariff("Калиниград", 1800.0);
            station->addDiscountTariff("Москва", 1200.0, 15.0);
            station->addDiscountTariff("Хельсинки", 3500.0, 20.0);
            UpdateDataGrid();
        }
        catch (const std::exception& e) {
            std::string errorMsg = e.what();
            MessageBox::Show(gcnew String(errorMsg.c_str()), "Ошибка инициализации");
        }
    }

    ShowDatabaseStatus();
}

// Деструктор
MainForm::~MainForm() {
    this->!MainForm();
    if (components) {
        delete components;
    }
}

// Финализатор
MainForm::!MainForm() {
    if (station != nullptr) {
        delete station;
        station = nullptr;
    }
    if (databaseManager != nullptr) {
        databaseManager->Disconnect();
    }
}

// Инициализация компонентов формы
void MainForm::InitializeComponent(void) {
    this->components = gcnew System::ComponentModel::Container();
    this->Size = System::Drawing::Size(850, 550);
    this->Text = "Управление тарифами железнодорожного вокзала";
    this->StartPosition = FormStartPosition::CenterScreen;

    // Label для заголовка
    this->label1 = gcnew Label();
    this->label1->Text = "Список тарифов:";
    this->label1->Location = System::Drawing::Point(12, 20);
    this->label1->Size = System::Drawing::Size(150, 20);
    this->label1->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 10, FontStyle::Bold);

    // Label для статуса БД
    this->labelStatus = gcnew Label();
    this->labelStatus->Location = System::Drawing::Point(12, 450);
    this->labelStatus->Size = System::Drawing::Size(800, 40);
    this->labelStatus->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->labelStatus->ForeColor = Color::DarkBlue;

    // DataGridView
    this->dataGridView1 = gcnew DataGridView();
    this->dataGridView1->Location = System::Drawing::Point(12, 50);
    this->dataGridView1->Size = System::Drawing::Size(810, 250);
    this->dataGridView1->ReadOnly = true;
    this->dataGridView1->AllowUserToAddRows = false;
    this->dataGridView1->AllowUserToDeleteRows = false;
    this->dataGridView1->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
    this->dataGridView1->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;

    // Кнопки основной функциональности
    this->btnAdd = gcnew Button();
    this->btnAdd->Text = "Добавить";
    this->btnAdd->Location = System::Drawing::Point(12, 310);
    this->btnAdd->Size = System::Drawing::Size(120, 35);
    this->btnAdd->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnAdd->Click += gcnew System::EventHandler(this, &MainForm::btnAdd_Click);

    this->btnEdit = gcnew Button();
    this->btnEdit->Text = "Редактировать";
    this->btnEdit->Location = System::Drawing::Point(142, 310);
    this->btnEdit->Size = System::Drawing::Size(120, 35);
    this->btnEdit->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnEdit->Click += gcnew System::EventHandler(this, &MainForm::btnEdit_Click);

    this->btnDelete = gcnew Button();
    this->btnDelete->Text = "Удалить";
    this->btnDelete->Location = System::Drawing::Point(272, 310);
    this->btnDelete->Size = System::Drawing::Size(120, 35);
    this->btnDelete->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnDelete->Click += gcnew System::EventHandler(this, &MainForm::btnDelete_Click);

    this->btnSortDest = gcnew Button();
    this->btnSortDest->Text = "Сортировать по городу";
    this->btnSortDest->Location = System::Drawing::Point(402, 310);
    this->btnSortDest->Size = System::Drawing::Size(140, 45);
    this->btnSortDest->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnSortDest->Click += gcnew System::EventHandler(this, &MainForm::btnSortDest_Click);

    this->btnSortPrice = gcnew Button();
    this->btnSortPrice->Text = "Сортировать по цене";
    this->btnSortPrice->Location = System::Drawing::Point(552, 310);
    this->btnSortPrice->Size = System::Drawing::Size(140, 45);
    this->btnSortPrice->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnSortPrice->Click += gcnew System::EventHandler(this, &MainForm::btnSortPrice_Click);

    // Кнопки работы с файлами
    this->btnLoad = gcnew Button();
    this->btnLoad->Text = "Загрузить из файла";
    this->btnLoad->Location = System::Drawing::Point(12, 355);
    this->btnLoad->Size = System::Drawing::Size(120, 45);
    this->btnLoad->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnLoad->Click += gcnew System::EventHandler(this, &MainForm::btnLoad_Click);

    this->btnSave = gcnew Button();
    this->btnSave->Text = "Сохранить в файл";
    this->btnSave->Location = System::Drawing::Point(142, 355);
    this->btnSave->Size = System::Drawing::Size(120, 45);
    this->btnSave->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnSave->Click += gcnew System::EventHandler(this, &MainForm::btnSave_Click);

    // Кнопки работы с БД
    this->btnImportDB = gcnew Button();
    this->btnImportDB->Text = "Импорт в БД";
    this->btnImportDB->Location = System::Drawing::Point(272, 355);
    this->btnImportDB->Size = System::Drawing::Size(120, 35);
    this->btnImportDB->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnImportDB->Click += gcnew System::EventHandler(this, &MainForm::btnImportDB_Click);

    this->btnExportDB = gcnew Button();
    this->btnExportDB->Text = "Экспорт из БД";
    this->btnExportDB->Location = System::Drawing::Point(402, 355);
    this->btnExportDB->Size = System::Drawing::Size(120, 35);
    this->btnExportDB->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnExportDB->Click += gcnew System::EventHandler(this, &MainForm::btnExportDB_Click);

    this->btnRefreshDB = gcnew Button();
    this->btnRefreshDB->Text = "Обновить из БД";
    this->btnRefreshDB->Location = System::Drawing::Point(532, 355);
    this->btnRefreshDB->Size = System::Drawing::Size(120, 35);
    this->btnRefreshDB->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnRefreshDB->Click += gcnew System::EventHandler(this, &MainForm::btnRefreshDB_Click);

    // Кнопки анализа данных
    this->btnStats = gcnew Button();
    this->btnStats->Text = "Статистика";
    this->btnStats->Location = System::Drawing::Point(12, 400);
    this->btnStats->Size = System::Drawing::Size(120, 35);
    this->btnStats->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnStats->Click += gcnew System::EventHandler(this, &MainForm::btnStats_Click);

    this->btnFindMin = gcnew Button();
    this->btnFindMin->Text = "Минимальная цена";
    this->btnFindMin->Location = System::Drawing::Point(142, 400);
    this->btnFindMin->Size = System::Drawing::Size(140, 35);
    this->btnFindMin->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 9);
    this->btnFindMin->Click += gcnew System::EventHandler(this, &MainForm::btnFindMin_Click);

    // Диалоговые окна
    this->openFileDialog1 = gcnew OpenFileDialog();
    this->openFileDialog1->Filter = "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";
    this->openFileDialog1->Title = "Выберите файл для загрузки";

    this->saveFileDialog1 = gcnew SaveFileDialog();
    this->saveFileDialog1->Filter = "Текстовые файлы (*.txt)|*.txt|Все файлы (*.*)|*.*";
    this->saveFileDialog1->Title = "Выберите файл для сохранения";

    // Добавление элементов на форму
    this->Controls->Add(this->label1);
    this->Controls->Add(this->labelStatus);
    this->Controls->Add(this->dataGridView1);
    this->Controls->Add(this->btnAdd);
    this->Controls->Add(this->btnEdit);
    this->Controls->Add(this->btnDelete);
    this->Controls->Add(this->btnLoad);
    this->Controls->Add(this->btnSave);
    this->Controls->Add(this->btnSortDest);
    this->Controls->Add(this->btnSortPrice);
    this->Controls->Add(this->btnStats);
    this->Controls->Add(this->btnFindMin);
    this->Controls->Add(this->btnImportDB);
    this->Controls->Add(this->btnExportDB);
    this->Controls->Add(this->btnRefreshDB);

    // Подписка на событие загрузки формы
    this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
}

// Обновление DataGridView
void MainForm::UpdateDataGrid() {
    dataGridView1->Rows->Clear();
    dataGridView1->Columns->Clear();

    // Создаем колонки
    dataGridView1->Columns->Add("Number", "№");
    dataGridView1->Columns->Add("Destination", "Направление");
    dataGridView1->Columns->Add("BasePrice", "Базовая цена");
    dataGridView1->Columns->Add("Discount", "Скидка (%)");
    dataGridView1->Columns->Add("FinalPrice", "Итоговая цена");
    dataGridView1->Columns->Add("Type", "Тип тарифа");

    for (size_t i = 0; i < station->getTariffCount(); ++i) {
        auto tariff = station->getTariff(i);
        double discount = 0.0;

        if (tariff->getTariffType() == "Со скидкой") {
            auto discountTariff = std::dynamic_pointer_cast<DiscountTariff>(tariff);
            if (discountTariff) {
                discount = discountTariff->getDiscountPercent();
            }
        }

        dataGridView1->Rows->Add(
            (i + 1).ToString(),
            StdStringToSystemString(tariff->getDestination()),
            String::Format("{0:F2}", tariff->getBasePrice()),
            String::Format("{0:F1}", discount),
            String::Format("{0:F2}", tariff->getFinalPrice()),
            StdStringToSystemString(tariff->getTariffType())
        );
    }
}

// Загрузка данных из базы данных
void MainForm::LoadFromDatabase() {
    if (!databaseManager->IsConnected()) return;

    try {
        DataTable^ tariffs = databaseManager->GetAllTariffs();
        station->clearAllTariffs();

        for each (DataRow ^ row in tariffs->Rows) {
            String^ destination = safe_cast<String^>(row["Destination"]);
            double basePrice = Convert::ToDouble(row["BasePrice"]);
            double discount = Convert::ToDouble(row["Discount"]);
            String^ type = safe_cast<String^>(row["TariffType"]);

            std::string dest = SystemStringToStdString(destination);

            if (type == "Discount" && discount > 0) {
                station->addDiscountTariff(dest, basePrice, discount);
            }
            else {
                station->addRegularTariff(dest, basePrice);
            }
        }

        UpdateDataGrid();
    }
    catch (Exception^ e) {
        MessageBox::Show("Ошибка при загрузке данных из БД: " + e->Message, "Ошибка");
    }
}

// Отображение статуса базы данных
void MainForm::ShowDatabaseStatus() {
    if (databaseManager->IsConnected()) {
        labelStatus->Text = databaseManager->GetConnectionInfo();
        labelStatus->ForeColor = Color::DarkGreen;

        // Включаем кнопки работы с БД
        btnImportDB->Enabled = true;
        btnExportDB->Enabled = true;
        btnRefreshDB->Enabled = true;
    }
    else {
        labelStatus->Text = "Режим работы: Локальный (без базы данных)";
        labelStatus->ForeColor = Color::DarkRed;

        // Отключаем кнопки работы с БД
        btnImportDB->Enabled = false;
        btnExportDB->Enabled = false;
        btnRefreshDB->Enabled = false;
    }
}

// Обработчики событий

System::Void MainForm::MainForm_Load(System::Object^ sender, System::EventArgs^ e) {
    ShowDatabaseStatus();
}

System::Void MainForm::btnAdd_Click(System::Object^ sender, System::EventArgs^ e) {
    EditForm^ editForm = gcnew EditForm();
    if (editForm->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            std::string destination = SystemStringToStdString(editForm->Destination);
            double price = editForm->Price;
            double discount = editForm->HasDiscount ? editForm->Discount : 0.0;

            if (discount > 0) {
                station->addDiscountTariff(destination, price, discount);
            }
            else {
                station->addRegularTariff(destination, price);
            }

            // Сохраняем в БД, если подключены
            if (databaseManager->IsConnected()) {
                String^ type = editForm->HasDiscount ? "Discount" : "Regular";
                databaseManager->InsertTariff(editForm->Destination, price, discount, type);
            }

            UpdateDataGrid();
            MessageBox::Show("Тариф успешно добавлен", "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

System::Void MainForm::btnEdit_Click(System::Object^ sender, System::EventArgs^ e) {
    if (dataGridView1->SelectedRows->Count == 0) {
        MessageBox::Show("Выберите тариф для редактирования", "Предупреждение", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    int selectedIndex = dataGridView1->SelectedRows[0]->Index;
    if (selectedIndex < 0 || selectedIndex >= (int)station->getTariffCount()) {
        MessageBox::Show("Неверный выбранный индекс", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return;
    }

    auto tariff = station->getTariff(selectedIndex);
    double discount = 0.0;
    if (tariff->getTariffType() == "Со скидкой") {
        auto discountTariff = std::dynamic_pointer_cast<DiscountTariff>(tariff);
        if (discountTariff) {
            discount = discountTariff->getDiscountPercent();
        }
    }

    EditForm^ editForm = gcnew EditForm(
        StdStringToSystemString(tariff->getDestination()),
        tariff->getBasePrice(),
        discount
    );

    if (editForm->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            std::string destination = SystemStringToStdString(editForm->Destination);
            double price = editForm->Price;
            double newDiscount = editForm->HasDiscount ? editForm->Discount : 0.0;

            station->updateTariff(selectedIndex, destination, price, newDiscount);

            // Обновляем в БД, если подключены
            if (databaseManager->IsConnected()) {
                // Здесь нужно получить ID записи из БД
                // Для простоты, обновляем все данные из станции
                databaseManager->ClearDatabase();

                // Сохраняем все тарифы в БД
                for (size_t i = 0; i < station->getTariffCount(); ++i) {
                    auto t = station->getTariff(i);
                    String^ dest = StdStringToSystemString(t->getDestination());
                    double bp = t->getBasePrice();
                    double disc = 0.0;
                    String^ type = "Regular";

                    if (t->getTariffType() == "Со скидкой") {
                        auto dt = std::dynamic_pointer_cast<DiscountTariff>(t);
                        if (dt) {
                            disc = dt->getDiscountPercent();
                            type = "Discount";
                        }
                    }

                    databaseManager->InsertTariff(dest, bp, disc, type);
                }
            }

            UpdateDataGrid();
            MessageBox::Show("Тариф успешно обновлен", "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

System::Void MainForm::btnDelete_Click(System::Object^ sender, System::EventArgs^ e) {
    if (dataGridView1->SelectedRows->Count == 0) {
        MessageBox::Show("Выберите тариф для удаления", "Предупреждение", MessageBoxButtons::OK, MessageBoxIcon::Warning);
        return;
    }

    int selectedIndex = dataGridView1->SelectedRows[0]->Index;
    if (selectedIndex < 0 || selectedIndex >= (int)station->getTariffCount()) {
        MessageBox::Show("Неверный выбранный индекс", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return;
    }

    if (MessageBox::Show("Вы уверены, что хотите удалить выбранный тариф?",
        "Подтверждение удаления",
        MessageBoxButtons::YesNo,
        MessageBoxIcon::Question) == Windows::Forms::DialogResult::Yes) {

        try {
            station->removeTariff(selectedIndex);

            // Удаляем из БД, если подключены
            if (databaseManager->IsConnected()) {
                // Для простоты, обновляем все данные из станции
                databaseManager->ClearDatabase();

                // Сохраняем все тарифы в БД
                for (size_t i = 0; i < station->getTariffCount(); ++i) {
                    auto t = station->getTariff(i);
                    String^ dest = StdStringToSystemString(t->getDestination());
                    double bp = t->getBasePrice();
                    double disc = 0.0;
                    String^ type = "Regular";

                    if (t->getTariffType() == "Со скидкой") {
                        auto dt = std::dynamic_pointer_cast<DiscountTariff>(t);
                        if (dt) {
                            disc = dt->getDiscountPercent();
                            type = "Discount";
                        }
                    }

                    databaseManager->InsertTariff(dest, bp, disc, type);
                }
            }

            UpdateDataGrid();
            MessageBox::Show("Тариф успешно удален", "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

System::Void MainForm::btnLoad_Click(System::Object^ sender, System::EventArgs^ e) {
    if (openFileDialog1->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            std::string filename = SystemStringToStdString(openFileDialog1->FileName);
            if (station->loadFromFile(filename)) {
                UpdateDataGrid();
                MessageBox::Show("Данные успешно загружены из файла", "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Ошибка при загрузке данных из файла", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

System::Void MainForm::btnSave_Click(System::Object^ sender, System::EventArgs^ e) {
    if (saveFileDialog1->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            std::string filename = SystemStringToStdString(saveFileDialog1->FileName);
            if (station->saveToFile(filename)) {
                MessageBox::Show("Данные успешно сохранены в файл", "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Ошибка при сохранении данных в файл", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

System::Void MainForm::btnSortDest_Click(System::Object^ sender, System::EventArgs^ e) {
    station->sortByDestination();
    UpdateDataGrid();
}

System::Void MainForm::btnSortPrice_Click(System::Object^ sender, System::EventArgs^ e) {
    station->sortByFinalPrice();
    UpdateDataGrid();
}

System::Void MainForm::btnStats_Click(System::Object^ sender, System::EventArgs^ e) {
    std::string stats = station->getStatistics();
    MessageBox::Show(StdStringToSystemString(stats), "Статистика", MessageBoxButtons::OK, MessageBoxIcon::Information);
}

System::Void MainForm::btnFindMin_Click(System::Object^ sender, System::EventArgs^ e) {
    try {
        auto minTariff = station->findMinCostDestination();
        std::string info = "Направление с минимальной стоимостью:\n" + minTariff->getInfoString();
        MessageBox::Show(StdStringToSystemString(info), "Минимальная цена", MessageBoxButtons::OK, MessageBoxIcon::Information);
    }
    catch (const std::exception& ex) {
        MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
    }
}

System::Void MainForm::btnImportDB_Click(System::Object^ sender, System::EventArgs^ e) {
    if (!databaseManager->IsConnected()) {
        MessageBox::Show("Нет подключения к базе данных", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return;
    }

    if (openFileDialog1->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            if (databaseManager->ImportFromFile(openFileDialog1->FileName)) {
                LoadFromDatabase();
                MessageBox::Show("Данные успешно импортированы в базу данных", "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
                ShowDatabaseStatus();
            }
            else {
                MessageBox::Show("Ошибка при импорте данных в базу данных", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

System::Void MainForm::btnExportDB_Click(System::Object^ sender, System::EventArgs^ e) {
    if (!databaseManager->IsConnected()) {
        MessageBox::Show("Нет подключения к базе данных", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return;
    }

    if (saveFileDialog1->ShowDialog() == Windows::Forms::DialogResult::OK) {
        try {
            if (databaseManager->ExportToFile(saveFileDialog1->FileName)) {
                MessageBox::Show("Данные успешно экспортированы из базы данных", "Успех", MessageBoxButtons::OK, MessageBoxIcon::Information);
            }
            else {
                MessageBox::Show("Ошибка при экспорте данных из базы данных", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
            }
        }
        catch (const std::exception& ex) {
            MessageBox::Show(StdStringToSystemString(ex.what()), "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

System::Void MainForm::btnRefreshDB_Click(System::Object^ sender, System::EventArgs^ e) {
    if (!databaseManager->IsConnected()) {
        MessageBox::Show("Нет подключения к базе данных", "Ошибка", MessageBoxButtons::OK, MessageBoxIcon::Error);
        return;
    }

    LoadFromDatabase();
    MessageBox::Show("Данные обновлены из базы данных", "Информация", MessageBoxButtons::OK, MessageBoxIcon::Information);
    ShowDatabaseStatus();
}