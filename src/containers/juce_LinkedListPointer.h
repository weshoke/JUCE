/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCE_LINKEDLISTPOINTER_JUCEHEADER__
#define __JUCE_LINKEDLISTPOINTER_JUCEHEADER__


//==============================================================================
/**
    Helps to manipulate singly-linked lists of objects.

    For objects that are designed to contain a pointer to the subsequent item in the
    list, this class contains methods to deal with the list. To use it, the ObjectType
    class that it points to must contain a LinkedListPointer called nextListItem, e.g.

    @code
    struct MyObject
    {
        int x, y, z;

        // A linkable object must contain a member with this name and type, which must be
        // accessible by the LinkedListPointer class. (This doesn't mean it has to be public -
        // you could make your class a friend of a LinkedListPointer<MyObject> instead).
        LinkedListPointer<MyObject> nextListItem;
    };

    LinkedListPointer<MyObject> myList;
    myList.append (new MyObject());
    myList.append (new MyObject());

    int numItems = myList.size(); // returns 2
    MyObject* lastInList = myList.getLast();
    @endcode
*/
template <class ObjectType>
class LinkedListPointer
{
public:
    //==============================================================================
    /** Creates a null pointer to an empty list. */
    LinkedListPointer() throw()
        : item (0)
    {
    }

    /** Creates a pointer to a list whose head is the item provided. */
    explicit LinkedListPointer (ObjectType* const headItem) throw()
        : item (headItem)
    {
    }

    /** Sets this pointer to point to a new list. */
    LinkedListPointer& operator= (ObjectType* const newItem) throw()
    {
        item = newItem;
        return *this;
    }

    //==============================================================================
    /** Returns the item which this pointer points to. */
    inline operator ObjectType*() const throw()
    {
        return item;
    }

    /** Returns the item which this pointer points to. */
    inline ObjectType* get() const throw()
    {
        return item;
    }

    /** Returns the last item in the list which this pointer points to.
        This will iterate the list and return the last item found. Obviously the speed
        of this operation will be proportional to the size of the list. If the list is
        empty the return value will be this object.
        If you're planning on appending a number of items to your list, it's much more
        efficient to use the Appender class than to repeatedly call getLast() to find the end.
    */
    LinkedListPointer& getLast() throw()
    {
        LinkedListPointer* l = this;

        while (l->item != 0)
            l = &(l->item->nextListItem);

        return *l;
    }

    /** Returns the number of items in the list.
        Obviously with a simple linked list, getting the size involves iterating the list, so
        this can be a lengthy operation - be careful when using this method in your code.
    */
    int size() const throw()
    {
        int total = 0;

        for (ObjectType* i = item; i != 0; i = i->nextListItem)
            ++total;

        return total;
    }

    /** Returns the item at a given index in the list.
        Since the only way to find an item is to iterate the list, this operation can obviously
        be slow, depending on its size, so you should be careful when using this in algorithms.
    */
    LinkedListPointer& operator[] (int index) throw()
    {
        LinkedListPointer* l = this;

        while (--index >= 0 && l->item != 0)
            l = &(l->item->nextListItem);

        return *l;
    }

    /** Returns the item at a given index in the list.
        Since the only way to find an item is to iterate the list, this operation can obviously
        be slow, depending on its size, so you should be careful when using this in algorithms.
    */
    const LinkedListPointer& operator[] (int index) const throw()
    {
        const LinkedListPointer* l = this;

        while (--index >= 0 && l->item != 0)
            l = &(l->item->nextListItem);

        return *l;
    }

    /** Returns true if the list contains the given item. */
    bool contains (const ObjectType* const itemToLookFor) const throw()
    {
        for (ObjectType* i = item; i != 0; i = i->nextListItem)
            if (itemToLookFor == i)
                return true;

        return false;
    }

    //==============================================================================
    /** Inserts an item into the list, placing it before the item that this pointer
        currently points to.
    */
    void insertNext (ObjectType* const newItem)
    {
        jassert (newItem != 0);
        jassert (newItem->nextListItem == 0);
        newItem->nextListItem = item;
        item = newItem;
    }

