#ifndef __JCC_H__
#define __JCC_H__

/* 短跳转：rel8 */
make_helper(je_si_b);
make_helper(jbe_si_b);
make_helper(jle_si_b);
make_helper(jne_si_b);
make_helper(jge_si_b);
make_helper(jl_si_b);
make_helper(jg_si_b);
make_helper(js_si_b);
make_helper(jns_si_b);
make_helper(jb_si_b);
make_helper(ja_si_b);
make_helper(jp_si_b);
make_helper(jnp_si_b);
make_helper(jecxz_si_b);

/* 近跳转：rel32 */
make_helper(je_si_l);
make_helper(jne_si_l);
make_helper(jbe_si_l);
make_helper(ja_si_l);
make_helper(js_si_l);
make_helper(jl_si_l);
make_helper(jge_si_l);
make_helper(jle_si_l);
make_helper(jg_si_l);

#endif