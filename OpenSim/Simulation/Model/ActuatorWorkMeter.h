#ifndef __ActuatorWorkMeter_h__
#define __ActuatorWorkMeter_h__

// ActuatorWorkMeter.h
// Author: Ajay Seth
/*
 * Copyright (c)  2011, Stanford University. All rights reserved. 
* Use of the OpenSim software in source form is permitted provided that the following
* conditions are met:
* 	1. The software is used only for non-commercial research and education. It may not
*     be used in relation to any commercial activity.
* 	2. The software is not distributed or redistributed.  Software distribution is allowed 
*     only through https://simtk.org/home/opensim.
* 	3. Use of the OpenSim software or derivatives must be acknowledged in all publications,
*      presentations, or documents describing work in which OpenSim or derivatives are used.
* 	4. Credits to developers may not be removed from executables
*     created from modifications of the source.
* 	5. Modifications of source code must retain the above copyright notice, this list of
*     conditions and the following disclaimer. 
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
*  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
*  SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
*  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR BUSINESS INTERRUPTION) OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
*  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


// INCLUDE
#include <OpenSim/Simulation/Model/Actuator.h>
#include <OpenSim/Simulation/Model/ModelComponent.h>

namespace OpenSim {

class Model;

//==============================================================================
//==============================================================================
/**
 * ActuatorWorkMeter is a ModelComponent for computing the Work generated(+)
 * or dissipated by an Actuator.
 * Specific WorkMeters for specific actuators like muscles can be derived from 
 * this class. 
 *
 * @author Ajay Seth
 * @version 1.0
 */
class OSIMSIMULATION_API ActuatorWorkMeter : public ModelComponent  
{
//=============================================================================
// DATA
//=============================================================================

protected:

	Actuator* _actuator;

//=============================================================================
// METHODS
//=============================================================================
//--------------------------------------------------------------------------
// CONSTRUCTION
//--------------------------------------------------------------------------
public:
	// default 
	ActuatorWorkMeter();
	// convenience
	ActuatorWorkMeter(const Actuator &actuator, double initialWork=0);
	// copy
	ActuatorWorkMeter(const ActuatorWorkMeter &aActuatorWorkMeter);
	virtual ~ActuatorWorkMeter();
	virtual Object* copy() const;

#ifndef SWIG
	ActuatorWorkMeter& operator=(const ActuatorWorkMeter &aActuatorWorkMeter);
#endif

	OPENSIM_DECLARE_DERIVED(ActuatorWorkMeter, Object);

	virtual double getWork(const SimTK::State& state) const;

protected:
	// Model component interface
	virtual void setup(Model& aModel);
	virtual void createSystem(SimTK::MultibodySystem& system) const;
	virtual void initState(SimTK::State& state) const;
    virtual void setDefaultsFromState(const SimTK::State& state);
	virtual int getNumStateVariables() const { return 1; };
	
	SimTK::Vector ActuatorWorkMeter::computeStateVariableDerivatives(const SimTK::State& s) const;

private:
	void setNull();
	void setupProperties();

//=============================================================================
};	// END of class ActuatorWorkMeter
//=============================================================================
//=============================================================================

} // end of namespace OpenSim

#endif // __ActuatorWorkMeter_h__

