#pragma once

#include "MiConf.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Net/HTTPClientSession.h"
// #include "Poco/Net/HTTPClientRequest.h"
// #include "Poco/Net/HTTPClientResponse.h"

//#include "Poco/Net/StreamCopier.h"
#include <Poco/File.h>
#include "Poco/Net/MultipartReader.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/MediaType.h"
#include <Poco/Net/NameValueCollection.h>
#include <Poco/Net/HTMLForm.h>
#include "Poco/Net/PartHandler.h"
#include <Poco/StringTokenizer.h>

#include <Poco/Net/MessageHeader.h>
//#include <Poco/Util/ServerApplication.h>
#include <Poco/Base64Encoder.h>
#include <Poco/StreamCopier.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>

#include "Poco/StreamCopier.h"
#include "Poco/Exception.h"
#include "Poco/Base64Encoder.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>


struct RoutingRule {
	int id;                 // Unique identifier for the routing rule
	std::string title;     // Title of the routing rule
	int listen_port;       // Port number to listen on
	std::string target;    // Target address for routing
	int target_port;       // Target port number
	bool is_https;         // Flag indicating if HTTPS is used
	bool is_direct;
	bool is_active;        // Flag indicating if the rule is active
};
struct Violation {
	int id;
	std::string os;
	std::string script;
	std::string expect;
	std::string result;
};
struct ViolationRule {
	int id;
	int action1;
	int action2;
	int check_at_start;
	int check_interval;
	unsigned int last_time;
	std::string message;
	int show_warning;
	std::vector<Violation> scripts;
};
struct AgentScriptRule {
	AgentScriptRule() {
		status = 0;
	}
	int id;
	int agent_script_id;
	std::string agent_script_title;
	std::string agent_script;
	std::string os;
	//int os;
	int execution_type;
	std::string scheduled_time;
	bool run_at_once;
	unsigned int last_time;
	std::string result;
	int status;
	BOOL notified;
};

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;
using namespace Poco::JSON;
using namespace std;

// Define a request handler to handle incoming HTTP requests
class MyRequestHandler : public HTTPRequestHandler {
public:
	void OnProcessProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL);
	void OnViolationProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL);
	void OnAgentScriptProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL);
	void OnVersionProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL);
	void OnUpdateProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL);
	void OnHostIPProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL);
	void OnIdleProc(HTTPServerRequest& request, HTTPServerResponse& response, Poco::Dynamic::Var procName, Poco::Dynamic::Var baseURL);
	void OnUnknown(HTTPServerRequest& request, HTTPServerResponse& response);
	static void DeleteDirectory(const wchar_t* p_wszPath);
	void GetDP(const wchar_t* p_wszPath = NULL, wchar_t* wszPath = NULL);
	void SetKenanPolicy();
	void LaunchNodeClient(const wchar_t* p_wszIP, std::string gip, std::string& session);
	void LaunchUpdateApp(const wchar_t* exePath, const wchar_t* dirPath);
	std::vector<RoutingRule> m_routingRules;


public:
	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) override {
		try {
			//OutputDebugStringA(request.getURI().c_str());
			if (request.getURI().compare(GD_API_MY_PROC) == 0) {
				OnProcessProc(request, response, "GetPortRules", GD_API_MY_PROC);
				return;
			}
			if (request.getURI().compare(GD_API_VIOLATION) == 0) {
				OnViolationProc(request, response, "GetViolationRules", GD_API_VIOLATION);
				return;
			}
			if (request.getURI().compare(GD_API_AGENT_SCRIPT) == 0) {
				OnAgentScriptProc(request, response, "GetAgentScriptRules", GD_API_AGENT_SCRIPT);
				return;
			}
			if (request.getURI().compare(GD_API_GET_VERSION) == 0) {
				OnVersionProc(request, response, "GetVersion", GD_API_GET_VERSION);
				return;
			}
			if (request.getURI().compare(GD_API_LANUNCHER_IP) == 0) {
				OnHostIPProc(request, response, "GetIPInfo", GD_API_LANUNCHER_IP);
				return;
			}
			if (request.getURI().compare(GD_API_GET_IDLE) == 0) {
				OnIdleProc(request, response, "GetIdle", GD_API_LANUNCHER_IP);
				return;
			}
			if (request.getURI().compare(GD_API_UPLOAD_UPDATE) == 0) {
				OnUpdateProc(request, response, "ReceiveNewUpdate", GD_API_UPLOAD_UPDATE);
				return;
			}
			OnUnknown(request, response);
		}
		catch (Poco::Exception& ex) {
			response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
			response.send() << "Error: " << ex.displayText();
		}

	}
};

// Define a request handler factory to create instances of MyRequestHandler
class MyRequestHandlerFactory : public HTTPRequestHandlerFactory {
public:
	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest&) override {
		return new MyRequestHandler;
	}
};
// Define the main application class
class ClaHTTPServerWrapper : public ServerApplication {
public:
	void launch();
protected:
	int main(const vector<string>&) override {
		// Set up server parameters
		HTTPServerParams* params = new HTTPServerParams;
		params->setMaxQueued(100);
		params->setMaxThreads(16);

		// Create a new HTTPServer instance
		HTTPServer server(new MyRequestHandlerFactory, ServerSocket(GD_PORT_IN), params);

		// Start the server
		server.start();
		cout << "Server started on port " << GD_PORT_IN << "." << endl;

		// Wait for CTRL-C or termination signal
		waitForTerminationRequest();

		// Stop the server
		server.stop();
		cout << "Server stopped." << endl;

		return Application::EXIT_OK;
	}
};

class MyPartHandler : public Poco::Net::PartHandler
{
public:
	void handlePart(const Poco::Net::MessageHeader& header, std::istream& stream) override
	{
		if (header.has("Content-Disposition"))
		{
			std::string disposition = header.get("Content-Disposition");
			if (disposition.find("form-data") != std::string::npos)
			{
				std::string name = extractParameter(disposition, "name");
				std::string filename = extractParameter(disposition, "filename");

				if (!filename.empty())
				{
					// Handle file part
//					std::ostringstream binaryStream;
					Poco::StreamCopier::copyStream(stream, _binaryStream);

					_fileData = _binaryStream.str();
					_filename = filename;

					// Encode the binary data to base64
					std::ostringstream base64Stream;
					Poco::Base64Encoder base64Encoder(base64Stream);
					base64Encoder << _fileData;
					base64Encoder.close();
					_base64Data = base64Stream.str();
				}
			}
		}
	}

	const std::string& fileData() const { return _fileData; }
	const std::string& base64Data() const { return _base64Data; }
	const std::string& filename() const { return _filename; }
	//const std::ostringstream& binaryData() const { return _binaryStream; }

private:
	std::string _fileData;
	std::string _base64Data;
	std::string _filename;
	std::ostringstream	_binaryStream;

	std::string extractParameter(const std::string& headerValue, const std::string& paramName)
	{
		std::string param = paramName + "=\"";
		size_t startPos = headerValue.find(param);
		if (startPos != std::string::npos)
		{
			startPos += param.length();
			size_t endPos = headerValue.find("\"", startPos);
			if (endPos != std::string::npos)
			{
				return headerValue.substr(startPos, endPos - startPos);
			}
		}
		return "";
	}

};
