//
//  ndn-assembler.h
//  ndnrtc
//
//  Copyright 2013 Regents of the University of California
//  For licensing details see the LICENSE file.
//
//  Author:  Peter Gusev
//

#ifndef __ndnrtc__ndn_assembler__
#define __ndnrtc__ndn_assembler__

#include "ndnrtc-common.h"

namespace ndnrtc {
    namespace new_api {
        class Assembler {
        public:
            
            static shared_ptr<Assembler> getSharedInstance();
            static ndn::OnData defaultOnDataHandler();
            static ndn::OnTimeout defaultOnTimeoutHandler();
        private:
            
        };
    }
}

#endif /* defined(__ndnrtc__ndn_assembler__) */
