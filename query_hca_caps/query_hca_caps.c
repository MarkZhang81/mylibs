#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <infiniband/verbs.h>
#include <infiniband/mlx5dv.h>

#include "mlx5_ifc.h"

static char *ibdev_name;


#define INFO printf
#define ERR printf

int query_hca_cap(struct ibv_context *ibctx)
{
	uint16_t opmod = MLX5_SET_HCA_CAP_OP_MOD_GENERAL_DEVICE |
		HCA_CAP_OPMOD_GET_CUR;
	uint32_t out[DEVX_ST_SZ_DW(query_hca_cap_out)] = {};
	uint32_t in[DEVX_ST_SZ_DW(query_hca_cap_in)] = {};
	int ret;

	DEVX_SET(query_hca_cap_in, in, opcode, MLX5_CMD_OP_QUERY_HCA_CAP);
	DEVX_SET(query_hca_cap_in, in, op_mod, opmod);

	ret = mlx5dv_devx_general_cmd(ibctx, in, sizeof(in), out, sizeof(out));
	if (ret) {
		ERR("mlx5dv_devx_general_cmd failed: %d, errno %d\n", ret, errno);
		return ret;
	}

	INFO("Test devx_general_cmd(query_hca_cap): qos %d, log_max_qp_sz %d, log_max_qp %d, log_max_cq_sz %d, log_max_cq %d, port_type %d, ib_virt %d\n",
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.qos),
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.log_max_qp_sz),
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.log_max_qp),
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.log_max_cq_sz),
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.log_max_cq),
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.port_type),
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.ib_virt));
	INFO("Test devx_general_cmd(query_hca_cap): CAP.vport_group_manager %d, CAP.lag_master %d, CAP.num_lag_ports %d\n",
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.vport_group_manager),
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.lag_master),
	     DEVX_GET(query_hca_cap_out, out, capability.cmd_hca_cap.num_lag_ports));

	return 0;
}

int main(int argc, char *argv[])
{
        struct ibv_device **dev_list;
	struct ibv_context *ibctx;
	const char *devname;
	int i, ret = 0;

	if (argv[1] == NULL) {
		printf("usage: %s <ibdev_name>\n", argv[0]);
		exit (1);
	}
	ibdev_name = argv[1];

	dev_list = ibv_get_device_list(NULL);
	if (!dev_list) {
		perror("ibv_get_device_list()");
		return -errno;
	}

	for (i = 0; dev_list[i] != NULL; i++) {
		devname = ibv_get_device_name(dev_list[i]);
		//printf("=DEBUG:%s:%d: %s, %s\n", __func__, __LINE__, dev, s_ibv_devname);
		if (strncmp(devname, ibdev_name, strlen(ibdev_name)) == 0) {
			printf("Device found: %d/%s\n", i, devname);
			break;
		}
	}
	if (dev_list[i] == NULL) {
		printf("Device not found: %d/%s\n", i, ibdev_name);
		ret = errno;
		goto out;
	}

	ibctx = ibv_open_device(dev_list[i]);
	if (!ibctx) {
		perror("ibv_open_device");
		ret = errno;
		goto out;
	}

	query_hca_cap(ibctx);

	ibv_close_device(ibctx);

out:
	ibv_free_device_list(dev_list);
	return ret;
}
