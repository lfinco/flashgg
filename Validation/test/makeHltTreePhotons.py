import FWCore.ParameterSet.Config as cms
#from FWCore.ParameterSet.VarParsing import VarParsing
import FWCore.Utilities.FileUtils as FileUtils
import sys

process = cms.Process("tnp")


from flashgg.Taggers.flashggPreselectedDiPhotons_cfi import flashggPreselectedDiPhotons
###################################################################
options = dict()
#varOptions = VarParsing('analysis')
#varOptions.parseArguments()

isLEAD=False #True = SEEDED LEG, False = UNSEEDED LEG

options['isMC']                   = True #normally HLT efficiencies are computed on data
options['era']                   = "2017" #needed for PU reweighting
options['PHOTON_COLL']           = "slimmedPhotons"
options['DIPHOTON_COLL']         = "flashggPreselectedDiPhotons"
#options['DIPHOTON_COLL']         = "flashggDiPhotons"
#options['PHOTON_CUTS']           = ""
#options['PHOTON_PROBE_CUTS']           = ""
#options['PHOTON_TAG_CUTS']       = ""

if(options['isMC']):
    options['HLTProcessName']        = "HLT" 
    options['TRIGGER_RES']            = "TriggerResults::HLT"
    options['PAT_TRIG']               = "patTriggerUnpacker"
else:
    options['HLTProcessName']        = "HLT" 
    options['TRIGGER_RES']            = "TriggerResults::HLT"
    #options['PAT_TRIG']               = "selectedPatTrigger"
    options['PAT_TRIG']               = "patTriggerUnpacker"

options['PHOTON_CUTS']           = "(abs(eta)<2.5) && (pt>15) && !(1.4442<=abs(superCluster.eta)<=1.566)"

if(isLEAD):
    options['PHOTON_PROBE_CUTS']           = "(abs(eta)<2.5) && (pt>30.0)"
else:
    options['PHOTON_PROBE_CUTS']           = "(abs(eta)<2.5) && (pt>22.0)"

options['PHOTON_TAG_CUTS']       = "(abs(eta)<=2.1) && !(1.4442<=abs(eta)<=1.566) && (pt>32.0)"

options['MAXEVENTS']             = cms.untracked.int32(-1) 
options['useAOD']                = cms.bool(False)
options['OUTPUTEDMFILENAME']     = 'edmFile.root'
options['DEBUG']                 = cms.bool(False)
options['LEADING_PRESELECTION']  = "" 

options['SUBLEADING_PRESELECTION'] = ""

from flashgg.Validation.treeMakerOptionsHLT_cfi import *

if (options['isMC']): 
    options['OUTPUT_FILE_NAME']      = "TnPTree_mc.root"
    options['GLOBALTAG']             = '92X_dataRun2_Prompt_v4'#to be changed accordingly to the year
    mylist =  FileUtils.loadListFromFile('/afs/cern.ch/user/l/lfinco/work/Hgg/test/CMSSW_10_6_8/src/flashgg/Validation/test/tempFileList_MC.txt') #needed when you run locally
    
else: 
    options['OUTPUT_FILE_NAME']      = "TnPTree_data.root"
    options['GLOBALTAG']             = '92X_dataRun2_Prompt_v4' #to be changed accordingly to the year
    mylist =  FileUtils.loadListFromFile('/afs/cern.ch/user/l/lfinco/work/Hgg/test/CMSSW_10_6_8/src/flashgg/Validation/test/tempFileList.txt') #needed when you run locally

readFiles      = cms.untracked.vstring(*mylist)
options['INPUT_FILE_NAME']       = readFiles

###for 2016
#options['TnPPATHS']              = cms.vstring("HLT_Ele27_WPTight_Gsf_v*")
#options['TnPHLTTagFilters']      = cms.vstring("hltEle27WPTightGsfTrackIsoFilter")


###for 2017
options['TnPPATHS']              = cms.vstring("HLT_Ele32_WPTight_Gsf_L1DoubleEG_v*")
options['TnPHLTTagFilters']      = cms.vstring("hltEle32L1DoubleEGWPTightGsfTrackIsoFilter", "hltEGL1SingleEGOrFilter")

###for 2018
#options['TnPPATHS']              = cms.vstring("HLT_Ele32_WPTight_Gsf_v*")
#options['TnPHLTTagFilters']      = cms.vstring("hltEle32WPTightGsfTrackIsoFilter")

options['TnPHLTProbeFilters']    = cms.vstring()

if(isLEAD):
    options['TagLeadMatchFilters']    = cms.vstring()  #lead eff only                                        
    options['HLTFILTERTOMEASURE']    = cms.vstring("hltEG30LIso60CaloId15b35eHE12R9Id50b80eEcalIsoLastFilter", "hltEG30LR9Id85b90eHE12R9Id50b80eR9IdLastFilter")  #ECal iso filter OR high R9 filter                                                                       
