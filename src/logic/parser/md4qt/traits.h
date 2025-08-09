/*
    SPDX-FileCopyrightText: 2022-2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: MIT
*/

#ifndef MD4QT_MD_TRAITS_HPP_INCLUDED
#define MD4QT_MD_TRAITS_HPP_INCLUDED

#ifdef MD4QT_QT_SUPPORT

// C++ include.
#include <map>
#include <memory>
#include <variant>

// Qt include.
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QVector>

#endif // MD4QT_QT_SUPPORT

namespace MD
{

/*!
 * \class MD::StringVariant
 * \inmodule md4qt
 * \inheaderfile md4qt/traits.h
 *
 * \brief String variant that may holds string or string view.
 *
 * String variant that may holds string or string view. Modifications are
 * allowed even if it holds string view - in this case will be made deep copy
 * to string from view, and modifications will be made on copied string.
 */
template<class String, class StringView>
class StringVariant
{
public:
    /*!
     * Default constructor of empty string variant.
     */
    StringVariant()
    {
    }

    /*!
     * Constructor from string.
     */
    StringVariant(const String &s)
        : m_string(s)
        , m_isView(false)
    {
    }

    /*!
     * Constructor from string view.
     */
    StringVariant(const StringView &v)
        : m_view(v)
    {
    }

    /*!
     * Copy operator.
     */
    StringVariant &operator=(const StringVariant &) = default;
    /*!
     * Copy constructor.
     */
    StringVariant(const StringVariant &) = default;

    /*!
     * Returns character at the given position.
     *
     * \a pos Position.
     */
    typename String::value_type operator[](long long int pos) const
    {
        return (m_isView ? m_view[pos] : m_string[pos]);
    }

    /*!
     * Returns length of the string.
     */
    long long int length() const
    {
        return (m_isView ? m_view.length() : m_string.length());
    }

    /*!
     * Returns deep copy of the string.
     */
    String toString() const
    {
        return (m_isView ? m_view.toString() : m_string);
    }

    /*!
     * Returns sliced of this string variant. Doesn't do any copying.
     *
     * \a pos Start position.
     *
     * \a n Count of characters.
     */
    StringVariant sliced(long long int pos, long long int n) const
    {
        return (m_isView ? StringVariant(m_view.sliced(pos, n)) : StringVariant(m_string.sliced(pos, n)));
    }

    /*!
     * Returns whether this string is empty.
     */
    bool isEmpty() const
    {
        return (m_isView ? m_view.isEmpty() : m_string.isEmpty());
    }

    /*!
     * Insert string at the given position.
     *
     * \a pos Position.
     *
     * \a what A string to insert.
     */
    StringVariant &insert(long long int pos, const String &what)
    {
        if (m_isView) {
            m_string = m_view.toString();
            m_isView = false;
            m_view = {};
        }

        m_string.insert(pos, what);

        return *this;
    }

    /*!
     * Removes from the given position the given count of characters.
     *
     * \a pos Position.
     *
     * \a size Count of characters.
     */
    StringVariant &remove(long long int pos, long long int size)
    {
        if (m_isView) {
            m_string = m_view.toString();
            m_isView = false;
            m_view = {};
        }

        m_string.remove(pos, size);

        return *this;
    }

    /*!
     * Clear this string variant to default empty state.
     */
    void clear()
    {
        m_isView = true;
        m_view = {};
        m_string.clear();
    }

private:
    String m_string;
    StringView m_view;
    bool m_isView = true;
};

/*!
 * \class MD::InternalStringT
 * \inmodule md4qt
 * \inheaderfile md4qt/traits.h
 *
 * \brief Internal string, used to get virgin (original) string from transformed string.
 *
 * Actually this is a wrapper around string implemented to have ability to get virgin string/substring
 * after implemented modifications. In other words - if from string "abc" remove letter "b" and then
 * ask for virgin substring starting at position 0 and length 2, this class will give you virgin
 * "abc" string.
 */
template<class String, class StringView, class Char, class Latin1Char>
class InternalStringT
{
public:
    /*!
     * \typealias MD::InternalStringT::value_type
     * \inmodule md4qt
     * \inheaderfile md4qt/traits.h
     *
     * \brief Value type of a symbol in string.
     */
    using value_type = Char;

