#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace RailwaySystem;

[STAThread]
int main(array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    MainForm^ form = gcnew MainForm();
    Application::Run(form);

    return 0;
}