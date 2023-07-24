/*
* @Author: xiong
* @Date: 2023/2/27
*/

#include "RecordStd.hpp"

void Emx::RecordStd::Param::Serialize(std::string &out) {
    out.append(path, sizeof(path));
    out.append((char *) &reservedSpace, sizeof(reservedSpace));

    out.append((char *) &record.vencChn, sizeof(record.vencChn));
    out.append((char *) &record.aencChn, sizeof(record.aencChn));
    out.append((char *) &record.mode, sizeof(record.mode));
    out.append((char *) &record.splitTimeS, sizeof(record.splitTimeS));
    record.schedule.schedule.Serialize(out);
    out.append((char *) &record.preRecTimeS, sizeof(record.preRecTimeS));
    int eventNum = (int) record.events.size();
    out.append((char *) &eventNum, sizeof(eventNum));
    for (auto &e : record.events) {
        out.append((char *) &e, sizeof(e));
    }

    out.append((char *) &snap.chn, sizeof(snap.chn));
    out.append((char *) &snap.mode, sizeof(snap.mode));
    out.append((char *) &snap.snapInvS, sizeof(snap.snapInvS));
    snap.schedule.schedule.Serialize(out);
    eventNum = (int) snap.events.size();
    out.append((char *) &eventNum, sizeof(eventNum));
    for (auto &e : snap.events) {
        out.append((char *) &e, sizeof(e));
    }
}

void Emx::RecordStd::Param::Parse(const char *data, int size) {
    int off = 0;
    memcpy(path, data + off, sizeof(path));
    off += sizeof(path);
    memcpy(&reservedSpace, data + off, sizeof(reservedSpace));
    off += sizeof(reservedSpace);

    memcpy(&record.vencChn, data + off, sizeof(record.vencChn));
    off += sizeof(record.vencChn);
    memcpy(&record.aencChn, data + off, sizeof(record.aencChn));
    off += sizeof(record.aencChn);
    memcpy(&record.mode, data + off, sizeof(record.mode));
    off += sizeof(record.mode);
    memcpy(&record.splitTimeS, data + off, sizeof(record.splitTimeS));
    off += sizeof(record.splitTimeS);
    off += record.schedule.schedule.Parse(data + off, size - off);
    memcpy(&record.preRecTimeS, data + off, sizeof(record.preRecTimeS));
    off += sizeof(record.preRecTimeS);
    int eventNum = 0;
    memcpy(&eventNum, data + off, sizeof(eventNum));
    off += sizeof(eventNum);
    record.events.clear();
    for (int i = 0; i < eventNum; i++) {
        EventBaseItem item = {};
        memcpy(&item, data + off, sizeof(item));
        off += sizeof(item);
        record.events.emplace_back(item);
    }

    memcpy(&snap.chn, data + off, sizeof(snap.chn));
    off += sizeof(snap.chn);
    memcpy(&snap.mode, data + off, sizeof(snap.mode));
    off += sizeof(snap.mode);
    memcpy(&snap.snapInvS, data + off, sizeof(snap.snapInvS));
    off += sizeof(snap.snapInvS);
    off += snap.schedule.schedule.Parse(data + off, size - off);
    eventNum = 0;
    memcpy(&eventNum, data + off, sizeof(eventNum));
    off += sizeof(eventNum);
    snap.events.clear();
    for (int i = 0; i < eventNum; i++) {
        EventBaseItem item = {};
        memcpy(&item, data + off, sizeof(item));
        off += sizeof(item);
        snap.events.emplace_back(item);
    }
}

void Emx::RecordStd::Param::Serialize(Json::Value &json) {
    json["path"] = path;
    json["reservedSpace"] = reservedSpace;
    auto &jsonRecord = json["record"];
    jsonRecord["vencChn"] = record.vencChn;
    jsonRecord["aencChn"] = record.aencChn;
    jsonRecord["mode"] = (int) record.mode;
    jsonRecord["splitTimeS"] = record.splitTimeS;
    record.schedule.schedule.Serialize(jsonRecord["schedule"]);
    jsonRecord["preRecTimeS"] = record.preRecTimeS;
    jsonRecord["events"].resize(0);
    for (auto &event:record.events) {
        Json::Value item;
        item["type"] = (int) event.type;
        item["chn"] = event.chn;
        jsonRecord["events"].append(item);
    }
    auto &jsonSnap = json["snap"];
    jsonSnap["chn"] = snap.chn;
    jsonSnap["mode"] = (int) snap.mode;
    jsonSnap["snapInvS"] = snap.snapInvS;
    snap.schedule.schedule.Serialize(jsonSnap["schedule"]);
    jsonSnap["events"].resize(0);
    for (auto &event:snap.events) {
        Json::Value item;
        item["type"] = (int) event.type;
        item["chn"] = event.chn;
        jsonSnap["events"].append(item);
    }
}

void Emx::RecordStd::Param::Parse(const Json::Value &json) {
    strncpy(path, json["path"].asCString(), sizeof(path));
    reservedSpace = json["reservedSpace"].asInt();

    auto &jsonRecord = json["record"];
    record.vencChn = jsonRecord["vencChn"].asInt();
    record.aencChn = jsonRecord["aencChn"].asInt();
    record.mode = (ModeE) jsonRecord["mode"].asInt();
    record.splitTimeS = jsonRecord["splitTimeS"].asInt();
    record.schedule.schedule.Parse(jsonRecord["schedule"]);
    record.preRecTimeS = jsonRecord["preRecTimeS"].asInt();
    auto &eventBaseRecord = jsonRecord["events"];
    record.events.clear();
    for (auto &event:eventBaseRecord) {
        EventBaseItem item = {};
        item.type = (Event::TypeE) event["type"].asInt();
        item.chn = event["chn"].asInt();
        record.events.emplace_back(item);
    }

    auto &jsonSnap = json["snap"];
    snap.chn = jsonSnap["chn"].asInt();
    snap.mode = (ModeE) jsonSnap["mode"].asInt();
    snap.snapInvS = jsonSnap["snapInvS"].asInt();
    snap.schedule.schedule.Parse(jsonSnap["schedule"]);
    snap.events.clear();
    for (auto &event:jsonSnap["events"]) {
        EventBaseItem item = {};
        item.type = (Event::TypeE) event["type"].asInt();
        item.chn = event["chn"].asInt();
        snap.events.emplace_back(item);
    }
}
