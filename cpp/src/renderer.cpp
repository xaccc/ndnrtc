//
//  canvas-renderer.cpp
//  ndnrtc
//
//  Created by Peter Gusev on 8/19/13.
//  Copyright (c) 2013 Peter Gusev. All rights reserved.
//

#include "renderer.h"

// xpcom
#include "nsXPCOMCIDInternal.h"
#include "nsServiceManagerUtils.h"
#import "objc/cocoa-renderer.h"

using namespace ndnrtc;
using namespace webrtc;

//********************************************************************************
#pragma mark - construction/destruction
NdnRenderer::NdnRenderer(int rendererId, NdnRendererParams *params) : NdnRtcObject(params), rendererId_(rendererId)
{
    TRACE("");
    
    unsigned int width = getParams()->getRenderAreaWidth();
    unsigned int height = getParams()->getRenderAreaHeight();
    
    render_ = VideoRender::CreateVideoRender(rendererId_, createCocoaRenderWindow(rendererId_, width, height), false, kRenderCocoa);
    frameSink_ = render_->AddIncomingRenderStream(rendererId_, 0, 0.f, 0.f, 1.f, 1.f);
    render_->StartRender(rendererId_);
}
NdnRenderer::~NdnRenderer()
{
    TRACE("");
    render_->StopRender(rendererId_);
}
//********************************************************************************
#pragma mark - intefaces realization - IRawFrameConsumer
void NdnRenderer::onDeliverFrame(I420VideoFrame &frame)
{
    TRACE("rendering frame");
    frameSink_->RenderFrame(rendererId_, frame);
}

//********************************************************************************
#pragma mark - private