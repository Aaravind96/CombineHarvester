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

#include "plotunroll.h"

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

TGraph* getBand(TTree* limit, double q_dn, double q_up, const vector<double>& xsecs, vector<double> masses_i, double scale = 1.0){
    stringstream ss_dn;
    ss_dn << "abs(quantileExpected-" << q_dn << ")<0.01";
    int npts = limit->Draw("limit:mh",ss_dn.str().c_str(), "goff");
    std::cout<< "\nnpts in getBand: " << npts;
    double* rtmp_dn = limit->GetV1();
    double* mtmp_dn = limit->GetV2();
    std::cout<< "\n rtmp_dn in getBand: " << limit->GetV1();
    multiplyXsec(rtmp_dn,xsecs);
    if (scale != 1.0) {
        scaleArray(rtmp_dn, npts, scale);
    }

    double* rtmp = new double[npts*2];
    double* mtmp = new double[npts*2];
    double* masses_tmp = new double[npts*2];
    for(int m = 0; m < npts; ++m){
        rtmp[npts*2-1-m] = rtmp_dn[m];
        mtmp[npts*2-1-m] = mtmp_dn[m];
        masses_tmp[npts*2-1-m] = masses_i[m];
    }

    stringstream ss_up;
    ss_up << "abs(quantileExpected-" << q_up << ")<0.01";
    npts = limit->Draw("limit:mh",ss_up.str().c_str(), "goff");
    double* rtmp_up = limit->GetV1();
    double* mtmp_up = limit->GetV2();
    multiplyXsec(rtmp_up,xsecs);
    if (scale != 1.0) {
        scaleArray(rtmp_up, npts,
        scale);
    }

    for(int m = 0; m < npts; ++m){
        rtmp[m] = rtmp_up[m];
        mtmp[m] = mtmp_up[m];
        masses_tmp[m] = masses_i[m];
    }
    std::cout<<"Number of points: "<<npts<<"\nMedian: "<<mtmp;
    TGraph* gtmp = new TGraph(npts*2,masses_tmp,rtmp);
    return gtmp;
}

void getRange(int n, double* arr, double& ymin, double& ymax){
    double ymin_ = TMath::MinElement(n,arr);
    std::cout<<"ymin_"<<ymin_;
    if(ymin_ < ymin) ymin = ymin_;

    double ymax_ = TMath::MaxElement(n,arr);
    std::cout<<"ymax_"<<ymax_;
    if(ymax_ > ymax) ymax = ymax_;
    std::cout<<"\nymin: "<<ymin<<" ymax: "<<ymax;
}

