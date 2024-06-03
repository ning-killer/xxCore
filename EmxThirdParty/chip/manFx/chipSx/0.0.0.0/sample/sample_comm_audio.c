#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "sample_comm.h"

typedef struct tagSAMPLE_AENC_S
{
    TS_BOOL bStart;
	pthread_t stAencPid;
    TS_S32  AeChn;
    TS_S32  AdChn;
    FILE*    pfd;
    TS_BOOL bSendAdChn;
} SAMPLE_AENC_S;

typedef struct tagSAMPLE_AI_S
{
    TS_BOOL bStart;
    TS_S32  AiDev;
    TS_S32  AiChn;
    TS_S32  AencChn;
    TS_S32  AoDev;
    TS_S32  AoChn;
    TS_BOOL bSendAenc;
    TS_BOOL bSendAo;
    pthread_t stAiPid;
} SAMPLE_AI_S;

typedef struct tagSAMPLE_ADEC_S
{
    TS_BOOL bStart;
    TS_S32 AdChn;
    FILE* pfd;
    pthread_t stAdPid;
} SAMPLE_ADEC_S;

typedef struct tagSAMPLE_AO_S
{
    TS_BOOL bStart;
    TS_S32  AoDev;
    TS_S32  AoChn;
	TS_S32  type;
    pthread_t stAoPid;
} SAMPLE_AO_S;

static SAMPLE_AI_S   gs_stSampleAi[AI_DEV_MAX_NUM* AI_MAX_CHN_NUM];
static SAMPLE_AO_S   gs_stSampleAo[AO_DEV_MAX_NUM* AO_MAX_CHN_NUM];
static SAMPLE_AENC_S gs_stSampleAenc[AENC_MAX_CHN_NUM];
static SAMPLE_ADEC_S gs_stSampleAdec[ADEC_MAX_CHN_NUM];

#if 1
static ts_char *ai_cap_store = "./aiao_test.pcm";
//static ts_char *ai_cap_system_op = "rm -rf ./aiao_test.pcm";

static ts_u32 pcm_total_size;

static FILE *file;
static ts_u32 bytes_per_frame;
static int test_flag = 1;

ts_bool is_pcm_file_open(void)
{
	if (file)
		return TS_TRUE;
	return TS_FALSE;
}

ts_void create_pcm_file(ts_char *file_path, ts_u8 bit_width, ts_u8 channel_num, ts_u32 sample_rate)
{
	file = fopen(file_path, "wb");
	bytes_per_frame = channel_num * bit_width / 8;
	printf("%s channels=%d, rate=%d, bit_width=%d, bytes_per_frame=%d\n", __func__, channel_num, sample_rate,
	       bit_width, bytes_per_frame);
}

ts_void close_pcm_file(void)
{
	if (!file)
		return;

	fclose(file);
	file = NULL;
}

ts_void write_pcm_file(ts_char *buffer, ts_u32 size)
{
	//printf("%s size=%d, bytes_per_frame=%d, frames=%d\n", __func__, size, bytes_per_frame,
	//       (ts_u32)(size / bytes_per_frame));
	//fwrite(buffer, bytes_per_frame, size / bytes_per_frame, file);
	fwrite(buffer, 1, size, file);
}

static void test_save_pcm(AUDIO_FRAME_S *pFrame)
{
	if (0 == test_flag)
		return;

	//pr_debug("%s: port%d, buffer=%p\n", __func__, pBuffer->nOutputPortIndex, pBuffer->pBuffer);
	//printf("%s: port%d, buffer=%p, appprivate=%p,%d\n", __func__, pBuffer->nOutputPortIndex, pBuffer->pBuffer, pBuffer->pAppPrivate, *(int *)pBuffer->pAppPrivate);
	AIO_ATTR_S aioAttr		= {0};
#if defined(TX5215) || defined(TX5112) || defined(TX5239DV200)
	aioAttr.u8CardNum 		= 0;
#else
	aioAttr.u8CardNum 		= 1;
#endif
	aioAttr.u8DeviceNum 	= 0;
	aioAttr.enSamplerate 	= AUDIO_SAMPLE_RATE_16000;
	aioAttr.enBitwidth 		= AUDIO_BIT_WIDTH_16;
	aioAttr.enSoundmode		= AUDIO_SOUND_MODE_MONO;
	aioAttr.u32FrmNum		= 4;
	aioAttr.u32PtNumPerFrm 	= 160;

	//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p", __func__, pFrame->u32Len, pFrame->u64VirAddr[0]);
	if (TS_FALSE == is_pcm_file_open())
		create_pcm_file(ai_cap_store, aioAttr.enBitwidth, (aioAttr.enSoundmode + 1),
			aioAttr.enSamplerate);
	if (pFrame && pFrame->u32Len ==  320) {
		write_pcm_file(pFrame->u64VirAddr[0], pFrame->u32Len);
		pcm_total_size += pFrame->u32Len;
	}
	else
	{
		printf("length is error:[%d]\n", pFrame->u32Len);
	}

	if (pcm_total_size > 1 * 512 * 1024) {
		test_flag = 0;
		close_pcm_file();
	}

	return;
}
#endif
#if 1
#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT 0x20746d66
#define ID_DATA 0x61746164

