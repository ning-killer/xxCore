//
// Created by tfh on 2021/9/15.
//
#include "andlink_adapt.h"
#include "EmxCore.hpp"

using namespace Emx;

static andlink_attr_t g_andlinkAttrInfo = {};
static int andlink_init_flag = 0;

char *andlink_getDevExtInfo(andlink_dm_t *info) {
    //TODO
#ifdef APP_USING_ANDLINK_CJSON
    // 创建一个root对象
    cJSON *root = adl_cJSON_CreateObject();
    if (nullptr == root) {
        return nullptr;
    }
    // 1.添加cmei,真实mac,sn,操作系统信息
    adl_cJSON_AddStringToObject(root, (char *) "cmei", info->cmei);    // 必填,设备唯一标识
    adl_cJSON_AddStringToObject(root, (char *) "mac", info->mac);        // 必填,设备真实MAC,全大写不带冒号
    adl_cJSON_AddStringToObject(root, (char *) "sn", info->sn);                // 必填,设备真实SN
    adl_cJSON_AddStringToObject(root, (char *) "OS", info->OS);    // 必填,操作系统(包含版本号)
    // 2.添加一机一密相关信息

    adl_cJSON_AddNumberToObject(root, (char *) "authMode", info->authMode);        //必填, 0 一型一密;1 一机一密
    adl_cJSON_AddStringToObject(root, (char *) "authId", info->authId);    //一机一密必填,智能设备一机一密安全认证的唯一标识,数字家庭合作伙伴门户申请
    adl_cJSON_AddStringToObject(root, (char *) "authKey", info->authKey);    //一机一密必填,智能设备一机一密安全认证的预置秘钥,数字家庭合作伙伴门户申请

    // 3.添加保留字段及设备生产日期信息
    adl_cJSON_AddStringToObject(root, (char *) "reserve", info->reserve);            // 可选,厂商特属标记字段
    adl_cJSON_AddStringToObject(root, (char *) "manuDate", info->manuDate);        // 必填,设备生产日期,格式为:年-月

    // 4. 添加各个芯片信息
    //root对象中添加名为chips的一个数组
    cJSON *array_chips = adl_cJSON_CreateArray();
    if (array_chips == nullptr) {
        emxloge("array_chips == nullptr");
        return nullptr;
    }
    adl_cJSON_AddItemToObject(root, (char *) "chips", array_chips);
    cJSON *chipObj = adl_cJSON_CreateObject();
    if (chipObj == nullptr) {
        emxloge("chipObj == nullptr");
        return nullptr;
    }
    //芯片1的信息
    {
        // array_chips数组内添加一个对象chipObj
        adl_cJSON_AddItemToArray(array_chips, chipObj);
        // chipObj对象内添加三个条目(芯片类型,芯片厂商,芯片型号)
        adl_cJSON_AddStringToObject(chipObj, (char *) "type", info->chips_type);        //必填
        adl_cJSON_AddStringToObject(chipObj, (char *) "factory", info->chips_factory);    //必填
        adl_cJSON_AddStringToObject(chipObj, (char *) "model", info->chips_model); //必填
    }


    char *s_extInfo = adl_cJSON_PrintUnformatted(root);
    emxlogi("s_extInfo=%s\n", s_extInfo);
    // 注意s_extInfo这块内存在sdk运行期间,不允许释放;SDK若有需要会自己释放;
    adl_cJSON_Delete(root);
    return s_extInfo;

#else
    static char s_extInfo[1024] = {0};
    char *ps_extInfo = s_extInfo;
    // 填写设备生产日期,OS信息,各个芯片信息等等
    int cat_len = sprintf(ps_extInfo,"{\"cmei\":\"%s\",",info->cmei);
    cat_len += sprintf(ps_extInfo+cat_len,"\"authMode\":%d,",info->authMode);
    cat_len += sprintf(ps_extInfo+cat_len,"\"authId\":\"%s\",",info->authId);
    cat_len += sprintf(ps_extInfo+cat_len,"\"authKey\":\"%s\",",info->authKey);
    cat_len += sprintf(ps_extInfo+cat_len,"\"mac\":\"%s\",",info->mac);
    cat_len += sprintf(ps_extInfo+cat_len,"\"sn\":\"%s\",",info->sn);
    cat_len += sprintf(ps_extInfo+cat_len,"\"reserve\":\"%s\",",info->reserve);
    cat_len += sprintf(ps_extInfo+cat_len,"\"manuDate\":\"%s\",",info->manuDate);
    cat_len += sprintf(ps_extInfo+cat_len,"\"OS\":\"%s\",",info->OS);
    cat_len += sprintf(ps_extInfo+cat_len,"\"chips\":[{");
        cat_len += sprintf(ps_extInfo+cat_len,"\"type\":\"%s\",",info->chips_type);
        cat_len += sprintf(ps_extInfo+cat_len,"\"factory\":\"%s\",",info->chips_factory);
        cat_len += sprintf(ps_extInfo+cat_len,"\"model\":\"%s\"",info->chips_model);
    cat_len += sprintf(ps_extInfo+cat_len,"}]}");

    return s_extInfo;
#endif
}

