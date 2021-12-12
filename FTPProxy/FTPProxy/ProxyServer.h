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

#pragma once

#include <QObject>
#include <QTcpServer>
#include <QQueue>
#include "AppDef.h"


class ProxyServer : public QObject
{
	Q_OBJECT

public:
	ProxyServer(QObject *parent = nullptr);
	~ProxyServer();

	/**
	* @brief 读取与下游通信的DPI通道服务的端口号
	*  用于将端口号发送个下游客户端建立DPI客户端接收数据
	* @param
	*/
	quint16 getServerPort();

signals:
	void ftpClientConnected();

	void ftpClientDisconnected();

	void ftpClientRequestReady(QByteArray ba);

public slots:

	/**
	* @brief 客户端PI连接请求建立，将缓存中的指令下发到客户端，
	并与上有建立代理的PI（指令）通道
	* @param
	*/
	void onFtpClientConnected();

	/**
	* @brief 客户端连接断开
	* @param
	*/
	void onFtpClientDisconnected();

	/**
	* @brief 读取客户端PI的指令，并通过代理PI通道透传到终端服务
	* @param
	*/
	void onFtpClientReadRequest();

	/**
	* @brief 创建与客户端进行数据传输的DPI通道
	* @param
	*/
	void onCreateFtpDataServer(int &portNo);

	/**
	* @brief 与客户端进行数据传输的DPI通道，创建完成将缓存的数据下发
	* @param
	*/
	void onFtpDataClientConnected();
	void onFtpDataServerAcceptError(QAbstractSocket::SocketError socketError);

	/**
	* @brief 与客户端进行数据传输的DPI通道断开
	* @param
	*/
	void onFtpDataClientDisconnected();

	/**
	* @brief 客户端通过DPI上传数据（无数据上传业务，所以此接口不实现）
	* @param
	*/
	void onReadFtpDataClientData();

	/**
	* @brief 读取与上游终端服务通信的PI（指令），并透传到下游PI通道
	* @param
	*/
	void onReadCommandfFromProxyClient(QByteArray command);


	/**
	* @brief 读取与上游终端服务通信的DPI（数据），并透传到下游DPI通道
	* 若DPI还未建立，则缓存数据
	* @param
	*/
	void onReadDataFromProxyClient(QByteArray data);


private:

	class QTcpServer *m_dataServer = nullptr;  //DPI server
	class QTcpSocket *m_dataSocket = nullptr; //DPI client
	bool m_DPIConnected = false;
	QQueue<QByteArray> m_DPIBuffer;


	class QTcpServer *m_server = nullptr; // PI server
	class QTcpSocket *m_socket = nullptr; // PI client
	bool m_PIConnected = false;
	QQueue<QByteArray> m_PICommandBuffer;
};
