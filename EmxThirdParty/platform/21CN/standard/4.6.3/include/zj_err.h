#ifndef _ZJ_ERR_H_
#define _ZJ_ERR_H_

#ifdef __cplusplus
extern "C" {
#endif
 
/*
**定义服务器和sdk之间通信的错误码
*/
typedef enum enum_ZJ_ERR_TYPE{
	EN_ZJ_ERR             								= -1,	/*common err*/
	EN_ZJ_ERR_PARAM       								= -2,	/*param err*/
	EN_ZJ_ERR_NOMEM       								= -3,	/*no memory*/
	EN_ZJ_ERR_NOINIT      								= -4,	/*parameter not init*/
	EN_ZJ_ERR_NORES       								= -5, 	 /*no mutex, no thread, no file,inc*/
	EN_ZJ_ERR_OVERFLOW    								= -6, 	/*stack overflow*/
	EN_ZJ_ERR_MAGIC_N     								= -7, 	/*the heap PTR of struct is modified err */
	EN_ZJ_ERR_KEY_UNEXIST 								= -8, 	/*key not exist */
	EN_ZJ_ERR_FILE_EXIST  								= -9, 	/*key is exist */
	EN_ZJ_ERR_AUTHORITY   								= -10,	 /*No authority  */
	EN_ZJ_ERR_CLOSE       								= -11,	 /*Fun Not open  */
	EN_ZJ_ERR_SUPPORT     								= -12,	 /*Fun Not support  */
	EN_ZJ_ERR_NOADDR      								= -13,	/*addr path is err*/
	EN_ZJ_ERR_FREQUENTLY  								= -14,	/*request too many times*/
	EN_ZJ_ERR_TRYAGAIN    								= -15,	/*please try again*/
	EN_ZJ_ERR_FULL        								= -16,	/*dev list full*/
	EN_ZJ_ERR_NET         								= -80,	/*connect net err*/
	EN_ZJ_ERR_TIMEOUT     								= -83,	/*wait time out*/
	EN_ZJ_DES_CHANGE      								= -100,	/*des changed*/
	EN_ZJ_ERR_FILEEND     								= -101,	/*at the end of file*/
	EN_ZJ_ERR_FILEWAIT    								= -102, 
	EN_ZJ_ERR_EXIST       								= -110,	/*dev exist*/
	EN_ZJ_ERR_SLEEP                                     = -111, /*dev sleep*/

    EN_ZJ_ERR_OK                                  		= 0,       //请求成功
    EN_ZJ_ERR_SDK_CHARACTER                             = 1001,    //配置WIFI时，传入SSID有无效字符
    EN_ZJ_ERR_SDK_NO_WIFI_MODULE                        = 1002,    //查询WIFI列表，设备没有WIFI模块
    EN_ZJ_ERR_SDK_WIFI_ISCLOSE                          = 1003,    //查询WIFI列表，WIFI模块关闭
    EN_ZJ_ERR_SVR_SIGN_NO_SVR_DISTRI                    = 1101,    //SIGN分配服务时，没有可分配的服务
    EN_ZJ_ERR_SVR_FREQUENT_OPERATION            		= 1103,    //访问频繁
    EN_ZJ_ERR_SVR_COMPANYID_INVALID             		= 1111,    //鉴权的企业ID不存在
    EN_ZJ_ERR_SVR_COMPANY_NO_PRIVILEGE          		= 1112,    //鉴权的企业ID对应的企业被平台禁用
    EN_ZJ_ERR_SVR_APPID_INVALID                 		= 1113,    //鉴权传入的AppID在企业下不存在
    EN_ZJ_ERR_SVR_APPID_NO_PRIVILEGE            		= 1114,    //鉴权传入的AppID被企业/平台禁用
    EN_ZJ_ERR_SVR_LICENSENOTEXIST               		= 1115,    //传入授权的CTEI码没有找到
    EN_ZJ_ERR_SVR_LICENSEEXPIRE                 		= 1116,    //授权用的CTEI码已经过期
    EN_ZJ_ERR_SVR_LICENSEDISABLE                		= 1117,    //授权用的CTEI码被平台禁用
    EN_ZJ_ERR_SVR_APPHAVENOLICENSECOUNT         		= 1118,    //按量授权的AppID下无可用的授权数量
    EN_ZJ_ERR_SVR_DEVICENOTHERE                 		= 1121,    //设备登陆传入的设备ID无效，设备收到该错误码，重新注册新的ID
    EN_ZJ_ERR_SVR_DEVICEDISABLE                 		= 1122,    //该设备被平台禁用，设备无法上云
    EN_ZJ_ERR_SVR_DEVICE_VERSION_DISABLE        		= 1123,    //设备版本太旧，系统不再兼容。设备此时需要进行升级
    EN_ZJ_ERR_SVR_DEVICESVR_CALL_FAILED         		= 1131,    //设备配置管理系统访问报错
    EN_ZJ_ERR_SDK_DEVICE_NO_BUSI_CONFIG             	= 1141,    //设备尚未上传业务配置
    EN_ZJ_ERR_SDK_RESOLUTION_ABILITY_NOT_SUPPORT    	= 1142,    //设置的设备分辨率，当前设备不支持
    EN_ZJ_ERR_SDK_RESOLUTION_BUSI_NOT_SUPPORT 			= 1143,    //设置的设备分辨率，可能因业务限制不支持（授权限制）
    EN_ZJ_ERR_SDK_PTZ_ISMAX   							= 1144,    //设备执行PTZ已经转到最大值
    EN_ZJ_ERR_SDK_EXITGROUP_ERR   						= 1146,    //设备要出的组，不是设备当前的组，出组失败
    EN_ZJ_ERR_SDK_SDCARD_FORMATTING_ERR   				= 1148,    //设备格式化SD失败
    EN_ZJ_ERR_SDK_SDCARD_NOT_EXIST    					= 1149,    //格式化SD卡时，SD卡不存在
    EN_ZJ_ERR_SDK_SDCARD_WR_ERR   						= 1150,    //读写SD卡时，操作失败
    EN_ZJ_ERR_SDK_ADD_CHILD_DEVICE_TIMEOUT    			= 1151,    //添加HUB子设备时，通信超时，失败
    EN_ZJ_ERR_SDK_CHILD_DEVICE_EXIST  					= 1152,    //添加HUB子设备时，设备ID重复
    EN_ZJ_ERR_SDK_HUB_OPT_ERR 							= 1153,    //HUB发生异常，操作失败
    EN_ZJ_ERR_SDK_QUERY_RECORD_NO_PRIVILEGE  			= 1154,    //当前查询记录没有操作权限
    EN_ZJ_ERR_SVR_CLOUDSVR_CALL_FAILED    				= 1155,    //无法访问当前云存记录服务
    EN_ZJ_ERR_SDK_LOCAL_RECORD_NOT_EXIST  				= 1156,    //当前查询详情的本地记录文件已被清理
    EN_ZJ_ERR_SDK_CLOUD_FILE_EXPIRE   					= 1157,    //当前查询的云记录文件已经过期
    EN_ZJ_ERR_SVR_ACCOUNT_ISEXIST 						= 2001,    //账号已存在
    EN_ZJ_ERR_SVR_VERIFICATION_CODE_NOTEXIST  			= 2207,    //验证码不存在
    EN_ZJ_ERR_SVR_REG_OTHER_ERR   						= 2003,    //其他注册失败原因
    EN_ZJ_ERR_SVR_ACCOUNT_NOTEIXST    					= 2004,    //账号不存在
    EN_ZJ_ERR_SVR_USER_ACCOUNT_PWD_ERR    				= 2005,    //User登录账号密码错误
    EN_ZJ_ERR_SVR_VERIFY_OPENID_ERR   					= 2006,    //第三方登陆时，第三方校验失败
    EN_ZJ_ERR_SVR_ACCOUNT_FORBIDDEN   					= 2007,    //登陆时，发现账户已经被禁用
    EN_ZJ_ERR_SVR_UTOKEN_NOTEXIST 						= 2008,    //UTOKEN不存在
    EN_ZJ_ERR_SVR_SMSCODE_FREQUENT    					= 2009,    //两次获取短信验证码时间过短，返回失败
    EN_ZJ_ERR_SVR_USER_REJECT_INVITE  					= 2010,    //给用户发送分享邀请，被邀请者拒绝
    EN_ZJ_ERR_SVR_GTOKENNOTMATCH  						= 2011,    //通过Gtoken入组，Gtoken失效
    EN_ZJ_ERR_SVR_DEVICEISINGROUP 						= 2012,    //添加设备进组时失败，设备已经加入了另一个组
    EN_ZJ_ERR_SVR_USERNOPRIVILEGE 						= 2013,    //用户无操作权限
	EN_ZJ_ERR_SVR_ACCOUNT_TYPE_NOT_SUPPORT				= 2014,    //获取短信验证码时，不支持手机邮箱以外的方式获取
	EN_ZJ_ERR_SVR_ACCOUNT_FORMAT_ERR					= 2015,    //账号格式错误
	EN_ZJ_ERR_SVR_ACCOUNT_ALREADY_BIND					= 2016,    //账号已被绑定
	EN_ZJ_ERR_SVR_ACCOUNT_BIND_ERR						= 2017,    //绑定账号错误
	EN_ZJ_ERR_SVR_FILEID_NOT_EXIST						= 2018,	   //云存储文件ID不存在
	EN_ZJ_ERR_SVR_APPID_NOTFOUND_TEMPLATE            	= 2027,    //APPID未找到对应的短信/ems模板
    EN_ZJ_ERR_SVR_DEVICE_P2P_NOT_SUPPORT  				= 3001,    //当用户向设备请求P2P时，设备返回不支持P2P
    EN_ZJ_ERR_SVR_CONN_NOT_MATCH  						= 3002,    //用户与设备的P2P连接校验不匹配
    EN_ZJ_ERR_SVR_MEDIA_MATCH_TIMEOUT 					= 3003,    //用户和设备在MEDIA上配对超时
    EN_ZJ_ERR_SVR_MEDIA_CHANNEL_NOTEXIST 				= 3004,    //媒体操作时，对应的ChannelID不存在
    EN_ZJ_ERR_SVR_MEDIA_PLAY_UPPER_LIMIT				= 3005,    //点播流媒体时，人数超上限
    EN_ZJ_ERR_SVR_MEDIA_INVALID   						= 3006,    //流媒体服务时，资源过期或无效
	EN_ZJ_ERR_SVR_MALLOC_ERR		     				= 4001,		//服务申请内存对象错误
	EN_ZJ_ERR_SVR_JSON_DECODE_ERR	     				= 4002,		//服务解析JSON协议错误
	EN_ZJ_ERR_SVR_JSON_ENCODE_ERR	     				= 4003,		//服务构造JSON字符串错误
	EN_ZJ_ERR_SVR_API_PARAM_ERR		     				= 4004,		//API调用参数错误
	EN_ZJ_ERR_SVR_METHOD_ERR		     				= 4005,		//API返回NIL
	EN_ZJ_ERR_SVR_ENCRYPT_ERR		     				= 4006,		//协议加密错误
	EN_ZJ_ERR_SVR_DECRYPT_ERR		     				= 4007,		//协议解密错误
	EN_ZJ_ERR_SVR_BUFREAD_ERR		     				= 4008,		//链接读取BUFFER错误
	EN_ZJ_ERR_SVR_TLS_PEMKEY_ERR	     				= 4009,		//TLS链接找不到PEM KEY文件错误
	EN_ZJ_ERR_SVR_AUTHATOKEN_ERR	     				= 4010,		//服务器之间访问调用 ATOKEN出错
	EN_ZJ_ERR_SVR_REDIS_PUB_ERR		     				= 4011,		//Redis服务访问错误
	EN_ZJ_ERR_SVR_DB_CONNECT_ERR	     				= 4012,		//数据库连接失败
	EN_ZJ_ERR_SVR_DB_INSERT_ERR		     				= 4013,		//数据库数据插入失败
	EN_ZJ_ERR_SVR_DB_SELECT_ERR		     				= 4014,		//数据库数据查询失败
	EN_ZJ_ERR_SVR_DB_UPDATE_ERR		     				= 4015,		//数据库数据更新失败
	EN_ZJ_ERR_SVR_DB_DELETE_ERR		     				= 4016,		//数据库数据删除失败
	EN_ZJ_ERR_SVR_DB_NORECORD_ERR	     				= 4017,		//没有找到相关记录
	EN_ZJ_ERR_SVR_DB_DUPLICATE_KEY	     				= 4018,		//主键重复
	EN_ZJ_ERR_SVR_NET_LISTEN_ERR	     				= 4019,		//服务端口侦听错误
	EN_ZJ_ERR_SVR_CLOSED			     				= 4020,		//服务关闭错误
	EN_ZJ_ERR_SVR_IDSVR_NOT_FOUND	     				= 4021,		//IDSVR服务找不到
	EN_ZJ_ERR_SVR_IDSTUNSVR_NOT_FOUND	    			= 4022,		//IDSTUN服务找不到
	EN_ZJ_ERR_SVR_BUSISVR_NOT_FOUND						= 4023,		//BUSICENTRE服务找不到
	EN_ZJ_ERR_SVR_MEDIASVR_NOT_FOUND					= 4024,		//媒体服务找不到
	EN_ZJ_ERR_SVR_LINKSVR_NOT_FOUND						= 4025,		//LINK服务找不到
	EN_ZJ_ERR_SVR_USERSVR_CALL_FAILED					= 4026,		//用户系统接口调用失败
	EN_ZJ_ERR_SVR_IDSVR_CALL_FAILED						= 4027,		//IDSVR系统接口调用失败
	EN_ZJ_ERR_SVR_IDSTUN_CALL_FAILED					= 4028,		//IDSTUN系统接口调用失败
	EN_ZJ_ERR_SVR_PUSHSVR_NOT_FOUND						= 4029,		//推送服务找不到
	EN_ZJ_ERR_SVR_GATEWAYSVR_CALL_FAILED				= 4030,		//网关服务调用失败
	EN_ZJ_ERR_SVR_COMPMNGSVR_CALL_FAILED				= 4031,		//企业后台服务调用失败
	EN_ZJ_ERR_SVR_PUBSUBSVR_NOT_FOUND					= 4032,		//订阅发布服务找不到
	EN_ZJ_ERR_SVR_SYSNOTICESVR_NOT_FOUND				= 4033,		//sysnotice找不到
	EN_ZJ_ERR_SVR_GATEWAYSVR_NOT_FOUND					= 4034,		//网关服务找不到
	EN_ZJ_ERR_SVR_HTTPSVR_NOT_FOUND						= 4035,		//http服务找不到
	EN_ZJ_ERR_SVR_HTTPSVR_CALL_FAILED					= 4036,		//http服务调用失败
	EN_ZJ_ERR_SVR_BUSISVR_CALL_FAILED					= 4037,		//busicentre调用失败
	EN_ZJ_ERR_SVR_ID_CALL_FAILED						= 4038,		//IDSVR调用失败
	EN_ZJ_ERR_SVR_SIGN_CALL_FAILED						= 4039,		//SIGN服务调用失败
	EN_ZJ_ERR_SVR_LINK_CALL_FAILED						= 4040,		//Link服务调用失败
	EN_ZJ_ERR_SVR_LOGSVR_CALL_FAILED					= 4041,		//LOG系统调用失败
	EN_ZJ_ERR_SVR_USERSVR_NOT_FOUND						= 4042,		//用户系统找不到
	EN_ZJ_ERR_SVR_SYSNOTICESVR_CALL_FAILED				= 4043,		//sysnotice找不到调用失败
	EN_ZJ_ERR_SVR_PUBSUBSVR_CALL_FAILED					= 4044,		//订阅发布服务调用失败
	EN_ZJ_ERR_SVR_DYNAMICMETHOD_CALL_FAILED				= 4045,		//notice动态调用其他服务失败
	EN_ZJ_ERR_SVR_PLAYLOAD_IS_FULL						= 4046,		//负载为空
	EN_ZJ_ERR_SVR_GRPCSVR_CALL_FAILED					= 4047,		//GRPC调用失败
	EN_ZJ_ERR_SVR_DEVICESVR_NOT_FOUND					= 4048,		//设备系统位找不到
	EN_ZJ_ERR_SVR_EVENTSVR_NOT_FOUND					= 4049,		//事件系统找不到	
	EN_ZJ_ERR_SVR_ZONE_NOT_FOUND						= 4050,		//ZONE服务找不到
	EN_ZJ_ERR_SVR_NET_WRITE_BLOCK						= 4051,		//网络通道写入包失败
	EN_ZJ_ERR_SVR_SOCKET_CLOSED							= 4052,		//SOCKET连接已关闭
	EN_ZJ_ERR_SVR_SOCKET_READERR						= 4053,		//SOCKET连接读取包失败
	EN_ZJ_ERR_SVR_ID_PARSEERR							= 4054,		//ID参数解析失败(格式不争取)
	EN_ZJ_ERR_SVR_NET_DNSPARSE_ERR						= 4055,		//
	EN_ZJ_ERR_SVR_NET_REQ_TIMEOUT						= 4056,		//网络请求超时
	EN_ZJ_ERR_SVR_SUBPUB_TOKEN_NOTEXIST					= 4057,		
	EN_ZJ_ERR_SVR_SOCKET_ACCEPT_ERROR					= 4058,		
	EN_ZJ_ERR_SVR_OPERATION_TOO_FREQUENT				= 4059,		
	EN_ZJ_ERR_SVR_PARSE_IP_ERR							= 4060,		
	EN_ZJ_ERR_SVR_GETSVRPUBKEY_ERR						= 4061,		//获取pubkey
	EN_ZJ_ERR_SVR_LINKNOTICESVR_NOT_FOUND				= 4062,		//LINKNOTICE找不到
	EN_ZJ_ERR_SVR_ENCRYPTTYPE_ERROR						= 4064,		//加密类型错误
	EN_ZJ_ERR_SVR_VIDEOSVR_NOT_FOUND					= 4065,		//视频存储服务找不到
	EN_ZJ_ERR_SVR_EXCEPTIONSVR_NOT_FOUND				= 4066,		//EXCEPTION服务找不到
	EN_ZJ_ERR_SVR_LOGSVR_NOT_FOUND						= 4067,		//LOGSVR找不到
	EN_ZJ_ERR_SVR_OPRECORDSVR_NOT_FOUND					= 4068,		//OPRECORD服务找不到
	EN_ZJ_ERR_SVR_NATSTUNSVR_NOT_FOUND					= 4069,		//NATSTUN服务找不到
	EN_ZJ_ERR_SVR_COMBO_CALL_FAILED						= 4070,		//套餐服务调用失败
	EN_ZJ_ERR_SVR_COMBO_NOT_FOUND						= 4071,		//套餐服务找不到
	EN_ZJ_ERR_SVR_BINDSVR_NOT_FOUND						= 4073,		//绑定服务找不到
	EN_ZJ_ERR_SVR_PICTURESVR_NOT_FOUND					= 4074,		//图片系统找不到
	EN_ZJ_ERR_SVR_GROUPNOTHERE							= 4407,		//组不存在
	EN_ZJ_ERR_SVR_DEVICENOTINGROUP						= 4409,		//Device不在组里
	EN_ZJ_ERR_SVR_WAKEUP_DEVICEERR						= 4422,		//唤醒出错
	EN_ZJ_ERR_SVR_DEVICEOFFLINE							= 4602,		//Device不在线
	EN_ZJ_ERR_SVR_DEVICE_ISSLEEP						= 4610,		//设备已休眠
	EN_ZJ_ERR_SVR_HAVENOGROUPS							= 4612,		//没有组列表
	EN_ZJ_ERR_SVR_REG_AUTH_ERR							= 5001,		//SIGN服务注册 SVRID&SVRPWD校验错误
	EN_ZJ_ERR_SVR_SIGN_ATOKEN_NOT_EXIST					= 5002,		//SIGN服务通过SVR ATOKEN找不到服务
	EN_ZJ_ERR_SVR_SIGN_MANAGE_NOMETHOD					= 5004,		//SIGN的Manage接口method不支持
	EN_ZJ_ERR_SVR_GENERALUTOKEN_ERR						= 5005,		//BUSI生成UTOKEN错误
	EN_ZJ_ERR_SVR_UTOKEN_NOTAUTHED						= 5006,	
	EN_ZJ_ERR_SVR_USERNOTHERE							= 5007,	
	EN_ZJ_ERR_SVR_USERISINGROUP							= 5011,	
	EN_ZJ_ERR_SVR_ROLENOTEXIST							= 5013,	
	EN_ZJ_ERR_SVR_CHILDGROUPNOTEXIST					= 5014,	
	EN_ZJ_ERR_SVR_ROLEISINUSE							= 5015,	
	EN_ZJ_ERR_SVR_GROUPISINGROUP						= 5016,	
	EN_ZJ_ERR_SVR_USERNOTINGROUP						= 5017,	
	EN_ZJ_ERR_SVR_GROUPNOTINGROUP						= 5019,	
	EN_ZJ_ERR_SVR_DEVICEISSLEEP							= 5021,	
	EN_ZJ_ERR_SVR_ID_MAXIDINDEX_GETERR					= 5024,	
	EN_ZJ_ERR_SVR_APPNOTEXIST							= 5026,	
	EN_ZJ_ERR_SVR_APPDISABLE							= 5027,	
	EN_ZJ_ERR_SVR_APPAUTHTYPE_ERR						= 5031,	
	EN_ZJ_ERR_SVR_APPISEXIST							= 5032,	
	EN_ZJ_ERR_SVR_ACCOUNTTYPENOTBIND					= 5034,	
	EN_ZJ_ERR_SVR_APPAUTHTYPENOTSUPPORT					= 5036,	
	EN_ZJ_ERR_SVR_DEVICE_NOT_AUTH						= 5038,	
	EN_ZJ_ERR_SVR_UTOKEN_NOT_AUTH						= 5039,	
	EN_ZJ_ERR_SVR_DEVICE_FORBIDDEN						= 5040,	
	EN_ZJ_ERR_SVR_UTOKEN_FORBIDDEN						= 5041,	
	EN_ZJ_ERR_SVR_UTOKEN_CHECKERR						= 5042,	
	EN_ZJ_ERR_SVR_SYSCOMMAND_FAIL						= 5043,	
	EN_ZJ_ERR_SVR_PUSHTEMPLATENOTEXIST					= 5044,	
	EN_ZJ_ERR_SVR_DEVICE_LIMIT_AUTH						= 5045,	
	EN_ZJ_ERR_SVR_SIGN_SVRKEY_NOTEXIST					= 5046,	
	EN_ZJ_ERR_SVR_SVRPUBKEY_NOTEXIST					= 5047,	
	EN_ZJ_ERR_SVR_FILETYPE_NOTEXIST						= 5048,	
	EN_ZJ_ERR_SVR_SUBPUB_SUBJ_NOT_EXIST					= 8000,	
	EN_ZJ_ERR_SVR_SUBPUB_ID_TYPE_NOT_SUPPORT    		= 8001,	
	EN_ZJ_ERR_SVR_NOTICE_QUEUE_WRITE_ERR				= 8002,		//NOTICE服务写入队列失败
	EN_ZJ_ERR_SVR_NOTICE_PROTO_NOT_SUPPORT				= 8003,		//NOTICE通知协议不支持
	EN_ZJ_ERR_SVR_LANGUAGE_NOT_EXIST					= 8004,		//语言不存在
	EN_ZJ_ERR_SVR_SUBPUB_PUBLISH_MSG_ERR				= 8005,		//订阅发布服务Publish失败
	EN_ZJ_ERR_SVR_NOTICE_JUMP_NOTIE_SUPPORT				= 8006,		//NOTICE跨机房调用不支持
    EN_ZJ_ERR_SVR_SIGNAL_NOT_SUPPORT					= 9001,    //所发送的指令，对方不支持该指令集；（版本/型号错误导致）
    EN_ZJ_ERR_SVR_SERVICE_TIMEOUT						= 9002,    //指令发送后，超过超时时间仍没有回应；
    EN_ZJ_SVR_SIGNAL_ALREADY_CACHE					    = 9003,    //通过服务端转发信令，设备支持信令缓存，设备在线，则回应。
    EN_ZJ_ERR_SVR_SIGNAL_DISCARD						= 9004,    //通过服务端转发信令，不支持信令缓存，设备不在线，则回应。
    EN_ZJ_ERR_SVR_CMD_NOT_SUPPORT						= 9010,    //版本较旧，或设备类型不符合不支持当前指令。
    EN_ZJ_ERR_SVR_DEVICE_REG_FORBIDDEN					= 9030,    //设备注册无权限，或服务限制，设备不再重试
    EN_ZJ_ERR_SVR_DEVICE_LOGIN_FORBIDDEN				= 9031,    //设备登陆被禁用，设备不再重试


}EN_ZJ_ERR_TYPE;


#ifdef __cplusplus
}
#endif


#endif



