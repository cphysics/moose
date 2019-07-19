//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FluidProperties.h"

class SinglePhaseFluidProperties;

template <>
InputParameters validParams<SinglePhaseFluidProperties>();

#define propfunc(want, prop1, prop2)                                                               \
  virtual Real want##_from_##prop1##_##prop2(Real, Real) const                                     \
  {                                                                                                \
    mooseError(name(), ": ", __PRETTY_FUNCTION__, " not implemented.");                            \
  }                                                                                                \
                                                                                                   \
  virtual void want##_from_##prop1##_##prop2(                                                      \
      Real prop1, Real prop2, Real & val, Real & d##want##d1, Real & d##want##d2) const            \
  {                                                                                                \
    fluidPropError(name(), ": ", __PRETTY_FUNCTION__, " derivatives not implemented.");            \
    d##want##d1 = 0;                                                                               \
    d##want##d2 = 0;                                                                               \
    val = want##_from_##prop1##_##prop2(prop1, prop2);                                             \
  }                                                                                                \
                                                                                                   \
  virtual void want##_from_##prop1##_##prop2(const DualReal & prop1,                               \
                                             const DualReal & prop2,                               \
                                             DualReal & val,                                       \
                                             DualReal & d##want##d1,                               \
                                             DualReal & d##want##d2) const                         \
  {                                                                                                \
    fluidPropError(name(), ": ", __PRETTY_FUNCTION__, " derivative derivatives not implemented."); \
    Real dummy, tmp1, tmp2;                                                                        \
    val = want##_from_##prop1##_##prop2(prop1, prop2);                                             \
    want##_from_##prop1##_##prop2(prop1.value(), prop2.value(), dummy, tmp1, tmp2);                \
    d##want##d1 = tmp1;                                                                            \
    d##want##d2 = tmp2;                                                                            \
  }                                                                                                \
                                                                                                   \
  DualReal want##_from_##prop1##_##prop2(const DualReal & p1, const DualReal & p2) const           \
  {                                                                                                \
    Real x = 0;                                                                                    \
    Real raw1 = p1.value();                                                                        \
    Real raw2 = p2.value();                                                                        \
    Real dxd1 = 0;                                                                                 \
    Real dxd2 = 0;                                                                                 \
    want##_from_##prop1##_##prop2(raw1, raw2, x, dxd1, dxd2);                                      \
                                                                                                   \
    DualReal result = x;                                                                           \
    for (size_t i = 0; i < p1.derivatives().size(); i++)                                           \
      result.derivatives()[i] = p1.derivatives()[i] * dxd1 + p2.derivatives()[i] * dxd2;           \
    return result;                                                                                 \
  }

/**
 * Common class for single phase fluid properties
 */
class SinglePhaseFluidProperties : public FluidProperties
{
public:
  SinglePhaseFluidProperties(const InputParameters & parameters);
  virtual ~SinglePhaseFluidProperties();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
  // clang-format off

  /**
   * @brief Compute a fluid property given for the state defined by two given properties.
   *
   * For all functions, the first two arguments are the given properties that define the fluid
   * state.  For the two-argument variants, the desired property is the return value.
   * The five-argument variants also provide partial derivatives dx/da and dx/db where x is the desired property being
   * computed, a is the first given property, and b is the second given property.  The desired
   * property, dx/da, and dx/db are stored into the 3rd, 4th, and 5th arguments respectively.
   *
   * Properties/parameters used in these function are listed below with their units:
   *
   * @begincode
   * p      pressure [Pa]
   * T      temperature [K]
   * e      specific internal energy [J/kg]
   * v      specific volume [m^3/kg]
   * rho    density [kg/m^3]
   * h      specific enthalpy [J/kg]
   * s      specific entropy [J/(kg*K)]
   * mu     viscosity [Pa*s]
   * k      thermal conductivity [W/(m*K)]
   * c      speed of sound [m/s]
   * cp     constant-pressure specific heat [J/K]
   * cv     constant-volume specific heat [J/K]
   * beta   volumetric thermal expansion coefficient [1/K]
   * g      Gibbs free energy [J]
   * pp_sat partial pressure at saturation [Pa]
   * @endcode
   *
   * As an example:
   *
   * @begincode
   * // calculate pressure given specific vol and energy:
   * auto pressure = your_fluid_properties_object.p_from_v_e(specific_vol, specific_energy);
   *
   * // or use the derivative variant:
   * Real dp_dv = 0; // derivative will be stored into here
   * Real dp_de = 0; // derivative will be stored into here
   * your_fluid_properties_object.p_from_v_e(specific_vol, specific_energy, pressure, dp_dv, dp_de);
   * @endcode
   *
   * Automatic differentiation (AD) support is provided through x_from_a_b(DualReal a, DualReal b) versions
   * of the functions where a and b must be ADReal/DualNumber's calculated using all AD-supporting
   * values:
   *
   * @begincode
   * auto v = 1/rho; // rho must be an AD non-linear variable.
   * auto e = rhoE/rho - vel_energy; // rhoE and vel_energy must be AD variables/numbers also.
   * auto pressure = your_fluid_properties_object.p_from_v_e(v, e);
   * // pressure now contains partial derivatives w.r.t. all degrees of freedom
   * @endcode
   */
  ///@{
  propfunc(p, v, e)
  propfunc(T, v, e)
  propfunc(c, v, e)
  propfunc(cp, v, e)
  propfunc(cv, v, e)
  propfunc(mu, v, e)
  propfunc(k, v, e)
  propfunc(s, v, e)
  propfunc(s, h, p)
  propfunc(T, h, p)
  propfunc(rho, p, s)
  propfunc(e, v, h)
  propfunc(s, p, T)
  propfunc(pp_sat, p, T)
  propfunc(mu, rho, T)
  propfunc(k, rho, T)
  propfunc(c, p, T)
  propfunc(cp, p, T)
  propfunc(cv, p, T)
  propfunc(mu, p, T)
  propfunc(k, p, T)
  propfunc(rho, p, T)
  propfunc(e, p, rho)
  propfunc(e, T, v)
  propfunc(p, T, v)
  propfunc(h, T, v)
  propfunc(s, T, v)
  propfunc(cv, T, v)
  propfunc(h, p, T)
  propfunc(p, h, s)
  propfunc(g, v, e)
  ///@}

