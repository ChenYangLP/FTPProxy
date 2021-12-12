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
#include <QQueue>
#include "AppDef.h"



class ProxyClient : public QObject
{
	Q_OBJECT

public:
	ProxyClient(QObject *parent = nullptr);
	~ProxyClient();

signals:
	void sendCommandToFtpClient(QByteArray ba);

	void sendServerDataToFtpDataClient(QByteArray ba);

	void createDataServer(int &portNo);

public slots:

	/**
	* @brief  连接终端FTP服务器，建立PI（传输指令）通道.
	* @param  
	*/
	void onConnectToFtpServer();

	/**
	* @brief  代理客户端与终端FTP服务器连接断开.
	* @param
	*/
	void onFtpServerDisconnected();
	

	/**
	* @brief  与终端FTP服务器连接PI（传输指令）通道成功，把缓存中的指令与数据透传到终端服务器.
	* @param
	*/
	void onFtpServerConnected();
	void onFtpServerDisconnect();


	/**
	* @brief  从上游PI通道接收命令数据及状态数据，解析命令数据.
	若是进入Entering Passive Mode则创建DPI（传输数据）代理服务，
	并通知终端进入Entering Passive Mode让其准备接收数据
	* @param
	*/
	void onReadServerCommand();

	/**
	* @brief  代理客户端进入Entering Passive Mode，并与终端FTP服务器DPI（传输数据）通道建立
	* @param
	*/
	void onDataServerconnected();

	/**
	* @brief 与终端FTP服务器DPI通道（传输数据）断开
	* @param
	*/
	void onDataServerDisconnected();

	/**
	* @brief 读取终端FTP服务器DPI通道中的数据（文件）
	* @param
	*/
	void onReadFTPDataServerData();


	/**
	* @brief 从代理服务器中读取下游ftp客户端的指令
	* @param
	*/
	void onReadFTPCommandInProxyServer(QByteArray command);

private:
	// 读取ftp协议中的端口号解析
	int getPassivePort(QByteArray *message);

	// 将端口号和ip地址转换为FTP协议中的地址描述
	QByteArray setDataProxyPort(QByteArray *message, quint16 port);


private:
	class QTcpSocket *m_socket = nullptr;  //PI socket
	class QTcpSocket *m_dataSocket = nullptr; // DPI socket

	bool m_PIConnected = false;
	QQueue<QByteArray> m_PICommandBuffer;
};
