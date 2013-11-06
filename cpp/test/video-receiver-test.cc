//
//  video-receiver-test.cc
//  ndnrtc
//
//  Copyright 2013 Regents of the University of California
//  For licensing details see the LICENSE file.
//
//  Author:  Peter Gusev
//

#include "test-common.h"
#include "frame-buffer.h"
#include "playout-buffer.h"
#include "video-receiver.h"
#include "ndnrtc-namespace.h"

using namespace ndnrtc;

::testing::Environment* const env = ::testing::AddGlobalTestEnvironment(new NdnRtcTestEnvironment(ENV_NAME));

//********************************************************************************
// VideoReceiver tests
class NdnReceiverTester :
public NdnRtcObjectTestHelper,
public IEncodedFrameConsumer,
public UnitTestHelperNdnNetwork
{
public :
    void SetUp()
    {
        NdnRtcObjectTestHelper::SetUp();
        
        params_ = DefaultParams;
        
        string userPrefix = "", streamAccessPrefix = "";
        ASSERT_EQ(RESULT_OK, MediaSender::getUserPrefix(params_, userPrefix));
        ASSERT_EQ(RESULT_OK, MediaSender::getStreamKeyPrefix(params_, streamAccessPrefix));
        
        UnitTestHelperNdnNetwork::NdnSetUp(streamAccessPrefix, userPrefix);
        
        frame = NdnRtcObjectTestHelper::loadEncodedFrame();
        payload = new NdnFrameData(*frame);
    }
    void TearDown()
    {
        NdnRtcObjectTestHelper::TearDown();
        UnitTestHelperNdnNetwork::NdnTearDown();
    }
    
    void onEncodedFrameDelivered(webrtc::EncodedImage &encodedImage)
    {
        receivedFrames_.push_back(encodedImage.capture_time_ms_);
        TRACE("fetched encoded frame. queue size %d", receivedFrames_.size());
    }
    
    void flushFlags()
    {
        NdnRtcObjectTestHelper::flushFlags();
        
        fetchedFrames_.clear();
        sentFrames_.clear();
        receivedFrames_.clear();
    }
    
protected:
    std::vector<webrtc::EncodedImage*> fetchedFrames_;
    std::vector<uint64_t> sentFrames_;
    std::vector<uint64_t> receivedFrames_;
    webrtc::EncodedImage *frame;
    NdnFrameData *payload;
    
    void publishMediaPacket(unsigned int dataLen, unsigned char *dataPacket,
                            unsigned int frameNo, unsigned int segmentSize,
                            const string &framePrefix, int freshness,
                            bool mixedSendOrder)
    {
        UnitTestHelperNdnNetwork::publishMediaPacket(dataLen, dataPacket,
                                                     frameNo, segmentSize,
                                                     framePrefix, freshness,
                                                     mixedSendOrder);
        sentFrames_.push_back(millisecondTimestamp());
    }
};


TEST_F(NdnReceiverTester, CreateDelete)
{
    NdnVideoReceiver *receiver = new NdnVideoReceiver(DefaultParams);
    delete receiver;
}

TEST_F(NdnReceiverTester, WrongCallSequences)
{
    NdnVideoReceiver *receiver = new NdnVideoReceiver(DefaultParams);

    receiver->setObserver(this);
    
    flushFlags();
    { // call stop
        EXPECT_EQ(RESULT_ERR, receiver->stopFetching());
        EXPECT_TRUE(obtainedError_);
    }
    
    flushFlags();
    { // call start
        EXPECT_EQ(RESULT_ERR, receiver->startFetching());
        EXPECT_TRUE(obtainedError_);
    }
    
    flushFlags();
    { // call stop without start
        EXPECT_EQ(RESULT_OK, receiver->init(ndnFace_));
        EXPECT_EQ(RESULT_ERR, receiver->stopFetching());
        EXPECT_TRUE(obtainedError_);
    }
    
    delete receiver;
}

TEST_F(NdnReceiverTester, Init)
{
    NdnVideoReceiver *receiver = new NdnVideoReceiver(params_);
    
    EXPECT_EQ(0, receiver->init(ndnReceiverFace_));
    
    delete receiver;
}

TEST_F(NdnReceiverTester, EmptyFetching)
{
    NdnVideoReceiver *receiver = new NdnVideoReceiver(params_);
    
    receiver->setObserver(this);
    
    EXPECT_EQ(0, receiver->init(ndnReceiverFace_));
    EXPECT_FALSE(obtainedError_);
    
    EXPECT_EQ(0,receiver->startFetching());
    
    WAIT(100);
    
    EXPECT_EQ(0,receiver->stopFetching());
    
    delete receiver;
}

