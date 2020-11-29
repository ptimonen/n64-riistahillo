#include "audio.h"

void Rom2Ram(void *from_addr, void *to_addr, s32 seq_size)
{
    // Cannot transfer if size is an odd number, so make it even
    if(seq_size & 0x00000001)
        seq_size++;

    // Transfer data from the ROM’s address to the buffer provided
    nuPiReadRom((u32)from_addr, to_addr, seq_size);
}

void initAudio(void)
{
    musConfig c;

    c.control_flag = 0; // Set to MUSCONTROL_RAM if wbk file also placed in RAM
    c.channels = NU_AU_CHANNELS; // Maximum total number of channels
    c.sched = NULL; // The address of the Scheduler structure. NuSystem uses NULL
    c.thread_priority = NU_AU_MGR_THREAD_PRI; // Thread priority (highest)
    c.heap = (unsigned char*)NU_AU_HEAP_ADDR; // Heap address
    c.heap_length = NU_AU_HEAP_SIZE; // Heap size
    c.ptr = NULL; // Allows you to set a default ptr file
    c.wbk = NULL; // Allows you to set a default wbk file
    c.default_fxbank = NULL; // Allows you to set a default bfx file
    c.fifo_length = NU_AU_FIFO_LENGTH; // The size of the library's FIFO buffer
    c.syn_updates = NU_AU_SYN_UPDATE_MAX; // Number of updates for the synthesizer.
    c.syn_output_rate = 44100; // Audio output rate: 44100hz
    c.syn_rsp_cmds = NU_AU_CLIST_LEN; // Maximum length of the audio command list.
    c.syn_retraceCount = 1; // The number of frames per retrace message

    // Number of buffers the Audio Manager can use for DMA from ROM transfer.
    c.syn_num_dma_bufs = NU_AU_DMA_BUFFER_NUM;
    c.syn_dma_buf_size = NU_AU_DMA_BUFFER_SIZE;

    // Initialize the Audio Manager.
    nuAuStlMgrInit(&c);

    // Register the PRENMI function.
    nuAuPreNMIFuncSet(nuAuPreNMIProc);

    // Read and register the sample bank and its pointers.
    Rom2Ram((void *)PBANK_START, (void *)ptr_buf,
            PBANK_END-PBANK_START);
    MusPtrBankInitialize(ptr_buf, WBANK_START);

    // Read and register the sound effects.
    Rom2Ram((void *)SFX_START, (void *)sfx_buf,
            SFX_END-SFX_START);
    MusFxBankInitialize(sfx_buf);
}
