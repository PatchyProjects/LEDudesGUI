#pragma once
#include <msclr\marshal_cppstd.h>

//typedef std::basic_string<TCHAR, std::char_traits<TCHAR>> tstring;

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

	private: System::Windows::Forms::Button^  tagFrameButton;
	private: System::Windows::Forms::TextBox^  tagFramePathText;
	private: System::Windows::Forms::Label^  TagFrameLabel;

	private: System::Windows::Forms::OpenFileDialog^  openTagFrameDialog;
	private: System::Windows::Forms::CheckBox^  tagFrameCheckBox;
	private: System::Windows::Forms::Button^  stopVideoButton;
	private: System::Windows::Forms::Button^  replayButton;



	private: System::ComponentModel::BackgroundWorker^  backgroundWorkerRefresh;


	public: 

		/****Sends file to the selected LED module****/
		void Connect( System::String^ LEDmodule, List<System::String^>^ data )
		{
			try
			{
				// Create a TcpClient. 

				Int32 port = 2000;
				TcpClient^ client = gcnew TcpClient(LEDmodule, port);

				NetworkStream^ stream = client->GetStream();
				Encoding^ ascii = Encoding::ASCII;

				// Keyword to erase the flash memory on the LED module
				System::String^ Keyword1 = "TECZMOO";
				array<Byte>^ msg1 = ascii->GetBytes(Keyword1);
				
				// Send the message to the connected LED module 
				stream->Write( msg1, 0, msg1->Length );

				// Buffer to store the response bytes.
				array<Byte>^ ack1 = gcnew array<Byte>(256);

				// String to store the response ASCII representation.
				System::String^ response1 = nullptr;
				Int32 l;
				while (l = stream->Read( ack1, 0, ack1->Length ))
				{
					// Read the response bytes from the LED module.
					response1 = ascii->GetString( ack1 );

					if (response1->Contains("ACK"))
					{
						break;
					}
				}

				// Keyword to signal the start of when the file will be sent
				System::String^ Keyword = "TECXMOO";
				array<Byte>^ msg = ascii->GetBytes(Keyword);
				
				// Send file 10 frames at a time
				for (int c = 0; c < data->Count; c++)
				{
					array<Byte>^ byte_frames;
					Encoding^ ascii = Encoding::ASCII;
					byte_frames = ascii->GetBytes(data[c]);

					// Send the frames to the connected LED module. 
					stream->Write( byte_frames, 0, byte_frames->Length );

					// Receive the response. 

					// Buffer to store the response bytes.
					array<Byte>^ ack = gcnew array<Byte>(256);

					// String to store the response ASCII representation.
					System::String^ response = nullptr;
					Int32 k;
					while (k = stream->Read( ack, 0, ack->Length ))
					{
						// Read the response bytes.
						response = ascii->GetString( ack );

						if (response->Contains("ACK"))
						{
							break;
						}
					}
				}

				// Keyword to signal that the last of the frames has been sent and the file has been loaded
				System::String^ Keyword2 = "TECYMOO";
				array<Byte>^ msg2 = ascii->GetBytes(Keyword2);

				stream->Write( msg2, 0, msg2->Length );

				// Close everything.
				client->Close();
			}
			catch ( ArgumentNullException^ e ) 
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Error, could not connect, make sure the csv file is formated correctly!"); 
			}
			catch ( SocketException^ e ) 
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Error, could not connect, try refreshing and try again.");
			}
		}

		/****Sends command to the selected LED module****/
		void Connect2( System::String^ LEDmodule, array<Byte>^ data )
		{
			try
			{
				// Create a TcpClient. 
				Int32 port = 2000;
				TcpClient^ client = gcnew TcpClient(LEDmodule, port);
				
				NetworkStream^ stream = client->GetStream();
				
				// Send the message to the connected LED module
				stream->Write( data, 0, data->Length );

				// Close everything.
				client->Close();
			}
			catch ( ArgumentNullException^ e ) 
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Error, could not connect, make sure the csv file is formated correctly!"); 
			}
			catch ( SocketException^ e ) 
			{
				System::String^ s = System::Convert::ToString(e);
				MessageBox::Show("Error, could not connect, try refreshing and try again.");
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
				for (int i = 0; i < 5; i++)
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
			this->replayButton = (gcnew System::Windows::Forms::Button());
			this->stopVideoButton = (gcnew System::Windows::Forms::Button());
			this->tagFrameCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->tagFrameButton = (gcnew System::Windows::Forms::Button());
			this->tagFramePathText = (gcnew System::Windows::Forms::TextBox());
			this->TagFrameLabel = (gcnew System::Windows::Forms::Label());
			this->processButton = (gcnew System::Windows::Forms::Button());
			this->videoBrowseButton = (gcnew System::Windows::Forms::Button());
			this->videoPathText = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->WifiPage = (gcnew System::Windows::Forms::TabPage());
			this->repeatButton = (gcnew System::Windows::Forms::Button());
			this->openVideoDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->openTagFrameDialog = (gcnew System::Windows::Forms::OpenFileDialog());
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
			this->loadButton->Location = System::Drawing::Point(221, 104);
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
			this->tabControl->Size = System::Drawing::Size(331, 280);
			this->tabControl->TabIndex = 9;
			// 
			// VideoPage
			// 
			this->VideoPage->BackColor = System::Drawing::Color::WhiteSmoke;
			this->VideoPage->Controls->Add(this->replayButton);
			this->VideoPage->Controls->Add(this->stopVideoButton);
			this->VideoPage->Controls->Add(this->tagFrameCheckBox);
			this->VideoPage->Controls->Add(this->tagFrameButton);
			this->VideoPage->Controls->Add(this->tagFramePathText);
			this->VideoPage->Controls->Add(this->TagFrameLabel);
			this->VideoPage->Controls->Add(this->processButton);
			this->VideoPage->Controls->Add(this->videoBrowseButton);
			this->VideoPage->Controls->Add(this->videoPathText);
			this->VideoPage->Controls->Add(this->label2);
			this->VideoPage->Location = System::Drawing::Point(4, 22);
			this->VideoPage->Name = L"VideoPage";
			this->VideoPage->Padding = System::Windows::Forms::Padding(3);
			this->VideoPage->Size = System::Drawing::Size(323, 254);
			this->VideoPage->TabIndex = 0;
			this->VideoPage->Text = L"Video Processor";
			this->VideoPage->Click += gcnew System::EventHandler(this, &Form1::tabPage1_Click);
			this->VideoPage->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::VideoPage_Paint);
			// 
			// replayButton
			// 
			this->replayButton->Location = System::Drawing::Point(161, 187);
			this->replayButton->Name = L"replayButton";
			this->replayButton->Size = System::Drawing::Size(75, 60);
			this->replayButton->TabIndex = 9;
			this->replayButton->Text = L"Replay";
			this->replayButton->UseVisualStyleBackColor = true;
			this->replayButton->Click += gcnew System::EventHandler(this, &Form1::replayButton_Click);
			// 
			// stopVideoButton
			// 
			this->stopVideoButton->Location = System::Drawing::Point(242, 187);
			this->stopVideoButton->Name = L"stopVideoButton";
			this->stopVideoButton->Size = System::Drawing::Size(72, 60);
			this->stopVideoButton->TabIndex = 8;
			this->stopVideoButton->Text = L"Stop";
			this->stopVideoButton->UseVisualStyleBackColor = true;
			this->stopVideoButton->Click += gcnew System::EventHandler(this, &Form1::stopVideoButton_Click);
			// 
			// tagFrameCheckBox
			// 
			this->tagFrameCheckBox->AutoSize = true;
			this->tagFrameCheckBox->Location = System::Drawing::Point(15, 141);
			this->tagFrameCheckBox->Name = L"tagFrameCheckBox";
			this->tagFrameCheckBox->Size = System::Drawing::Size(99, 17);
			this->tagFrameCheckBox->TabIndex = 7;
			this->tagFrameCheckBox->Text = L"Use Tag Frame";
			this->tagFrameCheckBox->UseVisualStyleBackColor = true;
			// 
			// tagFrameButton
			// 
			this->tagFrameButton->Location = System::Drawing::Point(221, 137);
			this->tagFrameButton->Name = L"tagFrameButton";
			this->tagFrameButton->Size = System::Drawing::Size(93, 23);
			this->tagFrameButton->TabIndex = 6;
			this->tagFrameButton->Text = L"Browse";
			this->tagFrameButton->UseVisualStyleBackColor = true;
			this->tagFrameButton->Click += gcnew System::EventHandler(this, &Form1::tagFrameButton_Click);
			// 
			// tagFramePathText
			// 
			this->tagFramePathText->Location = System::Drawing::Point(12, 111);
			this->tagFramePathText->Name = L"tagFramePathText";
			this->tagFramePathText->Size = System::Drawing::Size(302, 20);
			this->tagFramePathText->TabIndex = 5;
			// 
			// TagFrameLabel
			// 
			this->TagFrameLabel->AutoSize = true;
			this->TagFrameLabel->Location = System::Drawing::Point(12, 95);
			this->TagFrameLabel->Name = L"TagFrameLabel";
			this->TagFrameLabel->Size = System::Drawing::Size(148, 13);
			this->TagFrameLabel->TabIndex = 4;
			this->TagFrameLabel->Text = L"Choose Tag Frame (Optional):";
			// 
			// processButton
			// 
			this->processButton->Location = System::Drawing::Point(12, 187);
			this->processButton->Name = L"processButton";
			this->processButton->Size = System::Drawing::Size(143, 60);
			this->processButton->TabIndex = 3;
			this->processButton->Text = L"Process";
			this->processButton->UseVisualStyleBackColor = true;
			this->processButton->Click += gcnew System::EventHandler(this, &Form1::processButton_Click);
			// 
			// videoBrowseButton
			// 
			this->videoBrowseButton->Location = System::Drawing::Point(221, 59);
			this->videoBrowseButton->Name = L"videoBrowseButton";
			this->videoBrowseButton->Size = System::Drawing::Size(93, 23);
			this->videoBrowseButton->TabIndex = 2;
			this->videoBrowseButton->Text = L"Browse";
			this->videoBrowseButton->UseVisualStyleBackColor = true;
			this->videoBrowseButton->Click += gcnew System::EventHandler(this, &Form1::videoBrowseButton_Click);
			// 
			// videoPathText
			// 
			this->videoPathText->Location = System::Drawing::Point(12, 33);
			this->videoPathText->Name = L"videoPathText";
			this->videoPathText->Size = System::Drawing::Size(302, 20);
			this->videoPathText->TabIndex = 1;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(9, 15);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(95, 13);
			this->label2->TabIndex = 0;
			this->label2->Text = L"Choose Video File:";
			// 
			// WifiPage
			// 
			this->WifiPage->BackColor = System::Drawing::Color::WhiteSmoke;
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
			this->WifiPage->Size = System::Drawing::Size(323, 254);
			this->WifiPage->TabIndex = 1;
			this->WifiPage->Text = L"LED Output";
			// 
			// repeatButton
			// 
			this->repeatButton->Location = System::Drawing::Point(9, 224);
			this->repeatButton->Name = L"repeatButton";
			this->repeatButton->Size = System::Drawing::Size(287, 25);
			this->repeatButton->TabIndex = 9;
			this->repeatButton->Text = L"Repeat";
			this->repeatButton->UseVisualStyleBackColor = true;
			this->repeatButton->Click += gcnew System::EventHandler(this, &Form1::repeatButton_Click);
			// 
			// openTagFrameDialog
			// 
			this->openTagFrameDialog->FileName = L"openTagFrameDialog";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(331, 283);
			this->Controls->Add(this->tabControl);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(2);
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->Text = L"Light Emitting Software";
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

				 // A list of strings that contain the values for every chunk of 10 frames
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

				 // Store the last few frames if there are less than 10 frames left
				 if (frames != "")
				 {
					 frame_strings->Add(frames);
				 }

				 // If the last string in the list is an empty string, remove it 
				 if (frame_strings[(frame_strings->Count)-1] == "")
				 {
					 frame_strings->RemoveAt((frame_strings->Count)-1);
				 }

				 // Send the csv data to the LED module
				 Connect(IPadd[j],frame_strings);
			 }
			 MessageBox::Show("Load process ended");
		 }
