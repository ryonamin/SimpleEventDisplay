#include "MCParticle.h"
#include <iostream>

ClassImp(MCParticle)

MCParticle::MCParticle(float xv, float yv, float zv, float xo, float yo, float zo) : 
                        TEveArrow(xv,yv,zv,xo,yo,zo), 
                        _px(0),
                        _py(0), 
                        _pz(0),
                        _e(0),
                        _startx(xo),
                        _starty(yo),
                        _startz(zo),
                        _endx(xv+xo),
                        _endy(yv+yo),
                        _endz(zv+zo),
                        _index(0),
                        _pdg(0),
                        _parentpdg(0),
                        _linecolor(1),
                        _linewidth(1)
{
  SetSourceObject(this);
}

void MCParticle::Dump() const
{
  //std::cerr << "line width : " << GetTubeR() << std::endl;
  //TEveVector origin = GetOrigin();
  //TEveVector vector = GetVector();
  std::cerr << "Dump : " << _pdg << std::endl;
  std::cerr << "       index      : " << _index << std::endl;
  std::cerr << "       pdg        : " << _pdg << std::endl;
  std::cerr << "       parent pdg : " << _parentpdg << std::endl;
  std::cerr << "       (e,px,py,pz) : (" << _e << ", " << _px << ", " << _py << ", " << _pz << ")" << std::endl;
  std::cerr << "       start pos     : (" << _startx << ", " << _starty << ", " << _startz << ")" << std::endl;
  std::cerr << "       end pos       : (" << _endx << ", " << _endy << ", " << _endz << ")" << std::endl;
}
