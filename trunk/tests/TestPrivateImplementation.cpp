/* -------------------------------------------------------------------------- *
 *                      SimTK Core: SimTKcommon                               *
 * -------------------------------------------------------------------------- *
 * This is part of the SimTK Core biosimulation toolkit originating from      *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2007 Stanford University and the Authors.           *
 * Authors: Michael Sherman                                                   *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "SimTKcommon.h"

#include "SimTKcommon/internal/PrivateImplementation.h"
// we'll include PrivateImplementation_Defs.h later to make sure
// none of the client-visible stuff is dependent on it.

#include <iostream>
using std::cout; using std::endl;

#include <string>
using std::string;

using namespace SimTK;

#define ASSERT(cond) {SimTK_ASSERT_ALWAYS(cond, "Assertion failed");}


    /////////////////
    // CLIENT SIDE //
    /////////////////

// The Handles defined here represent ones that would be present in
// user-includable header files. They should contain no hint of their
// implementations, except for a forward-declared name!

// This is a simple, concrete class which has been split into a
// Handle and an Implementation. It has ordinary object (value)
// semantics meaning that copy construction an copy assignment 
// produce independent objects, not an additional reference to an
// existing object.

class Concrete_Impl;
class Concrete : public PIMPLHandle<Concrete, Concrete_Impl> {
public:
    Concrete(); // default constructor allocates an implementation

    // The implementation object can store a string.
    void setString(const string n);
    const string& getString() const;
};

// This is the identical class except it has reference semantics.
class ConcreteRef_Impl;
class ConcreteRef : public PIMPLHandle<ConcreteRef, ConcreteRef_Impl, true> {
public:
    ConcreteRef(); // default constructor allocates an implementation

    // The implementation object can store a string.
    void setString(const string n);
    const string& getString() const;
};


class MyHandle_Impl;
class MyHandle : public PIMPLHandle<MyHandle, MyHandle_Impl, false> {
public:
    MyHandle();
    explicit MyHandle(MyHandle_Impl* p) : HandleBase(p) { }

    void setName(const string n);
    const string& getName() const;
};
std::ostream& operator<<(std::ostream& o, const MyHandle& h) {
    o << "MyHandle ";
    if (h.isEmptyHandle()) return o << "IS EMPTY\n";

    o << "(name=" << h.getName() << "):";
    return o << static_cast<const MyHandle::ParentHandle&>(h);
}

class DerivedHandle_Impl;
class DerivedHandle : public PIMPLDerivedHandle<DerivedHandle,DerivedHandle_Impl,MyHandle> {
public:
    DerivedHandle();
    explicit DerivedHandle(DerivedHandle_Impl*);

    void setReal(Real);
    Real getReal() const;
};
std::ostream& operator<<(std::ostream& o, const DerivedHandle& h) {
    o << "DerivedHandle ";
    if (h.isEmptyHandle()) return o << "IS EMPTY\n";

    o << "(real=" << h.getReal() << "):";
    return o << static_cast<const DerivedHandle::ParentHandle&>(h);
}

class DerDerivedHandle_Impl;
class DerDerivedHandle : public PIMPLDerivedHandle<DerDerivedHandle,DerDerivedHandle_Impl,DerivedHandle> {
public:
    DerDerivedHandle();
    explicit DerDerivedHandle(DerDerivedHandle_Impl*);

    void setString(string);
    string getString() const;
};
std::ostream& operator<<(std::ostream& o, const DerDerivedHandle& h) {
    o << "DerDerivedHandle ";
    if (h.isEmptyHandle()) return o << "IS EMPTY\n";

    o << "(string=" << h.getString() << "):";
    return o << static_cast<const DerDerivedHandle::ParentHandle&>(h);
}


int main() {
  try
  {
    Concrete c1, c2;
    ConcreteRef cr1, cr2;

    // Handles should consist precisely of one pointer.
    ASSERT(sizeof(c1) == sizeof(void*));
    ASSERT(sizeof(cr1) == sizeof(void*));

    ASSERT(!c1.isEmptyHandle());
    ASSERT(c2.isOwnerHandle());
    ASSERT(!c1.isSameHandle(c2));
    ASSERT(c2.isSameHandle(c2));

    ASSERT(!cr1.isEmptyHandle());
    ASSERT(cr2.isOwnerHandle());
    ASSERT(!cr1.isSameHandle(cr2));
    ASSERT(cr2.isSameHandle(cr2));

    c1.setString("c1"); cr1.setString("cr1");
    c2.setString("c2"); cr2.setString("cr2");
    ASSERT(c1.getString() == "c1");
    ASSERT(c2.getString() == "c2");
    ASSERT(cr1.getString() == "cr1");
    ASSERT(cr2.getString() == "cr2");

    Concrete copyOfc2(c2);      ConcreteRef copyOfcr2(cr2);
    Concrete copyOfc2ViaAssign; ConcreteRef copyOfcr2ViaAssign;
    copyOfc2ViaAssign = c2; 
    copyOfc2ViaAssign.clearHandle();
    copyOfc2ViaAssign = c2; 

    // can't do reference assign to an owner handle
    copyOfcr2ViaAssign.clearHandle(); 
    copyOfcr2ViaAssign = cr2;

    ASSERT(copyOfc2.isOwnerHandle() && copyOfc2ViaAssign.isOwnerHandle());
    ASSERT(!copyOfcr2.isOwnerHandle() && !copyOfcr2ViaAssign.isOwnerHandle());


    ASSERT(&c2.getImpl() != &copyOfc2.getImpl());
    ASSERT(&c2.getImpl() != &copyOfc2ViaAssign.getImpl());
    ASSERT(&cr2.getImpl() == &copyOfcr2.getImpl());
    ASSERT(&cr2.getImpl() == &copyOfcr2ViaAssign.getImpl());


    ASSERT(copyOfc2.getString() == c2.getString());
    ASSERT(copyOfc2ViaAssign.getString() == c2.getString());
    ASSERT(copyOfcr2.getString() == cr2.getString());
    ASSERT(copyOfcr2ViaAssign.getString() == cr2.getString());

    copyOfc2.setString("copyOfc2");
    ASSERT(c2.getString() == "c2" && copyOfc2.getString() == "copyOfc2");
    ASSERT(copyOfc2ViaAssign.getString() == "c2");

    copyOfcr2.setString("copyOfcr2");
    ASSERT(cr2.getString() == "copyOfcr2");
    ASSERT(copyOfcr2ViaAssign.getString() == "copyOfcr2");


    copyOfc2ViaAssign.setString("copyOfc2ViaAssign");
    ASSERT(copyOfc2ViaAssign.getString() == "copyOfc2ViaAssign");
    ASSERT(c2.getString() == "c2" && copyOfc2.getString() == "copyOfc2");

    c1.clearHandle(); ASSERT(c1.isEmptyHandle()); ASSERT(!c1.isOwnerHandle());
    cr1.clearHandle(); ASSERT(cr1.isEmptyHandle()); ASSERT(!cr1.isOwnerHandle());

    c1.referenceAssign(c2); // now c1 & c2 point to same impl
    cr1.copyAssign(cr2);    // while cr1 & cr2 point to different implementations
    ASSERT(&c1.getImpl() == &c2.getImpl());
    ASSERT(&cr1.getImpl() != &cr2.getImpl());
    c1.setString("setThroughC1"); cr1.setString("setThroughCR1");

    ASSERT(c1.getString() == "setThroughC1" && c2.getString() == "setThroughC1");
    ASSERT(cr1.getString() == "setThroughCR1" && cr2.getString() == "copyOfcr2");
    
    // TODO: regression tests for hierarchical handle classes

    // TODO: regression tests for client-side custom implementations

    // TODO: code below is not yet in regression test form. However, these should not
    // thrown any exceptions.
    MyHandle h;
    h.setName("Fred");
    cout << "h=" << h;

    // Make empty handle
    MyHandle empty(0);
    h.disown(empty);
    cout << "after disown h=" << h;
    cout << "  empty=" << empty;

    MyHandle h2 = h;
    cout << "h2=" << h2;

    DerivedHandle hderived;
    hderived.setName("Joe");
    cout << "hderived=" << hderived;

    MyHandle h3 = hderived;
    cout << "h3=" << h3;
    cout << "downcast h3.getReal()=" << DerivedHandle::downcast(h3).getReal() << endl;

    DerivedHandle d2(hderived);
    cout << "d2=" << d2;

    DerivedHandle dempty(0);
    cout << "dempty=" << dempty;

    dempty = DerivedHandle::downcast(d2);
    cout << "dempty=h, dempty=" << dempty;

    DerDerivedHandle dd; dd.setString("hi there");
    cout << "dd=" << dd;

    cout << "dd.upcast()  =" << dd.upcast();
    cout << "dd.upcast^2()= "<< dd.upcast().upcast();

    cout << "sizeof(MyHandle)=" << sizeof(MyHandle) 
         << " sizeof(DerivedHandle)=" << sizeof(DerivedHandle) 
         << " sizeof(DerDerivedHandle)=" << sizeof(DerDerivedHandle) 
         << endl;


  } 
  catch (const std::exception& e) {
    printf("EXCEPTION THROWN: %s\n", e.what());
    return 1;
  } 
  catch (...) {
    printf("UNKNOWN EXCEPTION THROWN\n");
    return 1;
  }    

    return 0;
}



    //////////////////
    // LIBRARY SIDE //
    //////////////////

// Code here would normally be in some other compilation unit and available
// only as a binary.

#include "SimTKcommon/internal/PrivateImplementation_Defs.h"

    // CONCRETE IMPL //
class Concrete_Impl : public PIMPLImplementation<Concrete,Concrete_Impl> {
public:
    Concrete_Impl* clone() const {return new Concrete_Impl(*this);}
    string s;
    friend class Concrete;
};
    // CONCRETE REF IMPL //
class ConcreteRef_Impl : public PIMPLImplementation<ConcreteRef,ConcreteRef_Impl> {
public:
    ConcreteRef_Impl* clone() const {return new ConcreteRef_Impl(*this);}
    string s;
    friend class ConcreteRef;
};

    // CONCRETE & CONCRETE REF HANDLE IMPLEMENTATIONS //
Concrete::Concrete()       : HandleBase(new Concrete_Impl()) { }
ConcreteRef::ConcreteRef() : HandleBase(new ConcreteRef_Impl()) { }

void Concrete::setString(const string n)    {updImpl().s = n;}
void ConcreteRef::setString(const string n) {updImpl().s = n;}

const string& Concrete::getString()    const {return getImpl().s;}
const string& ConcreteRef::getString() const {return getImpl().s;}


    // OTHER IMPLs and HANDLE IMPLEMENTATIONS //

class MyHandle_Impl : public PIMPLImplementation<MyHandle,MyHandle_Impl> {
public:
    virtual ~MyHandle_Impl() { }
    virtual MyHandle_Impl* clone() const {return new MyHandle_Impl(*this);}

private:
    string name;
    friend class MyHandle;
};


class DerivedHandle_Impl : public MyHandle_Impl {
public:
    virtual DerivedHandle_Impl* clone() const {return new DerivedHandle_Impl(*this);}

private:
    Real r;
    friend class DerivedHandle;
};


class DerDerivedHandle_Impl : public DerivedHandle_Impl {
public:
    virtual DerDerivedHandle_Impl* clone() const {return new DerDerivedHandle_Impl(*this);}

private:
    string s;
    friend class DerDerivedHandle;
};

MyHandle::MyHandle() : HandleBase(new MyHandle_Impl()) {
}

void MyHandle::setName(const string n) {
    updImpl().name = n;
}

const string& MyHandle::getName() const {
    return getImpl().name;
}

DerivedHandle::DerivedHandle() : PIMPLDerivedHandleBase(new DerivedHandle_Impl()) {
    setReal(27);
}

DerivedHandle::DerivedHandle(DerivedHandle_Impl* p) : PIMPLDerivedHandleBase(p) { }

void DerivedHandle::setReal(Real rr) {updImpl().r=rr;}
Real DerivedHandle::getReal() const {return getImpl().r;}

DerDerivedHandle::DerDerivedHandle() : PIMPLDerivedHandleBase(new DerDerivedHandle_Impl()) {
    setString("default dd string");
    setReal(22.345);
}
DerDerivedHandle::DerDerivedHandle(DerDerivedHandle_Impl* p) : PIMPLDerivedHandleBase(p) { }

void DerDerivedHandle::setString(string s) {updImpl().s=s;}
string DerDerivedHandle::getString() const {return getImpl().s;}

