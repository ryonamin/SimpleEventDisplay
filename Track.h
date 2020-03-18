#ifndef __TRACK_H__
#define __TRACK_H__

#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TrackPropagatorParams.h"

class Track : public TParticle
{                   
  public :          
    //Track(double px, double py, double pz, double e, double vx, double vy, double vz, double t0, int pdg, TEveTrackPropagator* prop);
    Track(double px, double py, double pz, double e, double vx, double vy, double vz, double t0, int pdg, TrackPropagatorParams& params);
    ~Track() { _evtrk = 0; }
    TEveTrack* GetTEveTrack() { return _evtrk; }
    void SetLineColor(Color_t in) { _evtrk->SetMainColor(in); }
    void SetLineWidth(Width_t in) { _evtrk->SetLineWidth(in); }
    virtual void Dump() const;
    int _mcp_pdg;
    int _mcparent_pdg;
    int _mcp_index;
    float _chi2;
    int _ndf;
    float _rmin_hit;
    float _d0;
    float _z0;
    float _d0err;
    float _z0err;
    float _ref_x;
    float _ref_y;
    float _ref_z;
  private :
    
    TEveTrack* _evtrk; // This will be passed to gEve.

  ClassDef(Track,1) 
};
#endif
