#ifndef OPENSIM_EXCEPTION_H_
#define OPENSIM_EXCEPTION_H_
/* -------------------------------------------------------------------------- *
 *                           OpenSim:  Exception.h                            *
 * -------------------------------------------------------------------------- *
 * The OpenSim API is a toolkit for musculoskeletal modeling and simulation.  *
 * See http://opensim.stanford.edu and the NOTICE file for more information.  *
 * OpenSim is developed at Stanford University and supported by the US        *
 * National Institutes of Health (U54 GM072970, R24 HD065690) and by DARPA    *
 * through the Warrior Web program.                                           *
 *                                                                            *
 * Copyright (c) 2005-2012 Stanford University and the Authors                *
 * Author(s): Frank C. Anderson                                               *
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

/* Note: This code was originally developed by Realistic Dynamics Inc. 
 * Author: Frank C. Anderson 
 */


// INCLUDES
#include "osimCommonDLL.h"
#include <string>

#ifdef WIN32
#pragma warning(disable:4251) /*no DLL interface for type of member of exported class*/
#pragma warning(disable:4275) /*no DLL interface for base class of exported class*/
#endif

#ifdef SWIG
    #ifdef OSIMCOMMON_API
        #undef OSIMCOMMON_API
        #define OSIMCOMMON_API
    #endif
#endif

//=============================================================================
//=============================================================================

namespace OpenSim {


/** Macros to throw OpenSim exceptions. The purpose of these macros is to avoid
having to provide the first few arguments (which are common to all OpenSim
exceptions) to the exception constructor in the throw statements. This also
allows us to add more details (eg class name) later easily.                   */

// Note -- Extra braces enclosing "if" are to avoid problems when these macros 
// are called within if-else statements like:
// \code{.cpp}
//           if(<some condition>)
//               OPENSIM_THROW_IF(<arguments>)
//           else
//               <some statements>
// \endcode                                                                     
#define OPENSIM_THROW(EXCEPTION, ...)                                \
    throw EXCEPTION{__FILE__, __LINE__, __func__, __VA_ARGS__};

#define OPENSIM_THROW_IF(CONDITION, EXCEPTION, ...)                  \
    {                                                                \
    if(CONDITION)                                                    \
        OPENSIM_THROW(EXCEPTION, __VA_ARGS__)                        \
    }



/**
 * A class for basic exception functionality.
 * \if developer
 * To create exception classes in OpenSim, use the following guidlines.
 * If the intention is the derive from an exception named (say) BaseException
 * that is part of OpenSim, use the following blueprint:
 * \code{.cpp}
 *     class MyNewException : public BaseException {
 *     public:
 *         MyNewException(const std::string& file,
 *                        size_t line,
 *                        const std::string& func,
 *                        <more parameters as appropriate>) :
 *         BaseException(file, line, func) {
 *             std::string message = <create the desired message>;
 *             addMessage(message);
 *         }
 *     };
 * \endcode
 * Exception class manages the concatenation of error messages from all the 
 * derived classes. When creating new exceptions, remember to call addMessage()
 * as shown above if the exception class does have any error message.
 * \endif
 *
 * @author Frank C. Anderson
 */
class OSIMCOMMON_API Exception  : public std::exception {

//=============================================================================
// DATA
//=============================================================================
private:
    /** A user set message for the exception. */
    std::string _msg;
    /** File in which the error occurred. */
    std::string _file;
    /** Line number at which the error occurred. */
    int _line;

//=============================================================================
// METHODS
//=============================================================================
public:
    // CONSTRUCTORS
    /** This constructor is for backward compatibility. Use the constructor
    taking file, line, func.                                                  */
    Exception(const std::string &aMsg="",
              const std::string &aFile="",
              int aLine=-1);

    /** Call this constructor from derived classes to add file, line and 
    function information to the error message                                 */
    Exception(const std::string& file,
              size_t line,
              const std::string& func);

    virtual ~Exception() throw() {}

protected:
    /** Add to the error message that will be returned for the exception.     */
    void addMessage(const std::string& msg);

private:
    void setNull();

public:
    // SET AND GET
    void setMessage(const std::string &aMsg);
    const char* getMessage() const;

#ifndef SWIG
    // PRINT
    virtual void print(std::ostream &aOut) const;
#endif
    // override virtual function from std::exception
    const char* what() const noexcept override;

//=============================================================================
};  // END CLASS Exception


class IndexOutOfRange : public Exception {
public:
    IndexOutOfRange(const std::string& file,
                    size_t line,
                    const std::string& func,
                    size_t index, 
                    size_t min, 
                    size_t max) :
        Exception(file, line, func) {
        std::string msg = "min = " + std::to_string(min);
        msg += " max = " + std::to_string(max);
        msg += " index = " + std::to_string(index);

        addMessage(msg);
    }
};

class KeyNotFound : public Exception {
public:
    KeyNotFound(const std::string& file,
                size_t line,
                const std::string& func,
                const std::string& key) :
        Exception(file, line, func) {
        std::string msg = "Key '" + key + "' not found.";

        addMessage(msg);
    }
};

}; //namespace
//=============================================================================
//=============================================================================

#endif // OPENSIM_EXCEPTION_H_
