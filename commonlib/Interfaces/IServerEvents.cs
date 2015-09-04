﻿using ChainsAPM.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces
{
    public interface IServerEvents
    {
        event ConnectionEvent ServerListening;
        event ConnectionEvent ServerShutdown;
        
    }
}