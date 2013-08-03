#include "componentsprite.h"
#include "widgetmisc.h"
#include "widgetnumber.h"
#include "widgetTexture.h"
#include "cocos2d.h"
#include "CCGeometry.h"
#include <functional>

USING_NS_CC;

void ComponentSprite::Populate(QTreeWidget* tree, QTreeWidgetItem* parent, Node* node)
{
    ComponentNode::Populate(tree, parent, node);
    ADD_FIELD(tree, parent, "flip X",  widgetBool, Sprite, node, bool,    setFlipX,   isFlipX,    1);
    ADD_FIELD(tree, parent, "flip Y",  widgetBool, Sprite, node, bool,    setFlipY,   isFlipY,    1);
    ADD_FIELD(tree, parent, "opacity", widgetInt,  Sprite, node, uint8_t, setOpacity, getOpacity, 1);

    {
        // for textures we use a lamda for two reasons
        // 1. we need to remove the const qualifier since the method takes a non-const texture.
        // 2. we need to adjust the texture rect of the sprite to match the texture size.
        auto setter = [] (Sprite* node, Texture2D* const& value)
        {
            node->setTexture(const_cast<Texture2D*>(value));
            Size size = value->getContentSizeInPixels();
            node->setTextureRect(Rect(0, 0, size.width, size.height));
        };
        connectFieldT<widgetTexture, Sprite, Texture2D*>(this, tree, parent, "texture", node, setter, GETTER(Sprite, Texture2D*, getTexture));
    }
}
