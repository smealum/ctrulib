#include "soc_common.h"
#include <errno.h>
#include <sys/socket.h>
#include <3ds/ipc.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int ret = 0;
	int tmp_addrlen = 0;
	u32 *cmdbuf = getThreadCommandBuffer();
	u8 tmpaddr[0x1c];

	sockfd = soc_get_fd(sockfd);
	if(sockfd < 0) {
		errno = -sockfd;
		return -1;
	}

	memset(tmpaddr, 0, 0x1c);

	if(addr->sa_family == AF_INET)
		tmp_addrlen = 8;
	else
		tmp_addrlen = 0x1c;

	if(addrlen < tmp_addrlen) {
		errno = EINVAL;
		return -1;
	}

	tmpaddr[0] = tmp_addrlen;
	tmpaddr[1] = addr->sa_family;
	memcpy(&tmpaddr[2], &addr->sa_data, tmp_addrlen-2);

	cmdbuf[0] = IPC_MakeHeader(0x5,2,4); // 0x50084
	cmdbuf[1] = (u32)sockfd;
	cmdbuf[2] = (u32)tmp_addrlen;
	cmdbuf[3] = IPC_Desc_CurProcessId();
	cmdbuf[5] = IPC_Desc_StaticBuffer(tmp_addrlen,0);
	cmdbuf[6] = (u32)tmpaddr;

	ret = svcSendSyncRequest(SOCU_handle);
	if(ret != 0) {
		errno = SYNC_ERROR;
		return ret;
	}

	ret = (int)cmdbuf[1];
	if(ret == 0)
		ret = _net_convert_error(cmdbuf[2]);

	if(ret < 0) {
		errno = -ret;
		return -1;
	}

	return 0;
}
