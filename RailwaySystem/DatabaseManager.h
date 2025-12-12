#pragma once

#include "TariffClasses.h"
#include <string>
#include <vector>

namespace RailwaySystem {
    using namespace System;
    using namespace System::Data;
    using namespace System::Data::SqlClient;

    public ref class DatabaseManager {
    private:
        SqlConnection^ connection;
        bool isConnected;
        String^ connectionString;

    public:
        DatabaseManager();
        ~DatabaseManager();

        bool Connect();
        bool Connect(String^ server, String^ database);
        void Disconnect();
        bool IsConnected() { return isConnected; }

        bool CreateDatabase();
        bool CreateTable();
        bool InsertTariff(String^ destination, double basePrice, double discount, String^ type);
        bool UpdateTariff(int id, String^ destination, double basePrice, double discount, String^ type);
        bool DeleteTariff(int id);
        bool DeleteAllTariffs();
        DataTable^ GetAllTariffs();
        int GetTariffCount();
        void ClearDatabase();
        bool ImportFromFile(String^ filePath);
        bool ExportToFile(String^ filePath);

        String^ GetConnectionInfo();

    private:
        bool ExecuteNonQuery(String^ query);
    };
}