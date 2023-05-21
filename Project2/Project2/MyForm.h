#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv\cv.h>	//trong thu vien
#include <opencv\highgui.h>
#include <opencv\ml.h>	//nhan dang ki tu
#include <opencv\cxcore.h>
#include <stdio.h>
#include "PlateFinder.h"	//trong file ta dang code

namespace Project2 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	private:
		IplImage* src;	//chua anh goc
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
			if (src)
			{
				src = NULL;
			}
		}
	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::PictureBox^ pictureBox1;

	protected:

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
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(289, 404);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Browse";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::btnBrowse_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(382, 404);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 1;
			this->button2->Text = L"Recognize";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::recognize_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(11, 12);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(487, 355);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 2;
			this->pictureBox1->TabStop = false;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(511, 452);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: char* ConvertStringtoChar(System::String^ str)
		{
			char* str2 = (char*) (void*) Marshal::StringToHGlobalAnsi(str);
			return str2;
		}
	private: System::Void btnBrowse_Click(System::Object^ sender, System::EventArgs^ e) {

		OpenFileDialog^ dlg = gcnew OpenFileDialog;
		dlg->Filter = "Image (*.bmp; *.jpg; *.jpeg; *.png) | *.bmp; *.jpg; *.jpeg; *.png|All files (*.*)|*.*||";

		if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel)
		{
			return;
		}
		src = cvLoadImage(ConvertStringtoChar(dlg -> FileName));	//bien truyen vao la tring ten anh, ham cvLoadImage() chi hieu duoc tham so truyen vao la kieu char nen ta tao them mot ham chuyen kieu du lieu

		Bitmap^ bmp = gcnew Bitmap(dlg->FileName);
		pictureBox1 ->Image = bmp;
		pictureBox1 -> Refresh();

		//PlateFinder pf;
		//IplImage* plate = pf.FindPlate(src);
		//pictureBox2 ->Image = plate;
		//pictureBox2->Refresh();
	}
private: System::Void recognize_Click(System::Object^ sender, System::EventArgs^ e) {
	PlateFinder pf;

	if (!src)
	{
		MessageBox::Show("No image loaded", "Error", MessageBoxButtons::OK);
	}
	IplImage* resizeImg = cvCreateImage(cvSize(800, 600), src->depth, src->nChannels);	//Anh resize
	cvResize(src, resizeImg);

	//Convert sang anh xam//////////////////////////////
	
	//IplImage* grayImg = cvCreateImage(cvGetSize(resizeImg), IPL_DEPTH_8U, 1);	//Anh resize
	//cvCvtColor(resizeImg, grayImg, CV_RGB2GRAY);
	//cvNormalize(grayImg, grayImg, 0, 255, CV_MINMAX);

	//pf.ImageRestoration(grayImg);
	pf.FindPlate(resizeImg);

}
private: System::Void pictureBox2_Click(System::Object^ sender, System::EventArgs^ e) {
	
	
}
};
}
