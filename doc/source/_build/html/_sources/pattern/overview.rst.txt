###########
模式概论
###########

.. contents:: 大纲
   :depth: 3

什么是模式
===============

模式是一种可复用的解决方案，用于解决软件开发中的常见问题，应用于软件设计领域的模式称为设计模式。

最早的模式概念是由一位建筑师提出来的

The elements of this language are entities called patterns. Each pattern describes a problem that occurs over and over again in our environment, and then describes the core of the solution to that problem, in such a way that you can use this solution a million times over, without ever doing it the same way twice.

— Christopher Alexander, A Pattern Language

Documenting a pattern requires explaining why a particular situation causes problems, and how the components of the pattern relate to each other to give the solution.


关键点：

* 抽象常见问题特征
* 复用解决方案
* 背景 - 问题 - 方案

 - Context: a situation giving rise to a problem
 - Problem: the recurring problem arising in that Context
 - Solution: a proven resolution of the problem

模式分类
==================

* Pattern
    * Context
        * Design situation giving rise to a design problem
    * Problem
        * Set of forces repeatedly arising in the context
    * Solution
        * Configuration to balance the forces
        * Structure with components and relatioinship
        * Run-time behaviour

* GRASP patterns
    * Information Expert
    * Creator
    * Controller
    * Low Coupling
    * High Cohesion
    * Polymorphism
    * Pure Fabrication
    * Indirection
    * Protected Variations

* GOF patterns
    * Creational 创建型模式
        * Simple Factory--Class
        * Factory Method--Class
        * Abstract Factory
        * Builder
        * Prototype
        * Singleton
    * Structural 结构型模式
        * Adapter--Class
        * Adapter
        * Bridge
        * Composite
        * Decorator
        * Facade
        * Flyweight
        * Proxy
    * Behavioural 行为型模式
        * Intepreter--Class
        * Template method--Class
        * Chain of Responsibility
        * Command
        * Iterator
        * Mediator
        * Memento
        * Observer
        * State
        * Strategy
        * Visitor

* Micro Service Pattern
    * Decomposition patterns
        * by business capability
        * by subdomain
    * Database per service
    * API gateway
    * Service discovery
        * Client side discovery
        * Service side discovery
    * Message
        * REST
        * RPC
        * Event
        * Request-response
        * Pub-Sub
    * Deployment
        * Single Service per Host
        * Multiple Service per Host
    * Cross-cutting concerns pattern
        * Microservice chassis
        * Externalized configuration
    * Circuit Breaker
    * Access Token
    * Observability Patterns
        * Log Aggregation
        * Application Metrics
        * Audit loggin
        * Distributed tracing
        * Exception tracking
        * Health check API
        * Log deployments and changes
    * UI Pattern
        * Service-side page fragment composition
        * Client-side UI composition

* J2EE patterns
    * Presentation tier
        * FrontController
        * Interception Filter C
        * 
        * 
        * 
        * 
        * 
        * 
    * Business tier
        * 
        * 
        * 
        * 
    * Integration tier
        * 
        * 

* [[ConcurrencyPatterns.mm Concurrency patterns]]
    * Active Object
    * Balking
    * Binding properties
    * Compute kernel
    * Double-checked locking
    * Event-based Asynchronous
    * Guarded Suspension
    * Join
    * Lock
    * MDP - Messaging Design Pattern
    * Monitor Object
    * Reactor
    * Read-write lock
    * Scheduler
    * Thread pool
    * Thread-specific storage
