#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <time.h>

#pragma once

#define MAX_CLIENTS_Q 10

HANDLE hPipe[MAX_CLIENTS_Q];
int    fConnect[MAX_CLIENTS_Q] = { 0 };

typedef struct {
	time_t timestamp;
	wchar_t message_text[512];
} ClientMessage;

ClientMessage message_history[255];
int message_count = 0;

void AddMessageToHistory(const wchar_t* text) {
	if (message_count >= 255) return;
	message_history[message_count].timestamp = time(NULL);
	wcscpy_s(message_history[message_count].message_text, 512, text);
	message_count++;
}

// Мгновенная поштучная отправка всей истории новому клиенту
void SendMessageHistory(int client_id) {
	wchar_t formatted_message[600];
	unsigned long cbWritten;
	tm tm;

	for (int i = 0; i < message_count; i++) {
		localtime_s(&tm, &message_history[i].timestamp);

		swprintf_s(
			formatted_message, 600,
			L"%02d.%02d.%04d %02d:%02d:%02d %ls",
			tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			message_history[i].message_text
		);

		DWORD bytesToWrite = (DWORD)((wcslen(formatted_message) + 1) * sizeof(wchar_t));
		WriteFile(hPipe[client_id], formatted_message, bytesToWrite, &cbWritten, NULL);
	}
}

char last_query_host = { 0 };
ULONGLONG last_query_time = 0;

HANDLE ReceiveMailslotHandle, SendMailslotHandle;

namespace Server {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class Form1 : public System::Windows::Forms::Form
	{
	private:
		System::Windows::Forms::Label^ lb_info;
		System::Windows::Forms::ListBox^ listBox1;
		System::Windows::Forms::TextBox^ textBox1;
		System::Windows::Forms::Button^ button1;
		System::Windows::Forms::Timer^ timer1;
		System::Windows::Forms::Timer^ timerMailslot;
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
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->timerMailslot = (gcnew System::Windows::Forms::Timer(this->components));
			this->lb_info = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->ItemHeight = 16;
			this->listBox1->Location = System::Drawing::Point(8, 7);
			this->listBox1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(719, 324);
			this->listBox1->TabIndex = 0;
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(9, 350);
			this->textBox1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(612, 22);
			this->textBox1->TabIndex = 1;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(627, 346);
			this->button1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(99, 30);
			this->button1->TabIndex = 2;
			this->button1->Text = L"Отправить";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 50;
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// timerMailslot
			// 
			this->timerMailslot->Interval = 100;
			this->timerMailslot->Tick += gcnew System::EventHandler(this, &Form1::timerMailslot_Tick);
			// 
			// lb_info
			// 
			this->lb_info->AutoSize = true;
			this->lb_info->Location = System::Drawing::Point(12, 386);
			this->lb_info->Name = L"lb_info";
			this->lb_info->Size = System::Drawing::Size(65, 16);
			this->lb_info->TabIndex = 3;
			this->lb_info->Text = L"Ожидание";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(739, 423);
			this->Controls->Add(this->lb_info);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->listBox1);
			this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->Name = L"Form1";
			this->Text = L"Сервер (Единый Pipe)";
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

		// 1. Создаём Mailslot для приёма широковещательных запросов поиска
		ReceiveMailslotHandle = CreateMailslot(L"\\\\.\\mailslot\\server", 0, MAILSLOT_WAIT_FOREVER, NULL);
		if (ReceiveMailslotHandle == INVALID_HANDLE_VALUE) {
			listBox1->Items->Add("Ошибка при создании mailslot");
			timerMailslot->Enabled = false;
		}
		else {
			listBox1->Items->Add("Mailslot создан");
			timerMailslot->Enabled = true;
		}

