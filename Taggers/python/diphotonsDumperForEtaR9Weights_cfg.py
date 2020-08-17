#!/usr/bin/env cmsRun

import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
import os

process = cms.Process("Analysis")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(
        #"root://xrootd-cms.infn.it//store/group/phys_higgs/cmshgg/alesauva/flashgg/UL2017/10_6_4/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/UL2017-10_6_4-v2-RunIISummer19UL17MiniAOD-106X_mc2017_realistic_v6-v2/200501_133820/0000/myMicroAODOutputFile_956.root"
       "root://cms-xrd-global.cern.ch//store/group/phys_higgs/cmshgg/alesauva/flashgg/UL2017/10_6_4/GluGluHToGG_M125_TuneCP5_13TeV-amcatnloFXFX-pythia8/UL2017-10_6_4-v0-RunIISummer19UL17MiniAOD-106X_mc2017_realistic_v6-v2/200514_143038/0000/myMicroAODOutputFile_40.root"
        )
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32( 1000 )

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff') ## please double check

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("test.root")
)

process.load("flashgg.Taggers.flashggUpdatedIdMVADiPhotons_cfi") 
from flashgg.Taggers.flashggPreselectedDiPhotons_cfi import flashggPreselectedDiPhotons
process.kinPreselDiPhotons = flashggPreselectedDiPhotons.clone()

process.load("flashgg.Taggers.diphotonDumper_cfi") ##  import diphotonDumper 
import flashgg.Taggers.dumperConfigTools as cfgTools

process.diphotonDumper.src = "kinPreselDiPhotons"

process.diphotonDumper.dumpTrees = True
process.diphotonDumper.dumpWorkspace = False
process.diphotonDumper.quietRooFit = True


# split tree, histogram and datasets by process
process.diphotonDumper.nameTemplate ="$PROCESS_$SQRTS_$LABEL_$SUBCAT"
## do not split by process
## process.diphotonDumper.nameTemplate = "minitree_$SQRTS_$LABEL_$SUBCAT"

## define categories and associated objects to dump
cfgTools.addCategory(process.diphotonDumper,
                     "Reject",
                     "abs(leadingPhoton.superCluster.eta)>=1.4442&&abs(leadingPhoton.superCluster.eta)<=1.566||abs(leadingPhoton.superCluster.eta)>=2.5"
                     "||abs(subLeadingPhoton.superCluster.eta)>=1.4442 && abs(subLeadingPhoton.superCluster.eta)<=1.566||abs(subLeadingPhoton.superCluster.eta)>=2.5",
                      -1 ## if nSubcat is -1 do not store anythings
                     )

# interestng categories 
cfgTools.addCategories(process.diphotonDumper,
                       ## categories definition
                       ## cuts are applied in cascade. Events getting to these categories have already failed the "Reject" selection
                       [("AllR9Eta_","abs(leadingPhoton.superCluster.eta)<2.5 && abs(subLeadingPhoton.superCluster.eta)<2.5"
                         "&& leadingPhoton.full5x5_r9 <1.1 && subLeadingPhoton.full5x5_r9<1.1",0)
                       ],
                       ## variables to be dumped in trees/datasets. Same variables for all categories
                       ## if different variables wanted for different categories, can add categorie one by one with cfgTools.addCategory
                       
                       variables=["CMS_hgg_mass[320,60,120]:=mass", 
                                  "leadPt                   :=leadingPhoton.pt",
                                  "subleadPt                :=subLeadingPhoton.pt",
                                  "R9_lead                  :=leadingPhoton.full5x5_r9",
                                  "R9_sublead               :=subLeadingPhoton.full5x5_r9",
                                  "Eta_lead                 :=abs(leadingPhoton.superCluster.eta)",
                                  "Eta_sublead              :=abs(subLeadingPhoton.superCluster.eta)",
                                  "leadIDMVA                :=leadingView.phoIdMvaWrtChosenVtx",
                                  "subleadIDMVA             :=subLeadingView.phoIdMvaWrtChosenVtx",
                                  ],
                       ## histograms to be plotted. 
                       ## the variables need to be defined first
                       histograms=["CMS_hgg_mass>>mass(320,60.,140.)",
                                   "subleadPt:leadPt>>ptSubVsLead(180,20,200:180,20,200)", 
                                   "subleadPt>>subleadPt(80,0,200)",
                                   "leadPt>>leadPt(80,0,200)",
                                   "R9_lead>>R9_lead(110,0,1.1)",
                                   "R9_sublead>>R9_sublead(110,0,1.1)",
                                   "Eta_lead>>Eta_lead[0.,0.1,0.2,0.3,0.4,0.6,0.8,1.0,1.2,1.4442,1.566,1.7,1.8,2.,2.2,2.3,2.5]",
                                   "Eta_sublead>>Eta_sublead[0.,0.1,0.2,0.3,0.4,0.6,0.8,1.0,1.2,1.4442,1.566,1.7,1.8,2.,2.2,2.3,2.5]",
                                   "R9_lead:Eta_lead>>R9VsEtaLead[0.,0.5,0.55,0.6,0.65,0.7,0.74,0.76,0.78,0.8,0.82,0.84,0.86,0.88,0.9,0.91,0.92,0.93,0.94,0.95,0.96,0.97,0.98,0.99,1.05,2.:0.,0.8,1.,1.2,1.4442,1.566,2,2.5,3.]",
                                   "R9_sublead:Eta_sublead>>R9VsEtaSubLead[0.,0.5,0.55,0.6,0.65,0.7,0.74,0.76,0.78,0.8,0.82,0.84,0.86,0.88,0.9,0.91,0.92,0.93,0.94,0.95,0.96,0.97,0.98,0.99,1.05,2.:0.,0.8,1.,1.2,1.4442,1.566,2,2.5,3.]"
                                                                      
                                   ]
                       )


process.p1 = cms.Path(
    process.flashggUpdatedIdMVADiPhotons*process.kinPreselDiPhotons*process.diphotonDumper
    )



from flashgg.MetaData.JobConfig import customize
customize.setDefault("maxEvents",10000)
customize(process)