struct wav_header {
	ts_u32 riff_id;
	ts_u32 riff_sz;
	ts_u32 riff_fmt;
	ts_u32 fmt_id;
	ts_u32 fmt_sz;
	ts_u16 audio_format;
	ts_u16 num_channels;
	ts_u32 sample_rate;
	ts_u32 byte_rate;
	ts_u16 block_align;
	ts_u16 bits_per_sample;
	ts_u32 data_id;
	ts_u32 data_sz;
};

struct riff_wave_header {
	ts_u32 riff_id;
	ts_u32 riff_sz;
	ts_u32 wave_id;
};

struct chunk_header {
	ts_u32 id;
	ts_u32 sz;
};

struct chunk_fmt {
	ts_u16 audio_format;
	ts_u16 num_channels;
	ts_u32 sample_rate;
	ts_u32 byte_rate;
	ts_u16 block_align;
	ts_u16 bits_per_sample;
};

static ts_u32 parse_wave_file(FILE *file)
{
	struct riff_wave_header wave_header;
	struct chunk_header chunk_header;
	struct chunk_fmt chunk_fmt;

	if (fread(&wave_header, sizeof(wave_header), 1, file) != 1) {
		printf("error: does not contain a riff/wave header\n");
		return -1;
	}

	if (wave_header.riff_id != ID_RIFF || wave_header.wave_id != ID_WAVE) {
		printf("error: is not a riff/wave file\n");
		return -1;
	}

	ts_bool more_chunks = TS_TRUE;
	do {
		if (fread(&chunk_header, sizeof(chunk_header), 1, file) != 1) {
			printf("error: does not contain a data chunk\n");
			return -1;
		}
		switch (chunk_header.id) {
		case ID_FMT:
			if (fread(&chunk_fmt, sizeof(chunk_fmt), 1, file) != 1) {
				printf("error: has incomplete format chunk\n");
				return -1;
			}
			/* If the format header is larger, skip the rest */
			if (chunk_header.sz > sizeof(chunk_fmt)) {
				fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
			}
			break;
		case ID_DATA:
			/* Stop looking for chunks */
			more_chunks = TS_FALSE;
			break;
		default:
			/* Unknown chunk, skip bytes */
			fseek(file, chunk_header.sz, SEEK_CUR);
		}
	} while (more_chunks);

	return chunk_header.sz;
}

#endif

