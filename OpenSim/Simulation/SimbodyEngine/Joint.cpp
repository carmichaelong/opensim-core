/* -------------------------------------------------------------------------- *
 *                            OpenSim:  Joint.cpp                             *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2005-2014 Stanford University and the Authors                *
 * Author(s): Ajay Seth, Frank C. Anderson                                                       *
 *                                                                            *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may    *
 * not use this file except in compliance with the License. You may obtain a  *
 * copy of the License at http://www.apache.org/licenses/LICENSE-2.0.         *
 *                                                                            *
 * Unless required by applicable law or agreed to in writing, software        *
 * distributed under the License is distributed on an "AS IS" BASIS,          *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 * See the License for the specific language governing permissions and        *
 * limitations under the License.                                             *
 * -------------------------------------------------------------------------- */

//=============================================================================
// INCLUDES
//=============================================================================
#include "Joint.h"
#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/PhysicalFrame.h>
#include <OpenSim/Simulation/Model/PhysicalOffsetFrame.h>
#include <OpenSim/Common/ScaleSet.h>

//=============================================================================
// STATICS
//=============================================================================
using namespace std;
using namespace SimTK;
using namespace OpenSim;

//=============================================================================
// CONSTRUCTOR(S) AND DESTRUCTOR
//=============================================================================
//_____________________________________________________________________________
/**
 * Destructor.
 */
Joint::~Joint()
{
}
//_____________________________________________________________________________
/**
 * Default constructor.
 */
Joint::Joint() : Super()
{
    setNull();
    constructInfrastructure();
}

/* API constructor. */
Joint::Joint(const std::string &name, const std::string& parentName, 
                                      const std::string& childName,
                                      bool reverse) : Joint()
{
    setName(name);
    set_reverse(reverse);

    updConnector<PhysicalFrame>("parent_frame").set_connectee_name(parentName);
    updConnector<PhysicalFrame>("child_frame").set_connectee_name(childName);
}

/* Convenience Constructor*/
Joint::Joint(const std::string &name,
    const PhysicalFrame& parent,
    const SimTK::Vec3& locationInParent,
    const SimTK::Vec3& orientationInParent,
    const PhysicalFrame& child,
    const SimTK::Vec3& locationInChild,
    const SimTK::Vec3& orientationInChild,
    bool reverse)
    // TODO. prefixing the joint name to the frame names should not be necessary.
    // This is only required now because the search does not respect the local
    // (relative) name, which it should find immediately, and instead is searching
    // from the root of the tree.
    : Joint(name, parent.getName() + "_offset", child.getName() + "_offset")
{
    // PARENT TRANSFORM
    Rotation parentRotation(BodyRotationSequence,
        orientationInParent[0], XAxis,
        orientationInParent[1], YAxis,
        orientationInParent[2], ZAxis);
    SimTK::Transform parentTransform(parentRotation, locationInParent);
    PhysicalOffsetFrame parentOffset(parent, parentTransform);
    parentOffset.setName(parent.getName() + "_offset");

    // CHILD TRANSFORM
    Rotation childRotation(BodyRotationSequence,
        orientationInChild[0], XAxis,
        orientationInChild[1], YAxis,
        orientationInChild[2], ZAxis);
    SimTK::Transform childTransform(childRotation, locationInChild);
    PhysicalOffsetFrame childOffset(child, childTransform);
    childOffset.setName(child.getName() + "_offset");

    // Append the offset frames to the Joints internal list of frames
    append_frames(parentOffset);
    append_frames(childOffset);
}

//=============================================================================
// CONSTRUCTION Utility
//=============================================================================
Joint::CoordinateIndex Joint::constructCoordinate(Coordinate::MotionType mt)
{
    Coordinate* coord = new Coordinate();
    coord->setMotionType(mt);
    coord->setName(getName() + "_coord_"
        + std::to_string(get_CoordinateSet().getSize()));
    // CoordinateSet takes ownership
    upd_CoordinateSet().adoptAndAppend(coord);
    return CoordinateIndex(get_CoordinateSet().getIndex(coord));
}

//_____________________________________________________________________________
/**
 * Set the data members of this Joint to their null values.
 */
void Joint::setNull()
{
    setAuthors("Ajay Seth");
}

//_____________________________________________________________________________
/**
 * Construct properties and initialize their default values.
 */
