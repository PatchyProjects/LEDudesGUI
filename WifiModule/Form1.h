#pragma once
#include <msclr\marshal_cppstd.h>



namespace WifiModule {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Net;
	using namespace System::Net::Sockets;
	using namespace System::Net::NetworkInformation;
	using namespace System::IO;
	using namespace System::Text;
	using namespace System::Threading;
	using namespace System::Collections::Generic;
	

	using std::ofstream;
	using std::endl;
	

	/*
	using namespace cv;
	using std::cout;
	using std::cin;
	using std::endl;
	using std::ofstream;
	*/

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		/****The csv file to send****/
		System::String^ file;

		/****The LED module to send to, identified by IP address****/
		System::String^ LEDmodule;
		System::String^ Keyword;

		/****Holds the list of modules active (IP addresses) and the file(s) selected to load to them****/
		array<System::String^>^ IPadd;
		array<System::String^>^ Files;

		// Flag for whether an Ack has been recveived
		bool acked;

		// Flag for whether all LED modules have files loaded onto them 
		bool loaded;

	private: System::Windows::Forms::TextBox^  fileSelectedTextBox;

	public: 

		/****Flag to see if listbox has been clicked****/
		int listboxclick;
	private: System::Windows::Forms::TabControl^  tabControl;
	private: System::Windows::Forms::TabPage^  VideoPage;
	public: 

	public: 

	private: System::Windows::Forms::TabPage^  WifiPage;
	private: System::Windows::Forms::Button^  videoBrowseButton;
	private: System::Windows::Forms::TextBox^  videoPathText;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::OpenFileDialog^  openVideoDialog;
	private: System::Windows::Forms::Button^  processButton;
	private: System::Windows::Forms::Button^  repeatButton;
	private: System::Windows::Forms::Button^  eraseButton;
	private: System::Windows::Forms::Button^  tagFrameButton;
	private: System::Windows::Forms::TextBox^  tagFramePathText;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::OpenFileDialog^  openTagFrameDialog;
	private: System::Windows::Forms::CheckBox^  tagFrameCheckBox;
	private: System::Windows::Forms::Button^  previewPlayButton;
	private: System::Windows::Forms::Label^  label4;

	private: System::ComponentModel::BackgroundWorker^  backgroundWorkerRefresh;


	public: 

