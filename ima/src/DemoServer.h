/*
 * demo_server.h - multi-threaded slim VNC-server for demo-purposes (optimized
 *                 for lot of clients accessing server in read-only-mode)
 *           
 * Copyright (c) 2006-2009 Tobias Doerffel <tobydox/at/users/dot/sf/dot/net>
 *  
 * This file is part of iTALC - http://italc.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */


#ifndef _DEMO_SERVER_H
#define _DEMO_SERVER_H

#include <QtCore/QMutex>
#include <QtCore/QPair>
#include <QtCore/QReadWriteLock>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>

#include "ItalcVncConnection.h"


// there's one instance of a demo-server on the iTALC-master
class DemoServer : public QTcpServer
{
	Q_OBJECT
public:
	DemoServer();
	virtual ~DemoServer();

	inline QPoint cursorPos( void )
	{
		m_cursorLock.lockForRead();
		const QPoint p = m_cursorPos;
		m_cursorLock.unlock();

		return p;
	}


private slots:
	// checks whether cursor was moved and sets according flags and
	// variables used by moveCursor() - connection has to be done in
	// GUI-thread as we're calling QCursor::pos() which at least under X11
	// must not be called from another thread than the GUI-thread
	void checkForCursorMovement( void );


private:
	virtual void incomingConnection( int _sd );

	QReadWriteLock m_cursorLock;
	QPoint m_cursorPos;

} ;



// the demo-server creates an instance of this class for each client, i.e.
// each client is connected to a different server-thread for a maximum
// performance
class DemoServerClient : public QThread
{
	Q_OBJECT
public:
	DemoServerClient( int _sd, DemoServer * _parent );
	virtual ~DemoServerClient();


private slots:
	// connected to regionUpdated(...)-signal of global ItalcVncConnection
	// this way we can record changes in screen, later we extract single
	// non-overlapping rectangles out of changed region for updating
	// as less as possible of screen
	void updateRegion( int x, int y, int w, int h );

	// called whenever ivsConnection::cursorShapeChanged() is emitted
	void updateCursorShape( void );

	// called regularly for sending pointer-movement-events detected by
	// checkForCursorMovement() to clients - connection has to be done
	// in demoServerClient-thread-context as we're writing to socket
	void moveCursor( void );

	// connected to readyRead()-signal of our client-socket and called as
	// soon as the clients sends something (e.g. an update-request)
	void processClient( void );


private:
	// thread-entry-point - does some initializations and then enters
	// event-loop of thread
	virtual void run( void );

	DemoServer * m_ds;
	QMutex m_dataMutex;
	QRegion m_changedRegion;
	QPoint m_lastCursorPos;
	volatile bool m_cursorShapeChanged;

	int m_socketDescriptor;
	QTcpSocket * m_sock;
	bool m_otherEndianess;
	Q_UINT8 * m_lzoWorkMem;

} ;


#endif