void Joint::constructProperties()
{
    // Generalized coordinates
    constructProperty_CoordinateSet(CoordinateSet());

    // Transform direction (parent->child or child->parent)
    constructProperty_reverse(false);

    //Default frames list is empty
    constructProperty_frames();
}

void Joint::constructConnectors()
{
    constructConnector<PhysicalFrame>("parent_frame");
    constructConnector<PhysicalFrame>("child_frame");
}

void Joint::setParentFrameName(const std::string& name)
{
    updConnector<PhysicalFrame>("parent_frame").set_connectee_name(name);
}
const std::string& Joint::getParentFrameName() const
{
    return getConnector<PhysicalFrame>("parent_frame").get_connectee_name();
}

void Joint::setChildFrameName(const std::string& name)
{
    updConnector<PhysicalFrame>("child_frame").set_connectee_name(name);
}
const std::string& Joint::getChildFrameName() const
{
    return getConnector<PhysicalFrame>("child_frame").get_connectee_name();
}

void Joint::extendFinalizeFromProperties()
{
    Super::extendFinalizeFromProperties();
    CoordinateSet& coords = upd_CoordinateSet();

    // add all coordinates listed under this joint 
    for (int i = 0; i < coords.getSize(); ++i) {
        coords[i].setJoint(*this);
        // Append a pointer otherwise the model will make a copy that will not
        // be updated properly
        addComponent(&coords[i]);
    }
    for (int i = 0; i < getProperty_frames().size(); ++i) {
        addComponent(&upd_frames(i));
    }
}

//=============================================================================
// GET AND SET
//=============================================================================
//-----------------------------------------------------------------------------
// CHILD BODY
//-----------------------------------------------------------------------------
//_____________________________________________________________________________
const PhysicalFrame& Joint::getChildFrame() const
{
    return getConnector<PhysicalFrame>("child_frame").getConnectee();
}

//-----------------------------------------------------------------------------
// PARENT BODY
//-----------------------------------------------------------------------------
const OpenSim::PhysicalFrame& Joint::getParentFrame() const
{
    return getConnector<PhysicalFrame>("parent_frame").getConnectee();
}

//_____________________________________________________________________________
/**
 * Check if a coordinate is used by the Joint.
 *
 * @param aCoordinate Coordinate to look for in joint.
 * @return True if the coordinate is used.
 */
bool Joint::isCoordinateUsed(const Coordinate& aCoordinate) const
{
    const CoordinateSet& coordinateSet = get_CoordinateSet();
    int i, size = coordinateSet.getSize();
    for(i=0; i<size; i++) {
        if(&coordinateSet.get(i) == &aCoordinate) return true;
    }

    return false;
}

//=============================================================================
// SCALING
//=============================================================================
//_____________________________________________________________________________

void Joint::scale(const ScaleSet& scaleSet)
{
    SimTK::Vec3 parentFactors(1.0);
    SimTK::Vec3 childFactors(1.0);

    // Find the factors associated with the PhysicalFrames this Joint connects
    const string& parentName = getParentFrame().findBaseFrame().getName();
    const string& childName = getChildFrame().findBaseFrame().getName();
    // Get scale factors
    bool found_p = false;
    bool found_b = false;
    for (int i = 0; i < scaleSet.getSize(); i++) {
        Scale& scale = scaleSet.get(i);
        if (!found_p && (scale.getSegmentName() == parentName)) {
            scale.getScaleFactors(parentFactors);
            found_p = true;
        }
        if (!found_b && (scale.getSegmentName() == childName)) {
            scale.getScaleFactors(childFactors);
            found_b = true;
        }
        if (found_p && found_b)
            break;
    }

    // if the frame is owned by this Joint scale it,
    // otherwise let the owner of the frame decide.
    int found = getProperty_frames().findIndex(getParentFrame());
    if (found >= 0) {
        PhysicalOffsetFrame& offset
            = SimTK_DYNAMIC_CAST_DEBUG<PhysicalOffsetFrame&>(upd_frames(found));
        offset.scale(parentFactors);
    }
    found = getProperty_frames().findIndex(getChildFrame());
    if (found >= 0) {
        PhysicalOffsetFrame& offset
            = SimTK_DYNAMIC_CAST_DEBUG<PhysicalOffsetFrame&>(upd_frames(found));
        offset.scale(childFactors);
    }
}

