/******************************************************************************
* Copyright (c) 2024 CrossDB ORG.
*
*   https://crossdb.org
*   https://github.com/crossdb-org/crossdb
*
* This program is free software: you can use, redistribute, and/or modify
* it under the terms of the GNU Affero General Public License, version 3
* or later ("AGPL"), as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef __XDB_COMMON_H__
#define __XDB_COMMON_H__

#ifndef __restrict
#define __restrict	restrict
#endif


typedef int	xdb_rowid;


/******************************************************************************
	MACRO
******************************************************************************/
	
#define XDB_LEN_MASK				0xFFFFFFF

#define XDB_ROWID_VALID(row_id, max_id)	((row_id > 0) && (row_id <= max_id))
#define XDB_ROWID_MSBIT				((sizeof(xdb_rowid)<<3)-1)
#define XDB_ROWID_MSB				(1<<XDB_ROWID_MSBIT)
#define XDB_ROWID_MASK				(~XDB_ROWID_MSB)

#define XDB_SETERR(code, errmsgfmt...)	\
		memset (&pConn->conn_res, 0, sizeof(pConn->conn_res));	\
		pConn->conn_res.len_type = (sizeof (xdb_res_t)) | (XDB_RET_REPLY<<28);	\
		pConn->conn_res.errcode = code; \
		pConn->conn_res.row_data = (uintptr_t)pConn->conn_msg.msg;	\
		pConn->conn_msg.len = snprintf(pConn->conn_msg.msg, sizeof(pConn->conn_msg.msg)-1, errmsgfmt);	\
		pConn->conn_msg.len_type = (XDB_RET_MSG<<28) | pConn->conn_msg.len;	\
		pConn->conn_res.data_len = pConn->conn_msg.len;

#define XDB_EXPECT_BRK(expr, code, errmsgfmt...)	\
	if (!(expr)) {	\
		XDB_SETERR (code, errmsgfmt) \
		break;	\
	}

#define XDB_EXPECT_GOTO(expr, gototag, code, errmsgfmt...)	\
	if (!(expr)) {	\
		XDB_SETERR (code, errmsgfmt) \
		goto gototag;	\
	}

#define XDB_EXPECT_RET(expr, retval, code, errmsgfmt...)	\
	if (!(expr)) {	\
		XDB_SETERR (code, errmsgfmt) \
		return retval;	\
	}

#define XDB_EXPECT2(expr)	\
	if (!(expr)) {	\
		goto error;	\
	}

#define XDB_EXPECT_RETE(expr, code, errmsgfmt...)	XDB_EXPECT_RET(expr, -code, code, errmsgfmt)

#define XDB_EXPECT(expr, code, errmsgfmt...)		XDB_EXPECT_GOTO(expr, error, code, errmsgfmt)

#define XDB_EXPECT_SOCK(expr, code, errmsgfmt...)	\
	if (!(expr)) {	\
		XDB_SETERR (code, errmsgfmt) \
		xdb_sock_close (pConn->sockfd);	\
		pConn->sockfd = -1;	\
		goto error;	\
	}


/******************************************************************************
	Types
******************************************************************************/

typedef struct xdb_field_t {
	xdb_obj_t		obj;
	uint8_t			fld_type;
	uint8_t			fld_decmial;
	uint16_t		fld_id;
	uint32_t		fld_off;	
	int				fld_len;
	uint16_t		fld_flags;
	int8_t			idx_fid[XDB_MAX_MATCH_COL];
} xdb_field_t;


/******************************************************************************
	RowSet
******************************************************************************/

typedef struct {
	xdb_rowid 	rid;
	void		*ptr;
} xdb_rowptr_t;

#define XDB_ROWLIST_CNT		16

// multiple table rowset map
typedef struct {
	// when change, use change xdb_rowset_init
	xdb_rowid 		count;
	xdb_rowid 		seqid;
	xdb_rowid		limit;
	xdb_rowid		offset;
	
	xdb_rowid 		cap;
	xdb_meta_t		*pTblMeta;
	uint16_t		*pFldMap;
	xdb_rowptr_t 	*pRowList;
	xdb_rowptr_t 	rowlist[XDB_ROWLIST_CNT];
	//uint8_t		buf[xxx]; // if no lock, then cache result ?
} xdb_rowset_t;

static bool s_xdb_bInit;

int 
xdb_init ();

int 
xdb_exit ();

XDB_STATIC const char* 
xdb_type2str (xdb_type_t tp);

#endif // __XDB_COMMON_H__