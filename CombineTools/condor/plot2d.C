#include <iostream>
#include <fstream>
#include <string>
#include <list>

void plot2d(string ch)
{

  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,600);
  c1->SetFillColor(0);
  c1->GetFrame()->SetFillColor(0);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  c1->SetRightMargin(0.2);
  gStyle->SetPalette(kLightTemperature);
  gStyle->SetPaintTextFormat("4.1f");

  float m1values[6] = {12.5, 17.5, 25, 35, 45, 55};
  float m2values[11] = {15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115};
  TH2F   *hist       = new TH2F("", "", 5, m1values, 10, m2values);

  double x;
  double y;
  double z;


  hist->GetXaxis()->SetTitle("m_{h_{1}} [GeV]");
  hist->GetXaxis()->SetTitleSize(0.04);
  hist->GetXaxis()->CenterTitle();
  hist->GetXaxis()->SetTitleOffset(1.1);

  hist->GetYaxis()->SetTitle("m_{h_{2}} [GeV]");
  hist->GetYaxis()->SetTitleSize(0.04);
  hist->GetYaxis()->CenterTitle();
  hist->GetYaxis()->SetTitleOffset(1.2);

  hist->GetZaxis()->SetTitle("95% CL upper limit on #sigma(H#rightarrow h_{1} h_{2} #rightarrow2#tau4b/2#tau2b) (pb)");
  hist->GetZaxis()->CenterTitle();  // Optional: Center the title
  hist->GetZaxis()->SetTitleOffset(1.4);  // Adjust to push the title to the right
  hist->GetZaxis()->SetTitleSize(0.04);
  hist->GetZaxis()->SetLabelSize(0.035);
  hist->SetMaximum(100.);
  if (ch == "allchannels") { hist->SetMaximum(60.); }
  if (ch == "etau") { hist->SetMaximum(500.); }

  fstream file;
  file.open("median_limits_"+ch+".txt", ios::in);

  while(1)
  {
        file >> y >> x >> z;
        cout<<x<<"\t"<<y<<"\t"<<z<<endl;
        hist->Fill(x, y, z);
        if(file.eof()) break;

  }

  TLatex* text = new TLatex(0.585, 0.91, "138 fb^{-1} (13 TeV)");
  text->SetNDC();
  text->SetTextSize(0.04);
  text->SetTextFont(42);

  string channel;
  if (ch == "mutau") { channel = "#mu#tau_{h} channel"; }
  else if (ch == "etau") { channel = "e#tau_{h} channel"; }
  else if (ch == "emu") { channel = "e#mu channel"; }
  else if (ch == "allchannels") { channel = "Combined"; }
  TPaveText* pave = new TPaveText(0.5,0.78,0.75,0.88,"NDC");
  pave->SetFillColor(0);
  pave->SetBorderSize(0);
  pave->SetTextSize(0.042);
  pave->SetTextFont(42);
  pave->SetTextAlign(12);
  pave->AddText("#bf{CMS} #it{Preliminary}");
  pave->AddText(channel.c_str());

  file.close();

  hist->SetStats(0);
  hist->SetMarkerSize(1.5);
  hist->Draw("COLZ");
  hist->Draw("TEXT45 same");
  pave->Draw("same");
  text->Draw("same");
  c1->RedrawAxis();
  std::string title = "plotLimit_2d_"+ch;
  c1->SaveAs((title+".png").c_str(),"png");
  c1->SaveAs((title+".pdf").c_str(),"pdf");
}

