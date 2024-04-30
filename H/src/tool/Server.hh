//
// This is not part of the engine because it depends on ProjectLoader.
//

#ifndef SERVER_HH
#define SERVER_HH

namespace Tool
{
	class Server
	{
	public:
		static void Init();
		static void DoIO(long currentTime);
	};
}

#endif // SERVER_HH
