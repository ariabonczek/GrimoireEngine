import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom
import os

cached_tree = ""
cached_file = ""

ns = { 'ms': 'http://schemas.microsoft.com/developer/msbuild/2003' }

def generateFileProps(fileList, propsFilename):
    root = ET.Element('Project')
    root.attrib['xmlns'] ="http://schemas.microsoft.com/developer/msbuild/2003"

    files = ET.Element('ItemGroup')

    for filename in fileList:
        if '.h' in filename:
            element = ET.Element('ClInclude')
        elif '.cpp' in filename:
            element = ET.Element('ClCompile')
        else:
            element = ET.Element('ERROR')
            
        element.set('Include', str(filename))

        files.append(element)

    root.append(files)

    roughString = ET.tostring(root, 'utf-8')
    prettyString = minidom.parseString(roughString).toprettyxml(indent="    ")

    with open(propsFilename, 'w') as f:
        f.write(prettyString)