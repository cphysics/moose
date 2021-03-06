//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SideFluxIntegral.h"

#include "metaphysicl/raw_type.h"

registerMooseObject("MooseApp", SideFluxIntegral);
registerMooseObject("MooseApp", ADSideFluxIntegral);

defineLegacyParams(SideFluxIntegral);

template <bool is_ad>
InputParameters
SideFluxIntegralTempl<is_ad>::validParams()
{
  InputParameters params = SideIntegralVariablePostprocessor::validParams();
  params.addRequiredParam<MaterialPropertyName>(
      "diffusivity",
      "The name of the diffusivity material property that will be used in the flux computation.");
  params.addClassDescription("Computes the integral of the flux over the specified boundary");
  return params;
}

template <bool is_ad>
SideFluxIntegralTempl<is_ad>::SideFluxIntegralTempl(const InputParameters & parameters)
  : SideIntegralVariablePostprocessor(parameters),
    _diffusivity(parameters.get<MaterialPropertyName>("diffusivity")),
    _diffusion_coef(getGenericMaterialProperty<Real, is_ad>(_diffusivity))
{
}

template <bool is_ad>
Real
SideFluxIntegralTempl<is_ad>::computeQpIntegral()
{
  return -MetaPhysicL::raw_value(_diffusion_coef[_qp]) * _grad_u[_qp] * _normals[_qp];
}

template class SideFluxIntegralTempl<false>;
template class SideFluxIntegralTempl<true>;
