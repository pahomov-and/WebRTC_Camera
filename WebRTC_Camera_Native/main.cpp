#include <signal.h>

#include <iostream>
#include <fstream>

#include "rtc_base/ssl_adapter.h"
#include "rtc_base/thread.h"
#include "p2p/base/stun_server.h"
#include "p2p/base/basic_packet_socket_factory.h"
#include "p2p/base/turn_server.h"

#include "PeerConnectionManager.h"
#include "HttpServerRequestHandler.h"

//#include "opencv2/opencv.hpp"

PeerConnectionManager* webRtcServer = NULL;

void sighandler(int n)
{
    printf("SIGINT\n");
    // delete need thread still running
    delete webRtcServer;
    webRtcServer = NULL;
    rtc::Thread::Current()->Quit();
}

int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;

//    cv::Mat testMat;

    std::vector<std::string> options;
    const char *turnurl = "";
    const char *defaultlocalstunurl = "0.0.0.0:3478";
    const char *localstunurl = NULL;
    const char *defaultlocalturnurl = "turn:turn@0.0.0.0:3478";
    const char *localturnurl = NULL;
    const char *stunurl = "stun.l.google.com:19302";
    std::string defaultWebrtcUdpPortRange = "0:65535";
    std::string localWebrtcUdpPortRange = "";
    int logLevel = rtc::LERROR;
    const char *webroot = "./html";

    webrtc::AudioDeviceModule::AudioLayer audioLayer = webrtc::AudioDeviceModule::kPlatformDefaultAudio;
    std::string sslCertificate;
    std::string streamName;
    std::string nbthreads;
    std::string passwdFile;
    std::string authDomain = "mydomain.com";
    std::string publishFilter(".*");

    Json::Value config;

    std::ifstream stream("./config.json");
    stream >> config;

    std::string httpAddress("0.0.0.0:");
    std::string httpPort = "8000";
    const char *port = getenv("PORT");
    if (port) {
        httpPort = port;
    }
    httpAddress.append(httpPort);

    rtc::LogMessage::LogToDebug((rtc::LoggingSeverity) logLevel);
    rtc::LogMessage::LogTimestamps();
    rtc::LogMessage::LogThreads();
    std::cout << "Logger level:" << rtc::LogMessage::GetLogToDebug() << std::endl;

    rtc::Thread *thread = rtc::Thread::Current();
    rtc::InitializeSSL();

    // webrtc server
    std::list<std::string> iceServerList;
    if ((strlen(stunurl) != 0) && (strcmp(stunurl, "-") != 0)) {
        iceServerList.push_back(std::string("stun:") + stunurl);
    }
    if (strlen(turnurl)) {
        iceServerList.push_back(std::string("turn:") + turnurl);
    }


    std::list<std::string> urlList;
    while (optind<argc)
    {
        urlList.push_back(argv[optind]);
        optind++;
    }

    webRtcServer = new PeerConnectionManager(iceServerList, config["urls"], audioLayer, publishFilter, localWebrtcUdpPortRange);
    if (!webRtcServer->InitializePeerConnection()) {
        std::cout << "Cannot Initialize WebRTC server" << std::endl;
    } else {
        // http server
//        std::vector<std::string> options;
        options.push_back("document_root");
        options.push_back(webroot);
        options.push_back("enable_directory_listing");
        options.push_back("no");
        options.push_back("additional_header");
        options.push_back("X-Frame-Options: SAMEORIGIN");
        options.push_back("access_control_allow_origin");
        options.push_back("*");
        options.push_back("listening_ports");
        options.push_back(httpAddress);
        options.push_back("enable_keep_alive");
        options.push_back("yes");
        options.push_back("keep_alive_timeout_ms");
        options.push_back("1000");
        if (!sslCertificate.empty()) {
            options.push_back("ssl_certificate");
            options.push_back(sslCertificate);
        }
        if (!nbthreads.empty()) {
            options.push_back("num_threads");
            options.push_back(nbthreads);
        }
        if (!passwdFile.empty()) {
            options.push_back("global_auth_file");
            options.push_back(passwdFile);
            options.push_back("authentication_domain");
            options.push_back(authDomain);
        }

    }
    try {
        std::map<std::string, HttpServerRequestHandler::httpFunction> func = webRtcServer->getHttpApi();
        std::cout << "HTTP Listen at " << httpAddress << std::endl;
        HttpServerRequestHandler httpServer(func, options);

        // start STUN server if needed
        std::unique_ptr<cricket::StunServer> stunserver;
        if (localstunurl != NULL) {
            rtc::SocketAddress server_addr;
            server_addr.FromString(localstunurl);
            rtc::AsyncUDPSocket *server_socket = rtc::AsyncUDPSocket::Create(thread->socketserver(), server_addr);
            if (server_socket) {
                stunserver.reset(new cricket::StunServer(server_socket));
                std::cout << "STUN Listening at " << server_addr.ToString() << std::endl;
            }
        }

        // start TRUN server if needed
        std::unique_ptr<cricket::TurnServer> turnserver;
        if (localturnurl != NULL) {
            std::istringstream is(localturnurl);
            std::string addr;
            std::getline(is, addr, '@');
            std::getline(is, addr, '@');
            rtc::SocketAddress server_addr;
            server_addr.FromString(addr);
            turnserver.reset(new cricket::TurnServer(rtc::Thread::Current()));

            rtc::AsyncUDPSocket *server_socket = rtc::AsyncUDPSocket::Create(thread->socketserver(), server_addr);
            if (server_socket) {
                std::cout << "TURN Listening UDP at " << server_addr.ToString() << std::endl;
                turnserver->AddInternalSocket(server_socket, cricket::PROTO_UDP);
            }
            rtc::AsyncSocket *tcp_server_socket = thread->socketserver()->CreateAsyncSocket(AF_INET, SOCK_STREAM);
            if (tcp_server_socket) {
                std::cout << "TURN Listening TCP at " << server_addr.ToString() << std::endl;
                tcp_server_socket->Bind(server_addr);
                tcp_server_socket->Listen(5);
                turnserver->AddInternalServerSocket(tcp_server_socket, cricket::PROTO_TCP);
            }

            is.str(turnurl);
            is.clear();
            std::getline(is, addr, '@');
            std::getline(is, addr, '@');
            rtc::SocketAddress external_server_addr;
            external_server_addr.FromString(addr);
            std::cout << "TURN external addr:" << external_server_addr.ToString() << std::endl;
            turnserver->SetExternalSocketFactory(new rtc::BasicPacketSocketFactory(),
                                                 rtc::SocketAddress(external_server_addr.ipaddr(), 0));
        }

        // mainloop
        signal(SIGINT, sighandler);
        thread->Run();

    } catch (const CivetException &ex) {
        std::cout << "Cannot Initialize start HTTP server exception:" << ex.what() << std::endl;
    }


    rtc::CleanupSSL();
    std::cout << "Exit" << std::endl;
    return 0;
}