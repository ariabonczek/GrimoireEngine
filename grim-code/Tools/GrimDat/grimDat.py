import sys
import os
import json
import jinja2
import argparse

#import grimP4
import grimVS

# -*- coding: utf-8 -*-

CODE_PATH = os.environ["CODE_PATH"]

class Attribute:

    def __init__(self):
        self.name = ""
        self.type = ""
        self.default= None

class Enumeration:

    def __init__(self):
        self.name= ""
        self.valueList= []
        self.valueDict= dict()

class Type:

    def __init__(self):
        self.name= ""
        self.forwardDeclarations= []
        self.headers= []

        self.allAttributes= []
        self.enums = []

        self.namespace= ""
        self.typeSpecifier= ""
        self.serializeable= False
        self.anyDefaults= False

class GrimDat:
    def __init__(self):
        self.types= []

    def getAttributes(self, jsonType, datType):
        for jsonAttribute in jsonType["attributes"]:
            datAttribute = Attribute()

            # Required
            datAttribute.name = jsonAttribute["name"]
            datAttribute.type = jsonAttribute["type"]

            if "default" in jsonAttribute:
                datAttribute.default = jsonAttribute["default"]
                datType.anyDefaults = True
            
            datType.allAttributes.append(datAttribute);

    def getTypes(self, jsonData):
        for jsonType in jsonData["types"]:
            datType = Type()

            # Required
            datType.name = jsonType["name"]
            datType.typeSpecifier = jsonType["typeSpecifier"]

            if "namespace" in jsonType:
                datType.namespace = jsonType["namespace"]
            if "serializeable" in jsonType:
                datType.serializeable= jsonType["serializeable"]

            if "headers" in jsonType:
                for jsonHeader in jsonType["headers"]:
                    datType.headers.append(jsonHeader)

            if "enums" in jsonType:
                for jsonEnum in jsonType["enums"]:
                    datEnum = Enumeration()
                    datEnum.name = jsonEnum["name"]
                    it = 0
                    for datEnumVal in jsonEnum["values"]:
                        datEnum.valueList.append(datEnumVal)
                        datEnum.valueDict[str(datEnumVal)] = ++it
                    datType.enums.append(datEnum)

            self.getAttributes(jsonType, datType)

            self.types.append(datType)

    def loadFromFile(self, filepath):
        with open(filepath, 'r') as f:
            data = json.load(f)
            self.getTypes(data)
        

def main(argv):

    parser = argparse.ArgumentParser(description='grimDat argument parser')
    parser.add_argument("-i", dest="inputFilename", required=True, help="path to .grimdat file")
    parser.add_argument("-o", dest="outputDirectory", required=True, help="directory at which to output results")
    parser.add_argument("-j", dest="jinjaFilename", required=True, help="name of jinja file to use")
    parser.add_argument("-p", dest="propsFilename", required=True, help="name of .props file to generate")
    args = parser.parse_args();

    #grimP4.init()

    env = jinja2.Environment(loader=jinja2.PackageLoader('grimDat', 'jinja'), trim_blocks=True, lstrip_blocks=True)

    jinjaTemplates= dict()
    allTemplateFiles = os.listdir(os.path.join(CODE_PATH,'Tools', 'GrimDat', 'jinja'))
    for templateFile in allTemplateFiles:
        jinjaTemplates[templateFile] = env.get_template(templateFile)

    grimDat = GrimDat()
    grimDat.loadFromFile(args.inputFilename)

    # File base names, NOT full paths
    allFilenames = []

    for _type in grimDat.types:
        t = jinjaTemplates[args.jinjaFilename]
        output = t.render(grimtype=_type)

        if 'header' in args.jinjaFilename:
            generatedName = str('generated_' + _type.name + '.h')
        elif 'source' in args.jinjaFilename:
            generatedName = str('generated_' + _type.name + '.cpp')

        allFilenames.append(generatedName)
        filename = os.path.join(CODE_PATH, args.outputDirectory, generatedName)

        dirname = os.path.dirname(filename)
        if not os.path.exists(dirname):
            os.makedirs(dirname)

        #checkout = grimP4.checkout(filename)

        with open(filename, 'w') as f:
            f.write(output)

        #add = grimP4.add(filename)

        #if not (checkout or add):
        #    print "WARNING: Failed to checkout or add " + filename + " to Perforce"
        #else:
        #    print "Successfully generated " + filename

    propsPath = os.path.join(CODE_PATH, 'Generated', 'generated_' + args.propsFilename)

    #grimP4.checkout(propsPath)
    grimVS.generateFileProps(allFilenames, propsPath)
    #grimP4.add(propsPath)

    #grimP4.revertUnchanged()

    return

if __name__ == "__main__":
    main(sys.argv[1:])