// usage:
// root -l -b -q 'plotLimitsAsymm_ur_sepmasspoints_C.cpp+("bdtbased_root/higgsCombine_a1a2_4b2t_allchannels_combined.root", "4b2t", "allyears", "allchannels", 2, true)'
// infile : input ROOT file path combining all mass points
// sigtype: 4b2t (cascade) or 2b2t (non-cascade), right now the binning needs to be changed by hand for each case \FIXME
// year: individual or allyears
// channel: allchannels, mutau, etau, or emu
// nsigma: sd band being considered
// doLog: y-axis choice
void plotLimitsAsymm_ur_sepmasspoints_C(string infile, string signame, string year, string ch, int nsigma=0, bool doLog=false){
    //cross section values
    vector<double> masses = {};
    vector<double> masses_i = {};
    vector<double> xsecs = {};
    double xsecVal = 48.6+3.78; //(ggH + VBF in pb)

    if ((signame == "4b2t")) {
	masses = {30, 40, 50, 60, 70, 80, 90, 100, 110,40, 50, 60, 70, 80, 90, 100, 60, 70, 80, 90};
        masses_i = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
        xsecs = {xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal };
    }
    else {
	masses = {20,30, 30, 40, 40, 50, 60, 50, 60, 70, 80,70, 80};
        masses_i = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
        xsecs = {xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal,xsecVal };
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
    string process, yname, xname, channel;

    if (signame == "4b2t") {
	process = "H #rightarrow #phi_{1} #phi_{2} #rightarrow 3 #phi_{1} #rightarrow 2#tau4b";
        yname = "#sigma B_{C} [pb]";
    }
    else if (signame == "2b2t") {
        process = "H #rightarrow #phi_{1} #phi_{2}, #phi_{1} #rightarrow 2#tau, #phi_{2} #rightarrow 2b";
        yname = "#sigma B_{NC} [pb]";
    }
    xname ="(m_{#phi_{1}}, m_{#phi_{2}}) [GeV]";

    if (ch == "mutau") { channel = "#mu#tau_{h}"; }
    else if (ch == "etau") { channel = "e#tau_{h}"; }
    else if (ch == "emu") { channel = "e#mu"; }
    else if (ch == "allchannels") { channel = "#mu#tau_{h}, e#tau_{h}, e#mu"; }

    //initialize legend
    double legsize =  0.055;
    double legx1 = 0.54;
    double legx2 = 1.0;
    double legy2 = 0.92;
    double legy1 = legy2-legsize*(4+nsigma+0.5)+ 0.125;
    TLegend* leg = new TLegend(legx1,legy1,legx2,legy2);
    leg->SetFillColorAlpha(0, 0);
    leg->SetBorderSize(0);
    leg->SetTextSize(legsize);
    leg->SetTextFont(42);
    leg->SetMargin(0.15);
    leg->SetNColumns(2);
    leg->SetHeader("95% CL upper limits","C");
    //initialize pave
    double pavex1 = 0.085;
    double pavex2 = 0.32;
    double pavey2 = 0.92;
    double pavey1 = 0.65; // 0.56 for adding "Cut-based", 0.65 otherwise
    TPaveText* pave = new TPaveText(pavex1,pavey1,pavex2,pavey2,"NDC");
    pave->SetFillColorAlpha(0, 0);
    pave->SetBorderSize(0);
    pave->SetTextSize(legsize);
    pave->SetTextFont(42);
    pave->SetTextAlign(12);
    pave->AddText("#bf{CMS}");// #it{Preliminary}");
    pave->AddText(process.c_str());
    pave->AddText(channel.c_str());
    //pave->AddText("Cut-based");

    //get observed limit
    double percentageScale = 1;
    int npts = limit->Draw(Form("limit*%g:limitErr*%g:mh", percentageScale, percentageScale), "quantileExpected==-1", "goff");
    double* rtmp = limit->GetV1();
    double* rerrtmp = limit->GetV2();
    double* mtmp = limit->GetV3();

    //const Int_t n = 20;
    //char const *range[n] = {"(15,30)","(15,40)","(15,50)","(15,60)","(15,70)","(15,80)","(15,90)","(15,100)","(15,110)","(20,40)","(20,50)","(20,60)","(20,70)","(20,80)","(20,90)","(20,100)","(30,60)","(30,70)","(30,80)","(30,90)"};

    //// Change by hand \FIXME
    //if (signame == "2b2t")
    const Int_t n = 13;
    char const *range[n] = {"(15,20)","(15,30)","(20,30)","(20,40)","(30,40)","(30,50)","(30,60)","(40,50)","(40,60)","(40,70)","(40,80)","(50,60)","(50,70)"};

    multiplyXsec(rtmp,xsecs);
    TGraph* g_obs = new TGraph(npts,masses_i.data(),rtmp);
    g_obs->SetMarkerColor(kBlack);
    g_obs->SetLineColor(kBlack);
    g_obs->SetMarkerStyle(20);
    g_obs->SetMarkerSize(2);
    g_obs->SetLineWidth(2);
    leg->AddEntry(g_obs,"Observed","pl");
    getRange(npts,rtmp,ymin,ymax);

    //get central value (expected)
    int nptsC = limit->Draw(Form("limit*%g:mh", percentageScale), "quantileExpected==0.5", "goff");
    double* rtmpC = limit->GetV1();
    double* mtmpC = limit->GetV2();
    for (int i=0;i<nptsC;i++) {
	std::cout<<"\ni for central: "<<i<<", mtmpC: "<<mtmp[i]<<", rtmpC: "<<rtmpC[i];
    }
    multiplyXsec(rtmpC,xsecs);
    TGraph* g_central = new TGraph(npts,masses_i.data(),rtmpC);
    g_central->SetLineColor(kBlack);
    g_central->SetLineStyle(3);
    g_central->SetLineWidth(2);
    leg->AddEntry(g_central,"Median expected","l");
    getRange(npts,rtmpC,ymin,ymax);

    int cOne = TColor::GetColor("#228b22");
    int cTwo = TColor::GetColor("#ffcc00");
    
    //get bands (expected)
    TGraph* g_one = NULL;
    if(nsigma>=1){
        g_one = getBand(limit, 0.16, 0.84, xsecs, masses_i, percentageScale);
        g_one->SetFillColor(cOne);
        leg->AddEntry(g_one,"68% expected","f");
        getRange(npts*2,g_one->GetY(),ymin,ymax);
	std::cout<<"========================\ng_one: "<<g_one->GetY();
    }
    TGraph* g_two = NULL;
    if(nsigma>=2){
        g_two = getBand(limit, 0.025, 0.975, xsecs, masses_i, percentageScale);
        g_two->SetFillColor(cTwo);
        leg->AddEntry(g_two,"95% expected","f");
        getRange(npts*2,g_two->GetY(),ymin,ymax);
    }

    //extend range
    ymax = ymax*2 ;
    ymin = ymin/2;

    //make histo for axes
    TH1F* hbase = new TH1F("hbase","", n+1, 0., n+1);
    std::cout<<"\n\n---------------- \n For hbase, xmin: "<<xmin<<", xmax: "<<xmax<<std::endl;
    hbase->GetYaxis()->SetMaxDigits(4);
    hbase->GetYaxis()->SetTitle(yname.c_str());
    hbase->GetYaxis()->SetTitleOffset(0.6);
    hbase->GetXaxis()->SetTitle(xname.c_str());
    hbase->GetXaxis()->SetTitleOffset(1.7);
    hbase->GetXaxis()->SetTitleSize(0.95);
    hbase->GetYaxis()->SetRangeUser(0, 800);
    hbase->GetXaxis()->SetNdivisions(n, kFALSE);
    hbase->GetXaxis()->SetLabelOffset(1.0);
    hbase->GetXaxis()->CenterLabels(true);
    for (int i=1;i<=n;i++){
        hbase->GetXaxis()->SetBinLabel(i+1,range[i-1]);
    }
    hbase->GetXaxis()->SetRangeUser(0., n+1);
    hbase->LabelsOption("u", "X");
    if (signame == "4b2t" && ch == "allchannels") hbase->GetYaxis()->SetRangeUser(0, 300);
    if (signame == "2b2t") hbase->GetYaxis()->SetRangeUser(0, 90);
    if (doLog) hbase->GetYaxis()->SetRangeUser(0.5, 500);
    if (doLog && signame == "4b2t") hbase->GetYaxis()->SetRangeUser(5, 1000);

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

    Plot plot("plotLimit_"+year+"_a1a2_"+signame+"_"+ch,lumi,false,false);
    plot.Initialize(hbase);
    plot.SetLegend(leg);
    TCanvas* can = plot.GetCanvas();
    TPad* pad1 = plot.GetPad1();
    pad1->cd();
    if (doLog) pad1->SetLogy();

    //draw blank histo for axes
    plot.DrawHist();

    leg->Draw();
    plot.DrawText();
    //draw graphs
    for (int i=1;i<=n;i++) g_central->GetXaxis()->SetBinLabel(i,range[i-1]);
    
    TGraphAsymmErrors *g_central_err =new TGraphAsymmErrors(g_central->GetN());

    for (int i = 0; i < g_obs->GetN(); ++i) {
        double x, y;
        g_central->GetPoint(i, x, y);

        g_central_err->SetPoint(i, x, y);

        // horizontal bar half-width
        g_central_err->SetPointEXlow(i, 0.45);
        g_central_err->SetPointEXhigh(i, 0.45);
        // no vertical error
        g_central_err->SetPointEYlow(i, 0.0);
        g_central_err->SetPointEYhigh(i, 0.0);
    }
    g_central_err->SetLineColor(kBlack);
    g_central_err->SetLineStyle(3);
    g_central_err->SetLineWidth(2);

    for (int i=1;i<=n;i++) g_obs->GetXaxis()->SetBinLabel(i,range[i-1]);
    g_obs->Draw("P same");

    TGraphAsymmErrors *g_obs_err =new TGraphAsymmErrors(g_obs->GetN());

    for (int i = 0; i < g_obs->GetN(); ++i) {
        double x, y;
        g_obs->GetPoint(i, x, y);
        
        g_obs_err->SetPoint(i, x, y);
        
        // horizontal bar half-width
        g_obs_err->SetPointEXlow(i, 0.45);
        g_obs_err->SetPointEXhigh(i, 0.45);
        
        // no vertical error
        g_obs_err->SetPointEYlow(i, 0.0);
        g_obs_err->SetPointEYhigh(i, 0.0);
    }
    g_obs_err->SetMarkerStyle(20);
    g_obs_err->SetMarkerSize(2);
    g_obs_err->SetMarkerColor(kBlack);	
    g_obs_err->SetLineColor(kBlack);
    g_obs_err->SetLineWidth(2);

    TGraphAsymmErrors *g_one_err =new TGraphAsymmErrors(g_one->GetN());
    std::cout<<"\nRange of g_one : "<<g_one->GetN();
    for (int i = 0; i < g_one->GetN()/2; ++i) {
        double x,y, ylow, yhigh;
        g_central->GetPoint(i, x, y);
        g_one->GetPoint(i, x, yhigh);
        g_one->GetPoint(g_one->GetN()-1-i, x, ylow);
        std::cout<<"\n x: "<<x<<", y: "<<yhigh<<"\n";
        g_one_err->SetPoint(i, x, y);

        g_one_err->SetPointEXlow(i, 0.45);
        g_one_err->SetPointEXhigh(i, 0.45);

        g_one_err->SetPointEYlow(i, (y-ylow));
        g_one_err->SetPointEYhigh(i, (yhigh-y));
    }
    g_one_err->SetFillColor(cOne);
    g_one_err->SetLineColor(cOne); 
    g_one_err->SetMarkerSize(0); 
    
    TGraphAsymmErrors *g_two_err =new TGraphAsymmErrors(g_two->GetN());

    for (int i = 0; i < g_two->GetN()/2; ++i) {
        double x, y, ylow, yhigh;
        g_central->GetPoint(i, x, y);
        g_two->GetPoint(i, x, yhigh);
        g_two->GetPoint(g_two->GetN()-1-i, x, ylow);
        g_two_err->SetPoint(i, x, y);

        // horizontal bar half-width
        g_two_err->SetPointEXlow(i, 0.45);
        g_two_err->SetPointEXhigh(i, 0.45);

        // no vertical error
        g_two_err->SetPointEYlow(i, (y-ylow));
        g_two_err->SetPointEYhigh(i,(yhigh-y));
    }
    g_two_err->SetFillStyle(1001);
    g_two_err->SetFillColor(cTwo);
    g_two_err->SetLineColor(0); 
    g_two_err->SetMarkerSize(0); 
    if(nsigma>=2) g_two_err->Draw("2 same");
    if(nsigma>=1) g_one_err->Draw("2 same");

    g_central_err->Draw("E same");
    g_obs_err->Draw("PE same");
    
    TLine* line = new TLine(9.5, 0.0, 9.5, 500);
    line->SetLineColor(kGray+2);
    line->SetLineStyle(3);  
    line->SetLineWidth(1);
    line = new TLine(16.5, 0.0, 16.5, 500);
    line->SetLineColor(kGray+2);
    line->SetLineStyle(3);
    line->SetLineWidth(1);
    pave->Draw("same");
    gPad->RedrawAxis();
    std::string title = plot.GetName();
    if (doLog) {
        can->Print(("/eos/user/p/pdas/www/Ha1a2/dataMC/PAPER/v15/limits/"+title+"_unrolled_bdt_logY.png").c_str(),"png");
        can->Print(("/eos/user/p/pdas/www/Ha1a2/dataMC/PAPER/v15/limits/"+title+"_unrolled_bdt_logY.pdf").c_str(),"pdf");
    }
    else {
        can->Print(("/eos/user/p/pdas/www/Ha1a2/dataMC/PAPER/v15/limits/"+title+"_unrolled_bdt.png").c_str(),"png");
        can->Print(("/eos/user/p/pdas/www/Ha1a2/dataMC/PAPER/v15/limits/"+title+"_unrolled_bdt.pdf").c_str(),"pdf");
    }
}