private: System::Void refreshButton_Click(System::Object^  sender, System::EventArgs^  e) {

			 backgroundWorkerRefresh->RunWorkerAsync();
		 }
private: System::Void wifiListBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 listboxclick = 1;

			 fileSelectedTextBox->Text = Files[wifiListBox->SelectedIndex];
		 }
private: System::Void playButton_Click(System::Object^  sender, System::EventArgs^  e) {

			
				 Keyword = "TECKMOO";
				 Encoding^ ascii = Encoding::ASCII;
				 // Translate the passed message into ASCII and store it as a Byte array. 
				 array<Byte>^ message = ascii->GetBytes(Keyword);

				 for (int j = 0; j < Files->Length; j++)
				 {
					 Connect2(IPadd[j], message);
				 }
			 
		 }
private: System::Void stopButton_Click(System::Object^  sender, System::EventArgs^  e) {

			Keyword = "TECQMOO";
			 Encoding^ ascii = Encoding::ASCII;
			 // Translate the passed message into ASCII and store it as a Byte array. 
			 array<Byte>^ message = ascii->GetBytes(Keyword);

			 for (int j = 0; j < Files->Length; j++)
			 {
				 Connect2(IPadd[j], message);
				 Connect2(IPadd[j], message);
				 Connect2(IPadd[j], message);
			 } 
		 }
