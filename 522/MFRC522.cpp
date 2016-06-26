/*
* MFRC522.cpp - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
* _Please_ see the comments in MFRC522.h - they give useful hints and background.
* Released into the public domain.
*/

#include <Arduino.h>
#include <MFRC522.h>

/////////////////////////////////////////////////////////////////////////////////////
// Functions for setting up the Arduino
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 * Prepares the output pins.
 */
MFRC522::MFRC522(
		byte chipSelectPin,	///< Arduino pin connected to MFRC522's SPI slave select input (Pin 24, NSS, active low)
		byte resetPowerDownPin)	///< Arduino pin connected to MFRC522's reset and power down input (Pin 6, NRSTPD, active low)
{
	// Set the chipSelectPin as digital output, do not select the slave yet
	_chipSelectPin = chipSelectPin;
	pinMode(_chipSelectPin, OUTPUT);
	digitalWrite(_chipSelectPin, HIGH);
	
	// Set the resetPowerDownPin as digital output, do not reset or power down.
	_resetPowerDownPin = resetPowerDownPin;
	pinMode(_resetPowerDownPin, OUTPUT);
	digitalWrite(_resetPowerDownPin, LOW);
	
	// Set SPI bus to work with MFRC522 chip.
	setSPIConfig();
}

/**
 * Set SPI bus to work with MFRC522 chip.
 * Please call this function if you have changed the SPI config since the MFRC522 constructor was run.
 */
void MFRC522::setSPIConfig() 
{
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
}

/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Writes a byte to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_WriteRegister(byte reg, byte value)
{
	digitalWrite(_chipSelectPin, LOW);		// Select slave
	SPI.transfer(reg & 0x7E);				// MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
	SPI.transfer(value);
	digitalWrite(_chipSelectPin, HIGH);		// Release slave again
}

/**
 * Writes a number of bytes to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_WriteRegister(byte reg, byte count, const byte *values) 
{
	digitalWrite(_chipSelectPin, LOW);		// Select slave
	SPI.transfer(reg & 0x7E);				// MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
	for (int index = 0; index < count; index++)
		SPI.transfer(values[index]);
	digitalWrite(_chipSelectPin, HIGH);		// Release slave again
}

/**
 * Reads a byte from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
byte MFRC522::PCD_ReadRegister(byte reg)	///< The register to read from. One of the PCD_Register enums.
{
	digitalWrite(_chipSelectPin, LOW);			// Select slave
	SPI.transfer(0x80 | (reg & 0x7E));			// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	byte value = SPI.transfer(0);				// Read the value back. Send 0 to stop reading.
	digitalWrite(_chipSelectPin, HIGH);			// Release slave again
	return value;
}

/**
 * Reads a number of bytes from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_ReadRegister(byte reg,		///< The register to read from. One of the PCD_Register enums.
                               byte count,		///< The number of bytes to read
                               byte *values,	///< Byte array to store the values in.
                               byte rxAlign)	///< Only bit positions rxAlign..7 in values[0] are updated.
{
	if (count == 0)
		return;

	//Serial.print("Reading "); 	Serial.print(count); Serial.println(" bytes from register.");
	byte address = 0x80 | (reg & 0x7E);		// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	byte index = 0;							// Index in values array.
	digitalWrite(_chipSelectPin, LOW);		// Select slave
	count--;								// One read is performed outside of the loop
	SPI.transfer(address);					// Tell MFRC522 which address we want to read
	while (index < count) 
	{
		if (index == 0 && rxAlign) 
		{ // Only update bit positions rxAlign..7 in values[0]
			// Create bit mask for bit positions rxAlign..7
			byte mask = 0;
			for (int i = rxAlign; i <= 7; i++)
				mask |= (1 << i);

			// Read value and tell that we want to read the same address again.
			byte value = SPI.transfer(address);	
			// Apply mask to both current value of values[0] and the new data in value.
			values[0] = (values[index] & ~mask) | (value & mask);
		}
		else  // Normal case
			values[index] = SPI.transfer(address);	// Read value and tell that we want to read the same address again.
		index++;
	}
	values[index] = SPI.transfer(0);			// Read the final byte. Send 0 to stop reading.
	digitalWrite(_chipSelectPin, HIGH);			// Release slave again
}

/**
 * Sets the bits given in mask in register reg.
 */
void MFRC522::PCD_SetRegisterBitMask(byte reg, byte mask)
{ 
	byte tmp = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, tmp | mask);			// set bit mask
}

/**
 * Clears the bits given in mask from register reg.
 */
void MFRC522::PCD_ClearRegisterBitMask(byte reg, byte mask) 
{
	byte tmp = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, tmp & (~mask));		// clear bit mask
}


/**
 * Use the CRC coprocessor in the MFRC522 to calculate a CRC_A.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_CalculateCRC(const byte *data, byte length, byte *result) 
{
	PCD_WriteRegister(CommandReg, PCD_Idle);		// Stop any active command.
	PCD_WriteRegister(DivIrqReg, 0x04);				// Clear the CRCIRq interrupt request bit
	PCD_SetRegisterBitMask(FIFOLevelReg, 0x80);		// FlushBuffer = 1, FIFO initialization
	PCD_WriteRegister(FIFODataReg, length, data);	// Write data to the FIFO
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);		// Start the calculation
	
	// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73us.
	int i = 5000;
	while (1) {
		byte n = PCD_ReadRegister(DivIrqReg);	// DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq   reserved CRCIRq reserved reserved
		if (n & 0x04) 						// CRCIRq bit set - calculation done
			break;
		if (--i == 0) 						// The emergency break. We will eventually terminate on this one after 89ms. Communication with the MFRC522 might be down.
			return STATUS_TIMEOUT;
	}
	PCD_WriteRegister(CommandReg, PCD_Idle);			// Stop calculating CRC for new content in the FIFO.
	
	// Transfer the result from the registers to the result buffer
	result[0] = PCD_ReadRegister(CRCResultRegL);
	result[1] = PCD_ReadRegister(CRCResultRegH);
	return STATUS_OK;
}


/////////////////////////////////////////////////////////////////////////////////////
// Functions for manipulating the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Initializes the MFRC522 chip.
 */
