############
Raft
############

Overview
===============

Raft is a consensus algorithm that is designed to be easy to understand.

It's equivalent to Paxos in fault-tolerance and performance.

The difference is that it's decomposed into relatively independent subproblems, and it cleanly addresses all major pieces needed for practical systems.

We hope Raft will make consensus available to a wider audience, and that this wider audience will be able to develop a variety of higher quality consensus-based systems than are available today

参见 https://raft.github.io/

Paper: https://raft.github.io/raft.pdf


* 将复杂问题分解成若干个子问题

    1. 领袖选举

    2. 日志复制

    3. 安全性

    4. 成员关系变化


* 减少状态空间


隐喻
======================

黑社会的运作机制

* 三类角色

  - Leader 领导 -> 老大
  - Candidate 候选人 -> 资深大佬
  - Follower 跟随者 -> 小弟

领导是有任期的 (Term) 的

选老大
----------------------------

运作规则

1. 老大要定期给小弟们发布心跳消息 (heartbeat ), 如果小弟们好长时间没有收到老大的心跳消息，则有理由认为老大已经挂了，需要选新老大

2. 小弟们发现大佬挂了，

 2.1 将自己本地维护的当前任期号递增： current_term_id += 1

 2.2 将自己的状态切换到候选人 (Candidate), 并为自己投票

 2.3 向其他小弟发送请求(RequestVote RPC) 包括 (current_term_id),要求他们投票给自己


这时候，有三种可能的结果

1. 得到多数选票，成功当选 Leader
2. 其他节点得到多数，自己切换为 Follower
3. 一段时间后发觉没人赢得多数票，重新发起一次选举

在一个任期内，一个小弟（ Raft 节点）只有一张选票，只能为一个候选人投票，按照先到先到的原则，投给最早来拉票的人
一个小弟在等待他人的选票时，如果收到一个自称领导人的心跳消息，如果比当前自己的任期值大，则承认这个新老大合法，否则拒绝，返回最新的 term_id
如果小弟们在一个超时范围内没有人赢得选举，在这个超时范围内随机开始新一轮的拉票



RequestVote Reqeust
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* term
* canndiateId
* lastLogIndex
* lastLogTerm

RequestVode Response
~~~~~~~~~~~~~~~~~~~~~~~~~~~
* term
* voteGranted



日志复制
----------------------------
老大当选了，要向小弟同步状态

1. 内线告诉老大最近当局要进行一轮扫黑除恶，大家最近小心点别犯事

2. 老大把这条消息加入其日志文件中，然后并行向小弟们发给 AppendEntries RPC ，要求小弟们复制这个日志条目

3. 老大会不断重试 AppendEntries RPC 指令给各个小弟，确保所有的小弟都同步了这条消息

