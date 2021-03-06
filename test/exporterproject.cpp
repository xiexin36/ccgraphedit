#include "exporterproject.h"
#include "nodeitem.h"
#include "NodeDriver.h"
#include "cocos2d.h"
#include "CCStreamFormatted.h"
#include "mysceneeditor.h"

namespace
{
    const uint32_t kExporterFormatMagicNumber = 0xdeadbeef;
    const uint32_t kVersion = 1;
}

USING_NS_CC;

bool ExporterProject::ExportToStream(StreamFormatted& stream)
{
    // write out file format sentinel
    stream.write(kExporterFormatMagicNumber);

    // write out the version
    stream.write(kVersion);

    Node* rootNode = MySceneEditor::instance()->GetRootNode();
    NodeItem* rootItem = MainWindow::instance()->GetNodeItemFromNode(rootNode);

    return ExportNode(stream, rootItem);
}

//
// Specializations
//

bool ExporterProject::ExportProperty(cocos2d::StreamFormatted& stream, float* value)
{
    return sizeof(*value) == stream.write(*value);
}

bool ExporterProject::ExportProperty(cocos2d::StreamFormatted& stream, int* value)
{
    return sizeof(*value) == stream.write(*value);
}

bool ExporterProject::ExportProperty(cocos2d::StreamFormatted& stream, bool* value)
{
    return sizeof(*value) == stream.write(*value);
}

bool ExporterProject::ExportProperty(cocos2d::StreamFormatted& stream, cocos2d::Point* value)
{
    stream.write(value->x);
    stream.write(value->y);
    return true;
}

bool ExporterProject::ExportProperty(cocos2d::StreamFormatted& stream, cocos2d::Size* value)
{
    stream.write(value->width);
    stream.write(value->height);
    return true;
}

bool ExporterProject::ExportProperty(cocos2d::StreamFormatted& stream, cocos2d::_ccColor3B* value)
{
    stream.write(value->r);
    stream.write(value->g);
    stream.write(value->b);
    return true;
}

bool ExporterProject::ExportProperty(cocos2d::StreamFormatted& stream, std::string* value)
{
    uint32_t length = value->length();
    stream.write(length);
    stream.write(value->c_str(), length);
    return true;
}

bool ExporterProject::ExportProperty(cocos2d::StreamFormatted& stream, uint8_t* value)
{
    return sizeof(*value) == stream.write(*value);
}

//
// Protected Methods
//

bool ExporterProject::ExportNode(StreamFormatted& stream, NodeItem* item)
{
    Node* node = item->GetNode();

    // write out the node class id
    stream.write(node->classId());

    // write out the node name
    std::string name(item->SceneItem()->text(0).toUtf8());
    ExportProperty(stream, &name);

    // write out the count of items
    stream.write(uint32_t(item->Drivers().size()));

    // export each node driver
    const NodeItem::tNodeDrivers& drivers = item->Drivers();
    NodeItem::tNodeDrivers::const_iterator it(drivers.begin()), itEnd(drivers.end());
    for (; it != itEnd; ++it)
    {
        INodeDriver* driver = *it;

        // write out id of driver
        stream.write(driver->Id());

        // write out driver data
        driver->Update();
        driver->Export(stream, this);
    }

    // export children
    Array* children = node->getChildren();

    // write child count
    stream.write(uint32_t(children ? children->count() : 0));

    Object* object;
    CCARRAY_FOREACH(children, object)
    {
        Node* child = (Node*)object;
        NodeItem* childItem = MainWindow::instance()->GetNodeItemFromNode(child);
        if (childItem)
        {
            if (!ExportNode(stream, childItem))
            {
                // insert logging
                return false;
            }
        }
    }

    return true;
}
