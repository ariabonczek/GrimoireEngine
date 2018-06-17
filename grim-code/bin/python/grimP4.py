from P4 import P4, P4Exception
import os

p4 = P4()

def revertUnchanged():
    p4.run('revert', '-a')

def checkoutOrAdd(filename, silent=True):
    if not checkout(filename, silent):
        if not add(filename, silent):
            return False
    return True

def add(filename, silent=True):
    if os.path.exists(filename):
        try:
            p4.run('add', filename)
        except:
            if not silent:
                print "WARNING: Tried to add " + filename + " but an error occured:"
                for e in p4.errors:
                    print e
            return False
        return True
    else:
        if not silent:
            print "ERROR: Tried to checkout " + filename + " but the file does not exist"
        return False

def checkout(filename, silent=True):
    if os.path.exists(filename):
        try:
            p4.run('edit', filename)
        except P4Exception:
            if not silent:
                print "WARNING: Tried to checkout " + filename + " but an error occured:"
                for e in p4.errors:
                    print e
            return False
        return True
    else:
        if not silent:
            print "ERROR: Tried to checkout " + filename + " but the file does not exist"
        return False

def init():
    try:
        p4.connect()
    except P4Exception:
        print "Failed to initialize p4"