  // clang-format on

#undef propfunc

      /**
       * Fluid name
       * @return string representing fluid name
       */
      virtual std::string fluidName() const;

  /**
   * Molar mass [kg/mol]
   * @return molar mass
   */
  virtual Real molarMass() const;

  /**
   * Critical pressure
   * @return critical pressure (Pa)
   */
  virtual Real criticalPressure() const;

  /**
   * Critical temperature
   * @return critical temperature (K)
   */
  virtual Real criticalTemperature() const;

  /**
   * Critical density
   * @return critical density (kg/m^3)
   */
  virtual Real criticalDensity() const;

  /**
   * Critical specific internal energy
   * @return specific internal energy (J/kg)
   */
  virtual Real criticalInternalEnergy() const;

  /**
   * Triple point pressure
   * @return triple point pressure (Pa)
   */
  virtual Real triplePointPressure() const;

  /**
   * Triple point temperature
   * @return triple point temperature (K)
   */
  virtual Real triplePointTemperature() const;

  /**
   * Methods with default implementations.
   * The following methods have default implementations, so cannot
   * be implemented using the propfunc macro.
   */

  /**
   * Volumetric thermal expansion coefficient
   *
   * @param p   pressure (Pa)
   * @param T   temperature (K)
   * @return    volumetric thermal expansion coefficient (1/K)
   */
  virtual Real beta_from_p_T(Real p, Real T) const;

  /**
   * Volumetric thermal expansion coefficient
   *
   * @param p                pressure (Pa)
   * @param T                temperature (K)
   * @param[out] beta        volumetric thermal expansion coefficient (1/K)
   * @param[out] dbeta_dp    derivative of beta w.r.t. pressure
   * @param[out] dbeta_dT    derivative of beta w.r.t. temperature
   */

  virtual void beta_from_p_T(Real p, Real T, Real & beta, Real & dbeta_dp, Real & dbeta_dT) const;
  DualReal beta_from_p_T(const DualReal & p, const DualReal & T) const;
  /**
   * Specific volume from pressure and temperature
   *
   * @param[in] p   pressure
   * @param[in] T   temperature
   */
  virtual Real v_from_p_T(Real p, Real T) const;
  virtual DualReal v_from_p_T(const DualReal & p, const DualReal & T) const;

  /**
   * Specific volume and its derivatives from pressure and temperature
   *
   * @param[in] p          pressure
   * @param[in] T          temperature
   * @param[out] v         specific volume
   * @param[out] dv_dp     derivative of specific volume w.r.t. pressure
   * @param[out] dv_dT     derivative of specific volume w.r.t. temperature
   */
  virtual void v_from_p_T(Real p, Real T, Real & v, Real & dv_dp, Real & dv_dT) const;

  /**
   * Specific internal energy from temperature and specific volume
   *
   * @param[in] T     temperature
   * @param[in] v     specific volume
   */
  virtual Real e_spndl_from_v(Real v) const;

  /**
   * Specific internal energy from temperature and specific volume
   *
   * @param[in] T     temperature
   * @param[in] v     specific volume
   */
  virtual void v_e_spndl_from_T(Real T, Real & v, Real & e) const;

