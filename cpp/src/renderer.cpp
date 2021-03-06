//
//  canvas-renderer.cpp
//  ndnrtc
//
//  Copyright 2013 Regents of the University of California
//  For licensing details see the LICENSE file.
//
//  Author:  Peter Gusev
//  Created: 8/19/13
//

#undef DEBUG

#include "renderer.h"
#import "objc/cocoa-renderer.h"
#include "ndnrtc-utils.h"

using namespace ndnrtc;
using namespace webrtc;

//******************************************************************************
//******************************************************************************
#pragma mark - construction/destruction
NdnRenderer::NdnRenderer(int rendererId, const ParamsStruct &params) :
NdnRtcObject(params),
rendererId_(rendererId)
{
}
NdnRenderer::~NdnRenderer()
{
    stopRendering();
    
    if (renderWindow_)
    {
        destroyCocoaRenderWindow(renderWindow_);
        renderWindow_ = NULL;
    }
    
    if (render_)
    {
        VideoRender::DestroyVideoRender(render_);
        render_ = NULL;
    }
}
//******************************************************************************
#pragma mark - public
int NdnRenderer::init()
{
    int res = RESULT_OK;
    
    unsigned int width = ParamsStruct::validateLE(params_.renderWidth, MaxWidth,
                                                  res, DefaultParams.renderWidth);
    unsigned int height = ParamsStruct::validateLE(params_.renderHeight, MaxHeight,
                                                   res, DefaultParams.renderHeight);
    
    renderWindow_ = createCocoaRenderWindow("", width, height);
    render_ = VideoRender::CreateVideoRender(rendererId_, renderWindow_, false, kRenderCocoa);
    
    if (render_ == nullptr)
        return notifyError(-1, "can't initialize renderer");
    
    frameSink_ = render_->AddIncomingRenderStream(rendererId_, 0, 0.f, 0.f, 1.f, 1.f);
    
    if (res)
        notifyError(RESULT_WARN, "renderer params were not verified. \
                    using these instead: width %d, height %d", width, height);
    
    return res;
}
int NdnRenderer::startRendering(const string &windowName)
{
    TRACE("[BUGTRACE] SETTING RENDER TITLE");
    setWindowTitle(windowName.c_str(), renderWindow_);
    
    if (render_->StartRender(rendererId_) < 0)
    {
        TRACE("[BUGTRACE] CAN'T START RENDERER");
        return notifyError(RESULT_ERR, "can't start rendering");
    }
    
    TRACE("[BUGTRACE] STARTED RENDERER");
    isRendering_ = true;
    
    return 0;
}
int NdnRenderer::stopRendering()
{
    if (render_)
    {
        render_->StopRender(rendererId_);
    }
    
    isRendering_ = false;
    return RESULT_OK;
}
//******************************************************************************
#pragma mark - intefaces realization - IRawFrameConsumer
void NdnRenderer::onDeliverFrame(I420VideoFrame &frame)
{
    if (isRendering_)
    {
        TRACE("render frame at %ld", NdnRtcUtils::millisecondTimestamp());
        int res = frameSink_->RenderFrame(rendererId_, frame);

        if (res < 0)
            NDNERROR("error while rendering frame");
        else
            TRACE("rendering result %d", res);
    }
    else
        notifyError(RESULT_ERR, "render not started");
}

//******************************************************************************
#pragma mark - private
