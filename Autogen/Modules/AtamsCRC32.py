class CRC32:
    CRC_RESET_VALUE: int     = 0xFFFFFFFF
    FINAL_XOR_VALUE: int     = 0xFFFFFFFF
    CRC32_POLYNOMIAL: int    = 0x82F63B78
    BITS_IN_A_BYTE: int      = 8
    BYTE_MASK: int           = 0xFF
    DECIMAL_WIDTH_8_BIT: int = 256

    def __init__(self, generatorPolynomial: int = CRC32_POLYNOMIAL) -> None:
        self.crcTable   = self.generateCrcTable(generatorPolynomial)
        self.rollingCrc = self.CRC_RESET_VALUE

    def generateCrcTable(self, poly: int):
        table = []
        for byte in range(self.DECIMAL_WIDTH_8_BIT):
            crc = byte
            for _ in range(self.BITS_IN_A_BYTE):
                if crc & 1:
                    crc = (crc >> 1) ^ (poly & 0xFFFFFFFF)
                else:
                    crc >>= 1
                crc &= 0xFFFFFFFF
            table.append(crc & 0xFFFFFFFF)
        return table

    def calculateCrc(self, byteBuffer: bytes) -> int:
        crc = self.CRC_RESET_VALUE
        for byte in byteBuffer:
            crc = (crc >> self.BITS_IN_A_BYTE) ^ self.crcTable[(crc ^ byte) & self.BYTE_MASK]
            crc &= 0xFFFFFFFF
        return (crc ^ self.FINAL_XOR_VALUE) & 0xFFFFFFFF

    def beginRollingCrc(self) -> None:
        self.rollingCrc = self.CRC_RESET_VALUE

    def updateRollingCrc(self, byte: int) -> None:
        self.rollingCrc = (self.rollingCrc >> self.BITS_IN_A_BYTE) ^ self.crcTable[(self.rollingCrc ^ byte) & self.BYTE_MASK]
        self.rollingCrc &= 0xFFFFFFFF

    def getRollingCrc(self) -> int:
        return (self.rollingCrc ^ self.FINAL_XOR_VALUE) & 0xFFFFFFFF