//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Misc.hpp"
#include "core/utils/Cmd.hpp"
#include "core/utils/Time.hpp"
#include "core/EmxLog.hpp"
#include "EmxCoreApiVersion.h"
#include <cstring>

using namespace Emx;


void Misc::BindCpu(int num) {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(num, &mask);
    if (sched_setaffinity(num, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }
}

void Misc::UIntToArray(uint32_t num, uint8_t *array) {
    array[0] = (num >> 24) & 0xff;
    array[1] = (num >> 16) & 0xff;
    array[2] = (num >> 8) & 0xff;
    array[3] = (num) & 0xff;
}

uint32_t Misc::ArrayToUint32(uint8_t *array) {
    return (array[0] & 0xff) << 24 |
           (array[1] & 0xff) << 16 |
           (array[2] & 0xff) << 8 |
           (array[3] & 0xff);
}

uint32_t Misc::GetRSS() {
    size_t size;
    uv_resident_set_memory(&size);
    return size;
}

void Misc::GetResUsage(uv_rusage_t &usage) {
    uv_getrusage(&usage);
}

bool Misc::ApplicationExist(const char *app) {
    bool find = false;
    char temp[1024] = {};
    sprintf(temp, "ps | grep %s | grep -v grep", app);
    FILE *pp = popen(temp, "r");
    if (pp) {
        memset(temp, 0, 1024);
        while (fgets(temp, 1024, pp)) {
            if (strstr(temp, app)) {
                find = true;
                break;
            }
        }
        pclose(pp);
    }
    return find;
}

void Misc::KillBeforeRun(const char *cmd) {
    auto name = (char *) cmd;
    if (strrchr(name, '/'))
        name = strrchr(name, '/') + 1;

    char temp[1024] = {};
    sprintf(temp, "ps | grep %s | grep -v grep", name);
    FILE *pp = popen(temp, "r");
    if (pp) {
        memset(temp, 0, 1024);
        while (fgets(temp, 1024, pp)) {
            auto pid = (pid_t) strtol(temp, nullptr, 10);
            if (pid > 0 && pid != getpid()) {
                Cmd::Run("kill %d", pid);
            }
        }
        pclose(pp);
    }
}

void Misc::KillBeforeRun() {
    char temp[1024] = {};
    FILE *fp = fopen("/proc/self/stat", "r");
    if (fp) {
        fread(temp, 1, 1024, fp);
        char *ps = strstr(temp, "(");
        char *pe = strstr(temp, ")");
        if (ps && pe) {
            ps++;
            char app[256] = {};
            memcpy(app, ps, pe - ps);
            KillBeforeRun(app);
        }
        fclose(fp);
    }
}

int Misc::GetRandom(int min, int max) {
    unsigned int seed = Time::GetMonoClockUs();
    int result = (rand_r(&seed) % (max - min + 1)) + min;
    if (result < min || result > max) {
        result = min;
    }
    return result;
}

const char *Misc::GetAppConfigsDir() {
    auto p = getenv("EMX_APP_CONFIGS_DIR");
    if (!p) {
        printf("can not found environment EMX_APP_CONFIGS_DIR\n");
    }
    return p;
}

const char *Misc::GetMediaConfigsDir() {
    auto p = getenv("EMX_MEDIA_CONFIGS_DIR");
    if (!p) {
        printf("can not found environment EMX_MEDIA_CONFIGS_DIR\n");
    }
    return p;
}

const char *Misc::GetDataDir() {
    auto p = getenv("EMX_DATA_DIR");
    if (!p) {
        printf("can not found environment EMX_DATA_DIR\n");
    }
    return p;
}

typedef struct CPU_PACKED         //定义一个cpu occupy的结构体
{
    char name[20];      //定义一个char类型的数组名name有20个元素
    unsigned int user; //定义一个无符号的int类型的user
    unsigned int nice; //定义一个无符号的int类型的nice
    unsigned int system;//定义一个无符号的int类型的system
    unsigned int idle; //定义一个无符号的int类型的idle
} CPU_OCCUPY;

typedef struct MEM_PACKED         //定义一个mem occupy的结构体
{
    char name[20];      //定义一个char类型的数组名name有20个元素
    unsigned long total;
    char name2[20];
    unsigned long free;
    unsigned long MemAvailable;
} MEM_OCCUPY;


int get_memoccupy(MEM_OCCUPY *mem) //对无类型get函数含有一个形参结构体类弄的指针O
{
    /*
    MemTotal:          39316 kB
    MemFree:            6484 kB
    MemAvailable:      23616 kB
    Buffers:            5308 kB
    Cached:            15440 kB
    SwapCached:            0 kB
    Active:             8040 kB
    Inactive:          12896 kB
    Active(anon):        212 kB
    Inactive(anon):        4 kB
    Active(file):       7828 kB
    Inactive(file):    12892 kB
    Unevictable:           0 kB
    Mlocked:               0 kB
    SwapTotal:             0 kB
    SwapFree:              0 kB
    Dirty:                 0 kB
    Writeback:             0 kB
    AnonPages:           212 kB
    Mapped:             1208 kB
    Shmem:                28 kB
    Slab:               4564 kB
    SReclaimable:        676 kB
    SUnreclaim:         3888 kB
    KernelStack:         488 kB
    PageTables:           40 kB
    NFS_Unstable:          0 kB
    Bounce:                0 kB
    WritebackTmp:          0 kB
    CommitLimit:       19656 kB
    Committed_AS:        636 kB
    VmallocTotal:     991232 kB
    VmallocUsed:           0 kB
    VmallocChunk:          0 kB
     */
    FILE *fd;
    char buff[256];

    MEM_OCCUPY *m;
    m = mem;
    fd = fopen("/proc/meminfo", "r");
    if (!fd) {
        emxloge("open /proc/meminfo failed\n");
        return -1;
    }
    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %lu %s", m->name, &m->total, m->name2);

    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %lu %s", m->name2, &m->free, m->name2);
#ifdef T31
    char nameTmp[20] = {0};
    unsigned long Buffers = 0, Cached = 0;
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %u %s", nameTmp, &Buffers, nameTmp);

    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%s %u %s", nameTmp, &Cached, nameTmp);
    m->MemAvailable = Cached/2+m->free+Buffers;
//    xlog(XLOG_DBG, XLOGMOD, "[%s:%d] Buffers:%lu, Cached:%lu, MemAvailable:%lu\n",
//         __FUNCTION__ , __LINE__, Buffers, Cached, m->MemAvailable);
#else
    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %lu %s", m->name2, &m->MemAvailable, m->name2);
#endif
//    printf("[%s:%d] total:%lu, free:%lu\n", __FUNCTION__ , __LINE__, m->total, m->free);

    fclose(fd);     //关闭文件fd
    return 0;
}

int get_cpuoccupy(CPU_OCCUPY *cpust) //对无类型get函数含有一个形参结构体类弄的指针O
{
    /*
    cpu  189 0 631 46548 1 0 30 0 0 0
    cpu0 189 0 631 46548 1 0 30 0 0 0
    intr 458414 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 315956 0 0 0 643 0 0 0 134 45979 0 0 0 0 0 0 0 0 0 0 0 0 0 34628 11544 23248 0 0 11550 0 214 0 14418 73 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 27
    ctxt 173265
    btime 0
    processes 1206
    procs_running 1
    procs_blocked 0
    softirq 93510 0 47395 1 0 0 0 42913 0 0 3201
     */
    FILE *fd;
    char buff[256];
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy = cpust;

    fd = fopen("/proc/stat", "r");
    if (!fd) {
        emxloge("open /proc/stat failed\n");
        return -1;
    }
    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system,
           &cpu_occupy->idle);
//    printf("[%s:%d] name:%s, user:%u, nice:%u, system:%u, idle:%u\n",
//           __FUNCTION__ , __LINE__, cpu_occupy->name, cpu_occupy->user, cpu_occupy->nice,cpu_occupy->system, cpu_occupy->idle);

    fclose(fd);
    return 0;
}

int cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n) {
    unsigned long od, nd;
    unsigned long id, sd;
    int cpu_use = 0;

    od = (unsigned long) (o->user + o->nice + o->system + o->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od
    nd = (unsigned long) (n->user + n->nice + n->system + n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给od

    id = (unsigned long) (n->user - o->user);    //用户第一次和第二次的时间之差再赋给id
    sd = (unsigned long) (n->system - o->system);//系统第一次和第二次的时间之差再赋给sd
    if ((nd - od) != 0)
        cpu_use = (int) ((sd + id) * 10000) / (nd - od); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
    else cpu_use = 0;
//    printf("[%s:%d] cpu: %d\n",__FUNCTION__ , __LINE__, cpu_use);
    return cpu_use / 100;
}

ErrCodeE Misc::GetSysUsage(Misc::SysUsage &usage, int calcTimeMs) {
    unsigned int cpu = 0;

    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    MEM_OCCUPY mem_stat;

    //获取内存
    if (get_memoccupy((MEM_OCCUPY *) &mem_stat) != 0)
        return ErrCodeE::Failure;

    //第一次获取cpu使用情况
    if (get_cpuoccupy((CPU_OCCUPY *) &cpu_stat1) != 0)
        return ErrCodeE::Failure;

    usleep(calcTimeMs * 1000);

    //第二次获取cpu使用情况
    if (get_cpuoccupy((CPU_OCCUPY *) &cpu_stat2) != 0)
        return ErrCodeE::Failure;

    //计算cpu使用率
    cpu = cal_cpuoccupy((CPU_OCCUPY *) &cpu_stat1, (CPU_OCCUPY *) &cpu_stat2);
    usage.cpu = (int) cpu;
    usage.memTotal = (int) mem_stat.total;
    usage.memFree = (int) mem_stat.MemAvailable;
    usage.memLoad = (int) (mem_stat.total - mem_stat.MemAvailable);
    return ErrCodeE::Success;
}
