#include "Event.h"
//STL
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
//ROOT
#include "TGNumberEntry.h"
#include "TGFileDialog.h"
#include "TEveText.h"
#include "TEveTrans.h"
#include "TGLViewer.h"
#include "TEveViewer.h"
#include "TEveEventManager.h"
// LCIO 
#include <EVENT/LCCollection.h>
#include <EVENT/ReconstructedParticle.h>
#include <EVENT/Vertex.h>
#include <EVENT/MCParticle.h>
#include "UTIL/LCRelationNavigator.h"

// Note
// TEveElement will take care of deleting children. No need to delete one by one.
// Instead just call removeElements from parent object.

ClassImp(Event)

Event::Event() : _view_type(1),
                 _view_type_pre(0), 
                 _nevt_field(0),
                 _b_field(0), 
                 _zmax_field(0), 
                 _fdialog(0),
	             _fileinfo(0),
                 _mcplist(0),
                 //_prop(0),
                 _trklist(0),
                 _pvtxtrklist(0),
                 _svtxtrklist(0),
                 _jetlist(0),
                 _axislist(0),
                 _Blist{ 511, 521, 531, 541, 5122, 5132, 5232, 5332},
                 _Clist{ 411, 421, 431, 4122, 4132, 4232, 4332},
                 _xaxis(0), 
                 _yaxis(0), 
                 _zaxis(0),
                 _xlabel(0)
{} 

void Event::OpenFile() 
{ 
  _ev = 0; 
  if (!_nevt_field) _nevt_field = new TGNumberEntryField(); 
  _nevt_field->SetNumber(_ev); 

  if (!_b_field) _b_field = new TGNumberEntryField(); 
  _b_field->SetNumber(-3.5); 

  if (!_rmax_field) _rmax_field = new TGNumberEntryField(); 
  _rmax_field->SetNumber(1692.1); 

  if (!_zmax_field) _zmax_field = new TGNumberEntryField(); 
  _zmax_field->SetNumber(2225.); 

  if (!_fileinfo) _fileinfo = new TGFileInfo();
  _fileinfo->fIniDir=(char*)".";

  if (!_fdialog) _fdialog = new TGFileDialog(gClient->GetDefaultRoot(),0,kFDOpen,_fileinfo);
  std::cout << "Selected file name = " << _fileinfo->fFilename << std::endl;

  _lcReader = lcio::LCFactory::getInstance()->createLCReader(IO::LCReader::directAccess) ;
  _lcReader->open(_fileinfo->fFilename) ;

  _nevt = _lcReader->getNumberOfEvents();
  _lcReader->getEvents(_runAndEvts); // runnumber + evtnumber

#if 0
  if (!_mcplist) {
    _mcplist = new TEveElementList();
  }

  if (!_trklist) {
    std::cerr << "_trklist has been set." << std::endl;
    _trklist = new TEveTrackList();
    SetPropagator(_trklist);
  }

  if (!_pvtxtrklist) {
    std::cerr << "_pvtxtrklist has been set." << std::endl;
    _pvtxtrklist = new TEveTrackList();
    SetPropagator(_pvtxtrklist);
  }

  if (!_svtxtrklist) {
    std::cerr << "_svtxtrklist has been set." << std::endl;
    _svtxtrklist = new TEveTrackList();
    SetPropagator(_svtxtrklist);
  }

  if (!_jetlist) {
    _jetlist = new TEveElementList();
  }

  if (!_axislist) {
    _axislist = new TEveElementList();
  }
#endif

  // Get first event to get collection names
  std::cerr << "checking collection names stored in this file ..." << std::endl;
  std::cerr << "_nevt = " << _nevt << std::endl;
  //for (int ev = 0; ev < _nevt; ev++) {
  EVENT::LCEvent* evt = GetEvent(1); 
  const std::vector<std::string>* collist = evt->getCollectionNames();
  for (int icol = 0; icol < collist->size(); icol++) {
    //std::cerr << "col name : " << (*collist)[icol] << std::endl;
    _collist.insert((*collist)[icol]);
  }

  std::cerr << "Loading collection names done." << std::endl;
  std::cerr << "N = " << _collist.size() << std::endl;
  std::set<std::string>::iterator colitr;
  int icol = 0;
  for (colitr = _collist.begin(); colitr != _collist.end(); colitr++) {

    icol++;
    _col_mcps->AddEntry(colitr->c_str(),icol);
    _col_tracks->AddEntry(colitr->c_str(),icol);
    _col_rel->AddEntry(colitr->c_str(),icol);
    _col_pvtxtracks->AddEntry(colitr->c_str(),icol);
    _col_svtxtracks->AddEntry(colitr->c_str(),icol);
    _col_jets->AddEntry(colitr->c_str(),icol);

    // set default
    if ((*colitr)=="MCParticle") _col_mcps->Select(icol);
    if ((*colitr)=="PandoraPFOs") _col_tracks->Select(icol);
    if ((*colitr)=="RecoMCTruthLink") _col_rel->Select(icol);
    if ((*colitr)=="PrimaryVertex") _col_pvtxtracks->Select(icol);
    if ((*colitr)=="BuildUpVertex") _col_svtxtracks->Select(icol);
    if ((*colitr)=="PrimaryVertex_NewVtx") _col_pvtxtracks->Select(icol);
    if ((*colitr)=="BuildUpVertex_NewVtx") _col_svtxtracks->Select(icol);
    if ((*colitr)=="RecoveredJets") _col_jets->Select(icol);
    if ((*colitr)=="RefinedJets") _col_jets->Select(icol);

  }
  std::cerr << "Open file done." << std::endl;
}

