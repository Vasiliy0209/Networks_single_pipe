#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <time.h>

#define MAX_CLIENTS_Q 10

#pragma once

HANDLE hPipe[MAX_CLIENTS_Q];

typedef struct {
	time_t connection_time;
	wchar_t host_name[255];
}ClientInfo;

ClientInfo client_info_db[MAX_CLIENTS_Q];
int clientinfo_count=0;

void AddClientInfo(ClientInfo client_info) {
	client_info_db[clientinfo_count] = client_info;
	clientinfo_count++;
}

typedef struct {
	time_t timestamp;
	wchar_t from[255];
	wchar_t message_text[255];
}ClientMessage;

ClientMessage message_history[255];
int message_count = 0;

void AddMessageToHistory(ClientMessage message) {
	message_history[message_count] = message;
	message_count++;
}

//Отправка истории сообщений вновь подключившемуся клиенту
void SendMessageHistory(int client_id) {
	wchar_t formatted_message[2000];
	wchar_t delimiter[5] = L"";

	tm tm;

	for (int i = 0;i < message_count;i++) {
		localtime_s(&tm, &message_history[i].timestamp);

		//Формирование ответа клиенту
		swprintf(
			formatted_message,
			L"%02d.%02d.%04d %02d:%02d:%02d [%S] %S",					
			tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec,
			message_history[i].from,
			message_history[i].message_text
		);

		unsigned long cbWritten;
		WriteFile(hPipe[client_id], formatted_message, (wcslen(formatted_message) + 1) * 2, &cbWritten, NULL);

		//WriteFile(hPipe[client_id], formatted_message, 2000, NULL, NULL);
	}

	//Отправка данных в Pipe
	//if(message_count>0)
	//	WriteFile(hPipe[client_id], formatted_message, 2000, NULL, NULL);		
}




HANDLE ReceiveMailslotHandle, SendMailslotHandle;
int    fConnect[MAX_CLIENTS_Q];

namespace Server {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для Form1
	///
	/// Внимание! При изменении имени этого класса необходимо также изменить
	///          свойство имени файла ресурсов ("Resource File Name") для средства компиляции управляемого ресурса,
	///          связанного со всеми файлами с расширением .resx, от которых зависит данный класс. В противном случае,
	///          конструкторы не смогут правильно работать с локализованными
	///          ресурсами, сопоставленными данной форме.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		int first = 1;
	private: System::Windows::Forms::Label^ lb_info;

		   int client_count = 0;
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  listBox1;
	protected: 
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::Timer^ timerMailslot;
	private: System::ComponentModel::IContainer^ components;

