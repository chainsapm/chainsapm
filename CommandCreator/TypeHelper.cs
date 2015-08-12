using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CommandCreator
{
    public class TypeHelper
    {
        public string Int8
        {
            get
            {
                return typeof(byte).FullName;
            }
        }

        public string Int16
        {
            get
            {
                return typeof(Int16).FullName;
            }
        }
        public string Int32
        {
            get
            {
                return typeof(Int32).FullName;
            }
        }
        public string Int64
        {
            get
            {
                return typeof(Int64).FullName;
            }
        }
        public string String
        {
            get
            {
                return typeof(string).FullName;
            }
        }
        public string Float
        {
            get
            {
                return typeof(float).FullName;
            }
        }
        public string Double
        {
            get
            {
                return typeof(double).FullName;
            }
        }

    }
}
