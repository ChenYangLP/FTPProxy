/*
	Copyright (C) 2021  xx

	This software is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "FTPProxy.h"
#include <QDebug>

FTPProxy::FTPProxy(QObject *parent)
	: QObject(parent)
{
	connect(&proxyServer, &ProxyServer::ftpClientConnected, \
		&proxyClient, &ProxyClient::onConnectToFtpServer);

	connect(&proxyServer, &ProxyServer::ftpClientDisconnected, \
		&proxyClient, &ProxyClient::onFtpServerDisconnect);

	connect(&proxyServer, &ProxyServer::ftpClientRequestReady, \
		&proxyClient, &ProxyClient::onReadFTPCommandInProxyServer);

	connect(&proxyClient, &ProxyClient::sendCommandToFtpClient, \
		&proxyServer, &ProxyServer::onReadCommandfFromProxyClient);
	connect(&proxyClient, &ProxyClient::createDataServer, \
		&proxyServer, &ProxyServer::onCreateFtpDataServer);

	connect(&proxyClient, &ProxyClient::sendServerDataToFtpDataClient, \
		&proxyServer, &ProxyServer::onReadDataFromProxyClient);

	qDebug() << "FTP proxy started";
}

FTPProxy::~FTPProxy()
{
}
