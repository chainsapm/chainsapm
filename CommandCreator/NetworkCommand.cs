using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.Windows.Data;
using Microsoft.CSharp;
using System.ComponentModel.DataAnnotations;


namespace CommandCreator {

        public class NetCmdMultiValue : IMultiValueConverter {
                public object Convert (object [] values, Type targetType, object parameter, System.Globalization.CultureInfo culture) {
                        return values;
                }

                public object [] ConvertBack (object value, Type [] targetTypes, object parameter, System.Globalization.CultureInfo culture) {
                        throw new NotImplementedException ();
                }
        }

        [DataContract]
        public class NetworkCommandViewModel : INotifyPropertyChanged {

                public string SaveDirectory { get; set; }
                public TypeHelper TypeHelp { get; internal set; }

                public int lastCommandNumber;

                private bool m_HasChanges;
                public bool HasChanges {
                        get { return m_HasChanges; }
                        set {
                                m_HasChanges = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("SelectedCommand"));
                                }
                        }
                }

                private NetworkCommand m_NetCommand;
                public NetworkCommand SelectedCommand {
                        get { return m_NetCommand; }
                        set {
                                m_NetCommand = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("SelectedCommand"));
                                }
                        }
                }

                private string m_SelectedType;



                public string SelectedType {
                        get {
                                if ( string.IsNullOrEmpty (m_SelectedType) ) {
                                        return typeof (Int32).FullName;
                                }
                                return m_SelectedType;
                        }
                        set {
                                m_SelectedType = value;

                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("SelectedType"));
                                }
                        }
                }

                private bool m_IsList;
                public bool IsList {
                        get {
                                return m_IsList;
                        }
                        set {
                                m_IsList = value;

                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("IsList"));
                                }
                        }
                }

                private bool m_IsArray;
                public bool IsArray {
                        get {
                                return m_IsArray;
                        }
                        set {
                                m_IsArray = value;

                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("IsArray"));
                                }
                        }
                }

                private ObservableCollection<TypeMap> m_CSTypeMapping;
                [DataMember (Order = 1)]
                public ObservableCollection<TypeMap> CSharpTypeMapping {
                        get { return m_CSTypeMapping; }
                        set {
                                m_CSTypeMapping = value;
                                if ( m_CSTypeMapping != null ) {
                                        m_CSTypeMapping.CollectionChanged += CollectionChanged;
                                }
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("CSharpTypeMapping"));
                                }
                        }
                }

                void CollectionChanged (object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e) {
                        HasChanges = true;
                }

                private ObservableCollection<TypeMap> m_CppTypeMapping;
                [DataMember (Order = 2)]
                public ObservableCollection<TypeMap> CppTypeMapping {
                        get { return m_CppTypeMapping; }
                        set {
                                m_CppTypeMapping = value;
                                if ( m_CppTypeMapping != null ) {
                                        m_CppTypeMapping.CollectionChanged += CollectionChanged;
                                }
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("CppTypeMapping"));
                                }

                        }
                }

                private ObservableCollection<NetworkCommandTreeItemModel> m_Commands;
                [DataMember (Order = 3)]
                public ObservableCollection<NetworkCommandTreeItemModel> NetworkCommands {
                        get { return m_Commands; }
                        set {
                                m_Commands = value;
                                if ( m_Commands != null ) {
                                        m_Commands.CollectionChanged += CollectionChanged;
                                }
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("NetworkCommands"));
                                }
                        }
                }

                private string m_ProjectRoot;



                public string ProjectRoot {
                        get {
                                if ( string.IsNullOrEmpty (m_ProjectRoot) ) {
                                        return @"..\\..\\..";
                                }
                                return m_ProjectRoot;
                        }
                        set {
                                m_ProjectRoot = value;

                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("ProjectRoot"));
                                }
                        }
                }

                public NetworkCommandViewModel () {
                        TypeHelp = new TypeHelper ();
                        CppTypeMapping = new ObservableCollection<TypeMap> ();
                        CSharpTypeMapping = new ObservableCollection<TypeMap> ();
                        NetworkCommands = new ObservableCollection<NetworkCommandTreeItemModel> ();
                }
                public event PropertyChangedEventHandler PropertyChanged;


        }

        public class NetworkCommandTreeItemModel : INotifyPropertyChanged {

                private bool cmdNameRegister = false;
                // C:\Users\James\Source\Repos\chainsapm\clrprofiler\src\commands
                private NetworkCommand m_Command;
                public NetworkCommand NetworkCommand {
                        get { return m_Command; }
                        set {
                                m_Command = value;
                                if ( !cmdNameRegister ) {
                                        cmdNameRegister = true;
                                        m_Command.PropertyChanged += m_Command_PropertyChanged;
                                }
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("NetworkCommand"));

                                }
                        }
                }

                void m_Command_PropertyChanged (object sender, PropertyChangedEventArgs e) {
                        if ( e.PropertyName == "Name" | e.PropertyName == "Code" ) {
                                CommandName = ((NetworkCommand)sender).Name;
                        }

                }

                private string m_cmdName;
                public string CommandName {
                        get { return m_cmdName; }
                        set {
                                m_cmdName = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("CommandName"));
                                }
                        }
                }



                public event PropertyChangedEventHandler PropertyChanged;
        }

        [DataContract]
        public class NetworkCommand : INotifyPropertyChanged {


                private string m_Name;
                [DataMember]
                public string Name {
                        get { return m_Name; }
                        set {
                                m_Name = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("Name"));
                                        if ( string.IsNullOrEmpty (m_ClassName) ) {
                                                PropertyChanged (this, new PropertyChangedEventArgs ("ClassName"));
                                        }
                                }
                        }
                }
                private string m_Description;
                [DataMember]
                public string Description {
                        get { return m_Description; }
                        set {
                                m_Description = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("Description"));
                                }
                        }

                }

                private int m_Code;
                [DataMember]
                [Range (0, 0xffff)]
                public int Code {
                        get { return m_Code; }
                        set {
                                m_Code = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("Code"));
                                }
                        }
                }

                private string m_Namespace;
                [DataMember]
                public string Namespace {
                        get { return m_Namespace; }
                        set {
                                m_Namespace = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("Namespace"));
                                }
                        }
                }

                private string m_ClassName;
                [DataMember]
                public string ClassName {
                        get {
                                //
                                if ( string.IsNullOrEmpty (m_ClassName) ) {
                                        return m_Name.Replace (" ", "").Replace ("!", "").Replace (":", "").Replace (".", "");
                                }
                                return m_ClassName;
                        }
                        set {
                                m_ClassName = value.Replace (" ", "").Replace ("!", "").Replace (":", "").Replace (".", ""); ;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("ClassName"));
                                }
                        }
                }

                private Type m_CommandType;
                [DataMember]
                public Type CommandType {
                        get { return m_CommandType; }
                        set {
                                m_CommandType = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("CommandType"));
                                }
                        }
                }

                private ObservableCollection<ClassProperty> m_ClassProperties;
                [DataMember]
                public ObservableCollection<ClassProperty> ClassProperties {
                        get { return m_ClassProperties; }
                        set {
                                m_ClassProperties = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("ClassProperties"));
                                }
                        }
                }

                private ObservableCollection<EnumerationDefinition> m_EnumerationList;
                [DataMember]
                public ObservableCollection<EnumerationDefinition> EnumerationList {
                        get { return m_EnumerationList; }
                        set {
                                m_EnumerationList = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("EnumerationList"));
                                }
                        }
                }

                private EnumerationDefinition m_SelectedEnumeration;
                public EnumerationDefinition SelectedEnumeration {
                        get { return m_SelectedEnumeration; }
                        set {
                                m_SelectedEnumeration = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("SelectedEnumeration"));
                                }
                        }
                }
                public ICollection<TypeMap> TypeMapping { get; set; }

                public event PropertyChangedEventHandler PropertyChanged;
        }

        [DataContract]
        public class TypeMap {
                public TypeMap () { }
                public TypeMap (string orig, string newty) {
                        OriginalType = orig;
                        NewType = newty;
                }
                public string Name {
                        get {
                                return string.Format ("{0} is mapped to {1}", OriginalType, NewType);
                        }
                }
                [DataMember]
                public string OriginalType { get; set; }
                [DataMember]
                public string NewType { get; set; }
        }
        [DataContract]
        public class EnumerationItem {
                [DataMember]
                public string Name { get; set; }
                [DataMember]
                public int Value { get; set; }

        }

        [DataContract]
        public class EnumerationDefinition : INotifyPropertyChanged {
                [DataMember]
                public string EnumerationName { get; set; }
                [DataMember]
                public ObservableCollection<EnumerationItem> Items { get; set; }

                private int m_LastVal;
                public int LastValue {
                        get {
                                if ( Items.Count > 0 ) {
                                        foreach ( var item in Items ) {
                                                m_LastVal = Math.Max (item.Value, m_LastVal);
                                        }


                                        if ( Flags ) {
                                                if ( m_LastVal > 1 ) {
                                                        m_LastVal = (int)Math.Log ((double)m_LastVal, 2.0);
                                                }
                                        }


                                }
                                return m_LastVal;

                        }
                        set { m_LastVal = value; }
                }

                private bool m_Flags;
                public bool Flags {
                        get { return m_Flags; }
                        set {
                                m_Flags = value;
                                if ( PropertyChanged != null ) {
                                        PropertyChanged (this, new PropertyChangedEventArgs ("Flags"));
                                }
                        }
                }

                public void AddEnumerationItem (string ItemName) {

                        if ( Items == null ) {
                                Items = new ObservableCollection<EnumerationItem> ();
                        }

                        int value = LastValue;
                        if ( Flags && value >= 1 ) {
                                ++value;
                        }
                        if ( Flags ) {
                                value = 1 << value;
                        }

                        if ( !Flags ) {
                                ++value;
                        }

                        Items.Add (new EnumerationItem () { Name = ItemName, Value = value });
                }


                public override string ToString () {
                        return EnumerationName;
                }

                public event PropertyChangedEventHandler PropertyChanged;
        }
        [DataContract]
        public class ClassProperty {
                [DataMember]
                public string TypeName { get; set; }
                [DataMember]
                public string Name { get; set; }

                [DataMember]
                public bool? IsList { get; set; }
                [DataMember]
                public bool? IsArray { get; set; }


                public string CppTypeName (Dictionary<string, string> typemap) {

                        var transformedtype = TypeName;
                        if ( typemap.ContainsKey(TypeName)) {
                                transformedtype = typemap [TypeName];
                        }

                        if ( IsList == true ) {
                                return string.Format ("std::vector<{0}>", transformedtype);
                        }
                        if ( IsArray == true ) {
                                return string.Format ("{0}[]", transformedtype);
                        }
                        return string.Format ("{0}", transformedtype);
                }

                public string CsTypeName (Dictionary<string, string> typemap) {

                        var transformedtype = TypeName;
                        if ( typemap.ContainsKey (TypeName) ) {
                                transformedtype = typemap [TypeName];
                        }
                        if ( IsList == true ) {
                                return string.Format ("List<{0}>", transformedtype);
                        }
                        if ( IsArray == true ) {
                                return string.Format ("{0}[]", transformedtype);
                        }
                        return string.Format ("{0}", transformedtype);
                }
                public override string ToString () {
                        if ( IsList == true ) {
                                return string.Format ("List<{0}> {1}", TypeName, Name);
                        }
                        if ( IsArray == true ) {
                                return string.Format ("{0}[] {1}", TypeName, Name);
                        }
                        return string.Format ("{0} {1}", TypeName, Name);
                }
        }
}
