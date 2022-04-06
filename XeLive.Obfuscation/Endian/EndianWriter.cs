using System;
using System.IO;
using System.Text;

namespace XeLive.Obfuscation
{
  internal class EndianWriter : BinaryWriter
  {
    private readonly EndianStyle _endianStyle;

    public EndianWriter(Stream Stream, EndianStyle EndianStyle)
      : base(Stream)
    {
      this._endianStyle = EndianStyle;
    }

    public void Seek(int position) => this.Seek(position, SeekOrigin.Begin);

    public void SeekNWrite(int position, double value)
    {
      this.Seek(position);
      this.Write(value, this._endianStyle);
    }

    public void SeekNWrite(int position, byte value)
    {
      this.Seek(position);
      this.Write((short) value, this._endianStyle);
    }

    public void SeekNWrite(int position, short value)
    {
      this.Seek(position);
      this.Write(value, this._endianStyle);
    }

    public void SeekNWrite(int position, byte[] value)
    {
      this.Seek(position);
      this.Write(value);
    }

    public void SeekNWrite(int position, int value)
    {
      this.Seek(position);
      this.Write(value, this._endianStyle);
    }

    public void SeekNWrite(int position, long value)
    {
      this.Seek(position);
      this.Write(value, this._endianStyle);
    }

    public void SeekNWrite(int position, float value)
    {
      this.Seek(position);
      this.Write(value, this._endianStyle);
    }

    public void SeekNWrite(int position, ushort value)
    {
      this.Seek(position);
      this.Write(value, this._endianStyle);
    }

    public void SeekNWrite(int position, uint value)
    {
      this.Seek(position);
      this.Write(value, this._endianStyle);
    }

    public void SeekNWrite(int position, ulong value)
    {
      this.Seek(position);
      this.Write(value, this._endianStyle);
    }

    public override void Write(double value) => this.Write(value, this._endianStyle);

    public override void Write(short value) => this.Write(value, this._endianStyle);

    public override void Write(int value) => this.Write(value, this._endianStyle);

    public override void Write(long value) => this.Write(value, this._endianStyle);

    public override void Write(float value) => this.Write(value, this._endianStyle);

    public override void Write(ushort value) => this.Write(value, this._endianStyle);

    public override void Write(uint value) => this.Write(value, this._endianStyle);

    public override void Write(ulong value) => this.Write(value, this._endianStyle);

    public void Write(double value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void Write(short value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void Write(int value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void Write(long value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void Write(float value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void Write(ushort value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void Write(uint value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void Write(ulong value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void WriteInt24(int value) => this.WriteInt24(value, this._endianStyle);

    public void WriteInt24(int value, EndianStyle EndianStyle)
    {
      byte[] bytes = BitConverter.GetBytes(value);
      Array.Resize<byte>(ref bytes, 3);
      if (EndianStyle == EndianStyle.BigEndian)
        Array.Reverse((Array) bytes);
      this.Write(bytes);
    }

    public void WriteString(string value) => this.Write(value.ToCharArray());

    public void WriteUnicodeString(string Value)
    {
      this.Write(Encoding.BigEndianUnicode.GetBytes(Value));
      this.Write(new byte[2]);
    }

    public void WriteUnicodeString(string String, int length) => this.WriteUnicodeString(String, length, this._endianStyle);

    public void WriteUnicodeString(string String, int length, EndianStyle endianStyle)
    {
      int length1 = String.Length;
      for (int index = 0; index < length1 && index <= length; ++index)
        this.Write((ushort) String[index], endianStyle);
      int length2 = (length - length1) * 2;
      if (length2 <= 0)
        return;
      this.Write(new byte[length2]);
    }
  }
}