const SimTK::MobilizedBodyIndex Joint::
    getMobilizedBodyIndex(const OpenSim::Body& body) const
{
        return body.getMobilizedBodyIndex();
} 

void Joint::setChildMobilizedBodyIndex(const SimTK::MobilizedBodyIndex index) const
{ 
    getChildFrame().setMobilizedBodyIndex(index);
}


void Joint::extendAddToSystem(SimTK::MultibodySystem& system) const
{
    Super::extendAddToSystem(system);

    // The parent node in the multibody tree must part of the system
    if(get_reverse())
        // this will be the child if the joint definition is reversed
        getConnector<PhysicalFrame>("child_frame").getConnectee().addToSystem(system);
    else // otherwise it is the parent frame
        getConnector<PhysicalFrame>("parent_frame").getConnectee().addToSystem(system);
}

void Joint::extendInitStateFromProperties(SimTK::State& s) const
{
    Super::extendInitStateFromProperties(s);

    const CoordinateSet& coordinateSet = get_CoordinateSet();
    for (int i = 0; i < coordinateSet.getSize(); i++)
        coordinateSet.get(i).extendInitStateFromProperties(s);
}

void Joint::extendSetPropertiesFromState(const SimTK::State& state)
{
    Super::extendSetPropertiesFromState(state);

    const CoordinateSet& coordinateSet = get_CoordinateSet();
    for (int i = 0; i < coordinateSet.getSize(); i++)
        coordinateSet.get(i).extendSetPropertiesFromState(state);
}


//=============================================================================
// Computation
//=============================================================================
/* Calculate the equivalent spatial force, FB_G, acting on the body connected by
   this joint at its location B, expressed in ground.  */
SimTK::SpatialVec Joint::calcEquivalentSpatialForce(const SimTK::State &s, 
    const SimTK::Vector &mobilityForces) const
{
    // The number of mobilities for the entire system.
    int nm = _model->getMatterSubsystem().getNumMobilities();

    if(nm != mobilityForces.size()){
        throw Exception("Joint::calcEquivalentSpatialForce(): input mobilityForces does not match model's mobilities");
    }

    const SimTK::MobilizedBodyIndex &mbx = getChildFrame().getMobilizedBodyIndex();
    // build a unique list of underlying MobilizedBodies that are involved
    // with this Joint in addition to and not including that of the child body

    std::set<SimTK::MobilizedBodyIndex> mbds;

    const CoordinateSet& coordinateSet = get_CoordinateSet();

    for(int i=0; i<coordinateSet.getSize(); ++i){
        const MobilizedBodyIndex& coordsMbx = coordinateSet[i].getBodyIndex();
        if (coordsMbx != mbx){
            mbds.insert(coordsMbx);
        }
    }
    
    SimTK::SpatialVec FB_G = calcEquivalentSpatialForceForMobilizedBody(s, mbx, mobilityForces);
    SimTK::SpatialVec FBx_G;

    std::set<SimTK::MobilizedBodyIndex>::const_iterator it = mbds.begin();

    const SimTK::MobilizedBody &G = getModel().getMatterSubsystem().getGround();
    const SimTK::MobilizedBody &B = getModel().getMatterSubsystem().getMobilizedBody(mbx);
    SimTK::Vec3 r_BG =
        B.expressVectorInAnotherBodyFrame(s, B.getOutboardFrame(s).p(), G);

    while(it != mbds.end()){
        FBx_G = calcEquivalentSpatialForceForMobilizedBody(s, *it, mobilityForces);

        const SimTK::MobilizedBody &b = 
            getModel().getMatterSubsystem().getMobilizedBody(*it);

        
        SimTK::Vec3 r_bG = 
            b.expressVectorInAnotherBodyFrame(s, b.getOutboardFrame(s).p(), G);

        // Torques add and include term due to offset in forces
        FB_G += FBx_G; // shiftForceFromTo(FBx_G, r_bG, r_BG);
        ++it;
    }
    
    return FB_G;
}

/** Joints only produce power when internal constraint forces have components along
    the mobilities of the joint (for example to satisfy prescribed motion). In 
    which case the joint power is the constraint forces projected onto the mobilities
    multiplied by the mobilities (internal coordinate velocities). Only constraints
    internal to the joint are accounted for, not external constraints that effect
    joint motion. */
