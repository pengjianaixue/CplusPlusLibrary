#include "stdafx.h"
#include "QProcessRunWithThread.h"

subProcessRunner::subProcessRunner(QObject *parent)
	: QObject(parent)
{
	m_runCmdProcess = new QProcess(this);
	m_runCmdProcess->setParent(this);
	m_runCmdProcess->setProcessChannelMode(QProcess::ProcessChannelMode::MergedChannels);
	connect(this->m_runCmdProcess, &QProcess::readyReadStandardOutput, this, &subProcessRunner::emitProcessMsg);
}

subProcessRunner::~subProcessRunner()
{
	m_runCmdProcess->deleteLater();
}
void subProcessRunner::emitProcessMsg()
{
	if (this->m_runCmdProcess)
	{
		QByteArray baStandardoutpt = this->m_runCmdProcess->readAllStandardOutput();
		QString msg = QString::fromLocal8Bit(baStandardoutpt);
		emit s_processMsgReaded(msg);
		return;
	}
	
}
void subProcessRunner::terminateProcess()
{
	if (m_runCmdProcess)
	{
		m_runCmdProcess->terminate();
	}
	return;
}
void subProcessRunner::run(const QList<QString>& cmdList)
{
	for (auto &cmditem : cmdList)
	{
		m_runCmdProcess->start(cmditem);
		m_runCmdProcess->waitForFinished();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));//wait the script stdout display to textbrowser
		emit s_runFinished(cmditem);
	}
	return;
}