void Event::UpdatePropagator() 
{
//std::cerr << "Event::UpdatePropagator() before delete" << std::endl;
//  if (_prop) delete _prop;
//std::cerr << "Event::UpdatePropagator() after delete" << std::endl;
//  _prop = new TEveTrackPropagator();
  _prop.bz = float(_b_field->GetNumber());
  _prop.maxR = float(_rmax_field->GetNumber());
  _prop.maxZ = float(_zmax_field->GetNumber());
  _prop.maxOrbs = 1.;
  //_prop->SetMaxAng(90.);
  //_prop->SetMaxStep(100.);
  //std::cerr << " GetMaxAng() = " << _prop->GetMaxAng() << 
  //             " GetMaxOrbs() = " << _prop->GetMaxOrbs() << 
  //             " GetDelta() = " << _prop->GetDelta() << 
  //             " GetMaxStep() = " << _prop->GetMaxStep() << std::endl;
}

void Event::SetPropagator(TEveTrackList* in) 
{
#if 0
  if (!_prop) {
    _prop = in->GetPropagator();
  }
#endif
  UpdatePropagator();
}

std::string Event::GetCollectionNameByEntryId(int id) {
  std::set<std::string>::iterator colitr;
  int icol = 0;
  for (colitr = _collist.begin(); colitr != _collist.end(); colitr++) {
    icol++;
    if (icol==id) {
      return *colitr;
    }
  } 
}

void Event::SetNevtField(TGNumberEntryField* in) 
{
  _nevt_field = in;
}

void Event::SetBField(TGNumberEntryField* in) 
{
  _b_field = in;
}

void Event::SetRmaxField(TGNumberEntryField* in) 
{
  _rmax_field = in;
}

void Event::SetZmaxField(TGNumberEntryField* in) 
{
  _zmax_field = in;
}

void Event::SetMCParticleVisState(TGCheckButton* in)
{
  _vis_mcps = in;
}

void Event::SetTrackVisState(TGCheckButton* in)
{
  _vis_tracks = in;
}

void Event::SetPVtxTrackVisState(TGCheckButton* in)
{
  _vis_pvtxtracks = in;
}

void Event::SetSVtxTrackVisState(TGCheckButton* in)
{
  _vis_svtxtracks = in;
}

void Event::SetJetVisState(TGCheckButton* in)
{
  _vis_jets = in;
}

void Event::SetAxisVisState(TGCheckButton* in)
{
  _vis_axis = in;
}

void Event::SetMCParticleColList(TGComboBox* in)
{
  _col_mcps = in;
}

void Event::SetRelColList(TGComboBox* in)
{
  _col_rel = in;
}

void Event::SetTrackColList(TGComboBox* in)
{
  _col_tracks = in;
}

void Event::SetPVtxTrackColList(TGComboBox* in)
{
  _col_pvtxtracks = in;
}

void Event::SetSVtxTrackColList(TGComboBox* in)
{
  _col_svtxtracks = in;
}

void Event::SetJetColList(TGComboBox* in)
{
  _col_jets = in;
}

void Event::Next()
{
  _ev++;
  _nevt_field->SetIntNumber(_ev);
  Update();
}

void Event::Prev()
{
  _ev--;
  _nevt_field->SetIntNumber(_ev);
  Update();
}

void Event::Goto()
{
  _ev=_nevt_field->GetIntNumber();
  Update();
}

void Event::ViewSet(Int_t id)
{
  std::cerr << "ViewSet called id = " << id << std::endl;
  _view_type = id;
}

void Event::DumpMCParticles()
{
  std::cerr << "DumpMCParticles called" << std::endl;
  if ( _col_mcps->GetSelectedEntry()) { 
    EVENT::LCEvent* evt = GetEvent(_ev); 
    if (evt) { 
      std::string colname_mcps = GetCollectionNameByEntryId(_col_mcps->GetSelectedEntry()->EntryId());
      EVENT::LCCollection* colmcp = evt->getCollection(colname_mcps.c_str()) ;

      int nMCP = colmcp->getNumberOfElements();
      //std::cerr << "nMCP "<< nMCP << std::endl;

      // MCParticle index map
      _mcpIndex.clear();
      for(int i=0; i< nMCP ; i++){
         EVENT::MCParticle* p = dynamic_cast<EVENT::MCParticle*>( colmcp->getElementAt( i ) ) ;
         // making MCParticle index map
         _mcpIndex.insert(std::map<EVENT::MCParticle*,int>::value_type(p,i)); 
      }
      //std::cout.setf(std::ios::right, std::ios::adjustfield);
      std::stringstream dump;
      dump <<  "##### Dump MCParticles #####" << std::endl;
      dump <<  "index    pdg      e     px     py     pz  parent-daughter" << std::endl;
      for(int i=0; i< nMCP ; i++){
         EVENT::MCParticle* p = dynamic_cast<EVENT::MCParticle*>( colmcp->getElementAt( i ) ) ;
         const EVENT::MCParticleVec& parents = p->getParents();
         std::vector<int> parentIndices;
         for (int ip = 0; ip < parents.size(); ip++) {
           EVENT::MCParticle* parent = dynamic_cast<EVENT::MCParticle*>( parents[ip] ) ;
           // Find MCParticle index
           std::map<EVENT::MCParticle*,int>::iterator mcindexitr;
           mcindexitr = _mcpIndex.find(parent);
           if (mcindexitr != _mcpIndex.end()) parentIndices.push_back(mcindexitr->second);
         }
         const EVENT::MCParticleVec& daughters = p->getDaughters();
         std::vector<int> daughterIndices;
         for (int id = 0; id < daughters.size(); id++) {
           EVENT::MCParticle* daughter = dynamic_cast<EVENT::MCParticle*>( daughters[id] ) ;
           // Find MCParticle index
           std::map<EVENT::MCParticle*,int>::iterator mcindexitr;
           mcindexitr = _mcpIndex.find(daughter);
           if (mcindexitr != _mcpIndex.end()) daughterIndices.push_back(mcindexitr->second);
         }

         int pdg = p->getPDG();
         //if (1000020040)
         char mod = ' ';
         if (pdg>1000000000) {
           pdg -= 1000000000;  
           mod = '*';
         }
         const double* mom = p->getMomentum();
         double e = p->getEnergy();

         dump << std::setw(4) << i;
         dump << " " << std::setw(7) << std::setfill(mod) << pdg;
         dump << std::setfill(' ');
         dump << std::setw(7) << std::fixed << std::setprecision(1) << e;
         dump << std::setw(7) << std::fixed << std::setprecision(1) << mom[0];
         dump << std::setw(7) << std::fixed << std::setprecision(1) << mom[1];
         dump << std::setw(7) << std::fixed << std::setprecision(1) << mom[2];
         dump << "  [";
         for (int ip = 0; ip < parentIndices.size(); ip++) {
           if (ip==0) dump << parentIndices[ip];
           else       dump << "," << parentIndices[ip];
         }
         dump << "] - [";
         for (int id = 0; id < daughterIndices.size(); id++) {
           if (id==0) dump << daughterIndices[id];
           else       dump << "," << daughterIndices[id];
         }
         dump << "]";
         dump << std::endl;
      }
      std::stringstream cmd;
      cmd << "(xterm -bg black -fg white -e \"echo \\\"\\\033]2; Evt " << _ev << "  \007\\\"  && echo \\\"" << dump.str().data() << "\\\" | less\") &"<< std::ends;
      std::system(cmd.str().data());
    } else {
       std::cerr << "Set event number to dump information." << std::endl;
    } 
  } else {
     std::cerr << "Select input file to dump information." << std::endl;
  }
}

