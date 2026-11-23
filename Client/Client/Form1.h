#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#pragma once

HANDLE hPipe = INVALID_HANDLE_VALUE;
HANDLE ReceiveMailslotHandle = INVALID_HANDLE_VALUE;
wchar_t Buf3[1024];

namespace Client {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class Form1 : public System::Windows::Forms::Form
	{
	private:
		System::Windows::Forms::ListBox^ listBox1;
		System::Windows::Forms::Button^ button1;
		System::Windows::Forms::Button^ button2;
		System::Windows::Forms::Label^ label2;
		System::Windows::Forms::TextBox^ textBox2;
		System::Windows::Forms::Button^ button3;
		System::Windows::Forms::Timer^ timerPipe;
		System::Windows::Forms::Timer^ timerMailslot;
		System::Windows::Forms::TextBox^ tb_clientuid;
		System::Windows::Forms::Label^ label1;
		System::ComponentModel::IContainer^ components;

	public:
		Form1(void)
		{
			InitializeComponent();
		}

	protected:
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->timerPipe = (gcnew System::Windows::Forms::Timer(this->components));
			this->timerMailslot = (gcnew System::Windows::Forms::Timer(this->components));
			this->tb_clientuid = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->ItemHeight = 16;
			this->listBox1->Location = System::Drawing::Point(1, 1);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(748, 308);
			this->listBox1->TabIndex = 0;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(501, 326);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(116, 28);
			this->button1->TabIndex = 3;
			this->button1->Text = L"Подключиться";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(633, 326);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(116, 28);
			this->button2->TabIndex = 4;
			this->button2->Text = L"Отключиться";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 371);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(119, 16);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Текст сообщения";
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(149, 368);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(468, 22);
			this->textBox2->TabIndex = 6;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(633, 365);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(116, 28);
			this->button3->TabIndex = 7;
			this->button3->Text = L"Отправить";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// timerPipe
			// 
			this->timerPipe->Interval = 50;
			this->timerPipe->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// timerMailslot
			// 
			this->timerMailslot->Interval = 100;
			this->timerMailslot->Tick += gcnew System::EventHandler(this, &Form1::timerMailslot_Tick);
			// 
			// tb_clientuid
			// 
			this->tb_clientuid->Location = System::Drawing::Point(149, 329);
			this->tb_clientuid->Name = L"tb_clientuid";
			this->tb_clientuid->Size = System::Drawing::Size(150, 22);
			this->tb_clientuid->TabIndex = 8;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 335);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(86, 16);
			this->label1->TabIndex = 9;
			this->label1->Text = L"Имя клиента";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(753, 409);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->tb_clientuid);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->listBox1);
			this->Name = L"Form1";
			this->Text = L"Клиент (Единый Pipe)";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Form1::Form1_FormClosed);
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

	private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e) {
		WSADATA WSAData;
		if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0) {
			listBox1->Items->Add("Ошибка инициализации WSAStartup");
			return;
		}

		// Подставляем имя ПК как ник по умолчанию (можно изменить в поле)
		char host[255] = { 0 };
		if (gethostname(host, 255) == 0) {
			tb_clientuid->Text = gcnew String(host);
		}
	}

	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		if (hPipe != INVALID_HANDLE_VALUE) {
			listBox1->Items->Add("Уже подключено к серверу");
			return;
		}

		if (ReceiveMailslotHandle != INVALID_HANDLE_VALUE) {
			CloseHandle(ReceiveMailslotHandle);
		}

		// 1. Создаём Mailslot клиента для приёма ответа от сервера
		ReceiveMailslotHandle = CreateMailslot(L"\\\\.\\mailslot\\client", 0, MAILSLOT_WAIT_FOREVER, NULL);
		if (ReceiveMailslotHandle == INVALID_HANDLE_VALUE) {
			listBox1->Items->Add("Ошибка при создании mailslot клиента");
			return;
		}
		timerMailslot->Enabled = true;

		// 2. Отправляем широковещательный запрос на поиск сервера
		HANDLE SendMailslotHandle = CreateFile(L"\\\\*\\mailslot\\server", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (SendMailslotHandle == INVALID_HANDLE_VALUE) {
			listBox1->Items->Add("Ошибка открытия широковещательного mailslot");
			return;
		}

		char client_hostname[255] = { 0 };
		unsigned long cbWritten;
		gethostname(client_hostname, 255);

		if (WriteFile(SendMailslotHandle, client_hostname, (DWORD)(strlen(client_hostname) + 1), &cbWritten, NULL)) {
			listBox1->Items->Add(L"Отправлен широковещательный запрос на поиск сервера...");
		}
		else {
			listBox1->Items->Add("Ошибка отправки запроса поиска");
		}
		CloseHandle(SendMailslotHandle);
	}

	private: System::Void timerMailslot_Tick(System::Object^ sender, System::EventArgs^ e) {

		

		unsigned long cbMessages, cbMsgNumber, cbRead;
		char server_hostname[255] = { 0 };
		wchar_t server_hostname_w[255] = { 0 };
		wchar_t pipe_name[300] = { 0 };

		if (!GetMailslotInfo(ReceiveMailslotHandle, NULL, &cbMessages, &cbMsgNumber, NULL)) {
			return;
		}

		if (cbMsgNumber > 0) {
			if (ReadFile(ReceiveMailslotHandle, server_hostname, 255, &cbRead, NULL) && cbRead > 0) {
				timerMailslot->Enabled = false;
				CloseHandle(ReceiveMailslotHandle);
				ReceiveMailslotHandle = INVALID_HANDLE_VALUE;

				listBox1->Items->Add(L"Найден сервер: " + gcnew String(server_hostname));

				mbstowcs_s(nullptr, server_hostname_w, server_hostname, _TRUNCATE);
				// Подключаемся к единому имени канала без индекса
				swprintf_s(pipe_name, 300, L"\\\\%ls\\pipe\\server", server_hostname_w);

				// Ждём освобождения слота до 1 секунды (на случай одновременного подключения)
				WaitNamedPipe(pipe_name, 1000);

				hPipe = CreateFile(
					pipe_name,
					GENERIC_READ | GENERIC_WRITE,
					0, NULL, OPEN_EXISTING, 0, NULL
				);

				if (hPipe != INVALID_HANDLE_VALUE) {
					// Переводим клиентский конец канала в режим чтения сообщений
					DWORD mode = PIPE_READMODE_MESSAGE;
					SetNamedPipeHandleState(hPipe, &mode, NULL, NULL);

					listBox1->Items->Add("Подключение к чату выполнено!");
					timerPipe->Enabled = true;
				}
				else {
					listBox1->Items->Add("Ошибка подключения к pipe (возможно, сервер переполнен)");
				}
			}
		}
	}

	private: System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e) {
		if (hPipe == INVALID_HANDLE_VALUE) return;

		DWORD bytesAvail = 0, cbRead = 0;

		// Проверяем, разорвал ли сервер соединение
		if (!PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvail, NULL)) {
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				timerPipe->Enabled = false;
				CloseHandle(hPipe);
				hPipe = INVALID_HANDLE_VALUE;
				listBox1->Items->Add("Соединение с сервером разорвано");
			}
			return;
		}

		if (bytesAvail == 0) return;

		listBox1->BeginUpdate();
		while (PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvail, NULL) && bytesAvail > 0) {
			ZeroMemory(Buf3, sizeof(Buf3));
			if (ReadFile(hPipe, Buf3, sizeof(Buf3) - sizeof(wchar_t), &cbRead, NULL) && cbRead > 0) {
				listBox1->Items->Add(gcnew String(Buf3));
			}
			else {
				break;
			}
		}
		listBox1->EndUpdate();
	}
