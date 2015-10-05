   

namespace ChainsAPM.Interfaces {
        public interface ICommandProcessor {
                void Process (Commands.Agent.AgentInformation cmd);
                void Process (Commands.Agent.ACK cmd);
                void Process (Commands.Agent.MethodsToInstrument cmd);
                void Process (Commands.Agent.DefineMethod cmd);
                void Process (Commands.Agent.MethodEnter cmd);
                void Process (Commands.Agent.MethodExit cmd);
                void Process (Commands.Agent.DefineModule cmd);
                void Process (Commands.Agent.DefineInstrumentationMethods cmd);
                void Process (Commands.Agent.SendInjectionMetadata cmd);
        }
}
