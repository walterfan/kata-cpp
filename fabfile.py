from fabric import task
from datetime import date
from sys import platform
import os, subprocess

BASE_PATH = os.path.dirname(__file__)
GUIDE_FOLDER = "doc"
GUIDE_PATH = BASE_PATH + "/" + GUIDE_FOLDER 
default_hosts = ["localhost"]

@task(hosts=default_hosts)
def usage(c):
    print("usage: fab make_doc|publish_doc")

@task(hosts=default_hosts)
def make_doc(c):
    #with lcd(GUIDE_PATH):
    with c.prefix('cd %s' % GUIDE_PATH):
        build_cmd = 'make clean html'
        c.local(build_cmd)

@task(hosts=default_hosts)
def publish_doc(c):
    c.local("touch %s/build/html/.nojekyll" % GUIDE_PATH)
    c.local("git add %s" % GUIDE_PATH)
    c.local('git commit -m "update doc"')
    c.local("git subtree push --prefix %s/build/html origin gh-pages" % GUIDE_FOLDER)

@task(hosts=default_hosts)
def build_webrtc(c, src_dir=None, isClean=False):
    if not src_dir:
        src_dir = os.getenv('WEBRTC_SRC')
    with c.prefix('cd %s' % src_dir):
        c.local("python build/util/lastchange.py build/util/LASTCHANGE")
        if isClean:
            c.local("gn clean out/Default")
        c.local("gn gen out/Default")
        c.local("ninja -C out/Default")