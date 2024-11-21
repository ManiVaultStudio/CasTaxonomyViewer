#include "TaxonomyWidget.h"

#include <QLayout>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QHash>

#include <iostream>

// =============================================================================
// JSCommunicationObject
// =============================================================================

JSCommunicationObject::JSCommunicationObject()
{

}

// =============================================================================
// TaxonomyWidget
// =============================================================================

TaxonomyWidget::TaxonomyWidget() :
    _commObject()
{
    connect(this, &WebWidget::webPageFullyLoaded, this, &TaxonomyWidget::onWebPageFullyLoaded);

    // For more info on drag&drop behavior, see the ExampleViewPlugin project
    setAcceptDrops(true);

    // Ensure linking to the resources defined in res/cas_taxonomy_viewer_resources.qrc
    Q_INIT_RESOURCE(cas_taxonomy_viewer_resources);

    // ManiVault and Qt create a "QtBridge" object on the js side which represents _comObject
    // there, we can connect the signals qt_js_* and call the slots js_qt_* from our communication object
    init(&_commObject);

    layout()->setContentsMargins(0, 0, 0, 0);
}

TaxonomyWidget::~TaxonomyWidget()
{

}

void TaxonomyWidget::setData(const Taxonomy& taxonomy)
{
    std::vector<int> ranks(taxonomy.annotations.size());
    int maxRank = 0;
    QHash<QString, int> idToAnnotationIdx;
    std::vector<QJsonObject> nodes;

    QJsonArray plot_array;

    // Construct JSON tree nodes
    for (int i = 0; i < taxonomy.annotations.size(); i++)
    {
        const Annotation& annotation = taxonomy.annotations[i];

        QJsonObject item_data;

        item_data.insert("children", QJsonArray());
        item_data.insert("name", annotation.cell_label);

        if (annotation.cell_set_accession.isEmpty())
        {
            qWarning() << "Cell annotation: " << annotation.cell_label << " does not contain cell_set_accession field, can't include it in the hierarchy";
            continue;
        }

        idToAnnotationIdx[annotation.cell_set_accession] = i;
        nodes.push_back(item_data);
    }

    // Compute hierarchy ranks of annotations
    for (int i = 0; i < taxonomy.annotations.size(); i++)
    {
        const Annotation& annotation = taxonomy.annotations[i];

        int rank = 0;
        QString parent_id = annotation.parent_cell_set_accession;
        while (!parent_id.isEmpty())
        {
            if (idToAnnotationIdx.find(parent_id) != idToAnnotationIdx.end())
            {
                parent_id = taxonomy.annotations[idToAnnotationIdx[parent_id]].parent_cell_set_accession;
            }
            else
                break;
            rank++;
        }

        if (rank > maxRank)
            maxRank = rank;

        ranks[i] = rank;
        qDebug() << "Found rank: " << rank;
    }
    qDebug() << "Max rank" << maxRank;
    // Construct hierarchy
    int currentRank = maxRank;
    while (currentRank > 0)
    {
        for (int i = 0; i < taxonomy.annotations.size(); i++)
        {
            if (ranks[i] != currentRank)
                continue;

            const Annotation& annotation = taxonomy.annotations[i];

            QJsonObject& node = nodes[i];
            QJsonObject& parent = nodes[idToAnnotationIdx[annotation.parent_cell_set_accession]];

            if (currentRank == maxRank)
            {
                node.remove("children");
                node.insert("value", QJsonValue(100));
            }

            QJsonArray arr = parent["children"].toArray();
            arr.push_back(node);
            parent["children"] = arr;
        }
        currentRank--;
    }

    // Adding root nodes to final object
    for (int i = 0; i < ranks.size(); i++)
    {
        if (ranks[i] == 0)
        {
            qDebug() << "Found root node" << taxonomy.annotations[i].cell_label;
            plot_array.push_back(nodes[i]);
        }
    }

    QJsonObject final_object;

    final_object.insert("children", plot_array);
    final_object.insert("name", "taxonomy");

    QJsonDocument doc(final_object);
    QString strJson(doc.toJson(QJsonDocument::Indented));
    std::cout << strJson.toStdString() << std::endl;
    _commObject.setData(strJson);
}

void JSCommunicationObject::js_partitionHovered(const QString& data) {
    if (!data.isEmpty())
    {
        qDebug() << "PARTITION SIGNAL" << data;
        emit partitionHovered(data);
    }
}

void TaxonomyWidget::onWebPageFullyLoaded()
{
    qDebug() << "FilterView::onWebPageFullyLoaded: Web page completely loaded.";
    //emit webPageLoaded();
}

void TaxonomyWidget::onPartitionHovered(QString name)
{
    qDebug() << "You hovered over partition: " << name;
}