TEST_F(NdnReceiverTester, Fetching30FPS)
{
    unsigned int framesNum = 30;
    unsigned int segmentSize = 100;
    unsigned int bufferSize = 10;
    unsigned int producerFrameRate = 30;
    
    params_.bufferSize = bufferSize;
    params_.segmentSize = segmentSize;
    params_.producerRate = producerFrameRate;
    
    NdnVideoReceiver *receiver = new NdnVideoReceiver(params_);
    
    receiver->setFrameConsumer(this);
    receiver->setObserver(this);
    
    EXPECT_EQ(0, receiver->init(ndnReceiverFace_));
    EXPECT_FALSE(obtainedError_);
    
    TRACE("start fetching");
    EXPECT_EQ(0, receiver->startFetching());
    
    // we should start publishing frames later, so that receiver will get first frame
    // by issuing interest with RightMostChild selector
    WAIT(1000);
    string framePrefix;
    EXPECT_EQ(RESULT_OK, MediaSender::getStreamFramePrefix(params_, framePrefix));
    
    for (int i = 0; i < framesNum; i++)
    {
        payload = new NdnFrameData(*frame);
        publishMediaPacket(payload->getLength(), payload->getData(), i,
                           segmentSize, framePrefix, params_.freshness, true);
        TRACE("published frame %d",i);
        WAIT(1000/producerFrameRate);
    }
    EXPECT_TRUE_WAIT(receivedFrames_.size() == framesNum, 5000);

    if (framesNum == receivedFrames_.size())
    {
        for (int i = 0; i < framesNum; i++)
        {
            // check arrival order
            if (i != framesNum-1)
                EXPECT_LE(receivedFrames_[i], receivedFrames_[i+1]);
        }
    }

    receiver->stopFetching();
    delete receiver;
    WAIT(params_.freshness*1000);
}

TEST_F(NdnReceiverTester, Fetching1Segment30FPS)
{
    WAIT(5200);
    
    unsigned int framesNum = 30;
    unsigned int segmentSize = 1000;
    unsigned int bufferSize = 10;
    unsigned int producerFrameRate = 30;
    
    params_.bufferSize = bufferSize;
    params_.segmentSize = segmentSize;
    params_.producerRate = producerFrameRate;
    
    NdnVideoReceiver *receiver = new NdnVideoReceiver(params_);
    
    receiver->setFrameConsumer(this);
    receiver->setObserver(this);
    
    EXPECT_EQ(0, receiver->init(ndnReceiverFace_));
    EXPECT_FALSE(obtainedError_);
    
    TRACE("start fetching");
    receiver->startFetching();
    
    // we should start publishing frames later, so that receiver will get first frame
    // by issuing interest with RightMostChild selector
    WAIT(100);
    string framePrefix;
    EXPECT_EQ(RESULT_OK, MediaSender::getStreamFramePrefix(params_, framePrefix));

    for (int i = 0; i < framesNum; i++)
    {
        payload = new NdnFrameData(*frame);
        publishMediaPacket(payload->getLength(), payload->getData(), i,
                           segmentSize, framePrefix, params_.freshness, true);
        TRACE("published frame %d",i,5);
        WAIT(1000/producerFrameRate);
    }
    
    EXPECT_TRUE_WAIT(receivedFrames_.size() == framesNum, 5000);

    if (framesNum == receivedFrames_.size())
    {
        for (int i = 0; i < framesNum; i++)
            if (i != framesNum-1)
                EXPECT_LE(receivedFrames_[i], receivedFrames_[i+1]);
    }
    
    receiver->stopFetching();
    
    delete receiver;
    WAIT(params_.freshness*1000);
}

TEST_F(NdnReceiverTester, FetchingWithRecovery)
{
    unsigned int framesNum = 60;
    unsigned int segmentSize = 100;
    unsigned int bufferSize = 5;
    unsigned int producerFrameRate = 30;
    
    params_.bufferSize = bufferSize;
    params_.segmentSize = segmentSize;
    params_.producerRate = producerFrameRate;
    params_.interestTimeout = 1;
    
    NdnVideoReceiver *receiver = new NdnVideoReceiver(params_);
    
    receiver->setFrameConsumer(this);
    receiver->setObserver(this);
    
    EXPECT_EQ(0, receiver->init(ndnReceiverFace_));
    EXPECT_FALSE(obtainedError_);
    
    TRACE("start fetching");
    EXPECT_EQ(0, receiver->startFetching());
    string framePrefix;
    EXPECT_EQ(RESULT_OK, MediaSender::getStreamFramePrefix(params_, framePrefix));

    // we should start publishing frames later, so that receiver will get first frame
    // by issuing interest with RightMostChild selector
    WAIT(1000);
    {
        for (int i = 0; i < framesNum/2; i++)
        {
            payload = new NdnFrameData(*frame);
            publishMediaPacket(payload->getLength(), payload->getData(), i,
                               segmentSize, framePrefix, params_.freshness, true);
            TRACE("published frame %d",i);
            WAIT(1000/producerFrameRate);
        }
        
        EXPECT_TRUE_WAIT(receivedFrames_.size() == framesNum/2, 5000);
        
        if (framesNum == receivedFrames_.size())
        {
            for (int i = 0; i < framesNum; i++)
            {
                if (i != framesNum-1)
                    EXPECT_LE(receivedFrames_[i], receivedFrames_[i+1]);
            }
        }
    }
    
    flushFlags();
    
    // now let's wait a bit so receiver can switch back to chasing mode
    WAIT(FULL_TIMEOUT_CASE_THRESHOLD*params_.interestTimeout*1000+5000);
    
    { // now publish again
        for (int i = 0; i < framesNum/2; i++)
        {
            payload = new NdnFrameData(*frame);
            publishMediaPacket(payload->getLength(), payload->getData(), i,
                               segmentSize, framePrefix, params_.freshness, true);
            TRACE("published frame %d",i);
            WAIT(1000/producerFrameRate);
        }
        
        EXPECT_TRUE_WAIT(receivedFrames_.size() == framesNum/2, 5000);
        
        if (framesNum == receivedFrames_.size())
        {
            for (int i = 0; i < framesNum; i++)
            {
                if (i != framesNum-1)
                    EXPECT_LE(receivedFrames_[i], receivedFrames_[i+1]);
            }
        }
    }
    
    receiver->stopFetching();
    
    delete receiver;
    WAIT(params_.freshness*1000);
}

