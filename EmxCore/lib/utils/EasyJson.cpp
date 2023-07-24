//
// Created by xiong on 2022/6/9.
//

#include "core/utils/EasyJson.hpp"
#include "core/utils/File.hpp"

using namespace Emx;


ErrCodeE EasyJson::Load(const char *path, Json::Value &json) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return ErrCodeE::OpenFailed;
    }
    int size = File::Size(path);
    auto buffer = (char *) malloc(size + 1);
    if (!buffer) {
        printf("Malloc failed\n");
        fclose(fp);
        return ErrCodeE::MallocFailed;
    }
    memset(buffer, 0, size + 1);
    int n, off = 0;
    int tmp = size;
    while (tmp) {
        n = (int) fread(buffer + off, 1, tmp, fp);
        if (n <= 0)break;
        off += n;
        tmp -= n;
    }
    fclose(fp);
    if (buffer[0] != '[' && buffer[0] != '{') {
//        Aes::Do((uint8_t *) buffer, size);
    }
    ErrCodeE e = EasyJson::Parse(buffer, json);
    free(buffer);
    return e;
}

ErrCodeE EasyJson::Parse(const char *data, Json::Value &json) {
    return Parse(data, (int) strlen(data), json);
}

ErrCodeE EasyJson::Parse(const std::string &data, Json::Value &json) {
    return Parse(data.data(), (int) data.size(), json);
}

ErrCodeE EasyJson::Parse(const char *data, int size, Json::Value &json) {
    JSONCPP_STRING errs;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> const reader(builder.newCharReader());
    if (!reader->parse(data, data + size, &json, &errs)) {
        printf("Cannot parse %s : %s\n", data, errs.empty() ? "unknown" : errs.data());
        return ErrCodeE::ParseFailed;
    }
    return ErrCodeE::Success;
}

ErrCodeE EasyJson::Serialize(const Json::Value &json, std::string &jsonStr) {
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = "";
    std::ostringstream os;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(json, &os);
    jsonStr.append(os.str().data(), os.str().size());
    return ErrCodeE::Success;
}
