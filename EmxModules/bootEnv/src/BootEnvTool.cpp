//
// Created by xiong on 2022/2/27.
//
#include <stdio.h>
#include "BootEnv.hpp"

using namespace Emx;

void usage() {
    //需要指定配置文件所在目录
    printf("usage:\n");
    printf("BootEnvTool set key [value] #set one boot env, none value means delete it\n");
    printf("BootEnvTool get [key] #print boot env, you can just print one key-value by specify it's key\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage();
        return -1;
    }
    const char *cmd = argv[1];
    if (strcmp(cmd, "get") == 0) {
        if (argc == 3) {
            //获取某个key的value
            const char *key = argv[2];
            std::string value;
            if (BootEnv::GetEnv(key, value) != ErrCodeE::Success) {
                printf("get env failed\n");
                return -1;
            }
            printf("%s\n", value.c_str());
            return 0;
        } else {
            //获取所有环境变量的键值对
            BootEnv::EnvMap map;
            if (BootEnv::GetEnv(map) != ErrCodeE::Success) {
                printf("get env failed\n");
                return -1;
            }
            for (auto &m : map)
                printf("%s=%s\n", m.first.c_str(), m.second.c_str());
            return 0;
        }
    } else if (strcmp(cmd, "set") == 0) {
        if (argc == 3) {
            //删除某个key及其值
            BootEnv::SetEnv(argv[2], "");
        } else if (argc == 4) {
            //设置或创建某个key值
            BootEnv::SetEnv(argv[2], argv[3]);
        } else {
            usage();
            return -1;
        }
    } else {
        usage();
        return -1;
    }
    return 0;
}
