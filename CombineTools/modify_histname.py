import ROOT
import os

# Open the input ROOT file in read mode
input_file = ROOT.TFile.Open("/eos/cms/store/group/phys_susy/AN-24-166/pdas/for_datacards/2025-02-17-21h57m-year-2016postVFP-emu-iteration1-m_tt/out_emu.root", "READ")
if not input_file or input_file.IsZombie():
    print("Error: Could not open input file.")
    exit(1)

# Create a new output ROOT file
auxiliaries  = os.environ['CMSSW_BASE'] + '/src/auxiliaries/'
output_file = ROOT.TFile.Open(auxiliaries+"shapes2016postVFP/out_emu.root", "RECREATE")

# Function to recursively copy directories and rename histograms
def copy_and_rename_dir(input_dir, output_dir):
    # Loop through all keys in the directory
    for key in input_dir.GetListOfKeys():
        obj = key.ReadObj()
        
        # Check if the object is a directory
        if isinstance(obj, ROOT.TDirectory):
            # Create a new directory in the output file
            new_dir = output_dir.mkdir(obj.GetName())
            new_dir.cd()
            copy_and_rename_dir(obj, new_dir)  # Recursively copy contents

        # Check if the object is a histogram (TH1 or derived class)
        elif isinstance(obj, ROOT.TH1):
            # Rename the histogram
            new_name = obj.GetName().replace("2016", "2016postVFP")
            obj.SetName(new_name)

            # Write the modified histogram to the output directory
            output_dir.cd()
            obj.Write()

        else:
            # If it's an unknown type, just copy it
            output_dir.cd()
            obj.Write()

# Start copying and renaming from the root directory
output_file.cd()
copy_and_rename_dir(input_file, output_file)

# Close the files
input_file.Close()
output_file.Close()

print("Histograms successfully renamed and saved.")

