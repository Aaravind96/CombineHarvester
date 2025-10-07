//adapted from Kevin Pedro's plotLimit.C code from 2018 CMSDAS, and from https://github.com/samhiggie/cppLimits/blob/main/haa/scripts/plotLimit.C

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TMarker.h>
#include <TBox.h>
#include <TLine.h>
#include <TMath.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TVirtualPadPainter.h>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <iterator>

#include "plot.h"

using namespace std;

void multiplyXsec(double* rvals, const vector<double>& xsecs){
    for(unsigned i = 0; i < xsecs.size(); ++i){
        rvals[i] = rvals[i]*xsecs[i];
    }
}

void scaleArray(double* vals, int nPoints, double scale = 1.0) {
    for(unsigned i = 0; i < nPoints; ++i){
        vals[i] = vals[i] * scale;
        std::cout << vals[i] << std::endl;
    }
}

TGraph* getBand(TTree* limit, double q_dn, double q_up, const vector<double>& xsecs, double scale = 1.0){
    stringstream ss_dn;
    ss_dn << "abs(quantileExpected-" << q_dn << ")<0.01";
    int npts = limit->Draw("limit:mh",ss_dn.str().c_str(), "goff");
    std::cout << "npts " << npts << std::endl;
    double* rtmp_dn = limit->GetV1();
    double* mtmp_dn = limit->GetV2();
    // multiplyXsec(rtmp_dn,xsecs);
    if (scale != 1.0) {
        scaleArray(rtmp_dn, npts, scale);
    }

    double* rtmp = new double[npts*2];
    double* mtmp = new double[npts*2];
    for(int m = 0; m < npts; ++m){
        rtmp[npts*2-1-m] = rtmp_dn[m];
        mtmp[npts*2-1-m] = mtmp_dn[m];
    }

    stringstream ss_up;
    ss_up << "abs(quantileExpected-" << q_up << ")<0.01";
    npts = limit->Draw("limit:mh",ss_up.str().c_str(), "goff");
    double* rtmp_up = limit->GetV1();
    double* mtmp_up = limit->GetV2();
    // multiplyXsec(rtmp_up,xsecs);
    if (scale != 1.0) {
        scaleArray(rtmp_up, npts,
        scale);
    }

    for(int m = 0; m < npts; ++m){
        rtmp[m] = rtmp_up[m];
        mtmp[m] = mtmp_up[m];
    }

    TGraph* gtmp = new TGraph(npts*2,mtmp,rtmp);
    return gtmp;
}

void getRange(int n, double* arr, double& ymin, double& ymax){
    double ymin_ = TMath::MinElement(n,arr);
    if(ymin_ < ymin) ymin = ymin_;

    double ymax_ = TMath::MaxElement(n,arr);
    if(ymax_ > ymax) ymax = ymax_;
}

