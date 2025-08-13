# Welcome to KleverNotes!

__*KleverNotes*__ is a note taking and management application for your mobile and desktop devices. 
It uses Markdown, a lightweight markup language to format text in a simple and readable way, and allows you to preview your content.

KleverNotes supports different versions of Markdown thanks to the [md4qt](https://github.com/igormironchik/md4qt) parser with which the project works closely.

This note will try to give an overview of what Markdown, and to be more precise, the [CommonMark specification](https://commonmark.org/) offers.

* * *

# Heading Level 1
## Heading Level 2
### Heading Level 3
#### Heading Level 4
##### Heading Level 5
###### Heading Level 6

Headings are created using hash symbols (`#`) at the very start of a line.

Or, alternatively, by using `=` (level 1) or `-` (level 2) right under a line.


Emphasis
---

To make text bold, use two asterisks (`**`) or two underscores (`__`) on either side of the text. Example:

**Bold text**  
__Bold text__


To make text italic, use one asterisk (`*`) or one underscore (`_`) on either side of the text. Example:

*Italic text*  
_Italic text_

To make strikethrough text, use two tildes (`~~`) on either side of the text. Example:

~~Strikethrough text~~

You can also combine those formats. Example:

~~_**Strikethrough bold and italic text**_~~

Lists
---

Unordered lists can be created using asterisks (`*`), hyphens (`-`), or plus signs (`+`). Example:

* Item 1
- Item 2
  + Sub-item 2.1
  - Sub-item 2.2
* Item 3

Ordered lists are created using numbers followed by a period (`1.`, `2.`, etc.). Example:

1. First item
2. Second item
   1. Sub-item 2.1
   2. Sub-item 2.2
3. Third item


Blockquotes
---

Blockquotes are created using the `>` symbol followed by a space before the text. Example:

> This is a blockquote.  
> It can span multiple lines.

Blockquotes can be nested by adding additional `>` symbols. Example:

> First level of blockquote
>> Second level of blockquote
>>> Third level of blockquote

Each additional level is indented by one more `>` symbol.


Code
---

`Inline code` is created by wrapping the text in single backticks.

You can create a code block by indenting the code with four spaces or one tab. Example:

   #include <iostream>

    int main() {
        std::cout << "Hello World!";
        return 0;
    }

Or by wrapping it by 3 backticks. This method will also let you specify the code syntax.

```C++
#include <iostream>

int main() {
    std::cout << "Hello World!";
    return 0;
}
```

Horizontal Rules
---

Horizontal rules are created using three or more hyphens (`---`), asterisks (`***`), or underscores (`___`). Example:

---

***

___


Links
---

Links are created by wrapping the link text in square brackets (`[ ]`), followed by the URL in parentheses (`( )`).
You can also include optional titles in quotes after the URL. Example:

[KleverNotes repository](https://invent.kde.org/office/klevernotes)

Images
---

Images are similar to links but are preceded by an exclamation mark (`!`). The alt text is placed in square brackets (`[ ]`), followed by the image URL in parentheses (`( )`). Example:

![KleverNotes](./logo.png "KleverNotes logo !")

Using notation like "/home/user/myfile.png" or "~/myfile.png" will also work if you want to access images stored on your system but not alongside your note 😁


Tables
---

Tables are created using pipes (`|`) and hyphens (`-`). You need to separate columns with pipes and define the header row with hyphens. Example:

| Header 1 | Header 2 | Header 3 |
|----------|----------|----------|
| Row 1, Col 1 | Row 1, Col 2 | Row 1, Col 3 |
| Row 2, Col 1 | Row 2, Col 2 | Row 2, Col 3 |

Alignment can be adjusted by including colons (`:`) in the separator row:

| Header 1 | Header 2 | Header 3 |
|:---------|:---------:|---------:|
| Left aligned | Center aligned | Right aligned |
| Left aligned | Center aligned | Right aligned |


HTML
---

You can also include raw HTML in Markdown for additional control over formatting. Example:

<p>This is a paragraph in HTML.</p>
<a href="https://invent.kde.org/office/klevernotes">KleverNotes repository</a>
<p>Line breaks can also be used that way !<p><br><br>

---

Extended syntax
---

KleverNotes includes some extended syntax that follows the syntax style of emphasis and lets you:

Have ==highlighted part== of text.
Or show text as --subscript-- or ^superscript^.

Plugins
---

> A feature that goes beyond the normal scope of Markdown.
> It is considered a 'plugin' and can be enabled/disabled at any time in the settings.
> A plugin is disabled by default.

Those are opt-in and accessible in the settings. 
For more information, see the [plugin section](https://invent.kde.org/office/klevernotes#plugins) inside the KleverNotes readme.
