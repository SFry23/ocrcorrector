#include "Dictionary.h"


//------------------------------------------------------------------------------
//  Dictionary::Dictionary()
//------------------------------------------------------------------------------
Dictionary::Dictionary(const QString path)
{
    _path = path;
    load();
}

//------------------------------------------------------------------------------
//  Dictionary::add()
//------------------------------------------------------------------------------
void Dictionary::add(const QString s)
{
    if (_words.size() > 1)
    {
        QStringList::iterator it = qLowerBound(_words.begin(), _words.end(), s);

        if (*it != s)
            _words.insert(it, s);
    }
    else if (not _words.empty() and s < _words[0])
    {
        _words.insert(_words.begin(), s);
    }
    else
    {
        _words.insert(_words.end(), s);
    }
}

//------------------------------------------------------------------------------
//  Dictionary::getPath()
//------------------------------------------------------------------------------
QString Dictionary::getPath()
{
    return _path;
}

//------------------------------------------------------------------------------
//  Dictionary::load()
//------------------------------------------------------------------------------
bool Dictionary::load()
{
    QFile file(_path);
    if (file.open(QFile::ReadOnly))
    {
        _words.clear();
        QTextStream content(&file);
        while (not content.atEnd())
            _words.append(content.readLine());

        return true;
    }
    else
    {
        return false;
    }
}

//------------------------------------------------------------------------------
//  Dictionary::remove()
//------------------------------------------------------------------------------
bool Dictionary::remove(const QString s)
{
    QStringList::const_iterator it = qLowerBound(_words, s);
    int i = it - _words.begin();

    if (_words.at(i) != s)
        return false;
    else
    {
        _words.removeAt(i);
        return true;
    }
}

//------------------------------------------------------------------------------
//  Dictionary::save()
//------------------------------------------------------------------------------
bool Dictionary::save(const QString f)
{
    QString filename;

    if (not f.isEmpty())
        filename = f;
    else
        filename = _path;

    if (not filename.isNull())
    {
        QFile file(filename);

        if (file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
        {
            QTextStream out(&file);
            for (int i = 0; i < _words.length(); i++)
                out << _words.at(i) + "\n";
            file.flush();

            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
//  Dictionary::search()
//------------------------------------------------------------------------------
int Dictionary::search(const QString value)
{
    if (not _words.empty())
    {
        QStringList::const_iterator i = qBinaryFind(_words, value);

        if (i != _words.end())
            return (i - _words.begin());
    }

    return -1;
}

//------------------------------------------------------------------------------
//  Dictionary::set()
//------------------------------------------------------------------------------
void Dictionary::set(QStringList l)
{
    qSort(l);
    _words = QStringList(l);
}

//------------------------------------------------------------------------------
//  Dictionary::setPath()
//------------------------------------------------------------------------------
void Dictionary::setPath(const QString filename)
{
    if (not filename.isEmpty())
    {
        _path = filename;
    }
}

