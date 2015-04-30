using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Commands.Common
{
    class GetString : Interfaces.ICommand<byte>

    {
        public string Name
        {
            get { throw new NotImplementedException(); }
        }

        public ushort Code
        {
            get { throw new NotImplementedException(); }
        }

        public string Description
        {
            get { throw new NotImplementedException(); }
        }

        public Type CommandType
        {
            get { throw new NotImplementedException(); }
        }


        public Interfaces.ICommand<byte> Decode(ArraySegment<byte> input)
        {
            throw new NotImplementedException();
        }

        public byte[] Encode()
        {
            throw new NotImplementedException();
        }
    }
}
