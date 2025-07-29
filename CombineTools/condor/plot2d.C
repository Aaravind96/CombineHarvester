#include <iostream>
#include <fstream>
#include <string>
#include <list>

void plot2d()
{

  TCanvas *c1 = new TCanvas("c1","c1",0,0,600,600);
  c1->SetFillColor(0);
  c1->GetFrame()->SetFillColor(0);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);
  c1->SetRightMargin(0.2);
  gStyle->SetPalette(kLightTemperature);

  float m1values[6] = {12.5, 17.5, 25, 35, 45, 55};
  float m2values[11] = {15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115};
  TH2F   *hist       = new TH2F("", "", 5, m1values, 10, m2values);

  double x;
  double y;
  double z;


  hist->GetXaxis()->SetTitle("m_{a_{1}} [GeV]");
  hist->GetXaxis()->CenterTitle();
  hist->GetXaxis()->SetTitleOffset(1.2);

  hist->GetYaxis()->SetTitle("m_{a_{2}} [GeV]");
  hist->GetYaxis()->CenterTitle();
  hist->GetYaxis()->SetTitleOffset(1.25);

  hist->GetZaxis()->SetTitle("95% CL upper limit on B(H#rightarrow a_{1} a_{2} #rightarrow4#tau2b/2#tau2b) (%)");
  hist->GetZaxis()->CenterTitle();  // Optional: Center the title
  hist->GetZaxis()->SetTitleOffset(1.4);  // Adjust to push the title to the right
  hist->GetZaxis()->SetTitleSize(0.04);
  hist->GetZaxis()->SetLabelSize(0.035);
  hist->SetMaximum(100.);

  fstream file;
  file.open("median_limits.txt", ios::in);

  while(1)
  {
        file >> y >> x >> z;
        cout<<x<<"\t"<<y<<"\t"<<z<<endl;
        hist->Fill(x, y, z);
        if(file.eof()) break;

  }

  file.close();

  hist->SetStats(0);
  hist->Draw("COLZ");
  hist->Draw("TEXT45 same");
  c1->SaveAs("/eos/user/p/pdas/www/Ha1a2/limits/280725/plotLimit_2d.png");
  c1->SaveAs("/eos/user/p/pdas/www/Ha1a2/limits/280725/plotLimit_2d.pdf");
}