    /*!
     * Default constructor.
     */
    InternalStringT()
    {
    }

    /*!
     * Constructor from string.
     */
    InternalStringT(const String &s)
        : m_string(s)
        , m_virginStr(s)
    {
    }

    /*!
     * Constructor from string view.
     */
    InternalStringT(StringView s)
        : m_string(s)
        , m_virginStr(s)
    {
    }

    /*!
     * Returns full virgin string.
     */
    String fullVirginString() const
    {
        return m_virginStr.toString();
    }

    /*!
     * Returns copy of string view as string
     */
    String toString() const
    {
        return m_string.toString();
    }

    /*!
     * Returns a length of this string.
     */
    long long int length() const
    {
        return m_string.length();
    }

    /*!
     * Clear string.
     */
    void clear()
    {
        m_string.clear();
        m_virginStr = {};
        m_changedPos.clear();
    }

    /*!
     * Returns virgin sub-string with position and length in the transformed string.
     *
     * \a pos Position.
     *
     * \a len Length.
     */
    String virginSubString(long long int pos = 0, long long int len = -1) const
    {
        if (pos < 0) {
            pos = 0;
        }

        if (pos + len > length() || len < 0) {
            len = length() - pos;
        }

        if (len == 0) {
            return (isEmpty() ? m_virginStr.toString() : String());
        }

        auto virginStartPos = virginPos(pos);
        String startStr, endStr;

        if (m_virginStr[virginStartPos] == Latin1Char('\t')) {
            const auto spaces = countOfSpacesForTab(virginStartPos);

            for (long long int i = 1; i < spaces; ++i) {
                if (virginPos(pos + i) != virginStartPos) {
                    startStr = String(i, Latin1Char(' '));
                    ++virginStartPos;
                    break;
                }
            }
        }

        auto virginEndPos = virginPos(pos + len - 1, true);

        if (m_virginStr[virginEndPos] == Latin1Char('\t')) {
            const auto spaces = countOfSpacesForTab(virginEndPos);

            for (long long int i = 1; i < spaces; ++i) {
                if (virginPos(pos + len - 1 - i) != virginEndPos) {
                    endStr = String(i, Latin1Char(' '));
                    --virginEndPos;
                    break;
                }
            }
        }

        return startStr + m_virginStr.sliced(virginStartPos, virginEndPos - virginStartPos + 1).toString() + endStr;
    }

    /*!
     * Returns virgin position from transformed.
     *
     * \a pos Transformed position.
     *
     * \a end If true will be return last virgin position before transformation.
     *        For example if in virgin string 2 characters were replaced with 1,
     *        we will receive position of second character if \a end is true.
     */
    long long int virginPos(long long int pos, bool end = false) const
    {
        for (auto it = m_changedPos.crbegin(), last = m_changedPos.crend(); it != last; ++it) {
            pos = virginPosImpl(pos, *it, end);
        }

        return pos;
    }

    /*!
     * Returns character at a given \a position position.
     *
     * \a position Position.
     */
    Char operator[](long long int position) const
    {
        return m_string[position];
    }

    /*!
     * Replace substring.
     *
     * \a pos Position.
     *
     * \a size Length.
     *
     * \a with Value to insert.
     */
    InternalStringT &replaceOne(long long int pos, long long int size, const String &with)
    {
        const auto len = m_string.length();

        m_string.remove(pos, size);
        m_string.insert(pos, with);

        if (with.length() != size) {
            m_changedPos.push_back({{0, len}, {}});
            m_changedPos.back().second.push_back({pos, size, with.length()});
        }

        return *this;
    }

    /*!
     * Find string.
     *
     * \a what What to find.
     *
     * \a from Start position.
     */
    template<class T>
    long long int indexOf(const T &what, long long int from = 0) const
    {
        if (from < 0 || from >= length()) {
            return -1;
        }

        if (what.isEmpty()) {
            return 0;
        }

        for (long long int i = from; i <= length() - what.length(); ++i) {
            if (this->operator[](i) == what[0]) {
                bool match = true;
                for (long long int j = 1; j < what.length(); ++j) {
                    if (this->operator[](i + j) != what[j]) {
                        match = false;
                        break;
                    }
                }

                if (match) {
                    return i;
                }
            }
        }

        return -1;
    }

