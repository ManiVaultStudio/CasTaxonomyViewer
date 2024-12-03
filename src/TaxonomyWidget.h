#pragma once

#include "CasTaxonomyData/Taxonomy.h"

#include <widgets/WebWidget.h>

class JSCommunicationObject : public mv::gui::WebCommunicationObject
{
    Q_OBJECT
public:
    JSCommunicationObject();

signals:
    // Signals from Qt to JS side
signals:
    void setData(QString data);
    void setFilterInJS(const QVariantList& data);
    void setHeaderOptions(const QVariantList& data);

public slots:
    // Invoked from JS side 
    void js_partitionHovered(const QString& data);

signals:
    // Signals from comm object to web widget
    void partitionHovered(QString name);

private:

};

class TaxonomyWidget : public mv::gui::WebWidget
{
    Q_OBJECT
public:
    TaxonomyWidget();
    ~TaxonomyWidget();

    JSCommunicationObject& getCommObject() { return _commObject; }

    void setData(const Taxonomy& taxonomy);

private slots:
    void onWebPageFullyLoaded();
    void onPartitionHovered(QString name);

protected:
    void resizeEvent(QResizeEvent* event);

public slots:
    void applyAspectRatio();

private:
    JSCommunicationObject   _commObject;    // Communication Object between Qt (cpp) and JavaScript
};