/******************************************************************************
* function : get frame from Ai, send it  to Aenc or Ao
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AiProc(void* parg)
{
    TS_S32 s32Ret;
    TS_S32 AiFd;
    SAMPLE_AI_S* pstAiCtl = (SAMPLE_AI_S*)parg;
    AUDIO_FRAME_S stFrame;
    AEC_FRAME_S   stAecFrm;
    fd_set read_fds;
    struct timeval TimeoutVal;

    FD_ZERO(&read_fds);
    AiFd = TS_MPI_AI_GetFd(pstAiCtl->AiDev, pstAiCtl->AiChn);
    FD_SET(AiFd, &read_fds);

    while (pstAiCtl->bStart)
    {
        TimeoutVal.tv_sec = 15;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AiFd, &read_fds);

        s32Ret = TS_MPI_AI_SELECT(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }

        if (FD_ISSET(AiFd, &read_fds))
        {
            /* get frame from ai chn */
            memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));
            s32Ret = TS_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm, TS_FALSE);
            if (TS_SUCCESS != s32Ret )
            {
#if 0
                printf("%s: TS_MPI_AI_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = TS_FALSE;
                return NULL;
#else
                continue;
#endif
            }
			//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p\n", __func__, stFrame.u32Len, stFrame.u64VirAddr[0]);
			test_save_pcm(&stFrame);

            /* send frame to encoder */
            if (TS_TRUE == pstAiCtl->bSendAenc)
            {
                s32Ret = TS_MPI_AENC_SendFrame(pstAiCtl->AencChn, &stFrame, &stAecFrm);
                if (TS_SUCCESS != s32Ret )
                {
                    printf("%s: TS_MPI_AENC_SendFrame(%d), failed with %#x!\n", \
                           __FUNCTION__, pstAiCtl->AencChn, s32Ret);
                    pstAiCtl->bStart = TS_FALSE;
                    return NULL;
                }
            }

            /* send frame to ao */
            if (TS_TRUE == pstAiCtl->bSendAo)
            {
                s32Ret = TS_MPI_AO_SendFrame(pstAiCtl->AoDev, pstAiCtl->AoChn, &stFrame, 1000);
                if (TS_SUCCESS != s32Ret )
                {
                    printf("%s: TS_MPI_AO_SendFrame(%d, %d), failed with %#x!\n", \
                           __FUNCTION__, pstAiCtl->AoDev, pstAiCtl->AoChn, s32Ret);
                    pstAiCtl->bStart = TS_FALSE;
                    return NULL;
                }

            }

            /* finally you must release the stream */
            s32Ret = TS_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm);
            if (TS_SUCCESS != s32Ret )
            {
                printf("%s: TS_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = TS_FALSE;
                return NULL;
            }

        }
    }

    pstAiCtl->bStart = TS_FALSE;
    return NULL;
}

/******************************************************************************
* function : get frame from file, send it to Ao
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AoProc(void* parg)
{
    TS_S32 s32Ret;
	TS_U32 file_size, read_size, num_read;
	TS_U32 remaining_data_size;
    SAMPLE_AO_S* pstAoCtl = (SAMPLE_AO_S*)parg;
    AUDIO_FRAME_S stFrame;
    AEC_FRAME_S   stAecFrm;
	FILE *pfd = NULL;

	TS_U32 buffer_size = 320;
	TS_U8 *buffer = NULL;

	buffer = malloc(buffer_size);
	if (!buffer) {
		SAMPLE_PRT("unable to allocate %d bytes\n", buffer_size);
		goto exit;
	}

	//pfd = fopen("./test.pcm", "rb");
	pfd = fopen("./record.wav", "rb");
	if (!pfd) {
		SAMPLE_PRT("can not open file\n");
		goto exit;
	}

	fseek(pfd, 0L, SEEK_END);
	file_size = ftell(pfd);
	remaining_data_size = file_size;
	fseek(pfd, 0L, SEEK_SET);

	remaining_data_size = parse_wave_file(pfd);

    while (pstAoCtl->bStart)
    {
 		//printf("start\n");
		read_size = remaining_data_size > buffer_size ? buffer_size : remaining_data_size;
		num_read = fread(buffer, 1, read_size, pfd);
		remaining_data_size -= num_read;
		if (num_read <= 0)
		{
			SAMPLE_PRT("file read over\n");
#if 0
			break;
#else
			fseek(pfd, 0L, SEEK_END);
			file_size = ftell(pfd);
			remaining_data_size = file_size;
			fseek(pfd, 0L, SEEK_SET);
			remaining_data_size = parse_wave_file(pfd);
			continue;
#endif
		}

		//ref frame
		memset(&stFrame, 0, sizeof(AUDIO_FRAME_S));
		stFrame.u64VirAddr[0] 	= buffer;
		stFrame.u32Len			= num_read;

		//aec frame
        memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));

		//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p", __func__, stFrame.u32Len, stFrame.u64VirAddr[0]);

        /* send frame to ao */
        s32Ret = TS_MPI_AO_SendFrame(pstAoCtl->AoDev, pstAoCtl->AoChn, &stFrame, 1000);
        if (TS_SUCCESS != s32Ret )
        {
            SAMPLE_PRT("%s: TS_MPI_AO_SendFrame(%d, %d), failed with %#x!\n",
                  		__func__, pstAoCtl->AoDev, pstAoCtl->AoChn, s32Ret);
            pstAoCtl->bStart = TS_FALSE;
            break;
        }

    }

