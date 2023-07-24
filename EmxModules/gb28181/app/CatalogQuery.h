#pragma once
#include <string>
#include <vector>
#include <memory>

class CatalogItem {
public:
    std::string ToString();
public:
    std::string DeviceID;//设备/区域/系统编码
    std::string Name;//设备/区域/系统名称
    std::string Manufacturer;

    std::string Model;//当为设备时，设备型号(必选)
    std::string Owner;//当为设备时，设备归属(必选)
    std::string CivilCode;// 行政区域(必选)
    std::string Block;//警区(可选)
    std::string Address;// 当为设备时,安装地址(必选)
    int Parental;// 当为设备时,是否有子设备(必选) 1有, 0没有
    std::string ParentID;//父设备/区域/系统编码
    int RegisterWay;// 注册方式(必选)缺省为1;
    //  1:符合IETFRFC3261标准的认证注册模式;
    //  2:基于口令的双向认证注册模式;
    //  3:基于数字证书的双向认证注册模式
    std::string CertNum; // 证书序列号(有证书的设备必选)
    int Certifiable;// 证书有效标识(有证书的设备必选)缺省为0;证书有效标识:0:无效1: 有效
    int ErrCode;
    std::string EndTime;
    int Secrecy; // 保密属性(必选)缺省为0; 0:不涉密, 1:涉密
    std::string Status;//ON or OFF
    std::string IPAddress;// 设备/区域/系统IP地址(可选)
    unsigned short Port;// 设备/区域/系统端口(可选)
    std::string Password;// 设备口令(可选)
    double Longitude;// 经度(可选)
    double Latitude;// 纬度(可选)
};

class CatalogQuery
{
public:
    std::string DeviceId;
public:
    std::vector<std::shared_ptr<CatalogItem>> GetCatalog();
};

