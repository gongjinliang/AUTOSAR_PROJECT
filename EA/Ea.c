#include "Ea_MemMap.h"
#include "Ea.h"
#include "SchM_Ea.h"
#include "Det.h"
#include "Ea_Cbk.h"


static uint16 local_BlockNumber;
static uint16 local_BlockOffset;
static uint8* local_DataBufferPtr;
static uint16 local_Length;

static MemIf_StatusType MemIf_Status = MEMIF_UNINIT;
static MemIf_JobResultType MemIf_JobResult ;

static MemIf_ModeType MemIf_Mode;

static uint8 Local_ChangeModeFlag;

static uint16 Local_PhysicalAddress;
static uint8 Local_NumberOfPhysicalPagesPerBlock;

//////////////////// Ea_Init Funftion ////////////////////////////

/**
*@func	init function 
*@brief Initializes the EEPROM abstraction module.
*@param ConfigPtr : Pointer to the selected configuration set..
*/
extern void Ea_Init(const Ea_ConfigType* ConfigPtr)
{
	MemIf_Status = 		MEMIF_BUSY_INTERNAL;
	
	local_BlockNumber = 0;
	local_BlockOffset = 0;
	local_DataBufferPtr = NULL_PTR;
	local_Length = 0;
	Local_PhysicalAddress=0;
	MemIf_Mode = MEMIF_MODE_SLOW;
	Local_ChangeModeFlag=0;
	
	Local_NumberOfPhysicalPagesPerBlock=(EaBlockSize / EaVirtualPageSize);	
	
	if ( (EaBlockSize > EaVirtualPageSize) && ( (EaBlockSize % EaVirtualPageSize) > 0 ) )
	{
	Local_NumberOfPhysicalPagesPerBlock+=1;
	}
	
	MemIf_Status = MEMIF_IDLE;
}


//////////////////// Ea_SetMode Funftion ////////////////////////////

#ifdef EaSetModeSupported

/**
*@func	SetMode function 
*@brief Sets the mode of reading and writing operations.
*@param Mode : The mode of opertaion, type MemIf_ModeType, Range:[MEMIF_MODE_SLOW,MEMIF_MODE_FAST].
*@return 	Void
*/

extern void Ea_SetMode(MemIf_ModeType Mode)
{
	#if EaDevErrorDetect == true
		if(MemIf_Status == MEMIF_UNINIT)
		{
			//raise the development error EA_E_UNINIT
			return	;
		}
		
		if(MemIf_Status == MEMIF_BUSY)
		{
			//raise the development error EA_E_BUSY
			return	;
		}
		
	#endif
	
	if((MemIf_Status == MEMIF_IDLE) || (MemIf_Status == MEMIF_BUSY_INTERNAL))
	{
		MemIf_Mode = Mode ;
		Local_ChangeModeFlag =1; // to be excecuted asynchronously inside the main function 
	}
}


#endif

//////////////////// Ea_Read Funftion ////////////////////////////