else:
    options['TagLeadMatchFilters']    = cms.vstring("hltEG30LIso60CaloId15b35eHE12R9Id50b80eEcalIsoLastFilter", "hltEG30LR9Id85b90eHE12R9Id50b80eR9IdLastFilter")  #sublead eff only     
    options['HLTFILTERTOMEASURE']    = cms.vstring("hltEG22R9Id85b90eHE12R9Id50b80eR9UnseededLastFilter", "hltEG22Iso60CaloId15b35eHE12R9Id50b80eTrackIsoUnseededLastFilter")  #High R9 filter OR track iso filter  

options['EVENTSToPROCESS']       = cms.untracked.VEventRange()


###################################################################

setModules(process, options)
from EgammaAnalysis.TnPTreeProducer.egmTreesContent_cff import *

process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = options['GLOBALTAG']

process.load('FWCore.MessageService.MessageLogger_cfi')
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

process.MessageLogger.cerr.threshold = ''
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options['INPUT_FILE_NAME']),
                            #fileNames = readFiles,
                            eventsToProcess = options['EVENTSToPROCESS'],
                            bypassVersionCheck = cms.untracked.bool(True)
                            )

process.maxEvents = cms.untracked.PSet( input = options['MAXEVENTS'])

###################################################################
## ID
###################################################################

from EgammaAnalysis.TnPTreeProducer.egmPhotonIDModules_cff import *
setIDs(process, options)

###################################################################
##rerunning PAT trigger
##################################################################

if(options['isMC']):
    process.load('PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff')
    process.patTrigger.processName = cms.string('HLT') #FLASHggMicroAOD                                                                                                                      
    process.patTrigger.onlyStandAlone = cms.bool(False)
    process.patTrigger.packTriggerPrescales = cms.bool(False)
    process.patTrigger.triggerResults = cms.InputTag( "TriggerResults::HLT" )
    process.patTrigger.triggerEvent   = cms.InputTag( "hltTriggerSummaryAOD::HLT" )

###################################################################
## SEQUENCES
###################################################################

process.egmPhotonIDs.physicsObjectSrc = cms.InputTag("photonFromDiPhotons")

process.patTriggerUnpacker = cms.EDProducer("PATTriggerObjectStandAloneUnpacker",
                                            patTriggerObjectsStandAlone = cms.InputTag("slimmedPatTrigger"),
                                            triggerResults = cms.InputTag('TriggerResults'      , '', 'HLT'),
                                            unpackFilterLabels = cms.bool(True)
                                                )
if(options['isMC']):
    process.pho_sequence = cms.Sequence( 
        process.patTriggerUnpacker +
        process.photonFromDiPhotons +
        process.goodPhotonTags +
        process.goodPhotonProbes +
        process.goodPhotonProbesIDMVA +
        process.goodPhotonsProbeTnPprobe +
        process.goodPhotonProbesPreselection +
        process.goodPhotonProbesL1 +
        process.goodPhotonsProbeHLT +
        process.goodPhotonTagsIDMVA +
        process.goodPhotonTagsPreselection +
        process.goodPhotonsTagLeadMatch +
        process.goodPhotonsTagHLT 
        )
else:
    process.pho_sequence = cms.Sequence(
        process.patTriggerUnpacker +
        process.photonFromDiPhotons +
        process.goodPhotonTags +
        process.goodPhotonProbes +
        process.goodPhotonProbesIDMVA +
        process.goodPhotonsProbeTnPprobe +
        process.goodPhotonProbesPreselection +
        process.goodPhotonProbesL1 +
        process.goodPhotonsProbeHLT +
        process.goodPhotonTagsIDMVA +
        process.goodPhotonTagsPreselection +
        process.goodPhotonsTagLeadMatch +
        process.goodPhotonsTagHLT 
        )    
###################################################################
## TnP PAIRS
###################################################################

process.allTagsAndProbes = cms.Sequence()
process.allTagsAndProbes *= process.noL1RECO
process.allTagsAndProbes *= process.L1RECO

process.mc_sequence = cms.Sequence()

#if (options['isMC']):
#    process.mc_sequence *= (process.McMatchTag + process.McMatchRECO)

##########################################################################
## TREE MAKER OPTIONS
##########################################################################
#if ( options['isMC']):
#    mcTruthCommonStuff = cms.PSet(
#        isMC = cms.bool(True),
#        probeMatches  = cms.InputTag("McMatchRECO"),
#        tagMatches  = cms.InputTag("McMatchRECO"),
#        eventWeight   = cms.InputTag("generator"),
#        motherPdgId   = cms.int32(23),
#        makeMCUnbiasTree = cms.bool(isMC),
#        checkMotherInUnbiasEff = cms.bool(False),
#        )
#else:    
mcTruthCommonStuff = cms.PSet(
    isMC = cms.bool(False)
    )

