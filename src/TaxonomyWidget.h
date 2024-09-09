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

private:

};

class TaxonomyWidget : public mv::gui::WebWidget
{
    Q_OBJECT
public:
    TaxonomyWidget();
    ~TaxonomyWidget();

    void setData(const Taxonomy& taxonomy);

private slots:
    void onWebPageFullyLoaded();

private:
    JSCommunicationObject   _commObject;    // Communication Object between Qt (cpp) and JavaScript
};