    /*!
     * Returns whether this string contains a given string.
     *
     * \a what What to find.
     */
    template<class T>
    bool contains(const T &what) const
    {
        return (indexOf(what) != -1);
    }

    /*!
     * Returns whether this string starts with a given string.
     *
     * \a what What to find.
     */
    template<class T>
    bool startsWith(const T &what) const
    {
        if (what.isEmpty()) {
            return true;
        }

        if (what.length() > length()) {
            return false;
        }

        for (long long int i = 0; i < what.length(); ++i) {
            if (this->operator[](i) != what[i]) {
                return false;
            }
        }

        return true;
    }

    /*!
     * Returns whether this string ends with a given string.
     *
     * \a what What to find.
     */
    template<class T>
    bool endsWith(const T &what) const
    {
        if (what.isEmpty()) {
            return true;
        }

        const auto l = length();

        if (what.length() > l) {
            return false;
        }

        for (long long int i = 0; i < what.length(); ++i) {
            if (this->operator[](l - i - 1) != what[i]) {
                return false;
            }
        }

        return true;
    }

    /*!
     * Replace string.
     *
     * \a what What to replace.
     *
     * \a with Value to insert.
     */
    InternalStringT &replace(const String &what, const String &with)
    {
        long long int pos = 0;

        while ((pos = indexOf(what, pos)) != -1) {
            replaceOne(pos, what.length(), with);
            pos += with.length();
        }

        return *this;
    }

    /*!
     * Remove sub-string.
     *
     * \a pos Position.
     *
     * \a size Length.
     */
    InternalStringT &remove(long long int pos, long long int size)
    {
        const auto len = m_string.length();

        m_string.remove(pos, size);

        m_changedPos.push_back({{0, len}, {}});
        m_changedPos.back().second.push_back({pos, size, 0});

        return *this;
    }

    /*!
     * Returns whether this string empty?
     */
    bool isEmpty() const
    {
        return m_string.isEmpty();
    }

    /*!
     * Returns simplified string.
     */
    InternalStringT simplified() const
    {
        if (isEmpty()) {
            return *this;
        }

        const auto len = length();

        InternalStringT result = *this;
        String tmpString;

        long long int i = 0;
        bool init = false;
        bool first = true;
        long long int spaces = 0;

        while (true) {
            long long int tmp = i;

            while (i < length() && this->operator[](i).isSpace()) {
                ++i;
            }

            spaces = i - tmp;

            if (i != tmp) {
                if (!init) {
                    result.m_changedPos.push_back({{0, len}, {}});
                    init = true;
                }

                if (i - tmp > 1 || first) {
                    result.m_changedPos.back().second.push_back({tmp, i - tmp, (first ? 0 : 1)});
                }
            }

            first = false;

            while (i != len && !this->operator[](i).isSpace()) {
                tmpString.push_back(this->operator[](i));
                ++i;
            }

            if (i == len) {
                break;
            }

            tmpString.push_back(Latin1Char(' '));
        }

        if (!tmpString.isEmpty() && tmpString[tmpString.length() - 1] == Latin1Char(' ')) {
            tmpString.remove(tmpString.length() - 1, 1);

            if (spaces > 1) {
                result.m_changedPos.back().second.back().m_len = 0;
            } else if (spaces == 1) {
                result.m_changedPos.back().second.push_back({length() - spaces, spaces, 0});
            }
        }

        result.m_string = tmpString;

        return result;
    }