		/****Sends file to the selected LED module****/
		void Connect( System::String^ LEDmodule, List<System::String^>^ data )
		{
			try
			{
				// Create a TcpClient. 
				// Note, for this client to work you need to have a TcpServer  
				// connected to the same address as specified by the server, port 
				// combination.

				//MessageBox::Show(System::Convert::ToString(data->Count));

				Int32 port = 2000;
				TcpClient^ client = gcnew TcpClient(LEDmodule, port);

				NetworkStream^ stream = client->GetStream();
				
				System::String^ Keyword = "TECXMOO";
				Encoding^ ascii = Encoding::ASCII;
				array<Byte>^ msg = ascii->GetBytes(Keyword);
				
				// Close everything.
				//client->Close();
				
				/*
				// Counter to tell if 1st or 2nd response
				Int32 w = 0;
				
				// Send keyword to signal sending a file
				for (int r = 0; r < 5; r++)
				{
					stream->Write( msg, 0, msg->Length );
					
					// Buffer to store the response bytes.
					array<Byte>^ ack = gcnew array<Byte>(256);

					// String to store the response ASCII representation.
					System::String^ response = nullptr;
					Int32 i;

					while (i = stream->Read( ack, 0, ack->Length ))
					{
						// Read the TcpServer response bytes.
						response = ascii->GetString( ack );
						
						// First response
						if (w == 2)
						{
							break;
						}

						//Second response
						else if (w == 1)
						{
							w++;
						}

						//Third response
						else if (w == 0)
						{
							w++;
						}

						if (response->Contains("ACK"))
						{
							acked = true;
							break;
						}

					}

					if (acked == true)
					{
						acked = false;
						break;
					}

					if (r == 4)
					{
						MessageBox::Show("Error, please reset the LED module.");
						return;
					}
				}
				*/
				
				// Send file frame by frame
				for (int c = 0; c < data->Count; c++)
				{
					//TcpClient^ client2 = gcnew TcpClient(LEDmodule, port);

					//NetworkStream^ stream2 = client2->GetStream();

					array<Byte>^ byte_frames;
					Encoding^ ascii = Encoding::ASCII;
					byte_frames = ascii->GetBytes(data[c]);

					//MessageBox::Show(data[c]);

					// Send the frames to the connected LED module. 
					stream->Write( byte_frames, 0, byte_frames->Length );

					// Receive the TcpServer::response. 

					// Buffer to store the response bytes.
					array<Byte>^ ack = gcnew array<Byte>(256);

					// String to store the response ASCII representation.
					System::String^ response = nullptr;
					Int32 k;
					while (k = stream->Read( ack, 0, ack->Length ))
					{
						// Read the TcpServer response bytes.
						response = ascii->GetString( ack );

						if (response->Contains("ACK"))
						{
							break;
						}
					}
					// Close everything.
					//client2->Close();
				}
				//TcpClient^ client3 = gcnew TcpClient(LEDmodule, port);

				//NetworkStream^ stream3 = client3->GetStream();

				System::String^ Keyword2 = "TECYMOO";
				array<Byte>^ msg2 = ascii->GetBytes(Keyword2);

				stream->Write( msg2, 0, msg2->Length );

				
				// Close everything.
				client->Close();
			}
			catch ( ArgumentNullException^ e ) 
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Could not connect, " + s); 
			}
			catch ( SocketException^ e ) 
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Could not connect, " + s);
			}
		}

		/****Sends command to the selected LED module****/
		void Connect2( System::String^ LEDmodule, array<Byte>^ data )
		{
			try
			{
				// Create a TcpClient. 
				// Note, for this client to work you need to have a TcpServer  
				// connected to the same address as specified by the server, port 
				// combination.
				Int32 port = 2000;
				TcpClient^ client = gcnew TcpClient(LEDmodule, port);
				//Encoding^ ascii = Encoding::ASCII;
				NetworkStream^ stream = client->GetStream();
				// Send the message to the connected TcpServer.
				stream->Write( data, 0, data->Length );

				//// Send keyword to signal sending a file
				//for (int r = 0; r < 10; r++)
				//{
				//	// Send the message to the connected TcpServer.
				//	//stream->Write( data, 0, data->Length );
				//	
				//	// Buffer to store the response bytes.
				//	array<Byte>^ ack = gcnew array<Byte>(256);

				//	// String to store the response ASCII representation.
				//	System::String^ response = nullptr;
				//	Int32 i;
				//	
				//	// Counter to tell if 1st or 2nd response
				//	Int32 w = 0;
				//	while (i = stream->Read( ack, 0, ack->Length ))
				//	{
				//		// Read the TcpServer response bytes.
				//		response = ascii->GetString( ack );
				//		
				//		// Third response
				//		//if (w == 2)
				//		//{
				//			//break;
				//		//}

				//		//Second response
				//		if (w == 1)
				//		{
				//			if (response->Contains("J"))
				//			{
				//			acked = true;
				//			//sent = true;
				//			break;
				//			}
				//			else break;
				//		}

				//		//First response
				//		else if (w == 0)
				//		{
				//			if (response->Contains("J"))
				//			{
				//			acked = true;
				//			//sent = true;
				//			break;
				//			}
				//			else w++;
				//		}
				//	}

				//	if (acked == true)
				//	{
				//		//acked = false;
				//		break;
				//	}

				//	if (r == 9)
				//	{
				//		MessageBox::Show("Error, could not send command");
				//		//sent = false;
				//		return;
				//	}
				//}
				// Close everything.
				client->Close();
			}
			catch ( ArgumentNullException^ e ) 
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Could not connect, " + s);
			}
			catch ( SocketException^ e ) 
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Could not connect, " + s );
			}
		}

		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			try 
			{
				/****Pings a list if IP addresses to fill listbox with all active modules****/
				System::String^ left = "170.255.1.";
				int rightInt = 1;
				System::String^ right;
				for (int i = 0; i < 3; i++)
				{
					right = System::Convert::ToString(rightInt);
					System::String^ ipAddress = left + right;
					Ping ^ pingSender = gcnew Ping;
					IPAddress^ address = IPAddress::Parse(ipAddress);
					PingReply^ reply = pingSender->Send(address);

					if ( reply->Status == IPStatus::Success )
					{
						wifiListBox->Items->Add(ipAddress);
					}

					rightInt++;
				}
			}
			catch (PingException^ e)
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Error in locating LED modules, please press the refresh button. Exception " + s);
			}
			/****Number of modules in listbox****/
			int n = wifiListBox->Items->Count;
			
			/****IPadd is an array containing all the modules in the listbox****/
			IPadd = gcnew array<System::String^>(n);

			/****Files is an array containing the filenames to be loaded onto the corresponding modules in IPadd with the same indices****/
			/****If no file opened for the module, then value is NULL****/
			Files = gcnew array<System::String^>(n);

			for (int i = 0; i < n; i++)
			{
				IPadd[i] = wifiListBox->Items[i]->ToString();
				Files[i] = "Please Select a File";
			}
			acked = false;
			loaded = false;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox^  wifiListBox;
	protected: 
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  refreshButton;
	private: System::Windows::Forms::Button^  playButton;
	private: System::Windows::Forms::Button^  stopButton;
	private: System::Windows::Forms::Button^  loadButton;
	private: System::Windows::Forms::Button^  chooseButton;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->wifiListBox = (gcnew System::Windows::Forms::ListBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->refreshButton = (gcnew System::Windows::Forms::Button());
			this->playButton = (gcnew System::Windows::Forms::Button());
			this->stopButton = (gcnew System::Windows::Forms::Button());
			this->loadButton = (gcnew System::Windows::Forms::Button());
			this->chooseButton = (gcnew System::Windows::Forms::Button());
			this->fileSelectedTextBox = (gcnew System::Windows::Forms::TextBox());
			this->backgroundWorkerRefresh = (gcnew System::ComponentModel::BackgroundWorker());
			this->tabControl = (gcnew System::Windows::Forms::TabControl());
			this->VideoPage = (gcnew System::Windows::Forms::TabPage());
			this->tagFrameCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->tagFrameButton = (gcnew System::Windows::Forms::Button());
			this->tagFramePathText = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->processButton = (gcnew System::Windows::Forms::Button());
			this->videoBrowseButton = (gcnew System::Windows::Forms::Button());
			this->videoPathText = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->WifiPage = (gcnew System::Windows::Forms::TabPage());
			this->eraseButton = (gcnew System::Windows::Forms::Button());
			this->repeatButton = (gcnew System::Windows::Forms::Button());
			this->openVideoDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->openTagFrameDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->previewPlayButton = (gcnew System::Windows::Forms::Button());
			this->tabControl->SuspendLayout();
			this->VideoPage->SuspendLayout();
			this->WifiPage->SuspendLayout();
			this->SuspendLayout();
			// 
			// wifiListBox
			// 
			this->wifiListBox->FormattingEnabled = true;
			this->wifiListBox->Location = System::Drawing::Point(8, 38);
			this->wifiListBox->Margin = System::Windows::Forms::Padding(2);
			this->wifiListBox->Name = L"wifiListBox";
			this->wifiListBox->Size = System::Drawing::Size(112, 108);
			this->wifiListBox->TabIndex = 0;
			this->wifiListBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::wifiListBox_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 13);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(71, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"LED Modules";
			// 
			// refreshButton
			// 
			this->refreshButton->Location = System::Drawing::Point(9, 156);
			this->refreshButton->Margin = System::Windows::Forms::Padding(2);
			this->refreshButton->Name = L"refreshButton";
			this->refreshButton->Size = System::Drawing::Size(112, 20);
			this->refreshButton->TabIndex = 2;
			this->refreshButton->Text = L"Refresh";
			this->refreshButton->UseVisualStyleBackColor = true;
			this->refreshButton->Click += gcnew System::EventHandler(this, &Form1::refreshButton_Click);
			// 
			// playButton
			// 
			this->playButton->Location = System::Drawing::Point(9, 190);
			this->playButton->Margin = System::Windows::Forms::Padding(2);
			this->playButton->Name = L"playButton";
			this->playButton->Size = System::Drawing::Size(112, 25);
			this->playButton->TabIndex = 3;
			this->playButton->Text = L"Play";
			this->playButton->UseVisualStyleBackColor = true;
			this->playButton->Click += gcnew System::EventHandler(this, &Form1::playButton_Click);
			// 
			// stopButton
			// 
			this->stopButton->Location = System::Drawing::Point(184, 190);
			this->stopButton->Margin = System::Windows::Forms::Padding(2);
			this->stopButton->Name = L"stopButton";
			this->stopButton->Size = System::Drawing::Size(112, 25);
			this->stopButton->TabIndex = 4;
			this->stopButton->Text = L"Stop";
			this->stopButton->UseVisualStyleBackColor = true;
			this->stopButton->Click += gcnew System::EventHandler(this, &Form1::stopButton_Click);
			// 
			// loadButton
			// 
			this->loadButton->Location = System::Drawing::Point(214, 103);
			this->loadButton->Margin = System::Windows::Forms::Padding(2);
			this->loadButton->Name = L"loadButton";
			this->loadButton->Size = System::Drawing::Size(72, 20);
			this->loadButton->TabIndex = 5;
			this->loadButton->Text = L"Load Files";
			this->loadButton->UseVisualStyleBackColor = true;
			this->loadButton->Click += gcnew System::EventHandler(this, &Form1::loadButton_Click);
			// 
			// chooseButton
			// 
			this->chooseButton->Location = System::Drawing::Point(221, 26);
			this->chooseButton->Margin = System::Windows::Forms::Padding(2);
			this->chooseButton->Name = L"chooseButton";
			this->chooseButton->Size = System::Drawing::Size(72, 20);
			this->chooseButton->TabIndex = 6;
			this->chooseButton->Text = L"Choose File";
			this->chooseButton->UseVisualStyleBackColor = true;
			this->chooseButton->Click += gcnew System::EventHandler(this, &Form1::chooseButton_Click);
			// 
			// fileSelectedTextBox
			// 
			this->fileSelectedTextBox->Location = System::Drawing::Point(123, 51);
			this->fileSelectedTextBox->Name = L"fileSelectedTextBox";
			this->fileSelectedTextBox->ReadOnly = true;
			this->fileSelectedTextBox->Size = System::Drawing::Size(168, 20);
			this->fileSelectedTextBox->TabIndex = 8;
			this->fileSelectedTextBox->Text = L"Please select a Module";
			// 
			// backgroundWorkerRefresh
			// 
			this->backgroundWorkerRefresh->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &Form1::backgroundWorkerRefresh_RunWorkerCompleted);
			// 
			// tabControl
			// 
			this->tabControl->Controls->Add(this->VideoPage);
			this->tabControl->Controls->Add(this->WifiPage);
			this->tabControl->Location = System::Drawing::Point(-1, 1);
			this->tabControl->Name = L"tabControl";
			this->tabControl->SelectedIndex = 0;
			this->tabControl->Size = System::Drawing::Size(342, 290);
			this->tabControl->TabIndex = 9;
			// 
			// VideoPage
			// 
			this->VideoPage->BackColor = System::Drawing::Color::WhiteSmoke;
			this->VideoPage->Controls->Add(this->previewPlayButton);
			this->VideoPage->Controls->Add(this->label4);
			this->VideoPage->Controls->Add(this->tagFrameCheckBox);
			this->VideoPage->Controls->Add(this->tagFrameButton);
			this->VideoPage->Controls->Add(this->tagFramePathText);
			this->VideoPage->Controls->Add(this->label3);
			this->VideoPage->Controls->Add(this->processButton);
			this->VideoPage->Controls->Add(this->videoBrowseButton);
			this->VideoPage->Controls->Add(this->videoPathText);
			this->VideoPage->Controls->Add(this->label2);
			this->VideoPage->Location = System::Drawing::Point(4, 22);
			this->VideoPage->Name = L"VideoPage";
			this->VideoPage->Padding = System::Windows::Forms::Padding(3);
			this->VideoPage->Size = System::Drawing::Size(334, 264);
			this->VideoPage->TabIndex = 0;
			this->VideoPage->Text = L"Video Processor";
			this->VideoPage->Click += gcnew System::EventHandler(this, &Form1::tabPage1_Click);
			this->VideoPage->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::VideoPage_Paint);
			// 
			// tagFrameCheckBox
			// 
			this->tagFrameCheckBox->AutoSize = true;
			this->tagFrameCheckBox->Location = System::Drawing::Point(15, 95);
			this->tagFrameCheckBox->Name = L"tagFrameCheckBox";
			this->tagFrameCheckBox->Size = System::Drawing::Size(99, 17);
			this->tagFrameCheckBox->TabIndex = 7;
			this->tagFrameCheckBox->Text = L"Use Tag Frame";
			this->tagFrameCheckBox->UseVisualStyleBackColor = true;
			// 
			// tagFrameButton
			// 
			this->tagFrameButton->Location = System::Drawing::Point(239, 69);
			this->tagFrameButton->Name = L"tagFrameButton";
			this->tagFrameButton->Size = System::Drawing::Size(75, 23);
			this->tagFrameButton->TabIndex = 6;
			this->tagFrameButton->Text = L"Browse";
			this->tagFrameButton->UseVisualStyleBackColor = true;
			this->tagFrameButton->Click += gcnew System::EventHandler(this, &Form1::tagFrameButton_Click);
			// 
			// tagFramePathText
			// 
			this->tagFramePathText->Location = System::Drawing::Point(12, 69);
			this->tagFramePathText->Name = L"tagFramePathText";
			this->tagFramePathText->Size = System::Drawing::Size(218, 20);
			this->tagFramePathText->TabIndex = 5;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(12, 53);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(148, 13);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Choose Tag Frame (Optional):";
			// 
			// processButton
			// 
			this->processButton->Location = System::Drawing::Point(239, 98);
			this->processButton->Name = L"processButton";
			this->processButton->Size = System::Drawing::Size(75, 53);
			this->processButton->TabIndex = 3;
			this->processButton->Text = L"Process";
			this->processButton->UseVisualStyleBackColor = true;
			this->processButton->Click += gcnew System::EventHandler(this, &Form1::processButton_Click);
			// 
			// videoBrowseButton
			// 
			this->videoBrowseButton->Location = System::Drawing::Point(239, 30);
			this->videoBrowseButton->Name = L"videoBrowseButton";
			this->videoBrowseButton->Size = System::Drawing::Size(75, 23);
			this->videoBrowseButton->TabIndex = 2;
			this->videoBrowseButton->Text = L"Browse";
			this->videoBrowseButton->UseVisualStyleBackColor = true;
			this->videoBrowseButton->Click += gcnew System::EventHandler(this, &Form1::videoBrowseButton_Click);
			// 
			// videoPathText
			// 
			this->videoPathText->Location = System::Drawing::Point(12, 30);
			this->videoPathText->Name = L"videoPathText";
			this->videoPathText->Size = System::Drawing::Size(221, 20);
			this->videoPathText->TabIndex = 1;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(9, 13);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(95, 13);
			this->label2->TabIndex = 0;
			this->label2->Text = L"Choose Video File:";
			// 
			// WifiPage
			// 
			this->WifiPage->BackColor = System::Drawing::Color::WhiteSmoke;
			this->WifiPage->Controls->Add(this->eraseButton);
			this->WifiPage->Controls->Add(this->repeatButton);
			this->WifiPage->Controls->Add(this->wifiListBox);
			this->WifiPage->Controls->Add(this->playButton);
			this->WifiPage->Controls->Add(this->stopButton);
			this->WifiPage->Controls->Add(this->loadButton);
			this->WifiPage->Controls->Add(this->fileSelectedTextBox);
			this->WifiPage->Controls->Add(this->label1);
			this->WifiPage->Controls->Add(this->chooseButton);
			this->WifiPage->Controls->Add(this->refreshButton);
			this->WifiPage->Location = System::Drawing::Point(4, 22);
			this->WifiPage->Name = L"WifiPage";
			this->WifiPage->Padding = System::Windows::Forms::Padding(3);
			this->WifiPage->Size = System::Drawing::Size(334, 264);
			this->WifiPage->TabIndex = 1;
			this->WifiPage->Text = L"LED Output";
			// 
			// eraseButton
			// 
			this->eraseButton->Location = System::Drawing::Point(184, 224);
			this->eraseButton->Name = L"eraseButton";
			this->eraseButton->Size = System::Drawing::Size(112, 25);
			this->eraseButton->TabIndex = 10;
			this->eraseButton->Text = L"Erase";
			this->eraseButton->UseVisualStyleBackColor = true;
			this->eraseButton->Click += gcnew System::EventHandler(this, &Form1::eraseButton_Click);
			// 
			// repeatButton
			// 
			this->repeatButton->Location = System::Drawing::Point(9, 224);
			this->repeatButton->Name = L"repeatButton";
			this->repeatButton->Size = System::Drawing::Size(112, 25);
			this->repeatButton->TabIndex = 9;
			this->repeatButton->Text = L"Repeat";
			this->repeatButton->UseVisualStyleBackColor = true;
			this->repeatButton->Click += gcnew System::EventHandler(this, &Form1::repeatButton_Click);
			// 
			// openTagFrameDialog
			// 
			this->openTagFrameDialog->FileName = L"openTagFrameDialog";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(10, 119);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(48, 13);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Preview:";
			// 
			// previewPlayButton
			// 
			this->previewPlayButton->Location = System::Drawing::Point(127, 224);
			this->previewPlayButton->Name = L"previewPlayButton";
			this->previewPlayButton->Size = System::Drawing::Size(93, 23);
			this->previewPlayButton->TabIndex = 11;
			this->previewPlayButton->Text = L"Play Preview";
			this->previewPlayButton->UseVisualStyleBackColor = true;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(340, 293);
			this->Controls->Add(this->tabControl);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(2);
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->Text = L"LED GUI";
			this->tabControl->ResumeLayout(false);
			this->VideoPage->ResumeLayout(false);
			this->VideoPage->PerformLayout();
			this->WifiPage->ResumeLayout(false);
			this->WifiPage->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void chooseButton_Click(System::Object^  sender, System::EventArgs^  e) {
				 

				 if (listboxclick == 1)
				 {
					 // Displays an OpenFileDialog so the user can select a Cursor.
					 OpenFileDialog ^ openFileDialog1 = gcnew OpenFileDialog();
					 openFileDialog1->Filter = "LED Files|*.csv";
					 openFileDialog1->Title = "Select an LED File";
					 
					 file = nullptr;

					 // Show the Dialog.
					 // If the user clicked OK in the dialog and a .csv file was selected, open it.
					 if(openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
					 {
						 file = openFileDialog1->FileName;
						 fileSelectedTextBox->Text = file;
					 }
					 
					 for (int i = 0; i < IPadd->Length; i++)
					 {
						if (wifiListBox->SelectedItem->ToString() == IPadd[i])
						{
							if (file != nullptr)
							{
								Files[i] = file;
								return;
							}
						}
					 }
				 }
				 else MessageBox::Show("Please select an LED module from the listbox first!");
			 }
private: System::Void loadButton_Click(System::Object^  sender, System::EventArgs^  e) {

			 for (int i = 0; i < Files->Length; i++)
			 {
				 //If a module does not yet have a file loaded
				 if (Files[i] == "Please Select a File")
				 {
					 MessageBox::Show("Please choose files for all modules before loading!");
					 return;
				 }
			 }

			 for (int j = 0; j < Files->Length; j++)
			 {
				 
				 StreamReader^ din = File::OpenText(Files[j]);

				 System::String^ frames = "";
				 System::String^ temp = "";
				 int line = 1;

				 // A list of strings that contain the values for every 10 frames
				 List<System::String^>^ frame_strings = gcnew List<System::String^>(); 

				 while ((temp = din->ReadLine()) != nullptr) 
				 {
					 //send every 10 lines as a string
					 if (line%10 == 0)
					 {
						 if (frames == ""){
							break;
						 }
						 else
						 {
							 frames += temp;
							 frame_strings->Add(frames);
							 frames = "";
							 line++;
						 }
					 }
					 else 
					 {
						 frames += temp;
						 line++;
					 }
				 }

				 if (frames != "")
				 {
					 frame_strings->Add(frames);
				 }

				 if (frame_strings[(frame_strings->Count)-1] == "")
				 {
					 frame_strings->RemoveAt((frame_strings->Count)-1);
				 }
				 Connect(IPadd[j],frame_strings);
			 }
			 loaded = true;
			 MessageBox::Show("All files loaded!");
		 }
private: System::Void refreshButton_Click(System::Object^  sender, System::EventArgs^  e) {

			 backgroundWorkerRefresh->RunWorkerAsync();
		 }
private: System::Void wifiListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 listboxclick = 1;

			 fileSelectedTextBox->Text = Files[wifiListBox->SelectedIndex];
		 }
