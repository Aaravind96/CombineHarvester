#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"
#include "TKey.h"
#include "TClass.h"

using namespace std;

// Add shape unc only when template norms are positive
// and when there are no negative bins in both Up and Down shapes
// e.g. diboson has some negative genweights, fake estimation involves data/MC subtraction...
void addshapes(ch::CombineHarvester* cb, TFile* input_file, vector<pair<int,string>> categories, vector<string> proc_names, string syst_name, float init_value) {
  // Loop over categories
  for (auto categories_itn = categories.begin(); categories_itn != categories.end(); ++categories_itn) {
    string category_name = categories_itn->second;// ch::Categories is of the type vector<pair<int, string>>
    TDirectory* dir = (TDirectory*) input_file->Get(category_name.c_str());
    if (dir == nullptr) {
      cout << "Warning: category " << category_name << " missing in file!" << endl;
      throw;
    }
    // Loop over the given processes
    for (auto proc_names_itn = proc_names.begin(); proc_names_itn != proc_names.end(); ++proc_names_itn) {
      string proc_var = *proc_names_itn;
      TH1F* shapeBase = (TH1F*) dir->Get(proc_var.c_str());
      TH1F* shapeUp = (TH1F*) dir->Get((proc_var + "_" + syst_name + "Up").c_str());
      TH1F* shapeDown = (TH1F*) dir->Get((proc_var + "_" + syst_name + "Down").c_str());
      // Check if each of the given processes has at least the shape templates in file
      // They could be invalid for use but must be there if passed in argument
      if (shapeBase == nullptr) {
	cout << "Warning: the nominal process shape " << proc_var << " in category " << category_name << " does not exist!" << endl;
	throw;
      }
      if (shapeUp == nullptr) {
	cout << "Warning: the Up shape " << proc_var << "_" << syst_name << "Up" << " in category " << category_name << " does not exist!" << endl;
	throw;
      }
      if (shapeDown == nullptr) {
	cout << "Warning: the Down shape " << proc_var << "_" << syst_name << "Down" << " in category " << category_name << " does not exist!" << endl;
	throw;
      }
      // Check if the template shapes have positive norms
      Float_t shapeBase_norm = 0.0;
      Float_t shapeUp_norm = 0.0;
      Float_t shapeDown_norm = 0.0;
      shapeBase_norm = shapeBase->Integral();
      shapeUp_norm = shapeUp->Integral();
      shapeDown_norm = shapeDown->Integral();
      bool HasPositiveNorms = shapeBase_norm > 0.005 and shapeUp_norm > 0.0 and shapeDown_norm > 0.0;            
      if (HasPositiveNorms) {
        cb->cp().bin({category_name}).process({*proc_names_itn}).AddSyst(*cb, syst_name, "shape", ch::syst::SystMap<>::init(init_value));
      }
      else {
        cout << "Skipping shape with non-positive norms: " << syst_name << " for process " << *proc_names_itn << " in category " << category_name << endl;
      }
    }
  }
}

void addfakenorm(ch::CombineHarvester* cb, vector<pair<int,string>> categories, string fakeProcName, string channel_abbrv, string year) {
  for (auto categories_itn = categories.begin(); categories_itn != categories.end(); ++categories_itn) {
    string category_name = categories_itn->second;// ch::Categories is of the type vector<pair<int, string>>
    if (category_name == "SR1_1b" || category_name == "SR2_1b") {
      cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_SR12_1b_"+year, "lnN", ch::syst::SystMap<>::init(1.30));
    }
    else if ((category_name == "SR3_1b" || category_name == "SR4_1b") && channel_abbrv == "mt") {
      cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_SR34_1b_"+year, "lnN", ch::syst::SystMap<>::init(1.20));
    }
    else if ((category_name == "SR3_1b" || category_name == "SR4_1b") && channel_abbrv == "et") {
      cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_SR34_1b_"+year, "lnN", ch::syst::SystMap<>::init(1.30));
    }
    else if (category_name == "SR1_2b" || category_name == "SR2_2b") {
      cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_SR12_2b_"+year, "lnN", ch::syst::SystMap<>::init(1.30));
    }
    else if (category_name == "lowMassSR" && channel_abbrv == "mt") {
      cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_lowMassSR_"+year, "lnN", ch::syst::SystMap<>::init(1.20));
    }
    else if (category_name == "lowMassSR" && channel_abbrv == "et") {
      cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_lowMassSR_"+year, "lnN", ch::syst::SystMap<>::init(1.30));
    }
    else if (category_name == "mediumMassSR") {
      cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_mediumMassSR_"+year, "lnN", ch::syst::SystMap<>::init(1.20));
    }
    else if (category_name == "highMassSR") {
      cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_highMassSR_"+year, "lnN", ch::syst::SystMap<>::init(1.20));
    }
    else cb->cp().bin({category_name}).process({fakeProcName}).AddSyst(*cb, "CMS_NPS25003_normalization_fake_"+channel_abbrv+"_CR_"+year, "lnN", ch::syst::SystMap<>::init(1.20));
  }
}

