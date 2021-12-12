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
	* @brief  �����ն�FTP������������PI������ָ�ͨ��.
	* @param  
	*/
	void onConnectToFtpServer();

	/**
	* @brief  ����ͻ������ն�FTP���������ӶϿ�.
	* @param
	*/
	void onFtpServerDisconnected();
	

	/**
	* @brief  ���ն�FTP����������PI������ָ�ͨ���ɹ����ѻ����е�ָ��������͸�����ն˷�����.
	* @param
	*/
	void onFtpServerConnected();
	void onFtpServerDisconnect();


	/**
	* @brief  ������PIͨ�������������ݼ�״̬���ݣ�������������.
	���ǽ���Entering Passive Mode�򴴽�DPI���������ݣ��������
	��֪ͨ�ն˽���Entering Passive Mode����׼����������
	* @param
	*/
	void onReadServerCommand();

	/**
	* @brief  ����ͻ��˽���Entering Passive Mode�������ն�FTP������DPI���������ݣ�ͨ������
	* @param
	*/
	void onDataServerconnected();

	/**
	* @brief ���ն�FTP������DPIͨ�����������ݣ��Ͽ�
	* @param
	*/
	void onDataServerDisconnected();

	/**
	* @brief ��ȡ�ն�FTP������DPIͨ���е����ݣ��ļ���
	* @param
	*/
	void onReadFTPDataServerData();


	/**
	* @brief �Ӵ���������ж�ȡ����ftp�ͻ��˵�ָ��
	* @param
	*/
	void onReadFTPCommandInProxyServer(QByteArray command);

private:
	// ��ȡftpЭ���еĶ˿ںŽ���
	int getPassivePort(QByteArray *message);

	// ���˿ںź�ip��ַת��ΪFTPЭ���еĵ�ַ����
	QByteArray setDataProxyPort(QByteArray *message, quint16 port);


private:
	class QTcpSocket *m_socket = nullptr;  //PI socket
	class QTcpSocket *m_dataSocket = nullptr; // DPI socket

	bool m_PIConnected = false;
	QQueue<QByteArray> m_PICommandBuffer;
};
