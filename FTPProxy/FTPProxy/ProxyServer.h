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
	* @brief ��ȡ������ͨ�ŵ�DPIͨ������Ķ˿ں�
	*  ���ڽ��˿ںŷ��͸����οͻ��˽���DPI�ͻ��˽�������
	* @param
	*/
	quint16 getServerPort();

signals:
	void ftpClientConnected();

	void ftpClientDisconnected();

	void ftpClientRequestReady(QByteArray ba);

public slots:

	/**
	* @brief �ͻ���PI�������������������е�ָ���·����ͻ��ˣ�
	�������н��������PI��ָ�ͨ��
	* @param
	*/
	void onFtpClientConnected();

	/**
	* @brief �ͻ������ӶϿ�
	* @param
	*/
	void onFtpClientDisconnected();

	/**
	* @brief ��ȡ�ͻ���PI��ָ���ͨ������PIͨ��͸�����ն˷���
	* @param
	*/
	void onFtpClientReadRequest();

	/**
	* @brief ������ͻ��˽������ݴ����DPIͨ��
	* @param
	*/
	void onCreateFtpDataServer(int &portNo);

	/**
	* @brief ��ͻ��˽������ݴ����DPIͨ����������ɽ�����������·�
	* @param
	*/
	void onFtpDataClientConnected();
	void onFtpDataServerAcceptError(QAbstractSocket::SocketError socketError);

	/**
	* @brief ��ͻ��˽������ݴ����DPIͨ���Ͽ�
	* @param
	*/
	void onFtpDataClientDisconnected();

	/**
	* @brief �ͻ���ͨ��DPI�ϴ����ݣ��������ϴ�ҵ�����Դ˽ӿڲ�ʵ�֣�
	* @param
	*/
	void onReadFtpDataClientData();

	/**
	* @brief ��ȡ�������ն˷���ͨ�ŵ�PI��ָ�����͸��������PIͨ��
	* @param
	*/
	void onReadCommandfFromProxyClient(QByteArray command);


	/**
	* @brief ��ȡ�������ն˷���ͨ�ŵ�DPI�����ݣ�����͸��������DPIͨ��
	* ��DPI��δ�������򻺴�����
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