void addothernorm(ch::CombineHarvester* cb, TFile* input_file, vector<pair<int,string>> categories) {
  for (auto categories_itn = categories.begin(); categories_itn != categories.end(); ++categories_itn) {
    string category_name = categories_itn->second;// ch::Categories is of the type vector<pair<int, string>>
    TDirectory* dir = (TDirectory*) input_file->Get(category_name.c_str());
    // sum all other process yields
    TH1F* hist_others = (TH1F*) dir->Get("others");
    double totyield = hist_others->Integral();
    if (totyield == 0) continue;
    // then sum specific processes that get the norm unc and calculate the ratio
    // multiply ratio * norm percentage (?) to get the value
    // e.g. if it is 20% uncertainty. 1+0.2*(rate_process/total_rate_processes)
    double temp = 0;
    double temp_1 = 0;
    double temp_2 = 0;
    TH1F* hist_ggh_htt = (TH1F*) dir->Get("ggh_htt");
    TH1F* hist_qqh_htt = (TH1F*) dir->Get("qqh_htt");
    TH1F* hist_Zh_htt = (TH1F*) dir->Get("Zh_htt");
    TH1F* hist_Wh_htt = (TH1F*) dir->Get("Wh_htt");
    TH1F* hist_ggh_hww = (TH1F*) dir->Get("ggh_hww");
    TH1F* hist_qqh_hww = (TH1F*) dir->Get("qqh_hww");
    TH1F* hist_Zh_hww = (TH1F*) dir->Get("Zh_hww");
    TH1F* hist_Wh_hww = (TH1F*) dir->Get("Wh_hww");
    TH1F* hist_tth = (TH1F*) dir->Get("tth");
    TH1F* hist_VV = (TH1F*) dir->Get("VV");
    TH1F* hist_ST = (TH1F*) dir->Get("ST");
    TH1F* hist_ZJ = (TH1F*) dir->Get("ZJ");

    temp = 1. + 0.018*(hist_ggh_htt->Integral()+hist_qqh_htt->Integral()+hist_Zh_htt->Integral()+hist_Wh_htt->Integral())/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "BR_htt", "lnN", ch::syst::SystMap<>::init(temp));

    temp = 1. + 0.015*(hist_ggh_hww->Integral()+hist_qqh_hww->Integral()+hist_Zh_hww->Integral()+hist_Wh_hww->Integral())/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "BR_hww", "lnN", ch::syst::SystMap<>::init(temp));

    temp = 1. + 0.032*(hist_ggh_htt->Integral()+hist_ggh_hww->Integral())/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "pdf_Higgs_gg", "lnN", ch::syst::SystMap<>::init(temp));

    temp = 1. + 0.021*(hist_qqh_htt->Integral()+hist_qqh_hww->Integral())/totyield + 0.019*(hist_Wh_htt->Integral()+hist_Wh_hww->Integral())/totyield + 0.013*(hist_Zh_htt->Integral()+hist_Zh_hww->Integral())/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "pdf_Higgs_qqbar", "lnN", ch::syst::SystMap<>::init(temp));

    temp = 1. + 0.036*hist_tth->Integral()/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "pdf_Higgs_ttH", "lnN", ch::syst::SystMap<>::init(temp));

    temp = 1. + 0.039*(hist_ggh_htt->Integral()+hist_ggh_hww->Integral())/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "QCDscale_gg", "lnN", ch::syst::SystMap<>::init(temp));

    temp_1 = 1 - 0.0033*(hist_qqh_htt->Integral()+hist_qqh_hww->Integral())/totyield + 0.007*(hist_Wh_htt->Integral()+hist_Wh_hww->Integral())/totyield + 0.031*(hist_Zh_htt->Integral()+hist_Zh_hww->Integral())/totyield;
    temp_2 = 1 + 0.0043*(hist_qqh_htt->Integral()+hist_qqh_hww->Integral())/totyield + 0.005*(hist_Wh_htt->Integral()+hist_Wh_hww->Integral())/totyield + 0.038*(hist_Zh_htt->Integral()+hist_Zh_hww->Integral())/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "QCDscale_qqbar", "lnN", ch::syst::SystMapAsymm<>::init(temp_1,temp_2));

    temp_1 = 1. - 0.092*hist_tth->Integral()/totyield;
    temp_2 = 1. + 0.058*hist_tth->Integral()/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "QCDscale_ttH", "lnN", ch::syst::SystMapAsymm<>::init(temp_1,temp_2));

    temp = 1. + 0.05*hist_VV->Integral()/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "cross_section_VV", "lnN", ch::syst::SystMap<>::init(temp));

    temp = 1. + 0.05*hist_ST->Integral()/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "cross_section_ST", "lnN", ch::syst::SystMap<>::init(temp));

    temp = 1. + 0.02*hist_ZJ->Integral()/totyield;
    cb->cp().bin({category_name}).process({"others"}).AddSyst(*cb, "CMS_NPS25003_normalization_Z", "lnN", ch::syst::SystMap<>::init(temp));
  }
}

