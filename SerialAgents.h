#pragma once
// SerialAgents.h

#include "Serial.h"

class CSerialAgents
{
public:
	CSerialAgents();
	~CSerialAgents();

	enum Status
	{
		ReadyToConnect,
		OutputConnected,
		CommandConnected,
		DownloadConnected,
	};

	Status GetStatus() { return m_status; }
protected:
	Status m_status;
	CSerial m_serial;
};