exit:
	if (buffer)
	{
		free(buffer);
	}

	if (pfd)
	{
		fclose(pfd);
	}

    pstAoCtl->bStart = TS_FALSE;
    return NULL;
}

/******************************************************************************
* function : get stream from Aenc, send it to Adec & save it to file
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AencProc(void* parg)
{
    TS_S32 s32Ret;
    TS_S32 AencFd;
    SAMPLE_AENC_S* pstAencCtl = (SAMPLE_AENC_S*)parg;
    AUDIO_STREAM_S stStream;
    AEC_FRAME_S   stAecFrm;
    fd_set read_fds;
    struct timeval TimeoutVal;

    FD_ZERO(&read_fds);
    AencFd = TS_MPI_AENC_GetFd(pstAencCtl->AeChn);
    FD_SET(AencFd, &read_fds);

    while (pstAencCtl->bStart)
    {
 		//printf("start\n");
        TimeoutVal.tv_sec = 15;
        TimeoutVal.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(AencFd, &read_fds);

        s32Ret = TS_MPI_AENC_SELECT(AencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            break;
        }
        else if (0 == s32Ret)
        {
            printf("%s: get aenc frame select time out\n", __FUNCTION__);
            break;
        }

		//printf("select\n");
        if (FD_ISSET(AencFd, &read_fds))
        {
            /* get frame from aenc chn */
            memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));
            s32Ret = TS_MPI_AENC_GetStream(pstAencCtl->AeChn, &stStream, -1);
			//printf("getStream\n");
            if (TS_SUCCESS != s32Ret )
            {
#if 0
                printf("%s: TS_MPI_AENC_GetFrame(%d, %d), failed with %#x!\n", \
                       __FUNCTION__, pstAencCtl->AencDev, pstAencCtl->AencChn, s32Ret);
                pstAencCtl->bStart = TS_FALSE;
                return NULL;
#else
                continue;
#endif
            }
			//printf("%s:u32Len = %d, pFrame->u64VirAddr[0]= %p", __func__, stFrame.u32Len, stFrame.u64VirAddr[0]);

			//test_save_pcm(&stStream);

			/* save audio stream to file */
			printf("len=%d\n", stStream.u32Len);
            (TS_VOID)fwrite(stStream.pStream, 1, stStream.u32Len, pstAencCtl->pfd);

            fflush(pstAencCtl->pfd);

            /* send frame to decoder */
            /*if (TS_TRUE == pstAencCtl->bSendAdChn)
            {
                s32Ret = TS_MPI_ADEC_SendStream(pstAencCtl->AdChn, &stStream, TS_TRUE);
                if (TS_SUCCESS != s32Ret )
                {
                    printf("%s: TS_MPI_ADEC_SendStream(%d), failed with %#x!\n", \
                           __FUNCTION__, pstAencCtl->AdChn, s32Ret);
                    pstAencCtl->bStart = TS_FALSE;
                    return NULL;
                }
            }*/


            /* finally you must release the stream */
            s32Ret = TS_MPI_AENC_ReleaseStream(pstAencCtl->AeChn, &stStream);
            if (TS_SUCCESS != s32Ret )
            {
                printf("%s: TS_MPI_AENC_ReleaseFrame(%d), failed with %#x!\n", \
                       __FUNCTION__, pstAencCtl->AeChn, s32Ret);
                pstAencCtl->bStart = TS_FALSE;
                return NULL;
            }

        }
    }

    pstAencCtl->bStart = TS_FALSE;
    return NULL;
}