	private:
		/// <summary>
		/// Требуется переменная конструктора.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Обязательный метод для поддержки конструктора - не изменяйте
		/// содержимое данного метода при помощи редактора кода.
		/// </summary>
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
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// timerMailslot
			// 
			this->timerMailslot->Tick += gcnew System::EventHandler(this, &Form1::timerMailslot_Tick);
			// 
			// lb_info
			// 
			this->lb_info->AutoSize = true;
			this->lb_info->Location = System::Drawing::Point(12, 386);
			this->lb_info->Name = L"lb_info";
			this->lb_info->Size = System::Drawing::Size(44, 16);
			this->lb_info->TabIndex = 3;
			this->lb_info->Text = L"label1";
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
			this->Text = L"Сервер";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Form1::Form1_FormClosed);
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
			//Инициализация библиотеки WinSock (для использования gethostname)
			WSADATA	    WSAData;
			int		    rc;
			rc = WSAStartup(MAKEWORD(2, 0), &WSAData);
			if (rc != 0) {
				listBox1->Items->Add("Ошибка инициализации WSAStartup");
				return;
			} // if

			//Создание мэйлслота сервера
			ReceiveMailslotHandle = CreateMailslot(L"\\\\.\\mailslot\\server", 0,
				MAILSLOT_WAIT_FOREVER, NULL);

			if (ReceiveMailslotHandle == INVALID_HANDLE_VALUE) {
				listBox1->Items->Add("Ошибка при создании mailslot");
				timerMailslot->Enabled = false;
			}
			else {
				listBox1->Items->Add("Mailslot создан");
				timerMailslot->Enabled = true;
			} // else

			
	}   
	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
		wchar_t   Buf[512];
		DWORD  cbRead, LastError;
		unsigned long cbWritten;
		int			  i, l;

		for (int i = 0;i < client_count;i++) {
			//Получение информации о состоянии Pipe
			ConnectNamedPipe(hPipe[i], NULL);
			LastError = GetLastError();

			if (LastError == ERROR_PIPE_LISTENING) {
				if(i==0) lb_info->Text = "PIPE_LISTENING";
				fConnect[i] = 0;
			} // if
			if (LastError == ERROR_PIPE_CONNECTED) {
				if (i == 0) lb_info->Text = "PIPE_CONNECTED";
				if (fConnect[i] == 0) {
					listBox1->Items->Add("Клиент подключен. Отправка истории сообщений");
					SendMessageHistory(i);
				} // if
				fConnect[i] = 1;
			} // if
			if (LastError == ERROR_NO_DATA) {
				if (i == 0) lb_info->Text = "PIPE_NO_DATA";
				//if (fConnect[i]) {
					listBox1->Items->Add("Клиент отключен");
					DisconnectNamedPipe(hPipe[i]);
					fConnect[i] = 0;
				//}
			} // if
			if (fConnect[i]) {
				//Ожидание сообщения от клиента
				if (ReadFile(hPipe[i], Buf, 512, &cbRead, NULL)) {
					//Вывод принятого сообщения
					String^ s = gcnew String(Buf);
					listBox1->Items->Add(s);

					int j;

					ClientMessage mess;
					mess.timestamp = time(NULL);

					//Формирование подписи к сообщению имя хоста:id_клиента
					swprintf_s(mess.from, L"%S:%d", client_info_db[i].host_name, i);
					
					wcscpy(mess.message_text,Buf);
					AddMessageToHistory(mess);

					tm tm;
					localtime_s(&tm, &mess.timestamp);

					//Формирование ответа клиенту
					swprintf(
						Buf,
						L"%02d.%02d.%04d %02d:%02d:%02d [%S] %S",
						tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec,
						mess.from,
						mess.message_text
					);

					//Рассылка принятого сообщения всем клиентам
					for (j = 0;j < client_count;j++) {
						//пропускаем клиента, отправившего данное сообщение
						//if (i == j) continue;

						//Отправка данных в Pipe
						if (WriteFile(hPipe[j], Buf, 255/*2 * l + 2*/, &cbWritten, NULL)) {
							//listBox1->Items->Add(L"Отправлено: " + textBox1->Text);
						}
						else {
							listBox1->Items->Add(L"Ошибка WriteFile при ретрансляции сообщения клиенту");
						} // else
					}
				}
				else {
					//Обработка ошибки чтения
					LastError = GetLastError();
					if (LastError != ERROR_NO_DATA) {
						//sprintf(Buf, "Ошибка ReadFile LastError=%d", LastError);
						//listBox1->Items->Add(Buf);
						listBox1->Items->Add(String::Format("Ошибка ReadFile LastError= {0}", LastError));
					} // if
				} // if
			} // if
		}
	}

private: System::Void HandleClient() {

	wchar_t pipe_name[255];
	swprintf(pipe_name, 100, L"\\\\.\\pipe\\server%d", client_count);

	//Создание Pipe для данного клиента
	hPipe[client_count] = CreateNamedPipe(pipe_name, PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, PIPE_UNLIMITED_INSTANCES, 65536, 4096, 5000,
		NULL);
	if (hPipe[client_count] != INVALID_HANDLE_VALUE) {
		listBox1->Items->Add("Pipe создан");
	}
	else {
		listBox1->Items->Add("Ошибка при создании pipe");
	} // else

	client_count++;

	
}

