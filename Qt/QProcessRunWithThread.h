#pragma once

#include <QObject>
#include <thread>
class subProcessRunner : public QObject
{
	Q_OBJECT

public:
	explicit subProcessRunner(QObject *parent = nullptr);
	~subProcessRunner();
	void terminateProcess();
	//void registerrunCommandList(const QList<QString> &cmdList);
	void run(const QList<QString>& cmdList);
public slots:
	
private slots:
	void emitProcessMsg();
signals:
	void s_processMsgReaded(const QString &);
	void s_runFinished(const QString &);
private:
	QProcess				*m_runCmdProcess = {nullptr};
};
