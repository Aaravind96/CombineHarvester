import ROOT

def get_object_names(directory):
    keys = directory.GetListOfKeys()
    return set(key.GetName() for key in keys)

# Open ROOT files
#file1 = ROOT.TFile.Open("/eos/cms/store/group/phys_susy/AN-24-166/aaravind/for_datacards/2025-06-12-15h13m_2018-em-BDT-scan/out_emu.root")
file1 = ROOT.TFile.Open("/eos/cms/store/group/phys_susy/AN-24-166/pdas/for_datacards/2025-06-07-18h42m_2018-em-BDT/out_emu.root") 
file2 = ROOT.TFile.Open("/eos/cms/store/group/phys_susy/AN-24-166/aaravind/for_datacards/BDT_scan/iter_thr1_1/iter_thr2_2/out_emu.root")

# Get directories
d1 = file1.Get("BDTSR1_1bjet")
d2 = file2.Get("SR3_1b")

# Get object names
d1_objects = get_object_names(d1)
d2_objects = get_object_names(d2)

# Compare
missing = d1_objects - d2_objects

if not missing:
    print("All objects in D1 are present in D2.")
else:
    print("The following objects from D1 are missing in D2:")
    for obj in missing:
        print(f"  - {obj}")