/******************************************************************************
* function : get stream from file, and send it  to Adec
******************************************************************************/
void* SAMPLE_COMM_AUDIO_AdecProc(void* parg)
{
    TS_S32 s32Ret;
    AUDIO_STREAM_S stAudioStream;
    TS_U32 u32Len = 2048;
    TS_U32 u32ReadLen;
    TS_S32 s32AdecChn;
    TS_U8* pu8AudioStream = NULL;
    SAMPLE_ADEC_S* pstAdecCtl = (SAMPLE_ADEC_S*)parg;
    FILE* pfd = pstAdecCtl->pfd;
    s32AdecChn = pstAdecCtl->AdChn;

    pu8AudioStream = (TS_U8*)malloc(sizeof(TS_U8) * MAX_AUDIO_STREAM_LEN);
    if (NULL == pu8AudioStream)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        return NULL;
    }

    while (TS_TRUE == pstAdecCtl->bStart)
    {
        /* read from file */
        stAudioStream.pStream = pu8AudioStream;
        u32ReadLen = fread(stAudioStream.pStream, 1, u32Len, pfd);
        if (u32ReadLen <= 0)
        {
            s32Ret = TS_MPI_ADEC_SendEndOfStream(s32AdecChn, TS_FALSE);
            if (TS_SUCCESS != s32Ret)
            {
                printf("%s: TS_MPI_ADEC_SendEndOfStream failed!\n", __FUNCTION__);
            }
            (TS_VOID)fseek(pfd, 0, SEEK_SET);/*read file again*/
			sleep(5);
            continue;
        }

        /* here only demo adec streaming sending mode, but pack sending mode is commended */
        stAudioStream.u32Len = u32ReadLen;
        s32Ret = TS_MPI_ADEC_SendStream(s32AdecChn, &stAudioStream, TS_TRUE);
        if (TS_SUCCESS != s32Ret)
        {
            printf("%s: TS_MPI_ADEC_SendStream(%d) failed with %#x!\n", \
                   __FUNCTION__, s32AdecChn, s32Ret);
            break;
        }
    }

    free(pu8AudioStream);
    pu8AudioStream = NULL;
    fclose(pfd);
    pstAdecCtl->bStart = TS_FALSE;
    return NULL;
}