int main(int argc, char** argv) {
    
  std::string channel = *(argv + 1);
  std::string signalType = *(argv + 2); // 4b2t (cascade) or 2b2t (non-cascade)
  std::string year = *(argv + 3);
  // these determine which processes actually go into the signal for each datacard
  std::string mass1 = *(argv + 4);
  std::string mass2 = *(argv + 5);
  std::string ncats = *(argv + 6);

  // Take category names from inputs
  cout << "Categories for channel and year and signal type : " << channel << " " << year << " " << signalType << ":" << endl;
  std::vector<std::pair<int, std::string>> cats = {};
  for (int i = 1; i <= stoi(ncats); i++) {
    cats.push_back({i, *(argv + 6 + i)});
    cout << "Categories are: " << cats[i-1].first << ": " << cats[i-1].second << endl;
  }
    
  // Where the datacard is stored
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/other_aux/shapes" + year + "/";
    
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  cb.SetVerbosity(3);
    
  // List of categories (also called 'bins' in Combine)
  // ch::Categories cats = {
  //    {1, "1"},
  // };
     
  // Observed data (name must be "data_obs" in datacards)
  cb.AddObservations({"*"}, {"hToA1A2"}, {year}, {channel}, cats);

  std::string fakeProcName;
  if ((channel == "mutau") || (channel == "etau")) {
    fakeProcName = "fake";
  }
  else if (channel == "emu") {
    fakeProcName = "qcd";
  }
    
  vector<string> bkg_procs = {"ttbar", "others"};
  vector<string> bkg_procs_noEMB_nofake = bkg_procs;

  bkg_procs.push_back("embedded");
  bkg_procs.push_back(fakeProcName);

  cb.AddProcesses({"*"}, {"hToA1A2"}, {year}, {channel}, bkg_procs, cats, false);
    
  // List of signals in the datacards

  vector<string> sig_ggh = {"ggh" + signalType + "-" + mass1 + "-" + mass2};
  vector<string> sig_vbf = {"vbf" + signalType + "-" + mass1 + "-" + mass2};

  vector<string> sig_procs = {"ggh" + signalType + "-" + mass1 + "-" + mass2, "vbf" + signalType + "-" + mass1 + "-" + mass2};

  cb.AddProcesses({"*"}, {"hToA1A2"}, {year}, {channel}, sig_procs, cats, true);

  using ch::syst::SystMap;
  using ch::syst::SystMapAsymm;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;
  using ch::JoinStr;

  std::string channel_abbrv;

  if (channel == "mutau") channel_abbrv = "mt";
  if (channel == "etau")  channel_abbrv = "et";
  if (channel == "emu")   channel_abbrv = "em";

  TFile* file;
  file = new TFile((aux_shapes+"out_"+channel+".root").c_str());// To be used for the addshapes function
    
  // =========================== Normalization uncertainties ===========================
  // The AddSyst method supports {$BIN, $PROCESS, $MASS, $ERA, $CHANNEL, $ANALYSIS}
  cb.cp().process({"ttbar"}).AddSyst(cb, "cross_section_ttbar", "lnN", SystMap<>::init(1.042));
  cb.cp().process({"embedded"}).AddSyst(cb, "CMS_NPS25003_normalization_embedded", "lnN", SystMap<>::init(1.04));
  cb.cp().process(JoinStr({sig_ggh,sig_vbf})).AddSyst(cb, "CMS_NPS25003_signal_theory", "lnN", SystMap<>::init(1.036));

  // Luminosity uncertainties, no embedded
  if (year=="2016preVFP" || year=="2016postVFP"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_2016", "lnN", SystMap<>::init(1.01));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated16-18", "lnN", SystMap<>::init(1.006));
  }
  if (year=="2017"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_2017", "lnN", SystMap<>::init(1.02));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated16-18", "lnN", SystMap<>::init(1.009));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated17-18", "lnN", SystMap<>::init(1.006));
  }
  if (year=="2018"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_2018", "lnN", SystMap<>::init(1.015));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated16-18", "lnN", SystMap<>::init(1.02));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated17-18", "lnN", SystMap<>::init(1.002));
  }

  if (channel=="emu"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "CMS_eleID_13TeV", "lnN", SystMap<>::init(1.02));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "CMS_muID_13TeV", "lnN", SystMap<>::init(1.02));
    // eleID 50% correlated with MC
    cb.cp().process({"embedded"}).AddSyst(cb, "CMS_eleID_13TeV", "lnN", SystMap<>::init(1.01));// 2% * 50%
    cb.cp().process({"embedded"}).AddSyst(cb, "CMS_EMB_eleID_13TeV", "lnN", SystMap<>::init(1.01732));// 2% * sqrt(1-50%^2)
    // muID 50% correlated with MC
    cb.cp().process({"embedded"}).AddSyst(cb, "CMS_muID_13TeV", "lnN", SystMap<>::init(1.01));
    cb.cp().process({"embedded"}).AddSyst(cb, "CMS_EMB_muID_13TeV", "lnN", SystMap<>::init(1.01732));
    cb.cp().process({fakeProcName}).AddSyst(cb, "CMS_NPS25003_normalization_qcd_"+channel_abbrv+"_"+year, "lnN", SystMap<>::init(1.20));
  }

    
  if (channel=="etau"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "CMS_eleID_13TeV", "lnN", SystMap<>::init(1.02));
    // eleID 50% correlated with MC
    cb.cp().process({"embedded"}).AddSyst(cb, "CMS_eleID_13TeV", "lnN", SystMap<>::init(1.01));
    cb.cp().process({"embedded"}).AddSyst(cb, "CMS_EMB_eleID_13TeV", "lnN", SystMap<>::init(1.01732));
  }
    
  if (channel=="mutau"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).AddSyst(cb, "CMS_muID_13TeV", "lnN", SystMap<>::init(1.02));
    // muID 50% correlated with MC
    cb.cp().process({"embedded"}).AddSyst(cb, "CMS_muID_13TeV", "lnN", SystMap<>::init(1.01));
    cb.cp().process({"embedded"}).AddSyst(cb, "CMS_EMB_muID_13TeV", "lnN", SystMap<>::init(1.01732));
  }

  // Add category dependent fake normalization uncertainty
  if (channel=="etau" or channel=="mutau") addfakenorm(&cb, cats, fakeProcName, channel_abbrv, year); 
    
  // // =========================== Shape uncertainties ===========================
  // // The AddSyst method supports {$BIN, $PROCESS, $MASS, $ERA, $CHANNEL, $ANALYSIS}
    
  // btagging efficiency, no embedded (correlated between eras: hf/lf/cferr1/cferr2, rename later)
  addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_btagsf_hf_"+year, 1.00);
  addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_btagsf_lf_"+year, 1.00);
  addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_btagsf_hfstats1_"+year, 1.00);
  addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_btagsf_hfstats2_"+year, 1.00);
  addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_btagsf_lfstats1_"+year, 1.00);
  addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_btagsf_lfstats2_"+year, 1.00);
  addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_btagsf_cferr1_"+year, 1.00);
  addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_btagsf_cferr2_"+year, 1.00);

  // Trigger efficiency
  if (channel=="etau" or channel=="mutau"){
    // TODO: make sure this works
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_trgeff_single_"+channel_abbrv+"_"+year, 1.00);
    // 50% correlated with MC
    addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_single_"+channel_abbrv+"_"+year, 0.50);// 1.00 * 50%
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_single_"+channel_abbrv+"_"+year, 0.866);// 1.00 * sqrt(1-50%^2)
    if (channel=="mutau" or (channel=="etau" && year!="2016preVFP" && year!="2016postVFP")){
      addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_trgeff_cross_"+channel_abbrv+"_"+year, 1.00);
      addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_cross_"+channel_abbrv+"_"+year, 0.50);
      addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_cross_"+channel_abbrv+"_"+year, 0.866);
    }
  }
  if (channel=="emu"){
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_trgeff_Mu8E23_"+channel_abbrv+"_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_trgeff_Mu23E12_"+channel_abbrv+"_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_trgeff_both_"+channel_abbrv+"_"+year, 1.00);
    // 50% correlated with MC
    addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_Mu8E23_"+channel_abbrv+"_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_Mu23E12_"+channel_abbrv+"_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_both_"+channel_abbrv+"_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_Mu8E23_"+channel_abbrv+"_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_Mu23E12_"+channel_abbrv+"_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_both_"+channel_abbrv+"_"+year, 0.866);
  }
    
  // tau related corrections, no fake bkg
  if (channel=="etau" or channel=="mutau"){
    // tau ID efficiency (VSjet)
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt20to25_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt25to30_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt30to35_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt35to40_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt40to500_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt500to1000_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_ptgt1000_"+year, 1.00);
        
    // 50% correlated with MC
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt20to25_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt25to30_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt30to35_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt35to40_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt40to500_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt500to1000_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_ptgt1000_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt20to25_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt25to30_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt30to35_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt35to40_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt40to500_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt500to1000_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_ptgt1000_"+year, 0.866);
 
    // tau ID efficiency (VSe), no anti-lepton in embedded
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSe_bar_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSe_end_"+year, 1.00);
 
    // tau ID efficiency (VSmu), no anti-lepton in embedded
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta0to0p4_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta0p4to0p8_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta0p8to1p2_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta1p2to1p7_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta1p7to2p3_"+year, 1.00);
        
    // tau ES
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauES_dm0_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauES_dm1_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauES_dm10_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauES_dm11_"+year, 1.00);

    // 50% correlated with MC
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauES_dm0_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauES_dm1_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauES_dm10_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_tauES_dm11_"+year, 0.50);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauES_dm0_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauES_dm1_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauES_dm10_"+year, 0.866);
    addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauES_dm11_"+year, 0.866);

    // tau ES (ele fake), no embedded
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_eleTES_dm0_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_eleTES_dm1_"+year, 1.00);
    
    // tau ES (mu fake), no embedded
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_muTES_dm0_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_muTES_dm1_"+year, 1.00);
    }
    
    // Tau ID efficiency with different WP than used in measurement, for e+tau only
    if (channel=="etau"){
        addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauidWP_et_"+year, 1.00);
        addshapes(&cb, file, cats, {"embedded"}, "CMS_tauidWP_et_"+year, 0.50);
    }
    
    // Leptons ES (MC and embedded are fully uncorrelated)
    if (channel=="etau" or channel=="emu"){
        // ele ES
        addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_eleES_bar_"+year, 1.00);
        addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_eleES_end_"+year, 1.00);
        addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_eleES_bar_"+year, 1.00);
        addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_eleES_end_"+year, 1.00);
    }
    if (channel=="mutau" or channel=="emu"){
        // mu ES
        addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_muES_eta0to1p2_"+year, 1.00);
        addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_muES_eta1p2to2p1_"+year, 1.00);
        addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_muES_eta2p1to2p4_"+year, 1.00);
        addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_muES_eta0to1p2_"+year, 1.00);
        addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_muES_eta1p2to2p1_"+year, 1.00);
        addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_muES_eta2p1to2p4_"+year, 1.00);
    }
    
    // JES, no embedded
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetAbsolute", 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetAbsolute_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetBBEC1", 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetBBEC1_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetEC2", 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetEC2_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetFlavorQCD", 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetHF", 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetHF_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetRelativeBal", 1.00);
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JetRelativeSample_"+year, 1.00);

    // JER
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_JER_"+year, 1.00);

    // L1 prefiring
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_prefiring_"+year, 1.00);

    // PU
    addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf}), "CMS_puweight_"+year, 1.00);

    // recoil correction, for Z+jets, W+jets, ggh and qqh (no W+jets in e+tau and mu+tau)
    // UES uncertainties, for MC without recoil correction
    // TODO: should be CMS_boson_met_recoil_reso_0j also TODO: 2j?
    addshapes(&cb, file, cats, JoinStr({{fakeProcName},sig_ggh,sig_vbf}), "CMS_met_0j_resolution_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({{fakeProcName},sig_ggh,sig_vbf}), "CMS_met_0j_response_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({{fakeProcName},sig_ggh,sig_vbf}), "CMS_met_1j_resolution_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({{fakeProcName},sig_ggh,sig_vbf}), "CMS_met_1j_response_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({{fakeProcName},sig_ggh,sig_vbf}), "CMS_met_gt1j_resolution_"+year, 1.00);
    addshapes(&cb, file, cats, JoinStr({{fakeProcName},sig_ggh,sig_vbf}), "CMS_met_gt1j_response_"+year, 1.00);
    addshapes(&cb, file, cats, {"ttbar", fakeProcName}, "CMS_UES_"+year, 1.00);
    
    // Z pt reweighting
    addshapes(&cb, file, cats, {"others"}, "CMS_Zpt_"+year, 1.00);
    
    // top pt reweighting (no need to add if data/MC agreement is fairly good with nominal top pt sf applied)
    addshapes(&cb, file, cats, {"ttbar", fakeProcName}, "CMS_toppt_"+year, 1.00);

    // ttbar scale uncertainties (correlated across years so we need to rename later)
    addshapes(&cb, file, cats, {"ttbar"}, "CMS_renscfact_"+year, 1.00);
    addshapes(&cb, file, cats, {"ttbar"}, "CMS_facscfact_"+year, 1.00);
 
    // tau tracking efficiency in embedded (on real tauh, no effect on fake bkg)
    if (channel=="etau" or channel=="mutau"){
      addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tautrack_dm0dm10_"+year, 1.00);
      addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tautrack_dm1_"+year, 1.00);
      addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tautrack_dm11_"+year, 1.00);
    }
    
    // Non-DY MC contamination to embedded (selected from taus, no effect on fake bkg)
    addshapes(&cb, file, cats, {"embedded"}, "CMS_nonDY_"+year, 1.00);
    
    // Reducible background estimation
    if (channel=="etau" or channel=="mutau"){
      // fake factor for cross triggered events
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_crosstrg_fakefactor_"+year, 1.00);
      // fake rate measurement
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt0to25_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt25to30_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt30to35_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt35to40_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt40to50_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt50to60_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt60to80_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt80to100_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt100to120_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_pt120to150_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_jetFR_ptgt150_"+year, 1.00);
    }
    if (channel=="emu"){
      // SS correction and closure
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_SScorrection_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_SSclosure_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_SSboth2D_"+year, 1.00);
      addshapes(&cb, file, cats, {fakeProcName}, "CMS_osss_"+year, 1.00);
    }
  
    // Name of the input datacard
    // The ExtractShapes method supports {$BIN, $PROCESS, $MASS, $SYSTEMATIC}
    cb.cp().backgrounds().ExtractShapes(
					aux_shapes + "out_"+channel+".root",
                                        "$BIN/$PROCESS",
                                        "$BIN/$PROCESS_$SYSTEMATIC");
    cb.cp().signals().ExtractShapes(
				    aux_shapes + "out_"+channel+".root",
                                    "$BIN/$PROCESS",
                                    "$BIN/$PROCESS_$SYSTEMATIC");

    if (year=="2016preVFP" || year=="2016postVFP"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "lumi_13TeV_2016", "lumi_2016");
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "lumi_13TeV_correlated16-18", "lumi_13TeV_correlated");
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauES_dm0_"+year, "CMS_scale_t_DM0_2016");
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauES_dm1_"+year, "CMS_scale_t_DM1_2016");
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauES_dm10_"+year, "CMS_scale_t_DM10_2016");
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauES_dm11_"+year, "CMS_scale_t_DM11_2016");
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauES_dm0_"+year, "CMS_NPS25003_EMB_scale_t_DM0_2016");
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauES_dm1_"+year, "CMS_NPS25003_EMB_scale_t_DM1_2016");
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauES_dm10_"+year, "CMS_NPS25003_EMB_scale_t_DM10_2016");
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauES_dm11_"+year, "CMS_NPS25003_EMB_scale_t_DM11_2016");
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_eleTES_dm0_"+year, "CMS_scale_t_eFake_DM0_2016");
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_eleTES_dm1_"+year, "CMS_scale_t_eFake_DM1_2016");
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_muTES_dm0_"+year, "CMS_scale_t_muFake_DM0_2016");
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_muTES_dm1_"+year, "CMS_scale_t_muFake_DM1_2016");
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_puweight_"+year, "CMS_pileup_2016");
    }

    if (year=="2017"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "lumi_13TeV_2017", "lumi_2017");
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "lumi_13TeV_correlated16-18", "lumi_13TeV_correlated");
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "lumi_13TeV_correlated17-18", "lumi_13TeV_1718");
    }

    if (year=="2018"){
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "lumi_13TeV_2018", "lumi_2018");
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "lumi_13TeV_correlated16-18", "lumi_13TeV_correlated");
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "lumi_13TeV_correlated17-18", "lumi_13TeV_1718");
    }

    if (year=="2017" or year=="2018"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauES_dm0_"+year, "CMS_scale_t_DM0_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauES_dm1_"+year, "CMS_scale_t_DM1_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauES_dm10_"+year, "CMS_scale_t_DM10_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauES_dm11_"+year, "CMS_scale_t_DM11_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauES_dm0_"+year, "CMS_NPS25003_EMB_scale_t_DM0_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauES_dm1_"+year, "CMS_NPS25003_EMB_scale_t_DM1_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauES_dm10_"+year, "CMS_NPS25003_EMB_scale_t_DM10_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauES_dm11_"+year, "CMS_NPS25003_EMB_scale_t_DM11_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_eleTES_dm0_"+year, "CMS_scale_t_eFake_DM0_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_eleTES_dm1_"+year, "CMS_scale_t_eFake_DM1_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_muTES_dm0_"+year, "CMS_scale_t_muFake_DM0_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_muTES_dm1_"+year, "CMS_scale_t_muFake_DM1_"+year);
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_puweight_"+year, "CMS_pileup_"+year);
    }

    if (channel=="emu"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf, {"embedded"}})).RenameSystematic(cb, "CMS_trgeff_Mu8E23_"+channel_abbrv+"_"+year, "CMS_NPS25003_trgeff_Mu8E23_"+channel_abbrv+"_"+year);
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf, {"embedded"}})).RenameSystematic(cb, "CMS_trgeff_Mu23E12_"+channel_abbrv+"_"+year, "CMS_NPS25003_trgeff_Mu23E12_"+channel_abbrv+"_"+year);
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf, {"embedded"}})).RenameSystematic(cb, "CMS_trgeff_both_"+channel_abbrv+"_"+year, "CMS_NPS25003_trgeff_both_"+channel_abbrv+"_"+year);
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_trgeff_Mu8E23_"+channel_abbrv+"_"+year, "CMS_NPS25003_EMB_trgeff_Mu8E23_"+channel_abbrv+"_"+year);
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_trgeff_Mu23E12_"+channel_abbrv+"_"+year, "CMS_NPS25003_EMB_trgeff_Mu23E12_"+channel_abbrv+"_"+year);
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_trgeff_both_"+channel_abbrv+"_"+year, "CMS_NPS25003_EMB_trgeff_both_"+channel_abbrv+"_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_eleID_13TeV", "CMS_eff_e_id");
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_muID_13TeV", "CMS_eff_m_id");
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_eleID_13TeV", "CMS_NPS25003_EMB_eff_e_id");
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_muID_13TeV", "CMS_NPS25003_EMB_eff_m_id");
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_SScorrection_"+year, "CMS_NPS25003_SScorrection_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_SSclosure_"+year, "CMS_NPS25003_SSclosure_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_SSboth2D_"+year, "CMS_NPS25003_SSboth2D_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_osss_"+year, "CMS_NPS25003_osss_"+year);
    }

    if (channel=="etau"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_eleID_13TeV", "CMS_eff_e_id");
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_tauidWP_et_"+year, "CMS_NPS25003_eff_t_WP_et_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_eleID_13TeV", "CMS_eff_e_id");
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_eleID_13TeV", "CMS_NPS25003_EMB_eff_e_id");
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_tauidWP_et_"+year, "CMS_NPS25003_EMB_eff_t_WP_et_"+year);
    }

    if (channel=="mutau"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_muID_13TeV", "CMS_eff_m_id");
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_muID_13TeV", "CMS_eff_m_id");
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_muID_13TeV", "CMS_NPS25003_EMB_eff_m_id");
    }

    if (channel=="etau" or channel=="mutau"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf, {"embedded"}})).RenameSystematic(cb, "CMS_trgeff_single_"+channel_abbrv+"_"+year, "CMS_NPS25003_trgeff_single_"+channel_abbrv+"_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_trgeff_single_"+channel_abbrv+"_"+year, "CMS_NPS25003_EMB_trgeff_single_"+channel_abbrv+"_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf, {"embedded"}})).RenameSystematic(cb, "CMS_trgeff_cross_"+channel_abbrv+"_"+year, "CMS_NPS25003_trgeff_cross_"+channel_abbrv+"_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_trgeff_cross_"+channel_abbrv+"_"+year, "CMS_NPS25003_EMB_trgeff_cross_"+channel_abbrv+"_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauideff_pt20to25_"+year, "CMS_NPS25003_eff_t_pt20to25_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauideff_pt25to30_"+year, "CMS_NPS25003_eff_t_pt25to30_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauideff_pt30to35_"+year, "CMS_NPS25003_eff_t_pt30to35_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauideff_pt35to40_"+year, "CMS_NPS25003_eff_t_pt35to40_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauideff_pt40to500_"+year, "CMS_NPS25003_eff_t_pt40to500_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauideff_pt500to1000_"+year, "CMS_NPS25003_eff_t_pt500to1000_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf,{"embedded"}})).RenameSystematic(cb, "CMS_tauideff_ptgt1000_"+year, "CMS_NPS25003_eff_t_ptgt1000_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauideff_pt20to25_"+year, "CMS_NPS25003_EMB_eff_t_pt20to25_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauideff_pt25to30_"+year, "CMS_NPS25003_EMB_eff_t_pt25to30_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauideff_pt30to35_"+year, "CMS_NPS25003_EMB_eff_t_pt30to35_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauideff_pt35to40_"+year, "CMS_NPS25003_EMB_eff_t_pt35to40_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauideff_pt40to500_"+year, "CMS_NPS25003_EMB_eff_t_pt40to500_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauideff_pt500to1000_"+year, "CMS_NPS25003_EMB_eff_t_pt500to1000_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tauideff_ptgt1000_"+year, "CMS_NPS25003_EMB_eff_t_ptgt1000_"+year);
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tautrack_dm0dm10_"+year, "CMS_NPS25003_EMB_tautrack_dm0dm10_"+year);
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tautrack_dm1_"+year, "CMS_NPS25003_EMB_tautrack_dm1_"+year);
	cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_tautrack_dm11_"+year, "CMS_NPS25003_EMB_tautrack_dm11_"+year);
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_tauideff_VSe_bar_"+year, "CMS_NPS25003_eff_t_VSe_barrel_"+year);
	cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_tauideff_VSe_end_"+year, "CMS_NPS25003_eff_t_VSe_endcap_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_tauideff_VSmu_eta0to0p4_"+year, "CMS_NPS25003_eff_t_VSmu_eta0to0p4_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_tauideff_VSmu_eta0p4to0p8_"+year, "CMS_NPS25003_eff_t_VSmu_eta0p4to0p8_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_tauideff_VSmu_eta0p8to1p2_"+year, "CMS_NPS25003_eff_t_VSmu_eta0p8to1p2_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_tauideff_VSmu_eta1p2to1p7_"+year, "CMS_NPS25003_eff_t_VSmu_eta1p2to1p7_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_tauideff_VSmu_eta1p7to2p3_"+year, "CMS_NPS25003_eff_t_VSmu_eta1p7to2p3_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt0to25_"+year, "CMS_NPS25003_fake_t_pt0to25_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt25to30_"+year, "CMS_NPS25003_fake_t_pt25to30_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt30to35_"+year, "CMS_NPS25003_fake_t_pt30to35_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt35to40_"+year, "CMS_NPS25003_fake_t_pt35to40_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt40to50_"+year, "CMS_NPS25003_fake_t_pt40to50_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt50to60_"+year, "CMS_NPS25003_fake_t_pt50to60_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt60to80_"+year, "CMS_NPS25003_fake_t_pt60to80_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt80to100_"+year, "CMS_NPS25003_fake_t_pt80to100_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt100to120_"+year, "CMS_NPS25003_fake_t_pt100to120_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_pt120to150_"+year, "CMS_NPS25003_fake_t_pt120to150_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_jetFR_ptgt150_"+year, "CMS_NPS25003_fake_t_ptgt150_"+year);
	cb.cp().process({fakeProcName}).RenameSystematic(cb, "CMS_crosstrg_fakefactor_"+year, "CMS_NPS25003_crosstrg_fakefactor_"+year);
    }

    if (channel=="etau" or channel=="emu"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_eleES_bar_"+year, "CMS_NPS25003_scale_e_barrel_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_eleES_end_"+year, "CMS_NPS25003_scale_e_endcap_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_eleES_bar_"+year, "CMS_NPS25003_EMB_scale_e_barrel_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_eleES_end_"+year, "CMS_NPS25003_EMB_scale_e_endcap_"+year);
    }

    if (channel=="mutau" or channel=="emu"){
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_muES_eta0to1p2_"+year, "CMS_NPS25003_scale_m_eta0to1p2_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_muES_eta1p2to2p1_"+year, "CMS_NPS25003_scale_m_eta1p2to2p1_"+year);
        cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_muES_eta2p1to2p4_"+year, "CMS_NPS25003_scale_m_eta2p1to2p4_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_muES_eta0to1p2_"+year, "CMS_NPS25003_EMB_scale_m_eta0to1p2_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_muES_eta1p2to2p1_"+year, "CMS_NPS25003_EMB_scale_m_eta1p2to2p1_"+year);
        cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_EMB_muES_eta2p1to2p4_"+year, "CMS_NPS25003_EMB_scale_m_eta2p1to2p4_"+year);
    }

    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_btagsf_hf_"+year, "CMS_btag_shape_hf");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_btagsf_lf_"+year, "CMS_btag_shape_lf");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_btagsf_hfstats1_"+year, "CMS_btag_shape_hfstats1_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_btagsf_hfstats2_"+year, "CMS_btag_shape_hfstats2_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_btagsf_lfstats1_"+year, "CMS_btag_shape_lfstats1_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_btagsf_lfstats2_"+year, "CMS_btag_shape_lfstats2_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_btagsf_cferr1_"+year, "CMS_btag_shape_cferr1");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_btagsf_cferr2_"+year, "CMS_btag_shape_cferr2");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetAbsolute", "CMS_scale_j_Absolute");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetAbsolute_"+year, "CMS_scale_j_Absolute_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetBBEC1", "CMS_scale_j_BBEC1");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetBBEC1_"+year, "CMS_scale_j_BBEC1_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetEC2", "CMS_scale_j_EC2");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetEC2_"+year, "CMS_scale_j_EC2_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetFlavorQCD", "CMS_scale_j_FlavorQCD");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetHF", "CMS_scale_j_HF");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetHF_"+year, "CMS_scale_j_HF_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetRelativeBal", "CMS_scale_j_RelativeBal");
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JetRelativeSample_"+year, "CMS_scale_j_RelativeSample_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_JER_"+year, "CMS_res_j_"+year);
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_prefiring_"+year, "CMS_NPS25003_l1_prefiring_"+year);
    cb.cp().process(JoinStr({{"others",fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_met_0j_resolution_"+year, "CMS_NPS25003_met_0j_resolution_"+year);
    cb.cp().process(JoinStr({{"others",fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_met_0j_response_"+year, "CMS_NPS25003_met_0j_response_"+year);
    cb.cp().process(JoinStr({{"others",fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_met_1j_resolution_"+year, "CMS_NPS25003_met_1j_resolution_"+year);
    cb.cp().process(JoinStr({{"others",fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_met_1j_response_"+year, "CMS_NPS25003_met_1j_response_"+year);
    cb.cp().process(JoinStr({{"others",fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_met_gt1j_resolution_"+year, "CMS_NPS25003_met_gt1j_resolution_"+year);
    cb.cp().process(JoinStr({{"others",fakeProcName},sig_ggh,sig_vbf})).RenameSystematic(cb, "CMS_met_gt1j_response_"+year, "CMS_NPS25003_met_gt1j_response_"+year);
    cb.cp().process({"ttbar","others",fakeProcName}).RenameSystematic(cb, "CMS_UES_"+year, "CMS_NPS25003_UES_"+year);
    cb.cp().process({"others", fakeProcName}).RenameSystematic(cb, "CMS_Zpt_"+year, "CMS_NPS25003_Z_pt_reweighting_"+year);
    cb.cp().process({"ttbar", fakeProcName}).RenameSystematic(cb, "CMS_toppt_"+year, "CMS_NPS25003_top_pt_reweighting_"+year);
    cb.cp().process({"ttbar"}).RenameSystematic(cb, "CMS_renscfact_"+year, "QCDscale_ren_ttbar");
    cb.cp().process({"ttbar"}).RenameSystematic(cb, "CMS_facscfact_"+year, "QCDscale_fac_ttbar");
    cb.cp().process({"embedded"}).RenameSystematic(cb, "CMS_nonDY_"+year, "CMS_NPS25003_EMB_nonDY_"+year);

    // Add other normalization uncertainties
    addothernorm(&cb, file, cats);

    ch::SetStandardBinNames(cb);
    
    set<string> bins = cb.bin_set();
    
    TFile output(("hAsymm"+year+"_"+signalType+"_"+channel+"_"+mass1+"_"+mass2+".input.root").c_str(), "RECREATE");
    
    for (auto b : bins) {
      cout << ">> Writing datacard for bin: " << b << " and " << signalType <<  " mass point " << mass1 << ", " << mass2 << "\n";
      cb.cp().bin({b}).mass({mass1, "*"}).WriteDatacard(b + "_" + signalType + "_" + mass1 + "_" + mass2 + ".txt", output);
    }
}
