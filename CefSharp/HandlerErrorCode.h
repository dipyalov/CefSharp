#include "stdafx.h"
#pragma once

namespace CefSharp
{
	public ref class HandlerErrorCode sealed
	{
	private:
		HandlerErrorCode() {}
	public:
		literal int Unknown = 0;
		literal int Failed = -2;
		literal int Aborted = -3;
		literal int InvalidArgument = -4;
		literal int InvalidHandle = -5;
		literal int FileNotFound = -6;
		literal int TimedOut = -7;
		literal int FileTooBig = -8;
		literal int Unexpected = -9;
		literal int AccessDenied = -10;
		literal int NotImplemented = -11;
		literal int ConnectionClosed = -100;
		literal int ConnectionReset = -101;
		literal int ConnectionRefused = -102;
		literal int ConnectionAborted = -103;
		literal int ConnectionFailed = -104;
		literal int NameNotResolved = -105;
		literal int InternetDisconnected = -106;
		literal int SslProtocolError = -107;
		literal int AddressInvalid = -108;
		literal int AddressUnreachable = -109;
		literal int SslClientAuthCertNeeded = -110;
		literal int TunnelConnectionFailed = -111;
		literal int NoSslVersionsEnabled = -112;
		literal int SslVersionOrCipherMismatch = -113;
		literal int SslRenegotiationRequested = -114;
		literal int CertCommonNameInvalid = -200;
		literal int CertDateInvalid = -201;
		literal int CertAuthorityInvalid = -202;
		literal int CertContainsErrors = -203;
		literal int CertNoRevocationMechanism = -204;
		literal int CertUnableToCheckRevocation = -205;
		literal int CertRevoked = -206;
		literal int CertInvalid = -207;
		literal int CertEnd = -208;
		literal int InvalidUrl = -300;
		literal int DisallowedUrlScheme = -301;
		literal int UnknownUrlScheme = -302;
		literal int TooManyRedirects = -310;
		literal int UnsafeRedirect = -311;
		literal int UnsafePort = -312;
		literal int InvalidResponse = -320;
		literal int InvalidChunkedEncoding = -321;
		literal int MethodNotSupported = -322;
		literal int UnexpectedProxyAuth = -323;
		literal int EmptyResponse = -324;
		literal int ResponseHeadersTooBig = -325;
		literal int CacheMiss = -400;
		literal int InsecureResponse = -501;
	};
}