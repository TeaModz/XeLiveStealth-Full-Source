using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace XeLive.Obfuscation {
    internal class Program
    {
        public static uint dwBaseAddress = 0x90e00000;

        public static uint RandomStuff(uint rand) => (uint)(((((((((((((int)rand + 999 ^ 1337 ^ 4919) + 69 ^ 333 ^ 666) + 123 - 4221 ^ 100 ^ 18) + 999 ^ 1337 ^ 4919) + 69 ^ 333 ^ 666) + 123 - 4237 ^ 100 ^ 18) + 999 ^ 1337 ^ 4919) + 69 ^ 333 ^ 666) + 123 - 4253 ^ 100 ^ 18) + 999 ^ 1337 ^ 4919) + 69 ^ 333 ^ 666) + 123 - 4381 ^ 100 ^ 18);

        public static byte RandomStuffByte(uint rand) => (byte)((((((((((((((int)rand + 999 ^ 1337 ^ 4919) + 69 ^ 333 ^ 666) + 123 - 4221 ^ 100 ^ 18) + 999 ^ 1337 ^ 4919) + 69 ^ 333 ^ 666) + 123 - 4237 ^ 100 ^ 18) + 999 ^ 1337 ^ 4919) + 69 ^ 333 ^ 666) + 123 - 4253 ^ 100 ^ 18) + 999 ^ 1337 ^ 4919) + 69 ^ 333 ^ 666) + 123 - 4381 ^ 100 ^ 18) & (int)byte.MaxValue);

        public static void RC4(ref byte[] Data, byte[] Key, int startOffset = 0, int size = 0)
        {
            int index1 = 0;
            byte[] numArray1 = new byte[256];
            byte[] numArray2 = new byte[256];
            for (int index2 = 0; index2 < 256; ++index2)
            {
                numArray1[index2] = (byte)index2;
                numArray2[index2] = Key[index2 % Key.GetLength(0)];
            }
            for (int index3 = 0; index3 < 256; ++index3)
            {
                index1 = (index1 + (int)numArray1[index3] + (int)numArray2[index3]) % 256;
                byte num = numArray1[index3];
                numArray1[index3] = numArray1[index1];
                numArray1[index1] = num;
            }
            int index4;
            int index5 = index4 = 0;
            if (size == 0)
                size = Data.GetLength(0);
            for (int index6 = 0; index6 < size; ++index6)
            {
                index5 = (index5 + 1) % 256;
                index4 = (index4 + (int)numArray1[index5]) % 256;
                byte num = numArray1[index5];
                numArray1[index5] = numArray1[index4];
                numArray1[index4] = num;
                int index7 = ((int)numArray1[index5] + (int)numArray1[index4]) % 256;
                Data[startOffset + index6] = (byte)((uint)Data[startOffset + index6] ^ (uint)numArray1[index7]);
            }
        }

        public static int SearchByteByByte(byte[] bytes, byte[] pattern)
        {
            for (int index1 = 0; index1 < bytes.Length && bytes.Length - index1 >= pattern.Length; ++index1)
            {
                if ((int)pattern[0] == (int)bytes[index1])
                {
                    for (int index2 = 0; index2 < pattern.Length && (int)bytes[index1 + index2] == (int)pattern[index2]; ++index2)
                    {
                        if (index2 == pattern.Length - 1)
                            return index1;
                    }
                }
            }
            return 0;
        }

        public static XexSectionInfo GetSectionInfo(byte[] file, byte[] name)
        {
            XexSectionInfo sectionInfo = new XexSectionInfo();
            int count = SearchByteByByte(file, name);
            if (count != 0)
            {
                BinaryReader binaryReader = new BinaryReader(new MemoryStream(((IEnumerable<byte>)file).Skip<byte>(count).ToArray<byte>()));
                sectionInfo.szName = binaryReader.ReadBytes(8);
                sectionInfo.dwVirtualSize = binaryReader.ReadUInt32();
                sectionInfo.dwVirtualAddress = binaryReader.ReadUInt32();
                binaryReader.Close();
            }
            return sectionInfo;
        }

        public static byte[] GenerateRandomData(int count)
        {
            byte[] buffer = new byte[count];
            new Random().NextBytes(buffer);
            return buffer;
        }

        public static int FindArrayOffset(byte[] file) => Program.SearchByteByByte(file, new byte[8]
        {
            110,
            105,
            103,
            103,
            101,
            114,
            54,
            57
        });

        private static void Main(string[] args)
        {
            if (File.Exists("decrypted_XeLive.xex"))
            {
                byte[] Data = File.ReadAllBytes("decrypted_XeLive.xex");
                if (Data.Length == 0)
                    return;
                int arrayOffset = Program.FindArrayOffset(Data);
                if (arrayOffset == 0)
                    return;
                Program.XexSectionInfo sectionInfo1 = Program.GetSectionInfo(Data, new byte[5]
                {
          (byte) 46,
          (byte) 116,
          (byte) 101,
          (byte) 120,
          (byte) 116
                });
                Program.XexSectionInfo sectionInfo2 = Program.GetSectionInfo(Data, new byte[5]
                {
          (byte) 46,
          (byte) 100,
          (byte) 97,
          (byte) 116,
          (byte) 97
                });
                Console.WriteLine("[Section] .text - 0x{0:X} to 0x{1:X}", (object)(uint)((int)Program.dwBaseAddress + (int)sectionInfo1.dwVirtualAddress), (object)(uint)((int)Program.dwBaseAddress + (int)sectionInfo1.dwVirtualAddress + (int)sectionInfo1.dwVirtualSize));
                Console.WriteLine("[Section] .data - 0x{0:X} to 0x{1:X}", (object)(uint)((int)Program.dwBaseAddress + (int)sectionInfo2.dwVirtualAddress), (object)(uint)((int)Program.dwBaseAddress + (int)sectionInfo2.dwVirtualAddress + (int)sectionInfo2.dwVirtualSize));
                Random random = new Random();
                byte num1 = (byte)random.Next((int)byte.MaxValue);
                byte[] randomData = Program.GenerateRandomData(48);
                byte num2 = (byte)random.Next((int)byte.MaxValue);
                byte num3 = Program.RandomStuffByte(18U);
                Program.RC4(ref Data, randomData, (int)sectionInfo1.dwVirtualAddress + 4096, (int)sectionInfo1.dwVirtualSize - 1616);
                for (uint index = sectionInfo1.dwVirtualAddress + 4096U; index < (uint)((int)sectionInfo1.dwVirtualAddress + 4096 + (int)sectionInfo1.dwVirtualSize - 1616); ++index)
                    Data[(int)index] ^= num1;
                for (uint index = sectionInfo1.dwVirtualAddress + 4096U; index < (uint)((int)sectionInfo1.dwVirtualAddress + 4096 + (int)sectionInfo1.dwVirtualSize - 1616); ++index)
                    Data[(int)index] ^= num3;
                for (int index = 0; index < 48; ++index)
                    randomData[index] ^= num2;
                byte[] numArray = new byte[4096];
                EndianWriter endianWriter = new EndianWriter((Stream)new MemoryStream(numArray), EndianStyle.BigEndian);
                endianWriter.Write(Program.dwBaseAddress + sectionInfo1.dwVirtualAddress);
                endianWriter.Write(sectionInfo1.dwVirtualSize - 1616U);
                endianWriter.Write(num1);
                endianWriter.Write(num2);
                endianWriter.Write(randomData);
                endianWriter.Close();
                Array.Copy((Array)numArray, 0, (Array)Data, arrayOffset, numArray.Length);
                using (StreamWriter text = File.CreateText("output.xex"))
                    text.BaseStream.Write(Data, 0, Data.Length);
                for (int index = 0; index < 48; ++index)
                    randomData[index] ^= num2;
                using (StreamWriter text = File.CreateText("rc4_key_dec.bin"))
                    text.BaseStream.Write(randomData, 0, randomData.Length);
                using (StreamWriter text = File.CreateText("text.bin"))
                    text.BaseStream.Write(Data, (int)sectionInfo1.dwVirtualAddress + 4096, (int)sectionInfo1.dwVirtualAddress + 4096 + (int)sectionInfo1.dwVirtualSize - 1616);
            }
            else
                Console.WriteLine("Failed to find decrypted_XeLive.xex");
        }

        public struct XexSectionInfo
        {
            public byte[] szName;
            public uint dwVirtualAddress;
            public uint dwVirtualSize;
        }
    }
}
