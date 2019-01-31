using System;

namespace ChainsAPM.Helpers
{
    public class ArraySegmentStream
    {
        private int internalCounter = 0;
        private ArraySegment<byte> segmentRef;
        public ArraySegmentStream(ArraySegment<byte> streamInput)
        {
            internalCounter = streamInput.Offset;
            segmentRef = streamInput;
        }

        public byte GetByte() { return segmentRef.Array[internalCounter++]; }
        public short GetInt16() { int countercopy = internalCounter; internalCounter += 2; return BitConverter.ToInt16(segmentRef.Array, countercopy); }
        public int GetInt32() { int countercopy = internalCounter; internalCounter += 4; return BitConverter.ToInt32(segmentRef.Array, countercopy); }
        public long GetInt64() { int countercopy = internalCounter; internalCounter += 8; return BitConverter.ToInt64(segmentRef.Array, countercopy); }
        public string GetUnicode(int length) { int countercopy = internalCounter; internalCounter += length * 2; return System.Text.UnicodeEncoding.Unicode.GetString(segmentRef.Array, countercopy, (length - 1) * 2); }
        public string GetASCII(int length) { int countercopy = internalCounter; internalCounter += length; return System.Text.UnicodeEncoding.Unicode.GetString(segmentRef.Array, countercopy, length); }
        public char GetChar() { return (char)segmentRef.Array[internalCounter++]; }
    }
}
