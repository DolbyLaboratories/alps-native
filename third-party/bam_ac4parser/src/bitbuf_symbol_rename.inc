#ifdef SYMBOL_PREFIX

    #define PREFIX_JOIN(a,b)          a ## _ ## b
    #define EVALUATE_PREFIX_JOIN(a,b) PREFIX_JOIN(a,b)
    #define APPLY_SYMBOL_PREFIX(sid)  EVALUATE_PREFIX_JOIN(SYMBOL_PREFIX,sid)

    #define dlb_bitbuf_align                  APPLY_SYMBOL_PREFIX(dlb_bitbuf_align)
    #define dlb_bitbuf_get_abs_pos            APPLY_SYMBOL_PREFIX(dlb_bitbuf_get_abs_pos)
    #define dlb_bitbuf_get_alignment_bits     APPLY_SYMBOL_PREFIX(dlb_bitbuf_get_alignment_bits)
    #define dlb_bitbuf_get_bits_left          APPLY_SYMBOL_PREFIX(dlb_bitbuf_get_bits_left)
    #define dlb_bitbuf_init                   APPLY_SYMBOL_PREFIX(dlb_bitbuf_init)
    #define dlb_bitbuf_set_abs_pos            APPLY_SYMBOL_PREFIX(dlb_bitbuf_set_abs_pos)
    #define dlb_bitbuf_skip                   APPLY_SYMBOL_PREFIX(dlb_bitbuf_skip)
    #define skip_bits_forward                 APPLY_SYMBOL_PREFIX(skip_bits_forward)
    #define dlb_bitbuf_fast_peek              APPLY_SYMBOL_PREFIX(dlb_bitbuf_fast_peek)
    #define dlb_bitbuf_fast_peek_long         APPLY_SYMBOL_PREFIX(lb_bitbuf_fast_peek_long)
    #define dlb_bitbuf_fast_read              APPLY_SYMBOL_PREFIX(dlb_bitbuf_fast_read)
    #define dlb_bitbuf_fast_read_long         APPLY_SYMBOL_PREFIX(dlb_bitbuf_fast_read_long)
    #define dlb_bitbuf_get_version            APPLY_SYMBOL_PREFIX(dlb_bitbuf_get_version)
    #define dlb_bitbuf_peek                   APPLY_SYMBOL_PREFIX(dlb_bitbuf_peek)
    #define dlb_bitbuf_peek_long              APPLY_SYMBOL_PREFIX(dlb_bitbuf_peek_long)
    #define dlb_bitbuf_read                   APPLY_SYMBOL_PREFIX(dlb_bitbuf_read)
    #define dlb_bitbuf_read_long              APPLY_SYMBOL_PREFIX(dlb_bitbuf_read_long)
    #define dlb_bitbuf_safe_peek              APPLY_SYMBOL_PREFIX(dlb_bitbuf_safe_peek)
    #define dlb_bitbuf_safe_peek_long         APPLY_SYMBOL_PREFIX(dlb_bitbuf_safe_peek_long)
    #define dlb_bitbuf_safe_read              APPLY_SYMBOL_PREFIX(dlb_bitbuf_safe_read)
    #define dlb_bitbuf_safe_read_long         APPLY_SYMBOL_PREFIX(dlb_bitbuf_safe_read_long)
    #define dlb_bitbuf_write                  APPLY_SYMBOL_PREFIX(dlb_bitbuf_write)
    #define dlb_bitbuf_write_long             APPLY_SYMBOL_PREFIX(dlb_bitbuf_write_long)

#endif
