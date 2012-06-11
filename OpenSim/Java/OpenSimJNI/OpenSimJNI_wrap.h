/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 * 
 * This file is not intended to be easily readable and contains a number of 
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG 
 * interface file instead. 
 * ----------------------------------------------------------------------------- */

#ifndef SWIG_opensimModel_WRAP_H_
#define SWIG_opensimModel_WRAP_H_

class SwigDirector_AnalysisWrapper : public OpenSim::AnalysisWrapper, public Swig::Director {

public:
    void swig_connect_director(JNIEnv *jenv, jobject jself, jclass jcls, bool swig_mem_own, bool weak_global);
    SwigDirector_AnalysisWrapper(JNIEnv *jenv, OpenSim::Model *aModel = 0);
    virtual ~SwigDirector_AnalysisWrapper();
    virtual OpenSim::AnalysisWrapper *clone() const;
    virtual std::string const &getConcreteClassName() const;
    virtual OpenSim::VisibleObject const *getDisplayer() const;
    virtual OpenSim::VisibleObject *updDisplayer();
    virtual void updateFromXMLNode(SimTK::Xml::Element &node, int versionNumber);
    virtual void updateXMLNode(SimTK::Xml::Element &parent);
    virtual bool isA(char const *type) const;
    virtual void assign(OpenSim::Object &aObject);
    virtual int begin(SimTK::State &s);
    virtual int step(SimTK::State const &s, int stepNumber);
    virtual int end(SimTK::State &s);
    virtual void setModel(OpenSim::Model &aModel);
    virtual void setStatesStore(OpenSim::Storage const &aStatesStore);
    virtual bool proceed(int aStep = 0);
    virtual OpenSim::ArrayPtrs< OpenSim::Storage > &getStorageList();
    virtual int printResults(std::string const &aBaseName, std::string const &aDir = "", double aDT = -1.0, std::string const &aExtension = ".sto");
public:
    bool swig_overrides(int n) {
      return (n < 20 ? swig_override[n] : false);
    }
protected:
    bool swig_override[20];
};

class SwigDirector_SimtkLogCallback : public OpenSim::SimtkLogCallback, public Swig::Director {

public:
    void swig_connect_director(JNIEnv *jenv, jobject jself, jclass jcls, bool swig_mem_own, bool weak_global);
    SwigDirector_SimtkLogCallback(JNIEnv *jenv);
    virtual ~SwigDirector_SimtkLogCallback();
    virtual void log(std::string const &str);
public:
    bool swig_overrides(int n) {
      return (n < 1 ? swig_override[n] : false);
    }
protected:
    bool swig_override[1];
};


#endif
