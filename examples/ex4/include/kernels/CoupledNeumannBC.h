#include "BoundaryCondition.h"

#ifndef COUPLEDNEUMANNBC_H
#define COUPLEDNEUMANNBC_H

//Forward Declarations
class CoupledNeumannBC;

template<>
Parameters valid_params<CoupledNeumannBC>();

/**
 * Implements a simple constant Neumann BC where grad(u)=value on the boundary.
 * Uses the term produced from integrating the diffusion operator by parts.
 */
class CoupledNeumannBC : public BoundaryCondition
{
public:

  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  CoupledNeumannBC(std::string name,
                   Parameters parameters,
                   std::string var_name,
                   unsigned int boundary_id,
                   std::vector<std::string> coupled_to,
                   std::vector<std::string> coupled_as);
    
  virtual ~CoupledNeumannBC(){}

protected:
  virtual Real computeQpResidual();

private:
  /**
   * Multiplier on the boundary.
   */
  Real _value;

  /**
   * Holds the values at the quadrature points
   * of a coupled variable.
   */
  std::vector<Real> & _some_var_val;
};

#endif //COUPLEDNEUMANNBC_H
