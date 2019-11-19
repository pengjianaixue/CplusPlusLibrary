#pragma once

#include <QObject>
#include <pugixml.hpp>
#include <sstream>
#include <memory>
class XmlReader : public QObject
{
	Q_OBJECT
public:
	using VALUEPAIRLIST = QList<QList<std::pair<QString, QString>>>;
public:
	explicit XmlReader(QObject *parent = nullptr);
	~XmlReader();
	//API
	bool openFile(const QString	&FileName);
	bool setRootName(const QString &RootName = "root");
	bool getAnSpecialCategoryValue(const QString &categoryName, const QString  &childName, const QStringList &attriNameList, VALUEPAIRLIST &valueList);
private:
	pugi::xml_document					m_PugiXmlDocument;
	QString								m_strRootName;
};