    /** Inserts an item at a numeric index in the list.
        Obviously this will involve iterating the list to find the item at the given index,
        so be careful about the impact this may have on execution time.
    */
    void insertAtIndex (int index, ObjectType* newItem)
    {
        jassert (newItem != 0);
        LinkedListPointer* l = this;

        while (index != 0 && l->item != 0)
        {
            l = &(l->item->nextListItem);
            --index;
        }

        l->insertNext (newItem);
    }

    /** Replaces the object that this pointer points to, appending the rest of the list to
        the new object, and returning the old one.
    */
    ObjectType* replaceNext (ObjectType* const newItem) throw()
    {
        jassert (newItem != 0);
        jassert (newItem->nextListItem == 0);

        ObjectType* const oldItem = item;
        item = newItem;
        item->nextListItem = oldItem->nextListItem;
        oldItem->nextListItem = 0;
        return oldItem;
    }

    /** Adds an item to the end of the list.

        This operation involves iterating the whole list, so can be slow - if you need to
        append a number of items to your list, it's much more efficient to use the Appender
        class than to repeatedly call append().
    */
    void append (ObjectType* const newItem)
    {
        getLast().item = newItem;
    }

    /** Creates copies of all the items in another list and adds them to this one.
        This will use the ObjectType's copy constructor to try to create copies of each
        item in the other list, and appends them to this list.
    */
    void addCopyOfList (const LinkedListPointer& other)
    {
        LinkedListPointer* insertPoint = this;

        for (ObjectType* i = other.item; i != 0; i = i->nextListItem)
        {
            insertPoint->insertNext (new ObjectType (*i));
            insertPoint = &(insertPoint->item->nextListItem);
        }
    }

    /** Removes the head item from the list.
        This won't delete the object that is removed, but returns it, so the caller can
        delete it if necessary.
    */
    ObjectType* removeNext() throw()
    {
        if (item == 0)
            return 0;

        ObjectType* const oldItem = item;
        oldItem->nextListItem = 0;
        item = item->nextListItem;
        return oldItem;
    }

    /** Removes a specific item from the list.
        Note that this will not delete the item, it simply unlinks it from the list.
    */
    void remove (ObjectType* const item)
    {
        LinkedListPointer* l = findPointerTo (item);

        if (l != 0)
            l->removeNext();
    }

    /** Iterates the list, calling the delete operator on all of its elements and
        leaving this pointer empty.
    */
    void deleteAll()
    {
        while (item != 0)
        {
            ObjectType* const oldItem = item;
            item = oldItem->nextListItem;
            delete oldItem;
        }
    }

    /** Finds a pointer to a given item.
        If the item is found in the list, this returns the pointer that points to it. If
        the item isn't found, this returns null.
    */
    LinkedListPointer* findPointerTo (ObjectType* const itemToLookFor) throw()
    {
        LinkedListPointer* l = this;

        while (l->item != 0)
        {
            if (l->item == itemToLookFor)
                return l;

            l = &(l->item->nextListItem);
        }

        return 0;
    }

    /** Copies the items in the list to an array.
        The destArray must contain enough elements to hold the entire list - no checks are
        made for this!
    */
    void copyToArray (ObjectType** destArray) const throw()
    {
        jassert (destArray != 0);

        for (ObjectType* i = item; i != 0; i = i->nextListItem)
            *destArray++ = i;
    }

    //==============================================================================
    /**
        Allows efficient repeated insertions into a list.

        You can create an Appender object which points to the last element in your
        list, and then repeatedly call Appender::append() to add items to the end
        of the list in O(1) time.
    */
    class Appender
    {
    public:
        /** Creates an appender which will add items to the given list.
        */
        Appender (LinkedListPointer& endOfListPointer) throw()
            : endOfList (&endOfListPointer)
        {
            // This can only be used to add to the end of a list.
            jassert (endOfListPointer.item == 0);
        }

        /** Appends an item to the list. */
        void append (ObjectType* const newItem) throw()
        {
            *endOfList = newItem;
            endOfList = &(newItem->nextListItem);
        }

    private:
        LinkedListPointer* endOfList;

        JUCE_DECLARE_NON_COPYABLE (Appender);
    };

private:
    //==============================================================================
    ObjectType* item;
};


#endif   // __JUCE_LINKEDLISTPOINTER_JUCEHEADER__