// 控制WIFI(连接、断开热点;启动、关闭热点)
int ak_ctrl_wifi_callback(WIFI_CTRL_OPT_e opt, wifi_cfg_info_t *wificfg, char *outMsg, int msgBufSize) {
    emxlogi("ak_ctrl_wifi_callback:opt=%d,msgBufSize=%d\n", opt, msgBufSize);
    return 0;
}

// 通知设备状态
int ak_set_led_callback(ADL_DEV_STATE_e state) {
    emxlogi("ak_set_led_callback,state=%d\n", state);
    switch (state) {
        case ADL_BOOTSTRAP:
            emxlogi("========demo andlink bootstrap!\r\n");
            break;
        case ADL_BOOTSTRAP_SUC:
            emxlogi("========demo andlink bootstrap SUC!\r\n");
            break;
        case ADL_BOOT:
            emxlogi("========demo andlink boot!\r\n");
            break;
        case ADL_BOOT_SUC:
            emxlogi("========demo andlink boot SUC!\r\n");
            break;
        case ADL_ONLINE:
            emxlogi("========demo andlink online!\r\n");
            break;
        default:
            break;
    }
    return 0;
}

typedef struct {
    int funcIndex;
    char *function;
} FUNCTION_INDEX_MAP_T;
// 设备支持的下行管控功能列表
FUNCTION_INDEX_MAP_T g_adlFunctionSets[] = {
        {0, (char *) "Control"},
        {1, (char *) "Unbind"},
        {2, (char *) "SelfDetect"},
        {3, (char *) "Reboot"}
};

// 获取管控功能索引
static int checFunctionIndex(char *function, int *outIndex) {
    int i = 0;
    if (nullptr == function || 0 == strlen(function)) {
        return -1;
    }
    char tmpFunc[16] = {0};
    strncpy(tmpFunc, function, sizeof(tmpFunc) - 1);
    for (i = 0; i < (int) sizeof(g_adlFunctionSets) / (int) sizeof(FUNCTION_INDEX_MAP_T); ++i) {
        if (0 == strcmp(g_adlFunctionSets[i].function, tmpFunc)) {
            *outIndex = g_adlFunctionSets[i].funcIndex;
            return 0;
        }
    }
    return -1;
}

