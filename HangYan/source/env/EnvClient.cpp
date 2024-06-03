//
// Created by xiong on 2022/7/9.
//

#include "EnvClient.hpp"

using namespace Emx;


ErrCodeE EnvClient::Load() {
    try {
        Json::Value json;
        ErrCodeE e = Get(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param get ovd client failed\n");
            return e;
        }
        memset(&param, 0, sizeof(param));
        //netParam begin
        auto &netParam = param.netParam;
        auto &netParamJson = json["netParam"];
        strncpy(netParam.passDomain, netParamJson["passDomain"].asCString(), sizeof(netParam.passDomain));
        netParam.passPort = netParamJson["passPort"].asInt();
        netParam.Secure_PassPort = netParamJson["Secure_PassPort"].asInt();
        strncpy(netParam.p2p_passDomain, netParamJson["p2p_passDomain"].asCString(), sizeof(netParam.p2p_passDomain));
        netParam.p2p_passPort = netParamJson["p2p_passPort"].asInt();
        strncpy(netParam.turnDomain, netParamJson["turnDomain"].asCString(), sizeof(netParam.turnDomain));
        netParam.turnPort = netParamJson["turnPort"].asInt();
        strncpy(netParam.probeDomain, netParamJson["probeDomain"].asCString(), sizeof(netParam.probeDomain));
        netParam.probePort = netParamJson["probePort"].asInt();
        netParam.probeSecurePort = netParamJson["probeSecurePort"].asInt();
        strncpy(netParam.hibernationDomain, netParamJson["hibernationDomain"].asCString(),
                sizeof(netParam.hibernationDomain));
        netParam.hibernationPort = netParamJson["hibernationPort"].asInt();
        netParam.hibernationHBInterval = netParamJson["hibernationHBInterval"].asInt();
        netParam.maxP2PSession = netParamJson["maxP2PSession"].asInt();
        //netParam end

        param.enable_servicschedule = json["enable_servicschedule"].asInt();
        strncpy(param.servicescheduleurl, json["servicescheduleurl"].asCString(), sizeof(param.servicescheduleurl));
        strncpy(param.cafilepath, json["cafilepath"].asCString(), sizeof(param.cafilepath));
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("load ovd client failed\n");
        return ErrCodeE::Failure;
    }
}