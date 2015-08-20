using System;
using System.Security.Cryptography;

namespace ChainsAPM.Helpers
{
    public sealed class Fnv1a32 : HashAlgorithm
    {
        private const uint FnvPrime = unchecked(16777619);

        private const uint FnvOffsetBasis = unchecked(2166136261);

        private uint hash;

        public Fnv1a32()
        {
            this.Reset();
        }

        public override void Initialize()
        {
            this.Reset();
        }

        protected override void HashCore(byte[] array, int ibStart, int cbSize)
        {
            for (var i = ibStart; i < cbSize; i++)
            {
                unchecked
                {
                    this.hash ^= array[i];
                    this.hash *= FnvPrime;
                }
            }
        }

        protected override byte[] HashFinal()
        {
            return BitConverter.GetBytes(this.hash);
        }

        private void Reset()
        {
            this.hash = FnvOffsetBasis;
        }
    }
    public sealed class Fnv1a64 : HashAlgorithm
    {
        private const ulong FnvPrime = unchecked(1099511628211);

        private const ulong FnvOffsetBasis = unchecked(14695981039346656037);

        private ulong hash;

        public Fnv1a64()
        {
            this.Reset();
        }

        public override void Initialize()
        {
            this.Reset();
        }

        protected override void HashCore(byte[] array, int ibStart, int cbSize)
        {
            for (var i = ibStart; i < cbSize; i++)
            {
                unchecked
                {
                    this.hash ^= array[i];
                    this.hash *= FnvPrime;
                }
            }
        }

        protected override byte[] HashFinal()
        {
            return BitConverter.GetBytes(this.hash);
        }

        private void Reset()
        {
            this.hash = FnvOffsetBasis;
        }
    }
}
