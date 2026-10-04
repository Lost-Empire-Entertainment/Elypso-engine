//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "core_utils.hpp"

namespace KalaServer::Core
{
	using std::string;
	using std::string_view;
	using std::filesystem::path;
	using std::vector;
	using std::unordered_map;
	using std::chrono::steady_clock;

	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;

	//Minimum port, 1-1023 requires admin/root
	constexpr u16 MIN_PORT_RANGE = 1u;
	//Maximum port, cannot go past 16-bit unsigned integer TCP and UDP port fields
	constexpr u16 MAX_PORT_RANGE = 65535u;

	//How long is a user by IP timed out for in minutes
	//when violating max payload size or min packet spacing
	constexpr u8 TIME_OUT_PERIOD_M = 10u;

	//How long in seconds do we store all connections and check if the connected IP
	//has connected faster than MIN_PACKET_SPACING within this time period
	constexpr u8 ROLLING_WINDOW_TIMER_S = 5u;
	//Whats the shortest allowed time in milliseconds that a client
	//is allowed to have between each connection attempt
	constexpr u8 MIN_PACKET_SPACING_MS = 200u;

	//Wait for this amount of seconds before deeming the connection as inactive.
	constexpr u16 ACCEPT_WAIT_TIME_S = 60u;

	//Client must not exceed this max payload capacity in bytes at accept loop
	constexpr u16 MAX_TOTAL_PAYLOAD_SIZE_BYTES = 8192u;

	//Unreachable socket value for unassigned socket
	constexpr u32 UNASSIGNED_SOCKET_VALUE = 1000000u;

	//All further connect sockets are closed if this amount of total connections is reached
	constexpr u16 MAX_ACTIVE_CONNECTIONS = 1000u;

	//How many connections are allowed in one frame
	constexpr u8 MAX_CONNECTIONS_PER_FRAME = 10u;

	//Wait this many seconds to wait before allowing real health socket to check for server health,
	//otherwise return cached value 
	constexpr u8 SERVER_HEALTH_WAIT_S = 10;

	struct LIB_API DomainRoute
	{
		string domain{};  //the domain this route is added to
		string route{};   //how the route is displayed in the url
		path routePath{}; //path relative to executable path or full path

		bool operator==(const DomainRoute&) const = default;
	};

	struct LIB_API BannedIP
	{
		string targetIP{};

		//Leave unassigned to mark as permanent ban
		steady_clock::time_point expiresAt{};
	};

	//The data received from an accepted socket ready to be parsed
	struct LIB_API RequestData
	{
		string method{};
		string httpVersion{};
		DomainRoute domainRoute{};
		unordered_map<string, string> headers{};
		string body{};
	};

	//Any inbound or outbound socket and its data regardless of origin or destination,
	//connectionSocket will not be filled if it belongs to an
	//outgoing packet created by the server via SendPacketLocal
	struct LIB_API Connection
	{
		bool isRunning{};
		string connectionIP{};
		uintptr_t connectionSocket = UNASSIGNED_SOCKET_VALUE;

		//unparsed data
		string partialBuffer{};
		//parsed data
		RequestData requestData{};
	};

	class LIB_API KalaServerCore
	{
	friend class Cloudflare;
	public:
		//Converts Windows WSA or Linux error to its string equivalent
		static string ErrorToString(int error);

		//Force-close the program right this very moment with no cleanups
		static void ForceClose(
			string_view target,
			string_view reason);

		//Initialize a new server on this process.
		//Server name helps distinguish this server from other servers.
		//Server root is the true origin where the server will expose
		//routes from relative to where the process was run.
		//Server IP is the IP address users will connect to.
		//Server port is the local TCP port this server binds to and listens on.
		static void Initialize(
			string_view serverName,
			const path& serverRoot,
			string_view serverIP,
			u16 serverPort);

		//Returns true if this server instance has been initialized successfully
		static bool IsInitialized();

		//Creates a new listener socket if there is none created
		static void CreateListenerSocket();

		//Process incoming requests,
		//should be ran once per frame
		static void Update();

		//Returns true if this process can reach http://1.1.1.1 on port 53
		static bool HasInternet();

		static string_view GetServerName();
		static const path& GetServerRoot();
		static string_view GetServerIP();
		static u16 GetServerPort();

		static const Connection& GetListenerSocket();

		const vector<Connection>& GetConnectSockets();

		static void DisconnectConnectedUser(uintptr_t targetSocket);
		static void DisconnectConnectedUser(string_view targetIP);

		//Does this IP match any valid ipv4 or ipv6 structure
		static bool IsValidIP(string_view targetIP);

		//Lists banned IPs
		static const vector<BannedIP>& GetBannedIPs();
		//Ban IP, doesn't matter if it is currently connected or not
		static bool BanIP(string_view targetIP);
		//Unban existing IP, doesn't matter if its currently connected or not
		static bool UnbanIP(string_view targetIP);

		//Saves all existing banned ips to disk, overwrites existing txt file
		static bool SaveBannedIPsToDisk(const path& targetPath);
		//Loads all saved banned ips and appends to current list, duplicates are skipped
		static bool LoadBannedIPsFromDisk(const path& targetPath);

		//Lists existing domains
		static const vector<string>& GetDomains();
		//Lists existing routes
		static const vector<DomainRoute>& GetRoutes();
		//Add new route, cannot add add duplicates if domain+route matches,
		//cannot add routes if their path matches any existing route path of the same domain
		static bool AddRoute(const DomainRoute& newRoute);
		//Remove existing route
		static bool RemoveRoute(const DomainRoute& existingRoute);

		//Saves all existing routes to disk, overwrites existing txt file
		static bool SaveRoutesToDisk(const path& targetPath);
		//Loads all saved routes and appends to current list, duplicates are skipped
		static bool LoadRoutesFromDisk(const path& targetPath);

		//Add new blacklisted keyword, cannot add duplicates
		static bool AddBlacklistedKeyword(string_view newKeyword);
		//Remove existing blacklisted keyword
		static bool RemoveBlacklistedKeyword(string_view existingKeyword);

		//Saves all existing blacklisted keywords to disk, overwrites existing txt file
		static bool SaveBlacklistedKeywordsToDisk(const path& targetPath);
		//Loads all saved blacklisted keywords and appends to current list, duplicates are skipped
		static bool LoadBlacklistedKeywordsFromDisk(const path& targetPath);

		//Lists blacklisted keywords
		static const vector<string>& GetBlacklistedKeywords();

		//Close all sockets and clear all server resources
		static void Shutdown();
	private:
		static void SetServerReadyState(bool state);
	};
}