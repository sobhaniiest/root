// @(#)root/tmva $Id$
// Author: Omar Zapata 2015

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : PyMethodBase                                                          *
 *                                                                                *
 * Description:                                                                   *
 *      Virtual base class for all MVA method based on python                     *
 *                                                                                *
 **********************************************************************************/

#include<TMVA/PyMethodBase.h>
#include<TApplication.h>
using namespace TMVA;

ClassImp(PyMethodBase)

PyObject *PyMethodBase::fModuleBuiltin;
PyObject *PyMethodBase::fEval;
PyObject *PyMethodBase::fModulePickle;
PyObject *PyMethodBase::fPickleDumps;
PyObject *PyMethodBase::fPickleLoads;
      
//_______________________________________________________________________
PyMethodBase::PyMethodBase(const TString &jobName,
                         Types::EMVA methodType,
                         const TString &methodTitle,
                         DataSetInfo &dsi,
                         const TString &theOption ,
                         TDirectory *theBaseDir): MethodBase(jobName, methodType, methodTitle, dsi, theOption, theBaseDir)
{
  if(!PyIsInitialized())
  {
    PyInitialize();
  }
}

//_______________________________________________________________________
PyMethodBase::PyMethodBase(Types::EMVA methodType,
                         DataSetInfo &dsi,
                         const TString &weightFile,
                         TDirectory *theBaseDir): MethodBase(methodType, dsi, weightFile, theBaseDir)
{
  if(!PyIsInitialized())
  {
    PyInitialize();
  }
}

//_______________________________________________________________________
PyMethodBase::~PyMethodBase()
{
  if(PyIsInitialized())
  {
    PyFinalize();
  }
}

//_______________________________________________________________________
PyObject* PyMethodBase::Eval(TString code)
{    
    PyObject *pycode = Py_BuildValue("(s)",code.Data());
    PyObject *result = PyObject_CallObject(fEval,pycode);
    Py_DECREF(pycode); 
    return result;
}

//_______________________________________________________________________
void PyMethodBase::PyInitialize()
{
  TMVA::MsgLogger Log;
  if(!PyIsInitialized())
  {
    Py_Initialize();
    _import_array();
    import_array();
  }
  //preparing objects for eval
  PyObject *bName= PyString_FromString("__builtin__");
  // Import the file as a Python module.
  fModuleBuiltin= PyImport_Import(bName);
  if(!fModuleBuiltin)
  {
      Log <<kFATAL<< "Can't import __builtin__" << Endl;
      Log << Endl;
  }    
  PyObject *mDict = PyModule_GetDict(fModuleBuiltin);
  fEval = PyDict_GetItemString(mDict, "eval");

  Py_DECREF(bName);   
  Py_DECREF(mDict);   
  //preparing objects for pickle
  PyObject *pName= PyString_FromString("pickle");
  // Import the file as a Python module.
  fModulePickle= PyImport_Import(pName);
  if(!fModulePickle)
  {
      Log <<kFATAL<< "Can't import pickle" << Endl;
      Log << Endl;
  }    
  PyObject *pDict = PyModule_GetDict(fModulePickle);
  fPickleDumps = PyDict_GetItemString(pDict, "dumps");
  fPickleLoads = PyDict_GetItemString(pDict, "loads");

  Py_DECREF(pName);   
  Py_DECREF(pDict);   
  
    
}
      
//_______________________________________________________________________
void PyMethodBase::PyFinalize()
{
    Py_Finalize();
    if(fEval) delete fEval;
    if(fModuleBuiltin) delete fModuleBuiltin;
    if(fPickleDumps) delete fPickleDumps;
    if(fPickleLoads) delete fPickleLoads;
}
      
//_______________________________________________________________________
int  PyMethodBase::PyIsInitialized()
{
    if(!Py_IsInitialized()) return kFALSE;
    if(!fEval) return kFALSE;
    if(!fModuleBuiltin) return kFALSE;
    if(!fPickleDumps) return kFALSE;
    if(!fPickleLoads) return kFALSE;
    return kTRUE;
}

      
