#pragma once


#include <vector>
#include "Exception.h"
#include "MenuItem.h"


namespace wm {
    class Menu: public MenuItem {
    private:
        HMENU _handle;
        std::vector<ref<MenuItem>> _items;


    public:
        Menu(String caption):
            MenuItem(caption), _handle(CreatePopupMenu())
        {
            if (_handle == NULL) {
                Exception::throwLastError();
            }
        }


        virtual void add(ref<MenuItem> item) {
            UINT flags = item->getType();
            UINT_PTR id = item->getId();
            LPCTSTR caption = item->getCaption().c_str();

            if (!AppendMenu(getMenuHandle(), flags, id, caption)) {
                Exception::throwLastError();
            }

            _items.push_back(item);
        }


        virtual HMENU getMenuHandle() {
            return _handle;
        }


        virtual UINT_PTR getId() {
            return (UINT_PTR) getMenuHandle();
        }


        virtual ref<MenuItem> getItemById(UINT_PTR id) {
            for (size_t i = 0; i < getItemCount(); ++i) {
                ref<MenuItem> item = _items[i];

                if (item->getId() == id) {
                    return item;
                }
                else if (item->getType() == MF_POPUP) {
                    ref<MenuItem> child = item.cast<Menu>()->getItemById(id);

                    if (child != NULL) {
                        return child;
                    }
                }
            }

            throw Exception(S("No menu item with such identifier."));
        }


        virtual size_t getItemCount() {
            return _items.size();
        }


        virtual UINT getType() {
            return MF_POPUP;
        }


        virtual void setOwnerHandle(HWND owner) {
            MenuItem::setOwnerHandle(owner);

            for (size_t i = 0; i < getItemCount(); ++i) {
                _items[i]->setOwnerHandle(owner);
            }
        }
    };
}
