#ifdef CMD0

MEMORY
{

    RAMM0_BEGIN               : origin = 0x000000, length = 0x000002
    RAMM0                     : origin = 0x000123, length = 0x0002DD
    RAMM1                     : origin = 0x000400, length = 0x0003F8
    CLATOCPU_MSGRAM           : origin = 0x001480, length = 0x000080
    CPUTOCLA_MSGRAM           : origin = 0x001500, length = 0x000080
    RAMLS0                    : origin = 0x008000, length = 0x000800
    RAMLS1                    : origin = 0x008800, length = 0x000800
    RAMLS2                    : origin = 0x009000, length = 0x000800
    RAMLS3                    : origin = 0x009800, length = 0x000800
    RAMLS4                    : origin = 0x00A000, length = 0x000800
    RAMLS5                    : origin = 0x00A800, length = 0x000800
    RAMD0                     : origin = 0x00B000, length = 0x000800
    RAMD1                     : origin = 0x00B800, length = 0x000800
    RAMGS0                    : origin = 0x00C000, length = 0x001000
    RAMGS1                    : origin = 0x00D000, length = 0x001000
    RAMGS2                    : origin = 0x00E000, length = 0x001000
    RAMGS3                    : origin = 0x00F000, length = 0x001000
    RAMGS4                    : origin = 0x010000, length = 0x001000
    RAMGS5                    : origin = 0x011000, length = 0x001000
    RAMGS6                    : origin = 0x012000, length = 0x001000
    RAMGS7                    : origin = 0x013000, length = 0x001000
    RAMGS8                    : origin = 0x014000, length = 0x001000
    RAMGS9                    : origin = 0x015000, length = 0x001000
    RAMGS10                   : origin = 0x016000, length = 0x001000
    RAMGS11                   : origin = 0x017000, length = 0x001000
    RAMGS12                   : origin = 0x018000, length = 0x001000
    RAMGS13                   : origin = 0x019000, length = 0x001000
    RAMGS14                   : origin = 0x01A000, length = 0x001000
    RAMGS15                   : origin = 0x01B000, length = 0x000FF8
    CPU2TOCPU1RAM             : origin = 0x03F800, length = 0x000400
    CPU1TOCPU2RAM             : origin = 0x03FC00, length = 0x000400
    FLASHA                    : origin = 0x080000, length = 0x002000
    FLASHB                    : origin = 0x082000, length = 0x002000
    FLASHC                    : origin = 0x084000, length = 0x002000
    FLASHD                    : origin = 0x086000, length = 0x002000
    FLASHE                    : origin = 0x088000, length = 0x008000
    FLASHF                    : origin = 0x090000, length = 0x008000
    FLASHG                    : origin = 0x098000, length = 0x008000
    FLASHH                    : origin = 0x0A0000, length = 0x008000
    FLASHI                    : origin = 0x0A8000, length = 0x008000
    FLASHJ                    : origin = 0x0B0000, length = 0x008000
    FLASHK                    : origin = 0x0B8000, length = 0x002000
    FLASHL                    : origin = 0x0BA000, length = 0x002000
    FLASHM                    : origin = 0x0BC000, length = 0x002000
    FLASHN                    : origin = 0x0BE000, length = 0x001FF0
    RESET                     : origin = 0x3FFFC0, length = 0x000002
}


SECTIONS
{
    //
    // C28x Sections
    //
    .reset               : >  RESET, TYPE = DSECT /* not used, */
    codestart            : >  0x000000

    //
    // CLA Sections
    //
    Cla1Prog             : >  RAMLS5
    .const_cla           : >  RAMLS1
    .scratchpad          : >  RAMLS1
    cla1ToCpuMsgRAM      : >  CLATOCPU_MSGRAM
    cpuToCla1MsgRAM      : >  CPUTOCLA_MSGRAM
    MSGRAM_CPU1_TO_CPU2 : > CPU1TOCPU2RAM, type=NOINIT
    MSGRAM_CPU2_TO_CPU1 : > CPU2TOCPU1RAM, type=NOINIT

}

#endif

/*
//===========================================================================
// End of file.
//===========================================================================
*/
