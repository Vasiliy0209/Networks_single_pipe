#include <windows.h>
#include <winsock.h>
#pragma once

HANDLE hPipe;
HANDLE ReceiveMailslotHandle, SendMailslotHandle;

DWORD      cbRead, LastError;
OVERLAPPED Over = {0, 0, 0, 0, NULL};
wchar_t       Buf3[256];


namespace Client {

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
		int first=1;
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


	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Timer^ timerPipe;

	private: System::Windows::Forms::Timer^ timerMailslot;
	private: System::Windows::Forms::TextBox^ tb_clientuid;
	private: System::Windows::Forms::Label^ label1;
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
			this->button1->Text = L"Подключться";
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
			this->timerPipe->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// timerMailslot
			// 
			this->timerMailslot->Tick += gcnew System::EventHandler(this, &Form1::timerMailslot_Tick);
			// 
			// tb_clientuid
			// 
			this->tb_clientuid->Location = System::Drawing::Point(149, 329);
			this->tb_clientuid->Name = L"tb_clientuid";
			this->tb_clientuid->Size = System::Drawing::Size(100, 22);
			this->tb_clientuid->TabIndex = 8;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 338);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(61, 16);
			this->label1->TabIndex = 9;
			this->label1->Text = L"Client uid";
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
			this->Text = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		wchar_t       Buf[1001], Buf2[1001];
		unsigned long cbWritten;
		int			  i, l;
        
		//Создание мэйлслота клиента
		ReceiveMailslotHandle = CreateMailslot(L"\\\\.\\mailslot\\client", 0, MAILSLOT_WAIT_FOREVER, NULL);
		if (ReceiveMailslotHandle == INVALID_HANDLE_VALUE) {
			listBox1->Items->Add("Ошибка при создании mailslot");
			timerMailslot->Enabled = false;
		}
		else {
			listBox1->Items->Add("Mailslot создан");
			timerMailslot->Enabled = true;
		} // else

		//Подключение к мэйлслоту сервера		
		char client_hostname[255];

		//Формирование широковещательного адреса
		wcscpy(Buf2, L"\\\\*\\mailslot\\server");
		
		//Собственно подключение к мэйлслоту сервера
		SendMailslotHandle = CreateFile(Buf2, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (SendMailslotHandle == INVALID_HANDLE_VALUE) {
			listBox1->Items->Add("Ошибка при создании mailslot");
		} // if

		//Определение имени компьютера клиента
		gethostname(client_hostname, 255);

		if (WriteFile(SendMailslotHandle, client_hostname, 255, &cbWritten, NULL)) {
			listBox1->Items->Add(L"Отправлен широковещательный запрос на поиск сервера");
		}
		else {
			listBox1->Items->Add("Ошибка WriteFile");
		} // else
		CloseHandle(SendMailslotHandle);
		//---[/]---

		
    }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
		CloseHandle(hPipe);
		listBox1->Items->Add("Клиент отключен");
		timerPipe->Enabled = false;
	 }
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
		wchar_t       Buf[1001];
		unsigned long cbWritten;
		int			  i, l;
		//Перекодировка строк
		l = textBox2->Text->Length;
		if (l == 0) return;
		for (i=0; i < l; i++) {
			Buf[i] = textBox2->Text->default[i];
			Buf[i+1] = 0;
		} // for

		//Отправка строки в pipe
		if (WriteFile(hPipe, Buf, 2*l+2, &cbWritten, NULL)) {
			//listBox1->Items->Add(L"Отправлено: "+textBox2->Text);
			textBox2->Text = "";
		} else {
		    listBox1->Items->Add("Ошибка WriteFile при отправке сообщения");
		} // else
	}
private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
		//Проверка состояния pipe
		GetOverlappedResult(hPipe, &Over, &cbRead, FALSE);
		if (cbRead > 0) {
			
			/*String^ s = gcnew String(Buf3);
			listBox1->Items->Add(s);*/
			ReadFile(hPipe, Buf3, 512, &cbRead, &Over);

			String^ s = gcnew String(Buf3);
			listBox1->Items->Add(s);

			/*wchar_t* ctx = nullptr;

			if (wcsncmp(Buf3, L"LIST", 4) == 0) {
				for (wchar_t* tok = wcstok(Buf3 + 4, L"|", &ctx); tok; tok = wcstok(nullptr, L"|", &ctx)) {
					listBox1->Items->Add(gcnew String(tok));
				}
			}
			else {
				String^ s = gcnew String(Buf3);
				listBox1->Items->Add(L"Получено: " + s);
			}*/
		} // if
	}
private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e) {
	
	//Инициализация библиотеки WinSock (для использования gethostname)
	WSADATA	    WSAData;
	int		    rc;	
	rc = WSAStartup(MAKEWORD(2, 0), &WSAData);
	if (rc != 0) {
		listBox1->Items->Add("Ошибка инициализации WSAStartup");
		return;
	} // if

	
}
private: System::Void timerMailslot_Tick(System::Object^ sender, System::EventArgs^ e) {
	int           rc;
	unsigned long cbMessages, cbMsgNumber, cbRead;
	wchar_t       Buf[1001], Buf2[1001], pipe_name[255];

	char server_response[255], server_hostname[255], client_uid[255];
	wchar_t server_hostname_w[255] = { 0 };

	// Определяем состояние канала Mailslot
	rc = GetMailslotInfo(ReceiveMailslotHandle, NULL, &cbMessages, &cbMsgNumber, NULL);

	if (!rc) {
		listBox1->Items->Add("Ошибка GetMailslotInfo");
	} // if

	// Если в канале есть Mailslot сообщения, читаем первое из них
	if (cbMsgNumber != 0) {
		if (!first) return;
		if (ReadFile(ReceiveMailslotHandle, server_response, 255, &cbRead, NULL)) {

			//Отсоединение (освобождение) Mailslot
			CloseHandle(ReceiveMailslotHandle);
			//CloseHandle(SendMailslotHandle);

			strcpy(server_hostname,strtok(server_response, ":"));
			strcpy(client_uid, strtok(NULL, ":"));

			tb_clientuid->Text = gcnew String(client_uid);
		
			String^ s = gcnew String(server_hostname);
			listBox1->Items->Add(L"Найден сервер с именем: " + s);

			//Подключение к серверу по протоколу Pipe			
			//перекодирование char в wchar
			for (int i = 0;i < strlen(server_hostname);i++)
				server_hostname_w[i] = server_hostname[i];

			//Формирование имени серверного pipe
			wsprintf(pipe_name,L"\\\\%s\\pipe\\server%d",server_hostname_w,atoi(client_uid));

			/*//Формирование адреса для подключения к серверу
			wcscpy(Buf2, L"\\\\"); //присваивание C-строк
			wcscat(Buf2, server_hostname_w); //конкатенация C-строк
			wcscat(Buf2, L"\\pipe\\server"); //конкатенация C-строк*/

			//System::Threading::Thread::Sleep(2000);

			//Собственно подключение к pipe сервера
			hPipe = CreateFile(pipe_name, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

			if (hPipe != INVALID_HANDLE_VALUE) {
				listBox1->Items->Add("Pipe создан, клиент подключен");
			}
			else {
				listBox1->Items->Add("Ошибка при подключении к pipe");
			} // else

			//Чтение из канала
			ReadFile(hPipe, Buf3, 512, &cbRead, &Over);

			//Запуск таймера Pipe
			timerPipe->Enabled = true;
			//Остановка таймера Mailslot
			timerMailslot->Enabled = false;
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

