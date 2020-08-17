import ROOT
import array
from optparse import OptionParser
import sys

def main(options):
    fdata = ROOT.TFile(options.input)
    fmc = ROOT.TFile(options.weight)
    hmc = fmc.Get("ggh_125_13TeV_AllR9Eta_R9VsEtaLead")#name of the 2D matrix in the reweighting root file
    #hmc = fmc.Get("ggh_90_13TeV_AllR9Eta_R9VsEtaLead")#for low-mass analysis
    #hmc = fmc.Get("r9_eta_weights_histo")
    print hmc.GetName()
    hmc.Scale((float(hmc.GetNbinsX()))*(float(hmc.GetNbinsY()))/hmc.Integral())
    
    fdata.cd()
    directories = fdata.GetListOfKeys()

    outFile = options.input.split(".root")[0]+"_r9eta.root"    
    newFile = ROOT.TFile(outFile, "RECREATE")

    for d in directories:
        print "Reducing tree in directory: ", d.GetName()
        directory = fdata.Get(d.GetName())

        tree = directory.Get("fitter_tree")
        entries = tree.GetEntries()  

        #--- Write to new file
        #tree.SetBranchStatus("*", 1)
        #tree.SetBranchStatus("newWeight", 0) 
        #tree.SetBranchStatus("PUweight", 0) 

        directory_new = newFile.mkdir(d.GetName())
        directory_new.cd()
        tree_new = tree.CloneTree(0)

        #tree.SetBranchStatus("newWeight", 1)
        #tree.SetBranchStatus("PUweight", 1)

        b_newWeight = array.array('f', [0])
        b_PUweight = array.array('f', [0])
        tree_new.Branch("r9etaWeight", b_newWeight, "r9etaWeight/F")
        tree_new.Branch("PUweight", b_PUweight, "PUweight/F")
        
        for z in range(entries):
            tree.GetEntry(z)
            b_newWeight[0] = hmc.GetBinContent(hmc.FindBin(tree.ph_full5x5x_r9, tree.ph_sc_abseta))
            tree_new.Fill()
        tree_new.GetCurrentFile().Write()

    tree_new.GetCurrentFile().Close() 


if __name__ == "__main__":  
    parser = OptionParser()

#specify here the name of the r9-eta reweighting file
    parser.add_option("-w", "--weight",   default="r9_eta_weights2017UL.root",   help="weights input filename")
#specify here the file to which we want to add the weight branch
    parser.add_option("-i", "--input", default="~/work/Hgg/test/CMSSW_10_6_8/src/flashgg/Validation/test/STD_UNS_17UL_200807/htot.root", help="Data input filename")
    
    (options, arg) = parser.parse_args()
    
    main(options)

