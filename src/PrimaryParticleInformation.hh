/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _PrimaryParticleInformation_h_
#define _PrimaryParticleInformation_h_

#include "G4VUserPrimaryParticleInformation.hh"

namespace G4me {

class PrimaryParticleInformation : public G4VUserPrimaryParticleInformation
{
  
public:
  
  PrimaryParticleInformation();
  ~PrimaryParticleInformation() override;
  
  void Print() const override {};

  int GetIndex() const { return mIndex; };
  void SetIndex(int val) { mIndex = val; };
  
protected:

  int mIndex;
  
};

} /** namespace G4me **/
  
#endif /** _PrimaryParticleInformation_h_ **/
