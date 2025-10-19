class CRC32:
    CRC_RESET_VALUE: int     = 0xFFFFFFFF
    FINAL_XOR_VALUE: int     = 0xFFFFFFFF
    CRC32_POLYNOMIAL: int    = 0x04C11DB7
    BITS_IN_A_BYTE: int      = 8
    BYTE_MASK: int           = 0xFF
    DECIMAL_WIDTH_8_BIT: int = 256
    NUMBER_OF_CRC_BITS: int  = 32

    def __init__(self, generatorPolynomial: int = CRC32_POLYNOMIAL) -> None:
        self.crcTable   = self.generateCrcTable(generatorPolynomial)
        self.rollingCrc = self.CRC_RESET_VALUE

    def generateCrcTable(self, poly: int):
        table = []
        for byte in range(self.DECIMAL_WIDTH_8_BIT):
            crc = byte
            for _ in range(self.BITS_IN_A_BYTE):
                if crc & 1:
                    crc = (crc >> 1) ^ (poly & 0xFFFFFFFF)  # Mask polynomial to 32 bits
                else:
                    crc >>= 1
                crc &= 0xFFFFFFFF  # Ensure intermediate CRC remains within 32 bits
            table.append(crc & 0xFFFFFFFF)  # Mask table entry to 32 bits
        return table

    def reflect(self, data: int, bitCount: int) -> int:
        reflection = 0
        for i in range(bitCount):
            if data & (1 << i):
                reflection |= (1 << (bitCount - 1 - i))
        return reflection & 0xFFFFFFFF  # Ensure reflection remains within 32 bits

    def calculateCrc(self, byteBuffer: bytes) -> int:
        crc = self.CRC_RESET_VALUE
        for byte in byteBuffer:
            crc = (crc >> self.BITS_IN_A_BYTE) ^ self.crcTable[(crc ^ byte) & self.BYTE_MASK]
            crc &= 0xFFFFFFFF  # Ensure intermediate CRC remains within 32 bits
        return self.reflect(crc ^ self.FINAL_XOR_VALUE, self.NUMBER_OF_CRC_BITS) & 0xFFFFFFFF  # Mask final CRC to 32 bits

    def beginRollingCrc(self) -> None:
        self.rollingCrc = self.CRC_RESET_VALUE

    def updateRollingCrc(self, byte: int) -> None:
        byte &= 0xFFFFFFFF  # Ensure the input value is treated as uint32_t
        self.rollingCrc = (self.rollingCrc >> self.BITS_IN_A_BYTE) ^ self.crcTable[(self.rollingCrc ^ byte) & self.BYTE_MASK]
        self.rollingCrc &= 0xFFFFFFFF  # Ensure the CRC value remains within 32 bits

    def getRollingCrc(self) -> int:
        return self.reflect(self.rollingCrc ^ self.FINAL_XOR_VALUE, self.NUMBER_OF_CRC_BITS) & 0xFFFFFFFF  # Return the CRC value constrained to 32 bits