		// 2. Создаём пул инстансов ОДНОГО именованного канала \\.\pipe\server
		int created = 0;
		for (int i = 0; i < MAX_CLIENTS_Q; i++) {
			hPipe[i] = CreateNamedPipe(
				L"\\\\.\\pipe\\server",
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
				MAX_CLIENTS_Q,
				65536, 65536, 5000, NULL
			);
			if (hPipe[i] != INVALID_HANDLE_VALUE) {
				created++;
			}
		}
		listBox1->Items->Add(String::Format("Канал \\\\.\\pipe\\server готов (слотов: {0})", created));
	}

	private: System::Void BroadcastMessage(const wchar_t* raw_text) {
		AddMessageToHistory(raw_text);

		wchar_t outBuf[600];
		tm tm;
		time_t now = time(NULL);
		localtime_s(&tm, &now);

		swprintf_s(
			outBuf, 600,
			L"%02d.%02d.%04d %02d:%02d:%02d %ls",
			tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
			tm.tm_hour, tm.tm_min, tm.tm_sec,
			raw_text
		);

		listBox1->Items->Add(gcnew String(outBuf));

		unsigned long cbWritten;
		DWORD bytesToWrite = (DWORD)((wcslen(outBuf) + 1) * sizeof(wchar_t));

		for (int j = 0; j < MAX_CLIENTS_Q; j++) {
			if (fConnect[j]) {
				WriteFile(hPipe[j], outBuf, bytesToWrite, &cbWritten, NULL);
			}
		}
	}

	private: System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e) {
		wchar_t Buf[512];
		DWORD cbRead, LastError;

		for (int i = 0; i < MAX_CLIENTS_Q; i++) {
			if (hPipe[i] == INVALID_HANDLE_VALUE) continue;

			ConnectNamedPipe(hPipe[i], NULL);
			LastError = GetLastError();

			if (LastError == ERROR_PIPE_LISTENING) {
				if (i == 0) lb_info->Text = "Слот 0: PIPE_LISTENING";
				fConnect[i] = 0;
			}
			else if (LastError == ERROR_PIPE_CONNECTED) {
				if (i == 0) lb_info->Text = "Слот 0: PIPE_CONNECTED";
				if (fConnect[i] == 0) {
					listBox1->Items->Add(String::Format("Клиент подключен в слот #{0}. Отправка истории...", i));
					SendMessageHistory(i);
				}
				fConnect[i] = 1;
			}
			else if (LastError == ERROR_NO_DATA) {
				if (i == 0) lb_info->Text = "Слот 0: PIPE_NO_DATA";
				if (fConnect[i]) {
					listBox1->Items->Add(String::Format("Клиент отключен от слота #{0}", i));
					DisconnectNamedPipe(hPipe[i]); // Корректный сброс конкретного инстанса
					fConnect[i] = 0;
				}
			}

			if (fConnect[i]) {
				ZeroMemory(Buf, sizeof(Buf));
				if (ReadFile(hPipe[i], Buf, sizeof(Buf) - sizeof(wchar_t), &cbRead, NULL) && cbRead > 0) {
					BroadcastMessage(Buf);
				}
			}
		}
	}

	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		if (textBox1->Text->Length == 0) return;

		wchar_t msg[512] = L"[Сервер] ";
		int baseLen = (int)wcslen(msg);
		int l = textBox1->Text->Length;
		if (l > 490) l = 490;

		for (int i = 0; i < l; i++) {
			msg[baseLen + i] = textBox1->Text[i];
		}
		msg[baseLen + l] = L'\0';

		BroadcastMessage(msg);
		textBox1->Clear();
	}

	private: System::Void timerMailslot_Tick(System::Object^ sender, System::EventArgs^ e) {
		unsigned long cbMessages, cbMsgNumber, cbRead, cbWritten;
		char client_hostname[255] = { 0 };
		char server_hostname[255] = { 0 };
		wchar_t client_hostname_w[255] = { 0 };
		wchar_t target_mailslot[300] = { 0 };

		if (!GetMailslotInfo(ReceiveMailslotHandle, NULL, &cbMessages, &cbMsgNumber, NULL)) {
			return;
		}

		// Обрабатываем все накопившиеся запросы поиска
		while (cbMsgNumber > 0) {
			ZeroMemory(client_hostname, sizeof(client_hostname));
			if (ReadFile(ReceiveMailslotHandle, client_hostname, 255, &cbRead, NULL) && cbRead > 0) {
				
				listBox1->Items->Add(L"Запрос поиска от: " + gcnew String(client_hostname));

				mbstowcs_s(nullptr, client_hostname_w, client_hostname, _TRUNCATE);
				swprintf_s(target_mailslot, 300, L"\\\\%ls\\mailslot\\client", client_hostname_w);

				SendMailslotHandle = CreateFile(target_mailslot, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				if (SendMailslotHandle != INVALID_HANDLE_VALUE) {
					gethostname(server_hostname, 255);
					WriteFile(SendMailslotHandle, server_hostname, (DWORD)(strlen(server_hostname) + 1), &cbWritten, NULL);
					CloseHandle(SendMailslotHandle);
					listBox1->Items->Add(L"Отправлен адрес сервера клиенту: " + gcnew String(client_hostname));
				}
			}
			GetMailslotInfo(ReceiveMailslotHandle, NULL, &cbMessages, &cbMsgNumber, NULL);
		}
	}
private: System::Void Form1_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
	CloseHandle(ReceiveMailslotHandle);
	for (int i = 0; i < MAX_CLIENTS_Q; i++) {
		if (hPipe[i] != INVALID_HANDLE_VALUE) {
			DisconnectNamedPipe(hPipe[i]);
			CloseHandle(hPipe[i]);
		}
	}
	WSACleanup();
}
};
}