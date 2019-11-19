#pragma once

#include <QObject>
#include <memory>
#include <pugixml.hpp>
class XmlWirter : public QObject
{
	Q_OBJECT
	
public:
	explicit XmlWirter(QObject *parent = nullptr);
	~XmlWirter();
	//API
	bool emptyXmlDoc();
	void fileStructInit(const QString &rootname);
	bool setCurrentNode(const QString &nodeName);
	bool writeAncategoryData(const QString &categoryName, const QString &childName, const QStringList &attriNameList, const QList<QStringList> &valueList);
	bool saveToFile(const QString &FilePath);
	bool loadXmlFile(const QString &FilePath);
	bool removeChild(const QString &childName);
private:
	pugi::xml_document					m_PugiXmlDocument;
	pugi::xml_node						m_RootNode ;
	QString								m_strRootName;
};