/******************************************************************************
* function : Create the thread to get frame from ai and send to ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AI_S* pstAi = NULL;

	pstAi = &gs_stSampleAi[AiDev * AI_MAX_CHN_NUM + AiChn];
	pstAi->bSendAenc = TS_FALSE;
	pstAi->bSendAo = TS_FALSE;
	pstAi->bStart = TS_TRUE;
	pstAi->AiDev = AiDev;
	pstAi->AiChn = AiChn;
	pstAi->AoDev = AoDev;
	pstAi->AoChn = AoChn;

	pthread_create(&pstAi->stAiPid, 0, SAMPLE_COMM_AUDIO_AiProc, pstAi);

	return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAo(AUDIO_DEV AoDev, AO_CHN AoChn)
{
	SAMPLE_AO_S* pstAo = NULL;

	pstAo = &gs_stSampleAo[AoDev * AI_MAX_CHN_NUM + AoChn];
	pstAo->bStart = TS_TRUE;
	pstAo->AoDev = AoDev;
	pstAo->AoChn = AoChn;

	pthread_create(&pstAo->stAoPid, 0, SAMPLE_COMM_AUDIO_AoProc, pstAo);

	return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from aenc and send to adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE* pAecFd)
{
	SAMPLE_AENC_S* pstAenc = NULL;

	if (NULL == pAecFd)
	{
		return TS_FAILURE;
	}

	pstAenc = &gs_stSampleAenc[AeChn];
	pstAenc->AeChn = AeChn;
	pstAenc->AdChn = AdChn;
	pstAenc->bSendAdChn = TS_FALSE;
	pstAenc->pfd = pAecFd;
	pstAenc->bStart = TS_TRUE;
	pthread_create(&pstAenc->stAencPid, 0, SAMPLE_COMM_AUDIO_AencProc, pstAenc);

	return TS_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from file and send to adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE* pAdcFd)
{
	SAMPLE_ADEC_S* pstAdec = NULL;

	if (NULL == pAdcFd)
	{
		return TS_FAILURE;
	}

	pstAdec = &gs_stSampleAdec[AdChn];
	pstAdec->AdChn = AdChn;
	pstAdec->pfd = pAdcFd;
	pstAdec->bStart = TS_TRUE;
	pthread_create(&pstAdec->stAdPid, 0, SAMPLE_COMM_AUDIO_AdecProc, pstAdec);

	return TS_SUCCESS;
}
//new
TS_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn)
{
	SAMPLE_AENC_S* pstAenc = NULL;

	pstAenc = &gs_stSampleAenc[AeChn];
	pstAenc->bStart = TS_FALSE;

	return TS_SUCCESS;

}
//new
TS_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn)
{
	SAMPLE_ADEC_S* pstAdec = NULL;

	pstAdec = &gs_stSampleAdec[AdChn];
	pstAdec->bStart = TS_FALSE;

	return TS_SUCCESS;

}

/******************************************************************************
* function : Start Ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, TS_S32 s32AiChnCnt,
								 AIO_ATTR_S* pstAioAttr, TS_BOOL bAiChnEn, AUDIO_SAMPLE_RATE_E enOutSampleRate, TS_BOOL bResampleEn, TS_VOID* pstAiVqeAttr, TS_U32 u32AiVqeType)
{
	TS_S32 i;
	TS_S32 s32Ret;

	//stVqeConfig.stAgcCfg.stAgcAlgCfg.u8LimiterEnable 		= 1;
	//stVqeConfig.stAgcCfg.s32PreInmicLevel 					= 0;
	s32Ret = TS_MPI_AI_SetPubAttr(AiDevId, pstAioAttr);
	if (s32Ret)
	{
		printf("%s: TS_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	s32Ret = TS_MPI_AI_Enable(AiDevId);
	if (s32Ret)
	{
		printf("%s: TS_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	for (i = 0; i < s32AiChnCnt>>pstAioAttr->enSoundmode; i++)
	{
		if (TS_TRUE == bAiChnEn)
		{
			s32Ret = TS_MPI_AI_EnableChn(AiDevId, i);
			if (s32Ret)
			{
				printf("%s: TS_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
				return s32Ret;
			}
		}
		else
		{
			s32Ret = TS_MPI_AI_EnableChn(AiDevId, i);
			s32Ret = TS_MPI_AI_DisableChn(AiDevId, i);
		}

		if (TS_TRUE == bResampleEn)
		{
			s32Ret = TS_MPI_AI_EnableReSmp(AiDevId, i, enOutSampleRate);
			if (s32Ret)
			{
				printf("%s: TS_MPI_AI_EnableReSmp(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
				return s32Ret;
			}
		}

		if (NULL != pstAiVqeAttr)
		{
			TS_BOOL bAiVqe = TS_TRUE;
			switch (u32AiVqeType)
			{
				case 0:
					s32Ret = TS_SUCCESS;
					bAiVqe = TS_FALSE;
					break;
				case 1:
					//todo:liyi AoDevid AoChn
					s32Ret = TS_MPI_AI_SetTalkVqeAttr(AiDevId, i, 0, 0, (AI_TALKVQE_CONFIG_S *)pstAiVqeAttr);
					break;
				default:
					s32Ret = TS_FAILURE;
					break;
			}

			if (s32Ret)
			{
				printf("%s: SetAiVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, u32AiVqeType, AiDevId, i, s32Ret);
				return s32Ret;
			}

			if (bAiVqe)
			{
				SAMPLE_PRT("SAMPLE_COMM_AUDIO_StartAi bAiVqe:%d!\n", bAiVqe);
				s32Ret = TS_MPI_AI_EnableVqe(AiDevId, i);
				if (s32Ret)
				{
					printf("%s: TS_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
					return s32Ret;
				}
			}
		}
	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Stop Ai
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, TS_S32 s32AiChnCnt,
								TS_BOOL bResampleEn, TS_BOOL bVqeEn)
{
	TS_S32 i;
	TS_S32 s32Ret;

	for (i = 0; i < s32AiChnCnt; i++)
	{
		if (TS_TRUE == bResampleEn)
		{
			s32Ret = TS_MPI_AI_DisableReSmp(AiDevId, i);
			if (TS_SUCCESS != s32Ret)
			{
				printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
				return s32Ret;
			}
		}

		if (TS_TRUE == bVqeEn)
		{
			s32Ret = TS_MPI_AI_DisableVqe(AiDevId, i);
			if (TS_SUCCESS != s32Ret)
			{
				printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
				return s32Ret;
			}
		}

		s32Ret = TS_MPI_AI_DisableChn(AiDevId, i);
		if (TS_SUCCESS != s32Ret)
		{
			printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
			return s32Ret;
		}
	}

	s32Ret = TS_MPI_AI_Disable(AiDevId);
	if (TS_SUCCESS != s32Ret)
	{
		printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
		return s32Ret;
	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Start Ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt,
								 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, TS_BOOL bResampleEn)
{
	TS_S32 i;
	TS_S32 s32Ret;

	s32Ret = TS_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
			   AoDevId, s32Ret);
		return TS_FAILURE;
	}

	s32Ret = TS_MPI_AO_Enable(AoDevId);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
		return TS_FAILURE;
	}

	for (i = 0; i < s32AoChnCnt>>pstAioAttr->enSoundmode; i++)
	{
		s32Ret = TS_MPI_AO_EnableChn(AoDevId, i);
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: TS_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
			return TS_FAILURE;
		}

		if (TS_TRUE == bResampleEn)
		{
			s32Ret = TS_MPI_AO_DisableReSmp(AoDevId, i);
			s32Ret |= TS_MPI_AO_EnableReSmp(AoDevId, i, enInSampleRate);
			if (TS_SUCCESS != s32Ret)
			{
				printf("%s: TS_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
				return TS_FAILURE;
			}
		}

	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Stop Ao
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt, TS_BOOL bResampleEn)
{
	TS_S32 i;
	TS_S32 s32Ret;

	for (i = 0; i < s32AoChnCnt; i++)
	{
		if (TS_TRUE == bResampleEn)
		{
			s32Ret = TS_MPI_AO_DisableReSmp(AoDevId, i);
			if (TS_SUCCESS != s32Ret)
			{
				printf("%s: TS_MPI_AO_DisableReSmp failed with %#x!\n", __FUNCTION__, s32Ret);
				return s32Ret;
			}
		}

		s32Ret = TS_MPI_AO_DisableChn(AoDevId, i);
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: TS_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
			return s32Ret;
		}
	}

	s32Ret = TS_MPI_AO_Disable(AoDevId);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
		return s32Ret;
	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Start Aenc
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAenc(TS_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType)
{
	AENC_CHN AeChn;
	TS_S32 s32Ret, i;
	AENC_CHN_ATTR_S stAencAttr;

	/* set AENC chn attr */
	stAencAttr.enType = enType;
	stAencAttr.u32BufSize = 30;
	//stAencAttr.u32PtNumPerFrm = pstAioAttr->u32PtNumPerFrm;
	stAencAttr.u32PtNumPerFrm = pstAioAttr->u32PtNumPerFrm * (pstAioAttr->enSoundmode + 1) * pstAioAttr->enBitwidth / 8; //todo:liyi different from hi

	if (PT_ADPCMA == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else if (PT_G711A == stAencAttr.enType || PT_G711U == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else if (PT_G726 == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else if (PT_LPCM == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else if (PT_AAC == stAencAttr.enType)
	{
		stAencAttr.pValue = NULL;
	}
	else
	{
		printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAencAttr.enType);
		return TS_FAILURE;
	}

	for (i = 0; i < s32AencChnCnt; i++)
	{
		AeChn = i;

		/* create aenc chn*/
		s32Ret = TS_MPI_AENC_CreateChn(AeChn, &stAencAttr);
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: TS_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,
				   AeChn, s32Ret);
			return s32Ret;
		}
	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Stop Aenc
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopAenc(TS_S32 s32AencChnCnt)
{
	TS_S32 i;
	TS_S32 s32Ret;

	for (i = 0; i < s32AencChnCnt; i++)
	{
		s32Ret = TS_MPI_AENC_DestroyChn(i);
		if (TS_SUCCESS != s32Ret)
		{
			printf("%s: TS_MPI_AENC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
				   i, s32Ret);
			return s32Ret;
		}

	}

	return TS_SUCCESS;
}

/******************************************************************************
* function : Start Adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
	TS_S32 s32Ret;
	ADEC_CHN_ATTR_S stAdecAttr;

	stAdecAttr.enType = enType;
	stAdecAttr.u32BufSize = 20;
	stAdecAttr.enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */

	if (PT_ADPCMA == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_G711A == stAdecAttr.enType || PT_G711U == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_G726 == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_LPCM == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_AAC == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else if (PT_MP3 == stAdecAttr.enType)
	{
		stAdecAttr.pValue = NULL;
	}
	else
	{
		printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAdecAttr.enType);
		return TS_FAILURE;
	}

	/* create adec chn*/
	s32Ret = TS_MPI_ADEC_CreateChn(AdChn, &stAdecAttr);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_ADEC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, \
			   AdChn, s32Ret);
		return s32Ret;
	}
	return 0;
}

/******************************************************************************
* function : Stop Adec
******************************************************************************/
TS_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn)
{
	TS_S32 s32Ret;

	s32Ret = TS_MPI_ADEC_DestroyChn(AdChn);
	if (TS_SUCCESS != s32Ret)
	{
		printf("%s: TS_MPI_ADEC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
			   AdChn, s32Ret);
		return s32Ret;
	}

	return TS_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