    /*!
     * Split string.
     *
     * \a sep Separator.
     */
    std::vector<InternalStringT> split(const InternalStringT &sep) const
    {
        std::vector<InternalStringT> result;
        const auto len = length();

        if (sep.isEmpty()) {
            for (long long int i = 0; i < len; ++i) {
                auto is = *this;
                is.m_string.clear();
                is.m_string = String(1, this->operator[](i));
                is.m_changedPos.push_back({{i, len}, {}});

                result.push_back(is);
            }

            return result;
        }

        long long int pos = 0;
        long long int fpos = 0;

        while ((fpos = indexOf(sep, pos)) != -1 && fpos < len) {
            if (fpos - pos > 0) {
                result.push_back(sliced(pos, fpos - pos));
            }

            pos = fpos + sep.length();
        }

        if (pos < len) {
            result.push_back(sliced(pos, len - pos));
        }

        return result;
    }

    /*!
     * Returns sliced sub-string.
     *
     * \a pos Position.
     *
     * \a len Length.
     */
    InternalStringT sliced(long long int pos, long long int len = -1) const
    {
        InternalStringT tmp = *this;
        tmp.m_string.clear();
        const auto oldLen = length();
        tmp.m_string = m_string.sliced(pos, (len == -1 ? oldLen - pos : len));

        tmp.m_changedPos.push_back({{pos, oldLen}, {}});
        if (len != -1 && len < oldLen - pos) {
            tmp.m_changedPos.back().second.push_back({pos + len, oldLen - pos - len, 0});
        }

        return tmp;
    }

    /*!
     * Returns a substring that contains the \a n rightmost characters of the string.
     *
     * \a n Count of characters.
     */
    InternalStringT right(long long int n) const
    {
        return sliced(length() - n, n);
    }

    /*!
     * Insert one character.
     *
     * \a pos Position.
     *
     * \a ch Character.
     */
    InternalStringT &insert(long long int pos, Char ch)
    {
        return insert(pos, String(1, ch));
    }

    /*!
     * Insert string.
     *
     * \a pos Position.
     *
     * \a s String.
     */
    InternalStringT &insert(long long int pos, const String &s)
    {
        const auto len = m_string.length();
        const auto ilen = s.length();

        m_string.insert(pos, s);

        m_changedPos.push_back({{0, len}, {}});
        m_changedPos.back().second.push_back({pos, 1, ilen + 1});

        return *this;
    }

private:
    /*!
     * Transformed string.
     */
    StringVariant<String, StringView> m_string;
    /*!
     * Virgin (original) string.
     */
    StringVariant<String, StringView> m_virginStr;

    /*!
     * Auxiliary struct to store information about transformation.
     */
    struct ChangedPos {
        long long int m_pos = -1;
        long long int m_oldLen = -1;
        long long int m_len = -1;
    };

    /*!
     * Auxiliary struct to store information about transformation.
     */
    struct LengthAndStartPos {
        long long int m_firstPos = 0;
        long long int m_length = 0;
    };

    /*
     * Information about transformations.
     */
    std::vector<std::pair<LengthAndStartPos, std::vector<ChangedPos>>> m_changedPos;

private:
    long long int virginPosImpl(long long int pos, const std::pair<LengthAndStartPos, std::vector<ChangedPos>> &changed, bool end) const
    {
        for (const auto &c : changed.second) {
            const auto startPos = c.m_pos;
            const auto endPos = startPos + c.m_len - 1;

            if (pos >= startPos && pos <= endPos) {
                const auto oldEndPos = startPos + c.m_oldLen - 1;

                if (pos > oldEndPos || end) {
                    return oldEndPos + changed.first.m_firstPos;
                } else {
                    return pos + changed.first.m_firstPos;
                }
            } else if (pos > endPos) {
                pos += c.m_oldLen - c.m_len;
            } else {
                break;
            }
        }

        pos += changed.first.m_firstPos;

        return (pos > changed.first.m_length ? changed.first.m_length : pos);
    }

