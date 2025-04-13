class CRC32:
    CRC_RESET_VALUE: int     = 0xFFFFFFFF
    FINAL_XOR_VALUE: int     = 0xFFFFFFFF
    CRC32_POLYNOMIAL: int    = 0x04C11DB7
    BITS_IN_A_BYTE: int      = 8
    BYTE_MASK: int           = 0xFF
    DECIMAL_WIDTH_8_BIT: int = 256
    NUMBER_OF_CRC_BITS: int  = 32

    def __init__(self, generator_polynomial: int = CRC32_POLYNOMIAL) -> None:
        self._crc_table   = self._generate_crc_table(generator_polynomial)
        self._rolling_crc = self.CRC_RESET_VALUE

    def _generate_crc_table(self, poly: int):
        table = []
        for byte in range(self.DECIMAL_WIDTH_8_BIT):
            crc = byte
            for _ in range(self.BITS_IN_A_BYTE):
                if crc & 1:
                    crc = (crc >> 1) ^ poly
                else:
                    crc >>= 1
            table.append(crc)
        return table

    def _reflect(self, data: int, bit_count: int) -> int:
        reflection = 0
        for i in range(bit_count):
            if data & (1 << i):
                reflection |= (1 << (bit_count - 1 - i))
        return reflection

    def calculate_crc(self, byte_buffer: bytes) -> int:
        crc = self.CRC_RESET_VALUE
        for byte in byte_buffer:
            crc = (crc >> self.BITS_IN_A_BYTE) ^ self._crc_table[(crc ^ byte) & self.BYTE_MASK]
        return self._reflect(crc ^ self.FINAL_XOR_VALUE, self.NUMBER_OF_CRC_BITS)

    def begin_rolling_crc(self) -> None:
        self._rolling_crc = self.CRC_RESET_VALUE

    def update_rolling_crc(self, byte: int) -> None:
        self._rolling_crc = (self._rolling_crc >> self.BITS_IN_A_BYTE) ^ self._crc_table[(self._rolling_crc ^ byte) & self.BYTE_MASK]

    def get_rolling_crc(self) -> int:
        return self._reflect(self._rolling_crc ^ self.FINAL_XOR_VALUE, self.NUMBER_OF_CRC_BITS)