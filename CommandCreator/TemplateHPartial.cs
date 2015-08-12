using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

namespace CommandCreator
{
    public partial class CommandGeneratorH
    {
        public CommandGeneratorH(NetworkCommand nc)
        {
            if (string.IsNullOrEmpty(nc.ClassName))
                throw new ArgumentException("Required field cannot be null or empty.", "ClassName");
            if (string.IsNullOrEmpty(nc.Name))
                throw new ArgumentException("Required field cannot be null or empty.", "Name");
            if (nc.Code < 0 | nc.Code > 0xFFFF)
                throw new ArgumentOutOfRangeException("Code", "Your code number cannot be less than 0 or greater than 65,535.");
            Namespace = nc.Namespace ?? "Agent";
            ClassName = nc.ClassName;
            Name = nc.Name;
            Code = nc.Code;
            Description = nc.Description ?? "No Description Given";
            CommandType = nc.CommandType ?? typeof(string);
            ClassProperties = nc.ClassProperties ?? new ObservableCollection<ClassProperty>(); // If they are blank we need to continue
            EnumerationList = nc.EnumerationList ?? new ObservableCollection<EnumerationDefinition>(); // If they are blank we need to continue
            TypeMapping = new Dictionary<string, string>();
            foreach (var item in nc.TypeMapping)
            {
                TypeMapping.Add(item.OriginalType, item.NewType);
            }

        }
        public string Namespace { get; set; }
        public string ClassName { get; set; }
        public ICollection<ClassProperty> ClassProperties { get; set; }
        public ICollection<EnumerationDefinition> EnumerationList { get; set; }
        public string Description { get; set; }
        public string Name { get; set; }
        public int Code { get; set; }
        public Type CommandType { get; set; }
        public Dictionary<string, string> TypeMapping { get; set; }

    }
}