EVENT::LCEvent* Event::GetEvent(int ev)
{
  // First event seems to be ev = 0. 
  int ev_new = ev - 1; // This is necessary to match event number in dumpevent
  //std::cerr << "_ev = " << ev << std::endl;
  if (ev_new < _runAndEvts.size()/2) { // 2 corresponds to run number and event number
    int runnumber = _runAndEvts[2*ev_new];
    int evtnumber = _runAndEvts[2*ev_new+1];
    EVENT::LCEvent* evt = _lcReader->readEvent(runnumber,  evtnumber) ; 
    //std::cout << "current ev = " << evt->getEventNumber() << std::endl;
    return evt;   
  } 
  return 0;
}

void Event::Update()
{
  UpdatePropagator();
  EVENT::LCEvent* evt = GetEvent(_ev); 
  if (evt) {
#if 1
    TEveEventManager* eveEvent = gEve->GetCurrentEvent(); 
    if (eveEvent) {
      //TEveElement::List_i iter;
      //for (iter = eveEvent->BeginChildren(); iter != eveEvent->EndChildren(); iter++) {
      //  std::cerr << "Registered elements in eveEvent : " << (*iter)->GetElementName() << std::endl;
      //  //if ((*iter)->HasChildren()) { // clear daughter TEveElements
      //  //  (*iter)->DestroyElements();
      //  //}
      //}
      eveEvent->RemoveElements();
      _mcplist = 0; 
      _trklist = 0;
      _pvtxtrklist = 0;
      _svtxtrklist = 0;
      _jetlist = 0;
      _axislist = 0;
    }
#endif

    // collecting new updated information 
    // deciding what to be drawn
    ProcessMCParticles(evt); 
std::cerr << "after ProcessMCParticles" << std::endl;
    ProcessTracks(evt);     // ProcessMCParticles should be called beforehand to use MCParticle index
std::cerr << "after ProcessTracks" << std::endl;
    ProcessPVtxTracks(evt); 
    ProcessSVtxTracks(evt); 
    ProcessJets(evt); 

    // passing the updates to gEve
    UpdateMCParticles();
    UpdateTracks();
    UpdatePVtxTracks();
    UpdateSVtxTracks();
    UpdateJets();
    UpdateAxis();

    Draw();
  } 
  //lcReader->close();
}  

