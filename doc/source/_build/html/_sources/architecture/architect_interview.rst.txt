########################
Architect Interview
########################

.. contents::
   :local:

Top 10 questions
==========================

1. 多小的服务才是微服务
------------------------------
微服务是比较小的服务，到底可以有多小，我们团队的前CTO - Jonathan Rosenberg就曾经提出一个“两周原则”，一个服务如果可以从无到有在两周内上线，那么我们就称它是一个微服务，这个要求有点高，需要三个前提：

 *	一套完整的微服务框架。
 *	一套成熟的构建与部署流水线。
 *	一个稳定的 PaaS 云平台


2. 如何划分微服务的边界？
----------------------------

一个服务本身应该相对独立，具有一定的自治性，
实现一个相对完整的功能或者提供独特的数据，
一个服务对于其他的服务尽量避免强依赖。


“设计系统的架构受制于产生这些设计的组织的沟通结构”，如果把一个产品交由几个团队来做，那么应该把功能相对完整和独立的一个或一组服务交由一个团队来开发，团队之间定义好清晰的服务接口，然后各自进行开发和部署，只要服务的接口不改变，完全可以做到独立的实现、部署和升级


3. CAP 三性中应该如何取舍?
-----------------------------------

这个要看情况，我们的应用中一般选择 AP 和最终一致性, 参见 `distribution`_

AP 这样的系统关心可用性和分区容忍性。因此，这样的系统不能达成一致性，需要给出数据冲突，给出数据冲突就需要维护数据版本。

例如 `dynamo`_, `cassandra`_


4. 谈谈共识算法和一致性协议
---------------------------------------

参见  `consensus`_

业界公认和在使用的协议主要有

* Paxos
* ZAB
* Raft

大致来说就是讲究规则:

1）采用民主集中制，如果没有老大，大家就先选一个老大，老大和小弟随时都有可能挂掉
2) 少数服从多数
3）先到先得
4）最终决定之前先做一轮沟通


5. 微服务有什么优缺点，如何应对？
--------------------------------------

* 微服务的主要优点如下：

  - 微服务架构可以由一个相当小的团队开发，给开发者一定的自由来自主开发和部署。
  - 更好的故障隔离：如果一个微服务出现故障，另一个仍可以做到继续工作。
  - 不同服务的代码可以用不同的语言编写，没有长期的技术栈要求和限制，易于集成和自动部署。

* 微服务的局限也很明显：

  - 不断增长的微服务数量会导致信息屏障，可以没人能搞得清所有微服务是干什么的。
  - 必须应对分布式系统固有的各种常见故障，处理各种网络延迟、抖动、丢包、分流、重试、超时等一个也不能少，需要做很多措施（如容错、分流、限流）来保证高可用。
  - 问题的发现与诊断更加困难，需要多个微服务及对应团队共同协作解决。

* 应对措施

  - 化繁为简，把复杂性封装在简单的 API 下面
  - 快速迭代，小步快跑，持续交付
  - 自动化：集成，部署，测试和运维
  - 容错设计： 分流，限流和断流
  - 服务治理：服务注册与发现，伸缩与 Fallback, 版本管理和发布管理
  - 监控与度量： 见微知著，快速反应

6. 软件工程有用吗？
------------------------------

软件工程能力体现在

1. 提高服务的 SLA

* 高可用的系统
* 自动化的运维

2. 能力和资源的重用或复用

* 软件模块的重用
* 软件运行环境和资源的重用

3. 过程的自动化

* 软件生产流水线
* 软件运维自动化


7. 分布式锁怎么实现
---------------------------------

可以使用 Redis, ZooKeepr 及 etcd 这些中间件，后两者依赖共识算法


Redis