private: System::Void playButton_Click(System::Object^  sender, System::EventArgs^  e) {

			if (loaded == true)
			 {
				 Keyword = "TECKMOO";
				 Encoding^ ascii = Encoding::ASCII;
				 // Translate the passed message into ASCII and store it as a Byte array. 
				 array<Byte>^ message = ascii->GetBytes(Keyword);

				 for (int j = 0; j < Files->Length; j++)
				 {
					 Connect2(IPadd[j], message);
					 /*if (acked == false)
					 {
						 return;
					 }
					 else acked = false;*/
				 }
			 }
			 else MessageBox::Show("Please load all LED modules with files first!");
		 }
private: System::Void stopButton_Click(System::Object^  sender, System::EventArgs^  e) {

			Keyword = "TECQMOO";
			 Encoding^ ascii = Encoding::ASCII;
			 // Translate the passed message into ASCII and store it as a Byte array. 
			 array<Byte>^ message = ascii->GetBytes(Keyword);

			 for (int j = 0; j < Files->Length; j++)
			 {
				 Connect2(IPadd[j], message);
				/* if (acked == false)
					 {
						 return;
					 }
				 else acked = false;*/
			 } 
		 }
private: System::Void repeatButton_Click(System::Object^  sender, System::EventArgs^  e) {
			if (loaded == true)
			 {
				 Keyword = "TECRMOO";
				 Encoding^ ascii = Encoding::ASCII;
				 // Translate the passed message into ASCII and store it as a Byte array. 
				 array<Byte>^ message = ascii->GetBytes(Keyword);

				 for (int j = 0; j < Files->Length; j++)
				 {
					 Connect2(IPadd[j], message);
					/* if (acked == false)
					 {
						 return;
					 }
					 else acked = false;*/
				 }
			 }
			 else MessageBox::Show("Please load all LED modules with files first!");
		 }