void Event::ProcessMCParticles(EVENT::LCEvent* evt) {
//    for (auto itr = _mcps.rbegin(); itr != _mcps.rend(); itr++) delete *itr;
//std::cerr << "after delete _mcps elements" << std::endl;
    _mcps.clear();
//std::cerr << "after _mcps.clear" << std::endl;
    _mcps.shrink_to_fit();
//std::cerr << "after _mcps.shrink_to_fit" << std::endl;
//std::cerr << "_mcps.size() = " << _mcps.size() << std::endl;
    // MCParticle index map
    _mcpIndex.clear();
    if (_col_mcps->GetSelectedEntry() && _vis_mcps->IsOn()) { 
      std::string colname_mcps = GetCollectionNameByEntryId(_col_mcps->GetSelectedEntry()->EntryId());
      EVENT::LCCollection* colmcp = evt->getCollection(colname_mcps.c_str()) ;

      int nMCP = colmcp->getNumberOfElements();
      std::cerr << "nMCP "<< nMCP << std::endl;
      for(int i=0; i< nMCP ; i++){
         EVENT::MCParticle* p = dynamic_cast<EVENT::MCParticle*>( colmcp->getElementAt( i ) ) ;

         // making MCParticle index map
         _mcpIndex.insert(std::map<EVENT::MCParticle*,int>::value_type(p,i)); 

         int pdg = p->getPDG();
         int parentpdg = 0;
         int nParents = p->getParents().size(); 
         int nDaughters = p->getDaughters().size();

         // Filtering
         //if (nParents==0) continue;  // This shouldn't be a final state.
         //if (nDaughters>0) continue; // This shouldn't be a final state.
         bool isSemistable = false;
         bool isParentSemistable = false;
         std::vector<int>::iterator itr;
         itr = find(_Blist.begin(), _Blist.end(), abs(pdg));
         if (itr!=_Blist.end()) isSemistable = true; 
         itr = find(_Clist.begin(), _Clist.end(), abs(pdg));
         if (itr!=_Clist.end()) isSemistable = true; 
         if (nParents>0) {
           parentpdg = p->getParents()[0]->getPDG();
           itr = find(_Blist.begin(), _Blist.end(), abs(parentpdg));
           if (itr!=_Blist.end()) isParentSemistable = true; 
           itr = find(_Clist.begin(), _Clist.end(), abs(parentpdg));
           if (itr!=_Clist.end()) isParentSemistable = true; 
         }

#if 1
         // Temporary solution to get generator partons
         bool isFirstPartons = false;
         if (i<12&&i>4) {
           if (abs(pdg)>=1&&abs(pdg)<=16&&abs(pdg)!=5&&abs(pdg)!=4) isFirstPartons = true; // heavy quark should be treated above, not here.
         } 

         bool isSingleParticle = false;
         if (nMCP<10) { // Single particle sample
           isSingleParticle = true; // heavy quark should be treated above, not here.
         }
#endif

         if (!isSemistable && !isParentSemistable && !isFirstPartons && !isSingleParticle) continue; 

         //std::cout << p->getPDG() << std::endl;
         //std::cout << p->getCharge() << std::endl;
         std::cout << " i = " << i << " pdg = " << pdg  << std::endl;
         //std::cout << p->getEnergy() << std::endl;
         EVENT::MCParticle* parent = 0;
#if 0
         const double* mom = p->getMomentum();
         const double* sp = p->getVertex();
         const double* ep = p->getEndpoint();
#else
         TVector3 mom(p->getMomentum());
         TVector3 sp(p->getVertex());
         TVector3 ep(p->getEndpoint());
         TVector3 trkseg(ep-sp);
#endif
         std::cout << "pdg = " << pdg << " parent_pdg = " << parentpdg << std::endl;
         std::cout << "px = " << mom.X() << " py = " << mom.Y() << " pz = " << mom.Z() << std::endl;
         std::cout << "sx = " << sp.X() << " sy = " << sp.Y() << " sz = " << sp.Z() << std::endl;
         std::cout << "ex = " << ep.X() << " ey = " << ep.Y() << " ez = " << ep.Z() << std::endl;

         if (nParents>0) {
           parent = p->getParents()[0];
#if 0
           const double* ep_parent = parent->getEndpoint();
           std::cout << "ex_parent = " << ep_parent[0] << " ey_parent = " << ep_parent[1] << " ez_parent = " << ep_parent[2] << std::endl;
#else
           TVector3 ep_parent(parent->getEndpoint());
           std::cout << "ex_parent = " << ep_parent.X() << " ey_parent = " << ep_parent.Y() << " ez_parent = " << ep_parent.Z() << std::endl;
#endif
         }

         MCParticle* mcp;
#if 0
         // Fixed length for parton/parent of Semisateble/single particle cases (sp-ez line is a bit mis-leading.)
         if (isFirstPartons||isParentSemistable||isSingleParticle) {
           TVector3 vec(mom);
           //mcp = new MCParticle(vec.Unit().X(),vec.Unit().Y(),vec.Unit().Z(),sp[0],sp[1],sp[2]);
           mcp = new MCParticle(float(vec.X()),float(vec.Y()),float(vec.Z()),float(sp[0]),float(sp[1]),float(sp[2]));
         } 
         else mcp = new MCParticle(float(ep[0]-sp[0]),float(ep[1]-sp[1]),float(ep[2]-sp[2]),float(sp[0]),float(sp[1]),float(sp[2]));
#else
         // Short track is better to draw line from sp to ep.
         const float maxlen_short_track = 100.; // FIXME
         if (trkseg.Mag() < maxlen_short_track) {
           mcp = new MCParticle(float(trkseg.X()),float(trkseg.Y()),float(trkseg.Z()),float(sp.X()),float(sp.Y()),float(sp.Z()));
         } 
         else { // Long track is better to draw just momentum.
           mcp = new MCParticle(float(mom.X()),float(mom.Y()),float(mom.Z()),float(sp.X()),float(sp.Y()),float(sp.Z()));
           // Endpoint should be overwritten for this case.
           mcp->_endx = ep.X();
           mcp->_endy = ep.Y();
           mcp->_endz = ep.Z();
         }
#endif

         // Turned out that line width depends on TubeR and arrow length!
         // Should be normalized by length to keep a certain line width.
         //TVector3 vec(ep[0]-sp[0],ep[1]-sp[1],ep[2]-sp[2]);
         float length = mcp->GetVector().Mag();
//if (isFirstPartons) std::cerr << "length = " << length << " " << mcp->GetVector().fX << " " << mcp->GetVector().fY << " " << mcp->GetVector().fZ << std::endl;
std::cerr << "length = " << length << " " << mcp->GetVector().fX << " " << mcp->GetVector().fY << " " << mcp->GetVector().fZ << std::endl;
         if (length<1e-2) continue; // neglect too-short tracks except for initial partons
         if (isSemistable) {
           mcp->SetMainColor(kBlue);
           mcp->SetTubeR(0.02/length);
           mcp->SetConeR(.05/length);
           mcp->SetConeL(.05/length);
         } else if (isParentSemistable) {
std::cerr << "### isParentSemisatable trkseg.Mag() = " << trkseg.Mag() << std::endl;
           mcp->SetMainColor(kBlue-7);
           mcp->SetTubeR(0.02/length);
           mcp->SetConeR(0.05/length);
           mcp->SetConeL(0.05/length);
         } else if (isFirstPartons) {
std::cerr << "FirstParton found pdg = " << pdg << " index = " << i << std::endl;
           mcp->SetMainColor(kRed);
           mcp->SetTubeR(0.02/length);
           mcp->SetConeR(.05/length);
           mcp->SetConeL(.05/length);
         } else if (isSingleParticle) {
           mcp->SetMainColor(kRed);
           mcp->SetTubeR(0.04/length);
           mcp->SetConeR(.1/length);
           mcp->SetConeL(.1/length);
         }

         if (trkseg.Mag() > maxlen_short_track) {
std::cerr << "trkseg.Mag() = " << trkseg.Mag() << std::endl;
           mcp->SetMainColor(kYellow); // Making different for momentum arrow
         }
         mcp->_index = i;
         mcp->SetPickable(kTRUE);
         mcp->_px = mom.X();
         mcp->_py = mom.Y();
         mcp->_pz = mom.Z();
         mcp->_e = p->getEnergy();
         mcp->_pdg = pdg;
         mcp->_parentpdg = 0;
         if (!isSemistable && p->getCharge()==0) mcp->SetMainColor(kGray+2); // Set grey color for non-charged tracks except for B or C hadrons
         if (parent) mcp->_parentpdg= parent->getPDG();

         _mcps.push_back(mcp);
std::cerr << "_mcps.size() = " << _mcps.size() << std::endl;
      }
    }
}

