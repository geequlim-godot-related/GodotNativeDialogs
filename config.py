#!/usr/bin/env python

def can_build(platform):
	return platform in ["windows", "x11", "osx"]

def configure(env):
	pass

def get_doc_classes():
    return [
        "GDNativeDialog",
        "NativeColorDialog",
        "NativeFileDialog",
        "NativeMessageDialog",
    ]

def get_doc_path():
    return "doc_classes"