using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;

namespace aggregator
{
    public partial class Service1 : ServiceBase
    {
        public Service1()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            new aggregator.Listener.Listener();
            var md = Mono.Cecil.ModuleDefinition.ReadModule(@"C:\temp\mscorlib.dll");
            var ty = md.Assembly.MainModule.GetType("System.Console");
            foreach (var item in ty.Methods)
            {
                
            }
            var proc = ty.Methods[0].GetElementMethod().Resolve().Body.GetILProcessor();
            var il = proc.Create(Mono.Cecil.Cil.OpCodes.Ldstr,
                "test insert");
            proc.Body.Instructions[2] = il ;
            

        }

        protected override void OnStop()
        {
        }
    }
}
