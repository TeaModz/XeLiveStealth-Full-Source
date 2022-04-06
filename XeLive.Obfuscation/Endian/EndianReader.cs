using System;
using System.IO;

namespace XeLive.Obfuscation
{
  internal class EndianReader : BinaryReader
  {
    private readonly EndianStyle _endianStyle;

    public EndianReader(Stream Stream, EndianStyle EndianStyle)
      : base(Stream)
    {
      this._endianStyle = EndianStyle;
    }

    public override double ReadDouble() => this.ReadDouble(this._endianStyle);

    public double ReadDouble(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(8);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      return BitConverter.ToDouble(numArray, 0);
    }

    public override short ReadInt16() => this.ReadInt16(this._endianStyle);

    public short ReadInt16(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(2);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      return BitConverter.ToInt16(numArray, 0);
    }

    public int ReadInt24() => this.ReadInt24(this._endianStyle);

    public int ReadInt24(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(3);
      return EndianStyle == EndianStyle.BigEndian ? (int) numArray[0] << 16 | (int) numArray[1] << 8 | (int) numArray[2] : (int) numArray[2] << 16 | (int) numArray[1] << 8 | (int) numArray[0];
    }

    public override int ReadInt32() => this.ReadInt32(this._endianStyle);

    public int ReadInt32(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(4);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      return BitConverter.ToInt32(numArray, 0);
    }

    public int ReadInt32Seek(int seek) => this.ReadInt32Seek_(seek, this._endianStyle);

    public int ReadInt32Seek_(int seek, EndianStyle EndianStyle)
    {
      long position = this.BaseStream.Position;
      this.BaseStream.Position = (long) seek;
      byte[] numArray = this.ReadBytes(4);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      this.BaseStream.Position = position;
      return BitConverter.ToInt32(numArray, 0);
    }

    public override long ReadInt64() => this.ReadInt64(this._endianStyle);

    public long ReadInt64(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(8);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      return BitConverter.ToInt64(numArray, 0);
    }

    public override float ReadSingle() => this.ReadSingle(this._endianStyle);

    public float ReadSingle(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(4);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      return BitConverter.ToSingle(numArray, 0);
    }

    public string ReadStringNullTerminated()
    {
      string empty = string.Empty;
      while (true)
      {
        byte num = this.ReadByte();
        if (num != (byte) 0)
          empty += ((char) num).ToString();
        else
          break;
      }
      return empty;
    }

    public override ushort ReadUInt16() => this.ReadUInt16(this._endianStyle);

    public ushort ReadUInt16(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(2);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      return BitConverter.ToUInt16(numArray, 0);
    }

    public override uint ReadUInt32() => this.ReadUInt32(this._endianStyle);

    public uint ReadUInt32(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(4);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      return BitConverter.ToUInt32(numArray, 0);
    }

    public override ulong ReadUInt64() => this.ReadUInt64(this._endianStyle);

    public ulong ReadUInt64(EndianStyle EndianStyle)
    {
      byte[] numArray = this.ReadBytes(8);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) numArray);
      return BitConverter.ToUInt64(numArray, 0);
    }

    public string ReadUnicodeNullTermString()
    {
      string empty = string.Empty;
      while (true)
      {
        ushort num = this.ReadUInt16(EndianStyle.BigEndian);
        if (num != (ushort) 0)
          empty += ((char) num).ToString();
        else
          break;
      }
      return empty;
    }

    public string ReadUnicodeString(int Length) => this.ReadUnicodeString(Length, this._endianStyle);

    public string ReadUnicodeString(int length, EndianStyle endianStyle)
    {
      string empty = string.Empty;
      int num1 = 0;
      for (int index = 0; index < length; ++index)
      {
        ushort num2 = this.ReadUInt16(endianStyle);
        ++num1;
        if (num2 != (ushort) 0)
          empty += ((char) num2).ToString();
        else
          break;
      }
      this.BaseStream.Seek((long) ((length - num1) * 2), SeekOrigin.Current);
      return empty;
    }

    public void Seek(long position) => this.BaseStream.Position = position;

    public uint SeekNReed(long Address)
    {
      this.BaseStream.Position = Address;
      return this.ReadUInt32();
    }
  }
}
