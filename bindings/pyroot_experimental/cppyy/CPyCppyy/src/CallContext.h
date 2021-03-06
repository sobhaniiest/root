#ifndef CPYCPPYY_CALLCONTEXT_H
#define CPYCPPYY_CALLCONTEXT_H

// Standard
#include <vector>

#include <sys/types.h>


namespace CPyCppyy {

// small number that allows use of stack for argument passing
const int SMALL_ARGS_N = 8;

// general place holder for function parameters
struct Parameter {
    union Value {
        bool           fBool;
        int8_t         fInt8;
        uint8_t        fUInt8;
        short          fShort;
        unsigned short fUShort;
        Int_t          fInt;
        UInt_t         fUInt;
        Long_t         fLong;
        intptr_t       fIntPtr;
        ULong_t        fULong;
        Long64_t       fLongLong;
        ULong64_t      fULongLong;
        float          fFloat;
        double         fDouble;
        LongDouble_t   fLongDouble;
        void*          fVoidp;
    } fValue;
    void* fRef;
    char  fTypeCode;
};

// extra call information
struct CallContext {
    CallContext() : fFlags(0), fArgsVec(nullptr), fNArgs(0), fTemps(nullptr) {}
    CallContext(const CallContext&) = delete;
    CallContext& operator=(const CallContext&) = delete;
    ~CallContext() { if (fTemps) Cleanup(); delete fArgsVec; }

    enum ECallFlags {
        kNone           = 0x0000,
        kIsSorted       = 0x0001, // if method overload priority determined
        kIsCreator      = 0x0002, // if method creates python-owned objects
        kIsConstructor  = 0x0004, // if method is a C++ constructor
        kHaveImplicit   = 0x0008, // indicate that implicit converters are available
        kAllowImplicit  = 0x0010, // indicate that implicit coversions are allowed
        kNoImplicit     = 0x0020, // disable implicit to prevent recursion
        kUseHeuristics  = 0x0040, // if method applies heuristics memory policy
        kUseStrict      = 0x0080, // if method applies strict memory policy
        kReleaseGIL     = 0x0100, // if method should release the GIL
        kSetLifeline    = 0x0200, // if return value is part of 'this'
        kFast           = 0x0400, // if method should NOT handle signals
        kSafe           = 0x0800, // if method should return on signals
        kIsPseudoFunc   = 0x1000, // internal, used for introspection
    };

// memory handling
    static ECallFlags sMemoryPolicy;
    static bool SetMemoryPolicy(ECallFlags e);

    void AddTemporary(PyObject* pyobj);
    void Cleanup();

// signal safety
    static ECallFlags sSignalPolicy;
    static bool SetSignalPolicy(ECallFlags e);

    Parameter* GetArgs(size_t sz) {
        if (sz != (size_t)-1) fNArgs = sz;
        if (fNArgs <= SMALL_ARGS_N) return fArgs;
        if (!fArgsVec) fArgsVec = new std::vector<Parameter>();
        fArgsVec->resize(fNArgs);
        return fArgsVec->data();
    }

    Parameter* GetArgs() {
        if (fNArgs <= SMALL_ARGS_N) return fArgs;
        return fArgsVec->data();
    }
 
    size_t GetSize() { return fNArgs; }

public:
    uint64_t fFlags;

private:
// payload
    Parameter fArgs[SMALL_ARGS_N];
    std::vector<Parameter>* fArgsVec;
    size_t fNArgs;
    struct Temporary { PyObject* fPyObject; Temporary* fNext; };
    Temporary* fTemps;
};

inline bool IsSorted(uint64_t flags) {
    return flags & CallContext::kIsSorted;
}

inline bool IsCreator(uint64_t flags) {
    return flags & CallContext::kIsCreator;
}

inline bool IsConstructor(uint64_t flags) {
    return flags & CallContext::kIsConstructor;
}

inline bool HaveImplicit(CallContext* ctxt) {
    return ctxt ? (!(ctxt->fFlags & CallContext::kNoImplicit) && (ctxt->fFlags & CallContext::kHaveImplicit)) : false;
}

inline bool AllowImplicit(CallContext* ctxt) {
    return ctxt ? (!(ctxt->fFlags & CallContext::kNoImplicit) && (ctxt->fFlags & CallContext::kAllowImplicit)) : false;
}

inline bool NoImplicit(CallContext* ctxt) {
    return ctxt ? (ctxt->fFlags & CallContext::kNoImplicit) : false;
}

inline bool ReleasesGIL(CallContext* ctxt) {
    return ctxt ? (ctxt->fFlags & CallContext::kReleaseGIL) : false;
}

inline bool UseStrictOwnership(CallContext* ctxt) {
    if (ctxt && (ctxt->fFlags & CallContext::kUseStrict))
        return true;
    if (ctxt && (ctxt->fFlags & CallContext::kUseHeuristics))
        return false;

    return CallContext::sMemoryPolicy == CallContext::kUseStrict;
}

} // namespace CPyCppyy

#endif // !CPYCPPYY_CALLCONTEXT_H
