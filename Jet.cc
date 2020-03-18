#include "Jet.h"

ClassImp(Jet)

Jet::Jet() : TEveJetCone("JetCone"),
                        _px(0),
                        _py(0), 
                        _pz(0),
                        _e(0),
                        _phi(0),
                        _eta(0),
                        _coneR(0),
                        _linecolor(1),
                        _linewidth(1)
                        //_jetcone(0)
{
  SetSourceObject(this);
}

void Jet::Dump() const
{
  std::cerr << "Dump : " << std::endl;
  std::cerr << "       px : " << _px << std::endl;
  std::cerr << "       py : " << _py << std::endl;
  std::cerr << "       pz : " << _pz << std::endl;
  for (int itrk = 0; itrk < _trkmcps.size(); itrk++) {
    std::cerr << "       track " << itrk << " pdg : " << _trkmcps[itrk]->getPDG();
    std::vector<int> pdgs;
    FillAncestorPDGs(_trkmcps[itrk],pdgs);
    for (int ip = 0; ip < pdgs.size(); ip++) {
      std::cerr << " " << pdgs[ip];
    }
    std::cerr << std::endl;
  }
}

// indexもつけられるようにする。　
void Jet::FillAncestorPDGs(EVENT::MCParticle* mcp, std::vector<int>& pdgs) const
{
  if (mcp->getParents().size()>0) {
    EVENT::MCParticle* parent = mcp->getParents()[0];
    pdgs.push_back(parent->getPDG()); 
    FillAncestorPDGs(parent,pdgs);
  }
}