private: System::Void Form1_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
		//Освобождение ресурсов (удаление pipe)
		CloseHandle(hPipe);
	}
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		wchar_t       Buf[1001];
		unsigned long cbWritten;
		int			  i, l;

		//Перекодирование строк String > Char array
		l = textBox1->Text->Length;
		if (l == 0) return;
		for (i=0; i < l; i++) {
			Buf[i] = textBox1->Text->default[i];
			Buf[i+1] = 0;
		} // for

		

		//Отправка данных в Pipe
		if (WriteFile(hPipe, Buf, 2*l+2, &cbWritten, NULL)) {
			listBox1->Items->Add(L"Отправлено: "+textBox1->Text);
		} else {
			listBox1->Items->Add(L"Ошибка WriteFile");
		} // else
	}
private: System::Void timerMailslot_Tick(System::Object^ sender, System::EventArgs^ e) {
	int           rc;
	unsigned long cbMessages, cbMsgNumber, cbRead;
	//wchar_t       Buf[1001];
	wchar_t       Buf[1001], Buf2[1001];

	unsigned long cbWritten;
	char client_hostname[255], server_hostname[255];
	wchar_t client_hostname_w[255] = { 0 };

	// Определяем состояние канала Mailslot
	rc = GetMailslotInfo(ReceiveMailslotHandle, NULL, &cbMessages, &cbMsgNumber, NULL);

	if (!rc) {
		listBox1->Items->Add("Ошибка GetMailslotInfo");
	} // if

	// Если в канале есть Mailslot сообщения, читаем первое из них
	if (cbMsgNumber != 0) {
		
		//Принятие от клиента имени его компьютера
		if (ReadFile(ReceiveMailslotHandle, client_hostname, 255, &cbRead, NULL)) {
			String^ s = gcnew String(client_hostname);
			
			if (!first) return;

			listBox1->Items->Add(L"Попытка подключения клиента с именем: " + s);

			//ClientInfo client_info;
			//преобразование строки из char * в wchar_t *
			mbstowcs_s(nullptr, client_info_db[client_count].host_name, client_hostname, _TRUNCATE);			
			client_info_db[client_count].connection_time = time(nullptr);

			

			//AddClientInfo(client_info);
			

			HandleClient();

			

			//Отправить клиенту имя компьютера сервера

			//перекодирование char в wchar
			for (int i = 0;i < strlen(client_hostname);i++)
				client_hostname_w[i] = client_hostname[i];

			//формирование адреса для подключения к мэйлслоту клиента
			wcscpy(Buf2, L"\\\\");
			wcscat(Buf2, client_hostname_w);
			wcscat(Buf2, L"\\mailslot\\client");


			//собственно подключение к клиенту
			SendMailslotHandle = CreateFile(Buf2, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (SendMailslotHandle == INVALID_HANDLE_VALUE) {
				listBox1->Items->Add("Ошибка при создании mailslot");
			} // if

			//Перекодирование из типа String в тип null termnianted string (Char array)
			//Определение имени компьютера сервера
			gethostname(server_hostname, 255);

			char answer[255];
			sprintf(answer,"%s:%d", server_hostname,client_count-1);

			//Отправка клиенту в мэйлслот имени компьютера сервера (автоответ)
			if (WriteFile(SendMailslotHandle, answer, 255, &cbWritten, NULL)) {
				listBox1->Items->Add(L"Клиенту отправлено имя компьютера-сервера");
			}
			else {
				listBox1->Items->Add("Ошибка WriteFile");
			} // else

			//Закрытие соединения с мэйлслотом клиента
			CloseHandle(SendMailslotHandle);



			first = 0;
		}
		else {
			listBox1->Items->Add("Ошибка ReadFile");
		} // else
	}
	else first = 1; // if
}
};
}