double Joint::calcPower(const SimTK::State &s) const
{
    const CoordinateSet &coords = getCoordinateSet();
    int nc = coords.getSize();

    double power = 0;
    for(int i=0; i<nc; ++i){
        if (coords[i].isPrescribed(s)){
            // get the reaction force for this coordinate prescribed motion constraint
            const SimTK::Constraint &pc = _model->updMultibodySystem().updMatterSubsystem().getConstraint(coords[i]._prescribedConstraintIndex);
            power += pc.calcPower(s);
        }
    }

    return power;
}

//=============================================================================
// Helper
//=============================================================================
/* Calculate the equivalent spatial force, FB_G, acting on a mobilized body specified 
   by index acting at its mobilizer frame B, expressed in ground.  */
SimTK::SpatialVec Joint::calcEquivalentSpatialForceForMobilizedBody(const SimTK::State &s, 
    const SimTK::MobilizedBodyIndex mbx, const SimTK::Vector &mobilityForces) const
{
    // Get the mobilized body
    const SimTK::MobilizedBody mbd    = getModel().getMatterSubsystem().getMobilizedBody(mbx);
    const SimTK::UIndex        ustart = mbd.getFirstUIndex(s);
    const int                  nu     = mbd.getNumU(s);

    const SimTK::MobilizedBody ground = getModel().getMatterSubsystem().getGround();

    if (nu == 0) // No mobility forces (weld joint?).
        return SimTK::SpatialVec(SimTK::Vec3(0), SimTK::Vec3(0));

    // Construct the H (joint Jacobian, joint transition) matrix for this mobilizer
    SimTK::Matrix transposeH_PB_w(nu, 3);
    SimTK::Matrix transposeH_PB_v(nu, 3);
    // from individual columns
    SimTK::SpatialVec Hcol;
    
    // To obtain the joint Jacobian, H_PB (H_FM in Simbody) need to be realized to at least position
    _model->getMultibodySystem().realize(s, SimTK::Stage::Position);

    SimTK::Vector f(nu, 0.0);
    for(int i =0; i<nu; ++i){
        f[i] = mobilityForces[ustart + i];
        // Get the H matrix for this Joint by constructing it from the operator H*u
        Hcol = mbd.getH_FMCol(s, SimTK::MobilizerUIndex(i));
        const SimTK::Vector hcolw(Hcol[0]);
        const SimTK::Vector hcolv(Hcol[1]);

        transposeH_PB_w[i] = ~hcolw;
        transposeH_PB_v[i] = ~hcolv;
    }

    // Spatial force and torque vectors
    SimTK::Vector Fv(3, 0.0), Fw(3, 0.0);

    // Solve the pseudoinverse problem of Fv = pinv(~H_PB_G_v)*f;
    SimTK::FactorQTZ pinvForce(transposeH_PB_v);

    //if rank = 0, body force cannot contribute to the mobility force
    if(pinvForce.getRank() > 0)
        pinvForce.solve(f, Fv);
    
    // Now solve the pseudoinverse for torque for any unaccounted f: Fw = pinv(~H_PB_G_w)*(f - ~H_PB_G_v*Fv);
    SimTK::FactorQTZ pinvTorq(transposeH_PB_w);

    //if rank = 0, body torque cannot contribute to the mobility force
    if(pinvTorq.getRank() > 0)
        pinvTorq.solve(f, Fw);
    
    // Now we have two solution with either the body force Fv or body torque accounting for some or all of f
    SimTK::Vector fv =  transposeH_PB_v*Fv;
    SimTK::Vector fw =  transposeH_PB_w*Fw; 

    // which to choose? Choose the more effective as fx.norm/Fx.norm
    if(fv.norm() > SimTK::SignificantReal){ // if body force can contributes at all
        // if body torque can contribute too and it is more effective
        if(fw.norm() > SimTK::SignificantReal){
            if (fw.norm()/Fw.norm() > fv.norm()/Fv.norm() ){ 
                // account for f using torque, Fw, so compute Fv with remainder
                pinvForce.solve(f-fw, Fv);      
            }else{
                // account for f using force, Fv, first and Fw from remainder
                pinvTorq.solve(f-fv, Fw);
            }
        }
        // else no torque contribution and Fw should be zero
    }
    // no force contribution but have a torque
    else if(fw.norm() > SimTK::SignificantReal){
        // just Fw
    }
    else{
        // should be the case where gen force is zero.
        assert(f.norm() < SimTK::SignificantReal);
    }

    // The spatial forces above are expressed in the joint frame of the parent
    // Transform from parent joint frame, P into the parent body frame, Po
    const SimTK::Rotation R_PPo = (mbd.getInboardFrame(s).R());

    // Re-express forces in ground, first by describing force in the parent, Po, 
    // frame instead of joint frame
    SimTK::Vec3 vecFw = R_PPo*SimTK::Vec3::getAs(&Fw[0]);
    SimTK::Vec3 vecFv = R_PPo*SimTK::Vec3::getAs(&Fv[0]);

    //Force Acting on joint frame, B,  in child body expressed in Parent body, Po
    SimTK::SpatialVec FB_Po(vecFw, vecFv);

    const MobilizedBody parent = mbd.getParentMobilizedBody();
    // to apply spatial forces on bodies they must be expressed in ground
    vecFw = parent.expressVectorInAnotherBodyFrame(s, FB_Po[0], ground);
    vecFv = parent.expressVectorInAnotherBodyFrame(s, FB_Po[1], ground);

    // Package resulting torque and force as a spatial vec
    SimTK::SpatialVec FB_G(vecFw, vecFv);

    return FB_G;
}