private: System::Void button3_Click(System::Object^ sender, System::EventArgs^ e) {
	if (hPipe == INVALID_HANDLE_VALUE) {
		listBox1->Items->Add("Сначала подключитесь к серверу");
		return;
	}
	if (textBox2->Text->Length == 0) return;
	String^ senderName = String::IsNullOrWhiteSpace(tb_clientuid->Text) ? "Клиент" : tb_clientuid->Text;
	String^ fullMsg = String::Format("[{0}] {1}", senderName, textBox2->Text);
	wchar_t Buf[512];
	int l = fullMsg->Length;
	if (l > 510) l = 510;
	for (int i = 0; i < l; i++) {
		Buf[i] = fullMsg[i];
	}
	Buf[l] = L'\0';
	unsigned long cbWritten;
	DWORD bytesToWrite = (DWORD)((l + 1) * sizeof(wchar_t));
	if (WriteFile(hPipe, Buf, bytesToWrite, &cbWritten, NULL)) {
		textBox2->Clear();
	}
	else {
		listBox1->Items->Add("Ошибка отправки сообщения");
	}
}
private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
	timerPipe->Enabled = false;
	timerMailslot->Enabled = false;
	if (hPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(hPipe);
		hPipe = INVALID_HANDLE_VALUE;
	}
	if (ReceiveMailslotHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(ReceiveMailslotHandle);
		ReceiveMailslotHandle = INVALID_HANDLE_VALUE;
	}
	listBox1->Items->Add("Клиент отключен");
}
private: System::Void Form1_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
	button2_Click(nullptr, nullptr);
	WSACleanup();
}
};
}