void Event::ProcessTracks(EVENT::LCEvent* evt) {
std::cerr << "Event::ProcessTracks called" << std::endl;
    for (auto itr = _tracks.rbegin(); itr != _tracks.rend(); itr++) delete *itr;
    _tracks.clear();
    _tracks.shrink_to_fit();
std::cerr << "delete previous tracks" << std::endl;
    // PFO info
    if (_col_tracks->GetSelectedEntry() && _vis_tracks->IsOn()) { 
      std::string colname_tracks = GetCollectionNameByEntryId(_col_tracks->GetSelectedEntry()->EntryId());
      EVENT::LCCollection* colpfo = evt->getCollection(colname_tracks.c_str()) ;
      std::string colname_rel = GetCollectionNameByEntryId(_col_rel->GetSelectedEntry()->EntryId());
      EVENT::LCCollection* colRel = evt->getCollection(colname_rel.c_str());
	  UTIL::LCRelationNavigator relnav(colRel);
      //EVENT::LCCollection* colpfo = evt->getCollection("PandoraPFOs") ;
      int nPFO = colpfo->getNumberOfElements();
      for(int i=0; i< nPFO ; i++){
         std::cout << "PFO " << i << std::endl;
         EVENT::ReconstructedParticle* p = dynamic_cast<EVENT::ReconstructedParticle*>( colpfo->getElementAt( i ) ) ;
         int nrel = relnav.getRelatedToObjects(p).size();
         // Finding correspoinding MCParticle
         double trkwmax;
         EVENT::MCParticle *best = 0;
         for (int imc = 0; imc < nrel; imc++ ) {
           EVENT::MCParticle *mcp = dynamic_cast<EVENT::MCParticle*>(relnav.getRelatedToObjects(p)[imc]);
           double trkw = double((int(relnav.getRelatedToWeights(p)[imc])%10000)/1000.);
           if (imc==0) {
             trkwmax = trkw;
             best = mcp;
           } else if (trkw > trkwmax) {
             trkwmax = trkw;
             best = mcp;
           }
         }

         //std::cout << p->getType() << std::endl; // same as PDG
         //std::cout << p->getCharge() << std::endl;
         const double* mom = p->getMomentum();
         //std::cout << "px = " << mom[0] << " py = " << mom[1] << " pz = " << mom[2] << std::endl;
         //std::cout << p->getEnergy() << std::endl;
#if 0
         if (p->getStartVertex()) {
           const float* sp = p->getStartVertex()->getPosition(); 

           Track* trk = new Track(mom[0],mom[1],mom[2],p->getEnergy(),sp[0],sp[1],sp[2],0.,p->getType(),_prop);
           _tracks.push_back(trk);

         }
#else
         const EVENT::TrackVec& tracks = p->getTracks();
         for (int itrk = 0; itrk < tracks.size(); itrk++) {
           EVENT::Track* lciotrk = tracks[itrk];
           double d0 = lciotrk->getD0();  
           double z0 = lciotrk->getZ0();  
           double phi0 = lciotrk->getPhi();  
           //  phi0 - pi/2 => phi0 in the KalTest note.
           //  double x = refx - d0 * sin(phi0) + rho * cos(phi0 - pi/2) - rho * cos(-sign * t + phi0 - pi/2 );
           //  double y = refy + d0 * cos(phi0) + rho * sin(phi0 - pi/2) - rho * sin(-sign * t + phi0 - pi/2 );
           //  double z = refz + z0 - rho * (-sign * t) * tanl;
           
           //  ref point = (0,0,0), t = 0 
           //  -->
           double x0 =  - d0 * TMath::Sin(phi0);
           double y0 =  + d0 * TMath::Cos(phi0);
           Track* trk = new Track(mom[0],mom[1],mom[2],p->getEnergy(),x0,y0,z0,0.,p->getType(),_prop);
           trk->_mcp_pdg = best->getPDG();
           if (best->getParents().size()>0) {
             trk->_mcparent_pdg = best->getParents()[0]->getPDG();
           }

           trk->_d0       = d0;
           trk->_z0       = z0;
           trk->_chi2     = lciotrk->getChi2();
           trk->_ndf      = lciotrk->getNdf();
           trk->_rmin_hit = lciotrk->getRadiusOfInnermostHit();
           const EVENT::FloatVec& cov = lciotrk->getCovMatrix();
           trk->_d0err    = sqrt(cov[0]); // d0-d0, phi-d0, phi-phi, omg-d0, omg-phi, omg-omg, z0-d0, z0-phi, z0-omg, z0-z0, tanl-d0, ... 
           trk->_z0err    = sqrt(cov[9]); 
           const float* ref_pos = lciotrk->getReferencePoint();
           trk->_ref_x = ref_pos[0];
           trk->_ref_y = ref_pos[1];
           trk->_ref_z = ref_pos[2];
           const EVENT::TrackStateVec& states = lciotrk->getTrackStates();
           for (int istate = 0; istate < states.size(); istate++) {
              const float* ref_pos = states[istate]->getReferencePoint();
              double d0s = states[istate]->getD0(); 
              double z0s = states[istate]->getZ0(); 
              double phi0s = states[istate]->getPhi(); 
              double x0s =  - d0s * TMath::Sin(phi0s);
              double y0s =  + d0s * TMath::Cos(phi0s);
              TVector3 refv(ref_pos);
              std::cerr << "state " << istate << " R = " << refv.Mag() << std::endl; 
              std::cerr << "ref (" << ref_pos[0] << ", " << ref_pos[1] << ", " << ref_pos[2] << ")" << std::endl; 
              std::cerr << "pos (" << x0s << ", " << y0s << ", " << z0s << ")" << std::endl; 
           }

           // Find MCParticle index
           std::map<EVENT::MCParticle*,int>::iterator mcindexitr;
           mcindexitr = _mcpIndex.find(best);
           if (mcindexitr != _mcpIndex.end()) trk->_mcp_index = mcindexitr->second;
           else trk->_mcp_index = -1;

           _tracks.push_back(trk);
         }
#endif
         //if (p->getEndVertex()) {
         //  const float* endpoint = p->getEndVertex()->getPosition(); 
         //  std::cout << "x1 = " << endpoint[0] << " y1 = " << endpoint[1] << " z1 = " << endpoint[2] << std::endl;
         //}
      }
    }
}

