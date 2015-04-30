using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces
{
    public interface ICommand<Tdata>
    {
        /// <summary>
        /// Short name used to desribe the command
        /// </summary>
        string Name { get; }
        /// <summary>
        /// 16-bit unsigned integer used to identify the command
        /// </summary>
        ushort Code { get; }
        /// <summary>
        /// Long description of the implemented command
        /// </summary>
        string Description { get; }
        /// <summary>
        /// Returns the underlying type for this command.
        /// </summary>
        Type CommandType { get; }
        /// <summary>
        /// Encodes the instance of this command for transport using type of <see cref="{Tdata}"/>.
        /// </summary>
        Tdata[] Encode();

        /// <summary>
        /// Decodes type of <see cref="{Tdata}"/> and returns an instance of ICommand.
        /// </summary>
        ICommand<Tdata> Decode(ArraySegment<Tdata> input);

    }

}