.. code-block:: java

    //set resource_name random_value NX PX 30000
    //e.g.
    //set meeting:$meetingNumber telephonyPools NX PX


  	private String setAndGetOldValueIfExisted(String meetingKey,  RedisCacheItem redisCacheItem,  String value){
		 String cacheKey = String.format(redisCacheItem.getFormat(), meetingKey);
     	    // jedis.set(key, value, "NX"|"XX", "EX"|"PX")
    	    // NX -- Only set the key if it does not already exist.
    	    // XX -- Only set the key if it already exist.
    	    // EX seconds -- Set the specified expire time, in seconds.
    	    // PX milliseconds -- Set the specified expire time, in milliseconds.
	     String returnValue = this.set(cacheKey, value, "NX", "EX", redisCacheItem.getTtl());
	     if (JEDIS_RETURN_OK.equals(returnValue)) {  //'OK' is success, null is fail
		    LOGGER.info("the key " + cacheKey +  " set success" );
	        return value;
	     }

         MetricThreadLocal.setCurrentFeatureMetric(MetricConstants.Values.Extends.CONCURRENT, true);

	     String oldValue = this.get(cacheKey);
	     LOGGER.info("the key " + cacheKey +  " had been set value before, value is: " + oldValue);

		 return oldValue;
    }

we also consider the lock failure case

.. code-block:: sql

    CREATE TABLE wbxinprogressteleconference (
    accesscode text,
    poolname text,
    tasip text,
    configitem map<text, text>,
    siteid bigint,
    tasuri text,
    lastmodifiedtime timestamp,
    PRIMARY KEY(accesscode, poolname, tasip));


* 考虑

  1. 独占性
  2. 可释放
  3. 高可用
  4. 可以不用阻塞
  5. 可重入


8. Cassandra 如何做到高可用性和最终一致性
---------------------------------------------

在Cassandra中，客户端可以是运行CQL命令的用户，也可以是使用不同语言Cassandra驱动连接到Cassandra的系统。
Cassandra的无中心架构的主要优点之一是客户端可以连接到任意Cassandra节点。Cassandra驱动实现了负载均衡功能，可以跨节点分发客户端请求。

客户端连接到的节点称为协调器。

协调器负责与其他节点交互，收集结果，并将查询结果返回给客户端，如下图所示。为了确保即使节点发生故障或无法访问，
Cassandra也可以跨多个节点存储冗余数据，具体取决于创建Keyspace时指定的复制因子

参见：

* 微服务实战之 Cassandra 之一
* 微服务实战之 Cassandra 之二
* 微服务实战之 Cassandra 之三

一致性级别 QUORUM


9. 提高吞吐量和性能的方法有哪些
---------------------------------------------

* 多进程
* 多线程
* 协程

以 Global IVR 向 Webex IVR Service 的演化为例

* stateful --> stateless
* single thread --> multiple thread
* call flow control xml --> directed graph from json



10. 如何做服务发现和服务分流
----------------------------------------------

* filter
* comparator
* selector



Other general questions
==================================================================

* Can you tell me a little bit about yourself?
* Why are you leaving your job?
* Why do you want to work here?
* What projects of ours did you like most?
* Why do you want to be an architect?
* How you describe your work style as an architect?
* What are some of your strengths that could help in this role as an architect?
* What are some of your weaknesses that could help in your role as an architect?
* Where do you see yourself in five years?
* Who do you admire for their architectural ability and why?
* Are you interviewing anywhere else?

Questions about experience and background
==================================================================
* What do you like most about architecture?
* What is your least favorite thing about architecture?
* What qualities make you a great architect?
* Describe your design style as an architect.
* What skill has served you best as an architect?
* Describe a time a problem arose and how you dealt with it.
* Describe your worst day as an architect and what you learned from it.
* What factors led to your biggest success as an architect?
* What is your least favorite project in your portfolio and why?
* How would you describe your role in your last project?
* What types of projects could you see yourself working on?

In-depth questions
==================================================================
* How do you handle multiple tasks at the same time?
* What did you do to ensure one of your previous projects was a success?
* How do you balance function with the aesthetic appeal of a building?
* What do you hope to learn from your supervisor?
* Give me an example of a time you thought outside of the box on a project. How did it prove useful?
* Provide an example of when you demonstrated excellent communication skills.
* Describe a time you dealt with a difficult person and how you handled the situation.
* Describe a time you oversaw the building process and how you effectively communicated any concerns to the team.
* How do you organize, plan and prioritize your work as an architect?
* In your opinion, what is the key to ensuring a building is compliant with all laws and regulations applicable?
* Why should we choose you for this position?


.. _consensus: ./consensus.html
.. _paxos: ./paxos.html
.. _zab: ./zab.html
.. _raft: ./raft.html
.. _distribution: distribution.html
.. _dynamo: https://cassandra.apache.org/doc/latest/architecture/dynamo.html
.. _cassandra: ../middleware/cassandra.html