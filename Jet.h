#ifndef __JET_H__
#define __JET_H__
#include "TEveJetCone.h"
#include <EVENT/MCParticle.h>
class Jet : public TEveJetCone
{                   
  public :          
    Jet();
    //TEveJetCone* GetJet();
    void SetLineColor(int in) { _linecolor = in; }
    void SetLineWidth(int in) { _linewidth = in; }
    void Dump() const;
    double _px; 
    double _py; 
    double _pz; 
    double _e; 
    double _phi; 
    double _eta; 
    double _coneR; 
    std::vector<EVENT::MCParticle*> _trkmcps;

  private :
    void FillAncestorPDGs(EVENT::MCParticle* mcp, std::vector<int>& pdgs) const;
    int    _linecolor;
    int    _linewidth;
    //TEveJetCone* _jetcone; // This will be passed to gEve.
    
  ClassDef(Jet,1) 
};
#endif
