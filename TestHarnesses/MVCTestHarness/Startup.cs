using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(MVCTestHarness.Startup))]
namespace MVCTestHarness
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);
        }
    }
}
