using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace aggregator.NetworkPackets.Information
{
    enum ParameterType
    {

    }
    class FunctionMapping : IBaseItem
    {
        Int64 FunctionID;
        Int64 MetadataToken;
        Int64 ClassID;
        string FunctionName;
    }
    struct ClassMapping
    {
        Int64 ClassID;
        Int64 MetadataToken;
        string ClassName;
    }

    struct FunctionParameter
    {
        Int64 MetadataToken;
        
    }
}