void Event::ProcessPVtxTracks(EVENT::LCEvent* evt) {
    for (auto itr = _pvtxtracks.rbegin(); itr != _pvtxtracks.rend(); itr++) delete *itr;
    _pvtxtracks.clear();
    _pvtxtracks.shrink_to_fit();
    // Primary Vertex info
    if (_col_pvtxtracks->GetSelectedEntry() && _vis_pvtxtracks->IsOn()) { 
      std::string colname_pvtxtracks = GetCollectionNameByEntryId(_col_pvtxtracks->GetSelectedEntry()->EntryId());
      EVENT::LCCollection* colpvtx = evt->getCollection(colname_pvtxtracks.c_str()) ;
      int nPvtx = colpvtx->getNumberOfElements();
      std::cerr << "nPvtx = " << nPvtx << std::endl;
      for (int iv = 0; iv < nPvtx; iv++) {
        EVENT::Vertex* pvtx = dynamic_cast<EVENT::Vertex*>( colpvtx->getElementAt( iv ) );
        const float* vpos = pvtx->getPosition(); 
        EVENT::ReconstructedParticle* vp = pvtx->getAssociatedParticle();
        int nAssociatedParticles = vp->getParticles().size();
        std::cerr << "Associated particles = " << nAssociatedParticles << std::endl;
        for (int ip = 0; ip < nAssociatedParticles; ip++) {
          EVENT::ReconstructedParticle* p = vp->getParticles()[ip];
          const double* mom = p->getMomentum();
          Track* trk = new Track(mom[0],mom[1],mom[2],p->getEnergy(),vpos[0],vpos[1],vpos[2],0.,p->getType(),_prop);
          trk->SetLineColor(kYellow);
          _pvtxtracks.push_back(trk);
        }
      }
    }
}

void Event::ProcessSVtxTracks(EVENT::LCEvent* evt) {
    for (auto itr = _svtxtracks.rbegin(); itr != _svtxtracks.rend(); itr++) delete *itr;
    _svtxtracks.clear();
    _svtxtracks.shrink_to_fit();
    // Secondary Vertex info
    if (_col_svtxtracks->GetSelectedEntry() && _vis_svtxtracks->IsOn()) { 
      std::string colname_svtxtracks = GetCollectionNameByEntryId(_col_svtxtracks->GetSelectedEntry()->EntryId());
      EVENT::LCCollection* colsvtx = evt->getCollection(colname_svtxtracks.c_str()) ;
      int nSvtx = colsvtx->getNumberOfElements();
      std::cerr << "nSvtx = " << nSvtx << std::endl;
      for (int iv = 0; iv < nSvtx; iv++) {
        EVENT::Vertex* svtx = dynamic_cast<EVENT::Vertex*>( colsvtx->getElementAt( iv ) );
        const float* vpos = svtx->getPosition(); 
        EVENT::ReconstructedParticle* vp = svtx->getAssociatedParticle();
        int nAssociatedParticles = vp->getParticles().size();
        std::cerr << "Associated particles = " << nAssociatedParticles << std::endl;
        for (int ip = 0; ip < nAssociatedParticles; ip++) {
          EVENT::ReconstructedParticle* p = vp->getParticles()[ip];
          const double* mom = p->getMomentum();
          Track* trk = new Track(mom[0],mom[1],mom[2],p->getEnergy(),vpos[0],vpos[1],vpos[2],0.,p->getType(),_prop);
          //trk->SetLineColor(kCyan);
          trk->SetLineColor(6);
          trk->SetLineWidth(4);
          //trk->SetLineColor(kMagenta); // This doesn't show anything..
          _svtxtracks.push_back(trk);
        }
      }
    }
}