process.PhotonToRECO = cms.EDAnalyzer("TagProbeFitTreeProducer",
                                      mcTruthCommonStuff, CommonStuffForPhotonProbe,
                                      tagProbePairs = cms.InputTag("noL1RECO"),
                                      #tagProbePairs = cms.InputTag("tagL1RECO"),
                                      arbitration   = cms.string("None"),
                                      flags         = cms.PSet(passingL1  = cms.InputTag("goodPhotonProbesL1"),
                                                               passingHLT     = cms.InputTag("goodPhotonsProbeHLT"),
                                                               ),                                               
                                      allProbes     = cms.InputTag("goodPhotonProbesPreselection"),
                                      genParticles = cms.InputTag("flashggPrunedGenParticles"), #flashggGenPhotons
                                      useTauDecays = cms.bool(False),
                                      pdgId = cms.int32(22),
                                      checkCharge = cms.bool(False),
)
#process.PhotonToRECO.pfMet = cms.InputTag("slimmedMETs")
process.PhotonToRECO.pfMet = cms.InputTag("patCaloMet")

process.PhotonToRECOL1 = cms.EDAnalyzer("TagProbeFitTreeProducer",
                                        mcTruthCommonStuff, CommonStuffForPhotonProbe,
                                        tagProbePairs = cms.InputTag("L1RECO"),
                                        arbitration   = cms.string("None"),
                                        #arbitration   = cms.string("OneProbe"),
                                        flags         = cms.PSet(passingHLTl1     = cms.InputTag("goodPhotonsProbeHLT"),
                                                                 ),                                               
                                        allProbes     = cms.InputTag("goodPhotonProbesL1"),
                                        genParticles = cms.InputTag("flashggPrunedGenParticles"), #flashggGenPhotons
                                        useTauDecays = cms.bool(False),
                                        pdgId = cms.int32(22),
                                        checkCharge = cms.bool(False),
                                        )
process.PhotonToRECOL1.pfMet = cms.InputTag("patCaloMet")
#if(options['isMC']):
#    process.PhotonToRECO.PUWeightSrc   = cms.InputTag("pileupReweightingProducer","pileupWeights")
#    process.PhotonToRECOL1.PUWeightSrc   = cms.InputTag("pileupReweightingProducer","pileupWeights")
#    process.PhotonToRECO.tagVariables.probe_dRTau    = cms.InputTag("GsfDRToNearestTauProbe")
#    process.PhotonToRECOL1.tagVariables.probe_dRTau    = cms.InputTag("GsfDRToNearestTauProbe")
#    process.PhotonToRECO.variables.Pho_dRTau  = cms.InputTag("GsfDRToNearestTauProbe")
#    process.PhotonToRECOL1.variables.Pho_dRTau  = cms.InputTag("GsfDRToNearestTauProbe")
    
process.tree_sequence = cms.Sequence(process.PhotonToRECO)

process.tree_sequence2 = cms.Sequence(process.PhotonToRECOL1)

##########################################################################
## PATHS
##########################################################################

process.out = cms.OutputModule("PoolOutputModule", 
                               fileName = cms.untracked.string(options['OUTPUTEDMFILENAME']),
                               SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring("p"))
                               )
process.outpath = cms.EndPath(process.out)
if (not options['DEBUG']):
    process.outpath.remove(process.out)

##########################################################################################
###### MICROAOD STUFF
##########################################################################################
process.load("flashgg/Taggers/flashggUpdatedIdMVADiPhotons_cfi")
process.load("flashgg/Taggers/flashggDiPhotonMVA_cfi")
process.flashggDiPhotonMVA.DiPhotonTag = cms.InputTag('flashggDiPhotons')
process.load("flashgg/Taggers/flashggPreselectedDiPhotons_cfi")




if (options['isMC']):
    process.p = cms.Path(
        process.patTrigger +
        process.flashggUpdatedIdMVADiPhotons +
        process.flashggDiPhotonMVA +
        process.flashggPreselectedDiPhotons +
        #process.sampleInfo +
        process.hltFilter +
        process.pho_sequence + 
        process.allTagsAndProbes +
        #process.pileupReweightingProducer +
        process.mc_sequence + 
        #process.GsfDRToNearestTauProbe + 
        #process.GsfDRToNearestTauTag + 
        process.tree_sequence +
        process.tree_sequence2
        )
else:
    process.p = cms.Path(
        process.flashggUpdatedIdMVADiPhotons +
        process.flashggDiPhotonMVA +
        process.flashggPreselectedDiPhotons +
        #process.sampleInfo +
        process.hltFilter +
        process.pho_sequence + 
        process.allTagsAndProbes +
        process.mc_sequence +
        process.tree_sequence +
        process.tree_sequence2
        )

process.TFileService = cms.Service(
    "TFileService", fileName = cms.string(options['OUTPUT_FILE_NAME']),
    closeFileFast = cms.untracked.bool(True)
    )

# import flashgg customization
from flashgg.MetaData.JobConfig import customize
# set default options if needed
customize.setDefault("maxEvents",10000)
customize.setDefault("targetLumi",10e+3)
# call the customization
customize(process)
