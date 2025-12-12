#include "DatabaseManager.h"
#include "RailwayStation.h"
#include <string>
#include <fstream>
#include <sstream>

using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace RailwaySystem;

DatabaseManager::DatabaseManager() {
    connection = nullptr;
    isConnected = false;
    connectionString = "Data Source=(LocalDB)\\MSSQLLocalDB;Integrated Security=True;";
}

DatabaseManager::~DatabaseManager() {
    Disconnect();
}

bool DatabaseManager::Connect() {
    try {
        connection = gcnew SqlConnection(connectionString);
        connection->Open();
        isConnected = true;

        CreateDatabase();
        return true;
    }
    catch (Exception^ e) {
        isConnected = false;
        return false;
    }
}

bool DatabaseManager::Connect(String^ server, String^ database) {
    try {
        connectionString = String::Format("Data Source={0};Initial Catalog={1};Integrated Security=True;", server, database);
        connection = gcnew SqlConnection(connectionString);
        connection->Open();
        isConnected = true;
        return true;
    }
    catch (Exception^ e) {
        isConnected = false;
        return false;
    }
}

void DatabaseManager::Disconnect() {
    if (connection != nullptr && connection->State == ConnectionState::Open) {
        connection->Close();
    }
    isConnected = false;
}

bool DatabaseManager::CreateDatabase() {
    try {
        String^ checkDbQuery =
            "IF DB_ID('RailwayDatabase') IS NULL " \
            "CREATE DATABASE RailwayDatabase;";

        SqlCommand^ command = gcnew SqlCommand(checkDbQuery, connection);
        command->ExecuteNonQuery();

        connection->ChangeDatabase("RailwayDatabase");

        return CreateTable();
    }
    catch (Exception^ e) {
        return false;
    }
}

bool DatabaseManager::CreateTable() {
    try {
        String^ query =
            "IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='Tariffs' AND xtype='U') " \
            "CREATE TABLE Tariffs ( " \
            "   ID INT IDENTITY(1,1) PRIMARY KEY, " \
            "   Destination NVARCHAR(100) NOT NULL, " \
            "   BasePrice DECIMAL(10,2) NOT NULL, " \
            "   Discount DECIMAL(5,2) DEFAULT 0, " \
            "   TariffType NVARCHAR(20) NOT NULL, " \
            "   CreatedDate DATETIME DEFAULT GETDATE() " \
            ")";

        return ExecuteNonQuery(query);
    }
    catch (Exception^ e) {
        return false;
    }
}

bool DatabaseManager::InsertTariff(String^ destination, double basePrice, double discount, String^ type) {
    try {
        String^ query =
            "INSERT INTO Tariffs (Destination, BasePrice, Discount, TariffType) " \
            "VALUES (@Destination, @BasePrice, @Discount, @TariffType)";

        SqlCommand^ command = gcnew SqlCommand(query, connection);
        command->Parameters->AddWithValue("@Destination", destination);
        command->Parameters->AddWithValue("@BasePrice", basePrice);
        command->Parameters->AddWithValue("@Discount", discount);
        command->Parameters->AddWithValue("@TariffType", type);

        return command->ExecuteNonQuery() > 0;
    }
    catch (Exception^ e) {
        return false;
    }
}

bool DatabaseManager::UpdateTariff(int id, String^ destination, double basePrice, double discount, String^ type) {
    try {
        String^ query =
            "UPDATE Tariffs SET " \
            "Destination = @Destination, " \
            "BasePrice = @BasePrice, " \
            "Discount = @Discount, " \
            "TariffType = @TariffType " \
            "WHERE ID = @ID";

        SqlCommand^ command = gcnew SqlCommand(query, connection);
        command->Parameters->AddWithValue("@ID", id);
        command->Parameters->AddWithValue("@Destination", destination);
        command->Parameters->AddWithValue("@BasePrice", basePrice);
        command->Parameters->AddWithValue("@Discount", discount);
        command->Parameters->AddWithValue("@TariffType", type);

        return command->ExecuteNonQuery() > 0;
    }
    catch (Exception^ e) {
        return false;
    }
}

bool DatabaseManager::DeleteTariff(int id) {
    try {
        String^ query = "DELETE FROM Tariffs WHERE ID = @ID";
        SqlCommand^ command = gcnew SqlCommand(query, connection);
        command->Parameters->AddWithValue("@ID", id);

        return command->ExecuteNonQuery() > 0;
    }
    catch (Exception^ e) {
        return false;
    }
}