void Joint::updateFromXMLNode(SimTK::Xml::Element& aNode, int versionNumber)
{
    int documentVersion = versionNumber;
    bool converting = false;
    if (documentVersion < XMLDocument::getLatestVersion()){
        if (documentVersion<30500){
            XMLDocument::renameChildNode(aNode, "location", "location_in_child"); 
            XMLDocument::renameChildNode(aNode, "orientation", "orientation_in_child");
        }
        // Version 30501 converted Connector_Body_ to Connector_PhysicalFrame_
        if (documentVersion < 30501) {
            // Handle any models that have the Joint connecting to Bodies instead
            // of PhyscialFrames
            XMLDocument::renameChildNode(aNode, "Connector_Body_",
                                                "Connector_PhysicalFrame_");
        }
        // Version 30503 changed "parent_body" connector name to "parent_frame"
        // Convert location and orientation into PhysicalOffsetFrames owned by the Joint
        if (documentVersion < 30505) {
            // Elements for the parent and child names the joint connects
            SimTK::Xml::element_iterator parentNameElt;
            SimTK::Xml::element_iterator childNameElt;
            // The names of the two PhysicalFrames this joint connects
            std::string parentFrameName("");
            std::string childFrameName("");

            SimTK::Xml::element_iterator connectors_node = aNode.element_begin("connectors");

            SimTK::Xml::element_iterator connectorElement =
                connectors_node->element_begin("Connector_PhysicalFrame_");
            while (connectorElement != aNode.element_end()) {
                // If connector name is "parent_body" rename it to "parent_frame"
                if (connectorElement->getRequiredAttributeValue("name") == "parent_body") {
                    connectorElement->setAttributeValue("name", "parent_frame");
                }
                // If connector name is "parent_frame" get the name of the connectee
                if (connectorElement->getRequiredAttributeValue("name") == "parent_frame"){
                    parentNameElt = connectorElement->element_begin("connectee_name");
                    parentNameElt->getValueAs<std::string>(parentFrameName);
                }
                if (connectorElement->getRequiredAttributeValue("name") == "child_body") {
                    connectorElement->setAttributeValue("name", "child_frame");
                }
                if (connectorElement->getRequiredAttributeValue("name") == "child_frame") {
                    childNameElt =  connectorElement->element_begin("connectee_name");
                    childNameElt->getValueAs<std::string>(childFrameName);
                }
                ++connectorElement;
            }

            SimTK::Xml::element_iterator locParentElt =
                aNode.element_begin("location_in_parent");
            SimTK::Xml::element_iterator orientParentElt =
                aNode.element_begin("orientation_in_parent");
            SimTK::Xml::element_iterator locChildElt =
                aNode.element_begin("location_in_child");
            SimTK::Xml::element_iterator orientChildElt =
                aNode.element_begin("orientation_in_child");

            Vec3 location_in_parent(0);
            Vec3 orientation_in_parent(0);
            Vec3 location_in_child(0);
            Vec3 orientation_in_child(0);

            if (locParentElt != aNode.element_end()){
                locParentElt->getValueAs<Vec3>(location_in_parent);
            }
            if (orientParentElt != aNode.element_end()){
                orientParentElt->getValueAs<Vec3>(orientation_in_parent);
            }
            if (locChildElt != aNode.element_end()){
                locChildElt->getValueAs<Vec3>(location_in_child);
            }
            if (orientChildElt != aNode.element_end()){
                orientChildElt->getValueAs<Vec3>(orientation_in_child);
            }

            // now append updated frames to the property list if they are not
            // identity transforms.
            if ((location_in_parent.norm() > 0.0) ||
                (orientation_in_parent.norm() > 0.0)) {
                XMLDocument::addPhysicalOffsetFrame(aNode, parentFrameName+"_offset",
                    parentFrameName, location_in_parent, orientation_in_parent);
                parentNameElt->setValue(parentFrameName + "_offset");
            }

            // again for the offset frame on the child
            if ((location_in_child.norm() > 0.0) ||
                (orientation_in_child.norm() > 0.0)) {
                XMLDocument::addPhysicalOffsetFrame(aNode, childFrameName + "_offset",
                    childFrameName, location_in_child, orientation_in_child);
                childNameElt->setValue(childFrameName + "_offset");
            }
        }
    }

    Super::updateFromXMLNode(aNode, versionNumber);
}

