# coding:utf-8
# /usr/bin/python

# creator = wangkai
# creation time = 2018/3/8 21:04


from fabric.api import *

env.hosts = "pi@192.168.100.5"


@runs_once
@task
def local_update():
    with lcd("/Users/wangkai/PycharmProjects/smartFishTank"):
        local("git add python selfAccessories")
        local("git commit -m 'update 4'")
        local("git pull origin master")
        local("git push -u origin  master")


@task
def remote_update():
    with cd("/home/pi/Documents/smartFishTank"):
        # run("ls -l")
        run("git checkout -- *")
        run("git checkout master")
        run("git pull origin master")


@task
def go():
    local_update()
    remote_update()