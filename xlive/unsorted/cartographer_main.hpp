#pragma once

/*#include <OSHGui.hpp>

class cartographer_main : public OSHGui::Form
{
public:
	cartographer_main();

private:
	void InitializeComponent()
	{
		using namespace OSHGui;
		using namespace OSHGui::Misc;
		using namespace OSHGui::Drawing;

		SetName("cartographer_main");
		SetSize(SizeI(309, 357));
		SetBackColor(Color::FromRGB(0, 0, 139));
		SetText("Catographer");

		tabControl1 = new TabControl();
		tabControl1->SetName("tabControl1");
		tabControl1->SetLocation(PointI(-1, -1));
		tabControl1->SetAutoSize(true);
		tabControl1->SetBackColor(Color::FromRGB(0, 0, 139));
		tabControl1->SetSelectedIndex(1);

		tabPage1 = new TabPage();
		tabPage1->SetName("tabPage1");
		tabPage1->SetBackColor(Color::FromRGB(0, 0, 139));
		tabPage1->SetText("Profile");
		tabControl1->AddTabPage(tabPage1);

		tabPage2 = new TabPage();
		tabPage2->SetName("tabPage2");
		tabPage2->SetBackColor(Color::FromRGB(0, 0, 139));
		tabPage2->SetText("Friends");

		listBox1 = new ListBox();
		listBox1->SetName("listBox1");
		listBox1->SetLocation(PointI(-1, -1));
		listBox1->SetSize(SizeI(309, 357));
		listBox1->SetBackColor(Color::FromRGB(112, 128, 144));
		listBox1->AddItem("SuperSniper");
		listBox1->AddItem("Katanomo-Is-Gay");
		tabPage2->AddControl(listBox1);

		tabControl1->AddTabPage(tabPage2);

		AddControl(tabControl1);
	}

	OSHGui::ListBox *listBox1;
	OSHGui::TabControl *tabControl1;
	OSHGui::TabPage *tabPage1;
	OSHGui::TabPage *tabPage2;
};

*/