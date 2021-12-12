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

#include "ProxyServer.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

ProxyServer::ProxyServer(QObject *parent)
	: QObject(parent)
{
	m_server = new QTcpServer(this);

	connect(m_server, &QTcpServer::newConnection, this, &ProxyServer::onFtpClientConnected);
	if (!m_server->listen(QHostAddress::Any, PROXY_SERVER_PORT))
	{
		//
		qDebug() << "ProxyServer start failed";
	}
}

ProxyServer::~ProxyServer()
{
}

void ProxyServer::onFtpClientConnected()
{
	// [1] 处理下游客户端请求
	m_socket = m_server->nextPendingConnection();
	connect(m_socket, &QTcpSocket::readyRead, this, &ProxyServer::onFtpClientReadRequest);
	connect(m_socket, &QTcpSocket::disconnected, this, &ProxyServer::onFtpClientDisconnected);

	m_PIConnected = true;

	while (!m_PICommandBuffer.isEmpty())
	{
		onReadCommandfFromProxyClient(m_PICommandBuffer.dequeue());
	}

	// 通知上游创建客户端连接终端FTP服务
	emit ftpClientConnected();
	//[1]
}

void ProxyServer::onFtpClientDisconnected()
{
	emit ftpClientDisconnected();

	qDebug() << "browser disconnected";
	if (m_dataSocket && m_dataSocket->isValid())
	{
		m_dataSocket->close();
		m_dataSocket->deleteLater();
	}

	if (m_socket && m_socket->isValid())
	{
		m_socket->close();
		m_socket->deleteLater();
	}

	if (m_dataServer)
	{
		m_dataServer->close();
		m_dataServer->deleteLater();
		m_dataServer = nullptr;
	}

	m_PIConnected = false;
	m_DPIConnected = false;

	m_DPIBuffer.clear();
	m_PICommandBuffer.clear();
}

void ProxyServer::onFtpClientReadRequest()
{
	QByteArray receivedCommand = m_socket->readAll();
	emit ftpClientRequestReady(receivedCommand);
}

void ProxyServer::onCreateFtpDataServer(int &portNo)
{
	// [1] 处理下游客户的再PAssive模式下的数据链路建立
	m_dataServer = new QTcpServer(this);

	connect(m_dataServer, &QTcpServer::newConnection, this, &ProxyServer::onFtpDataClientConnected);
	connect(m_dataServer, &QTcpServer::acceptError, this, &ProxyServer::onFtpDataServerAcceptError);
	if (!m_dataServer->listen(QHostAddress::Any))
	{
		qDebug() << "DPI Server start failed";
	}
	//[1]

	portNo = m_dataServer->serverPort();
}

quint16 ProxyServer::getServerPort()
{
	if (m_dataServer)
	{
		return m_dataServer->serverPort();
	}

	return 0;
}

void ProxyServer::onFtpDataServerAcceptError(QAbstractSocket::SocketError socketError)
{
	if (m_dataServer != nullptr)
	{
		m_dataServer->close();
		delete m_dataServer;
		m_dataServer = nullptr;

		m_dataSocket = nullptr;
	}

	qDebug() << socketError;
}

void ProxyServer::onFtpDataClientConnected()
{
	m_dataSocket = m_dataServer->nextPendingConnection();
	connect(m_dataSocket, &QTcpSocket::readyRead, this, &ProxyServer::onReadFtpDataClientData);
	connect(m_dataSocket, &QTcpSocket::disconnected, this, &ProxyServer::onFtpDataClientDisconnected);
	
	m_DPIConnected = true;

	while (!m_DPIBuffer.isEmpty()) 
	{
		onReadDataFromProxyClient(m_DPIBuffer.dequeue());
	}
}

void ProxyServer::onFtpDataClientDisconnected()
{ 
	m_DPIConnected = false;
}

void ProxyServer::onReadFtpDataClientData()
{
	// 无数据上传业务，所以此接口不实现
	qDebug() << "browser data line ready to be read";
}

 

void ProxyServer::onReadDataFromProxyClient(QByteArray data)
{
	if (false == m_DPIConnected) 
	{
		m_DPIBuffer.enqueue(data);
		return;
	}

	// [1]文件数据返回下游客户端
	if (m_dataSocket && m_dataSocket->isValid())
	{
		m_dataSocket->write(data);
		m_dataSocket->flush();
	}
	// [1]
}

void ProxyServer::onReadCommandfFromProxyClient(QByteArray command)
{
	if (false == m_PIConnected) 
	{
		m_PICommandBuffer.enqueue(command);
		return;
	}

	if (command.contains("226")) // Transfer complete
	{
		if (m_dataSocket)
		{
			m_dataSocket->close();
		}

		if (m_dataServer)
		{
			m_dataServer->close();
		}
	}

	//[1] 将指令状态数据返回下游客户端
	if (m_socket && m_socket->isValid())
	{
		m_socket->write(command);
		m_socket->flush();
	}
	//[1]
}