private: System::Void repeatButton_Click(System::Object^  sender, System::EventArgs^  e) {
			
				 Keyword = "TECRMOO";
				 Encoding^ ascii = Encoding::ASCII;
				 // Translate the passed message into ASCII and store it as a Byte array. 
				 array<Byte>^ message = ascii->GetBytes(Keyword);

				 for (int j = 0; j < Files->Length; j++)
				 {
					 Connect2(IPadd[j], message);
				 }
			 
		 }
private: System::Void backgroundWorkerRefresh_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {

			 wifiListBox->Items->Clear();
			 listboxclick = 0;

			 /****Pings a list if IP addresses to fill listbox with all active modules****/
			 System::String^ left = "170.255.1.";
			 int rightInt = 1;
			 System::String^ right;
			 for (int i = 0; i < 5; i++)
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

			 fileSelectedTextBox->Text = "Please select a Module";
			 listboxclick = 0;
			 MessageBox::Show("Refreshed!");

		 }
		private: System::Void tabPage1_Click(System::Object^  sender, System::EventArgs^  e) {

		 }


/*******************************************************************************************
******			Video Processing GUI										****************
*******************************************************************************************/
			private: static bool stopVidFlag = false;
		/*private: static vector<vector<int>> rVals;		// vars used for replay
		private: static	vector<vector<int>> gVals;
		private: static	vector<vector<int>> bVals;
		private: static Mat savedFrame;
		private: static FrameProcessor processFrame;
				 */

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

		private: std::string getFilePath(std::string filePathAndExt)
		{
			std::string filePath;
			unsigned found = filePathAndExt.find_last_of("/\\");
			filePath = filePathAndExt.substr(0,found);
			/*
			for(int i = found+1; i<filePathAndExt.size(); i++)
			{
				if(filePathAndExt[i] == '.')
					break;
				fileName += filePathAndExt[i];
			}*/
			return filePath;
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
			this->stopVidFlag = false;
			using System::Runtime::InteropServices::Marshal;

			System::String^ managedString = this->videoPathText->Text;

			msclr::interop::marshal_context context;
			std::string videoFile = context.marshal_as<std::string>(managedString);

			//cvStartWindowThread();

			VideoCapture cap(videoFile);		//Open Video File
			if(!cap.isOpened())					// check if we succeeded
			{

				return -1;
			}
			
			FrameProcessor processFrame;

			namedWindow("video",1);

			//cout<<"Processing Video, Please Wait..."<<endl;
			int frameCount = cap.get(CV_CAP_PROP_FRAME_COUNT);

			Mat firstFrame;
			cap >> firstFrame;
			//this->savedFrame = firstFrame;

			TCHAR exePath[MAX_PATH];
			
			if( !GetModuleFileName( NULL, exePath, MAX_PATH ) )
			{
				//printf("Cannot install service (%d)\n", GetLastError());
				return -1;
			}
			
			std::string frameFileName = exePath;
			frameFileName = getFilePath(frameFileName)+"\\Frame Files\\"+ getFileName(videoFile); 

			if(this->tagFrameCheckBox->Checked)		// User is using a tag frame
			{
				System::String^ managedTagString = this->tagFramePathText->Text;
				msclr::interop::marshal_context tagContext;
				std::string tagFile = tagContext.marshal_as<std::string>(managedTagString);
				
				frameFileName = frameFileName+"_"+getFileName(tagFile);						// if tag frame is used, output file
																							// is videoName_tagFrameName.csv

				Mat image;
				image = imread(tagFile, CV_LOAD_IMAGE_COLOR);   // Read the tag file
				processFrame.locateLEDLocations(image);
				processFrame.scaleTagLocations(firstFrame,image);
			}
			else
			{
				processFrame.basicLayout(firstFrame);
			}

			frameFileName = frameFileName + ".csv";				// add extension to output file name
			std::ofstream frameFile;										// open file stream to write output
			frameFile.open (frameFileName);
			if (!frameFile.is_open()) //error opening writefile
			{
				//cout<<"Error Opening CSV file"<<endl;
				return -1;
			}

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
				frameFile<<processFrame.getFrameString();
				
			
				if( ( (i+1)%10 == 0 && i!=0) || i == frameCount-2 || this->stopVidFlag)
				{
					frameFile<<"H";
					if(this->stopVidFlag)
					{	
						break;
					}
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

			

			vector<vector<int>> rVals = processFrame.processedRVals;
			vector<vector<int>> gVals = processFrame.processedGVals;
			vector<vector<int>> bVals = processFrame.processedBVals;
			
			for(int replay = 0; replay< 1; replay++)			// Replay Video
			{
				for(int frameNum = 0; frameNum<rVals.size(); frameNum++)
				{
					processFrame.replayVideo(firstFrame, rVals[frameNum], gVals[frameNum], bVals[frameNum]);		//Reuse first frame to draw on
					if(this->stopVidFlag)
					{
						cvDestroyAllWindows();
						break;
					}
				}
				
				if(this->stopVidFlag)
				{
					cvDestroyAllWindows();
					break;
				} 
			}

			
			return 0;

		}
		/*
		public: void replayVideo()
		{
				for(int frameNum = 0; frameNum<this->rVals.size(); frameNum++)
				{
					processFrame.replayVideo(savedFrame, this->rVals[frameNum], this->gVals[frameNum], this->bVals[frameNum]);		//Reuse first frame to draw on
					if(this->stopVidFlag)
					{
						break;
					}
				}
				cvDestroyAllWindows();
		}
				*/
		private: System::Void VideoPage_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
		{
			
		}

		private: System::Void stopVideoButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			 this->stopVidFlag = true;
			 cvDestroyAllWindows();
		}
		private: System::Void replayButton_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			this->stopVidFlag = false;
		}
};
}

