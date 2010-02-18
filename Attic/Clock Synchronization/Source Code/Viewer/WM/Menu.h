#pragma once


#include <vector>
#include "Exception.h"
#include "MenuItem.h"


namespace WM {
    // TODO: Distinguish between MenuBar and PopupMenu?
    class Menu: public MenuItem {
        friend class Window;

    private:
        HMENU _handle;
        std::vector<ref<MenuItem>> _items;


    public:
        Menu(ref<String> caption):
            MenuItem(caption), _handle(CreatePopupMenu())
        {
            if (_handle == NULL) {
                throw Exception(GetLastErrorMessage());
            }
        }


        virtual void add(ref<MenuItem> item) {
            UINT flags = item->getType();
            UINT_PTR id = item->getId();
            LPCTSTR caption = item->getCaption()->c_str();

            if (!AppendMenu(getHandle(), flags, id, caption)) {
                throw Exception(GetLastErrorMessage());
            }

            _items.push_back(item);
        }


        virtual HMENU getHandle() {
            return _handle;
        }


        virtual size_t getItemCount() {
            return _items.size();
        }


    protected:
        virtual UINT_PTR getId() {
            return (UINT_PTR) getHandle();
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

            return NULL;
        }


        virtual UINT getType() {
            return MF_POPUP;
        }
    };
}
