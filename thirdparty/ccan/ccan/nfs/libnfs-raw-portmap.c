/* Licensed under GPLv3+ - see LICENSE file for details */
/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc/portmap.h"

bool_t
xdr_mapping (XDR *xdrs, mapping *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->prog))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->vers))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->prot))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->port))
				 return FALSE;
		} else {
			IXDR_PUT_U_LONG(buf, objp->prog);
			IXDR_PUT_U_LONG(buf, objp->vers);
			IXDR_PUT_U_LONG(buf, objp->prot);
			IXDR_PUT_U_LONG(buf, objp->port);
		}
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->prog))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->vers))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->prot))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->port))
				 return FALSE;
		} else {
			objp->prog = IXDR_GET_U_LONG(buf);
			objp->vers = IXDR_GET_U_LONG(buf);
			objp->prot = IXDR_GET_U_LONG(buf);
			objp->port = IXDR_GET_U_LONG(buf);
		}
	 return TRUE;
	}

	 if (!xdr_u_int (xdrs, &objp->prog))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->vers))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->prot))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->port))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_call_args (XDR *xdrs, call_args *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->prog))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->vers))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->proc))
				 return FALSE;

		} else {
		IXDR_PUT_U_LONG(buf, objp->prog);
		IXDR_PUT_U_LONG(buf, objp->vers);
		IXDR_PUT_U_LONG(buf, objp->proc);
		}
		 if (!xdr_bytes (xdrs, (char **)&objp->args.args_val, (u_int *) &objp->args.args_len, ~0))
			 return FALSE;
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->prog))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->vers))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->proc))
				 return FALSE;

		} else {
		objp->prog = IXDR_GET_U_LONG(buf);
		objp->vers = IXDR_GET_U_LONG(buf);
		objp->proc = IXDR_GET_U_LONG(buf);
		}
		 if (!xdr_bytes (xdrs, (char **)&objp->args.args_val, (u_int *) &objp->args.args_len, ~0))
			 return FALSE;
	 return TRUE;
	}

	 if (!xdr_u_int (xdrs, &objp->prog))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->vers))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->proc))
		 return FALSE;
	 if (!xdr_bytes (xdrs, (char **)&objp->args.args_val, (u_int *) &objp->args.args_len, ~0))
		 return FALSE;
	return TRUE;
}
