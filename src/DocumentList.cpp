#include "DocumentList.h"
#include <vector>



//------------------------------------------------------------------------------
//  DocumentList::DocumentList()
//------------------------------------------------------------------------------
DocumentList::DocumentList()
{
    _index = 0;

    _images.clear();
    _texts.clear();

    _singleText = 0;

    _locked = false;
}

//------------------------------------------------------------------------------
//  DocumentList::~DocumentList()
//------------------------------------------------------------------------------
DocumentList::~DocumentList()
{
    clear();
}

//------------------------------------------------------------------------------
//  DocumentList::add()
//------------------------------------------------------------------------------
void DocumentList::add(TextDocument* textFile, QFile* imageFile)
{
    _texts.push_back(textFile);
    _images.push_back(imageFile);
}

//------------------------------------------------------------------------------
//  DocumentList::add()
//------------------------------------------------------------------------------
void DocumentList::add(QString textFilename, QString imageFilename)
{
    if (not textFilename.isEmpty())
        _texts.push_back(new TextDocument(textFilename));
    else
        _texts.push_back(0);

    if (not imageFilename.isEmpty())
        _images.push_back(new QFile(imageFilename));
    else
        _images.push_back(0);
}

//------------------------------------------------------------------------------
//  DocumentList::clear()
//------------------------------------------------------------------------------
void DocumentList::clear()
{
    _images.clear();
    _texts.clear();

    _index = 0;
}

//------------------------------------------------------------------------------
//  DocumentList::getIndex()
//------------------------------------------------------------------------------
int DocumentList::getIndex()
{
    return _index;
}

//------------------------------------------------------------------------------
//  DocumentList::size()
//------------------------------------------------------------------------------
int DocumentList::size()
{
    return (int) _images.size();
}

//------------------------------------------------------------------------------
//  DocumentList::getImageFile()
//------------------------------------------------------------------------------
QFile* DocumentList::getImageFile(int position)
{
    if (position == -1 and not _images.empty())
        return _images[_index];
    else if (position >= 0 and position < (int) _images.size())
        return _images[position];
    else
        return 0;
}

//------------------------------------------------------------------------------
//  DocumentList::getImageFilename()
//------------------------------------------------------------------------------
QString DocumentList::getImageFilename(int position)
{
    if (getImageFile(position) != 0)
        return getImageFile(position)->fileName();
    else
        return QString();
}

//------------------------------------------------------------------------------
//  DocumentList::getTextFile()
//------------------------------------------------------------------------------
TextDocument* DocumentList::getTextFile(int position)
{
    if (isLocked())
    {
        return _singleText;
    }
    else
    {
        if (position == -1 and not _texts.empty())
            return _texts[_index];
        else if (position >= 0 and position < (int) _texts.size())
            return _texts[position];
        else
            return 0;
    }
}

//------------------------------------------------------------------------------
//  DocumentList::getTextFilename()
//------------------------------------------------------------------------------
QString DocumentList::getTextFilename(int position)
{
    if (isLocked())
    {
        if (_singleText != 0)
            return _singleText->fileName();
        else
            return QString();
    }
    else
    {
        if (getTextFile(position) != 0)
            return getTextFile(position)->fileName();
        else
            return QString();
    }
}

//------------------------------------------------------------------------------
//  DocumentList::hasNext()
//------------------------------------------------------------------------------
bool DocumentList::hasNext(bool textOnly)
{
    if (textOnly)
    {
        if (isLocked())
            return false;
    }

    return _index < static_cast<int>(_images.size()) - 1;
}

//------------------------------------------------------------------------------
//  DocumentList::hasPrevious()
//------------------------------------------------------------------------------
bool DocumentList::hasPrevious(bool textOnly)
{
    if (textOnly)
    {
        if (isLocked())
            return false;
    }

    return _index > 0;
}

//------------------------------------------------------------------------------
//  DocumentList::isLocked()
//------------------------------------------------------------------------------
bool DocumentList::isLocked()
{
    return _locked;
}

//------------------------------------------------------------------------------
//  DocumentList::moveTo()
//------------------------------------------------------------------------------
void DocumentList::moveTo(int position)
{
    if (position >= 0 and position < size())
        _index = position;
}

//------------------------------------------------------------------------------
//  DocumentList::next()
//------------------------------------------------------------------------------
void DocumentList::next()
{
    if (hasNext())
        _index++;
}

//------------------------------------------------------------------------------
//  DocumentList::previous()
//------------------------------------------------------------------------------
void DocumentList::previous()
{
    if (hasPrevious())
        _index--;
}

//------------------------------------------------------------------------------
//  DocumentList::removeCurrent()
//------------------------------------------------------------------------------
void DocumentList::removeCurrent()
{
    if (_index < size())
    {
        delete _texts[_index];
        delete _images[_index];

        _texts[_index] = 0;
        _images[_index] = 0;

        _texts.erase(_texts.begin() + _index);
        _images.erase(_images.begin() + _index);

        if (_index > 0 and _index > size() - 1)
            _index--;
    }
}

//------------------------------------------------------------------------------
//  DocumentList::setLocked()
//------------------------------------------------------------------------------
void DocumentList::setLocked(bool locked)
{
    _locked = locked;

    if (locked == false)
    {
        delete _singleText;
        _singleText = 0;
    }
}

//------------------------------------------------------------------------------
//  DocumentList::setTextFilename()
//------------------------------------------------------------------------------
void DocumentList::setTextFilename(QString filename, int position)
{
    if (not filename.isEmpty())
    {
        if (isLocked())
        {
            if (_singleText != 0)
                _singleText->setFileName(filename);
            else
            {
                _singleText = new TextDocument(filename);
            }
        }
        else
        {
            TextDocument* textFile = getTextFile(position);

            if (textFile != 0)
            {
                textFile->setFileName(filename);
            }
            else
            {
                int index = position;

                if (position == -1)
                    index = _index;

                if (position < (int) _texts.size())
                {
                    _texts[index] = new TextDocument(filename);
                }
            }
        }
    }
}



