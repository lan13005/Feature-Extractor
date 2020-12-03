// The goal of this program is to take in some TLorentzVector branches and flattens it into their 4 components

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
void splitLorentzComponents(){
    string fileName="./allMC_trees.root";
    string treeName="degALL_acc_mEllipse_tree_flat";
    // names of the tlorentzvector branches
    vector<string> lorentzBranches = {"g1_p4_kin","g2_p4_kin","g3_p4_kin","g4_p4_kin"};
    int nBranches=4; 

    // Old root file and tree to take data from
    TFile* oldFile = TFile::Open(fileName.c_str());
    TTree* oldTree;
    oldFile->GetObject(treeName.c_str(),oldTree);
    
    Long64_t nentries = oldTree->GetEntries();
    // Create some variables to hold the old data and split into their components to be saved into the new tree
    vector<double> pxs;
    vector<double> pys;
    vector<double> pzs;
    vector<double> es;
    vector<TLorentzVector*> lorentzVecs = {
        new TLorentzVector(),
        new TLorentzVector(),
        new TLorentzVector(),
        new TLorentzVector()
    };
    for (int i=0; i<nBranches; ++i){
        oldTree->SetBranchAddress(lorentzBranches[i].c_str(), &lorentzVecs[i]);
        pxs.push_back(0);
        pys.push_back(0);
        pzs.push_back(0);
        es.push_back(0);
    }

    // Create new root file and tree. Cloning the new tree and adding new branches
    TFile* newFile = TFile::Open("allMC_trees_lorentz.root","RECREATE");
    TTree* newTree = oldTree->CloneTree(-1,"fast");
    vector<TBranch*> newBranches;
    for (int ibranch=0; ibranch<nBranches; ++ibranch){
        newBranches.push_back(newTree->Branch((lorentzBranches[ibranch]+"_px").c_str(),&pxs[ibranch],(lorentzBranches[ibranch]+"_px/D").c_str()));
        newBranches.push_back(newTree->Branch((lorentzBranches[ibranch]+"_py").c_str(),&pys[ibranch],(lorentzBranches[ibranch]+"_py/D").c_str()));
        newBranches.push_back(newTree->Branch((lorentzBranches[ibranch]+"_pz").c_str(),&pzs[ibranch],(lorentzBranches[ibranch]+"_pz/D").c_str()));
        newBranches.push_back(newTree->Branch((lorentzBranches[ibranch]+"_e").c_str(), &es[ibranch],(lorentzBranches[ibranch]+"_e/D").c_str()));
    }
    for (Long64_t i=0; i<nentries; ++i){
        oldTree->GetEntry(i);
        for (int ibranch=0; ibranch<nBranches; ++ibranch){
            pxs[ibranch] = lorentzVecs[ibranch]->Px();
            pys[ibranch] = lorentzVecs[ibranch]->Py();
            pzs[ibranch] = lorentzVecs[ibranch]->Pz();
            es[ibranch] = lorentzVecs[ibranch]->E();
            for (int imom=0; imom<4; ++imom){
                newBranches[ibranch*4+imom]->Fill();
            }
        }
    }
    newTree->Write();
    newFile->Close(); 
}
