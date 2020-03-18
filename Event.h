#ifndef __EVENT_H__
#define __EVENT_H__
#include "TObject.h"

// iLCSoft 
#include "lcio.h"
#include "IO/LCReader.h"
#include "IOIMPL/LCFactory.h"
#include "IMPL/LCTOOLS.h"

// ROOT
#include "TEveManager.h"
#include "TGButton.h"
#include "TGComboBox.h"

#include "MCParticle.h"
#include "Track.h"
#include "Jet.h"
#include "TrackPropagatorParams.h"

class TGNumberEntryField;
class TGFileInfo;
class TGFileDialog;
class TEvePad;
class TEveText;

class Event : public TObject
{
  public :
	Event(); 
	void OpenFile();
    void SetNevtField(TGNumberEntryField* in);
    void SetBField(TGNumberEntryField* in);
    void SetRmaxField(TGNumberEntryField* in);
    void SetZmaxField(TGNumberEntryField* in);
    void SetMCParticleVisState(TGCheckButton* in);
    void SetTrackVisState(TGCheckButton* in);
    void SetPVtxTrackVisState(TGCheckButton* in);
    void SetSVtxTrackVisState(TGCheckButton* in);
    void SetJetVisState(TGCheckButton* in);
    void SetAxisVisState(TGCheckButton* in);
    void SetMCParticleColList(TGComboBox* in);
    void SetRelColList(TGComboBox* in);
    void SetTrackColList(TGComboBox* in);
    void SetPVtxTrackColList(TGComboBox* in);
    void SetSVtxTrackColList(TGComboBox* in);
    void SetJetColList(TGComboBox* in);
    void UpdatePropagator();
	void Update();
	void Next();
	void Prev();
	void Goto();
	void ViewSet(Int_t id);
	void DumpMCParticles();


  private : 

    void SetPropagator(TEveTrackList* in);
	int _nevt; 
	int _ev; 
	int _view_type; // 1 : xy, 2: yz (See RadioButton:rb_views)
	int _view_type_pre; // setting at previous event
    TGNumberEntryField* _nevt_field;
    TGNumberEntryField* _b_field;
    TGNumberEntryField* _rmax_field;
    TGNumberEntryField* _zmax_field;
    TGCheckButton* _vis_mcps;
    TGCheckButton* _vis_tracks;
    TGCheckButton* _vis_pvtxtracks;
    TGCheckButton* _vis_svtxtracks;
    TGCheckButton* _vis_jets;
    TGCheckButton* _vis_axis;
    TGComboBox* _col_mcps;
    TGComboBox* _col_tracks;
    TGComboBox* _col_rel;
    TGComboBox* _col_pvtxtracks;
    TGComboBox* _col_svtxtracks;
    TGComboBox* _col_jets;
	TGFileDialog* _fdialog;
	TGFileInfo* _fileinfo;

    TEveElementList *_mcplist;
#if 0
    TEveTrackPropagator* _prop;
#else
    TrackPropagatorParams _prop;
#endif
    TEveTrackList *_trklist;
    TEveTrackList *_pvtxtrklist;
    TEveTrackList *_svtxtrklist;
    TEveElementList *_jetlist;
    TEveElementList *_axislist;

    std::string GetCollectionNameByEntryId(int id); 

    std::set<std::string> _collist;
    std::vector<MCParticle*> _mcps;
    std::vector<Track*> _pvtxtracks;
    std::vector<Track*> _svtxtracks;
    std::vector<Track*> _tracks;
    std::vector<Jet*>      _jets;

    void ProcessMCParticles(EVENT::LCEvent* evt);
    void ProcessTracks(EVENT::LCEvent* evt);
    void ProcessPVtxTracks(EVENT::LCEvent* evt);
    void ProcessSVtxTracks(EVENT::LCEvent* evt);
    void ProcessJets(EVENT::LCEvent* evt);

    void UpdateMCParticles();
    void UpdatePVtxTracks();
    void UpdateSVtxTracks();
    void UpdateTracks();
    void UpdateJets();
    void UpdateAxis();
    void Draw();

    EVENT::LCEvent* GetEvent(int ev);

    std::vector<int> _Blist;
// = { 511, 521, 531, 541, 5122, 5132, 5232, 5332};
    std::vector<int> _Clist; 
//= { 411, 421, 431, 4122, 4132, 4232, 4332};

    std::map<EVENT::MCParticle*,int> _mcpIndex;

    lcio::LCReader* _lcReader;
    EVENT::IntVec _runAndEvts;

    TEveArrow* _xaxis;
    TEveText*  _xlabel;
    TEveArrow* _yaxis;
    TEveText*  _ylabel;
    TEveArrow* _zaxis;
    TEveText*  _zlabel;
  ClassDef(Event,1)

};

#endif

