//
//  ndnrtc-library.h
//  ndnrtc
//
//  Copyright 2013 Regents of the University of California
//  For licensing details see the LICENSE file.
//
//  Author:  Peter Gusev
//

#ifndef __ndnrtc__ndnrtc_library__
#define __ndnrtc__ndnrtc_library__

#include <dlfcn.h>

#include "ndnrtc-common.h"
#include "ndnrtc-object.h"

namespace ndnrtc {
    class INdnRtcLibraryObserver {
    public:
        virtual void onStateChanged(const char *state, const char *args) = 0;
    };
    
    class NdnRtcLibrary : public INdnRtcObjectObserver {
    public:
        // construction/desctruction
        NdnRtcLibrary(void *libHandle):observer_(NULL), libraryHandle_(libHandle){}
        ~NdnRtcLibrary(){}
        
        // public static methods go here
        static NdnRtcLibrary *instantiateLibraryObject(const char *libPath)
        {
            void *libHandle = dlopen(libPath, RTLD_LAZY);
            
            if (libHandle == NULL)
            {
                ERR("error while loading NdnRTC library: %s", dlerror());
                return NULL;
            }
            
            NdnRtcLibrary* (*create_ndnrtc)(void *);
            create_ndnrtc = (NdnRtcLibrary* (*)(void*))dlsym(libHandle, "create_ndnrtc");
            
            NdnRtcLibrary *libObject = create_ndnrtc(libHandle);
            
            return libObject;
        }
        
        static void destroyLibraryObject(NdnRtcLibrary *libObject)
        {
            void (*destroy_ndnrtc)(NdnRtcLibrary*);
            destroy_ndnrtc = (void (*)(NdnRtcLibrary*))dlsym(libObject->getLibraryHandle(), "destroy_ndnrtc");
            
            destroy_ndnrtc(libObject);
        }
        
        // <#public attributes go here#>
        
        // public methods go here
        virtual void setObserver(INdnRtcLibraryObserver *observer) { observer_ = observer; }
        virtual void* getLibraryHandle(){ return libraryHandle_; };

        virtual int startConference(const char *username);
//        virtual int startConference(ndnrtc::NdnParams &params);
        virtual int joinConference(const char *conferencePrefix);
        virtual int leaveConference(const char *conferencePrefix);
        virtual void onErrorOccurred(const char *errorMessage);
        
    private:
        void *libraryHandle_;
        INdnRtcLibraryObserver *observer_;
        
        // private methods go here
        int notifyObserverWithError(const char *format, ...);
        int notifyObserverWithState(const char *stateName, const char *format, ...);
        void notifyObserver(const char *state, const char *args);
    };
}


#endif /* defined(__ndnrtc__ndnrtc_library__) */