void Event::ProcessJets(EVENT::LCEvent* evt) {
std::cerr << "start jet analysis" << std::endl;
    //for (auto itr = _jets.rbegin(); itr != _jets.rend(); itr++) delete *itr;
    _jets.clear();
    _jets.shrink_to_fit();
    // Jet info
    std::cerr << "_col_jets->GetSelectedEntry() " << _col_jets->GetSelectedEntry() << std::endl;
    if (_col_jets->GetSelectedEntry() && _vis_jets->IsOn()) { 
      std::string colname_jets = GetCollectionNameByEntryId(_col_jets->GetSelectedEntry()->EntryId());
      EVENT::LCCollection* coljet = evt->getCollection(colname_jets.c_str()) ;
      int nJet = coljet->getNumberOfElements();

      std::string colname_rel = GetCollectionNameByEntryId(_col_rel->GetSelectedEntry()->EntryId());
      EVENT::LCCollection* colRel = evt->getCollection(colname_rel.c_str());
	  UTIL::LCRelationNavigator relnav(colRel);

      // Try to roughly estimate the vertex position
      float z0 = 0.;
#if 0
      int ntracks = 0;
      for(int i=0; i< nJet ; i++){
         EVENT::ReconstructedParticle* p = dynamic_cast<EVENT::ReconstructedParticle*>( coljet->getElementAt( i ) ) ;
         const EVENT::ReconstructedParticleVec& tracks = p->getParticles(); 
         if (tracks.size()>0) {
            for (int itrk = 0; itrk < tracks.size(); itrk++) {
              for (int itrk2 = 0; itrk2 < tracks[itrk]->getTracks().size(); itrk2++ ) {
                 EVENT::Track* atrack = tracks[itrk]->getTracks()[itrk2]; 
                 //std::cerr << "d0 = " << atrack->getD0() << " z0 = " << atrack->getZ0() << " phi0 = " << atrack->getPhi() << std::endl;
                 z0 += atrack->getZ0();
                 ntracks++;
              } 
            }
         }
      }
      z0 /= ntracks; // average 
#endif

      for(int i=0; i< nJet ; i++){
         std::cout << "Jet " << i << std::endl;
         Jet* jet = new Jet;

         EVENT::ReconstructedParticle* p = dynamic_cast<EVENT::ReconstructedParticle*>( coljet->getElementAt( i ) ) ;
         //TVector3 vpos(p->getStartVertex()->getPosition());
         //std::cerr << "vpos = (" << vpos.X() << ", " << vpos.Y() << ", " << vpos.Z() << ")" << std::endl;
         //const EVENT::TrackVec& tracks = p->getTracks();

         const double* mom = p->getMomentum();
         TVector3 pv(mom);

         double cosmin = 1.;
         double angle = 0.;
         const EVENT::ReconstructedParticleVec& tracks = p->getParticles();
         for(int itrk =0; itrk < tracks.size() ; itrk++){

            EVENT::ReconstructedParticle* trk = tracks[itrk];

            int nrel = relnav.getRelatedToObjects(trk).size();
            // Finding correspoinding MCParticle
            double trkwmax;
            EVENT::MCParticle *best = 0;
            for (int imc = 0; imc < nrel; imc++ ) {
              EVENT::MCParticle *mcp = dynamic_cast<EVENT::MCParticle*>(relnav.getRelatedToObjects(trk)[imc]);
              double trkw = double((int(relnav.getRelatedToWeights(trk)[imc])%10000)/1000.);
              if (imc==0) {
                trkwmax = trkw;
                best = mcp;
              } else if (trkw > trkwmax) {
                trkwmax = trkw;
                best = mcp;
              }
            }
            
            const double* momtrk = trk->getMomentum();
            TVector3 pvtrk(momtrk);
            double cos = pv.Unit().Dot(pvtrk.Unit());
            angle += TMath::ACos(cos);
            if (cosmin > cos) {
               cosmin = cos;
            }

            if (best) jet->_trkmcps.push_back(best);
//std::cerr << "   cos Theta for track " << itrk << " and jet " << i << " : " << pv.Unit().Dot(pvtrk.Unit()) << std::endl;
         }
         angle /= tracks.size();
//std::cerr << "cos min Theta for jet " << i << " : " << cosmin << std::endl;
//std::cerr << "cosTheta average for jet " << i << " : " << TMath::Cos(angle) << std::endl;
//std::cerr << "Theta average for jet " << i << " : " << angle << std::endl;
         

         jet->_px = mom[0];
         jet->_py = mom[1];
         jet->_pz = mom[2];
         jet->SetApex(TEveVector(0.,0.,-z0));
         jet->SetCylinder(250,250);
         //jet->AddCone(pv.Eta(),pv.Phi(),0.1); // coneR = 0.1 tentative
         jet->AddCone(pv.Eta(),pv.Phi(),angle); // coneR = 0.1 tentative
         jet->SetMainAlpha(0.5);
         jet->_e = p->getEnergy();
         _jets.push_back(jet);
      }
    }
}

void Event::UpdateMCParticles() {
//std::cerr << "Event::UpdateMCParticles() called" << std::endl;
//  if (_mcplist->HasChildren()) { // clear daughter TEveElements
//std::cerr << "_mcplist->NumChildren() = " << _mcplist->NumChildren() << std::endl;
//    _mcplist->DestroyElements();
//  }
  if (!_mcplist) {
    _mcplist = new TEveElementList();
  }
  

  if (_vis_mcps->IsOn()) { 
    for (int i = 0; i < _mcps.size(); i++) {
    //for (int i = 1; i < _mcps.size(); i++) {
      _mcplist->AddElement(_mcps[i]);
    }
std::cerr << "_mcplist->NumChildren() = " << _mcplist->NumChildren() << std::endl;
    gEve->AddElement(_mcplist);
  }

}