private: System::Void eraseButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 Keyword = "TECZ";
				 Encoding^ ascii = Encoding::ASCII;
				 // Translate the passed message into ASCII and store it as a Byte array. 
				 array<Byte>^ message = ascii->GetBytes(Keyword);

				 for (int j = 0; j < Files->Length; j++)
				 {
					 Connect2(IPadd[j], message);
					/* if (acked == false)
					 {
						 return;
					 }
					 else acked = false;*/
				 }
		 }
private: System::Void backgroundWorkerRefresh_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {

			 wifiListBox->Items->Clear();
			 listboxclick = 0;

			 /****Pings a list if IP addresses to fill listbox with all active modules****/
			 System::String^ left = "170.255.1.";
			 int rightInt = 1;
			 System::String^ right;
			 for (int i = 0; i < 3; i++)
			 {
				 right = System::Convert::ToString(rightInt);
				 System::String^ ipAddress = left + right;
				 Ping ^ pingSender = gcnew Ping;
				 IPAddress^ address = IPAddress::Parse(ipAddress);
				 PingReply ^ reply = pingSender->Send(address);

				 if ( reply->Status == IPStatus::Success )
				 {
					 wifiListBox->Items->Add(ipAddress);
				 }

				 rightInt++;
			 }

			 /****Number of modules in listbox****/
			 int n = wifiListBox->Items->Count;

			 for (int i = 0; i < n; i++)
			 {
				 IPadd[i] = wifiListBox->Items[i]->ToString();
				 Files[i] = "Please Select a File";
			 }

			 fileSelectedTextBox->Text = "Please select a Module";

			 MessageBox::Show("Refreshed!");

		 }
		private: System::Void tabPage1_Click(System::Object^  sender, System::EventArgs^  e) {

		 }


