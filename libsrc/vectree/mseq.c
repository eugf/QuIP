#include "quip_config.h"

char VersionId_vectree_mseq[] = QUIP_VERSION_STRING;

#include <math.h>
#include "data_obj.h"

int generate_packed_mseq( int reglen, int tap_n, u_long *data, u_long start_val )
{
	/* the data buffer must be able to hold (2^len-1)/reglen bits... */

	u_long reg;
	u_long high_bit;
	u_long tap_bit;
	u_long newbit;
	int i=0;

	reg=start_val;		/* start value */
	high_bit = 1 << (reglen-1);
	tap_bit = 1 << tap_n;

	while(1) {
		if( ((i++) % reglen) == 0 ){
			*data++ = reg;
		}


		newbit = (reg&1) ^ ((reg&tap_bit)>>tap_n);

		reg >>= 1;

		if( newbit )
			reg |= high_bit;

		if( reg == start_val ) return i;
	}
}

int generate_mseq( int reglen, int tap_n, u_long *data, u_long start_val )
{
	/* the data buffer must be able to hold (2^len-1) bits... */

	u_long reg;
	u_long high_bit;
	u_long tap_bit;
	u_long newbit;
	int i=0;

	reg=start_val;		/* start value */
	high_bit = 1 << (reglen-1);
	tap_bit = 1 << tap_n;
sprintf(DEFAULT_ERROR_STRING,"generate_mseq:  high_bit = %ld (0x%lx), tap_bit = %ld (0x%lx)",
		high_bit,high_bit,tap_bit,tap_bit);
advise(DEFAULT_ERROR_STRING);

	while(1) {
		i++;
		*data++ = reg;

		newbit = (reg&1) ^ ((reg&tap_bit)>>tap_n);

		reg >>= 1;

		if( newbit )
			reg |= high_bit;

		if( reg == start_val ) return i;
	}
}

static COMMAND_FUNC( do_packed_mseq )
{
	Data_Obj *dp;
	int t, m, n;
	u_long start_val;

	dp = PICK_OBJ("mseq vector");
	n = HOW_MANY("register length");
	t = HOW_MANY("tap bit index");
	start_val = HOW_MANY("start value");

	if( dp == NO_OBJ ) return;

	/* BUG check object here */

	if( (dimension_t)(((1<<n)+n-1)/n) > dp->dt_cols ){
		sprintf(ERROR_STRING,"target mseq vector %s (%d) too small for this register length %d",dp->dt_name,
				dp->dt_cols,n);
		WARN(ERROR_STRING);
		return;
	}

	m = generate_packed_mseq(n,t,(u_long *)dp->dt_data,start_val);
	if( (m+1) == 1<<(n) ){
		sprintf(msg_str,"GOOD reglen = %d, t = %d, m = %d (0x%x)",n,t,m,m);
		prt_msg(msg_str);
	}
}

static COMMAND_FUNC( do_mseq )
{
	Data_Obj *dp;
	int t, m, n;
	u_long start_val;

	dp = PICK_OBJ("mseq vector");
	n = HOW_MANY("register length");
	t = HOW_MANY("tap bit index");
	start_val = HOW_MANY("start value");

	if( dp == NO_OBJ ) return;

	/* BUG check object here */

	if( (dimension_t)((1<<n)-1) > dp->dt_cols ){
		sprintf(ERROR_STRING,"target mseq vector %s (%d) too small for this register length %d",dp->dt_name,
				dp->dt_cols,n);
		WARN(ERROR_STRING);
		return;
	}

	m = generate_mseq(n,t,(u_long *)dp->dt_data,start_val);
	if( (m+1) == 1<<(n) ){
		sprintf(msg_str,"GOOD reglen = %d, t = %d, seqlen = %d (0x%x)",n,t,m,m);
		prt_msg(msg_str);
	} else {
		sprintf(ERROR_STRING,"BAD reglen = %d, t = %d, seqlen = %d (0x%x)",n,t,m,m);
		WARN(ERROR_STRING);
	}
}

static COMMAND_FUNC( do_pack )
{
	Data_Obj *dst_dp, *src_dp;
	int reg_len, total_bits, nwords;
	u_long src_bit, dst_bit, max_src_bit, max_dst_bit;
	u_long *src, *dst;

	dst_dp = PICK_OBJ("destination vector");
	src_dp = PICK_OBJ("source vector");
	
	reg_len = HOW_MANY("source register length");
	total_bits = (1<<reg_len)-1;
	nwords = floor((total_bits+31)/32);

	if( dst_dp == NO_OBJ || src_dp == NO_OBJ ) return;
	src = (u_long *)src_dp->dt_data;
	dst = (u_long *)dst_dp->dt_data;

	/* BUG do size checks here */
	src_bit=1;
	dst_bit=1;
	max_src_bit = 1 << (reg_len-1);
	max_dst_bit = 1 << (32-1);

	while( total_bits -- ){
		if( *src & src_bit )
			*dst |= dst_bit;

		if( src_bit >= max_src_bit ){
			src_bit=1;
			src++;
		} else src_bit <<= 1;

		if( dst_bit >= max_dst_bit ){
			dst_bit=1;
			dst++;
		} else dst_bit <<= 1;
	}
}


static Command mseq_ctbl[]={
	{ "mseq",	do_mseq,	"generate M-sequence"	},
	{ "packed_mseq",	do_packed_mseq,	"generate packed M-sequence"	},
	{ "pack",	do_pack,	"pack N bit subwords into 32 bit long words" },
	{ "quit",	popcmd,		"exit submenu"		},
	{ NULL_COMMAND						}
};

COMMAND_FUNC( mseq_menu )
{
	PUSHCMD(mseq_ctbl,"mseq");
}


	
