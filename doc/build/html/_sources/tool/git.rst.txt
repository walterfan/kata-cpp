##################
git
##################

.. include:: ../links.ref
.. include:: ../tags.ref
.. include:: ../abbrs.ref

.. contents::
    :local:

overview
==============

work flow
------------------------------

Prepare
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  git clone https://yafan@stash-eng-chn-sjc1.cisco.com/stash/scm/cctg/webex-telephony-tahoe.git
 *  git remote add cctgfork https://yafan@stash-eng-chn-sjc1.cisco.com/stash/scm/cctgfork/webex-telephony-tahoe.git
 *  git remote add cctg https://yafan@stash-eng-chn-sjc1.cisco.com/stash/scm/cctg/webex-telephony-tahoe.git

Work For Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  git fetch -n cctg master
 *  git checkout -b workspace cctg/master

Coding and git commit
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  git push origin workspace:master

Code Review
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  pull request

Push code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  git fetch -n cctg master
 *  git merge cctg/master
 *  git push cctgfork workspace:gate/yafan

create local and remote branch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  从cctg仓库的master分支创建本支master, 创建一个自己的分支并push 到远程仓库

.. code-block::

  git checkout -b master cctg/master
  git checkout -b feature/customize_ivr
  git push origin feature/customize_ivr:feature/customize_ivr
  

Tips
--------------------------


1. 代码会存在于三个区域：
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1) 工作区 working directory
2) 暂存区 staging area
3) 仓库区 repository

5. 代码从拉到推
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
git fetch origin <branch>
git merge <branch>

以上两条命令等于 git pull origin <branch>

git fetch origin <branch>
git rebase <branch>

以上两条命令等于 git pull origin <branch> --rebase



git add xxx
git commit -m "xxx"
git push origin <branch>

3. 替换本地改动
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  撤销工作区修改

git checkout -- <filepath>

 *  撤销暂存区修改

git reset HEAD <filepath>

 *  用远程仓库中的代码覆盖本地代码

git fetch remote <branchname>
git checkout remote/branch -- a/file b/file
git reset --hard remote/branch
git reset --hard HEAD
git reset <commit_hash_id> <filepath>



4. 创新切换删除分支
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1) 创新分支 git checkout -b <branchname>

2) 切换分支 git checkout <branchname>

3) 删除分支 git branch -d <branchname>
   
*  删除远程分支: git push origin --delete <branchname>
   or git push origin :<branchname>

*  清空本地分支: git remote prune origin

4) 查看分支
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
git branch -a

5) 重命名分支
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
git branch -m <old_branch_name> <new_branch_name>
git push origin <new_branch_name>

note: origin is a remote repo
git remote add <repo_name> <git_url>

6) 比较分支
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
git diff <branch1>..<branch2> --stat


5. 查看历史
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

*  显示最近5次的提交的概要历史记录

.. code-block::

  git log --graph --stat --oneline -5

*  显示最近3次的提交的详细历史记录

.. code-block::

  git log -p -3

6. 比较区别
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1) 比较工作区和暂存区 git diff
2) 比较暂存区和仓库区 git diff --cached
3) 比较工作区和仓库区 git diff HEAD
4) 比较两个分支 git diff branch_1..branch_2

7. 管理标签
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*  显示标签

git tag

*  添加标签

git tag -a <tagname> -m <comments>

*  推送标签

git push --tags

*  获取远程标签

git fetch origin tag <tagname>

*  删除标签

git tag -d <tagname>
git push origin :refs/tags/<tagname>
git push origin --delete tag <tagname>

*  检出标签对应的代码

git checkout tags/<tag_name>

8. 子模块
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*  添加子模块
  
git submodule addhttps://chromium.googlesource.com/v8/v8.git

*  初始化子模块

git submodule init

*  修改子模块

git submodule update

1. 回退
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*  git log -10
*  git reset xxx
*  git push origin <branchname> --force

10. 冲突解决
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
git stash
git stash pop


Compare with SVN
-----------------------------------
 *  GIT is distributed, SVN is not:
 *  GIT stores content as metadata, SVN stores just files:
 *  GIT branches are not the same as SVN branches:
 *  GIT does not have a global revision no. like SVN do:
 *  GIT’s content integrity is better than SVN’s:
 * * refer to [http://boxysystems.com/index.php/5-fundamental-differences-between-git-svn/]

Book and tips
===================================

*  every version is snapshot, not diff

Three states
--------------------------------------

*  1) modified
*  2) staged <-- git add
*  3) committed <-- git commit

*  unmodified
*  untracked

ares
--------------------------------------
* working directory
* staging area
* git directory(repository)


commands
--------------------------------------
*  git remote -v
*  git checkout -b <branchname>
*  git branch
*  git reset # cancel add
*  git rebase/merge #
*  git reflog # change list
*  git diff --cached

