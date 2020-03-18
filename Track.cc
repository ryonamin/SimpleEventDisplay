#include "Track.h"
#include <iostream>

ClassImp(Track)

//Track::Track(double px, double py, double pz, double e, double vx, double vy, double vz, double t0, int pdg, TEveTrackPropagator* prop) : 
Track::Track(double px, double py, double pz, double e, double vx, double vy, double vz, double t0, int pdg, TrackPropagatorParams& params) : 
            TParticle(pdg,0,0,0,0,0,px,py,pz,e,vx,vy,vz,t0),
            _mcp_pdg(0),
            _mcparent_pdg(0)
{
#if 0
  _evtrk = new TEveTrack(this,0,prop);
#else
  TEveTrackPropagator* aprop = new TEveTrackPropagator;
  aprop->SetMagField(params.bz);
  aprop->SetMaxR(params.maxR);
  aprop->SetMaxZ(params.maxZ);
  aprop->SetMaxOrbs(params.maxOrbs);
  _evtrk = new TEveTrack(this,0,aprop);
#endif
  _evtrk->MakeTrack();
  _evtrk->SetSourceObject(this);
  _evtrk->SetMainColor(kGreen);
  _evtrk->SetLineWidth(2);
}

void Track::Dump() const {
  std::cerr << "Track info :" << std::endl;
  std::cerr << "   MCParticle Index = " << _mcp_index << std::endl;
  std::cerr << "   MCParticle PDG = " << _mcp_pdg << std::endl;
  std::cerr << "       Parent PDG = " << _mcparent_pdg << std::endl;
  std::cerr << "   chi2 = " << _chi2 << std::endl;
  std::cerr << "   ndf = " << _ndf << std::endl;
  std::cerr << "   d0 = " << _d0 << std::endl;
  std::cerr << "   d0err = " << _d0err << std::endl;
  std::cerr << "   z0 = " << _z0 << std::endl;
  std::cerr << "   z0err = " << _z0err << std::endl;
  std::cerr << "   rmin hit = " << _rmin_hit << std::endl;
  std::cerr << "   reference point = (" << _ref_x << ", " << _ref_y << ", " << _ref_z << ")" << std::endl;
  std::cerr << "   track energy = " << Energy() << std::endl;
}

