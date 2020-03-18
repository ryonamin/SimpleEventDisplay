#ifndef __MCPARTICLE_H__
#define __MCPARTICLE_H__

#include "TEveArrow.h"

class MCParticle : public TEveArrow
{                   
  public :          
    MCParticle(float xv, float yv, float zv, float xo, float yo, float zo);
    //~MCParticle(){};
    //TEveArrow* GetMCParticle();
    void Dump() const;
    double _px; 
    double _py; 
    double _pz; 
    double _e; 
    float _startx; // start vertex 
    float _starty; 
    float _startz; 
    float _endx; // end vertex 
    float _endy; 
    float _endz; 
    int    _index;
    int    _pdg;
    int    _parentpdg;

  private :
    //MCParticle& operator=(const MCParticle& in) { return *this; }
    //MCParticle(const MCParticle& in ) { }
    //TEveArrow* _mcp;
    int    _linecolor;
    int    _linewidth;
    
  ClassDef(MCParticle,1) 
};
#endif