int Joint::assignSystemIndicesToBodyAndCoordinates(
    const SimTK::MobilizedBody& mobod,
    const OpenSim::PhysicalFrame* mobilized,
    const int& numMobilities,
    const int& startingCoordinateIndex) const
{
    // If not OpenSim body provided as the one being mobilized assume it is 
    // and intermediate body and ignore.
    if (mobilized){
        // Index can only be assigned to a parent or child body connected by this
        // Joint

        SimTK_ASSERT3( ( (mobilized == &getParentFrame()) || 
                         (mobilized == &getChildFrame()) ||
                         (mobilized == _slaveBodyForParent.get()) ||
                         (mobilized == _slaveBodyForChild.get()) ),
            "%s::'%s' - Cannot assign underlying system index to a PhysicalFrame '%s', "
            "which is not a parent or child Frame of this Joint.",
                      getConcreteClassName().c_str(),
                      getName().c_str(), mobilized->getName().c_str());

        // ONLY the base Joint can do this assignment
        mobilized->setMobilizedBodyIndex(mobod.getMobilizedBodyIndex());

        // Note that setting the mobilized body index of a PhysicalOffsetFrame
        // does not set it on the parent PhysicalFrame. 
        // Do the check and set it here as well since only the Joint can set the index.
        const PhysicalOffsetFrame* physOff =
            dynamic_cast<const PhysicalOffsetFrame*>(mobilized);
        if (physOff) {
            physOff->getParentFrame().setMobilizedBodyIndex(mobod.getMobilizedBodyIndex());
        }
    }
    int nc = numCoordinates();
    SimTK_ASSERT3(numMobilities <= (nc - startingCoordinateIndex),
        "%s attempted to create an underlying SimTK::MobilizedBody that "
        "supplies %d mobilities but only %d required.",
                  getConcreteClassName().c_str(),
                  numMobilities, nc - startingCoordinateIndex);

    const CoordinateSet& coords = get_CoordinateSet();

    int j = startingCoordinateIndex;
    for (int iq = 0; iq < numMobilities; ++iq){
        if (j < nc){ // assign
            coords[j]._mobilizerQIndex = SimTK::MobilizerQIndex(iq);
            coords[j]._bodyIndex = mobod.getMobilizedBodyIndex();
            j++;
        }
        else{
            std::string msg = getConcreteClassName() +
                " creating MobilizedBody with more mobilities than declared Coordinates.";
            throw Exception(msg);
        }
    }
    return j;
}

/* Return the equivalent (internal) SimTK::Rigid::Body for a given parent OR
child OpenSim::Body. Not guaranteed to be valid until after addToSystem on
Body has be called  */
const SimTK::Body& Joint::getParentInternalRigidBody() const
{
    if (_slaveBodyForParent){
        return _slaveBodyForParent->extractInternalRigidBody();
    }

    return static_cast<const PhysicalFrame&>(getParentFrame()
        .findBaseFrame()).extractInternalRigidBody();
}
const SimTK::Body& Joint::getChildInternalRigidBody() const
{
    if (_slaveBodyForChild){
        return _slaveBodyForChild->extractInternalRigidBody();
    }

    return static_cast<const PhysicalFrame&>(getChildFrame()
        .findBaseFrame()).extractInternalRigidBody();;
}