void Event::UpdateTracks() {
//std::cerr << "Event::UpdateTracks() called" << std::endl;
//  if (_trklist->HasChildren()) { // clear daughter TEveElements
//std::cerr << "Preparing DestroyElements" << std::endl;
//  TEveElement::List_i vitr;
//  for (vitr = _trklist->BeginChildren(); vitr != _trklist->EndChildren(); vitr++ ) {
//     //std::cerr << " element name = " << (*vitr)->GetElementName() << std::endl;
//     TEveTrack* evtrk = dynamic_cast<TEveTrack*>(*vitr);
//std::cerr << "SetPropagator(0) before calling DestroyElements" << std::endl;
//std::cerr << "evtrk = " << evtrk << std::endl;
//     TEveTrackPropagator* tmpprop = evtrk->GetPropagator();
//std::cerr << "tmpprop = " << tmpprop << std::endl;
//std::cerr << "name = " << evtrk->GetElementName() << std::endl;
//std::cerr << "charge = " << evtrk->GetCharge() << std::endl;
//     evtrk->SetPropagator(0); //なぜかこれを呼ぶとsegmentation faultになる場合がある。evtrkが正しいアドレスを指していない？ --> TrackPropagatorは自動的にdeleteされるので、すべてのtrackでnewする必要があった。
//  }
//std::cerr << "DestroyElements called" << std::endl;
//    _trklist->DestroyElements();
//std::cerr << "DestroyElements done" << std::endl;
//  }
  if (!_trklist) {
    std::cerr << "_trklist has been set." << std::endl;
    _trklist = new TEveTrackList();
    SetPropagator(_trklist);
  }

  if (_vis_tracks->IsOn()) { 
    for (int i = 0; i < _tracks.size(); i++) {
      // step4 register the track to the track list.
      //_trklist->AddElement(_tracks[i].GetTrack(_prop));
      _trklist->AddElement(_tracks[i]->GetTEveTrack());
    }
    gEve->AddElement(_trklist);
  }
}

void Event::UpdatePVtxTracks() {

  //if (_pvtxtrklist->HasChildren()) { // clear daughter TEveElements
  //  _pvtxtrklist->DestroyElements();
  //}
  if (!_pvtxtrklist) {
    std::cerr << "_pvtxtrklist has been set." << std::endl;
    _pvtxtrklist = new TEveTrackList();
    SetPropagator(_pvtxtrklist);
  }

  if (_vis_pvtxtracks->IsOn()) { 
    for (int i = 0; i < _pvtxtracks.size(); i++) {
      _pvtxtrklist->AddElement(_pvtxtracks[i]->GetTEveTrack());
    }
    gEve->AddElement(_pvtxtrklist);
  }
}

void Event::UpdateSVtxTracks() {

  //if (_svtxtrklist->HasChildren()) { // clear daughter TEveElements
  //  _svtxtrklist->DestroyElements();
  //}
  if (!_svtxtrklist) {
    std::cerr << "_svtxtrklist has been set." << std::endl;
    _svtxtrklist = new TEveTrackList();
    SetPropagator(_svtxtrklist);
  }


  if (_vis_svtxtracks->IsOn()) { 
    for (int i = 0; i < _svtxtracks.size(); i++) {
      _svtxtrklist->AddElement(_svtxtracks[i]->GetTEveTrack());
    }
    gEve->AddElement(_svtxtrklist);
  }
}

void Event::UpdateJets() {
  std::cerr << "UpdateJets called" << std::endl;
  //if (_jetlist->HasChildren()) { // clear daughter TEveElements
  //  _jetlist->DestroyElements();
  //}


  if (_vis_jets->IsOn()) { 

    if (!_jetlist) {
      _jetlist = new TEveElementList();
    }

    for (int i = 0; i < _jets.size(); i++) {
      _jetlist->AddElement(_jets[i]);
    }
    gEve->AddElement(_jetlist);
  }

}

void Event::UpdateAxis() {

  //if (_axislist->HasChildren()) { // clear daughter TEveElements
  //  _axislist->DestroyElements();
  //}

  if (_vis_axis->IsOn()) { 
    if (!_axislist) {
      _axislist = new TEveElementList();
    }

    _xaxis = new TEveArrow(1.,0.,0.,0.,0.,0.);
    _xlabel = new TEveText("x");
    _xlabel->SetMainColor(kWhite);
    _xlabel->SetFontSize(26);;
    _xlabel->PtrMainTrans()->SetPos(1.1,0.,0.);
    _xaxis->SetMainColor(kWhite);

    _yaxis = new TEveArrow(0.,1.,0.,0.,0.,0.);
    _ylabel = new TEveText("y");
    _ylabel->SetMainColor(kWhite);
    _ylabel->SetFontSize(26);;
    _ylabel->PtrMainTrans()->SetPos(0.,1.1,0.);
    _yaxis->SetMainColor(kWhite);

    _zaxis = new TEveArrow(0.,0.,1.,0,0,0);
    _zlabel = new TEveText("z");
    _zlabel->SetMainColor(kWhite);
    _zlabel->SetFontSize(26);;
    _zlabel->PtrMainTrans()->SetPos(0.,0.,1.1);
    _zaxis->SetMainColor(kWhite);

    if (_axislist->HasChildren()==0) { 
      _axislist->AddElement(_xaxis);
      _axislist->AddElement(_xlabel);
      _axislist->AddElement(_yaxis);
      _axislist->AddElement(_ylabel);
      _axislist->AddElement(_zaxis);
      _axislist->AddElement(_zlabel);
      gEve->AddElement(_axislist);
    }
  } 
}

void Event::Draw() {
  std::cerr << "Draw called" << std::endl;
#if 0 // debug
  TEveEventManager* evt = gEve->GetCurrentEvent();
  std::cerr << "Num Element = " << evt->NumChildren() << std::endl;
#endif
  gEve->Redraw3D(); 
  //gEve->FullRedraw3D(); 
  //gEve->Redraw3D(kTRUE); // This changes camera setting (rotation center)

  if (_view_type_pre!=_view_type) {
    TGLViewer* glviewer = gEve->GetDefaultGLViewer();
    if (_view_type==1) { // xy
      glviewer->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    }
    if (_view_type==2) { // yz
      glviewer->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
    }
    TGLOrthoCamera& cam = (TGLOrthoCamera&) glviewer->CurrentCamera(); 
    cam.SetEnableRotate(kTRUE); 
    
    _view_type_pre = _view_type;
  
  }
} 
