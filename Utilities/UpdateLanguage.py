#!/usr/bin/env python
################################################################################
# UpdateLanguage.py - adds new strings to a language XML file
# in the 3.X format. A 'template' file is used to provide the new strings.
#
################################################################################

from xml.etree import ElementTree

class CommentedTreeBuilder(ElementTree.TreeBuilder):
    def comment(self, data):
        self.start(ElementTree.Comment, {})
        self.data(data)
        self.end(ElementTree.Comment)

def update(input, template, output):
    parser = ElementTree.XMLParser(target=CommentedTreeBuilder())
    update = ElementTree.parse(input)
    ur = update.getroot()
    template = ElementTree.parse(template, parser)
    tr = template.getroot()

    strings = set()
    for strtag in ur.findall('string'):
        strings.add(strtag.find('original').text)

    updates = {}
    for strtag in ur.findall('string'):
        str = strtag.find('original').text
        if str in strings:
            trans = strtag.find('translation').text
            updates[str] = trans

    for strtag in tr.findall('string'):
        orig = strtag.find('original')
        trans = strtag.find('translation')
        if orig == None or trans == None:
            continue

        if orig.text in updates:
            trans = strtag.find('translation')
            trans.text = updates[orig.text]
        else:
            print('Adding \"' + orig.text + '\"')
            trans.text = orig.text
    template.write(output, encoding='UTF-8')

def print_usage(appname='UpdateLanguage'):
    print('Usage: ' + appname + ' input.xml template.xml output.xml')
    print('File descriptions:')
    print('input.xml       The v3 language file to update')
    print('template.xml    Template file to use for new strings')
    print('output.xml      Output file name')

if __name__ == "__main__":
    import os
    import sys
    args = sys.argv
    if len(args) < 3:
        print_usage(args[0])
    else:
        update(args[1], args[2], args[3])