// usage:
// root -l -b -q 'plotLimitsAsymm.C+("cascade/higgsCombine_a1a2_4b2t_2018_mutau_m1_15.root", "4b2t", "2018", "mutau", "15", 2, false)
void plotLimitsAsymm(string infile, string signame, string year, string ch, string m1, int nsigma=0, bool showObserved=false){
    //cross section values
    vector<double> masses = {};
    vector<double> xsecs = {};
    double xsecVal = 48.6+3.78; //(ggH + VBF in pb)

    if ((signame == "4b2t") && (m1 == "15")) {
        masses = {30, 40, 50, 60, 70, 80, 90, 100, 110};
        xsecs = {xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal};
    }
    else if ((signame == "4b2t") && (m1 == "20")) {
        masses = {40, 50, 60, 70, 80, 90, 100};
        xsecs = {xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal};
    }
    else if ((signame == "4b2t") && (m1 == "30")) {
        masses = {60, 70, 80, 90};
        xsecs = {xsecVal,xsecVal,xsecVal,xsecVal};
    }
    else if ((signame == "2b2t") && (m1 == "15")) {
        masses = {20, 30};
        xsecs = {xsecVal,xsecVal};
    }
    else if ((signame == "2b2t") && (m1 == "20")) {
        masses = {30, 40};
        xsecs = {xsecVal,xsecVal};
    }
    else if ((signame == "2b2t") && (m1 == "30")) {
        masses = {40, 50, 60};
        xsecs = {xsecVal,xsecVal,xsecVal};
    }
    else if ((signame == "2b2t") && (m1 == "40")) {
        masses = {50, 60, 70, 80};
        xsecs = {xsecVal,xsecVal,xsecVal,xsecVal};
    }
    else if ((signame == "2b2t") && (m1 == "50")) {
        masses = {60, 70};
        xsecs = {xsecVal,xsecVal};
    }
    //ranges for plotting
    double ymin = 1e10, xmin = 1e10;
    double ymax = 0, xmax = 0;
    gStyle->SetOptStat(0);

    //extract info from hadded limit file
    TFile* file = TFile::Open(infile.c_str());
    if (!file) {
        cout << "Couldn't open " << infile << endl;
        return;
    }
    TTree* limit = (TTree*)file->Get("limit");
    if(!limit) {
        cout << "Couldn't get limit tree from " << infile << endl;
        return;
    }
    //setup plotting options
    string process, yname, xname, channel, description;

    if (signame == "4b2t") {
        process = "h #rightarrow a_{1} a_{2} #rightarrow 2#tau4b";
        yname = "B(H#rightarrow a_{1} a_{2} #rightarrow 2#tau4b) (\%)";
    }
    else if (signame == "2b2t") {
        process = "h #rightarrow a_{1} a_{2} #rightarrow 2#tau2b";
        yname = "B(H#rightarrow a_{1} a_{2} #rightarrow 2#tau2b) (\%)";
    }

    if (ch == "mutau") { channel = "#mu#tau_{h} channel"; }
    else if (ch == "etau") { channel = "e#tau_{h} channel"; }
    else if (ch == "emu") { channel = "e#mu channel"; }
    else if (ch == "allchannels") { channel = "Combined"; }

    xname = "m_{a_{2}} [GeV]";
    description = "m_{a_{1}} = " + m1 + " GeV";

    //initialize legend
    double legsize = 0.04;
    double legx1 = 0.54;
    double legx2 = 0.88;
    double legy2 = 0.93;
    double legy1 = legy2-legsize*(4+nsigma+0.5);
    TLegend* leg = new TLegend(legx1,legy1,legx2,legy2);
    leg->SetFillColorAlpha(0, 0);
    leg->SetBorderSize(0);
    leg->SetTextSize(legsize);
    leg->SetTextFont(42);
    leg->SetMargin(0.15);

    //initialize pave
    double pavex1 = 0.17;
    double pavex2 = 0.45;
    double pavey2 = 0.9;
    double pavey1 = pavey2-legsize*4;
    TPaveText* pave = new TPaveText(pavex1,pavey1,pavex2,pavey2,"NDC");
    pave->SetFillColor(0);
    pave->SetBorderSize(0);
    pave->SetTextSize(legsize);
    pave->SetTextFont(42);
    pave->SetTextAlign(12);
    pave->AddText(process.c_str());
    pave->AddText(channel.c_str());
    pave->AddText(description.c_str());

    //preamble of legend
    leg->AddEntry((TObject*)NULL,"95% CL upper limits","");

    //get cross section
    TGraph* g_xsec = new TGraph(xsecs.size(),masses.data(),xsecs.data());
    g_xsec->SetLineColor(kMagenta);
    g_xsec->SetLineStyle(1);
    g_xsec->SetLineWidth(2);
    getRange(xsecs.size(),xsecs.data(),ymin,ymax);
    //only get x range once
    getRange(masses.size(),masses.data(),xmin,xmax);

    //get observed limit
    double percentageScale = 100;
    int npts = limit->Draw(Form("limit*%g:limitErr*%g:mh", percentageScale, percentageScale), "quantileExpected==-1", "goff");
    double* rtmp = limit->GetV1();
    double* rerrtmp = limit->GetV2();
    double* mtmp = limit->GetV3();

    // multiplyXsec(rtmp,xsecs);
    TGraph* g_obs = new TGraph(npts,mtmp,rtmp);
    g_obs->SetMarkerColor(kBlack);
    g_obs->SetLineColor(kBlack);
    g_obs->SetMarkerStyle(20);
    g_obs->SetLineStyle(1);
    g_obs->SetLineWidth(2);
    if (showObserved) {
        leg->AddEntry(g_obs,"Observed","pl");
    }
    getRange(npts,rtmp,ymin,ymax);

    std::ofstream myfile;
    myfile.open("median_limits_"+ch+".txt", ios::in|ios::ate);

    for(int m = 0; m < npts; ++m){
	if (signame == "4b2t" && ((std::abs(mtmp[m]) == 30 && m1 == "15") || (mtmp[m] == 40 && m1 == "20") || (mtmp[m] == 60 && m1 == "30"))) continue; // skip duplicate mass points with higher limits
        myfile<<mtmp[m]<<"\t"<<m1<<"\t"<<(rtmp[m]*xsecVal/percentageScale)<<std::endl; // convert to cross section
    }

    myfile.close();

    //get central value (expected)
    int nptsC = limit->Draw(Form("limit*%g:mh", percentageScale), "quantileExpected==0.5", "goff");
    double* rtmpC = limit->GetV1();
    double* mtmpC = limit->GetV2();

    // multiplyXsec(rtmpC,xsecs);
    TGraph* g_central = new TGraph(npts,mtmpC,rtmpC);
    g_central->SetLineColor(kBlue);
    g_central->SetLineStyle(2);
    g_central->SetLineWidth(4);
    leg->AddEntry(g_central,"Median expected","l");
    getRange(npts,rtmpC,ymin,ymax);

    //get bands (expected)
    TGraph* g_one = NULL;
    if(nsigma>=1){
        g_one = getBand(limit, 0.16, 0.84, xsecs, percentageScale);
        int cOne = TColor::GetColor("#85D1FBff");
        g_one->SetFillColor(cOne);
        leg->AddEntry(g_one,"68% expected","f");
        getRange(npts*2,g_one->GetY(),ymin,ymax);
    }
    TGraph* g_two = NULL;
    if(nsigma>=2){
        g_two = getBand(limit, 0.025, 0.975, xsecs, percentageScale);
        int cTwo = TColor::GetColor("#FFDF7Fff");
        g_two->SetFillColor(cTwo);
        leg->AddEntry(g_two,"95% expected","f");
        getRange(npts*2,g_two->GetY(),ymin,ymax);
    }

    //extend range
    ymax = ymax*2;
    ymin = ymin/2;

    //make histo for axes
    TH1F* hbase = new TH1F("hbase","",100,xmin,xmax);
    hbase->GetYaxis()->SetMaxDigits(4);
    hbase->GetYaxis()->SetTitle(yname.c_str());
    hbase->GetYaxis()->SetTitleOffset(1.7);
    hbase->GetXaxis()->SetTitle(xname.c_str());
    hbase->GetXaxis()->SetTitleOffset(1.2);
    hbase->GetYaxis()->SetRangeUser(0, 300);
    if (signame == "2b2t" && m1 != "40") hbase->GetYaxis()->SetRangeUser(0, 80);
    if (signame == "2b2t" ) hbase->GetYaxis()->SetRangeUser(0, 30);
    if (signame == "2b2t"  && m1 == "40") hbase->GetYaxis()->SetRangeUser(0, 80);
    hbase->GetYaxis()->SetMaxDigits(6);

    //make plot
    cout<<"the year is "<<year<<endl;
    int lumi = 10;
    if (year=="2016preVFP"){
      lumi = 19500;
    }
    if (year=="2016postVFP"){
      lumi = 16800;
    }
    if(year=="2017"){
      lumi = 41500;
    }
    if(year=="2018"){
      lumi = 59800;
    }
    if(year=="allyears"){
      lumi = 138000;
    }

    Plot plot("plotLimit_"+year+"_a1a2_"+signame+"_m1_"+m1+"_"+ch,lumi,false,false);
    plot.Initialize(hbase);
    plot.SetLegend(leg);
    TCanvas* can = plot.GetCanvas();
    TPad* pad1 = plot.GetPad1();
    pad1->cd();

    //draw blank histo for axes
    plot.DrawHist();

    //draw graphs
    if(nsigma>=2) g_two->Draw("f same");
    if(nsigma>=1) g_one->Draw("f same");
    g_central->Draw("L same");
    if (showObserved) {
        g_obs->Draw("pL same");
    }

    plot.GetHisto()->Draw("sameaxis"); //draw again so axes on top
    plot.DrawText();
    pave->Draw("same");
    gPad->RedrawAxis();
    std::string title = plot.GetName();
    if (showObserved) {
        title += "_unblinded";
    }
    can->Print((title+".png").c_str(),"png");
    can->Print((title+".pdf").c_str(),"pdf");
}
