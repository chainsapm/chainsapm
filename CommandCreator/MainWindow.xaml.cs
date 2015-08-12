using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.Serialization.Json;
using System.Collections.ObjectModel;

namespace CommandCreator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public NetworkCommandViewModel NetworkCommands { get; set; }
        public MainWindow()
        {
            InitializeComponent();
            CreateCSharpTypeMap();
            CreateCppTypeMap();
            

        }

        private void CreateCSharpTypeMap()
        {
            var csTypeMap = CommandVM.CSharpTypeMapping;

            csTypeMap.Add(new TypeMap("FunctionID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("ProcessID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("AssemblyID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("AppDomainID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("ModuleID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("ClassID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("ThreadID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("ContextID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("ObjectID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("GCHandleID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("ReJITID", typeof(Int64).ToString()));
            csTypeMap.Add(new TypeMap("mdToken", typeof(Int32).ToString()));               // Generic token
            // Token  definitions
            csTypeMap.Add(new TypeMap("mdModule", typeof(Int32).ToString()));               // Module token (roughly, a scope)
            csTypeMap.Add(new TypeMap("mdTypeRef", typeof(Int32).ToString()));              // TypeRef reference (this or other scope)
            csTypeMap.Add(new TypeMap("mdTypeDef", typeof(Int32).ToString()));              // TypeDef in this scope
            csTypeMap.Add(new TypeMap("mdFieldDef", typeof(Int32).ToString()));             // Field in this scope
            csTypeMap.Add(new TypeMap("mdMethodDef", typeof(Int32).ToString()));            // Method in this scope
            csTypeMap.Add(new TypeMap("mdParamDef", typeof(Int32).ToString()));             // param token
            csTypeMap.Add(new TypeMap("mdInterfaceImpl", typeof(Int32).ToString()));        // interface implementation token
            csTypeMap.Add(new TypeMap("mdMemberRef", typeof(Int32).ToString()));            // MemberRef (this or other scope)
            csTypeMap.Add(new TypeMap("mdCustomAttribute", typeof(Int32).ToString()));      // attribute token
            csTypeMap.Add(new TypeMap("mdPermission", typeof(Int32).ToString()));           // DeclSecurity
            csTypeMap.Add(new TypeMap("mdSignature", typeof(Int32).ToString()));            // Signature object
            csTypeMap.Add(new TypeMap("mdEvent", typeof(Int32).ToString()));                // event token
            csTypeMap.Add(new TypeMap("mdProperty", typeof(Int32).ToString()));             // property token
            csTypeMap.Add(new TypeMap("mdModuleRef", typeof(Int32).ToString()));            // Module reference (for the imported modules)

            // Assembly tokens.
            csTypeMap.Add(new TypeMap("mdAssembly", typeof(Int32).ToString()));             // Assembly token.
            csTypeMap.Add(new TypeMap("mdAssemblyRef", typeof(Int32).ToString()));          // AssemblyRef token.
            csTypeMap.Add(new TypeMap("mdFile", typeof(Int32).ToString()));                 // File token.
            csTypeMap.Add(new TypeMap("mdExportedType", typeof(Int32).ToString()));         // ExportedType token.
            csTypeMap.Add(new TypeMap("mdManifestResource", typeof(Int32).ToString()));     // ManifestResource token.

            csTypeMap.Add(new TypeMap("mdTypeSpec", typeof(Int32).ToString()));             // TypeSpec object

            csTypeMap.Add(new TypeMap("mdGenericParam", typeof(Int32).ToString()));         // formal parameter to generic type or method
            csTypeMap.Add(new TypeMap("mdMethodSpec", typeof(Int32).ToString()));           // instantiation of a generic method
            csTypeMap.Add(new TypeMap("mdGenericParamConstraint", typeof(Int32).ToString())); // constraint on a formal generic parameter

            // Application string.
            csTypeMap.Add(new TypeMap("mdString", typeof(Int32).ToString()));               // User literal string token.
            csTypeMap.Add(new TypeMap("mdCPToken", typeof(Int32).ToString()));              // constantpool token
        }

        private void CreateCppTypeMap()
        {
            var csTypeMap = CommandVM.CppTypeMapping;

            csTypeMap.Add(new TypeMap("FunctionID", "__int64"));
            csTypeMap.Add(new TypeMap("ProcessID", "__int64"));
            csTypeMap.Add(new TypeMap("AssemblyID", "__int64"));
            csTypeMap.Add(new TypeMap("AppDomainID", "__int64"));
            csTypeMap.Add(new TypeMap("ModuleID", "__int64"));
            csTypeMap.Add(new TypeMap("ClassID", "__int64"));
            csTypeMap.Add(new TypeMap("ThreadID", "__int64"));
            csTypeMap.Add(new TypeMap("ContextID", "__int64"));
            csTypeMap.Add(new TypeMap("ObjectID", "__int64"));
            csTypeMap.Add(new TypeMap("GCHandleID", "__int64"));
            csTypeMap.Add(new TypeMap("ReJITID", "__int64"));
            csTypeMap.Add(new TypeMap("mdToken", "__int32"));               // Generic token
            // Token  definitions
            csTypeMap.Add(new TypeMap("mdModule", "__int32"));               // Module token (roughly, a scope)
            csTypeMap.Add(new TypeMap("mdTypeRef", "__int32"));              // TypeRef reference (this or other scope)
            csTypeMap.Add(new TypeMap("mdTypeDef", "__int32"));              // TypeDef in this scope
            csTypeMap.Add(new TypeMap("mdFieldDef", "__int32"));             // Field in this scope
            csTypeMap.Add(new TypeMap("mdMethodDef", "__int32"));            // Method in this scope
            csTypeMap.Add(new TypeMap("mdParamDef", "__int32"));             // param token
            csTypeMap.Add(new TypeMap("mdInterfaceImpl", "__int32"));        // interface implementation token
            csTypeMap.Add(new TypeMap("mdMemberRef", "__int32"));            // MemberRef (this or other scope)
            csTypeMap.Add(new TypeMap("mdCustomAttribute", "__int32"));      // attribute token
            csTypeMap.Add(new TypeMap("mdPermission", "__int32"));           // DeclSecurity
            csTypeMap.Add(new TypeMap("mdSignature", "__int32"));            // Signature object
            csTypeMap.Add(new TypeMap("mdEvent", "__int32"));                // event token
            csTypeMap.Add(new TypeMap("mdProperty", "__int32"));             // property token
            csTypeMap.Add(new TypeMap("mdModuleRef", "__int32"));            // Module reference (for the imported modules)

            // Assembly tokens.
            csTypeMap.Add(new TypeMap("mdAssembly", "__int32"));             // Assembly token.
            csTypeMap.Add(new TypeMap("mdAssemblyRef", "__int32"));          // AssemblyRef token.
            csTypeMap.Add(new TypeMap("mdFile", "__int32"));                 // File token.
            csTypeMap.Add(new TypeMap("mdExportedType", "__int32"));         // ExportedType token.
            csTypeMap.Add(new TypeMap("mdManifestResource", "__int32"));     // ManifestResource token.

            csTypeMap.Add(new TypeMap("mdTypeSpec", "__int32"));             // TypeSpec object

            csTypeMap.Add(new TypeMap("mdGenericParam", "__int32"));         // formal parameter to generic type or method
            csTypeMap.Add(new TypeMap("mdMethodSpec", "__int32"));           // instantiation of a generic method
            csTypeMap.Add(new TypeMap("mdGenericParamConstraint", "__int32")); // constraint on a formal generic parameter

            // Application string.
            csTypeMap.Add(new TypeMap("mdString", "__int32"));               // User literal string token.
            csTypeMap.Add(new TypeMap("mdCPToken", "__int32"));              // constantpool token


            // .NET Types
            csTypeMap.Add(new TypeMap(typeof(byte).FullName, "char"));
            csTypeMap.Add(new TypeMap(typeof(Int16).FullName, "__int16"));
            csTypeMap.Add(new TypeMap(typeof(Int32).FullName, "__int32"));
            csTypeMap.Add(new TypeMap(typeof(Int64).FullName, "__int64"));
            csTypeMap.Add(new TypeMap(typeof(float).FullName, "float"));
            csTypeMap.Add(new TypeMap(typeof(double).FullName, "double"));
            csTypeMap.Add(new TypeMap(typeof(string).FullName, "std::wstring"));

            CreateDummyCommands();
        }


        private void CreateDummyCommands()
        {
            var nc = new NetworkCommand();
            nc.Code = 0xff;
            nc.Name = "TestCommand";
            nc.Namespace = "Agent";
            nc.Description = "Test command long decription name";
            nc.ClassName = "TestCommandClass";
            nc.ClassProperties = new ObservableCollection<ClassProperty>();
            nc.ClassProperties.Add(new ClassProperty() { Name = "typ1", TypeName = typeof(string).FullName });
            nc.ClassProperties.Add(new ClassProperty() { Name = "typ2", TypeName = typeof(string).FullName });
            nc.EnumerationList = new ObservableCollection<EnumerationDefinition>();
            nc.EnumerationList.Add(new EnumerationDefinition());
            nc.EnumerationList.Last().Flags = true;
            nc.EnumerationList.Last().EnumerationName = "TestEnum";
            nc.EnumerationList.Last().Items = new ObservableCollection<EnumerationItem>();
            nc.EnumerationList.Last().Items.Add(new EnumerationItem() { Name = "BIG_ENUM_NAME", Value = 1 });
            nc.EnumerationList.Last().Items.Add(new EnumerationItem() { Name = "BIG_ENUM_NAME1", Value = 2 });
            nc.EnumerationList.Last().Items.Add(new EnumerationItem() { Name = "BIG_ENUM_NAME2", Value = 4 });
            nc.EnumerationList.Last().Items.Add(new EnumerationItem() { Name = "BIG_ENUM_NAME3", Value = 8 });

            nc.EnumerationList.Add(new EnumerationDefinition());
            nc.EnumerationList.Last().EnumerationName = "TestEnum 2";
            nc.EnumerationList.Last().Items = new ObservableCollection<EnumerationItem>();
            nc.EnumerationList.Last().Items.Add(new EnumerationItem() { Name = "BIG_ENUM_NAME", Value = 1 });
            nc.EnumerationList.Last().Items.Add(new EnumerationItem() { Name = "BIG_ENUM_NAME3", Value = 8 });

            var netcmd = CommandVM.NetworkCommands;
            netcmd.Add(new NetworkCommandTreeItemModel() { CommandName = nc.Name, NetworkCommand = nc });
            netcmd.Add(new NetworkCommandTreeItemModel() { CommandName = nc.Name, NetworkCommand = nc });
            netcmd.Add(new NetworkCommandTreeItemModel() { CommandName = nc.Name, NetworkCommand = nc });
            netcmd.Add(new NetworkCommandTreeItemModel() { CommandName = nc.Name, NetworkCommand = nc });
            netcmd.Add(new NetworkCommandTreeItemModel() { CommandName = nc.Name, NetworkCommand = nc });
            netcmd.Add(new NetworkCommandTreeItemModel() { CommandName = nc.Name, NetworkCommand = nc });
            netcmd.Add(new NetworkCommandTreeItemModel() { CommandName = nc.Name, NetworkCommand = nc });
            netcmd.Add(new NetworkCommandTreeItemModel() { CommandName = nc.Name, NetworkCommand = nc });

            var json = Newtonsoft.Json.JsonConvert.SerializeObject(CommandVM, Newtonsoft.Json.Formatting.Indented);
        }

        private void CommandBinding_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.Handled = true;
            e.CanExecute = true;
        }

        private void CommandBinding_Executed_1(object sender, ExecutedRoutedEventArgs e)
        {
            var oDiag = new OpenFileDialog();
            oDiag.Filter = "JSON Settings File (*.json)|*.json";
            if (oDiag.ShowDialog() == true)
            {
                var tr = new System.IO.StreamReader(oDiag.OpenFile());
                var cmd = Newtonsoft.Json.JsonConvert.DeserializeObject<NetworkCommandViewModel>(tr.ReadToEnd());
                CommandVM.CppTypeMapping = cmd.CppTypeMapping;
                CommandVM.CSharpTypeMapping = cmd.CSharpTypeMapping;
                CommandVM.NetworkCommands = cmd.NetworkCommands;
            }

        }

        private void CommandBinding_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }


        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var lb = sender as ListBox;
            var nc = lb.SelectedItem as NetworkCommandTreeItemModel;
            CommandVM.SelectedCommand = nc.NetworkCommand;
            e.Handled = true;
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var lb = sender as ComboBox;
            var nc = lb.SelectedItem as EnumerationDefinition;
            CommandVM.SelectedCommand.SelectedEnumeration = nc;
            e.Handled = true;
        }

        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            var lb = sender as RadioButton;
            CommandVM.SelectedType = lb.Content as string;
            e.Handled = true;
        }


        private void AddField_Click(object sender, RoutedEventArgs e)
        {
            if (CommandVM.SelectedCommand != null)
            {
                CommandVM.SelectedCommand.ClassProperties.Add(new ClassProperty() { TypeName = txtFeildType.Text, Name = txtFeildName.Text });
            }
        }

        private void AddNewCommand_Click(object sender, RoutedEventArgs e)
        {
            var ncti = new NetworkCommandTreeItemModel() { CommandName = "New Command", NetworkCommand = new NetworkCommand() { Name = "New Command", Code = 0, Description = "Update Command Description", Namespace = "Agent" } };
            ncti.NetworkCommand.ClassProperties = new ObservableCollection<ClassProperty>();
            ncti.NetworkCommand.EnumerationList = new ObservableCollection<EnumerationDefinition>();
            CommandVM.NetworkCommands.Add(ncti);
            CommandVM.SelectedCommand = ncti.NetworkCommand;
        }

        private void MoveCommandUp_Click(object sender, RoutedEventArgs e)
        {
            var lb = listFields;
            var nc = lb.SelectedIndex;
            if ((nc - 1) >= 0)
            {
                CommandVM.SelectedCommand.ClassProperties.Move(nc, --nc);
            }

            e.Handled = true;
        }

        private void MoveCommandDown_Click(object sender, RoutedEventArgs e)
        {


            var lb = listFields;
            var nc = lb.SelectedIndex;
            if ((nc + 1) <= CommandVM.SelectedCommand.ClassProperties.Count)
            {
                CommandVM.SelectedCommand.ClassProperties.Move(nc, ++nc);
            }

            e.Handled = true;
        }

        private void AddNewEnumeration_Click(object sender, RoutedEventArgs e)
        {
            if (CommandVM.SelectedCommand != null)
            {
                var enumList = new EnumerationDefinition() { EnumerationName = txtEnumName.Text, Items = new ObservableCollection<EnumerationItem>() };
                CommandVM.SelectedCommand.EnumerationList.Add(enumList);
            }
        }

        private void AddEnumerationItem_Click(object sender, RoutedEventArgs e)
        {

            if (CommandVM.SelectedCommand != null)
            {
                CommandVM.SelectedCommand.SelectedEnumeration.AddEnumerationItem(txtEnumItem.Text);
            }


        }


        private void ProcessSingleCommand_click(object sender, RoutedEventArgs e)
        {
            var json = Newtonsoft.Json.JsonConvert.SerializeObject(CommandVM, Newtonsoft.Json.Formatting.Indented);
            CommandVM.SelectedCommand.TypeMapping = CommandVM.CSharpTypeMapping;
            var tt = new CommandGeneratorCS(CommandVM.SelectedCommand);
            using (var sw = new System.IO.StreamWriter(string.Format("C:\\Logfiles\\{0}.cs", tt.ClassName)))
            {
                sw.Write(tt.TransformText());
            } 
            CommandVM.SelectedCommand.TypeMapping = CommandVM.CppTypeMapping;
            var ttCpp = new CommandGeneratorCpp(CommandVM.SelectedCommand);
            var ttH = new CommandGeneratorH(CommandVM.SelectedCommand);
            
            using (var sw = new System.IO.StreamWriter(string.Format("C:\\Logfiles\\{0}.cpp", ttCpp.ClassName)))
            {
                sw.Write(ttCpp.TransformText());
            }
            using (var sw = new System.IO.StreamWriter(string.Format("C:\\Logfiles\\{0}.h", ttH.ClassName)))
            {
                sw.Write(ttH.TransformText());
            }
            
            
        }

        private void CommandBinding_CanExecute_1(object sender, CanExecuteRoutedEventArgs e)
        {

        }

        private void CanSaveExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.Handled = true;
            e.CanExecute = true;
        }

        private void SaveFileCommand(object sender, ExecutedRoutedEventArgs e)
        {
            var oDiag = new SaveFileDialog();
            oDiag.Filter = "JSON Settings File (*.json)|*.json";
            if (oDiag.ShowDialog() == true)
            {
                using (var sw = new System.IO.StreamWriter(oDiag.FileName))
                {
                    var json = Newtonsoft.Json.JsonConvert.SerializeObject(CommandVM, Newtonsoft.Json.Formatting.Indented);
                    sw.Write(json);
                    sw.Flush();
                }
            }
            CommandVM.HasChanges = false;
        }

        private void ProcessAllCommands_Click(object sender, RoutedEventArgs e)
        {
            int[] commandNumbers = new int[CommandVM.NetworkCommands.Count];
            for (int i = 0; i < CommandVM.NetworkCommands.Count; i++)
            {
                commandNumbers[i] = CommandVM.NetworkCommands[i].NetworkCommand.Code;
            }
            if (commandNumbers.Distinct().Count() != CommandVM.NetworkCommands.Count)
            {
                MessageBox.Show("You have a duplicate number in your commands. Please verify.");
            }
            else
            {
                foreach (var item in CommandVM.NetworkCommands)
                {
                    item.NetworkCommand.TypeMapping = CommandVM.CSharpTypeMapping;
                    var tt = new CommandGeneratorCS(item.NetworkCommand);
                    using (var sw = new System.IO.StreamWriter(string.Format("C:\\Logfiles\\{0}.cs", tt.ClassName)))
                    {
                        sw.Write(tt.TransformText());
                    }
                }
            }
            
        }

        private void NewFileCommand(object sender, ExecutedRoutedEventArgs e)
        {
            if (CommandVM.HasChanges)
            {
                switch (MessageBox.Show("You have unsaved changes to this document. Do you want to save them now?", "Confirm New Document", MessageBoxButton.YesNoCancel))
                {
                    case MessageBoxResult.Cancel:
                        return;
                    case MessageBoxResult.No:
                        break;
                    case MessageBoxResult.None:
                        break;
                    case MessageBoxResult.OK:
                        break;
                    case MessageBoxResult.Yes:
                        SaveFileCommand(sender, e);
                        break;
                    default:
                        break;
                }
            }
            CommandVM.NetworkCommands.Clear();
            CommandVM.HasChanges = false;
            CommandVM.SelectedCommand = null;
        }

        private void SaveToCommand(object sender, ExecutedRoutedEventArgs e)
        {
            var oDiag = new System.Windows.Forms.FolderBrowserDialog();
            switch (oDiag.ShowDialog())
            {
                case System.Windows.Forms.DialogResult.Abort:
                    break;
                case System.Windows.Forms.DialogResult.Cancel:
                    break;
                case System.Windows.Forms.DialogResult.Ignore:
                    break;
                case System.Windows.Forms.DialogResult.No:
                    break;
                case System.Windows.Forms.DialogResult.None:
                    break;
                case System.Windows.Forms.DialogResult.OK:
                    CommandVM.SaveDirectory = oDiag.SelectedPath;
                    break;
                case System.Windows.Forms.DialogResult.Retry:
                    break;
                case System.Windows.Forms.DialogResult.Yes:
                    CommandVM.SaveDirectory = oDiag.SelectedPath;
                    break;
                default:
                    break;
            }

            CommandVM.HasChanges = false;
        }
    }

    public static class CustomCommands
    {
        public static readonly RoutedUICommand Exit = new RoutedUICommand
                (
                        "Exit",
                        "Exit",
                        typeof(CustomCommands),
                        new InputGestureCollection()
                                {
                                        new KeyGesture(Key.F4, ModifierKeys.Alt)
                                }
                );

        public static readonly RoutedUICommand SaveTo = new RoutedUICommand
                (
                        "Save To...",
                        "SaveTo",
                        typeof(CustomCommands),
                        new InputGestureCollection()
                                {
                                        new KeyGesture(Key.S, ModifierKeys.Control | ModifierKeys.Shift)
                                }
                );

        //Define more commands here, just like the one above
    }


}