  /**
   * Internal energy from pressure and temperature
   *
   * @param[in] p   pressure (Pa)
   * @param[in] T   temperature (K)
   * @return internal energy (J/kg)
   */
  virtual Real e_from_p_T(Real p, Real T) const;
  DualReal e_from_p_T(const DualReal & p, const DualReal & T) const;

  /**
   * Internal energy and its derivatives from pressure and temperature
   *
   * @param[in] p        pressure (Pa)
   * @param[in] T        temperature (K)
   * @param[out] e       internal energy (J/kg)
   * @param[out] de_dp   derivative of internal energy w.r.t. pressure
   * @param[out] de_dT   derivative of internal energy w.r.t. temperature
   */
  virtual void e_from_p_T(Real p, Real T, Real & e, Real & de_dp, Real & de_dT) const;

  /**
   * Temperature from pressure and specific enthalpy
   *
   * @param[in] p pressure (Pa)
   * @param[in] h enthalpy (J/kg)
   * @return temperature (K)
   */
  virtual Real T_from_p_h(Real p, Real h) const;
  virtual void T_from_p_h(Real p, Real h, Real & T, Real & dT_dp, Real & dT_dh) const;
  DualReal T_from_p_h(const DualReal & p, const DualReal & h) const;

  /**
   * Adiabatic index - ratio of specific heats
   * @param v specific volume
   * @param e specific internal energy
   * @return gamma (-)
   */
  virtual Real gamma_from_v_e(Real v, Real e) const;

  /**
   * Adiabatic index - ratio of specific heats
   * @param p fluid pressure (Pa)
   * @param T fluid temperature (K)
   * @return gamma (-)
   */
  virtual Real gamma_from_p_T(Real p, Real T) const;

  /**
   * Henry's law constant for dissolution in water and derivative wrt temperature
   * @param T fluid temperature (K)
   * @param[out] Kh Henry's constant
   * @param[out] dKh_dT derivative of Kh wrt temperature
   */
  virtual Real henryConstant(Real T) const;
  virtual void henryConstant(Real T, Real & Kh, Real & dKh_dT) const;

  /**
   * Vapor pressure. Used to delineate liquid and gas phases.
   * Valid for temperatures between the triple point temperature
   * and the critical temperature
   *
   * @param T fluid temperature (K)
   * @param[out] saturation pressure (Pa)
   * @param[out] derivative of saturation pressure wrt temperature (Pa/K)
   */
  virtual Real vaporPressure(Real T) const;
  virtual void vaporPressure(Real T, Real & psat, Real & dpsat_dT) const;
  DualReal vaporPressure(const DualReal & temperature) const;

  /**
   * Vapor temperature. Used to delineate liquid and gas phases.
   * Valid for pressures between the triple point pressure
   * and the critical pressure
   *
   * @param p fluid pressure (Pa)
   * @param[out] saturation temperature (K)
   * @param[out] derivative of saturation temperature wrt pressure
   */
  virtual Real vaporTemperature(Real p) const;
  virtual void vaporTemperature(Real p, Real & Tsat, Real & dTsat_dp) const;
  DualReal vaporTemperature(const DualReal & pressure) const;

  /**
   * Combined methods. These methods are particularly useful for the PorousFlow
   * module, where density and viscosity are typically both computed everywhere.
   * The combined methods allow the most efficient means of calculating both
   * properties, especially where rho(p, T) and mu(rho, T). In this case, an
   * extra density calculation would be required to calculate mu(p, T). All
   * propery names are described above.
   */
  virtual void rho_mu_from_p_T(Real p, Real T, Real & rho, Real & mu) const;
  virtual void rho_mu_from_p_T(Real p,
                               Real T,
                               Real & rho,
                               Real & drho_dp,
                               Real & drho_dT,
                               Real & mu,
                               Real & dmu_dp,
                               Real & dmu_dT) const;

  virtual void rho_e_from_p_T(Real p,
                              Real T,
                              Real & rho,
                              Real & drho_dp,
                              Real & drho_dT,
                              Real & e,
                              Real & de_dp,
                              Real & de_dT) const;

protected:
  /**
   * IAPWS formulation of Henry's law constant for dissolution in water
   * From Guidelines on the Henry's constant and vapour
   * liquid distribution constant for gases in H20 and D20 at high
   * temperatures, IAPWS (2004)
   */
  virtual Real henryConstantIAPWS(Real T, Real A, Real B, Real C) const;
  virtual void henryConstantIAPWS(Real T, Real & Kh, Real & dKh_dT, Real A, Real B, Real C) const;

private:
  template <typename... Args>
  void fluidPropError(Args... args) const
  {
    if (_allow_imperfect_jacobians)
      mooseDoOnce(mooseWarning(std::forward<Args>(args)...));
    else
      mooseError(std::forward<Args>(args)...);
  }
};

#pragma GCC diagnostic pop
