#############
ZAB
#############

.. contents::
   :local:


Overview
==================

ZooKeeper Atomic Broadcast

所有的事务请求必须由一个全局唯一的服务器来协调处理，这样的服务器称为 Leader,  余下的服务器称为 Follower

Leader 服务器负责将一个客户端请求转换成一个事务 Proposal(提议）， 并将该 Proposal 分发给集群中所有的 Follower

之后 Leader 需要等待所有 Follower 服务器的反馈， 一旦超过半数的 Follower 服务器进行了正确的反馈后

那么 Leader 就会再次向所有的 Followers 分发  Commit 消息， 要求其将前一个 Proposal 进行提交