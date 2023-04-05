/*---------------------------------------------------------------------------------
    overly simple ARM7 code for dumping firmware from flash chip
---------------------------------------------------------------------------------*/
#include <nds.h>

#include <nds/bios.h>
#include "dump.h"

#include "../../common/fifoChannels.h"

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
}

//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------
    // Reset the clock if needed
    rtcReset();

    irqInit();
    fifoInit();
    irqSet(IRQ_VBLANK, VblankHandler);
    irqEnable(IRQ_VBLANK);
	installSystemFIFO();

    fifoSendValue32(FIFO_RETURN, 1); // notify ARM9 that things ready

    // Keep the ARM7 out of main RAM
    while (1)
    {
        swiWaitForVBlank();

        if(fifoCheckValue32(FIFO_CONTROL)) {
            u32 dumpOption = fifoGetValue32(FIFO_CONTROL);
            u32 ret = 0;
            u32 mailAddr = fifoGetValue32(FIFO_BUFFER_ADDR);
            u32 mailSize = fifoGetValue32(FIFO_BUFFER_SIZE);
            if(dumpOption == DSBF_DUMP_FW) {
                readFirmware(0, (void *)mailAddr, mailSize);
                ret = 524288;
            } else if(dumpOption == DSBF_DUMP_BIOS7) {
                readBios((u8 *)mailAddr, 0, mailSize);
                ret = mailSize;
            } else if (dumpOption == DSBF_EXIT)
                // just exit, not like anything is alloc'd here
                return 0;
            fifoSendValue32(FIFO_RETURN, ret);
        }
    }
}
