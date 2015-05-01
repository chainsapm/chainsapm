/*
	This stub file will eventually be used to store the body bytes of methods that I want to change in order to track them.
	
	* Following Threads
	In theory we should just be able to inject a private member variable to the thread class and change the constructor
	to store the thread that created it. This *SHOULD* work for threadpool threads also.
	
	* Following WebRequests
	If a request is made from one machine to another we want to be able to follow that across physical boundaries. One way we can do this is by adding
	a header to the request being made and then correlating that with another incoming request on a backend server.

	* Injecting the SPID to a connetion object
	In order to trace all the way down to a query plan we need to inject a SqlCommand to call "SELECT @@SPID" we also need to modify the 
	SqlConnection object to be able to store the SPID for reuse. Calling @@SPID is not too detrimental but it is bad practice to call it once per query.
*/
namespace StaticILBodies
{
	static char ThreadStartBody[100];
	static char SQLSPIDInjection[100];
	static char WebRequsetAddHeaderBody[100];
};