    long long int countOfSpacesForTab(long long int virginPos) const
    {
        long long int p = 0;

        for (const auto &v : std::as_const(m_changedPos)) {
            p += v.first.m_firstPos;

            if (virginPos < p) {
                break;
            }

            for (const auto &c : std::as_const(v.second)) {
                if (c.m_pos + p == virginPos) {
                    return c.m_len;
                }

                virginPos += (virginPos > c.m_pos ? c.m_len - c.m_oldLen : 0);
            }
        }

        return -1;
    }
}; // class InternalString

/*!
 * \inheaderfile md4qt/traits.h
 *
 * Comparison of MD::InternalStringT with other string.
 *
 * \a str First string to compare.
 *
 * \a other Other string to compare.
 */
template<class String, class StringView, class Char, class Latin1Char, class T>
bool operator==(const InternalStringT<String, StringView, Char, Latin1Char> &str, const T &other)
{
    const auto length = str.length();

    if (length != other.length()) {
        return false;
    }

    for (long long int i = 0; i < length; ++i) {
        if (str[i] != other[i]) {
            return false;
        }
    }

    return true;
}

#ifdef MD4QT_QT_SUPPORT

//
// QStringTrait
//

/*!
 * \class MD::QStringTrait
 * \inmodule md4qt
 * \inheaderfile md4qt/traits.h
 *
 * \brief Trait to use this library with QString.
 *
 * Trait for \c {md4qt} library to work with QString as string.
 */
struct QStringTrait {
    template<class T>
    using Vector = QVector<T>;

    template<class T, class U>
    using Map = std::map<T, U>;

    using String = QString;

    using StringView = QStringView;

    using Char = QChar;

    using InternalString = InternalStringT<String, StringView, Char, QLatin1Char>;

    using InternalStringList = std::vector<InternalString>;

    using TextStream = QTextStream;

    using StringList = QStringList;

    using Url = QUrl;

    /*!
     * Returns whether Unicode whitespace?
     *
     * \a ch Character to check.
     */
    static bool isUnicodeWhitespace(const QChar &ch)
    {
        const auto c = ch.unicode();

        if (ch.category() == QChar::Separator_Space) {
            return true;
        } else if (c == 0x09 || c == 0x0A || c == 0x0C || c == 0x0D) {
            return true;
        } else {
            return false;
        }
    }

    /*!
     * Convert UTF-16 into trait's string.
     *
     * \a u16 String.
     */
    static String utf16ToString(const char16_t *u16)
    {
        return QString::fromUtf16(u16);
    }

    /*!
     * Convert Latin1 into trait's string.
     *
     * \a latin1 String.
     */
    static String latin1ToString(const char *latin1)
    {
        return QLatin1String(latin1);
    }

    /*!
     * Convert Latin1 char into trait's char.
     *
     * \a latin1 Character.
     */
    static Char latin1ToChar(char latin1)
    {
        return QLatin1Char(latin1);
    }

    /*!
     * Convert UTF8 into trait's string.
     *
     * \a utf8 UTF-8 string.
     */
    static String utf8ToString(const char *utf8)
    {
        return QString::fromUtf8(utf8, -1);
    }

    /*!
     * Returns whether file exist.
     *
     * \a fileName File name.
     *
     * \a workingPath Working path.
     */
    static bool fileExists(const String &fileName, const String &workingPath)
    {
        return QFileInfo::exists((workingPath.isEmpty() ? QString() : workingPath + latin1ToString("/")) + fileName);
    }

    /*!
     * Returns whether file exist.
     *
     * \a fileName File name.
     */
    static bool fileExists(const String &fileName)
    {
        return QFileInfo::exists(fileName);
    }

    /*!
     * Returns absolute file path.
     *
     * \a path Path.
     */
    static String absoluteFilePath(const String &path)
    {
        return QFileInfo(path).absoluteFilePath();
    }

    /*!
     * Add UCS4 to string.
     *
     * \a str String.
     *
     * \a ch Character to append.
     */
    static void appendUcs4(String &str, char32_t ch)
    {
        str += QChar::fromUcs4(ch);
    }

    /*!
     * Search for last occurrence of string.
     *
     * \a where String for checking.
     *
     * \a what What to look for?
     *
     * \a from Start position.
     */
    static long long int lastIndexOf(const String &where, const String &what, long long int from)
    {
        if (from < 0) {
            return -1;
        } else {
            return where.lastIndexOf(what, from);
        }
    }
}; // struct QStringTrait

#endif // MD4QT_QT_SUPPORT

} /* namespace MD */

#endif // MD4QT_MD_TRAITS_HPP_INCLUDED