/*******************************************************************************************
******			Video Processing GUI										****************
*******************************************************************************************/


		private: System::Void videoBrowseButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			if(openVideoDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				//this->videoPathText->Text = "Hello World";
				this->videoPathText->Text = openVideoDialog->FileName;
			}
			//this->Invalidate();
		}
		private: System::Void processButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			process();
		}
		

		private: System::Void tagFrameButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			if(openTagFrameDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				//this->videoPathText->Text = "Hello World";
				this->tagFramePathText->Text = openTagFrameDialog->FileName;
			}
		}

		private: std::string getFileName(std::string filePathAndExt)
		{
			std::string fileName;
			unsigned found = filePathAndExt.find_last_of("/\\");

			for(int i = found+1; i<filePathAndExt.size(); i++)
			{
				if(filePathAndExt[i] == '.')
					break;
				fileName += filePathAndExt[i];
			}
			return fileName;
		}

		public: int process()
		{
			using System::Runtime::InteropServices::Marshal;

			System::String^ managedString = this->videoPathText->Text;

			msclr::interop::marshal_context context;
			std::string videoFile = context.marshal_as<std::string>(managedString);

			//cvStartWindowThread();

			//std::string videoFile = "C:\\Programming\\Senior Design\\VideoProcessor\\GUI\\WifiCode\\WifiModule\\Debug\\testColorMix.avi";
			VideoCapture cap(videoFile);		//Open Video File
			if(!cap.isOpened())					// check if we succeeded
			{
				//cout<<"Error Opening Video"<<endl;
				return -1;
			}
			
			
			std::string frameFileName = getFileName(videoFile) + ".csv";
			std::ofstream frameFile;
			frameFile.open (frameFileName);
			if (!frameFile.is_open()) //error opening writefile
			{
				//cout<<"Error Opening CSV file"<<endl;
				return -1;
			}

			FrameProcessor processFrame;

			namedWindow("video",1);

			//cout<<"Processing Video, Please Wait..."<<endl;
			int frameCount = cap.get(CV_CAP_PROP_FRAME_COUNT);

			Mat firstFrame;
			cap >> firstFrame;

			//Mat image;
			//image = imread(, CV_LOAD_IMAGE_COLOR);   // Read the file
			/*
			if(processFrame.isTagFrame(firstFrame))
				processFrame.locateLEDLocations(firstFrame);
			else
				processFrame.basicLayout(firstFrame);*/

			if(this->tagFrameCheckBox->Checked)		// User is using a tag frame
			{
				System::String^ managedTagString = this->tagFramePathText->Text;
				msclr::interop::marshal_context tagContext;
				std::string tagFile = tagContext.marshal_as<std::string>(managedTagString);
				
				Mat image;
				image = imread(tagFile, CV_LOAD_IMAGE_COLOR);   // Read the file
				processFrame.locateLEDLocations(image);
				processFrame.scaleTagLocations(firstFrame,image);
			}
			else
				processFrame.basicLayout(firstFrame);

			for(int i = 0; i < frameCount-1; i++)		// frameCount-1 b/c we discarded first frame
			{
				if(i%10 == 0)
				{
					frameFile<<"TECG";
				}
				Mat frame;
				cap >> frame; // get new frame from video

				imshow("video", frame);

				processFrame.process(frame);
				//processFrame.colorCorrect();		// Apply gamma Correction
				frameFile<<processFrame.getFrameString();
				//if( (i-1)%30  == 0)
			
				if( ( (i+1)%10 == 0 && i!=0) || i == frameCount-2)
				{
					frameFile<<"H";
				}
				if( i != frameCount-2)
				{
					frameFile<<std::endl;
				}
				//if(i != frameCount-2)				// We don't want new line on the last line of file
					//frameFile<<endl;
				
			}
			
			//frameFile<<"Y";				// output file must end with "Y"
			cvDestroyWindow("video");
			frameFile.close();
			//cout<<"Number of frames processed: "<<frameCount<<endl;
			//cout<<"Approx. length of video: "<<frameCount/30<<" seconds"<<endl;

			

			vector<vector<int>> processedRVals = processFrame.processedRVals;
			vector<vector<int>> processedGVals = processFrame.processedGVals;
			vector<vector<int>> processedBVals = processFrame.processedBVals;

			for(int replay = 0; replay< 1; replay++)
			{
				//for(int i = 0; i<processedRVals.size(); i++)
				//{
					processFrame.replayVideo(firstFrame);		//Reuse first frame to draw on
				//}
			}

			
			return 0;

		}

		private: System::Void VideoPage_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
		{
			Graphics^ g = e->Graphics;
			//g->Clear(System::Drawing::Color::AntiqueWhite);

			System::Drawing::Rectangle rect = System::Drawing::Rectangle(10,120,124,124);
			System::Drawing::SolidBrush^ myBrush = gcnew System::Drawing::SolidBrush(System::Drawing::Color::Red);
			g->FillRectangle(myBrush, rect);
			/*
			System::Drawing::Rectangle smallRect;

			smallRect.X = rect.X + rect.Width / 4;

			smallRect.Y = rect.Y + rect.Height / 4;

			smallRect.Width = rect.Width / 2;

			smallRect.Height = rect.Height / 2;

 

		   Pen^ redPen = gcnew Pen(System::Drawing::Color::Red);

		   redPen->Width = 4;

		   g->DrawLine(redPen, 0, 0, rect.Width, rect.Height);

 

		   Pen^ bluePen = gcnew Pen(System::Drawing::Color::Blue);

		   bluePen->Width = 10;

		   g->DrawArc( bluePen, smallRect, 90, 270 );
		   /*
			System::Drawing::SolidBrush^ myBrush = gcnew System::Drawing::SolidBrush(System::Drawing::Color::Red);
			System::Drawing::Graphics^ formGraphics;
			formGraphics = this->CreateGraphics();
			formGraphics->FillRectangle(myBrush, System::Drawing::Rectangle(10, 120, 25, 25));
			delete myBrush;
			delete formGraphics;*/
		}

};
}