git branch -r
--------------------------------------
*  origin/HEAD -> origin/master
*  origin/master
*  HEAD 最新的代码
*  origin is remote server


branch management
--------------------------------------
*  git fetch
*  git merge
*  git rebase
*  git gc

create branch
--------------------------------------
*  git branch dev
*  git checkout dev

*  http://blog.jobbole.com/23398/

Tutorial
==================================
* Git simple tutorial
  
  http://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000

* setup a git server on macosx
  http://www.tomdalling.com/blog/software-processes/how-to-set-up-a-secure-git-server-at-home-osx/

Spark build pipeline
=================================

.. code-block::

    fork the cloud-apps to another repo, such as https://sqbu-github.cisco.com/yafan/cloud-apps
    then clone the repoe and add remote repo like below
    git remote add gauntlet https://gauntlet.wbx2.com/api/git/cloud-apps
    git remote add origin https://sqbu-github.cisco.com/WebExSquared/cloud-apps.git
    git remote add yafan https://sqbu-github.cisco.com/yafan/cloud-apps.git

    Do some codes change, then commit , and push the fork repo ( suppose the local branch name is issue4337 )
    git checkout -b issue4337
    git pull origin master
    git add <changed_files>
    git commit -m "change comments"
    git push yafan issue4337
    then create a pull request between WebExSquared:master from yafan:issue4337 for reviewing, send the pull request url to your reviewer
    if the pull request is approved, then push the codes to gauntlet
    git push gauntlet issue4337: master

    Note:  gauntlet uses a different password, please use the tool here to generate:
    https://sqbu-jenkins-01.cisco.com:8443/job/utilities/job/internal-utilities-password-changer/


    then check the front door build pipeline: https://gauntlet.wbx2.com/queue.html?queue=cloud-apps
    if the codes change is in waitng quue firlst ,If it goes to vestibule list then, that's ok.
    Otherwise, you need to check the failed reason. If the error is not caused by your codes change, you can click the retry button to retry the build again.




Tools
================
* gitosis: https://github.com/res0nat0r/gitosis

* gitweb

  - install and setup: http://help.github.com/win-set-up-git/

.. code-block::
  
    $ git config --global user.name "Walter Fan"
    $ git config --global user.email "walter.fan@gmail.com"

    Global setup:
    Set up git
      git config --global user.name "Walter Fan"
      git config --global user.email walter.fan@gmail.com

    Next steps:
      mkdir WFJUtils
      cd WFJUtils
      git init
      touch README
      git add README
      git commit -m 'first commit'
      git remote add origin git@github.com:wind007/WFJUtils.git
      git push -u origin master

    Existing Git Repo?
      cd existing_git_repo
      git remote add origin git@github.com:wind007/WFJUtils.git
      git push -u origin master

Install On CentOS
-----------------------
*  new version

.. code-block::

  yum install libcurl-devel -y --nogpgcheck
  yum install perl-devel -y --nogpgcheck
  yum install gettext-devel -y --nogpgcheck
  make
  make install



*  Old version

.. code-block::

  $ yum install curl-devel expat-devel gettext-devel openssl-devel zlib-devel -y --nogpgcheck
  $ wget http://www.codemonkey.org.uk/projects/git-snapshots/git/git-latest.tar.gz

  $ tar xzvf git-latest.tar.gz
  $ cd git-{date}
  $ autoconf
  $ ./configure --with-curl=/usr/local
  $ make
  $ make install

*  ssh-keygen -t rsa -C "walter.fan@gmail.com"

FAQ
========================================================

how-to-set-up-a-secure-git-server-at-home-osx
-------------------------------------------------------
http://www.tomdalling.com/blog/software-processes/how-to-set-up-a-secure-git-server-at-home-osx/

Q 使用命令 ssh -T git@github.com 的时候，系统总是提示 permission denied
--------------------------------------------------------------------------------------------------------------
A. 修改/etc/ssh/sshd_config

修改/etc/ssh/sshd_config文件.将其中的PermitRootLogin no修改为yes
或者用 ssh -vvv git@github.com 查看一下，是不是公钥没上传正确

How to disable gnome-ssh-askpass?
-------------------------------------------------------
*  unset SSH_ASKPASS

How to use submodule
-------------------------------------------------------
*  git submodule add https://github.com/libevent/libevent.git third_party/libevent
*  git submodule add https://chromium.googlesource.com/v8/v8.git
*  git submodule init
*  git submodule update

Q. unable to access git SSL certificate problem: Invalid certificate chain
------------------------------------------------------------------------------------------

A. git config --global http.sslVerify false


Reference
==========================
*  http://rogerdudler.github.com/git-guide/index.zh.html
*  rebase: https://git-scm.com/book/zh/ch3-6.html
*  http://git-scm.com/book/zh/v1