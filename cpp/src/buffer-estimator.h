//
//  buffer-estimator.h
//  ndnrtc
//
//  Copyright 2013 Regents of the University of California
//  For licensing details see the LICENSE file.
//
//  Author:  Peter Gusev
//

#ifndef __ndnrtc__buffer_estimator__
#define __ndnrtc__buffer_estimator__

#include "ndnrtc-common.h"

namespace ndnrtc {
    namespace new_api
    {
        class BufferEstimator {
        public:
            void
            setProducerRate(double producerRate);
        };
    }
}

#endif /* defined(__ndnrtc__buffer_estimator__) */