bool DatabaseManager::DeleteAllTariffs() {
    try {
        String^ query = "DELETE FROM Tariffs";
        SqlCommand^ command = gcnew SqlCommand(query, connection);

        return command->ExecuteNonQuery() >= 0;
    }
    catch (Exception^ e) {
        return false;
    }
}

DataTable^ DatabaseManager::GetAllTariffs() {
    DataTable^ table = gcnew DataTable();
    if (!isConnected) return table;

    try {
        String^ query = "SELECT * FROM Tariffs ORDER BY Destination";
        SqlCommand^ command = gcnew SqlCommand(query, connection);
        SqlDataAdapter^ adapter = gcnew SqlDataAdapter(command);
        adapter->Fill(table);
    }
    catch (Exception^ e) {
        // Empty table will be returned
    }

    return table;
}

int DatabaseManager::GetTariffCount() {
    if (!isConnected) return 0;

    try {
        String^ query = "SELECT COUNT(*) FROM Tariffs";
        SqlCommand^ command = gcnew SqlCommand(query, connection);
        Object^ result = command->ExecuteScalar();
        if (result != nullptr) {
            return Convert::ToInt32(result);
        }
        return 0;
    }
    catch (Exception^ e) {
        return 0;
    }
}

void DatabaseManager::ClearDatabase() {
    DeleteAllTariffs();
}

bool DatabaseManager::ImportFromFile(String^ filePath) {
    if (!isConnected) return false;

    try {
        RailwayStation station;

        // Преобразуем String^ в std::string
        std::string path;
        for each (wchar_t wch in filePath) {
            path.push_back(static_cast<char>(wch));
        }

        if (!station.loadFromFile(path)) {
            return false;
        }

        ClearDatabase();

        for (size_t i = 0; i < station.getTariffCount(); ++i) {
            auto tariff = station.getTariff(i);

            // Преобразуем std::string в String^
            std::string dest = tariff->getDestination();
            String^ destination = gcnew String(dest.c_str());

            double basePrice = tariff->getBasePrice();
            double discount = 0.0;
            String^ type = "Regular";

            if (tariff->getTariffType() == "Со скидкой") {
                auto discountTariff = std::dynamic_pointer_cast<DiscountTariff>(tariff);
                if (discountTariff) {
                    discount = discountTariff->getDiscountPercent();
                    type = "Discount";
                }
            }

            InsertTariff(destination, basePrice, discount, type);
        }

        return true;
    }
    catch (Exception^ e) {
        return false;
    }
}

bool DatabaseManager::ExportToFile(String^ filePath) {
    if (!isConnected) return false;

    try {
        DataTable^ tariffs = GetAllTariffs();

        // Создаем std::string путь
        std::string path;
        for each (wchar_t wch in filePath) {
            path.push_back(static_cast<char>(wch));
        }

        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }

        for each (DataRow ^ row in tariffs->Rows) {
            String^ destination = safe_cast<String^>(row["Destination"]);
            double basePrice = Convert::ToDouble(row["BasePrice"]);
            double discount = Convert::ToDouble(row["Discount"]);
            String^ type = safe_cast<String^>(row["TariffType"]);

            // Преобразуем String^ в std::string
            std::string destStr;
            for each (wchar_t wch in destination) {
                destStr.push_back(static_cast<char>(wch));
            }

            std::string typeStr;
            for each (wchar_t wch in type) {
                typeStr.push_back(static_cast<char>(wch));
            }

            file << destStr << ";"
                << basePrice << ";"
                << typeStr << ";"
                << discount << "\n";
        }

        file.close();
        return true;
    }
    catch (Exception^ e) {
        return false;
    }
}

String^ DatabaseManager::GetConnectionInfo() {
    if (!isConnected) return "Database not connected";

    return String::Format("Connected to: {0}\nDatabase: {1}\nTariffs in DB: {2}",
        connection->DataSource,
        connection->Database,
        GetTariffCount());
}

bool DatabaseManager::ExecuteNonQuery(String^ query) {
    try {
        SqlCommand^ command = gcnew SqlCommand(query, connection);
        int rowsAffected = command->ExecuteNonQuery();
        return true;
    }
    catch (Exception^ e) {
        return false;
    }
}