void MFRC522::PCD_Init() 
{
	if (digitalRead(_resetPowerDownPin) == LOW) 
	{ //The MFRC522 chip is in power down mode.
		digitalWrite(_resetPowerDownPin, HIGH);	// Exit power down mode. This triggers a hard reset.
		// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74�s. Let us be generous: 50ms.
		delay(50);
	}
	else { // Perform a soft reset
		PCD_Reset();
	}
	
	// When communicating with a PICC we need a timeout if something goes wrong.
	// f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
	// TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
    PCD_WriteRegister(TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
    PCD_WriteRegister(TPrescalerReg, 0xA9);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25us.
    PCD_WriteRegister(TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
    PCD_WriteRegister(TReloadRegL, 0xE8);
	
	PCD_WriteRegister(TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	PCD_WriteRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
	PCD_AntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
}

/**
 * Performs a soft reset on the MFRC522 chip and waits for it to be ready again.
 */
void MFRC522::PCD_Reset() 
{
	PCD_WriteRegister(CommandReg, PCD_SoftReset);	// Issue the SoftReset command.
	// The datasheet does not mention how long the SoftRest command takes to complete.
	// But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg) 
	// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74us. Let us be generous: 50ms.
	delay(50);
	// Wait for the PowerDown bit in CommandReg to be cleared
	while (PCD_ReadRegister(CommandReg) & (1<<4)) 
	{
		// PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
		delay(5);
	}
}

/**
 * Turns the antenna on by enabling pins TX1 and TX2.
 * After a reset these pins disabled.
 */
void MFRC522::PCD_AntennaOn() 
{
	byte value = PCD_ReadRegister(TxControlReg);
	if ((value & 0x03) != 0x03)
		PCD_WriteRegister(TxControlReg, value | 0x03);
}

byte MFRC522::PCD_GetVersion() 
{
	byte value = PCD_ReadRegister(VersionReg);
	return value;
}
/////////////////////////////////////////////////////////////////////////////////////
// Functions for communicating with PICCs
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Executes the Transceive command.
 * CRC validation can only be done if backData and backLen are specified.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_TransceiveData(
		const byte *sendData,///< Pointer to the data to transfer to the FIFO.
		byte sendLen,		///< Number of bytes to transfer to the FIFO.
		byte *backData,		///< nullptr or pointer to buffer if data should be read back after executing the command.
		byte *backLen,		///< In: Max number of bytes to write to *backData. Out: The number of bytes returned.
		byte *validBits,	///< In/Out: The number of valid bits in the last byte. 0 for 8 valid bits. Default nullptr.
		byte rxAlign,		///< In: Defines the bit position in backData[0] for the first bit received. Default 0.
		bool checkCRC)		///< In: True => The last two bytes of the response is assumed to be a CRC_A that must be validated.
{
	byte waitIRq = 0x30;		// RxIRq and IdleIRq
	return PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC);
}

/**
 * Transfers data to the MFRC522 FIFO, executes a commend, waits for completion and transfers data back from the FIFO.
 * CRC validation can only be done if backData and backLen are specified.
 *
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_CommunicateWithPICC(
		byte command,		///< The command to execute. One of the PCD_Command enums.
		byte waitIRq,		///< The bits in the ComIrqReg register that signals successful completion of the command.
		const byte *sendData,		///< Pointer to the data to transfer to the FIFO.
		byte sendLen,		///< Number of bytes to transfer to the FIFO.
		byte *backData,		///< nullptr or pointer to buffer if data should be read back after executing the command.
		byte *backLen,		///< In: Max number of bytes to write to *backData. Out: The number of bytes returned.
		byte *validBits,	///< In/Out: The number of valid bits in the last byte. 0 for 8 valid bits.
		byte rxAlign,		///< In: Defines the bit position in backData[0] for the first bit received. Default 0.
		bool checkCRC)		///< In: True => The last two bytes of the response is assumed to be a CRC_A that must be validated.
{
	byte n, _validBits = 0;

	// Prepare values for BitFramingReg
	byte txLastBits = validBits ? *validBits : 0;
	byte bitFraming	= (rxAlign << 4) + txLastBits;		// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]

	PCD_WriteRegister(CommandReg, PCD_Idle);			// Stop any active command.
	PCD_WriteRegister(ComIrqReg, 0x7F);					// Clear all seven interrupt request bits
	PCD_SetRegisterBitMask(FIFOLevelReg, 0x80);		// FlushBuffer = 1, FIFO initialization
	PCD_WriteRegister(FIFODataReg, sendLen, sendData);	// Write sendData to the FIFO
	PCD_WriteRegister(BitFramingReg, bitFraming);		// Bit adjustments
	PCD_WriteRegister(CommandReg, command);			// Execute the command
	if (command == PCD_Transceive)
		PCD_SetRegisterBitMask(BitFramingReg, 0x80);	// StartSend=1, transmission of data starts

	// Wait for the command to complete.
	// In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
	// Each iteration of the do-while-loop takes 17.86�s.
	int i = 2000;
	while (1) 
	{
		n = PCD_ReadRegister(ComIrqReg);	// ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq   HiAlertIRq LoAlertIRq ErrIRq TimerIRq
		if (n & waitIRq) 					// One of the interrupts that signal success has been set.
			break;

		if (n & 0x01) 						// Timer interrupt - nothing received in 25ms
			return STATUS_TIMEOUT;

		if (--i == 0) 						// The emergency break. If all other condions fail we will eventually terminate on this one after 35.7ms. Communication with the MFRC522 might be down.
			return STATUS_TIMEOUT;
	}

	// Stop now if any errors except collisions were detected.
	byte errorRegValue = PCD_ReadRegister(ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl   CollErr CRCErr ParityErr ProtocolErr
	if (errorRegValue & 0x13) 	 // BufferOvfl ParityErr ProtocolErr
		return STATUS_ERROR;

	// If the caller wants data back, get it from the MFRC522.
	if (backData && backLen) 
	{
		n = PCD_ReadRegister(FIFOLevelReg);						// Number of bytes in the FIFO
		if (n > *backLen)
			return STATUS_NO_ROOM;

		*backLen = n;												// Number of bytes returned
		PCD_ReadRegister(FIFODataReg, n, backData, rxAlign);		// Get received data from FIFO
		_validBits = PCD_ReadRegister(ControlReg) & 0x07;	// RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
		if (validBits)
			*validBits = _validBits;
	}

	// Tell about collisions
	if (errorRegValue & 0x08) // CollErr
		return STATUS_COLLISION;

	// Perform CRC_A validation if requested.
	if (backData && backLen && checkCRC) 
	{
		// In this case a MIFARE Classic NAK is not OK.
		if (*backLen == 1 && _validBits == 4)
			return STATUS_MIFARE_NACK;

		// We need at least the CRC_A value and all 8 bits of the last byte must be received.
		if (*backLen < 2 || _validBits != 0)
			return STATUS_CRC_WRONG;

		// Verify CRC_A - do our own calculation and store the control in controlBuffer.
		byte controlBuffer[2]; 
		StatusCode st = PCD_CalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
		if (st != STATUS_OK)
			return st;

		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1]))
			return STATUS_CRC_WRONG;
	}

	return STATUS_OK;
}

/**
 * Transmits a REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
 * Beware: When two PICCs are in the field at the same time I often get STATUS_TIMEOUT - probably due do bad antenna design.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_RequestA(
		byte *bufferATQA,	///< The buffer to store the ATQA (Answer to request) in
		byte *bufferSize)	///< Buffer size, at least two bytes. Also number of bytes returned if STATUS_OK.
{
	return PICC_REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
}

/**
 * Transmits a Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
 * Beware: When two PICCs are in the field at the same time I often get STATUS_TIMEOUT - probably due do bad antenna design.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_WakeupA(
		byte *bufferATQA,	///< The buffer to store the ATQA (Answer to request) in
		byte *bufferSize)	///< Buffer size, at least two bytes. Also number of bytes returned if STATUS_OK.
{
	return PICC_REQA_or_WUPA(PICC_CMD_WUPA, bufferATQA, bufferSize);
}

/**
 * Transmits REQA or WUPA commands.
 * Beware: When two PICCs are in the field at the same time I often get STATUS_TIMEOUT - probably due do bad antenna design.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */ 
MFRC522::StatusCode MFRC522::PICC_REQA_or_WUPA(
		byte command, 		///< The command to send - PICC_CMD_REQA or PICC_CMD_WUPA
		byte *bufferATQA,	///< The buffer to store the ATQA (Answer to request) in
		byte *bufferSize)	///< Buffer size, at least two bytes. Also number of bytes returned if STATUS_OK.
{
	if (bufferATQA == nullptr || *bufferSize < 2) 	// The ATQA response is 2 bytes long.
		return STATUS_NO_ROOM;
	
	PCD_ClearRegisterBitMask(CollReg, 0x80);			// ValuesAfterColl=1 => Bits received after collision are cleared.
	byte validBits = 7;										// For REQA and WUPA we need the short frame format - transmit only 7 bits of the last (and only) byte. TxLastBits = BitFramingReg[2..0]
	StatusCode status = PCD_TransceiveData(&command, 1, bufferATQA, bufferSize, &validBits);
	if (status != STATUS_OK) 
		return status;

	if (*bufferSize != 2 || validBits != 0) 		// ATQA must be exactly 16 bits.
		return STATUS_ERROR;
	
	return STATUS_OK;
}

/**
 * Transmits SELECT/ANTICOLLISION commands to select a single PICC.
 * Before calling this function the PICCs must be placed in the READY(*) state by calling PICC_RequestA() or PICC_WakeupA().
 * On success:
 * 	- The chosen PICC is in state ACTIVE(*) and all other PICCs have returned to state IDLE/HALT. (Figure 7 of the ISO/IEC 14443-3 draft.)
 * 	- The UID size and value of the chosen PICC is returned in *uid along with the SAK.
 * 
 * A PICC UID consists of 4, 7 or 10 bytes.
 * Only 4 bytes can be specified in a SELECT command, so for the longer UIDs two or three iterations are used:
 * 		UID size	Number of UID bytes		Cascade levels		Example of PICC
 * 		========	===================		==============		===============
 * 		single				 4						1				MIFARE Classic
 * 		double				 7						2				MIFARE Ultralight
 * 		triple				10						3				Not currently in use?
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PICC_Select(
		Uid *uid,			///< Pointer to Uid struct. Normally output, but can also be used to supply a known UID.
		byte validBits)		///< The number of known UID bits supplied in *uid. Normally 0. If set you must also supply uid->size.
{
	bool uidComplete;
	bool selectDone;
	bool useCascadeTag;
	byte cascadeLevel	= 1; 
	StatusCode result;
	byte count;
	byte index;
	byte uidIndex;					// The first index in uid->uidByte[] that is used in the current Cascade Level.
	int currentLevelKnownBits;		// The number of known UID bits in the current Cascade Level.
	byte buffer[9];					// The SELECT/ANTICOLLISION commands uses a 7 byte standard frame + 2 bytes CRC_A
	byte bufferUsed;				// The number of bytes used in the buffer, ie the number of bytes to transfer to the FIFO.
	byte rxAlign;					// Used in BitFramingReg. Defines the bit position for the first bit received.
	byte txLastBits;				// Used in BitFramingReg. The number of valid bits in the last transmitted byte. 
	byte *responseBuffer;
	byte responseLength;

	// Description of buffer structure:
	// 		Byte 0: SEL 				Indicates the Cascade Level: PICC_CMD_SEL_CL1, PICC_CMD_SEL_CL2 or PICC_CMD_SEL_CL3
	// 		Byte 1: NVB					Number of Valid Bits (in complete command, not just the UID): High nibble: complete bytes, Low nibble: Extra bits. 
	// 		Byte 2: UID-data or CT		See explanation below. CT means Cascade Tag.
	// 		Byte 3: UID-data
	// 		Byte 4: UID-data
	// 		Byte 5: UID-data
	// 		Byte 6: BCC					Block Check Character - XOR of bytes 2-5
	//		Byte 7: CRC_A
	//		Byte 8: CRC_A
	// The BCC and CRC_A is only transmitted if we know all the UID bits of the current Cascade Level.
	//
	// Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft: UID contents and cascade levels)
	//		UID size	Cascade level	Byte2	Byte3	Byte4	Byte5
	//		========	=============	=====	=====	=====	=====
	//		 4 bytes		1			uid0	uid1	uid2	uid3
	//		 7 bytes		1			CT		uid0	uid1	uid2
	//						2			uid3	uid4	uid5	uid6
	//		10 bytes		1			CT		uid0	uid1	uid2
	//						2			CT		uid3	uid4	uid5
	//						3			uid6	uid7	uid8	uid9

	// Sanity checks
	if (validBits > 80)
		return STATUS_INVALID;


	// Prepare MFRC522
	PCD_ClearRegisterBitMask(CollReg, 0x80);			// ValuesAfterColl=1 => Bits received after collision are cleared.

	// Repeat Cascade Level loop until we have a complete UID.
	uidComplete = false;
	while (!uidComplete) 
	{
		// Set the Cascade Level in the SEL byte, find out if we need to use the Cascade Tag in byte 2.
		switch (cascadeLevel) 
		{
		case 1:
			buffer[0] = PICC_CMD_SEL_CL1;
			uidIndex = 0;
			useCascadeTag = validBits && uid->size > 4;	// When we know that the UID has more than 4 bytes
			break;

		case 2:
			buffer[0] = PICC_CMD_SEL_CL2;
			uidIndex = 3;
			useCascadeTag = validBits && uid->size > 7;	// When we know that the UID has more than 7 bytes
			break;

		case 3:
			buffer[0] = PICC_CMD_SEL_CL3;
			uidIndex = 6;
			useCascadeTag = false;						// Never used in CL3.
			break;

		default:
			return STATUS_INTERNAL_ERROR;
			break;
		}

		// How many UID bits are known in this Cascade Level?
		currentLevelKnownBits = validBits - (8 * uidIndex);
		if (currentLevelKnownBits < 0)
			currentLevelKnownBits = 0;

		// Copy the known bits from uid->uidByte[] to buffer[]
		index = 2; // destination index in buffer[]
		if (useCascadeTag)
			buffer[index++] = PICC_CMD_CT;
		
		byte bytesToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1 : 0); // The number of bytes needed to represent the known bits for this level.
		if (bytesToCopy) 
		{
			byte maxBytes = useCascadeTag ? 3 : 4; // Max 4 bytes in each Cascade Level. Only 3 left if we use the Cascade Tag
			if (bytesToCopy > maxBytes)
				bytesToCopy = maxBytes;

			for (count = 0; count < bytesToCopy; count++)
				buffer[index++] = uid->uidByte[uidIndex + count];
		}
		// Now that the data has been copied we need to include the 8 bits in CT in currentLevelKnownBits
		if (useCascadeTag) 
			currentLevelKnownBits += 8;

		// Repeat anti collision loop until we can transmit all UID bits + BCC and receive a SAK - max 32 iterations.
		selectDone = false;
		while (!selectDone) 
		{
			// Find out how many bits and bytes to send and receive.
			if (currentLevelKnownBits >= 32) 
			{ // All UID bits in this Cascade Level are known. This is a SELECT.
				//Serial.print("SELECT: currentLevelKnownBits="); Serial.println(currentLevelKnownBits, DEC);
				buffer[1] = 0x70; // NVB - Number of Valid Bits: Seven whole bytes
				// Calulate BCC - Block Check Character
				buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
				// Calculate CRC_A
				result = PCD_CalculateCRC(buffer, 7, &buffer[7]);
				if (result != STATUS_OK) {
					return result;
				}
				txLastBits		= 0; // 0 => All 8 bits are valid.
				bufferUsed		= 9;
				// Store response in the last 3 bytes of buffer (BCC and CRC_A - not needed after tx)
				responseBuffer	= &buffer[6];
				responseLength	= 3;
			}
			else 
			{ // This is an ANTICOLLISION.
				//Serial.print("ANTICOLLISION: currentLevelKnownBits="); Serial.println(currentLevelKnownBits, DEC);
				txLastBits		= currentLevelKnownBits % 8;
				count			= currentLevelKnownBits / 8;	// Number of whole bytes in the UID part.
				index			= 2 + count;					// Number of whole bytes: SEL + NVB + UIDs
				buffer[1]		= (index << 4) + txLastBits;	// NVB - Number of Valid Bits
				bufferUsed		= index + (txLastBits ? 1 : 0);
				// Store response in the unused part of buffer
				responseBuffer	= &buffer[index];
				responseLength	= sizeof(buffer) - index;
			}

			// Set bit adjustments
			rxAlign = txLastBits;											// Having a seperate variable is overkill. But it makes the next line easier to read.
			PCD_WriteRegister(BitFramingReg, (rxAlign << 4) + txLastBits);	// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]

			// Transmit the buffer and receive the response.
			result = PCD_TransceiveData(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign);			
			if (result == STATUS_COLLISION) 
			{ // More than one PICC in the field => collision.
				byte collreg = PCD_ReadRegister(CollReg); // CollReg[7..0] bits are: ValuesAfterColl reserved CollPosNotValid CollPos[4:0]
				if (collreg & 0x20)  // CollPosNotValid
					return STATUS_COLLISION; // Without a valid collision position we cannot continue
				
				byte collisionPos = collreg & 0x1F; // Values 0-31, 0 means bit 32.
				if (collisionPos == 0) 
					collisionPos = 32;
				
				if (collisionPos <= currentLevelKnownBits)  // No progress - should not happen 
					return STATUS_INTERNAL_ERROR;
				
				// Choose the PICC with the bit set.
				currentLevelKnownBits = collisionPos;
				count			= (currentLevelKnownBits - 1) % 8; // The bit to modify
				index			= 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0); // First byte is index 0.
				buffer[index]	|= (1 << count); 
			}
			else if (result != STATUS_OK)
				return result;
			else 
			{ // STATUS_OK
				if (currentLevelKnownBits >= 32) { // This was a SELECT.
					selectDone = true; // No more anticollision 
					// We continue below outside the while.
				}
				else { // This was an ANTICOLLISION.
					// We now have all 32 bits of the UID in this Cascade Level
					currentLevelKnownBits = 32;
					// Run loop again to do the SELECT.
				}
			}
		}

		// We do not check the CBB - it was constructed by us above.

		// Copy the found UID bytes from buffer[] to uid->uidByte[]
		index       = (buffer[2] == PICC_CMD_CT) ? 3 : 2; // source index in buffer[]
		bytesToCopy = (buffer[2] == PICC_CMD_CT) ? 3 : 4;
		for (count = 0; count < bytesToCopy; count++) 
			uid->uidByte[uidIndex + count] = buffer[index++];


		// Check response SAK (Select Acknowledge)
		if (responseLength != 3 || txLastBits != 0) 		// SAK must be exactly 24 bits (1 byte + CRC_A).
			return STATUS_ERROR;

		// Verify CRC_A - do our own calculation and store the control in buffer[2..3] - those bytes are not needed anymore.
		result = PCD_CalculateCRC(responseBuffer, 1, &buffer[2]);
		if (result != STATUS_OK) 
			return result;

		if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2]))
			return STATUS_CRC_WRONG;

		if (responseBuffer[0] & 0x04)  // Cascade bit set - UID not complete yes
			cascadeLevel++;
		else 
		{
			uidComplete = true;
			uid->sak = responseBuffer[0];
		}
	} // End of while ( ! uidComplete)

	// Set correct uid->size
	uid->size = 3 * cascadeLevel + 1;

	return STATUS_OK;
}

/**
 * Instructs a PICC in state ACTIVE(*) to go to state HALT.
 *
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */ 
MFRC522::StatusCode MFRC522::PICC_HaltA() 
{
	byte buffer[4]; 

	// Build command buffer
	buffer[0] = PICC_CMD_HLTA;
	buffer[1] = 0;
	// Calculate CRC_A
	StatusCode result = PCD_CalculateCRC(buffer, 2, &buffer[2]);
	if (result != STATUS_OK)
		return result;

	// Send the command.
	// The standard says:
	//		If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
	//		HLTA command, this response shall be interpreted as 'not acknowledge'.
	// We interpret that this way: Only STATUS_TIMEOUT is an success.
	result = PCD_TransceiveData(buffer, sizeof(buffer), nullptr, nullptr);
	if (result == STATUS_TIMEOUT)
		return STATUS_OK;
	if (result == STATUS_OK) // That is ironically NOT ok in this case ;-)
		return STATUS_ERROR;
	return result;
}


/////////////////////////////////////////////////////////////////////////////////////
// Functions for communicating with MIFARE PICCs
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Executes the MFRC522 MFAuthent command.
 * This command manages MIFARE authentication to enable a secure communication to any MIFARE Mini, MIFARE 1K and MIFARE 4K card.
 * The authentication is described in the MFRC522 datasheet section 10.3.1.9 and http://www.nxp.com/documents/data_sheet/MF1S503x.pdf section 10.1.
 * For use with MIFARE Classic PICCs.
 * The PICC must be selected - ie in state ACTIVE(*) - before calling this function.
 * Remember to call PCD_StopCrypto1() after communicating with the authenticated PICC - otherwise no new communications can start.
 * 
 * All keys are set to FFFFFFFFFFFFh at chip delivery.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise. Probably STATUS_TIMEOUT if you supply the wrong key.
 */
MFRC522::StatusCode MFRC522::PCD_Authenticate(
		byte command,		///< PICC_CMD_MF_AUTH_KEY_A or PICC_CMD_MF_AUTH_KEY_B
		byte blockAddr, 	///< The block number. See numbering in the comments in the .h file.
		MIFARE_Key *key,	///< Pointer to the Crypteo1 key to use (6 bytes)
		const Uid *uid)			///< Pointer to Uid struct. The first 4 bytes of the UID is used.
{
	// Build command buffer
	byte sendData[12];
	sendData[0] = command;
	sendData[1] = blockAddr;

	for (int i = 0; i < MF_KEY_SIZE; i++) 	// 6 key bytes
		sendData[2+i] = key->keyByte[i];

	for (int i = 0; i < 4; i++) 				// The first 4 bytes of the UID
		sendData[8+i] = uid->uidByte[i];

	// Start the authentication.
	byte waitIRq = 0x10;		// IdleIRq
	return PCD_CommunicateWithPICC(PCD_MFAuthent, waitIRq, &sendData[0], sizeof(sendData));
}

/**
 * Used to exit the PCD from its authenticated state.
 * Remember to call this function after communicating with an authenticated PICC - otherwise no new communications can start.
 */
void MFRC522::PCD_StopCrypto1() 
{
	// Clear MFCrypto1On bit
	PCD_ClearRegisterBitMask(Status2Reg, 0x08); // Status2Reg[7..0] bits are: TempSensClear I2CForceHS reserved reserved   MFCrypto1On ModemState[2:0]
}

/**
 * Reads 16 bytes (+ 2 bytes CRC_A) from the active PICC.
 * 
 * For MIFARE Classic the sector containing the block must be authenticated before calling this function.
 * 
 * For MIFARE Ultralight only addresses 00h to 0Fh are decoded.
 * The MF0ICU1 returns a NAK for higher addresses.
 * The MF0ICU1 responds to the READ command by sending 16 bytes starting from the page address defined by the command argument.
 * For example; if blockAddr is 03h then pages 03h, 04h, 05h, 06h are returned.
 * A roll-back is implemented: If blockAddr is 0Eh, then the contents of pages 0Eh, 0Fh, 00h and 01h are returned.
 * 
 * The buffer must be at least 18 bytes because a CRC_A is also returned.
 * Checks the CRC_A before returning STATUS_OK.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Read(
		byte blockAddr, 	///< MIFARE Classic: The block (0-0xff) number. MIFARE Ultralight: The first page to return data from.
		byte *buffer,		///< The buffer to store the data in
		byte *bufferSize)	///< Buffer size, at least 18 bytes. Also number of bytes returned if STATUS_OK.
{
	// Sanity check
	if (buffer == nullptr || *bufferSize < 18)
		return STATUS_NO_ROOM;

	// Build command buffer
	buffer[0] = PICC_CMD_MF_READ;
	buffer[1] = blockAddr;

	// Calculate CRC_A
	StatusCode result = PCD_CalculateCRC(buffer, 2, &buffer[2]);
	if (result != STATUS_OK)
		return result;

	// Transmit the buffer and receive the response, validate CRC_A.
	result = PCD_TransceiveData(buffer, 4, buffer, bufferSize, nullptr, 0, true);
	return result;
}

/**
 * Writes 16 bytes to the active PICC.
 * 
 * For MIFARE Classic the sector containing the block must be authenticated before calling this function.
 * 
 * For MIFARE Ultralight the opretaion is called "COMPATIBILITY WRITE".
 * Even though 16 bytes are transferred to the Ultralight PICC, only the least significant 4 bytes (bytes 0 to 3)
 * are written to the specified address. It is recommended to set the remaining bytes 04h to 0Fh to all logic 0.
 * * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Write(
		byte blockAddr,		///< MIFARE Classic: The block (0-0xff) number. MIFARE Ultralight: The page (2-15) to write to.
		const byte *buffer,	///< The 16 bytes to write to the PICC
		byte bufferSize)	///< Buffer size, must be at least 16 bytes. Exactly 16 bytes are written.
{
	// Sanity check
	if (buffer == nullptr || bufferSize < 16)
		return STATUS_INVALID;

	// Mifare Classic protocol requires two communications to perform a write.
	// Step 1: Tell the PICC we want to write to block blockAddr.
	byte cmdBuffer[2];
	cmdBuffer[0] = PICC_CMD_MF_WRITE;
	cmdBuffer[1] = blockAddr;
	StatusCode result = PCD_MIFARE_Transceive(cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
	if (result != STATUS_OK)
		return result;

	// Step 2: Transfer the data
	result = PCD_MIFARE_Transceive(buffer, bufferSize); // Adds CRC_A and checks that the response is MF_ACK.
	return result;
}

/**
 * Writes a 4 byte page to the active MIFARE Ultralight PICC.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Ultralight_Write(
		byte page, 		///< The page (2-15) to write to.
		byte *buffer,	///< The 4 bytes to write to the PICC
		byte bufferSize)///< Buffer size, must be at least 4 bytes. Exactly 4 bytes are written.
{
	// Sanity check
	if (buffer == nullptr || bufferSize < 4)
		return STATUS_INVALID;

	// Build commmand buffer
	byte cmdBuffer[6];
	cmdBuffer[0] = PICC_CMD_UL_WRITE;
	cmdBuffer[1] = page;
	memcpy(&cmdBuffer[2], buffer, 4); // LEO c'è qualcosa che non va...

	// Perform the write
	StatusCode result = PCD_MIFARE_Transceive(cmdBuffer, 6); // Adds CRC_A and checks that the response is MF_ACK.
	return result;
}

/**
 * MIFARE Decrement subtracts the delta from the value of the addressed block, and stores the result in a volatile memory.
 * For MIFARE Classic only. The sector containing the block must be authenticated before calling this function.
 * Only for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * Use MIFARE_Transfer() to store the result in a block.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Decrement(
		byte blockAddr, ///< The block (0-0xff) number.
		long delta)		///< This number is subtracted from the value of block blockAddr.
{
	return MIFARE_TwoStepHelper(PICC_CMD_MF_DECREMENT, blockAddr, delta);
}

/**
 * MIFARE Increment adds the delta to the value of the addressed block, and stores the result in a volatile memory.
 * For MIFARE Classic only. The sector containing the block must be authenticated before calling this function.
 * Only for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * Use MIFARE_Transfer() to store the result in a block.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Increment(
		byte blockAddr, ///< The block (0-0xff) number.
		long delta)		///< This number is added to the value of block blockAddr.
{
	return MIFARE_TwoStepHelper(PICC_CMD_MF_INCREMENT, blockAddr, delta);
}

/**
 * MIFARE Restore copies the value of the addressed block into a volatile memory.
 * For MIFARE Classic only. The sector containing the block must be authenticated before calling this function.
 * Only for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * Use MIFARE_Transfer() to store the result in a block.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Restore(byte blockAddr) ///< The block (0-0xff) number.
{
	// The datasheet describes Restore as a two step operation, but does not explain what data to transfer in step 2.
	// Doing only a single step does not work, so I chose to transfer 0L in step two.
	return MIFARE_TwoStepHelper(PICC_CMD_MF_RESTORE, blockAddr, 0L);
}

/**
 * Helper function for the two-step MIFARE Classic protocol operations Decrement, Increment and Restore.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_TwoStepHelper(
		byte command,	///< The command to use
		byte blockAddr,	///< The block (0-0xff) number.
		long data)		///< The data to transfer in step 2
{
	// Step 1: Tell the PICC the command and block address
	byte cmdBuffer[2]; // We only need room for 2 bytes.
	cmdBuffer[0] = command;
	cmdBuffer[1] = blockAddr;
	StatusCode result = PCD_MIFARE_Transceive(cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
	if (result != STATUS_OK)
		return result;

	// Step 2: Transfer the data
	result = PCD_MIFARE_Transceive((byte *)&data, 4, true); // Adds CRC_A and accept timeout as success.
	return result;
}

/**
 * MIFARE Transfer writes the value stored in the volatile memory into one MIFARE Classic block.
 * For MIFARE Classic only. The sector containing the block must be authenticated before calling this function.
 * Only for blocks in "value block" mode, ie with access bits [C1 C2 C3] = [110] or [001].
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::MIFARE_Transfer(byte blockAddr) ///< The block (0-0xff) number.
{
	// Tell the PICC we want to transfer the result into block blockAddr.
	byte cmdBuffer[2]; // We only need room for 2 bytes.
	cmdBuffer[0] = PICC_CMD_MF_TRANSFER;
	cmdBuffer[1] = blockAddr;
	StatusCode result = PCD_MIFARE_Transceive(cmdBuffer, 2); // Adds CRC_A and checks that the response is MF_ACK.
	return result;
}


/////////////////////////////////////////////////////////////////////////////////////
// Support functions
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Wrapper for MIFARE protocol communication.
 * Adds CRC_A, executes the Transceive command and checks that the response is MF_ACK or a timeout.
 * 
 * @return STATUS_OK on success, STATUS_??? otherwise.
 */
MFRC522::StatusCode MFRC522::PCD_MIFARE_Transceive(
		const byte *sendData,	///< Pointer to the data to transfer to the FIFO. Do NOT include the CRC_A.
		byte sendLen,			///< Number of bytes in sendData.
		bool acceptTimeout)		///< True => A timeout is also success
{
	StatusCode result;
	byte cmdBuffer[18]; // We need room for 16 bytes data and 2 bytes CRC_A.

	// Sanity check
	if (sendData == nullptr || sendLen > 16)
		return STATUS_INVALID;
	
	// Copy sendData[] to cmdBuffer[] and add CRC_A
	memcpy(cmdBuffer, sendData, sendLen);
	result = PCD_CalculateCRC(cmdBuffer, sendLen, &cmdBuffer[sendLen]);
	if (result != STATUS_OK)
		return result;
	sendLen += 2;
	
	// Transceive the data, store the reply in cmdBuffer[]
	byte waitIRq = 0x30;		// RxIRq and IdleIRq
	byte cmdBufferSize = sizeof(cmdBuffer);
	byte validBits = 0;
	result = PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, cmdBuffer, sendLen, cmdBuffer, &cmdBufferSize, &validBits);
	if (acceptTimeout && result == STATUS_TIMEOUT) 
		return STATUS_OK;
	if (result != STATUS_OK)
		return result;
	
	// The PICC must reply with a 4 bit ACK
	if (cmdBufferSize != 1 || validBits != 4)
		return STATUS_ERROR;
	
	if (cmdBuffer[0] != MF_ACK) 
		return STATUS_MIFARE_NACK;
	
	return STATUS_OK;
}

/**
 * Returns a string pointer to a status code name.
 * 
 */
const char *MFRC522::GetStatusCodeName(StatusCode code)
{
	switch (code) 
	{
	case STATUS_OK:				return "Success."; break;
	case STATUS_ERROR:			return "Error in communication."; break;
	case STATUS_COLLISION:		return "Collission detected."; break;
	case STATUS_TIMEOUT:		return "Timeout in communication."; break;
	case STATUS_NO_ROOM:		return "A buffer is not big enough."; break;
	case STATUS_INTERNAL_ERROR:	return "Internal error in the code. Should not happen."; break;
	case STATUS_INVALID:		return "Invalid argument."; break;
	case STATUS_CRC_WRONG:		return "The CRC_A does not match."; break;
	case STATUS_MIFARE_NACK:	return "A MIFARE PICC responded with NAK."; break;
	default:                    return "Unknown error";
	}
}

/**
 * Translates the SAK (Select Acknowledge) to a PICC type.
 * 
 * @return PICC_Type
 */
MFRC522::PICC_Type MFRC522::PICC_GetType(
		byte sak		///< The SAK byte returned from PICC_Select().
			) 
{
	if (sak & 0x04)  // UID not complete
		return PICC_TYPE_NOT_COMPLETE;
	
	switch (sak) 
	{
	case 0x00:	return PICC_TYPE_MIFARE_UL;		break;
	case 0x09:	return PICC_TYPE_MIFARE_MINI;	break;
	case 0x08:	return PICC_TYPE_MIFARE_1K;		break;
	case 0x18:	return PICC_TYPE_MIFARE_4K;		break;
	case 0x10:
	case 0x11:	return PICC_TYPE_MIFARE_PLUS;	break;
	case 0x20:	return PICC_TYPE_MIFARE_DESFIRE;break;
	case 0x01:	return PICC_TYPE_TNP3XXX;		break;
	default:	break;
	}
	
	if (sak & 0x20) return PICC_TYPE_ISO_14443_4;
	if (sak & 0x40) return PICC_TYPE_ISO_18092;
	return PICC_TYPE_UNKNOWN;
}

/**
 * Returns a string pointer to the PICC type name.
 * 
 */
const char *MFRC522::PICC_GetTypeName(byte piccType	///< One of the PICC_Type enums.
										) {
	switch (piccType) 
	{
	case PICC_TYPE_ISO_14443_4:		return "PICC compliant with ISO/IEC 14443-4";		break;
	case PICC_TYPE_ISO_18092:		return "PICC compliant with ISO/IEC 18092 (NFC)";	break;
	case PICC_TYPE_MIFARE_MINI:		return "MIFARE Mini, 320 bytes";					break;
	case PICC_TYPE_MIFARE_1K:		return "MIFARE 1KB";								break;
	case PICC_TYPE_MIFARE_4K:		return "MIFARE 4KB";								break;
	case PICC_TYPE_MIFARE_UL:		return "MIFARE Ultralight or Ultralight C";			break;
	case PICC_TYPE_MIFARE_PLUS:		return "MIFARE Plus";								break;
	case PICC_TYPE_TNP3XXX:			return "MIFARE TNP3XXX";							break;
	case PICC_TYPE_NOT_COMPLETE:	return "SAK indicates UID is not complete.";		break;
	case PICC_TYPE_MIFARE_DESFIRE:	return "MIFARE DESFIRE";
	case PICC_TYPE_UNKNOWN:
	default:						return "Unknown type";								break;
	}
}

/**
 * Dumps debug info about the selected PICC to Serial.
 * On success the PICC is halted after dumping the data.
 * For MIFARE Classic the factory default key of 0xFFFFFFFFFFFF is tried. 
 */
void MFRC522::PICC_DumpToSerial(const Uid *uid) 
{
	MIFARE_Key key;

	uid->Dump();
	
	// UID
	Serial.print("Card UID:");
	for (int i = 0; i < uid->size; i++) 
	{
		Serial.print(uid->uidByte[i] < 0x10 ? " 0" : " ");
		Serial.print(uid->uidByte[i], HEX);
	} 
	Serial.println();

	// PICC type
	byte piccType = PICC_GetType(uid->sak);
	Serial.print("PICC type: ");
	Serial.println(PICC_GetTypeName(piccType));

	if (true)  // LEO ritorno senza il dump della carta.
		return;
	
	// Dump contents
	switch (piccType) 
	{
	case PICC_TYPE_MIFARE_MINI:
	case PICC_TYPE_MIFARE_1K:
	case PICC_TYPE_MIFARE_4K:
		// All keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
		for (int i = 0; i < 6; i++)
			key.keyByte[i] = 0xFF;
		PICC_DumpMifareClassicToSerial(uid, piccType, &key);
		break;

	case PICC_TYPE_MIFARE_UL:
		PICC_DumpMifareUltralightToSerial();
		break;

	case PICC_TYPE_ISO_14443_4:	
	case PICC_TYPE_ISO_18092:
	case PICC_TYPE_MIFARE_PLUS:
	case PICC_TYPE_TNP3XXX:
		Serial.print("Dumping memory contents not implemented for that PICC type.");
		Serial.println(piccType, HEX);
		break;

	case PICC_TYPE_MIFARE_DESFIRE:
		Serial.println("DEVO ANCORA FARLO IL DUMP");
		break;

	case PICC_TYPE_UNKNOWN:
	case PICC_TYPE_NOT_COMPLETE:
	default:
		break; // No memory dump here
	}

	Serial.println();
	PICC_HaltA(); // Already done if it was a MIFARE Classic PICC.
}

/**
 * Dumps memory contents of a MIFARE Classic PICC.
 * On success the PICC is halted after dumping the data.
 */
void MFRC522::PICC_DumpMifareClassicToSerial(
		const Uid *uid,		///< Pointer to Uid struct returned from a successful PICC_Select().
		byte piccType,	///< One of the PICC_Type enums.
		MIFARE_Key *key)///< Key A used for all sectors.
{
	byte no_of_sectors = 0;
	switch (piccType) 
	{
	case PICC_TYPE_MIFARE_MINI:
		// Has 5 sectors * 4 blocks/sector * 16 bytes/block = 320 bytes.
		no_of_sectors = 5;
		break;

	case PICC_TYPE_MIFARE_1K:
		// Has 16 sectors * 4 blocks/sector * 16 bytes/block = 1024 bytes.
		no_of_sectors = 16;
		break;

	case PICC_TYPE_MIFARE_4K:
		// Has (32 sectors * 4 blocks/sector + 8 sectors * 16 blocks/sector) * 16 bytes/block = 4096 bytes.
		no_of_sectors = 40;
		break;

	default: // Should not happen. Ignore.
		break; 
	}
	
	// Dump sectors, highest address first.
	if (no_of_sectors) 
	{
		Serial.println("Sector Block   0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15  AccessBits");
		for (int i = no_of_sectors - 1; i >= 0; i--)
			PICC_DumpMifareClassicSectorToSerial(uid, key, i);
	}
	PICC_HaltA(); // Halt the PICC before stopping the encrypted session.
	PCD_StopCrypto1();
}

/**
 * Dumps memory contents of a sector of a MIFARE Classic PICC.
 * Uses PCD_Authenticate(), MIFARE_Read() and PCD_StopCrypto1.
 * Always uses PICC_CMD_MF_AUTH_KEY_A because only Key A can always read the sector trailer access bits.
 */
void MFRC522::PICC_DumpMifareClassicSectorToSerial(
		const Uid *uid,		///< Pointer to Uid struct returned from a successful PICC_Select().
		MIFARE_Key *key,	///< Key A for the sector.
		byte sector)		///< The sector to dump, 0..39.
{
	StatusCode status;
	byte firstBlock;		// Address of lowest address to dump actually last block dumped)
	byte no_of_blocks;		// Number of blocks in sector
	bool isSectorTrailer;	// Set to true while handling the "last" (ie highest address) in the sector.

	// The access bits are stored in a peculiar fashion.
	// There are four groups:
	//		g[3]	Access bits for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
	//		g[2]	Access bits for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
	//		g[1]	Access bits for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
	//		g[0]	Access bits for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
	// Each group has access bits [C1 C2 C3]. In this code C1 is MSB and C3 is LSB.
	// The four CX bits are stored together in a nible cx and an inverted nible cx_.
	byte c1, c2, c3;		// Nibbles
	byte c1_, c2_, c3_;		// Inverted nibbles
	bool invertedError = false;		// True if one of the inverted nibbles did not match
	byte g[4];				// Access bits for each of the four groups.
	byte group;				// 0-3 - active group for access bits
	bool firstInGroup;		// True for the first block dumped in the group

	// Determine position and size of sector.
	if (sector < 32) { // Sectors 0..31 has 4 blocks each
		no_of_blocks = 4;
		firstBlock = sector * no_of_blocks;
	}
	else if (sector < 40) { // Sectors 32-39 has 16 blocks each
		no_of_blocks = 16;
		firstBlock = 128 + (sector - 32) * no_of_blocks;
	}
	else { // Illegal input, no MIFARE Classic PICC has more than 40 sectors.
		return;
	}

	// Dump blocks, highest address first.
	byte byteCount;
	byte buffer[18];
	byte blockAddr;
	isSectorTrailer = true;
	for (int blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset--) {
		blockAddr = firstBlock + blockOffset;
		// Sector number - only on first line
		if (isSectorTrailer) {
			Serial.print(sector < 10 ? "   " : "  "); // Pad with spaces
			Serial.print(sector);
			Serial.print("   ");
		}
		else {
			Serial.print("       ");
		}
		// Block number
		Serial.print(blockAddr < 10 ? "   " : (blockAddr < 100 ? "  "	 : " ")); // Pad with spaces
		Serial.print(blockAddr);
		Serial.print("  ");
		// Establish encrypted communications before reading the first block
		if (isSectorTrailer) {
			status = PCD_Authenticate(PICC_CMD_MF_AUTH_KEY_A, firstBlock, key, uid);
			if (status != STATUS_OK) {
				Serial.print("PCD_Authenticate() failed: ");
				Serial.println(GetStatusCodeName(status));
				return;
			}
		}
		// Read block
		byteCount = sizeof(buffer);
		status = MIFARE_Read(blockAddr, buffer, &byteCount);
		if (status != STATUS_OK) {
			Serial.print("(1) MIFARE_Read() failed: ");
			Serial.println(GetStatusCodeName(status));
			continue;
		}
		// Dump data
		for (int index = 0; index < 16; index++) {
			Serial.print(buffer[index] < 0x10 ? " 0" : " ");
			Serial.print(buffer[index], HEX);
			if ((index % 4) == 3) {
				Serial.print(" ");
			}
		}
		// Parse sector trailer data
		if (isSectorTrailer) {
			c1  = buffer[7] >> 4;
			c2  = buffer[8] & 0xF;
			c3  = buffer[8] >> 4;
			c1_ = buffer[6] & 0xF;
			c2_ = buffer[6] >> 4;
			c3_ = buffer[7] & 0xF;
			invertedError = (c1 != (~c1_ & 0xF)) || (c2 != (~c2_ & 0xF)) || (c3 != (~c3_ & 0xF));
			g[0] = ((c1 & 1) << 2) | ((c2 & 1) << 1) | ((c3 & 1) << 0);
			g[1] = ((c1 & 2) << 1) | ((c2 & 2) << 0) | ((c3 & 2) >> 1);
			g[2] = ((c1 & 4) << 0) | ((c2 & 4) >> 1) | ((c3 & 4) >> 2);
			g[3] = ((c1 & 8) >> 1) | ((c2 & 8) >> 2) | ((c3 & 8) >> 3);
			isSectorTrailer = false;
		}

		// Which access group is this block in?
		if (no_of_blocks == 4) {
			group = blockOffset;
			firstInGroup = true;
		}
		else {
			group = blockOffset / 5;
			firstInGroup = (group == 3) || (group != (blockOffset + 1) / 5);
		}

		if (firstInGroup) {
			// Print access bits
			Serial.print(" [ ");
			Serial.print((g[group] >> 2) & 1, DEC); Serial.print(" ");
			Serial.print((g[group] >> 1) & 1, DEC); Serial.print(" ");
			Serial.print((g[group] >> 0) & 1, DEC);
			Serial.print(" ] ");
			if (invertedError) {
				Serial.print(" Inverted access bits did not match! ");
			}
		}

		if (group != 3 && (g[group] == 1 || g[group] == 6)) { // Not a sector trailer, a value block
			long value = (long(buffer[3])<<24) | (long(buffer[2])<<16) | (long(buffer[1])<<8) | long(buffer[0]);
			Serial.print(" Value=0x"); Serial.print(value, HEX);
			Serial.print(" Adr=0x"); Serial.print(buffer[12], HEX);
		}
		Serial.println();
	}

	return;
}

/**
 * Dumps memory contents of a MIFARE Ultralight PICC.
 */
void MFRC522::PICC_DumpMifareUltralightToSerial() 
{
	Serial.println("Page  0  1  2  3");
	// Try the mpages of the original Ultralight. Ultralight C has more pages.

	for (int page = 0; page < 16; page +=4) 
	{ 
		// Read returns data for 4 pages at a time.
		// Read pages
		byte buffer[18];
		byte byteCount = sizeof(buffer);
		StatusCode status = MIFARE_Read(page, buffer, &byteCount);
		if (status != STATUS_OK) 
		{
			Serial.print("(2) MIFARE_Read() failed: ");
			Serial.println(GetStatusCodeName(status));
			break;
		}

		// Dump data
		for (int offset = 0; offset < 4; offset++) 
		{
			byte i = page + offset;
			Serial.print(i < 10 ? "  " : " "); // Pad with spaces
			Serial.print(i);
			Serial.print("  ");
			for (int index = 0; index < 4; index++) 
			{
				i = 4 * offset + index;
				Serial.print(buffer[i] < 0x10 ? " 0" : " ");
				Serial.print(buffer[i], HEX);
			}
			Serial.println();
		}
	}
}

/**
 * Calculates the bit pattern needed for the specified access bits. In the [C1 C2 C3] tupples C1 is MSB (=4) and C3 is LSB (=1).
 */
void MFRC522::MIFARE_SetAccessBits(	byte *accessBitBuffer,	///< Pointer to byte 6, 7 and 8 in the sector trailer. Bytes [0..2] will be set.
									byte g0,				///< Access bits [C1 C2 C3] for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
									byte g1,				///< Access bits C1 C2 C3] for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
									byte g2,				///< Access bits C1 C2 C3] for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
									byte g3					///< Access bits C1 C2 C3] for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
								) 
{
	byte c1 = ((g3 & 4) << 1) | ((g2 & 4) << 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
	byte c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) << 0) | ((g0 & 2) >> 1);
	byte c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) << 0);
	
	accessBitBuffer[0] = (~c2 & 0xF) << 4 | (~c1 & 0xF);
	accessBitBuffer[1] =          c1 << 4 | (~c3 & 0xF);
	accessBitBuffer[2] =          c3 << 4 | c2;
}

/////////////////////////////////////////////////////////////////////////////////////
// Convenience functions - does not add extra functionality
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Returns true if a PICC responds to PICC_CMD_REQA.
 * Only "new" cards in state IDLE are invited. Sleeping cards in state HALT are ignored.
 * 
 * @return bool
 */
bool MFRC522::PICC_IsNewCardPresent() 
{
	byte bufferATQA[2];
	byte bufferSize = sizeof(bufferATQA);
	StatusCode result = PICC_RequestA(bufferATQA, &bufferSize);

	//if (result == STATUS_OK || result == STATUS_COLLISION)
	//	printf("ATQA=%02x-%02x\n", bufferATQA[0], bufferATQA[1]);

	return (result == STATUS_OK || result == STATUS_COLLISION);
}

/**
 * Simple wrapper around PICC_Select.
 * Returns true if a UID could be read.
 * Remember to call PICC_IsNewCardPresent(), PICC_RequestA() or PICC_WakeupA() first.
 * The read UID is available in the class variable uid.
 * 
 * @return bool
 */
bool MFRC522::PICC_ReadCardSerial() 
{
	StatusCode result = PICC_Select(&uid);
	return (result == STATUS_OK);
}
 