/**
*@func	read function 
*@brief Reads Length bytes of block Blocknumber at offset BlockOffset into the buffer DataBufferPtr. 
*@param BlockNumber : Number of logical block, also denoting start address of that block in EEPROM.
*@param BlockOffset : Read address offset inside the block.
*@param DataBufferPtr : Pointer to data buffer.
*@param Length : Number of bytes to read.
*@return 	E_OK: The requested job has been accepted by the module.
*			E_NOT_OK: The requested job has not been accepted by the EA module.
*/
extern Std_ReturnType Ea_Read(uint16 BlockNumber,uint16 BlockOffset,uint8* DataBufferPtr,uint16 Length)
{
	if(MemIf_Status == MEMIF_UNINIT)
	{
		return E_NOT_OK;
	}
	#if EaDevErrorDetect == true
		if(MemIf_Status == MEMIF_UNINIT)
		{
			//raise the development error EA_E_UNINIT
			return E_NOT_OK;
		}
		if(MemIf_Status == MEMIF_BUSY)
		{
			// raise the development error EA_E_BUSY 
			return E_NOT_OK;
		}
		if((blockNumber == 0) || (blockNumber == 0xffff) )
		{
			// raise the development error EA_E_INVALID_BLOCK_NO
			return E_NOT_OK;
		}
		if(BlockOffset >= EaBlockNumber )
		{
			// raise the development error EA_E_INVALID_BLOCK_OFS
			return E_NOT_OK;
		}
	/*	if((Length+BlockOffset) >= EaBlockNumber )
		{
			// raise the development error EA_E_INVALID_BLOCK_LEN
			return E_NOT_OK;
		}*/
		if(DataBufferPtr == NULL_PTR  )
		{
			// raise the development error EA_E_PARAM_POINTER
			return E_NOT_OK;
		}
	#endif
	if((MemIf_Status == MEMIF_IDLE) || (MemIf_Status == MEMIF_BUSY_INTERNAL))
	{
		local_BlockNumber = BlockNumber;
		local_BlockOffset = BlockOffset;
		local_DataBufferPtr = DataBufferPtr;
		local_Length = Length;
		
		MemIf_Status = MEMIF_BUSY;
		MemIf_JobResult = MEMIF_JOB_PENDING;
		return E_OK;
	}
}

//////////////////// Ea_Write Funftion ////////////////////////////
extern Std_ReturnType Ea_Write(uint16 BlockNumber,const uint8* DataBufferPtr)
{
	if((MemIf_Status == MEMIF_IDLE) || (MemIf_Status == MEMIF_BUSY_INTERNAL))
	{
		Local_PhysicalAddress = ( (BlockNumber - 1 ) * EaVirtualPageSize * Local_NumberOfPhysicalPagesPerBlock ) ;
		local_Length = EaBlockSize;
		local_DataBufferPtr = DataBufferPtr ;
		
		MemIf_Status = MEMIF_BUSY;
		MemIf_JobResult = MEMIF_JOB_PENDING ;
		
		return E_OK;
	}
	
	else
	{
		#if EaDevErrorDetect == true
			if(MemIf_Status == MEMIF_UNINIT)
			{
				//raise the development error EA_E_UNINIT
				return E_NOT_OK;
			}
			if(MemIf_Status == MEMIF_BUSY)
			{
				// raise the development error EA_E_BUSY 
				return E_NOT_OK;
			}
			if((blockNumber == 0) || (blockNumber == 0xffff) )
			{
				// raise the development error EA_E_INVALID_BLOCK_NO
				return E_NOT_OK;
			}
			if(DataBufferPtr == NULL_PTR  )
			{
				// raise the development error EA_E_PARAM_POINTER
				return E_NOT_OK;
			}
			
		#endif

		return E_NOT_OK;

	}
}

//////////////////// Ea_Cancel Funftion ////////////////////////////
extern void Ea_Cancel(void)
{
}

//////////////////// Ea_GetStatus Funftion ////////////////////////////
extern MemIf_StatusType Ea_GetStatus(void)
{
}

//////////////////// Ea_GetJobResult Funftion ////////////////////////////
extern MemIf_JobResultType Ea_GetJobResult(void)
{
}

//////////////////// Ea_InvalidateBlock Funftion ////////////////////////////
extern Std_ReturnType Ea_InvalidateBlock(uint16 BlockNumber)
{
}

//////////////////// Ea_GetVersionInfo Funftion ////////////////////////////
extern void Ea_GetVersionInfo(Std_VersionInfoType* VersionInfoPtr)
{
}

//////////////////// Ea_EraseImmediateBlock Funftion ////////////////////////////
extern Std_ReturnType Ea_EraseImmediateBlock(uint16 BlockNumber)
{
}

//////////////////// Ea_JobEndNotification Funftion ////////////////////////////
extern void Ea_JobEndNotification(void)
{
}

//////////////////// Ea_JobErrorNotification Funftion ////////////////////////////
extern void Ea_JobErrorNotification(void)
{
}

//////////////////// Ea_MainFunction Funftion ////////////////////////////
extern void Ea_MainFunction(void)
{
}