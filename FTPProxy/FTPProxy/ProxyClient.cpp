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

#include "ProxyClient.h"
#include <QTcpSocket>
#include <QDebug>
#include <QHostAddress>
#include <QTimer>

ProxyClient::ProxyClient(QObject *parent)
	: QObject(parent)
{
}

ProxyClient::~ProxyClient()
{
}

void ProxyClient::onConnectToFtpServer()
{
	//[1] 连接终端FTP服务器
	m_socket = new QTcpSocket(this);
	connect(m_socket, &QTcpSocket::readyRead, this, &ProxyClient::onReadServerCommand);
	connect(m_socket, &QTcpSocket::connected, this, &ProxyClient::onFtpServerConnected);
	connect(m_socket, &QTcpSocket::disconnected, this, &ProxyClient::onFtpServerDisconnected);

	m_socket->connectToHost(FTP_SERVER_ADDRESS, FTP_SERVER_PORT);
	//[1]
}

void ProxyClient::onFtpServerConnected()
{
	m_PIConnected = true;

	while (!m_PICommandBuffer.isEmpty()) 
	{
		onReadFTPCommandInProxyServer(m_PICommandBuffer.dequeue());
	}
}

void ProxyClient::onFtpServerDisconnect()
{
	if (nullptr != m_socket)
	{
		m_socket->close();
		m_socket->deleteLater();
	}

	if (nullptr != m_dataSocket)
	{
		m_dataSocket->close();
		m_dataSocket->deleteLater();
	}

	m_PIConnected = false;
	m_PICommandBuffer.clear();
}

void ProxyClient::onFtpServerDisconnected()
{
	m_PIConnected = false;
}

void ProxyClient::onReadServerCommand()
{
	// 从上游PI接收命令数据及状态

	// 处理终端FTP服务返回的数据以及状态
	QByteArray receivedData = m_socket->readAll();

	if (receivedData.startsWith("227"))
	{
		qDebug() << "---------Upstream Enter Passive Mode--------";

		QByteArray changedData(receivedData);


		//[2] 下游进入Passive Mode，需要根据服务器返回的端口号重新与终端FTP服务建立新的连接
		int port = 0;
		emit createDataServer(port);
		// 将新建的下游数据服务监控端口下发到下游客户端
		changedData = setDataProxyPort(&changedData, port);
		emit sendCommandToFtpClient(changedData);
		//[2]


		// [1]上游进入Passive Mode，需要根据服务器返回的端口号重新与终端FTP服务建立新的连接
		m_dataSocket = new QTcpSocket(this);
		connect(m_dataSocket, &QTcpSocket::readyRead, this, &ProxyClient::onReadFTPDataServerData);
		connect(m_dataSocket, &QTcpSocket::connected, this, &ProxyClient::onDataServerconnected);
		connect(m_dataSocket, &QTcpSocket::disconnected, this, &ProxyClient::onDataServerDisconnected);

		int portNo = getPassivePort(&receivedData); // 获取DPI port
		m_dataSocket->connectToHost(FTP_SERVER_ADDRESS, portNo);
		// [1]
	}
	else if (receivedData.startsWith("226")) 
	{
		if (m_dataSocket)
		{
			m_dataSocket->waitForReadyRead();
		}

		emit sendCommandToFtpClient(receivedData);
	}
	else
	{
		emit sendCommandToFtpClient(receivedData);
	}
}


void ProxyClient::onReadFTPCommandInProxyServer(QByteArray command)
{
	if (false == m_PIConnected) 
	{
		m_PICommandBuffer.enqueue(command);
		return;
	}

	if (command.contains("QUIT"))
	{
		emit sendCommandToFtpClient("221 Bye\r\n"); 
	}

	m_socket->write(command);
	m_socket->flush();
}


void ProxyClient::onDataServerconnected()
{
}

void ProxyClient::onDataServerDisconnected()
{
}


void ProxyClient::onReadFTPDataServerData()
{
	// 从上有DPI接收数据
	QByteArray reveivedData = m_dataSocket->readAll();
	emit sendServerDataToFtpDataClient(reveivedData);
}

int ProxyClient::getPassivePort(QByteArray *message)
{
	QString portStr;
	QString portOffset;
	int commaNo = 0;

	for (auto it = message->indexOf('('); it < message->indexOf(')'); it++)
	{
		if (message->at(it) == ',')
		{
			++commaNo;
		}

		if (commaNo == 4 && message->at(it) != ',')
		{
			portStr.append(message->at(it));
		}

		if (commaNo == 5 && message->at(it) != ',')
		{
			portOffset.append(message->at(it));
		}
	}

	return portStr.toInt() << 8 + portOffset.toInt();
}


QByteArray ProxyClient::setDataProxyPort(QByteArray *message, quint16 port)
{
	QByteArray firstPart = *message;

	int firstPartEnd = 0;
	int commaNo = 0;

	for (auto it = message->indexOf('('); it < message->indexOf(')'); it++)
	{
		if (message->at(it) == ',' && ++commaNo == 4)
		{
			firstPartEnd = it;
		}
	}

	int index = message->indexOf('(');

	firstPart.truncate(index + 1);
	int portNo = port >> 8;
	int portOffset = port - portNo << 8;

	QByteArray byteArray;
	byteArray.setNum(portNo);
	firstPart.append(QString(PROXY_SERVER_IP).replace(".", ","));
	firstPart.append(',');
	firstPart.append(byteArray);
	firstPart.append(',');

	byteArray.setNum(portOffset);
	firstPart.append(byteArray);

	firstPart.append(").\r\n");
	return firstPart;
}