// 下行管控
int ak_dn_send_cmd_callback(RESP_MODE_e mode, dn_dev_ctrl_frame_t *ctrlFrame, char *eventType, char *respData,
                            int respBufSize) {
    emxlogi("ak_dn_send_cmd_callback(%s),mode=%d(0:no; 1:async; 2:sync;)\n", ctrlFrame->function, mode);
    int funcIndex = 0;
    char *localEventType = nullptr;
    char localResp[256] = {0};
    // 处理管控指令
    if (0 != checFunctionIndex(ctrlFrame->function, &funcIndex))
        return -1;

    switch (funcIndex) {
        case 0: // Control
        {
            emxlogi("andlink: Control\n");
            localEventType = (char *) "ParamChange";
            break;
        }
        case 1: // Unbind//解绑处理
        {
            emxlogi("andlink: Unbind\n");
            break;
        }
        case 2: // SelfDetect//自检
        {
            emxlogi("andlink: SelfDetect\n");
            char *format = (char *) "{\"cpuRate\":%d,\"ramRate\":%d,\"upLinkType\":\"%s\",\"rssi\":\"%s\"}";
            snprintf(localResp, sizeof(localResp) - 1, format, 30, 30, "WiFi", "-45");
            localEventType = (char *) "SelfDetect";
            break;
        }
        case 3: // Reboot//重启
        {
            emxlogi("andlink: Reboot\n");
            //指定设备重启
            break;
        }
        default:
            return -1;
    }

    //进行回应
    switch (mode) {
        case ASYNC_MODE: {
            //异步响应
            if (nullptr != ctrlFrame->data && nullptr != localEventType) {
                devDataReport(nullptr, localEventType, ctrlFrame->seqId, ctrlFrame->data, strlen(ctrlFrame->data));
            }
            break;
        }
        case SYNC_MODE: {
            //同步响应
            if (nullptr != eventType && nullptr != localEventType) {
                strncpy(eventType, localEventType, sizeof(ctrlFrame->function) - 1);
            }
            if (nullptr != respData) {
                if (strlen(localResp)) {
                    strncpy(respData, localResp, respBufSize);
                } else {
                    strncpy(respData, ctrlFrame->data, respBufSize);
                }
                emxlogi("sync response=type:%s,data:%s,datalen=%d\n", eventType, respData,
                       (int) strlen(respData));
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

// 通知设备参数同步;sdk上线成功后调用
int ak_dev_paramsSync_callback() {
    emxlogi("ak_dev_paramsSync_callback\n");
#if 1// 参考代码
    char data[1024] = {0};
    //举例1的data格式: 设备主动上报版本号的格式
    char *format1 = (char *) "{\"params\": [{\"paramCode\": \"softVersion\",\"paramValue\": \"V1.1\"}]}";
//    //举例2的data格式: 单端口单属性的灯设备 上报状态开
//    char *format2 = "{\"params\": [{\"paramCode\": \"powerStatus\",\"paramValue\": \"1\"}]}";
//    //举例3的data格式: 单端口的2个属性状态同时上报
//    char *format3 = "{\"params\": [ {\"paramCode\": \"powerStatus\",\"paramValue\": \"1\"},{\"paramCode\": \"brightness\",\"paramValue\": \"0\"} ]}";
//    //举例4的data格式: 多端口插排设备 上报状态; 上报第一个插孔状态;
//    char *format4 = "{\"params\": [{\"paramCode\": \"powerStatus\",\"paramIndex\": \"1\",\"paramValue\": \"1\"}]}";
//    //举例5的data格式: 多端口插排设备 上报状态; 上报第一个插孔状态和第二个插孔状态;
//    char *format5 = "{\"params\": [{\"paramCode\": \"powerStatus\",\"paramIndex\": \"1\",\"paramValue\": \"1\"},{\"paramCode\": \"powerStatus\",\"paramIndex\": \"2\",\"paramValue\": \"1\"}]}";
//    //举例6的data格式: 单端口设备的多个属性同时上报
    /*
    {"params":[
       {"paramCode":"softVersion","paramValue":"1.10.0009"},
       {"paramCode":"firmware","paramValue":"1.10.0009"},
       {"paramCode":"STATE","paramValue":"1"},
       {"paramCode":"brightness","paramValue":"0"},
       {"paramCode":"powerStatus","paramValue":"0"},
       {"paramCode":"lightMode","paramValue":"0"},
       {"paramCode":"countDown","paramValue":"0"},
       {"paramCode":"AISwitch","paramValue":"1"},
       {"paramCode":"delaySwitch","paramValue":"0"},
       {"paramCode":"DelaySwitch","paramValue":"0"},
       {"paramCode":"strongLightProtect","paramValue":"0"}
       ]}
    */

    char *format = format1;
    snprintf(data, sizeof(data) - 1, "%s", format);

    devDataReport(nullptr, (char *) "Inform", nullptr, data, strlen(data));
#endif

    return 0;
}

// OTA1:下载并升级版本,childDevId为空表示父设备,否则表示子设备
int ak_download_upgrade_version_callback(char *childDeviceID, char *downloadurl, char *filetype, int chkfilesize) {
    emxlogi("download_upgrade_version_callback\n");
    if (childDeviceID) {
        emxloge("download_upgrade_version_callback childDeviceID != nullptr\n");
        return -1;
    }
    //------------下载处理------------
    //1.上报下载进度:开始
    char data[64];
    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data) - 1, "{\"respCode\":2002,\"respCont\":\"%d\"}", 1);
    devDataReport(nullptr, (char *) "File", nullptr, data, strlen(data));

    //2.从downloadrul下载版本


    //3.上报下载进度:完成
    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data) - 1, "{\"respCode\":2002,\"respCont\":\"%d\"}", 100);
    devDataReport(nullptr, (char *) "File", nullptr, data, strlen(data));

    //------------升级处理------------
    //4.上报升级进度:开始
    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data) - 1, "{\"respCode\":2000,\"respCont\":\"%d\"}", 1);
    devDataReport(nullptr, (char *) "File", nullptr, data, strlen(data));

    //5.执行升级操作

    //6.上报升级进度:完成
    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data) - 1, "{\"respCode\":2000,\"respCont\":\"%d\"}", 100);
    devDataReport(nullptr, (char *) "File", nullptr, data, strlen(data));
    return 0;
}

