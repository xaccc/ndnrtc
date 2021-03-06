//
//  ndnrtc-namespace-test.cc
//  ndnrtc
//
//  Created by Peter Gusev on 9/19/13.
//  Copyright (c) 2013 Peter Gusev. All rights reserved.
//

#include "test-common.h"
#include "ndnrtc-namespace.h"

#define NDN_COMMON_HUB "ndn/edu/ucla"

using namespace ndnrtc;

::testing::Environment* const env = ::testing::AddGlobalTestEnvironment(new NdnRtcTestEnvironment(ENV_NAME));

TEST(NdnRtcNamespace, TestBuildPathComponents)
{
    std::string producerId = "producer1";
    std::string hub = NDN_COMMON_HUB;
    std::string stream = "video0";
    
    {
        shared_ptr<std::string> path = NdnRtcNamespace::buildPath(false, NULL);
        EXPECT_EQ("",*(path.get()));
    }
    {
        shared_ptr<std::string> path = NdnRtcNamespace::buildPath(true, NULL);
        EXPECT_EQ("",*(path.get()));
    }
    {
        shared_ptr<std::string> path = NdnRtcNamespace::buildPath(true, &hub, &producerId, &stream, NULL);
        char str[256];
        
        memset(str, 0, 256);
        sprintf(str, "/%s/%s/%s",hub.c_str(), producerId.c_str(), stream.c_str());
        
        EXPECT_EQ(str,*(path.get()));
    }
    {
        shared_ptr<std::string> path = NdnRtcNamespace::buildPath(false, &hub, &producerId, &stream, NULL);
        char str[256];
        
        memset(str, 0, 256);
        sprintf(str, "%s/%s/%s",hub.c_str(), producerId.c_str(), stream.c_str());
        
        EXPECT_EQ(str,*(path.get()));
    }
    
}

TEST(NdnRtcNamespace, TestUserPrefix)
{
    std::string producerId = "producer1";
    std::string hub = NDN_COMMON_HUB;
    std::string stream = "video0";
    
    {
        shared_ptr<std::string> path = NdnRtcNamespace::getProducerPrefix(hub, producerId);
        char str[256];
        
        memset(str, 0, 256);
        sprintf(str, "/%s/%s/%s/%s",
                hub.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentApp.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentUser.c_str(),
                producerId.c_str());
        
        EXPECT_EQ(str,*(path.get()));
    }
}

TEST(NdnRtcNamespace, TestStreamPath)
{
    std::string producerId = "producer1";
    std::string hub = NDN_COMMON_HUB;
    std::string stream = "video0";
    
    {
        shared_ptr<std::string> path = NdnRtcNamespace::getStreamPath(hub, producerId, stream);
        char str[256];
        
        memset(str, 0, 256);
        sprintf(str, "/%s/%s/%s/%s/%s/%s",hub.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentApp.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentUser.c_str(),
                producerId.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentUserStreams.c_str(),
                stream.c_str());
        
        EXPECT_EQ(str,*(path.get()));
    }
}

TEST(NdnRtcNamespace, TestKeyPath)
{
    std::string producerId = "producer1";
    std::string hub = NDN_COMMON_HUB;
    std::string stream = "video0";
    
    {
        shared_ptr<std::string> producerPref = NdnRtcNamespace::getProducerPrefix(hub, producerId);
        shared_ptr<Name> keyPrefix = NdnRtcNamespace::keyPrefixForUser(*producerPref);
        char str[256];
        
        memset(str, 0, 256);
        
        sprintf(str, "/%s/%s/%s/%s/%s/%s",hub.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentApp.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentUser.c_str(),
                producerId.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentStreamKey.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceKeyComponent.c_str());
        LOG_TRACE("%s",keyPrefix->toUri().c_str());
        EXPECT_STREQ(str,keyPrefix->toUri().c_str());
    }
}

TEST(NdnRtcNamespace, TestCertPath)
{
    std::string producerId = "producer1";
    std::string hub = NDN_COMMON_HUB;
    std::string stream = "video0";
    
    {
        shared_ptr<Name> keyPrefix = NdnRtcNamespace::certificateNameForUser(*NdnRtcNamespace::getProducerPrefix(hub, producerId));
        char str[256];
        
        memset(str, 0, 256);
        
        sprintf(str, "/%s/%s/%s/%s/%s/%s/%s",hub.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentApp.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentUser.c_str(),
                producerId.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceComponentStreamKey.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceKeyComponent.c_str(),
                NdnRtcNamespace::NdnRtcNamespaceCertificateComponent.c_str());
        LOG_TRACE("%s",keyPrefix->toUri().c_str());
        EXPECT_STREQ(str,keyPrefix->toUri().c_str());
    }
}

TEST(NdnRtcNamespace, TestNumberComponent)
{
    {
        char *number = "10";
        long unsigned int num = atoi(number);
        
        shared_ptr<const std::vector<unsigned char>> comp = NdnRtcNamespace::getNumberComponent(num);
        
        for (int i = 0; i < strlen(number); i++)
            EXPECT_EQ(number[i], (*comp)[i]);
    }
    {
        char *number = "0";
        long unsigned int num = atoi(number);
        
        shared_ptr<const std::vector<unsigned char>> comp = NdnRtcNamespace::getNumberComponent(num);
        
        for (int i = 0; i < strlen(number); i++)
            EXPECT_EQ(number[i], (*comp)[i]);
    }
    {
        char *number = "1000000";
        long unsigned int num = atoi(number);
        
        shared_ptr<const std::vector<unsigned char>> comp = NdnRtcNamespace::getNumberComponent(num);
        
        for (int i = 0; i < strlen(number); i++)
            EXPECT_EQ(number[i], (*comp)[i]);
    }
    { // check max
        char *number = "18446744073709551615";
        long unsigned int num = (unsigned long)-1;
        
        shared_ptr<const std::vector<unsigned char>> comp = NdnRtcNamespace::getNumberComponent(num);
        
        for (int i = 0; i < strlen(number); i++)
            EXPECT_EQ(number[i], (*comp)[i]);
    }
}