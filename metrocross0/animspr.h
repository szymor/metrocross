
// Prototypes.
void Anm_InitEngine(void);
s32 Anm_Set(u64 *pAnm, s32 nSlotNo);
s32 Anm_SetIfNew(u64 *pAnm, s32 nSlotNo);
void Anm_ReleaseSlot(s32 nSlotNo);
s32 Anm_GetImage(s32 nSlotNo);
s32 Anm_GetLastImage(s32 nSlotNo);
u32 Anm_GetKey(s32 nSlotNo);
u32 Anm_CheckEnd(s32 nSlotNo);
u32 Anm_CheckStepFlag(s32 nSlotNo);
u32 Anm_CheckNewImgFlag(s32 nSlotNo);