// OTA2:设备仅升级版本,childDevId为空表示父设备,否则表示子设备;实现可选
int ak_upgrade_version_callback(char *childDevId, char *filename, char *filetype) {
    emxlogi("ak_upgrade_version_callback\n");
    return 0;
}

// 获取设备IP;sdk以此判断设备是否联网
int ak_get_device_ipaddr(char *ip, char *broadAddr) {
    emxlogd("ak_get_device_ipaddr\n");
    if (nullptr != ip) {
        strcpy(ip, g_andlinkAttrInfo.dm_info.deviceIP);
        if (strlen(ip) > 0)
            emxlogi("ip=%s\n", ip);
    }
    if (nullptr != broadAddr) {
        emxlogi("broadAddr=%s\n", ip);
    }
    return 0;
}

// 复位设备IP
int ak_reset_device_Ipaddr() {
    emxlogi("ak_reset_device_Ipaddr\n");
    return 0;
}

//  SDK以24小时为周期调用,查询设备管理信息,替代之前中移终端公司DM功能
int ak_get_dmInfo_callback(char *childDeviceId, cJSON *root) {
    emxlogi("ak_get_dmInfo_callback\n");
    if (nullptr == root) {
        emxloge("ak_get_dmInfo_callback nullptr == root\n");
        return -1;
    }

    if (childDeviceId) {
        emxloge("ak_get_dmInfo_callback childDeviceId != nullptr\n");
        return -1;
    }

#ifdef APP_USING_ANDLINK_CJSON
    // 1.添加cmei,真实mac,sn,操作系统信息
    adl_cJSON_AddStringToObject(root, (char *) "cmei", g_andlinkAttrInfo.dm_info.cmei);    // 必填
    adl_cJSON_AddStringToObject(root, (char *) "mac", g_andlinkAttrInfo.dm_info.mac);        // 必填,设备真实MAC,全大写不带冒号
    adl_cJSON_AddStringToObject(root, (char *) "sn", g_andlinkAttrInfo.dm_info.sn);    // 必填,设备真实SN
    adl_cJSON_AddStringToObject(root, (char *) "OS", g_andlinkAttrInfo.dm_info.OS);    // 必填,操作系统(包含版本号)

    // 2产品依赖的众多中移插件版本信息
    cJSON *andVersionObj = adl_cJSON_CreateObject();
    if (andVersionObj == nullptr) {
        emxloge("andVersionObj == nullptr\n");
        return -1;
    }
    adl_cJSON_AddItemToObject(root, (char *) "cmccVersion", andVersionObj);
    adl_cJSON_AddStringToObject(andVersionObj, (char *) "andimsVersion",
                                (char *) VoipVersion);              // 和家固话 SDK,若使用必填;
    adl_cJSON_AddStringToObject(andVersionObj, (char *) "anddotVersion", (char *) OvdVersion);
    if (strcmp(AndLinkVersion, "1.5.1") == 0) {
        adl_cJSON_AddStringToObject(andVersionObj, (char *) "andlinkVersion",
                                    (char *) "1.5.1");              // 和家语音交互 SDK,若使用必填;
        adl_cJSON_AddStringToObject(andVersionObj, (char *) "anddmVersion",
                                    (char *) "1.0.1");              // 和家语音交互 SDK,若使用必填;
        //adl_cJSON_AddStringToObject(andVersionObj, "andfacerecVersion", "v1.0");          // 和家人脸识别 SDK,若使用必填;
    }

    // 3.添加DM设备信息
    adl_cJSON_AddStringToObject(root, (char *) "cpuModel", g_andlinkAttrInfo.dm_info.chips_model);//若使用必填,处理器型号
    adl_cJSON_AddStringToObject(root, (char *) "romStorageSize",
                                g_andlinkAttrInfo.dm_info.romSize);    //若使用必填,设备的存储总容量（ROM）大小，与工信部登记终端ROM信息一致
    adl_cJSON_AddStringToObject(root, (char *) "ramStorageSize",
                                g_andlinkAttrInfo.dm_info.ramSize);    //若使用必填,设备的内存总容量（RAM）大小，与工信部登记争端RAM信息一致
    //必填,网络类型分RJ45（有线）,wifi, 5G, 4G, 3G, NB, ZigBee等
    if (g_andlinkAttrInfo.cfgNetMode == NETWOKR_MODE_WIRED) {
        adl_cJSON_AddStringToObject(root, (char *) "networkType", (char *) "RJ45");
    } else if (g_andlinkAttrInfo.cfgNetMode == NETWOKR_MODE_WIFI) {
        adl_cJSON_AddStringToObject(root, (char *) "networkType", (char *) "wifi");
    }
    // 设备当前位置，前面表示经度、后面表示维度可选（支持定位设备必填）,第三个参数取值:1：GPS; 2：北斗; 4：伽利略; 8：格洛纳斯; 16:基站定位; 32：WiFi定位;  协同定位：GPS+北斗,值为1|2=3
//    adl_cJSON_AddStringToObject(root, "locationInfo", "123.52958679200002,25.77701556036132,1");//可选
    adl_cJSON_AddStringToObject(root, (char *) "deviceVendor", g_andlinkAttrInfo.deviceVendor);        // 必填,设备制造商
    adl_cJSON_AddStringToObject(root, (char *) "deviceBrand", g_andlinkAttrInfo.deviceBrand);        // 必填,设备品牌
    adl_cJSON_AddStringToObject(root, (char *) "deviceModel", g_andlinkAttrInfo.deviceModel);  // 必填,设备型号

    if (g_andlinkAttrInfo.cfgNetMode == NETWOKR_MODE_WIFI) {
        adl_cJSON_AddStringToObject(root, (char *) "wlanMac",
                                    g_andlinkAttrInfo.dm_info.mac);   // 可选,设备的WLAN MAC地址,WiFi接入设备必填，全大写不带冒号
        adl_cJSON_AddStringToObject(root, (char *) "wifiRssi", (char *) "-60");            //  可选,wifi信号场强WiFi接入设备必填
        // 3.添加DM模组信息,根据实际情况可选
        //root对象中添加名为moduleInfo的一个数组
        cJSON *array_moduleInfo = adl_cJSON_CreateArray();
        if (array_moduleInfo == nullptr) {
            emxloge("array_moduleInfo == nullptr\n");
            return -1;
        }
        adl_cJSON_AddItemToObject(root, (char *) "moduleInfo", array_moduleInfo);
        // 模组1:wifi模组信息
        cJSON *wifiModuleObj = adl_cJSON_CreateObject();
        if (wifiModuleObj == nullptr) {
            emxloge("wifiModuleObj == nullptr\n");
            return -1;
        }
        {
            // array_moduleInfo数组内添加一个对象wifiModuleObj
            adl_cJSON_AddItemToArray(array_moduleInfo, wifiModuleObj);
            // wifiModuleObj对象内添加1个条目
            adl_cJSON_AddStringToObject(wifiModuleObj, (char *) "moduleType",
                                        (char *) "WiFi");                // 模组类型, NB、WiFi、Zigbee、Bluetooth、Thread、lora、ZWAVE等
            adl_cJSON_AddStringToObject(wifiModuleObj, (char *) "moduleVendor", (char *) "LB-LINK");            // 模组厂商名
            adl_cJSON_AddStringToObject(wifiModuleObj, (char *) "moduleBrand",
                                        (char *) "Realtek");                // 模组品牌名
            adl_cJSON_AddStringToObject(wifiModuleObj, (char *) "moduleModel",
                                        (char *) "RTL8188FTV");                // 模组型号
            adl_cJSON_AddStringToObject(wifiModuleObj, (char *) "moduleFirmwareVersion", (char *) "v1.5");    // 模组固件版本
            adl_cJSON_AddStringToObject(wifiModuleObj, (char *) "moduleSystemVersion",
                                        (char *) "v5.4");        // 模组系统版本
            adl_cJSON_AddStringToObject(wifiModuleObj, (char *) "moduleMacAddress",
                                        g_andlinkAttrInfo.dm_info.mac);        // 模组MAC地址 全大写不带冒号
            adl_cJSON_AddStringToObject(wifiModuleObj, (char *) "moduleWlanMac",
                                        g_andlinkAttrInfo.dm_info.mac);            // 模组采集的设备Wlan MAC地址
        }
    }
    //电池供电:battery;   POE供电:POE;   市电:220V(110V);   USB供电:USB;   其他方式:other
    adl_cJSON_AddStringToObject(root, (char *) "powerSupplyMode",
                                g_andlinkAttrInfo.dm_info.powerSupplyMode);    //  必填,供电方式
    adl_cJSON_AddStringToObject(root, (char *) "deviceIP", g_andlinkAttrInfo.dm_info.deviceIP); //   IP设备必填,设备IP

    // 4.添加版本信息
    // 4.1产品本身版本信息
    adl_cJSON_AddStringToObject(root, (char *) "firmwareVersion", g_andlinkAttrInfo.firmWareVersion);
    adl_cJSON_AddStringToObject(root, (char *) "softwareVersion", g_andlinkAttrInfo.softWareVersion);

    // AHS中间件版本,必填;	组网设备:AHS-NET-1.0; 机顶盒:AHS-STB-1.0; 摄像头:AHS-IPC-1.0; 其他智能设备:AHS-DEV-1.0
    adl_cJSON_AddStringToObject(andVersionObj, (char *) "AHSVersion", (char *) "AOS-IPC-Linux V1.0.0");

    //添加其他扩展字段
    cJSON *dmExt = adl_cJSON_CreateObject();
    if (dmExt == nullptr) {
        emxloge("dmExt == nullptr\n");
        return -1;
    }
    adl_cJSON_AddItemToObject(root, (char *) "deviceManageExtInfo", dmExt);//必填

#endif
    return 0;
}


/*
 *andlink 接口初始化并运行
 * */
int andlink_adapt_init(andlink_attr_t *pAttrInfo) {
    memcpy(&g_andlinkAttrInfo, pAttrInfo, sizeof(andlink_attr_t));
    // 设置设备andlink基本信息
    static adl_dev_attr_t devAttr = {};
    devAttr.cfgNetMode = g_andlinkAttrInfo.cfgNetMode;
    devAttr.deviceVendor = g_andlinkAttrInfo.deviceVendor;
    devAttr.deviceType = g_andlinkAttrInfo.deviceType;
    devAttr.deviceMac = g_andlinkAttrInfo.deviceMac;
    devAttr.andlinkToken = g_andlinkAttrInfo.andlinkToken;
    devAttr.productToken = g_andlinkAttrInfo.productToken;
    devAttr.firmWareVersion = g_andlinkAttrInfo.firmWareVersion;
    devAttr.softWareVersion = g_andlinkAttrInfo.softWareVersion;
    devAttr.cfgPath = g_andlinkAttrInfo.cfgPath;

    emxlogd("\n*****andlink_adapt_init****\n");
    emxlogd("cfgNetMode:%d\n", devAttr.cfgNetMode);
    emxlogd("deviceVendor:%s\n", devAttr.deviceVendor);
    emxlogd("deviceType:%s\n", devAttr.deviceType);
    emxlogd("deviceMac:%s\n", devAttr.deviceMac);
    emxlogd("andlinkToken:%s\n", devAttr.andlinkToken);
    emxlogd("productToken:%s\n", devAttr.productToken);
    emxlogd("firmWareVersion:%s\n", devAttr.firmWareVersion);
    emxlogd("softWareVersion:%s\n", devAttr.softWareVersion);
    emxlogd("cfgPath:%s\n", devAttr.cfgPath);

    //下列属性,若有需要可进行动态更新
    devAttr.extInfo = andlink_getDevExtInfo(&g_andlinkAttrInfo.dm_info);
    // 设置andlink回调接口
    static adl_dev_callback_t devCbs =
            {
                    .ctrl_wifi_callback               = ak_ctrl_wifi_callback,
                    .set_led_callback                   = ak_set_led_callback,
                    .dn_send_cmd_callback               = ak_dn_send_cmd_callback,
                    .dev_paramsSync_callback           = ak_dev_paramsSync_callback,
                    .download_upgrade_version_callback = ak_download_upgrade_version_callback,
                    .upgrade_version_callback          = ak_upgrade_version_callback,
                    .get_device_ipaddr                   = ak_get_device_ipaddr,
                    .reset_device_Ipaddr               = ak_reset_device_Ipaddr,
                    .getCfg_callback = nullptr,
                    .setCfg_callback = nullptr,
                    .get_dmInfo_callback               = ak_get_dmInfo_callback
            };
    emxlogi("andlink_init========start\n");

    //设置andlink 单个日志文件使用的空间最大是500KB,若不设置此接口,默认是500KB*2;
    setAndlinkLogMaxSize(0x40000);

    //设置升级文件存储路径及文件扩展名
    setUpgradeFileStoragedPath((char *) "/tmp/rom.bin");

    //设置升级文件扩展名,如"xxx.bin","xxx.upz"
    setUpgradeFilenameExtension((char *) "rom.bin");

    //若是扫码绑定的设备,可以关闭APP发现服务;
    disableAdlFunc(ADL_APP_SEARCH_SERVICE);

    // logTo="file" 表示记录到文件;="terminal"表示记录到控制台
    set_printLog_debug_level(16, (char *) "terminal");

    // 若存在离线解绑后,需要立马绑定的场景时,可以禁止自动默认用户注册功能;
    //disableAdlFunc(ADL_OFFLINE_UNBIND_AUTO_REBOOTSTRAP);
    andlink_init_flag = 1;
    /*
    启动andlink SDK(立即返回,内部不会阻塞)
    注意:sdk不会申请内存存储devAttr和devCbs,只会保存其指针;
    因此SDK运行期间不能释放这两个指针;用户调用andlink_destroy使SDK消亡时,需要释放这两个指针;
    */
    return andlink_init(&devAttr, &devCbs);
}

/*
 *在恢复出厂或重新配网时需要调用
 * */
int andlink_adapt_reset() {
    if (andlink_init_flag) {
        return devReset();
    }
    return 0;
}

/*
 *若设备走扫码绑定,扫码成功后,调用此接口通知Andlink SDK 用户ID和云网关url(gwAddress2).
 * */
int andlink_adapt_setScanCode(char *userID, char *gwAddress, char *gwAddress2) {
    return setScanCodeBindConfigInfo(userID, gwAddress, gwAddress2);
}

/*
 *andlink 停止并释放资源
 * */
int andlink_adapt_uninit() {
    return andlink_destroy();
}

/*
 *andlink ip更改后要重新设置ip
 * */
int andlink_adapt_setIP(char *addr) {
    int ret = -1;
    if (addr && (strlen(addr) < 64)) {
        memset(g_andlinkAttrInfo.dm_info.deviceIP, 0, 64);
        strcpy(g_andlinkAttrInfo.dm_info.deviceIP, addr);
        ret = 0;
    }
    return ret;
}

